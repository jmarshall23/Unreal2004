/*=============================================================================
	DemoRecDrv.cpp: Unreal demo recording network driver.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/

#include "ReplicatorEngine.h"
#include "DemoPassthrough.h"

#define PACKETSIZE 512

/*-----------------------------------------------------------------------------
	UDemoRecConnection.
-----------------------------------------------------------------------------*/

void UDemoPassthroughConnection::StaticConstructor()
{
	guard(UDemoRecConnection::StaticConstructor);
	unguard;
}

UDemoPassthroughConnection::UDemoPassthroughConnection( UNetDriver* InDriver, const FURL& InURL )
: UDemoRecConnection( InDriver, InURL )
{
	guard(UDemoPassthroughConnection::UDemoPassthroughConnection);
	unguard;
}
/*
UDemoPassthroughDriver* UDemoPassthroughConnection::GetDriver()
{
	return (UDemoPassthroughDriver *)Driver;
}
*/
void UDemoPassthroughConnection::HandleClientPlayer( APlayerController* PC )
{
	guard(UDemoPassthroughConnection::HandleClientPlayer);
	Super::HandleClientPlayer(PC);
	PC->bDemoOwner = 1;
	unguard;
}

IMPLEMENT_CLASS(UDemoPassthroughConnection);

/*-----------------------------------------------------------------------------
	UDemoRecDriver.
-----------------------------------------------------------------------------*/

void UDemoPassthroughDriver::StaticConstructor()
{
	guard(UDemoRecDriver::StaticConstructor);
//	new(GetClass(),TEXT("DemoSpectatorClass"), RF_Public)UStrProperty(CPP_PROPERTY(DemoSpectatorClass), TEXT("Client"), CPF_Config);
	unguard;
}

UBOOL UDemoPassthroughDriver::InitConnect( FNetworkNotify* InNotify, FURL& ConnectURL, FString& Error )
{
	guard(UDemoPassthroughDriver::InitConnect);
	if( !Super::InitConnect( InNotify, ConnectURL, Error ) )
		return 0;
	if( !InitBase( 1, InNotify, ConnectURL, Error ) )
		return 0;

	mu = 0;

	// Playback, local machine is a client, and the demo stream acts "as if" it's the server.
	//ServerConnection = new UDemoPassthroughConnection( this, ConnectURL );
	ServerConnection = new UDemoRecConnection( this, ConnectURL );
	ServerConnection->CurrentNetSpeed = 1000000;
	ServerConnection->State        = USOCK_Pending;
	FileAr                         = GFileManager->CreateFileReader( *DemoFilename );
	if( !FileAr )
	{
		Error = FString::Printf( TEXT("Couldn't open demo file %s for reading"), *DemoFilename );//!!localize!!
		return 0;
	}
	LoopURL = ConnectURL;
	NoFrameCap          = ConnectURL.HasOption(TEXT("timedemo"));
	Loop				= ConnectURL.HasOption(TEXT("loop"));
	return 1;
	unguard;
}


void UDemoPassthroughDriver::TickDispatch( FLOAT DeltaTime )
{
	guard(UDemoPassthroughDriver::TickDispatch);
	UNetDriver::TickDispatch( DeltaTime );

	mu++;
	if (mu >= 1) {
		mu = 0;
	}
	else {
		return;
	}

	UpdateDemoTime (&DeltaTime, 1.f);

	if( ServerConnection && (ServerConnection->State==USOCK_Pending || ServerConnection->State==USOCK_Open) )
	{	
		BYTE Data[PACKETSIZE + 8];
		// Read data from the demo file
		DWORD PacketBytes;
		INT PlayedThisTick = 0;

		for( ; ; )
		{
			// At end of file?
			if( FileAr->AtEnd() || FileAr->IsError() )
			{
			AtEnd:
				ServerConnection->State = USOCK_Closed;
				DemoEnded = 1;

				DOUBLE Seconds = appSeconds()-PlaybackStartTime;
				if( NoFrameCap )
					ServerConnection->Actor->eventClientMessage( *FString::Printf(TEXT("Demo %s ended: %d frames in %lf seconds (%.3f fps)"), *DemoFilename, FrameNum-InitialFrameStart, Seconds, (FLOAT)(FrameNum-InitialFrameStart)/Seconds ), NAME_None );//!!localize!!
				else
					ServerConnection->Actor->eventClientMessage( *FString::Printf(TEXT("Demo %s ended: %d frames in %lf seconds"), *DemoFilename, FrameNum-InitialFrameStart, Seconds ), NAME_None );//!!localize!!

				if( Loop )
					GetLevel()->Exec( *(FString(TEXT("DEMOPLAY "))+(*LoopURL.String())), *GLog );
				return;
			}
	
			INT ServerFrameNum;
			FLOAT ServerDeltaTime;

			*FileAr << ServerDeltaTime;
			*FileAr << ServerFrameNum;

			//printf ("serverframe: %i ourframe: %i\n", ServerFrameNum, FrameNum);

			//hoppa tillbaka om det går för fort gör denna rad
			if( ServerFrameNum > FrameNum )
			{
				FileAr->Seek(FileAr->Tell() - sizeof(ServerFrameNum) - sizeof(ServerDeltaTime));
				break;
			}
			*FileAr << PacketBytes;

			if( PacketBytes )
			{
				// Read data from file.
				FileAr->Serialize( Data, PacketBytes );
				if( FileAr->IsError() )
				{
					debugf( NAME_DevNet, TEXT("Failed to read demo file packet") );
					goto AtEnd;
				}

				// Update stats.
				PlayedThisTick++;

				// Process incoming packet.
				ServerConnection->ReceivedRawPacket( Data, PacketBytes );
				//UtvEngine->SendRawDataToClients (Data, PacketBytes);
			}
			else {
				//printf ("jao?\n");
			}

			// Only play one packet per tick on demo playback, until we're 
			// fully connected.  This is like the handshake for net play.
			if(ServerConnection->State == USOCK_Pending)
			{
				FrameNum = ServerFrameNum;
				break;
			}
		}

	}
	unguard;
}

IMPLEMENT_CLASS(UDemoPassthroughDriver);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


/*=============================================================================
	GCNClient.cpp: UGCNClient code.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Brandon Reinhart
=============================================================================*/

#include "GCNDrv.h"

//!! excuse this please
#include "../../Engine/Inc/UnCon.h"

#define MEMORYCARD

/*-----------------------------------------------------------------------------
	Class implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UGCNClient);

/*-----------------------------------------------------------------------------
	UGCNClient implementation.
-----------------------------------------------------------------------------*/

//
// UGCNClient constructor.
//
UGCNClient::UGCNClient()
{
	guard(UGCNClient::UGCNClient);

	unguard;
}

//
// Static init.
//
void UGCNClient::StaticConstructor()
{
	guard(UGCNClient::StaticConstructor);

	unguard;
}

void UGCNClient::Serialize(FArchive& Ar)
{
	guard(GCNClient::Serialize);

	UClient::Serialize(Ar);

#ifdef MEMORYCARD
	Ar << InputManager << McManager;
#else
	Ar << InputManager;
#endif
	unguard;
}

//
// Initialize the platform-specific viewport manager subsystem.
// Must be called after the Unreal object manager has been initialized.
// Must be called before any viewports are created.
//
void UGCNClient::Init( UEngine* InEngine )
{
	guard(UGCNClient::UGCNClient);

	// Init base.
	UClient::Init( InEngine );

	FullscreenViewport = -1;

	// Default res option.
	if( ParseParam(appCmdLine(),TEXT("defaultres")) )
	{
		WindowedViewportX  = FullscreenViewportX  = 800;
		WindowedViewportY  = FullscreenViewportY  = 600;
	}

	// Initialize the input manager.
	InputManager = ConstructObject<UGCNInputManager>(UGCNInputManager::StaticClass(),this);
	InputManager->Init();

#ifdef MEMORYCARD
	McManager = ConstructObject<UGCNMcManager>(UGCNMcManager::StaticClass(),this);
	McManager->Init(this);
#endif

	// Success.
	debugf( NAME_Init, TEXT("GCNClient initialized.") );
	unguard;
}

//
// Shut down the platform-specific viewport manager subsystem.
//
void UGCNClient::Destroy()
{
	guard(UGCNClient::Destroy);

	debugf( NAME_Exit, TEXT("GCN client shut down.") );
	Super::Destroy();
	delete McManager;
	McManager = NULL;
	unguard;
}

//
// Failsafe routine to shut down viewport manager subsystem
// after an error has occured. Not guarded.
//
void UGCNClient::ShutdownAfterError()
{
	debugf( NAME_Exit, TEXT("Executing UGCNClient::ShutdownAfterError") );

	// Kill the audio subsystem.
	if( Engine && Engine->Audio )
	{
		Engine->Audio->ConditionalShutdownAfterError();
	}

	// Release all viewports.
	for( INT i=Viewports.Num()-1; i>=0; i-- )
	{
		UGCNViewport* Viewport = (UGCNViewport*)Viewports( i );
		Viewport->ConditionalShutdownAfterError();
	}

	Super::ShutdownAfterError();
}

void UGCNClient::NotifyDestroy( void* Src )
{
	guard(UGCNClient::NotifyDestroy);

	unguard;
}

void UGCNClient::OpenNewViewport( FURL& URL )
{
	guard(UGCNClient::OpenNewViewport);

	// Create viewport.
	UViewport* Viewport = NewViewport( NAME_None, Viewports.Num() );
	Viewport->RenDev = Viewports(0)->RenDev;
	UClass* ConsoleClass = StaticLoadClass( UConsole::StaticClass(), NULL, TEXT("ini:Engine.Engine.Console"), NULL, LOAD_NoFail, NULL );
	Viewport->Console = ConstructObject<UConsole>( ConsoleClass );
//SLUW	Viewport->Console->ViewportIndex = Viewports.Num()-1;
	Viewport->Console->_Init( Viewport );

	// Spawn play actor.
	FString Error;
	if( !Cast<UGameEngine>(Engine)->GLevel->SpawnPlayActor( Viewport, ROLE_SimulatedProxy, URL, Error ) )
		appErrorf( TEXT("%s"), *Error );

	CastChecked<UGCNViewport>(Viewport)->ViewportIndex = Viewports.Num()-1;

	Viewport->OpenWindow( 0, 0, INDEX_NONE, INDEX_NONE, INDEX_NONE, INDEX_NONE );
	Viewport->Input->Init( Viewport );

	unguard;
}

void UGCNClient::UpdateSplit()
{
	if(FullscreenViewport == -1)
	{
		switch(Viewports.Num())
		{
		case 1:
			Viewports(0)->SizeX = 640;
			Viewports(0)->SizeY = 448;
//SLUW			Viewports(0)->OffX = 0;
//SLUW			Viewports(0)->OffY = 0;
			break;
		case 2:
			Viewports(0)->SizeX = 640;
			Viewports(0)->SizeY = 224;
//SLUW			Viewports(0)->OffX = 0;
//SLUW			Viewports(0)->OffY = 0;
			Viewports(1)->SizeX = 640;
			Viewports(1)->SizeY = 224;
//SLUW			Viewports(1)->OffX = 0;
//SLUW			Viewports(1)->OffY = 224;
			break;
		case 4:
			Viewports(0)->SizeX = 320;
			Viewports(0)->SizeY = 224;
//SLUW			Viewports(0)->OffX = 0;
//SLUW			Viewports(0)->OffY = 0;
			Viewports(1)->SizeX = 320;
			Viewports(1)->SizeY = 224;
//SLUW			Viewports(1)->OffX = 320;
//SLUW			Viewports(1)->OffY = 0;
			Viewports(2)->SizeX = 320;
			Viewports(2)->SizeY = 224;
//SLUW			Viewports(2)->OffX = 0;
//SLUW			Viewports(2)->OffY = 224;
			Viewports(3)->SizeX = 320;
			Viewports(3)->SizeY = 224;
//SLUW			Viewports(3)->OffX = 320;
//SLUW			Viewports(3)->OffY = 224;
			break;
		}
	}
	else
	{
		for(INT Index = 0;Index < Viewports.Num();Index++)
		{
			if(Index == FullscreenViewport)
			{
//SLUW				Viewports(Index)->OffX = 0;
//SLUW				Viewports(Index)->OffY = 0;
				Viewports(Index)->SizeX = 640;
				Viewports(Index)->SizeY = 448;
			}
			else
			{
//SLUW				Viewports(Index)->OffX = 0;
//SLUW				Viewports(Index)->OffY = 0;
				Viewports(Index)->SizeX = 0;
				Viewports(Index)->SizeY = 0;
			}
		}
	}
}

//
// Command line.
//
UBOOL UGCNClient::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UGCNClient::Exec);
	if( UClient::Exec( Cmd, Ar ) )
	{
		return 1;
	}
	if( ParseCommand( &Cmd, TEXT("ADDSPLIT") ) )
	{
		FURL	DefaultURL;
		DefaultURL.LoadURLConfig( TEXT("DefaultPlayer"), TEXT("User") );

		FURL	URL(&DefaultURL,Cmd,TRAVEL_Relative);
		OpenNewViewport( URL );

		UpdateSplit();

		return 1;
	}
	if(ParseCommand(&Cmd,TEXT("CLEARSPLIT")))
	{
		FullscreenViewport = -1;

		while(Viewports.Num() > 1)
		{ 
			UViewport*	V = Viewports(Viewports.Num() - 1);
			V->RenDev = NULL;
			delete V;
		};

		UpdateSplit();

		return 1;
	}
	if(ParseCommand(&Cmd,TEXT("FULLSCREENVIEWPORT")))
	{
		FullscreenViewport = appAtoi(Cmd);

		UpdateSplit();

		return 1;
	}
	if(ParseCommand(&Cmd,TEXT("NUMVIEWPORTS")))
	{
		Ar.Logf(TEXT("%u"),Viewports.Num());

		return 1;
	}

	if(InputManager && InputManager->Exec(Cmd,Ar))
		return 1;
	else
	{
#ifdef MEMORYCARD
		return McManager && McManager->Exec(Cmd,Ar);
#else
		return 0;
#endif
	}

	unguard;
}

//
// Perform timer-tick processing on all visible viewports.  This causes
// all realtime viewports, and all non-realtime viewports which have been
// updated, to be blitted.
//
void UGCNClient::Tick()
{
	guard(UGCNClient::Tick);

	INT	Index;

	// Tick the viewports.

	if(FullscreenViewport != -1)
	{
		CastChecked<UGCNViewport>(Viewports(FullscreenViewport))->Repaint(1);

		// Hack to tick the consoles for hidden viewports.

		for(Index = 0;Index < Viewports.Num();Index++)
		{
			if(Index != FullscreenViewport)
			{
				Viewports(Index)->CurrentTime = appSeconds();
				Viewports(Index)->Console->eventTick(Viewports(Index)->CurrentTime - Viewports(Index)->LastUpdateTime);
				Viewports(Index)->LastUpdateTime = appSeconds();
			}
		}

		if(FullscreenViewport != 0 && Engine->Audio )
		{
			FCoords	AudioCoords;

			clock(Cast<UGameEngine>(Engine)->GLevel->AudioTickCycles);
			Engine->Audio->Update(Viewports(0)->Actor->Region,AudioCoords);
			unclock(Cast<UGameEngine>(Engine)->GLevel->AudioTickCycles);
		}
	}
	else
  		for(Index = 0;Index < Viewports.Num();Index++)
			CastChecked<UGCNViewport>(Viewports(Index))->Repaint(Index == Viewports.Num() - 1);

#ifdef MEMORYCARD
	// Tick the memory card.
	McManager->Tick();
#endif
	unguard;
}

//
// Create a new viewport.
//
UViewport* UGCNClient::NewViewport( const FName Name, int ViewportIndex )
{
	guard(UGCNClient::NewViewport);
//SLUW	return new( this, Name )UGCNViewport(ViewportIndex);
return new( this, Name )UGCNViewport;
	unguard;
}

UViewport* UGCNClient::NewViewport( const FName Name )
{
	guard(UGCNClient::NewViewport);
	return new( this, Name )UGCNViewport();
	unguard;
}

//
// Configuration change.
//
void UGCNClient::PostEditChange()
{
	guard(UGCNClient::PostEditChange);

	Super::PostEditChange();

	unguard;
}

//
// Enable or disable all viewport windows that have ShowFlags set (or all if ShowFlags=0).
//
void UGCNClient::EnableViewportWindows( DWORD ShowFlags, int DoEnable )
{
	guard(UGCNClient::EnableViewportWindows);

	unguard;
}

//
// Show or hide all viewport windows that have ShowFlags set (or all if ShowFlags=0).
//
void UGCNClient::ShowViewportWindows( DWORD ShowFlags, int DoShow )
{
	guard(UGCNClient::ShowViewportWindows);

	// Not necessary, GCN has no windowing system.

	unguard;
}

//
// Make this viewport the current one.
// If Viewport=0, makes no viewport the current one.
//
void UGCNClient::MakeCurrent( UViewport* InViewport )
{
}

/*-----------------------------------------------------------------------------
	End.
-----------------------------------------------------------------------------*/


/*=============================================================================
	WinClient.cpp: UWindowsClient code.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "XboxDrv.h"
#include "../E3Code/xe3uc.h" // sjs - e3 specific stats

//
//	Preallocation information.
//	Passed to XInitDevices.
//

XDEVICE_PREALLOC_TYPE PreallocationInfo[] =
{
	{XDEVICE_TYPE_GAMEPAD, 4},
	{XDEVICE_TYPE_MEMORY_UNIT, 2},
	{XDEVICE_TYPE_DEBUG_KEYBOARD, 1},		//!!KEYBOARD HACK
	{XDEVICE_TYPE_VOICE_HEADPHONE, 4},
	{XDEVICE_TYPE_VOICE_MICROPHONE, 4}
};

/*-----------------------------------------------------------------------------
	Class implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UXboxClient);

/*-----------------------------------------------------------------------------
	UXboxClient implementation.
-----------------------------------------------------------------------------*/

//
// UXboxClient constructor.
//
UXboxClient::UXboxClient()
{
}

//
// Static init.
//
void UXboxClient::StaticConstructor()
{
	guard(UXboxClient::StaticConstructor);

	unguard;
}

//
// Initialize the platform-specific viewport manager subsystem.
// Must be called after the Unreal object manager has been initialized.
// Must be called before any viewports are created.
//
void UXboxClient::Init( UEngine* InEngine )
{
	guard(UXboxClient::Init);

	// Init base.
	UClient::Init( InEngine );

	// Note configuration.
	PostEditChange();

	// Xbox input initialization.
	XInitDevices(ARRAY_COUNT(PreallocationInfo),PreallocationInfo);	

    XINPUT_DEBUG_KEYQUEUE_PARAMETERS KeyQueueParams = 
	{
		XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN		|
		XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP		|
		XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT,
		50,
		500,
		70 
	};
    
	XInputDebugInitKeyboardQueue( &KeyQueueParams );	

	// Success.
	debugf( NAME_Init, TEXT("Client initialized") );
	unguard;
}
 
//
// Shut down the platform-specific viewport manager subsystem.
//
void UXboxClient::Destroy()
{
	guard(UXboxClient::Destroy);

	debugf( NAME_Exit, TEXT("Xbox client shut down") );
	Super::Destroy();

	unguard;
}

//
// Failsafe routine to shut down viewport manager subsystem
// after an error has occured. Not guarded.
//
void UXboxClient::ShutdownAfterError()
{
	debugf( NAME_Exit, TEXT("Executing UXboxClient::ShutdownAfterError") );

	if( Engine && Engine->Audio )
		Engine->Audio->ConditionalShutdownAfterError();

	for( INT i=Viewports.Num()-1; i>=0; i-- )
	{
		UXboxViewport* Viewport = (UXboxViewport*)Viewports( i );
		Viewport->ConditionalShutdownAfterError();
	}

	Super::ShutdownAfterError();
}

//
// Command line.
//
UBOOL UXboxClient::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UXboxClient::Exec);
	if( UClient::Exec( Cmd, Ar ) )
	{
		return 1;
	}
    // sjs --- E3!
    else if( ParseCommand(&Cmd,TEXT("E3-GETPLAYER")) ) // do this before a server join!
	{
        // test for title region
        //FArchive* LogAr = GFileManager->CreateFileWriter( TEXT("T:\\TITLEREGION.txt"), FILEWRITE_AllowRead|FILEWRITE_Unbuffered);
        //LogAr->Serialize( TEXT("Test!"), 6*sizeof(TCHAR) );
        //delete LogAr;
        ///////////////////////////
        
        if(appStrstr(appCmdLine(),TEXT("-E3RedName")))
            UtilCreateDummyUCFile("PillLover",1,1);
        if(appStrstr(appCmdLine(),TEXT("-E3BlueName")))
            UtilCreateDummyUCFile("GeorgeBob",2,2);

        if( Engine==NULL || Viewports.Num()==0 )
        {
            Ar.Logf( TEXT("E3-GETPLAYER Failed (Engine==NULL || Viewports.Num()==0)"));
            return 0;
        }

        ANSICHAR ansiname[64];
        UINT teamId;
        UINT maskId;
        if (XE3GetUCPlayer(ansiname, &teamId, &maskId))
        {
            teamId--; // 0-based, filespec is 1-based
            FString nameString = FString::Printf(TEXT("Name=%s"),appFromAnsi(ansiname));
            FString teamString = FString::Printf(TEXT("Team=%d"),teamId);
            FString voiceString = FString::Printf(TEXT("VoiceMask=%d"),maskId);

            UGameEngine* GameEngine = CastChecked<UGameEngine>( Engine );
	        GameEngine->LastURL.AddOption( *nameString );
            GameEngine->LastURL.AddOption( *teamString );
            GameEngine->LastURL.AddOption( *voiceString );

            // save it
		    GameEngine->LastURL.SaveURLConfig( TEXT("DefaultPlayer"), TEXT("Name"), TEXT("User") );
            GameEngine->LastURL.SaveURLConfig( TEXT("DefaultPlayer"), TEXT("Team"), TEXT("User") );
            GameEngine->LastURL.SaveURLConfig( TEXT("DefaultPlayer"), TEXT("VoiceMask"), TEXT("User") );
            Ar.Logf( TEXT("E3-GETPLAYER complete [%s?%s?%s]"), *nameString, *teamString, *voiceString );

            if( GameEngine->Audio )
                GameEngine->Audio->Exec(TEXT("E3-VOICEMASK"), Ar );
        }
        else
        {
            Ar.Logf( TEXT("E3-GETPLAYER Failed [missing file or date/time hasn't changed]"));
        }
		return 1;
	}
    else if( ParseCommand(&Cmd,TEXT("E3-WRITEPLAYER")) ) // do this at game end!
	{
        if( Viewports.Num()==0 )
        {
            Ar.Logf( TEXT("E3-WRITEPLAYER Failed (Viewports.Num()==0)"));
            return 0;
        }
        APlayerController* Player = Viewports(0)->Actor;
        if( Player==NULL || Player->PlayerReplicationInfo==NULL )
        {
            Ar.Logf( TEXT("E3-WRITEPLAYER Failed (Player==NULL||Player->PlayerReplicationInfo==NULL)"));
            return 0;
        }
        AGameReplicationInfo* gri = Player->GameReplicationInfo;
        if( gri==NULL )
        {
            Ar.Logf( TEXT("E3-WRITEPLAYER Failed (GameReplicationInfo==NULL||Player->PlayerReplicationInfo->Team==NULL)"));
            return 0;
        }

		// do team calcs here:
		UINT teamId = 0;
		UINT teamWin = teamId;
		if( Player->PlayerReplicationInfo->Team )
		{
			teamId = Player->PlayerReplicationInfo->Team->TeamIndex;
			if( gri->Teams[teamId]->Score <= gri->Teams[(teamId+1)%2]->Score )
			{
				teamWin = !teamId;
			}
		}
        
        UINT frags = Player->Stats ? Player->Stats->Kills : 0;
        int totalFrags = frags - (Player->Stats ? Player->Stats->Suicides : 0);
        frags = (UINT)(Clamp<INT>(totalFrags,0,999999999));
        UINT flagCaps = Clamp<INT>(Player->PlayerReplicationInfo->Score, 0, 99999999);
        const ANSICHAR* ansiname = appToAnsi(*Player->PlayerReplicationInfo->PlayerName);

        ANSICHAR fmtname[13];
        appMemset(fmtname, 0, sizeof(ANSICHAR)*13);
        int len = Min(12, appStrlen(*Player->PlayerReplicationInfo->PlayerName));
        appMemcpy(fmtname, ansiname, len);

        if( !Player->Stats )
        {
            debugf(TEXT("Player->Stats == NULL!"));
        }
        // szName, uiTeam, uiTeamWin, uiFrags, uiFlagCaptures, uiReturns, uiSpecials, uiAccuracy, uiFavoriteWeapon
        // calc returns???
        UINT returns = 0;
        // calc 'specials'
        UINT specials = Player->Stats ? Player->Stats->Specials : 0;
        // calc accuracy
        UINT accuracy = 0;
        if( Player->Stats && Player->Stats->Shots > 0 )
        {
            accuracy = (int)(((float)Player->Stats->Hits / (float)Player->Stats->Shots) * 100.0f);
        }
        // calc favWeapon
        UINT favWeapon = 0;
        if( Player->Stats )
        {
            int bestKills = 0;
            AWeaponStat* bestWeap = NULL;
            for( int i = 0; i < ARRAY_COUNT(Player->Stats->WeaponStats); i++ )
            {
                if( Player->Stats->WeaponStats[i] != NULL )
                {
                    if( Player->Stats->WeaponStats[i]->Kills > bestKills )
                    {
                        bestKills = Player->Stats->WeaponStats[i]->Kills;
                        bestWeap = Player->Stats->WeaponStats[i];
                    }
                }
            }
            if ( bestWeap )
            {
                favWeapon = ((AWeapon*)bestWeap->WeaponClass->GetDefaultActor())->InventoryGroup;
            }
        }
        if( XE3WriteUCPlayer((ANSICHAR*)fmtname, teamId+1, teamWin+1, frags, flagCaps, returns, specials, accuracy, favWeapon) ) // !! teamid+1 (file spec is 1-based)
        {
            Ar.Logf( TEXT("E3-WRITEPLAYER Success: %s %d win:%d frags:%d points:%d returns:%d specials:%d accuracy:%d favWeapon:%d"),
                *Player->PlayerReplicationInfo->PlayerName, teamId+1, teamWin+1, frags, flagCaps, returns, specials, accuracy, favWeapon );
        }
        else
        {
            Ar.Logf( TEXT("E3-WRITEPLAYER Failed [file unchanged or missing]"));
        }
		return 1;
	}
    // --- sjs
	return 0;
	unguard;
}

//
// Perform timer-tick processing on all visible viewports.  This causes
// all realtime viewports, and all non-realtime viewports which have been
// updated, to be blitted.
//
void UXboxClient::Tick()
{
	guard(UXboxClient::Tick);

	// Repaint all viewports.
  	for( INT i=0; i<Viewports.Num(); i++ )
		Viewports(i)->Repaint(1);
	unguard;
}

//
// Create a new viewport.
//
UViewport* UXboxClient::NewViewport( const FName Name )
{
	guard(UXboxClient::NewViewport);
	return new( this, Name )UXboxViewport();
	unguard;
}

//
// Configuration change.
//
void UXboxClient::PostEditChange()
{
	guard(UXboxClient::PostEditChange);

	Super::PostEditChange();

	unguard;
}

//
// Enable or disable all viewport windows that have ShowFlags set (or all if ShowFlags=0).
//
void UXboxClient::EnableViewportWindows( DWORD ShowFlags, int DoEnable )
{
}

//
// Show or hide all viewport windows that have ShowFlags set (or all if ShowFlags=0).
//
void UXboxClient::ShowViewportWindows( DWORD ShowFlags, int DoShow )
{
}

//
// Make this viewport the current one.
// If Viewport=0, makes no viewport the current one.
//
void UXboxClient::MakeCurrent( UViewport* InViewport )
{
}

// Returns a pointer to the viewport that was last current.
UViewport* UXboxClient::GetLastCurrent()
{
	return NULL;
}


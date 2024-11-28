/*=============================================================================
	PSX2Launch.cpp: Game launcher.
	Copyright 1999-2000 Epic Games, Inc. All Rights Reserved.

	PSX2 command line options:
		usecd      -- use the CD drive for file loading
		nohost     -- don't use the host for file loading
		rebootiop  -- reboot the IOP when starting up; requires usecd and nohost
=============================================================================*/
/*=============================================================================
	Launch.cpp: Game launcher.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/
//#include "GCNLaunchPrivate.h"
#include <dolphin.h>
#include <demo.h>

#include "Engine.h"

#if __STATIC_LINK
#include "UnCon.h"
#include "UnRender.h"
#include "UnNet.h"
#include "GCNDrv.h"
#include "UnFractal.h"
#include "Audio.h"
#endif

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/

// General.
extern "C" {HINSTANCE hInstance;}
extern "C" {TCHAR GPackage[64]=TEXT("GCNLaunch");}

// Log file.
//#include "FOutputDevice.h"
class FOutputDeviceGCN : public FOutputDevice
{
public:
	void Serialize( const TCHAR* V, EName Event )
	{
		OSReport( "%s\n", V );
	}
} Log;

// Memory allocator.
#include "FMallocGCN.h"
FMallocGCN Malloc;

// Error handler.
#include "FOutputDeviceAnsiError.h"
FOutputDeviceAnsiError Error;

// Feedback.
#include "FFeedbackContextAnsi.h"
FFeedbackContextAnsi Warn;

// File manager.
#ifdef EMU
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include <errno.h>
#include "FFileManagerAnsi.h"
FFileManagerAnsi FileManagerBase;
//#include "FFileManagerGCN.h"
//FFileManagerGCN FileManager;
#else
#include "FFileManagerGCN.h"
FFileManagerGCN FileManagerBase;
#endif

#include "FFileManagerArc.h"
FFileManagerArc FileManager (&FileManagerBase,  TEXT("psx2game.umd"),0);
FFileManagerArc FileManagerTex (&FileManager,   TEXT("gcntpl.umd"),0);

// Config.
#include "FConfigCacheIni.h"


UBOOL ReadTextureFile(const TCHAR* InTextureName, TArray<BYTE>& OutTextureData)
{
	TCHAR TextureName[64];
	appSprintf(TextureName, TEXT("../GCNTextures/%s.tpl"), InTextureName);
	FArchive* TPLReader = FileManagerTex.CreateFileReader(TextureName, LOAD_NoFail);
	if (TPLReader == NULL)
	{
		return false;
	}
		
	INT FileSize = TPLReader->TotalSize();
	
	// Read the .tpl right into the GCN mips :)
	OutTextureData.Empty(FileSize);
	TPLReader->Serialize(&OutTextureData(0), FileSize);
	delete TPLReader;
	
	return true;
}



static UEngine* InitEngine()
{
	guard(InitEngine);
	DOUBLE LoadTime = appSeconds();

	// Create the global engine object.
	UClass* EngineClass;
	if( !GIsEditor )
	{
		// Create game engine.
		EngineClass = UObject::StaticLoadClass( UGameEngine::StaticClass(), NULL, TEXT("ini:Engine.Engine.GameEngine"), NULL, LOAD_NoFail, NULL );
	}
	else
	{
		// Editor.
		EngineClass = UObject::StaticLoadClass( UEngine::StaticClass(), NULL, TEXT("ini:Engine.Engine.EditorEngine"), NULL, LOAD_NoFail, NULL );
	}
	UEngine* Engine = ConstructObject<UEngine>( EngineClass );
	Engine->Init();
	debugf( TEXT("Startup time: %f seconds"), appSeconds()-LoadTime );

	return Engine;
	unguard;
}

//
// Unreal's main message loop.  All windows in Unreal receive messages
// somewhere below this function on the stack.
//
static void MainLoop( UEngine* Engine )
{
	guard(MainLoop);
	check(Engine);

	// Loop while running.
	GIsRunning = 1;
	DOUBLE OldTime = appSeconds();
	DOUBLE SecondStartTime = OldTime;
	INT TickCount = 0;
	while( GIsRunning && !GIsRequestingExit )
	{
		// Update the world.
		guard(UpdateWorld);
		DOUBLE NewTime   = appSeconds();
		FLOAT  DeltaTime = NewTime - OldTime;

		Engine->Tick( DeltaTime );
		if( GWindowManager )
			GWindowManager->Tick( DeltaTime );
		OldTime = NewTime;
		TickCount++;
		if( OldTime > SecondStartTime + 1 )
		{
			Engine->CurrentTickRate = (FLOAT)TickCount / (OldTime - SecondStartTime);
			SecondStartTime = OldTime;
			TickCount = 0;
		}
		unguard;

		// Enforce optional maximum tick rate.
		guard(EnforceTickRate);
		FLOAT MaxTickRate = Engine->GetMaxTickRate();
		if( MaxTickRate>0.0 )
		{
			FLOAT Delta = (1.0/MaxTickRate) - (appSeconds()-OldTime);
			appSleep( Max(0.f,Delta) );
		}
		unguard;

	}
	GIsRunning = 0;

	// Exit main loop.

	unguard;
}

/*-----------------------------------------------------------------------------
	main()
-----------------------------------------------------------------------------*/
//
// Main entry point.
// This is an example of how to initialize and launch the engine.
//
#ifdef EMU
void main( void )
#else
int main( int argc, char* argv[] )
#endif
{
	// Remember instance.
	INT ErrorLevel = 0;
	GIsStarted     = 1;
//	hInstance      = hInInstance;
//	const TCHAR* CmdLine = GetCommandLine();
	appStrcpy( GPackage, appPackage() );

	char* P = (char*)appToAnsi(GPackage);

	TCHAR CmdLineBuf[1024] = "";
#ifdef EMU
//	appStrcpy(CmdLineBuf, GetCommandLine());
//	appStrcat(CmdLineBuf, TEXT(" INI=gcngame.ini USERINI=gcnuser.ini"));
	appStrcpy(CmdLineBuf, TEXT("GCNLaunch.exe coyote INI=gcngame.ini USERINI=gcnuser.ini"));
#else
	INT i;
	for( i=1; i<argc; i++ )
	{
Log.Logf("arg: %s", argv[i]);
		if( i>1 )
			appStrcpy( &CmdLineBuf[appStrlen(CmdLineBuf)], TEXT(" "));
		appStrcpy( &CmdLineBuf[appStrlen(CmdLineBuf)], appFromAnsi(argv[i]));
	}
	appStrcat(CmdLineBuf, TEXT(" INI=gcngame.ini USERINI=gcnuser.ini"));
#endif


	const TCHAR* CmdLine = CmdLineBuf;

	


#if __STATIC_LINK
	for( INT k=0; k<ARRAY_COUNT(GNativeLookupFuncs); k++ )
		GNativeLookupFuncs[k] = NULL;

	// Core natives.
	GNativeLookupFuncs[0]  = &FindCoreUObjectNative;
	GNativeLookupFuncs[1]  = &FindCoreUCommandletNative;
	
	// Engine natives.
	GNativeLookupFuncs[2]  = &FindEngineAActorNative;
	GNativeLookupFuncs[3]  = &FindEngineALevelInfoNative;
	GNativeLookupFuncs[4]  = &FindEngineAGameInfoNative;
	GNativeLookupFuncs[5]  = &FindEngineAZoneInfoNative;
	GNativeLookupFuncs[6]  = &FindEngineANavigationPointNative;
	GNativeLookupFuncs[7]  = &FindEngineAWarpZoneInfoNative;
	GNativeLookupFuncs[8] = &FindEngineADecalNative;
	GNativeLookupFuncs[9] = &FindEngineAControllerNative;
	GNativeLookupFuncs[10] = &FindEngineAPlayerControllerNative;
	GNativeLookupFuncs[11] = &FindEngineUCanvasNative;
	GNativeLookupFuncs[12] = &FindEngineUConsoleNative;
	GNativeLookupFuncs[13] = &FindEngineAProjectorNative;
	GNativeLookupFuncs[14] = &FindEngineAVolumeNative;
	GNativeLookupFuncs[15] = &FindEngineUParticleEmitterNative;
#endif

	// Begin guarded code.
#ifndef _DEBUG
	try
	{
#endif
		DEMOInit(NULL);
#ifdef EMU
		DVDSetRoot("W:\\System");
#endif
//		OSInit();
//		DVDInit();


		// Init core.
		GIsClient = GIsGuarded = 1;
		appInit( GPackage, CmdLine, &Malloc, &Log, &Error, &Warn, &FileManager, FConfigCacheIni::Factory, 1 );
		
		
#if __STATIC_LINK
		AUTO_INITIALIZE_REGISTRANTS_ENGINE;
		AUTO_INITIALIZE_REGISTRANTS_GCNDRV;
		AUTO_INITIALIZE_REGISTRANTS_GCNAUDIO;
		AUTO_INITIALIZE_REGISTRANTS_FIRE;
#endif
	void InitGCNRenderPackage(); InitGCNRenderPackage();

		FileManagerTex.Init(1);
		
		
		TArray<BYTE> Poop;
		ReadTextureFile("sg_coglightube2", Poop);
		
		// Init mode.
		GIsServer     = 1;
		GIsClient     = !ParseParam(appCmdLine(),TEXT("SERVER"));
		GIsEditor     = 0;
		GIsScriptable = 1;
		GLazyLoad     = !GIsClient || ParseParam(appCmdLine(),TEXT("LAZY"));

/*		// Figure out whether to show log or splash screen.
		UBOOL ShowLog = ParseParam(CmdLine,TEXT("LOG"));
		FString Filename = FString(TEXT("..\\Help")) * GPackage + TEXT("Logo.bmp");
		if( GFileManager->FileSize(*Filename)<0 )
			Filename = TEXT("..\\Help\\Logo.bmp");
		appStrcpy( GPackage, appPackage() );
		if( !ShowLog && !ParseParam(CmdLine,TEXT("server")) && !appStrfind(CmdLine,TEXT("TestRenDev")) )
			InitSplash( *Filename );

		// Init windowing.
		InitWindowing();

		// Create log window, but only show it if ShowLog.
		GLogWindow = new WLog( Log.Filename, Log.LogAr, TEXT("GameLog") );
		GLogWindow->OpenWindow( ShowLog, 0 );
		GLogWindow->Log( NAME_Title, LocalizeGeneral(TEXT("Start"),GPackage) );
		if( GIsClient )
			SetPropX( *GLogWindow, TEXT("IsBrowser"), (HANDLE)1 );
*/
		// Init engine.
		UEngine* Engine = InitEngine();
		if( Engine )
		{

			// Start main engine loop, including the Windows message pump.
			if( !GIsRequestingExit )
				MainLoop( Engine );
		}

		// Clean shutdown.
		GFileManager->Delete(TEXT("Running.ini"),0,0);
		appPreExit();
		GIsGuarded = 0;
#ifndef _DEBUG
	}
	catch( ... )
	{
		// Crashed.
		ErrorLevel = 1;
		Error.HandleError();
	}
#endif

	// Final shut down.
	appExit();
	GIsStarted = 0;
#ifdef EMU
	return;
#else
	return 0;
#endif
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


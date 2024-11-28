/*=============================================================================
	Launch.cpp: Game launcher.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/

#define DEBUG_KEYBOARD		//!!KEYBOARD HACK
#include <xtl.h>
#include <malloc.h>
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <direct.h>

#include "Core.h"
#include "Engine.h"

#if __STATIC_LINK
#include "UnEngineNative.h"
#include "UnRender.h"
#include "UnNullRenderDevice.h"
#include "UnNet.h"
#include "XboxDrv.h"
#include "D3DDrv.h"
#include "UnFractal.h"
#include "XBoxAudio.h"

// IpDrv static stuff.
#include "UnIpDrv.h"
#include "UnTcpNetDriver.h"
#include "UnIpDrvCommandlets.h"
#include "UnIpDrvNative.h"

#include "XInterface.h" // gam
#include "XGameNative.h" // sjs
#include "UWeb.h"
#include "UWebNative.h" // sjs
#endif

#define NUM_SPLASH_IMAGES 5 // sjs
#define NO_STARTMAPS 1 // sjs

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/

// Linear loading fun.
#define USE_LINEAR_LOADING		0
#define START_LOADING			0
#define	TOKEN_LINEAR_LOAD		0
#define TOKEN_LINEAR_SAVE		1
#define USE_Z_DRIVE				0
#define BENCHMARKING			0
#define USE_STATS				(1 | BENCHMARKING)
TCHAR	GStartMaps[16][256];
INT		GStartMapsIndex			= 0;
INT		GStartMapsMaxIndex		= 0;
UBOOL	GLinearLoad				= 0;
UBOOL	GLinearSave				= 0;
DWORD	GLinearBytesSerialized	= 0;
TCHAR	GLinearURL[4096];
extern	TCHAR GCmdLine[];
UBOOL   Dedicated = 0; // sjs


// General.
extern "C" {HINSTANCE hInstance;}
extern "C" {TCHAR GPackage[64]=TEXT("UC");} // gam
extern void RegisterNamesXInterface();

// Memory allocator.
#ifdef _DEBUG
	#include "FMallocWindows.h"		// sjs was debug
	FMallocWindows Malloc;			// sjs was debug
#else
	#include "FMallocWindows.h"
	FMallocWindows Malloc;
#endif

// Log file.
#include "FOutputDeviceDebug.h"
FOutputDeviceDebug Log;

// Error handler.
#include "FOutputDeviceAnsiError.h"
FOutputDeviceAnsiError Error;

// Feedback.
#include "FFeedbackContextAnsi.h"
FFeedbackContextAnsi Warn;

// File managers.
#include "FFileManagerArc.h"
#include "FFileManagerXbox.h"
#include "FFileManagerLinear.h"

// Config.
#include "FConfigCacheIni.h"


// Setup start maps array.
static void InitStartMaps()
{
	/********************** MAPS GO HERE *********************/
	appStrcpy( GStartMaps[0], TEXT("CIN-COGCity") );
	appStrcpy( GStartMaps[1], TEXT("TDM-Torrent") );	
}

// sjs --- e3
struct DedicatedServerGfx
{
    DedicatedServerGfx()
    {
        Direct3D8 = NULL;
        Direct3DDevice8 = NULL;
        time = 0;
    }
    void Release()
    {
        // Release resources.
	    Direct3DDevice8->Release();
	    Direct3D8->Release();
    }
    IDirect3D8*	Direct3D8;
    IDirect3DDevice8* Direct3DDevice8;
    float time;
};

const float servergfxInterval = 1.0f;
DedicatedServerGfx servergfx;

static void PollDedicated() // sjs - e3
{
    if( servergfx.Direct3D8 == NULL )
    {
	    // Setup the presentation parameters.
	    D3DPRESENT_PARAMETERS	PresentParms;
	    appMemzero(&PresentParms,sizeof(PresentParms));
	    PresentParms.Flags							= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER| D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
	    PresentParms.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	    PresentParms.BackBufferWidth				= 640;
	    PresentParms.BackBufferHeight				= 480;
	    PresentParms.BackBufferCount				= 1;
	    PresentParms.EnableAutoDepthStencil			= TRUE;
	    PresentParms.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
	    PresentParms.MultiSampleType				= 0;
	    PresentParms.FullScreen_PresentationInterval= D3DPRESENT_INTERVAL_ONE;
	    PresentParms.AutoDepthStencilFormat			= D3DFMT_D24S8;
        PresentParms.BackBufferFormat				= D3DFMT_X8R8G8B8;

	    // Init D3D.
	    servergfx.Direct3D8 = Direct3DCreate8(D3D_SDK_VERSION);

	    // Create D3D device.
	    servergfx.Direct3D8->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &PresentParms, &servergfx.Direct3DDevice8 );

        // Retrieve the last back buffer.
	    IDirect3DSurface8*	BackBuffer;
	    servergfx.Direct3DDevice8->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&BackBuffer);

	    // Lock backbuffer.
	    D3DLOCKED_RECT LockedRect;
	    BackBuffer->LockRect( &LockedRect, NULL, D3DLOCK_TILED );

	    // Read texture into back buffer.
	    FArchive* FileReader = GFileManager->CreateFileReader( TEXT("Z:\\splash.raw") );
	    FileReader->Serialize( LockedRect.pBits, 640*480*4 );
	    FileReader->Close();

	    // Unlock the back buffer.
	    BackBuffer->UnlockRect();

	    // Release the back buffer.
	    BackBuffer->Release();

	    // Display and persist.
	    servergfx.Direct3DDevice8->Present( NULL, NULL, NULL, NULL );

        servergfx.time = appSeconds();

        return;
    }

    if( appSeconds() - servergfx.time < servergfxInterval )
        return;

    servergfx.time = appSeconds();

	// Retrieve the last back buffer.
	IDirect3DSurface8*	BackBuffer;
    IDirect3DSurface8*	FrontBuffer;
	servergfx.Direct3DDevice8->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&BackBuffer);
    servergfx.Direct3DDevice8->GetBackBuffer(-1,D3DBACKBUFFER_TYPE_MONO,&FrontBuffer);
    servergfx.Direct3DDevice8->CopyRects( FrontBuffer, NULL, 0, BackBuffer, NULL );
    // release front
	FrontBuffer->Release();

    // Lock backbuffer.
	D3DLOCKED_RECT LockedRect;
    RECT rect;
    int sizeX = 80;
    int sizeY = 12;
    rect.left = (640/2)-(sizeX/2);
    rect.top = 480-62;
    rect.right = rect.left + sizeX;
    rect.bottom = rect.top + sizeY;
    DWORD color = 0xff000000 | rand();

	BackBuffer->LockRect( &LockedRect, &rect, D3DLOCK_TILED );
    DWORD* rgba = (DWORD*)LockedRect.pBits;

    for( int x=0; x<sizeX; x++ )
    {
        for( int y=0; y<sizeY; y++ )
        {
            rgba[y*(LockedRect.Pitch/4)+x] = color;
        }
    }

	// Unlock the back buffer and release
	BackBuffer->UnlockRect();
	BackBuffer->Release();

	// Display and persist.
	servergfx.Direct3DDevice8->Present( NULL, NULL, NULL, NULL );
}
// --- sjs e3

// Unreal's main message loop.  All windows in Unreal receive messages
// somewhere below this function on the stack.
static void MainLoop( UEngine* Engine )
{
	guard(MainLoop);
	check(Engine);

	// Loop while running.
	GIsRunning = 1;
	DOUBLE OldTime = appSeconds();
	DOUBLE SecondStartTime = OldTime;
	INT TickCount = 0;
#if BENCHMARKING
	INT		BMFrames			= 0;
	INT		BMDiscardedFrames	= 10;
	INT		BMMaxFrames			= 2510;
	FLOAT	BMSeconds			= 0.0f;
	// Seed random number generator with constant seed for benchmarking.
	appRandInit( 0 );
#endif

	while( GIsRunning && !GIsRequestingExit )
	{
#if USE_STATS
		// Clear stats (will also update old stats).
		GStats.Clear();
		DWORD FrameStartCycles = appCycles();
#endif
		// Update the world.
		guard(UpdateWorld);
		DOUBLE NewTime   = appSeconds();
		FLOAT  DeltaTime = NewTime - OldTime;
		
#if BENCHMARKING
		if( BMFrames == BMDiscardedFrames )
			BMSeconds	= 0.0f;
		else if( BMFrames > BMMaxFrames )
			appRequestExit(0);
		
		//Update.
		Engine->Tick( 1.0f / 30.0f );
		if( GWindowManager )
			GWindowManager->Tick( 1.0f / 30.0f );

		if( BMFrames < BMMaxFrames )
			BMSeconds += DeltaTime;
		BMFrames++;
#else
		//Update.
		Engine->Tick( DeltaTime );

        if(Dedicated) // sjs - e3
            PollDedicated();

		// sjs --- engine::tick may load a new map and cause the timing to be reset (this is a good thing)
		if( appSeconds() < NewTime )
            SecondStartTime = NewTime = appSeconds();
		// --- sjs
		if( GWindowManager )
			GWindowManager->Tick( DeltaTime );
#endif	
	
		OldTime = NewTime;
		TickCount++;
		if( OldTime > SecondStartTime + 1 )
		{
			Engine->CurrentTickRate = (FLOAT)TickCount / (OldTime - SecondStartTime);
			SecondStartTime = OldTime;
			TickCount = 0;
		}
		unguard;
#if 0
		// Enforce optional maximum tick rate.
		guard(EnforceTickRate);
		FLOAT MaxTickRate = Engine->GetMaxTickRate();
		if( MaxTickRate>0.0 )
		{
			FLOAT Delta = (1.0/MaxTickRate) - (appSeconds()-OldTime);
			appSleep( Max(0.f,Delta) );
		}
		unguard;
#endif

#if USE_STATS
		GStats.DWORDStats( GEngineStats.STATS_Frame_TotalCycles ) = appCycles() - FrameStartCycles;
		GStats.DWORDStats( GEngineStats.STATS_Game_ScriptCycles ) = GScriptCycles;
		GScriptCycles = 0;
#endif
	}

#if BENCHMARKING
	INT Frames = BMMaxFrames - BMDiscardedFrames; 
	debugf(TEXT("Average: %f fps"), Frames / BMSeconds );
#endif

	GIsRunning = 0;

	unguard;
}

// Display splash screen.
static void ShowSplash()
{
	// Init D3D.
	IDirect3D8*	Direct3D8;
	IDirect3DDevice8* Direct3DDevice8;

	// Setup the presentation parameters.
	D3DPRESENT_PARAMETERS	PresentParms;
	appMemzero(&PresentParms,sizeof(PresentParms));
	PresentParms.Flags							= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER| D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
	PresentParms.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	PresentParms.BackBufferWidth				= 640;
	PresentParms.BackBufferHeight				= 480;
	PresentParms.BackBufferCount				= 1;
	PresentParms.EnableAutoDepthStencil			= TRUE;
	PresentParms.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
	PresentParms.MultiSampleType				= 0;
	PresentParms.FullScreen_PresentationInterval= D3DPRESENT_INTERVAL_ONE;
	PresentParms.AutoDepthStencilFormat			= D3DFMT_D24S8;
    PresentParms.BackBufferFormat				= D3DFMT_X8R8G8B8;

	// Init D3D.
	Direct3D8 = Direct3DCreate8(D3D_SDK_VERSION);

	// Create D3D device.
	Direct3D8->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &PresentParms, &Direct3DDevice8 );

	// Retrieve the last back buffer.
	IDirect3DSurface8*	BackBuffer;
	Direct3DDevice8->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&BackBuffer);

	// Lock backbuffer.
	D3DLOCKED_RECT LockedRect;
	BackBuffer->LockRect( &LockedRect, NULL, D3DLOCK_TILED );

	// Read texture into back buffer.
	FArchive* FileReader = GFileManager->CreateFileReader( TEXT("Z:\\splash.raw") );
	FileReader->Serialize( LockedRect.pBits, 640*480*4 );
	FileReader->Close();

	// Unlock the back buffer.
	BackBuffer->UnlockRect();

	// Release the back buffer.
	BackBuffer->Release();

	// Display and persist.
	Direct3DDevice8->Present( NULL, NULL, NULL, NULL );
	Direct3DDevice8->PersistDisplay();

	// Release resources.
	Direct3DDevice8->Release();
	Direct3D8->Release();
}


/*-----------------------------------------------------------------------------
	WinMain.
-----------------------------------------------------------------------------*/

//
// Main entry point.
// This is an example of how to initialize and launch the engine.
//
INT main(INT ArgC,char* ArgV[])
{
	// Remember instance.
	INT ErrorLevel = 0;
	GIsStarted     = 1;

    srand( appSeconds()*1000.0f ); // sjs - hmmm?

	XSetFileCacheSize( 4 * 1024 * 1024 );
	MEMORYSTATUS MemStatus;

	// Init array.
	InitStartMaps();

	// Get command line.
	DWORD LaunchType;
	LAUNCH_DATA LaunchData;

	UBOOL OptionLL		= 0;
	UBOOL OptionSL		= 0;
	UBOOL OptionSLE		= 0;
	UBOOL InitialLoad	= 1;

	DWORD Result = XGetLaunchInfo( &LaunchType, &LaunchData );
	if( Result != ERROR_NOT_FOUND )
    {
#if NO_STARTMAPS // sjs
        switch( LaunchType )
		{
		case LDT_FROM_DASHBOARD:
			break;
		case LDT_FROM_DEBUGGER_CMDLINE:
            if ( LaunchData.Data[0] != 0 )
			    appStrcpy( GCmdLine, appFromAnsi( (char*)LaunchData.Data ));
			break;
		case LDT_TITLE:
            if ( LaunchData.Data[0] != 0 )
			    appStrcpy( GCmdLine, appFromAnsi( (char*)LaunchData.Data ));
			break;
		default:
			break;
		}
#else
		switch( LaunchType )
		{
		case LDT_FROM_DASHBOARD:
			{
				appStrcpy(GCmdLine,TEXT(" "));
				appStrcat(GCmdLine,GStartMaps[GStartMapsIndex]);
				OptionLL = 1;
			}
			break;
		case LDT_FROM_DEBUGGER_CMDLINE:
			{
                if ( LaunchData.Data[0] != 0 )
			        appStrcpy( GCmdLine, appFromAnsi( (char*)LaunchData.Data ));
				//while(1);
			}
			break;
		case LDT_TITLE:
			{
				INT* Dummy		= (INT*) &LaunchData.Data;
				GStartMapsIndex = *(Dummy++);
				OptionLL		= (*(Dummy++) == TOKEN_LINEAR_LOAD);
				OptionSL		= !OptionLL;
				InitialLoad		= 0;
				appStrcpy( GCmdLine, GStartMaps[GStartMapsIndex] );
			}
			break;
		default:
			break;
		}
#endif
	}
	else
	{
        #if !NO_STARTMAPS // sjs
		    OptionLL		= START_LOADING;
		    OptionSL		= !START_LOADING;
		    OptionSLE		= OptionSL;
		    appStrcat(GCmdLine,GStartMaps[GStartMapsIndex]);
        #endif
	}

#if USE_LINEAR_LOADING
	GLazyLoad		= 0;	// Needed by linear loading.
	GUglyHackFlags |= 32;	//!!vogel: temporary hack.
#else
	OptionLL		= 0;
	OptionSL		= 0;
	OptionSLE		= 0;
#endif

	// Static classes.
	static UClass*  EngineClass;
	static UEngine* EngineObject;

	//!!
	GMalloc = &Malloc;

	// Examine command line.
	TCHAR LinearSrc[4096];
	appStrcpy(LinearSrc,GCmdLine);
	for(INT i=0; i<ARRAY_COUNT(LinearSrc); i++ )
		LinearSrc[i]=appToLower(LinearSrc[i]);
	while(appStrstr(LinearSrc,TEXT("/")))
		{TCHAR Temp[4096]; appStrcpy(Temp,LinearSrc); appStrcpy(LinearSrc,appStrstr(Temp,TEXT("/"))+1);}
	if(appStrstr(LinearSrc,TEXT("?")))
		LinearSrc[appStrstr(LinearSrc,TEXT("?"))-LinearSrc]=0;
	if(appStrstr(LinearSrc,TEXT(" ")))
		LinearSrc[appStrstr(LinearSrc,TEXT(" "))-LinearSrc]=0;
	if(appStrstr(LinearSrc,TEXT(".ut2")))
		LinearSrc[appStrstr(LinearSrc,TEXT(".ut2"))-LinearSrc]=0;

	appStrcpy(LinearSrc,GStartMaps[GStartMapsIndex]);
	appStrcat(LinearSrc,TEXT(".lin"));

	GLinearSave = OptionSLE;
	GLinearLoad = OptionLL;
	appStrcpy(GLinearURL,TEXT("entry"));

	// File managers.
	FFileManagerXbox	FileManagerRaw;
#if USE_LINEAR_LOADING
//	FFileManagerArc		FileManagerDynamic(&FileManagerRaw,		TEXT("xboxship.umd"),0); //!!vogel: REMOVEME
	FFileManagerArc		FileManagerDynamic(&FileManagerRaw,		TEXT("xboxgame.umd"),0);
	FFileManagerLinear	FileManagerLinear (&FileManagerRaw,		TEXT("xboxgame.umd"));
	FFileManagerArc		FileManagerStatic (&FileManagerLinear,	TEXT("xboxgame.umd"),0);
#else
	FFileManagerArc		FileManagerDynamic(&FileManagerRaw,		TEXT("xboxgame.umd"),0);
	FFileManagerLinear	FileManagerLinear (&FileManagerRaw,		TEXT("dumdidum.dum"));
	FFileManagerArc		FileManagerStatic (&FileManagerRaw,		TEXT("xboxgame.umd"),0);
#endif
	
//!!vogel: REMOVEME: E3 HACK
#if 1
	Malloc.Init();
	if( FileManagerRaw.FileSize(TEXT("SERVER")) > 0 )
	{
		char MapName[128];
		appMemzero( MapName, sizeof(MapName) );
		FArchive* Ar = FileManagerRaw.CreateFileReader(TEXT("SERVER"), 0, GError);
		Ar->Serialize( MapName, sizeof(MapName) );
		Ar->Close();
		delete Ar;

		appStrcpy(GCmdLine, TEXT(" "));
		appStrcat(GCmdLine, ANSI_TO_TCHAR(MapName) );
		appStrcat(GCmdLine, TEXT(" -server"));
	}
	Malloc.Exit();
#endif

	// Backup and clear the actual command line.
	TCHAR ActualCommandLine[4096];
	appStrcpy(ActualCommandLine,GCmdLine);

	// Start entry linearization.
	appStrcpy(FileManagerLinear.Src,TEXT("common.lin"));

	// Init core.
	GIsClient = GIsGuarded = 1;

    appInit( GPackage, GCmdLine, &Malloc, &Log, &Error, &Warn, &FileManagerRaw, FConfigCacheIni::Factory, 1 );
	//appInit( GPackage, GCmdLine, &Malloc, &Log, &Error, &Warn, &FileManagerStatic, FConfigCacheIni::Factory, 1 );

    // Static linking.
#if __STATIC_LINK
    for( INT k=0; k<ARRAY_COUNT(GNativeLookupFuncs); k++ )
		GNativeLookupFuncs[k] = NULL;
    INT Lookup = 0;
    // Core natives.
    GNativeLookupFuncs[Lookup++] = &FindCoreUObjectNative;
    GNativeLookupFuncs[Lookup++] = &FindCoreUCommandletNative;
    // auto-generated lookups and statics
	AUTO_INITIALIZE_REGISTRANTS_ENGINE;
	AUTO_INITIALIZE_REGISTRANTS_XBOXDRV;
	AUTO_INITIALIZE_REGISTRANTS_FIRE;
	AUTO_INITIALIZE_REGISTRANTS_XBOXAUDIO;
	AUTO_INITIALIZE_REGISTRANTS_D3DDRV;
	AUTO_INITIALIZE_REGISTRANTS_IPDRV;
    RegisterNamesXInterface();
    AUTO_INITIALIZE_REGISTRANTS_XINTERFACE;
    AUTO_INITIALIZE_REGISTRANTS_XGAME;
    AUTO_INITIALIZE_REGISTRANTS_UWEB;
    GNativeLookupFuncs[Lookup++] = &FindUWebUWebResponseNative;
    GNativeLookupFuncs[Lookup++] = &FindUWebUWebRequestNative;
    check( Lookup < ARRAY_COUNT(GNativeLookupFuncs) );
#endif


    appResetTimer(); // sjs
	DOUBLE StartTime = appSeconds();

	// Show splash
	if( InitialLoad )
	{
        FString imgName;
        if(appStrstr(GCmdLine,TEXT("-server"))) // sjs
        {
		    imgName = FString::Printf(TEXT("D:\\splashserver.raw"));
            Dedicated = 1;
        }
        else
        {
            int imgNum = (appRand() % NUM_SPLASH_IMAGES) + 1; // sjs
            imgName = FString::Printf(TEXT("D:\\splash%d.raw"),imgNum); // sjs
        }
        FileManagerRaw.Copy( TEXT("Z:\\splash.raw"), *imgName, 1, 1, 0, NULL ); // sjs
		ShowSplash();
	}

	// Init mode.
	GIsServer     = 1;
	GIsClient     = !ParseParam(appCmdLine(),TEXT("SERVER"));
	GIsEditor     = 0;
	GIsScriptable = 1;

	EngineClass   = UObject::StaticLoadClass(UGameEngine::StaticClass(), NULL, TEXT("ini:Engine.Engine.GameEngine"), NULL, LOAD_NoFail, NULL );
	EngineObject  = ConstructObject<UEngine>( EngineClass );
//SLUW	EngineObject->PreInit();

	// Finish entry linearization.
	if( GLinearLoad || GLinearSave )
	{
		debugf(TEXT("Entry linearization complete"));
		if( FileManagerStatic.Ar )
			FileManagerStatic.Ar->Close();
		FileManagerStatic.Ar  = NULL;
		FileManagerStatic.Cur = NULL;
		if( GLinearSave && !OptionSL )
			while(1);
	}

	// Print some memory stats.
	GlobalMemoryStatus( &MemStatus );
	debugf(TEXT("MEMORY: %d KByte of physical memory in use"), (MemStatus.dwTotalPhys - MemStatus.dwAvailPhys) / 1024 );

	// Switch to map linearization.
	GLinearSave = OptionSL;
	appStrcpy(GLinearURL, GStartMaps[GStartMapsIndex]);
	if( GLinearLoad || GLinearSave )
	{
		appStrcpy(GCmdLine,ActualCommandLine);
		appStrcpy(FileManagerLinear.Src,LinearSrc);
		FileManagerStatic.Cur = NULL;
		FileManagerStatic.Ar  = GFileManager->CreateFileReader(FileManagerStatic.Wad);
		verify(FileManagerStatic.Ar!=NULL);
	}

	// Engine initialization.
	DOUBLE Dummy = appSeconds();
	EngineObject->Init();
	debugf(TEXT("Engine->Init: %f"), (FLOAT) (appSeconds() - Dummy));

#if !USE_LINEAR_LOADING
	// Precaching.
	Dummy = appSeconds();
	GIsRunning = 1;
	if( EngineObject->Client && EngineObject->Client->Viewports(0) )
		EngineObject->Client->Viewports(0)->Repaint( 0 );
	GIsRunning = 0;
	debugf(TEXT("Precaching: %f"), (FLOAT) (appSeconds() - Dummy));	
#endif

	XSetFileCacheSize( 64 * 1024 );

	// Print some memory stats.
	GlobalMemoryStatus( &MemStatus );
	debugf(TEXT("MEMORY: %d KByte of physical memory in use"), (MemStatus.dwTotalPhys - MemStatus.dwAvailPhys) / 1024 );

	// Finish map linearization.
	if( GLinearLoad || GLinearSave )
	{
		debugf(TEXT("Map linearization complete"));
		if( FileManagerStatic.Ar )
			FileManagerStatic.Ar->Close();
		FileManagerStatic.Ar  = NULL;
		FileManagerStatic.Cur = NULL;

		if( GLinearSave )
		{
			// Restart and pass command-line to new instance.
			if( ++GStartMapsIndex > GStartMapsMaxIndex )
				while(1);
			INT* Dummy = (INT*) &LaunchData.Data;
			*(Dummy++) = GStartMapsIndex;
			*(Dummy++) = TOKEN_LINEAR_SAVE;
			debugf(TEXT("GStartMapsIndex [%i]"), *((INT*) &LaunchData.Data));
			debugf(TEXT("Launching map [%s]"), GStartMaps[GStartMapsIndex] );
			XLaunchNewImage( TCHAR_TO_ANSI(TEXT("D:\\default.xbe")), &LaunchData );	//!! DEFAULT.XBE
		}
		GUglyHackFlags|=4;

	    // Use dynamic files.
        GFileManager = &FileManagerDynamic;

		UObject::ResetLoaders( NULL, 0, 0 );
	}		

	// Start main engine loop.
	Log.Logf(TEXT("Starting Engine"));
//SLUW	EngineObject->PostInit();
	for( TObjectIterator<UClass> It; It; ++It )
		It->LoadConfig();
	
	debugf( TEXT("Startup time %f"),(FLOAT) (appSeconds() - StartTime));
	debugf( TEXT("Bytes linearly serialized %u"), GLinearBytesSerialized);

	debugf( TEXT("Entering Loop") );
	// Optionally Exec an exec file
	FString Temp;
	if( Parse(GCmdLine, TEXT("EXEC="), Temp) )
	{
		Temp = FString(TEXT("exec ")) + Temp;
		if( EngineObject->Client && EngineObject->Client->Viewports.Num() && EngineObject->Client->Viewports(0) )
			EngineObject->Client->Viewports(0)->Exec( *Temp, Log );
	}

	// Begin guarded code.
#ifndef _DEBUG
	try
	{
#endif
		// Main engine loop.
		if( !GIsRequestingExit )
			MainLoop( EngineObject );

		debugf(TEXT("Exiting Loop"));
		EngineObject->Exit();

		appPreExit();
		GIsGuarded = 0;
#ifndef _DEBUG
	}
	catch( ... )
	{
		// Crashed.
		ErrorLevel = 1;
		//Error.HandleError();
	}
#endif

	// Final shut down.
	appExit();
	GIsStarted = 0;

#if !BENCHMARKING
	if( GLinearLoad )
	{
		// Restart and pass command-line to new instance.
		INT* Dummy = (INT*) &LaunchData.Data;
			*(Dummy++) = GStartMapsIndex;
			*(Dummy++) = TOKEN_LINEAR_LOAD;
		debugf(TEXT("GStartMapsIndex [%i]"), *((INT*) &LaunchData.Data));
		debugf(TEXT("Launching map [%s]"), GStartMaps[GStartMapsIndex] );
		XLaunchNewImage( TCHAR_TO_ANSI(TEXT("D:\\default.xbe")), &LaunchData );	//!! DEFAULT.XBE
	}
	else
#endif
		while(1);

	return ErrorLevel;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


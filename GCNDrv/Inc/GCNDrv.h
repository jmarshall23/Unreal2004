/*=============================================================================
	GCNDrv.h: Unreal null viewport and platform driver.
	Copyright 1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Brandon Reinhart.
=============================================================================*/

#ifndef _INC_GCNDRV
#define _INC_GCNDRV

/*----------------------------------------------------------------------------
	Dependencies.
----------------------------------------------------------------------------*/

// System includes

// Unreal includes.
#include "Engine.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Declarations.
-----------------------------------------------------------------------------*/

// Classes.
class UGCNMcManager;
class UGCNInputManager;
class UGCNViewport;
class UGCNClient;

/*-----------------------------------------------------------------------------
	UGCNClient.
-----------------------------------------------------------------------------*/

//
// GCN implementation of the client.
//
class UGCNClient : public UClient, public FNotifyHook
{
	DECLARE_CLASS(UGCNClient,UClient,CLASS_Transient|CLASS_Config,GCNDrv)

	UGCNInputManager*	InputManager;
	UGCNMcManager*		McManager;

	INT	FullscreenViewport;

	// Constructors.
	UGCNClient();
	void StaticConstructor();

	// FNotifyHook interface.
	void NotifyDestroy( void* Src );

	// UObject interface.
	void Serialize(FArchive& Ar);
	void Destroy();
	void PostEditChange();
	void ShutdownAfterError();

	// UClient interface.
	void Init( UEngine* InEngine );
	void ShowViewportWindows( DWORD ShowFlags, INT DoShow );
	void EnableViewportWindows( DWORD ShowFlags, INT DoEnable );
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	void MakeCurrent( UViewport* InViewport );
	class UViewport* NewViewport( const FName Name );
	class UViewport* NewViewport( const FName Name, int ViewportIndex );
	void Tick();

	// UGCNClient interface
	void UpdateSplit();
	void OpenNewViewport( FURL& URL );
};

/*-----------------------------------------------------------------------------
	UGCNViewport.
-----------------------------------------------------------------------------*/

//
// A GCN viewport.
//
class UGCNViewport : public UViewport
{
	DECLARE_CLASS(UGCNViewport,UViewport,CLASS_Transient,GCNDrv)
	DECLARE_WITHIN(UGCNClient)

	// Viewport Variables.
	INT	ViewportIndex,
		HoldCount;

	// Constructor.
//SLUW	UGCNViewport(int InViewportIndex = 0);
UGCNViewport();

	void StaticConstructor();

	// UObject interface.
	void Destroy();
	void ShutdownAfterError();

	// UViewport interface.
	UBOOL Lock( BYTE* HitData=NULL, INT* HitSize=NULL );
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar );
	UBOOL ResizeViewport( DWORD BlitType, INT X=INDEX_NONE, INT Y=INDEX_NONE, UBOOL bSaveSize );
	UBOOL IsFullscreen();
	void Unlock( UBOOL Blit );
	void Repaint( UBOOL Blit );
	void SetModeCursor();
	void UpdateWindowFrame();
	void OpenWindow( DWORD ParentWindow, UBOOL Temporary, INT NewX, INT NewY, INT OpenX, INT OpenY );
	void CloseWindow();
	void UpdateInput( UBOOL Reset );
	void* GetWindow();
	void* GetServer();
	void SetMouseCapture( UBOOL Capture, UBOOL Clip, UBOOL FocusOnly );

	// UGCNViewport interface.
	void TryRenderDevice( const TCHAR* ClassName, INT NewX, INT NewY, INT NewColorBytes );
	UBOOL CauseInputEvent( INT iKey, EInputAction Action, FLOAT Delta=0.0 );
	void ExecInputCommands( const TCHAR* Cmd, FOutputDevice& Ar );
};

//
// The GCN input manager.
//
class UGCNInputManager : public USubsystem
{
	DECLARE_CLASS(UGCNInputManager,USubsystem,CLASS_Transient|CLASS_Config,GCNDrv)
	DECLARE_WITHIN(UGCNClient)

	// Common input device variables.
	DOUBLE	LastUpdateTime;

	BITFIELD	VibrationEnabled[4];

	// Pad variables.
	struct S2
	{
		UBOOL	Opened;
		DWORD	PadData;
		DWORD	OldPadData;

		UBOOL	IsVibrating;
		FLOAT	VibrateDecay;
	} PadStates[4];

	// Constructors.
	UGCNInputManager();
	void StaticConstructor();

	// UGCNInputManager interface.
	void Init();
	void Tick();
	UBOOL Exec(const TCHAR* Cmd,FOutputDevice& Ar = *GLog);
	UBOOL ClosePort(INT i);
};



/*-----------------------------------------------------------------------------
	Memory Card.
-----------------------------------------------------------------------------*/

class UGCNMcManager : public USubsystem
{
	DECLARE_CLASS(UGCNMcManager,USubsystem,CLASS_Transient|CLASS_Config,GCNDrv)
	DECLARE_WITHIN(UGCNClient)

	UGCNClient* Client;
	struct
	{
		UBOOL	CheckedStatus;
		UBOOL	Inserted;
		UBOOL	Formatted;
		UBOOL	Invalid;
		UBOOL	HasData;
		INT CheckCountdown;
		INT FreeSpace;
	} McStates[4];

	// info for checking status
	INT CurSlot;
	UBOOL Checking;
	INT CheckType, CheckFree, CheckFormat;


	// Constructors.
	UGCNMcManager();
	void StaticConstructor();

	// UGCNMcManager interface.
	void Init(UGCNClient* InClient);
	void Tick();
	UBOOL Exec(const TCHAR* Cmd,FOutputDevice& Ar = *GLog);
	
	// UObject interface.
	void Destroy();

	// Status stuff.
	void CheckStatus();
	void WaitStatus(UBOOL Wait);

	// FFileManagerGCN stuff.
	FArchive* CreateFileReader( INT Port, const TCHAR* filename, DWORD flags, FOutputDevice* error );
	FArchive* CreateFileWriter( INT Port, const TCHAR* Filename, DWORD Flags, FOutputDevice* Error );

	// MC setup stuff
	UBOOL CopyToMc( INT Port, const TCHAR *Source, const TCHAR* Dest );
	UBOOL SaveINIToMC( INT Port );
	UBOOL LoadINIFromMC( INT Port );
	UBOOL HasGameData( INT Port );

};

/*-----------------------------------------------------------------------------
	Memory card file managers.
-----------------------------------------------------------------------------*/

struct FArchiveFileReaderMC : public FArchive
{
	FOutputDevice*  Error;
	INT				File, Size, Pos;
	UGCNMcManager* McManager;
	UBOOL			ArAtEnd;

	FArchiveFileReaderMC( UGCNMcManager* InMcManager, INT InFile, FOutputDevice* InError )
	:	McManager	(InMcManager)
	,	File		(InFile)
	,	Error		(InError)
	,	Size		(0)
	,	Pos			(0)
	,	ArAtEnd		(0)
	{
		ArIsLoading = ArIsPersistent = 1;
	}
	~FArchiveFileReaderMC() {Close();}
	void Seek(INT InPos)      {Pos = InPos; ArAtEnd=0;}
	INT Tell()                {return Pos;}
	INT TotalSize()           {return Size;}
	UBOOL AtEnd()			  {return ArAtEnd;}
	UBOOL Close();
	void Serialize( void* V, INT Length );
};

class FArchiveFileWriterMC : public FArchive
{
	UGCNMcManager* McManager;
public:
	FArchiveFileWriterMC( UGCNMcManager* InMcManager, INT InFile, FOutputDevice* InError )
	:	McManager	(InMcManager)
	,	File		(InFile)
	,	Error		(InError)
	,	Pos			(0)
	{
		ArIsSaving = ArIsPersistent = 1;
	}
	~FArchiveFileWriterMC()
	{
		if( File )
			Close();
		File = NULL;
	}
	INT Tell()		{return Pos; }
	void Seek( INT InPos );
	UBOOL Close();
	void Serialize( void* V, INT Length );

protected:
	INT				File;
	FOutputDevice*	Error;
	INT				Pos;
};

/*-----------------------------------------------------------------------------
	Package.
-----------------------------------------------------------------------------*/

#define AUTO_INITIALIZE_REGISTRANTS_GCNDRV \
	UGCNClient::StaticClass(); \
	UGCNViewport::StaticClass();

//extern class UGCNDrvPackage : public UPackageBase
//{
//public:
//	const TCHAR* GetName() {return TEXT("GCNDrv");}
//	void InitPackage()
//	{
//		UGCNClient::StaticClass();
//		UGCNViewport::StaticClass();
//	}
//} GCNDrv;

#endif //_INC_GCNDRV
/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


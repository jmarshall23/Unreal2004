/*=============================================================================
	XboxDrv.h: Unreal Xbox viewport and platform driver.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/

#ifndef _INC_XBOXDRV
#define _INC_XBOXDRV

/*----------------------------------------------------------------------------
	Dependencies.
----------------------------------------------------------------------------*/

// Unreal includes.

#include "D3DDrv.h"
#include "Engine.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Declarations.
-----------------------------------------------------------------------------*/

// Classes.
class UXboxViewport;
class UXboxClient;

/*-----------------------------------------------------------------------------
	UXboxClient.
-----------------------------------------------------------------------------*/

//
// Xbox implementation of the client.
//
class DLL_EXPORT UXboxClient : public UClient
{
	DECLARE_CLASS(UXboxClient,UClient,CLASS_Transient|CLASS_Config,XboxDrv)

	// Constructors.
	UXboxClient();
	void StaticConstructor();

	// FNotifyHook interface.
	void NotifyDestroy( void* Src );

	// UObject interface.
	void Destroy();
	void PostEditChange();
	void ShutdownAfterError();

	// UClient interface.
	void Init( UEngine* InEngine );
	void ShowViewportWindows( DWORD ShowFlags, INT DoShow );
	void EnableViewportWindows( DWORD ShowFlags, INT DoEnable );
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	void Tick();
	void MakeCurrent( UViewport* InViewport );
	class UViewport* NewViewport( const FName Name );
	UViewport* GetLastCurrent();
};

/*-----------------------------------------------------------------------------
	UXboxViewport.
-----------------------------------------------------------------------------*/

//
// A Windows viewport.
//
class DLL_EXPORT UXboxViewport : public UViewport
{
	DECLARE_CLASS(UXboxViewport,UViewport,CLASS_Transient,XboxDrv)
	DECLARE_WITHIN(UXboxClient)

	enum EXboxGamepadButton
	{
		XGB_A =				0,
		XGB_B =				1,
		XGB_X =				2,
		XGB_Y =				3,
		XGB_Black =			4,
		XGB_White =			5,
		XGB_LeftTrigger =	6,
		XGB_RightTrigger =	7,
		XGB_DigitalUp =		8,
		XGB_DigitalDown =	9,
		XGB_DigitalLeft =	10,
		XGB_DigitalRight =	11,
		XGB_Start =			12,
		XGB_Back =			13,
		XGB_LeftThumb =		14,
		XGB_RightThumb =	15
	};

	struct FXboxGamepadState
	{
		HANDLE		Handle;

		FVector		LeftJoystick,
					RightJoystick;
		BYTE		ButtonPosition[16];

		EInputKey	RepeatKey;
		FLOAT		NextRepeatTime;
	};

	struct FXboxKeyboard
	{
		HANDLE		Handle;
	};

	FXboxGamepadState	Gamepads[4];

	FXboxKeyboard		Keyboards[4];

	INT	Width,
		Height;

	// Constructor.
	UXboxViewport();

	// UObject interface.
	void Destroy();
	void ShutdownAfterError();

	// UViewport interface.
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar );
	UBOOL ResizeViewport( DWORD BlitFlags, INT NewX=INDEX_NONE, INT NewY=INDEX_NONE, UBOOL bSaveSize=true );
	UBOOL IsFullscreen();
	void Repaint( UBOOL Blit );
	void SetModeCursor();
	void OpenWindow( DWORD ParentWindow, UBOOL Temporary, INT NewX, INT NewY, INT OpenX, INT OpenY );
	void CloseWindow();
	void UpdateInput( UBOOL Reset, FLOAT DeltaSeconds );
	void* GetWindow();
	void SetMouseCapture( UBOOL Capture, UBOOL Clip, UBOOL FocusOnly );
	void UpdateWindowFrame();
};

#ifdef __STATIC_LINK
	#define AUTO_INITIALIZE_REGISTRANTS_XBOXDRV \
		UXboxClient::StaticClass(); \
		UXboxViewport::StaticClass();
#endif

#endif //_INC_XBOXDRV

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


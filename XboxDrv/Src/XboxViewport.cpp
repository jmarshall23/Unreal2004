/*=============================================================================
	XboxViewport.cpp: UXboxViewport code.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "XboxDrv.h"
#include "d3d8perf.h"

//
//	Input remapping tables.
//

EInputKey	GamepadButtonMap[16] =
{
	IK_Joy1,
	IK_Joy2,
	IK_Joy3,
	IK_Joy4,
	IK_Joy5,
	IK_Joy6,
	IK_Joy7,
	IK_Joy8,
	IK_Joy9,
	IK_Joy10,
	IK_Joy11,
	IK_Joy12,
	IK_Joy13,
	IK_Joy14,
	IK_Joy15,
	IK_Joy16
};

/*-----------------------------------------------------------------------------
	Class implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UXboxViewport);

/*-----------------------------------------------------------------------------
	UXboxViewport Init/Exit.
-----------------------------------------------------------------------------*/

//
// Constructor.
//
UXboxViewport::UXboxViewport()
:	UViewport()
{
	guard(UXboxViewport::UXboxViewport);
	for (INT i=0; i<4; i++)
	{
		Gamepads[i].Handle	= NULL;
		Keyboards[i].Handle = NULL;
	}
	unguard;
}

//
// Destroy.
//
void UXboxViewport::Destroy()
{
	guard(UXboxViewport::Destroy);

	Super::Destroy();

	unguard;
}

//
// Error shutdown.
//
void UXboxViewport::ShutdownAfterError()
{
	Super::ShutdownAfterError();
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// Command line.
//
UBOOL UXboxViewport::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UWindowsViewport::Exec);
	if( UViewport::Exec( Cmd, Ar ) )
	{
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("GETCURRENTRES")) )
	{
		Ar.Logf( TEXT("%ix%i"), Width, Height );
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("GETCURRENTCOLORDEPTH")) )
	{
		Ar.Logf( TEXT("32") );
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("GETCOLORDEPTHS")) )
	{
		Ar.Log( TEXT("32") );
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("GETCURRENTRENDERDEVICE")) )
	{
		Ar.Log( RenDev->GetClass()->GetPathName() );
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("SETRES")) )
	{
		INT X=appAtoi(Cmd);
		TCHAR* CmdTemp = appStrchr(Cmd,'x') ? appStrchr(Cmd,'x')+1 : appStrchr(Cmd,'X') ? appStrchr(Cmd,'X')+1 : TEXT("");
		INT Y=appAtoi(CmdTemp);
		Cmd = CmdTemp;
		CmdTemp = appStrchr(Cmd,'x') ? appStrchr(Cmd,'x')+1 : appStrchr(Cmd,'X') ? appStrchr(Cmd,'X')+1 : TEXT("");
		if( X && Y )
		{
			UBOOL Result = RenDev->SetRes( this, X, Y, 1 );
			if( !Result )
			{
				debugf(TEXT("SetRes failed"));
			}
		}
		return 1;
	}
#ifdef _DEBUG
	else if( ParseCommand(&Cmd,TEXT("PROFILING")) )
	{
		if( ParseCommand(&Cmd,TEXT("START")) )
		{
			D3DPERF_StartPerfProfile();
			return 1;
		}
		if( ParseCommand(&Cmd,TEXT("STOP")) )
		{
			D3DPERF_StopPerfProfile();
			return 1;
		}
		if( ParseCommand(&Cmd,TEXT("RESET")) )
		{
			D3DPERF_Reset();
			return 1;
		}
		if( ParseCommand(&Cmd,TEXT("DUMP")) )
		{
			D3DPERF_Dump();
			return 1;
		}
		return 0;
	}
#endif
	else if( ParseCommand(&Cmd,TEXT("XBOXMEM")) )
	{
		// Print some memory stats.
		MEMORYSTATUS MemStatus;
		GlobalMemoryStatus( &MemStatus );
#define DETAILED_MEMSTATS 0
#if DETAILED_MEMSTATS
		debugf(TEXT("Memory Statistics\n"));
		debugf(TEXT("%8d total KByte of virtual memory"		),	MemStatus.dwTotalVirtual	/ 1024 );
		debugf(TEXT("%8d  free KByte of virtual memory."	),	MemStatus.dwAvailVirtual	/ 1024 );
		debugf(TEXT("%8d total KByte of physical memory."	),	MemStatus.dwTotalPhys		/ 1024 );
		debugf(TEXT("%8d  free KByte of physical memory."	),	MemStatus.dwAvailPhys		/ 1024 );
		debugf(TEXT("%8d total KByte of paging file."		),	MemStatus.dwTotalPageFile	/ 1024 );
		debugf(TEXT("%8d  free KByte of paging file."		),	MemStatus.dwAvailPageFile	/ 1024 );
		debugf(TEXT("%8d percent of memory is in use."		),	MemStatus.dwMemoryLoad );
		debugf(TEXT("\n"));
#else
		debugf(TEXT("%d KByte of physical memory in use"), (MemStatus.dwTotalPhys - MemStatus.dwAvailPhys) / 1024 );
		Ar.Logf(TEXT("%d KByte of physical memory in use"), (MemStatus.dwTotalPhys - MemStatus.dwAvailPhys) / 1024 );
#endif
		return 1;
	}
	else return 0;
	unguard;
}

//
//	Input handling
//
void UXboxViewport::UpdateInput( UBOOL Reset, FLOAT DeltaSeconds )
{
	guard(UXboxViewport::UpdateInput);

	UXboxClient*	Client = GetOuterUXboxClient();

	// Check for device insertions/removals.
	DWORD	GamepadInsertionMask,
			GamepadRemovalMask,
			KeyboardInsertionMask,
			KeyboardRemovalMask;
			
	XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD,&GamepadInsertionMask,&GamepadRemovalMask);
	XGetDeviceChanges(XDEVICE_TYPE_DEBUG_KEYBOARD,&KeyboardInsertionMask,&KeyboardRemovalMask);

	for(int Index = 0;Index < 4;Index++)
	{
		// Handle device removal.
		if(Gamepads[Index].Handle && (GamepadRemovalMask & (1 << Index)))
		{
			XInputClose(Gamepads[Index].Handle);
			Gamepads[Index].Handle = NULL;
		}

		if(Keyboards[Index].Handle && (KeyboardRemovalMask & (1 << Index)))
		{
			XInputClose(Keyboards[Index].Handle);
			Keyboards[Index].Handle = NULL;			
		}

		// Handle device insertion.
		if(!Gamepads[Index].Handle && (GamepadInsertionMask & (1 << Index)))
			Gamepads[Index].Handle = XInputOpen(XDEVICE_TYPE_GAMEPAD,Index,XDEVICE_NO_SLOT,NULL);

		if(!Keyboards[Index].Handle && (KeyboardInsertionMask & (1 << Index)))
			Keyboards[Index].Handle = XInputOpen(XDEVICE_TYPE_DEBUG_KEYBOARD,Index,XDEVICE_NO_SLOT,NULL);


		// Update device state.
		if(Gamepads[Index].Handle)
		{
			XINPUT_STATE	State;

			if(XInputGetState(Gamepads[Index].Handle,&State) == ERROR_SUCCESS)
			{
				Gamepads[Index].LeftJoystick  = FVector(float(State.Gamepad.sThumbLX)/32768.f, float(State.Gamepad.sThumbLY)/32768.f, 0.0f); //amb
 				Gamepads[Index].RightJoystick = FVector(float(State.Gamepad.sThumbRX)/32768.f, float(State.Gamepad.sThumbRY)/32768.f, 0.0f); //amb

				for(INT ButtonIndex = 0;ButtonIndex < 16;ButtonIndex++)
				{
					BYTE OldPosition = Gamepads[Index].ButtonPosition[ButtonIndex];

					if(ButtonIndex < 8)
						Gamepads[Index].ButtonPosition[ButtonIndex] = State.Gamepad.bAnalogButtons[ButtonIndex];
					else
						Gamepads[Index].ButtonPosition[ButtonIndex] = (State.Gamepad.wButtons & (1 << (ButtonIndex-8))) ? 255 : 0;

					if(OldPosition <= XINPUT_GAMEPAD_MAX_CROSSTALK && Gamepads[Index].ButtonPosition[ButtonIndex] > XINPUT_GAMEPAD_MAX_CROSSTALK) //amb
					{
						Client->Engine->InputEvent(this,GamepadButtonMap[ButtonIndex],IST_Press);
						Gamepads[Index].RepeatKey = GamepadButtonMap[ButtonIndex];
						Gamepads[Index].NextRepeatTime = appSeconds() + 0.2f;
					}
					else if(OldPosition > XINPUT_GAMEPAD_MAX_CROSSTALK && Gamepads[Index].ButtonPosition[ButtonIndex] <= XINPUT_GAMEPAD_MAX_CROSSTALK) //amb
					{
						Client->Engine->InputEvent(this,GamepadButtonMap[ButtonIndex],IST_Release);
						Gamepads[Index].RepeatKey = IK_None;
					}
				}
				
				FLOAT FudgeFactor = 1.0f; // sjs - why 0.2?

				Input->DirectAxis(IK_JoyX,Gamepads[Index].LeftJoystick.X*FudgeFactor,DeltaSeconds);
				Input->DirectAxis(IK_JoyY,Gamepads[Index].LeftJoystick.Y*FudgeFactor,DeltaSeconds);

				Input->DirectAxis(IK_JoyU,Gamepads[Index].RightJoystick.X*FudgeFactor,DeltaSeconds);
				Input->DirectAxis(IK_JoyV,Gamepads[Index].RightJoystick.Y*FudgeFactor,DeltaSeconds);
			}

			if(Gamepads[Index].RepeatKey != IK_None)
			{
				if(CurrentTime >= Gamepads[Index].NextRepeatTime)
				{
					Client->Engine->InputEvent(this,Gamepads[Index].RepeatKey,IST_Hold);
					Gamepads[Index].NextRepeatTime = CurrentTime + 0.1f;
				}
			}
		}

		if(Keyboards[Index].Handle)
		{
			XINPUT_DEBUG_KEYSTROKE Key;
			while( XInputDebugGetKeystroke( &Key ) == ERROR_SUCCESS )
			{ 
				if( Key.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP )
					Client->Engine->InputEvent( this, (EInputKey) Key.VirtualKey, IST_Release, 0 );
				else
				{
					Client->Engine->InputEvent( this, (EInputKey) Key.VirtualKey, IST_Press, 0 );
					if( Key.Ascii!=IK_Enter )
						Client->Engine->Key( this, (EInputKey) Key.Ascii, TCHAR(Key.Ascii) );
				}
			}
		}
	}



	unguard;
}


/*-----------------------------------------------------------------------------
	Window openining and closing.
-----------------------------------------------------------------------------*/

//
// Open this viewport's window.
//
void UXboxViewport::OpenWindow( DWORD InParentWindow, UBOOL IsTemporary, INT NewX, INT NewY, INT OpenX, INT OpenY )
{
	guard(UXboxViewport::OpenWindow);

	check(Actor);

	// Create the render device.
	RenDev = GetOuterUWindowsClient()->Engine->GRenDev;	
//	if( !RenDev )
//		RenDev = ConstructObject<URenderDevice>( FindObject<UClass>(ANY_PACKAGE,TEXT("D3DRenderDevice")), this );
	check(RenDev);

	// Initialize the render device.
	if(RenDev->Init() && RenDev->SetRes(this,640,480,1))
	{
		if(GIsRunning)
			Actor->GetLevel()->DetailChange(
				RenDev->SuperHighDetailActors ? DM_SuperHigh :
				RenDev->HighDetailActors ? DM_High :
				DM_Low
				);
	}
	else
		appErrorf(TEXT("Couldn't create render device!"));

	unguard;
}

//
// Close a viewport window.  Assumes that the viewport has been openened with
// OpenViewportWindow.  Does not affect the viewport's object, only the
// platform-specific information associated with it.
//
void UXboxViewport::CloseWindow()
{
	guard(UXboxViewport::CloseWindow);

	for(int Index = 0;Index < 4;Index++)
	{
		if(Gamepads[Index].Handle)
			XInputClose(Gamepads[Index].Handle);
	}

	unguard;
}

//
// Resize the viewport.
//
UBOOL UXboxViewport::ResizeViewport( DWORD NewBlitFlags, INT InNewX, INT InNewY, UBOOL bSaveSize )
{
	guard(UXboxViewport::ResizeViewport);

	SizeX = InNewX;
	SizeY = InNewY;

	return 1;
	unguard;
}

//
// Repaint the viewport.
//
void UXboxViewport::Repaint(UBOOL Blit)
{
	guard(UXboxViewport::Repaint);
	GetOuterUXboxClient()->Engine->Draw( this, Blit );
	unguard;
}

//
// Functions that do nothing.
//
void UXboxViewport::SetMouseCapture( UBOOL Capture, UBOOL Clip, UBOOL FocusOnly )
{
}

void* UXboxViewport::GetWindow()
{
	return NULL;
}

void UXboxViewport::SetModeCursor()
{
}

void UXboxViewport::UpdateWindowFrame()
{
}

UBOOL UXboxViewport::IsFullscreen()
{
	return 1;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


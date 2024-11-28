/*=============================================================================
	GCNViewport.cpp: UGCNViewport code.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Brandon Reinhart
=============================================================================*/

#include "GCNDrv.h"

/*-----------------------------------------------------------------------------
	Class implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UGCNViewport);

/*-----------------------------------------------------------------------------
	UGCNViewport Init/Exit.
-----------------------------------------------------------------------------*/

INT DEBUG_AddZeroed=0;
//
// Constructor.
//
//SLUWUGCNViewport::UGCNViewport(int InViewportIndex)
//SLUW:	UViewport(InViewportIndex)
UGCNViewport::UGCNViewport()
:	UViewport()
{
	guard(UGCNViewport::UGCNViewport);

	// Initial size.
	SizeX = 640;
	SizeY = 448;
	
	unguard;
}

//
// Static init.
//
void UGCNViewport::StaticConstructor()
{
	guard(UGCNViewport::StaticConstructor);

	unguard;
}

//
// Destroy.
//
void UGCNViewport::Destroy()
{
	guard(UGCNViewport::Destroy);

	Super::Destroy();
	unguard;
}

//
// Error shutdown.
//
void UGCNViewport::ShutdownAfterError()
{
	debugf( NAME_Exit, TEXT("Executing UGCNViewport::ShutdownAfterError") );

	Super::ShutdownAfterError();
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// Command line.
//
UBOOL UGCNViewport::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UGCNViewport::Exec);
	if( UViewport::Exec( Cmd, Ar ) )
	{
		return 1;
	}
	else if(ParseCommand(&Cmd,TEXT("VIBRATE")))
	{
		float	Length;

		Parse(Cmd,TEXT("Length="),Length);

		GetOuterUGCNClient()->InputManager->PadStates[ViewportIndex].VibrateDecay = Length;

		return 1;
	}
	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Window openining and closing.
-----------------------------------------------------------------------------*/

//
// Open this viewport's window.
//
void UGCNViewport::OpenWindow( DWORD InParentWindow, UBOOL IsTemporary, INT NewX, INT NewY, INT OpenX, INT OpenY )
{
	guard(UGCNViewport::OpenWindow);

	// Create rendering device.
	if( !RenDev && !GIsEditor && !ParseParam(appCmdLine(),TEXT("nohard")) )
		TryRenderDevice( TEXT("ini:Engine.Engine.GameRenderDevice"), NewX, NewY, ColorBytes );
	check(RenDev);
	UpdateWindowFrame();
	Repaint( 1 );

	unguard;
}

//
// Close a viewport window.  Assumes that the viewport has been openened with
// OpenViewportWindow.  Does not affect the viewport's object, only the
// platform-specific information associated with it.
//
void UGCNViewport::CloseWindow()
{
	guard(UGCNViewport::CloseWindow);
	unguard;
}

/*-----------------------------------------------------------------------------
	UGCNViewport operations.
-----------------------------------------------------------------------------*/

//
// Repaint the viewport.
//
void UGCNViewport::Repaint( UBOOL Blit )
{
	guard(UGCNViewport::Repaint);
	GetOuterUGCNClient()->Engine->Draw( this, Blit );
	unguard;
}

//
// Return whether fullscreen.
//
UBOOL UGCNViewport::IsFullscreen()
{
	guard(UGCNViewport::IsFullscreen);
	return 1;
	unguard;
}

//
// Set the mouse cursor according to Unreal or UnrealEd's mode, or to
// an hourglass if a slow task is active.
//
void UGCNViewport::SetModeCursor()
{
	guard(UGCNViewport::SetModeCursor);
	unguard;
}

//
// Update user viewport interface.
//
void UGCNViewport::UpdateWindowFrame()
{
	guard(UGCNViewport::UpdateWindowFrame);

	unguard;
}

//
// Return the viewport's window.
//
void* UGCNViewport::GetWindow()
{
	guard(UGCNViewport::GetWindow);
	return NULL;
	unguard;
}

//
// Return the viewport's display.
//
void* UGCNViewport::GetServer()
{
	guard(UGCNViewport::GetServer);
	return NULL;
	unguard;
}

void UGCNViewport::ExecInputCommands( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UGCNViewport::ExecInputCommands);
	TCHAR Line[256];
	while( ParseLine( &Cmd, Line, ARRAY_COUNT(Line)) )
	{
		const TCHAR* Str = Line;
		Input->Exec( Str, Ar );
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Input.
-----------------------------------------------------------------------------*/

//
// Input event router.
//
UBOOL UGCNViewport::CauseInputEvent( INT iKey, EInputAction Action, FLOAT Delta )
{
	guard(UGCNViewport::CauseInputEvent);

	// Route to engine if a valid key; some keyboards produce key
	// codes that go beyond IK_MAX.
	if( iKey>=0 && iKey<IK_MAX )
		return GetOuterUGCNClient()->Engine->InputEvent( this, (EInputKey)iKey, Action, Delta );
	else
		return 0;

	unguard;
}

//
// If the cursor is currently being captured, stop capturing, clipping, and 
// hiding it, and move its position back to where it was when it was initially
// captured.
//
void UGCNViewport::SetMouseCapture( UBOOL Capture, UBOOL Clip, UBOOL OnlyFocus )
{
	guard(UGCNViewport::SetMouseCapture);

	unguard;
}

//
// Update input for this viewport.
//
void UGCNViewport::UpdateInput( UBOOL Reset )
{
	guard(UGCNViewport::UpdateInput);
	if( ViewportIndex==0 )
		GetOuterUGCNClient()->InputManager->Tick();
	unguard;
}

/*-----------------------------------------------------------------------------
	Lock and Unlock.
-----------------------------------------------------------------------------*/

//
// Lock the viewport window and set the approprite Screen and RealScreen fields
// of Viewport.  Returns 1 if locked successfully, 0 if failed.  Note that a
// lock failing is not a critical error; it's a sign that a DirectDraw mode
// has ended or the user has closed a viewport window.
//
UBOOL UGCNViewport::Lock( BYTE* HitData, INT* HitSize )
{
	guard(UGCNViewport::LockWindow);
//	UGCNClient* Client = GetOuterUGCNClient();
//	clock(Client->DrawCycles);

	// Success here, so pass to superclass.
//	unclock(Client->DrawCycles);
	return UViewport::Lock(HitData, HitSize);
	
	unguard;
}

//
// Unlock the viewport window.  If Blit=1, blits the viewport's frame buffer.
//
void UGCNViewport::Unlock( UBOOL Blit )
{
	guard(UGCNViewport::Unlock);

	UViewport::Unlock( Blit );

	unguard;
}

/*-----------------------------------------------------------------------------
	Viewport modes.
-----------------------------------------------------------------------------*/

//
// Try switching to a new rendering device.
//
void UGCNViewport::TryRenderDevice( const TCHAR* ClassName, INT NewX, INT NewY, INT NewColorBytes )
{
	guard(UGCNViewport::TryRenderDevice);

	// Shut down current rendering device.
	if( RenDev )
	{
		RenDev->Exit(this);
		delete RenDev;
		RenDev = NULL;
	}

	// Use appropriate defaults.
	UGCNClient* C = GetOuterUGCNClient();
	if( NewX==INDEX_NONE )
		NewX = C->FullscreenViewportX;
	if( NewY==INDEX_NONE )
		NewY = C->FullscreenViewportY;

	SizeX = NewX;
	SizeY = NewY;

	// Find device driver.
	UClass* RenderClass = UObject::StaticLoadClass( URenderDevice::StaticClass(), NULL, ClassName, NULL, 0, NULL );
	if( RenderClass )
	{
		debugf( TEXT("Loaded render device class.") );
		HoldCount++;
		RenDev = ConstructObject<URenderDevice>( RenderClass, this );
//		if( RenDev->Init( this, NewX, NewY, NewColorBytes, 1 ) )
		if( RenDev->Init() )
		{
			if( GIsRunning )
				Actor->GetLevel()->DetailChange( 1 );
		}
		else
		{
			debugf( NAME_Log, LocalizeError(TEXT("Failed3D"), TEXT("Setup")) );
			delete RenDev;
			RenDev = NULL;
		}
		HoldCount--;
	}
	unguard;
}

//
// Resize the viewport.
//
UBOOL UGCNViewport::ResizeViewport( DWORD BlitType, INT X, INT Y, UBOOL bSaveSize )
{
	guard(UGCNViewport::ResizeViewport);
	return 1;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


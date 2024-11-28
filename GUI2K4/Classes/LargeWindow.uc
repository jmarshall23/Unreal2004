//==============================================================================
//	Created on: 10/10/2003
//	Base class for larger non-full screen menus
//  Background images are sized according to the size of the page.
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class LargeWindow extends FloatingWindow;

defaultproperties
{
	OnCreateComponent=None
	bResizeWidthAllowed=false
	bResizeHeightAllowed=false
	bMoveAllowed=false
	bRequire640x480=True
	PropagateVisibility=false
	bCaptureInput=True
	WinLeft=0.2
	WinTop=0.2
	WinHeight=0.6
	WinWidth=0.6
	InactiveFadeColor=(R=60,G=60,B=60,A=255)
}

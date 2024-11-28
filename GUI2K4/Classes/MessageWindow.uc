//==============================================================================
//	Created on: 10/10/2003
//	Base class for simple popups
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class MessageWindow extends PopupPageBase;

DefaultProperties
{
	Begin Object Class=FloatingImage Name=MessageWindowFrameBackground
//		WinWidth=1.000000
//		WinHeight=0.289063
//		WinLeft=0.000000
//		WinTop=0.335938
		WinWidth=1
		WinHeight=1
		WinLeft=0
		WinTop=0
		Image=material'2K4Menus.NewControls.Display2'
		bBoundToParent=true
		bScaleToParent=true
        DropShadowX=0
        DropShadowY=0
	End Object
	i_FrameBG=MessageWindowFrameBackground

	WinLeft=0.0
	WinHeight=0.38
	WinTop=0.3
	WinWidth=1.0

}

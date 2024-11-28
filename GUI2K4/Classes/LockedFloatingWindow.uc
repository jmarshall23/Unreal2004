//==============================================================================
//  Created on: 12/29/2003
//  This implementation of floating window has an internal frame, and is intended for
//  menus which contain one or two large components (like lists)
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class LockedFloatingWindow extends FloatingWindow;

var automated GUISectionBackground sb_Main;
var automated GUIButton b_Cancel, b_OK;

var() localized string SubCaption;  // this is the caption that will go onto the sectionbackground header
var() float EdgeBorder[4];

function InitComponent(GUIController InController, GUIComponent InOwner)
{
	Super.InitComponent(InController, InOwner);

	if ( SubCaption != "" )
		sb_Main.Caption = SubCaption;

	AlignButtons();

}

function InternalOnCreateComponent(GUIComponent NewComp, GUIComponent Sender)
{
	if ( Sender == Self )
		NewComp.bBoundToParent = True;
	else Super.InternalOnCreateComponent(NewComp, Sender);
}

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_OK )
	{
		Controller.CloseMenu(false);
		return true;
	}

	if ( Sender == b_Cancel )
	{
		Controller.CloseMenu(true);
		return true;
	}

	return false;
}

function AlignButtons()
{
	local float X,Y,Xs,Ys;
	local float WIP,HIP;

	WIP = ActualWidth();
	HIP = ActualHeight();

	Xs = b_Ok.ActualWidth() * 0.1;
	Ys = b_Ok.ActualHeight() * 0.1;

	X = 1 - ( (b_Ok.ActualWidth()  + Xs) / WIP) - (EdgeBorder[2] / WIP);
	Y = 1 - ( (b_Ok.ActualHeight() + Ys) / HIP) - (EdgeBorder[3] / WIP);

	b_Ok.WinLeft = X;
	b_Ok.WinTop = Y;

	X = 1 -( (b_Ok.ActualWidth() + b_Cancel.ActualWidth() + Xs) / WIP) - (EdgeBorder[2] / WIP);

	b_Cancel.WinLeft = X;
	b_Cancel.WinTop = Y;
}

defaultproperties
{
	InactiveFadeColor=(R=60,G=60,B=60,A=255)
	bResizeWidthAllowed=False
	bResizeHeightAllowed=False
	bAllowedAsLast=false
	bCaptureInput=True

	DefaultLeft=0.125
	DefaultTop=0.15
	DefaultWidth=0.74
	DefaultHeight=0.7

	WinLeft=0.125
	WinTop=0.15
	WinWidth=0.74
	WinHeight=0.7

	Begin Object class=AltSectionBackground name=InternalFrameImage
		WinWidth=0.675859
		WinHeight=0.550976
		WinLeft=0.040000
		WinTop=0.075000
    End Object
    sb_Main=InternalFrameImage

	Begin Object Class=GUIButton Name=LockedCancelButton
        bBoundToParent=true
		WinWidth=0.159649
		WinLeft=0.512695
		WinTop=0.872397
        Caption="Cancel"
        TabOrder=99
        OnClick=InternalOnClick
        bAutoShrink=False
    End Object
    b_Cancel=LockedCancelButton

	Begin Object Class=GUIButton Name=LockedOKButton
        bBoundToParent=true
		WinWidth=0.159649
		WinLeft=0.742188
		WinTop=0.872397
        Caption="OK"
        OnClick=InternalOnClick
        TabOrder=100
        bAutoShrink=False
    End Object
    b_OK=LockedOKButton
    EdgeBorder(0)=16
    EdgeBorder(1)=24
    EdgeBorder(2)=16
    EdgeBorder(3)=24

}

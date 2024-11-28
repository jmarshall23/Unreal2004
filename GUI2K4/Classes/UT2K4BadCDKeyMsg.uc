//==============================================================================
//	Created on: 08/15/2003
//	Menu that appears when user has invalid CD-key or CD-key is already used.
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4BadCDKeyMsg extends BlackoutWindow;

var automated GUIButton b_OK;
var automated GUILabel l_Title;

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_OK ) // OK
		Controller.ReplaceMenu(class'GameEngine'.default.MainMenuClass);

	return true;
}

function bool InternalOnKeyEvent(out byte Key, out byte State, float delta)
{
	if ( State == 3 && (Key == 0x0D || Key == 0x0B) )	// Enter / Escape
		return InternalOnClick(b_OK);

	return false;
}

defaultproperties
{
	OnKeyEvent=InternalOnKeyEvent

	Begin Object Class=GUILabel Name=BadCDLabel
		Caption="Your CD key is invalid or in use by another player"
		TextAlign=TXTA_Center
		StyleName="TextLabel"
		FontScale=FNS_Large
		WinWidth=1.000000
		WinHeight=0.231250
		WinLeft=0.000000
		WinTop=0.383333
		bMultiLine=true
		bBoundToParent=true
	End Object
	l_Title=BadCDLabel

	Begin Object Class=GUIButton Name=OkButton
		Caption="OK"
		WinWidth=0.2
		WinHeight=0.04
		WinLeft=0.4
		WinTop=0.55
		OnClick=InternalOnClick
	End Object
	b_OK=OKButton
}

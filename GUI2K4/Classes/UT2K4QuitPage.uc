//==============================================================================
//	Confirmation page for quitting the game.  Brings the user to an advertisement
//  page if this is the demo version of the game.
//
//	Created by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4QuitPage extends BlackoutWindow;

var automated GUIButton YesButton;
var automated GUIButton NoButton;
var automated GUILabel 	QuitDesc;

function bool InternalOnClick(GUIComponent Sender)
{
	if (Sender==YesButton)
	{
		if(PlayerOwner().Level.IsDemoBuild())
			Controller.ReplaceMenu("XInterface.UT2DemoQuitPage");
		else
			PlayerOwner().ConsoleCommand("exit");
	}
	else
		Controller.CloseMenu(false);

	return true;
}

function bool InternalOnKeyEvent( out byte Key, out byte State, float Delta )
{
	if ( State == 3 )
	{
		if ( Key == 0x0B ) // Cancel
			InternalOnClick(NoButton);

		else if ( Key == 0x0D )
			InternalOnClick(YesButton);
	}

	return false;
}

defaultproperties
{
	OnKeyEvent=InternalOnKeyEvent

	Begin Object Class=GUIButton Name=cYesButton
		Caption="YES"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.164063
		WinTop=0.515625
		OnClick=InternalOnClick
        TabOrder=0
        StyleName="SquareButton"
	End Object

	Begin Object Class=GUIButton Name=cNoButton
		Caption="NO"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.610937
		WinTop=0.515625
		OnClick=InternalOnClick
        TabOrder=1
        StyleName="SquareButton"
	End Object

	Begin Object class=GUILabel Name=cQuitDesc
		Caption="Are you sure you wish to quit?"
		TextALign=TXTA_Center
		TextColor=(R=253,G=237,B=244,A=255)
		TextFont="UT2HeaderFont"
		WinWidth=1
		WinLeft=0
		WinTop=0.426042
		WinHeight=32
        RenderWeight=4
	End Object

	YesButton=cYesButton
	NoButton=cNoButton
	QuitDesc=cQuitDesc
}

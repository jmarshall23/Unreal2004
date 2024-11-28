// ====================================================================
// Generic message box. For any dialog box that has nothing but a caption,
// message, and OK button.
//
// Written by Matt Oelfke
// (C) 2003, Epic Games
// ====================================================================

class UT2K4GenericMessageBox extends MessageWindow;

var automated GUIButton b_OK;
var automated GUILabel  l_Text, l_Text2;

function bool InternalOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(false);
	return true;
}

function HandleParameters(string Param1, string Param2)
{
	if ( Param1 != "" )
		l_Text.Caption = Param1;

	if ( Param2 != "" )
		l_Text2.Caption = Param2;
}

function bool InternalOnKeyEvent(out byte Key, out byte State, float delta)
{
	if ( Key == 0x0D && State == 3 )	// Enter
		return InternalOnClick(b_OK);

	return false;
}

defaultproperties
{
	OnKeyEvent=InternalOnKeyEvent

	Begin Object Class=GUIButton Name=OkButton
		Caption="OK"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.400000
		WinTop=0.549479
		OnClick=InternalOnClick
	End Object
    b_Ok=OKButton

	Begin Object class=GUILabel Name=DialogText
		Caption="WARNING"
		TextALign=TXTA_Center
		StyleName="TextButton"
		FontScale=FNS_Large
		WinWidth=0.884722
		WinHeight=0.042149
		WinLeft=0.056771
		WinTop=0.389843
		VertAlign=TXTA_Center
	End Object
    l_Text=DialogText

	Begin Object class=GUILabel Name=DialogText2
		TextALign=TXTA_Center
		StyleName="TextLabel"
		WinWidth=0.912500
		WinHeight=0.126524
		WinLeft=0.043750
		WinTop=0.431249
		bMultiline=true
	End Object
    l_Text2=DialogText2
}

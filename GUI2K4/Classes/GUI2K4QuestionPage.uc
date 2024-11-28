//==============================================================================
// UT2004 Style Question Page
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class GUI2K4QuestionPage extends GUIQuestionPage;

function bool ButtonClick(GUIComponent Sender)
{
	local int T;

	T = GUIButton(Sender).Tag;
	ParentPage.InactiveFadeColor=ParentPage.Default.InactiveFadeColor;
	if ( NewOnButtonClick(T) ) Controller.CloseMenu( bool(T & (QBTN_Cancel|QBTN_Abort)) );
	OnButtonClick(T);
	return true;
}

defaultproperties
{
	Begin Object Class=GUIImage Name=imgBack
		Image=material'2K4Menus.NewControls.Display2'
		ImageStyle=ISTY_Stretched
		ImageRenderStyle=MSTY_Normal
		DropShadow=Material'2K4Menus.Controls.shadow'
		DropShadowX=0
		DropShadowY=10
		WinWidth=1.000000
		WinHeight=0.401563
		WinLeft=0.000000
		WinTop=0.297917
	End Object

	Begin Object Class=GUILabel Name=lblQuestion
		WinTop=0.45
		WinLeft=0.15
		WinHeight=0.2
		WinWidth=0.7
		bMultiLine=true
		StyleName="TextLabel"
	End Object
	Controls(0)=imgBack
	Controls(1)=lblQuestion

	WinTop=0.0
	WinLeft=0.0
	WinWidth=1.0
	WinHeight=1.0
}

//==============================================================================
// General message page to display "ladder completed" messages, simple title
// with picture
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4SP_PictureMessage extends LargeWindow;

var automated GUIImage	imgPictureBg, imgPicture;
var automated GUIButton btnOk;
var automated GUILabel lblTitle;

function bool btnOkOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(false);
	return true;
}

/**
	param1 = title, param2 = image
*/
event HandleParameters(string Param1, string Param2)
{
	local Material img;
	Super.HandleParameters(Param1, Param2);
	lblTitle.Caption = Param1;
	img = Material(DynamicLoadObject(Param2, class'Material'));
	if (img != none) imgPicture.Image = img;
}

defaultproperties
{
/*	Begin Object class=GUIImage name=SPMimgBackground
		WinWidth=0.7
		WinHeight=0.7
		WinLeft=0.15
		WinTop=0.15
		RenderWeight=0.1
		Image=Material'2K4Menus.Controls.sectionback'
		DropShadow=Material'2K4Menus.Controls.shadow'
		DropShadowX=6
		DropShadowY=6
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Normal
		bVisible=true
	End Object
	imgBackground=SPMimgBackground
*/
	Begin Object Class=GUIButton Name=SPMbtnOk
		Caption="OK"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.401563
		WinTop=0.765625
		OnClick=btnOkOnClick
		FontScale=FNS_Small
	End Object
	btnOk=SPMbtnOk

	Begin Object class=GUIImage name=SPMimgPicture
		WinWidth=0.518750
		WinHeight=0.435000
		WinLeft=0.240000
		WinTop=0.292500
		X1=0
		Y1=0
		X2=1023
		Y2=767
		Image=Material'2K4Menus.Controls.sectionback'
		ImageColor=(R=255,G=255,B=255,A=255);
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
		RenderWeight=0.3
	End Object
	imgPicture=SPMimgPicture

	Begin Object class=GUIImage name=SPMimgPictureBg
		WinWidth=0.520750
		WinHeight=0.463250
		WinLeft=0.239000
		WinTop=0.278166
		Image=Material'2K4Menus.Controls.sectionback'
		ImageColor=(R=255,G=255,B=255,A=255);
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
		RenderWeight=0.2
	End Object
	imgPictureBg=SPMimgPictureBg

	Begin Object Class=GUILabel Name=SPLlblTitle
		WinWidth=0.680000
		WinHeight=0.078750
		WinLeft=0.160000
		WinTop=0.176667
		Caption="no title"
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextAlign=TXTA_Center
		RenderWeight=0.4
	End Object
	lblTitle=SPLlblTitle

	WinWidth=0.7
	WinHeight=0.7
	WinLeft=0.15
	WinTop=0.15
}

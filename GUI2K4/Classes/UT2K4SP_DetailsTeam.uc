//==============================================================================
// Used on the details page to display addition team deletails
// HandleParams request the teams name as 1st param
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4SP_DetailsTeam extends LargeWindow;

var automated GUIImage imgPicture;
var automated GUIButton btnOk;
var automated GUILabel lblTitle;
var automated GUIScrollTextBox cbDescription;

/** Param1 is the fully qualified name for the team */
event HandleParameters(string Param1, string Param2)
{
	local class<UT2K4TeamRoster> ETI;

	Super.HandleParameters(Param1, Param2);
	ETI = class<UT2K4TeamRoster>(DynamicLoadObject(Param1, class'Class'));
	if (ETI == none)
	{
		Warn(Param1@"is not a valid subclass of UT2K4TeamRoster");
		return;
	}
	lblTitle.Caption = ETI.default.TeamName;
	if (ETI.default.TeamSymbolName != "") imgPicture.Image = Material(DynamicLoadObject(ETI.default.TeamSymbolName, class'Material', true));
	cbDescription.SetContent(ETI.default.TeamDescription);
	if (ETI.default.VoiceOver != none) PlayerOwner().PlayOwnedSound(ETI.default.VoiceOver, SLOT_Interface, 1.0);
	else if (ETI.default.TeamNameSound != none) PlayerOwner().PlayOwnedSound(ETI.default.TeamNameSound, SLOT_Interface, 1.0);
}

function bool btnOkOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(false);
	return true;
}

defaultproperties
{
	Begin Object Class=GUIButton Name=SPDTbtnOk
		Caption="CLOSE"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.337813
		WinTop=0.865625
		OnClick=btnOkOnClick
		FontScale=FNS_Small
		bBoundToParent=true
	End Object
	btnOk=SPDTbtnOk

	Begin Object class=GUIImage name=SPDTimgPicture
		WinWidth=0.225000
		WinHeight=0.412500
		WinLeft=0.031250
		WinTop=0.162500
		ImageColor=(R=255,G=255,B=255,A=255);
		ImageRenderStyle=MSTY_Alpha
		ImageAlign=IMGA_Center
		ImageStyle=ISTY_Justified
		RenderWeight=0.3
		bBoundToParent=true
	End Object
	imgPicture=SPDTimgPicture

	Begin Object Class=GUILabel Name=SPDTlblTitle
		WinWidth=0.567500
		WinHeight=0.053750
		WinLeft=0.027500
		WinTop=0.068333
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextAlign=TXTA_Center
		RenderWeight=0.4
		bBoundToParent=true
	End Object
	lblTitle=SPDTlblTitle

	Begin Object Class=GUIScrollTextBox Name=SPDTcbDescription
		WinWidth=0.324999
		WinHeight=0.412500
		WinLeft=0.425000
		WinTop=0.166667
		bNoTeletype=true
		FontScale=FNS_Medium
		bBoundToParent=true
	End Object
	cbDescription=SPDTcbDescription

	DefaultLeft=0.080000
	DefaultTop=0.193333
	DefaultWidth=0.827500
	DefaultHeight=0.598749

	WinLeft=0.080000
	WinTop=0.193333
	WinWidth=0.827500
	WinHeight=0.598749
}

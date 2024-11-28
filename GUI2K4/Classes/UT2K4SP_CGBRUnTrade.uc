//==============================================================================
// Single Player Challenge Bloodrite Trade page
// Message that you lost a player, no interaction, just a notice
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SP_CGBRUnTrade extends LargeWindow;

var automated GUIScrollTextBox sbMessage;
var automated GUIButton btnOk;
var automated GUIImage imgPicture, imgPictureBg;
var automated GUILabel lblTitle;

/** class reference to the team this challenge was about */
var class<UT2K4TeamRoster> ETI;

/** The player this challenge was all about */
var string TargetPlayerName;

/** the message */
var localized string Message;

/**
	Param1 = enemy team class name
	Param2 = player name
*/
event HandleParameters(string Param1, string Param2)
{
	local xUtil.PlayerRecord PR;

	ETI = class<UT2K4TeamRoster>(DynamicLoadObject(Param1, class'Class'));
	TargetPlayerName = Param2;

	PR = class'xUtil'.static.FindPlayerRecord(Param2);
	imgPicture.Image = PR.Portrait;

	SetDescription();
}

function SetDescription()
{
	local string tmp;
	tmp = repl(Message, "%player%", TargetPlayerName);
	tmp = repl(tmp, "%enemyteam%", ETI.default.TeamName);
	sbMessage.SetContent(tmp);
}

function bool onOkClick(GUIComponent Sender)
{
	Controller.CloseMenu(false);
	return true;
}

DefaultProperties
{
    Begin Object Class=GUIButton Name=SPMbtnOk
		Caption="OK"
		WinWidth=0.222222
		WinHeight=0.050694
		WinLeft=0.310070
		WinTop=0.821064
		OnClick=onOkClick
		RenderWeight=0.5
		FontScale=FNS_Small
		TabOrder=1
		bBoundToParent=true
	End Object
	btnOk=SPMbtnOk

    Begin Object Class=GUILabel Name=SPClblTitle
		Caption="Bloodrites"
		WinWidth=0.568750
		WinHeight=0.046250
		WinLeft=0.024063
		WinTop=0.067846
		RenderWeight=0.2
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextALign=TXTA_Center
		bBoundToParent=true
	End Object
	lblTitle=SPClblTitle

	Begin Object class=GUIImage name=SPCimgPicture
		WinWidth=0.116667
		WinHeight=0.312500
		WinLeft=0.060972
		WinTop=0.172531
		RenderWeight=0.15
		ImageStyle=ISTY_Scaled
		bBoundToParent=true
	End Object
	imgPicture=SPCimgPicture

	Begin Object class=GUIImage name=SPCimgPictureBg
		WinWidth=0.121667
		WinHeight=0.326250
		WinLeft=0.057222
		WinTop=0.157531
		RenderWeight=0.10
		ImageStyle=ISTY_Scaled
		Image=Material'2K4Menus.Controls.sectionback'
		bBoundToParent=true
	End Object
	imgPictureBg=SPCimgPictureBg

	Begin Object Class=GUIScrollTextBox Name=SPCsbDetails
		WinWidth=0.377082
		WinHeight=0.318611
		WinLeft=0.316875
		WinTop=0.166488
		RenderWeight=0.2
		TabOrder=1
		bNoTeletype=true
		StyleName="NoBackground"
		bBoundToParent=true
	End Object
	sbMessage=SPCsbDetails

	DefaultWidth=0.6
	DefaultHeight=0.5
	DefaultLeft=0.2
	DefaultTop=0.25

	WinWidth=0.6
	WinHeight=0.5
	WinLeft=0.2
	WinTop=0.25

	Message="Because you lost the challenge against %enemyteam% you have to give up your team mate %player%.|%player% now belongs to the %enemyteam%"
}

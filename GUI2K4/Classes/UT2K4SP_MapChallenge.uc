//==============================================================================
// Challenge for the alternative map
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SP_MapChallenge extends LargeWindow;

var automated GUILabel lblTitle;
var automated GUIButton btnOk, btnCancel;
var automated GUIImage imgMapBg, imgMap;
var automated GUIScrollTextBox lblDesc;

var localized string ChallengeDesc;

var UT2K4GameProfile GP;
/** Challenge match info */
var UT2K4MatchInfo ChalMI;
var CacheManager.MapRecord ActiveMap;

var float ChangeCost;

/** called when the OK button was clicked */
delegate MapSelectionUpdate();

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local UT2K4MatchInfo MI;
	local UT2K4SP_Main MainWindow;
	local CacheManager.MapRecord MR;
	local string tmp;

	Super.Initcomponent(MyController, MyOwner);
	GP = UT2K4GameProfile(PlayerOwner().Level.Game.CurrentGameProfile);
	if (GP == none)
	{
		Warn("GP == none");
		return;
	}

	// find main window
	MainWindow = UT2K4SP_Main(Controller.FindMenuByClass(class'UT2K4SP_Main'));
/*
	for (i = Controller.MenuStack.length-1; i>=0; i--)
	{
		if (UT2K4SP_Main(Controller.MenuStack[i]) != None)
		{
			MainWindow = UT2K4SP_Main(Controller.MenuStack[i]);
			break;
		}
	}
*/
	// get alternative map info
	MI = GP.GetSelectedMatchInfo(GP.CurrentLadder, GP.CurrentMenuRung, GP.GetAltLevel(GP.CurrentLadder, GP.CurrentMenuRung), GP.GetSelectedLevel(GP.CurrentLadder, GP.CurrentMenuRung) != -1);
	MR = class'CacheManager'.static.getMapRecord(MI.LevelName);
	if (MR.FriendlyName != "") tmp = MR.FriendlyName;
		else tmp = MR.MapName;
	ChallengeDesc = repl(ChallengeDesc, "%altmap%", tmp);
	if (MR.ScreenshotRef != "")	imgMap.Image = Material(DynamicLoadObject(MR.ScreenshotRef, class'Material'));

	// get current map info
	MI = UT2K4MatchInfo(GP.GetMatchInfo(GP.CurrentLadder, GP.CurrentMenuRung));
	MR = class'CacheManager'.static.getMapRecord(MI.LevelName);
	if (MR.FriendlyName != "") tmp = MR.FriendlyName;
		else tmp = MR.MapName;
	ChallengeDesc = repl(ChallengeDesc, "%curmap%", tmp);

	ChangeCost = MI.PrizeMoney * GP.MapChallengeCost;
	ChallengeDesc = repl(ChallengeDesc, "%fee%", GP.MoneyToString(ChangeCost));

	lblDesc.setContent(ChallengeDesc);

	if (imgMap.Image==None) imgMap.Image = Material'UCGeneric.SolidColours.Black';
}

function bool OnOkClick(GUIComponent Sender)
{
	GP.Balance -= ChangeCost;
	if (GP.GetSelectedLevel(GP.CurrentLadder, GP.CurrentMenuRung) != -1) GP.ResetSelectedLevel(GP.CurrentLadder, GP.CurrentMenuRung);
	else GP.SetSelectedLevel(GP.CurrentLadder, GP.CurrentMenuRung, GP.GetAltLevel(GP.CurrentLadder, GP.CurrentMenuRung));
    MapSelectionUpdate();
	return Controller.CloseMenu(false);
}

function bool OnCancelClick(GUIComponent Sender)
{
	return Controller.CloseMenu(true);
}

defaultproperties
{

	Begin Object Class=GUILabel Name=SPClblTitle
		Caption="Change Arena"
		WinWidth=0.762501
		WinHeight=0.052500
		WinLeft=0.021563
		WinTop=0.057847
		RenderWeight=0.2
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextALign=TXTA_Center
		bBoundToParent=true
	End Object
	lblTitle=SPClblTitle

	Begin Object Class=GUIButton Name=SPMbtnOk
		Caption="CHANGE"
		WinWidth=0.222222
		WinHeight=0.044444
		WinLeft=0.685071
		WinTop=0.854397
		OnClick=onOkClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=1
		bBoundToParent=true
	End Object
	btnOk=SPMbtnOk

	Begin Object Class=GUIButton Name=SPCbtnCancel
		Caption="CANCEL"
		WinWidth=0.222222
		WinHeight=0.044444
		WinLeft=0.037848
		WinTop=0.854397
		OnClick=onCancelClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=2
		bBoundToParent=true
	End Object
	btnCancel=SPCbtnCancel

	Begin Object class=GUIImage name=SPCimgMapBg
		WinWidth=0.444444
		WinHeight=0.333333
		WinLeft=0.222222
		WinTop=0.144641
		RenderWeight=0.1
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
		bBoundToParent=true
	End Object
	imgMapBg=SPCimgMapBg

	Begin Object class=GUIImage name=SPCimgMap
		WinWidth=0.441667
		WinHeight=0.321250
		WinLeft=0.224722
		WinTop=0.155865
		RenderWeight=0.15
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
		bBoundToParent=true
	End Object
	imgMap=SPCimgMap

	Begin Object Class=GUIScrollTextBox Name=SPClblDesc
		WinWidth=0.731251
		WinHeight=0.090000
		WinLeft=0.042813
		WinTop=0.717846
		RenderWeight=0.2
		TabOrder=1
		bNoTeletype=true
		StyleName="NoBackground"
		bBoundToParent=true
	End Object
	lblDesc=SPClblDesc

	ChallengeDesc="The current arena for this match is %curmap%.|You can change this arena to %altmap% when you pay %fee%."

	DefaultLeft=0.1
	DefaultTop=0.2
	DefaultWidth=0.8
	DefaultHeight=0.6

	WinLeft=0.1
	WinTop=0.2
	WinWidth=0.8
	WinHeight=0.6
}

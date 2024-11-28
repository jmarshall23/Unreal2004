//==============================================================================
// Challenge a team for a deathmatch (SP)
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SP_Challenge extends LargeWindow;

var automated GUIImage imgMap1, imgMap2, imgMapBg1, imgMapBg2;
var automated GUIButton btnOk, btnCancel;
var automated GUILabel lblTitle, lblGameDescription, lblNoPreview1, lblNoPreview2;
var automated GUIComboBox cbGameType;
var automated GUIGFXButton cbMap1, cbMap2;
var automated moComboBox cbEnemyTeam;
var automated GUIScrollTextBox sbDetails;

var UT2K4GameProfile GP;

/** team role selection window */
var string TeamRoleWindow;
var UT2K4SP_TeamRoles RoleWindow;

/** we have been challenged by this team */
var string ChallengedBy;
/** the penalty we pay when we cancel the challenge */
var int CancelPenalty;

var array<CacheManager.MapRecord> MapData;
/** The two maps to choose from */
var CacheManager.MapRecord MapOptions[2];

/** PrizeMoney = enemy team size * this number */
var int BasePrizeMoney;
/** This is added to the base price money after it has been multiplied with a random float */
var int VarPrizeMoney;

struct ChallengeGameType
{
	var string GameType;
	var string ExtraUrl;
	var int MaxBots;
	var string MapAcronym;
	var string MapPrefix;
	var int BaseGoalScore;
	var int VarGoalScore;
};
/** Possible challenge matches */
var array<ChallengeGameType> ChallengeGames;
/** localized names for the above types */
var localized array<string> ChallengeGameNames;

var localized string PenaltyWarning, ChallengeDescription, ChallengedDescription, NotEnoughCash, YouveBeenChallenged, PlayTitle;
/** always append this to the url */
var string DefaultUrl;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;

	Super.Initcomponent(MyController, MyOwner);
	GP = UT2K4GameProfile(PlayerOwner().Level.Game.CurrentGameProfile);
	for (i = 0; i < ChallengeGames.length; i++)
	{
		cbGameType.AddItem(ChallengeGameNames[i]);
	}
	for ( i = 0; i < Controls.Length; i++ )
	{
		Controls[i].bScaleToParent=True;
		Controls[i].bBoundToParent=True;
	}
	cbGameType.SetIndex(rand(ChallengeGames.length));  // get an random game
	cbEnemyTeam.MyComboBox.List.Sort();
	cbEnemyTeam.SetIndex(0);
	cbGameType.Edit.FontScale = cbGameType.FontScale;
	cbGameType.Edit.CaptionAlign = TXTA_Center;
	cbGameType.List.FontScale = cbGameType.FontScale;
	cbGameType.List.TextAlign = TXTA_Center;

	SetupNoPreview(lblNoPreview1, imgMap1);
	SetupNoPreview(lblNoPreview2, imgMap2);
}

function GetGame(int index, optional bool noRecreateEnemies)
{
	local int j, n;
	local string tmp;
	local class<UT2K4TeamRoster> ET;

	GP.bIsChallenge = true;
	lblGameDescription.Caption = ChallengeGameNames[index];
	// get two random maps
	MapData.length = 0;
	class'CacheManager'.static.GetMapList(MapData, ChallengeGames[index].MapAcronym);
	if (ChallengeGames[index].MapPrefix != "") // check optional prefix
	{
		for (n = MapData.length-1; n >= 0; n--)
		{
			if (Left(MapData[n].MapName, Len(ChallengeGames[index].MapPrefix)) ~= ChallengeGames[index].MapPrefix)
			{
				continue;
			}
			MapData.Remove(n, 1);
		}
	}
	for (n = 0; n < 2; n++)
	{
		do {
			if (MapData.length == 0) break;
			j = rand(MapData.length);
			MapOptions[n] = MapData[j];
			//Log("Map select"@n@"="@MapOptions[n].MapName);
			//Log(MapOptions[n].PlayerCountMin@MapOptions[n].PlayerCountMax@ChallengeGames[index].MaxBots);
			MapData.Remove(j, 1);
		} until (((ChallengeGames[index].MaxBots+1) >= MapOptions[n].PlayerCountMin)
			&& ((ChallengeGames[index].MaxBots+1) <= MapOptions[n].PlayerCountMax));
	}
	if (MapOptions[0].ScreenshotRef != "") imgMap1.Image = Material(DynamicLoadObject(MapOptions[0].ScreenshotRef, class'Material'));
	else imgMap1.Image = None;
	imgMap1.SetVisibility(imgMap1.Image != None); lblNoPreview1.SetVisibility(imgMap1.Image == None);
	if (MapOptions[1].ScreenshotRef != "") imgMap2.Image = Material(DynamicLoadObject(MapOptions[1].ScreenshotRef, class'Material'));
	else imgMap2.Image = None;
	imgMap2.SetVisibility(imgMap2.Image != None); lblNoPreview2.SetVisibility(imgMap2.Image == None);
	cbMap1.Caption = PlayTitle@MapOptions[0].MapName;
	cbMap2.Caption = PlayTitle@MapOptions[1].MapName;

	GP.ChallengeInfo = new(GP) class'UT2K4MatchInfo';
	GP.ChallengeInfo.NumBots = ChallengeGames[index].MaxBots;
	GP.ChallengeInfo.URLString = DefaultUrl$ChallengeGames[index].ExtraUrl;
	GP.ChallengeInfo.GameType = ChallengeGames[index].GameType;
	BasePrizeMoney += VarPrizeMoney*frand();
	GP.ChallengeInfo.PrizeMoney = (GP.GetNumTeammatesForMatch()+1.5)*BasePrizeMoney;
	GP.ChallengeInfo.EntryFee = 0.5*BasePrizeMoney; // half the base price
	GP.ChallengeInfo.GoalScore = ChallengeGames[index].BaseGoalScore+(ChallengeGames[index].VarGoalScore*frand());

	tmp = repl(ChallengeDescription, "%entryfee%", GP.MoneyToString(GP.ChallengeInfo.EntryFee));
	tmp = repl(tmp, "%PrizeMoney%", GP.MoneyToString(GP.ChallengeInfo.PrizeMoney));
	tmp = repl(tmp, "%balance%", GP.MoneyToString(GP.Balance));
	sbDetails.SetContent(tmp);

	// Get enemies
	if (!noRecreateEnemies)
	{
		cbEnemyTeam.MyComboBox.List.Clear();
		for (j = 0; j < GP.TeamStats.length; j++)
		{
			if (GP.TeamStats[j].Matches > 0)
			{
				if ((GP.TeamStats[j].Level >=3) && (GP.ChallengeInfo.NumBots == 1)) continue; // never add strong team in 1vs1
				ET = class<UT2K4TeamRoster>(DynamicLoadObject(GP.TeamStats[j].Name, class'Class'));
				cbEnemyTeam.AddItem(ET.Default.TeamName,,GP.TeamStats[j].Name);
			}
		}
	}

	if (cbMap1.bChecked) OnMapSelect(cbMap1);
	else if (cbMap2.bChecked) OnMapSelect(cbMap2);
	GP.bIsChallenge = false;
}

event HandleParameters(string Param1, string Param2)
{
	local string tmp;
	local int i, teampos;
	local GUIQuestionPage QPage;
	local GUIController MyController;

	if (GP.ChallengeInfo.EntryFee > GP.Balance)
	{
		MyController = Controller;
		Controller.CloseMenu(true);
		if (MyController.OpenMenu("GUI2K4.GUI2K4QuestionPage"))
		{
			QPage=GUIQuestionPage(MyController.TopPage());
			QPage.SetupQuestion(NotEnoughCash, QBTN_Ok, QBTN_Ok);
		}
		return;
	}

	if (Param1 != "")
	{
		i = cbEnemyTeam.FindExtra(Param1);
		if (i > -1)
		{
			teampos = GP.GetTeamPosition(Param1);
			if (teampos > -1)
			{
				if (GP.TeamStats[teampos].Level >= 3)
				{
					while (GP.ChallengeInfo.NumBots == 1) GetGame(rand(ChallengeGames.length), true); // no 1vs1 against top team
				}
			}
			cbGameType.bVisible = false;
			cbEnemyTeam.SetIndex(i);
			lblTitle.Caption = YouveBeenChallenged;
			cbEnemyTeam.MenuStateChange(MSAT_Disabled);
			CancelPenalty = GP.ChallengeInfo.EntryFee;
			GP.ChallengeInfo.EntryFee = 0;
			tmp = repl(ChallengedDescription, "%entryfee%", GP.MoneyToString(GP.ChallengeInfo.EntryFee));
			tmp = repl(tmp, "%PrizeMoney%", GP.MoneyToString(GP.ChallengeInfo.PrizeMoney));
			tmp = repl(tmp, "%enemyteam%", cbEnemyTeam.GetText());
			tmp = repl(tmp, "%cancelfee%", GP.MoneyToString(CancelPenalty));
			tmp = repl(tmp, "%balance%", GP.MoneyToString(GP.Balance));
			sbDetails.SetContent(tmp);
		}
	}
}

/**
	Play the selected match, first check if we need to assign team mates
*/
function bool onOkClick(GUIComponent Sender)
{
	if (GP != none)
	{
		GP.bIsChallenge = true;
		GP.ChallengeInfo.EnemyTeamName = cbEnemyTeam.GetExtra();
		GP.EnemyTeam = GP.ChallengeInfo.EnemyTeamName;
		// Difficulty = Team Level / 1.2 (strongest team is level 3 -> 2.5)
		GP.ChallengeInfo.DifficultyModifier = GP.TeamStats[GP.GetTeamPosition(GP.ChallengeInfo.EnemyTeamName)].Level/1.2;
		// 1vs1 -> make enemy 33% stronger
		if (GP.ChallengeInfo.NumBots == 1) GP.ChallengeInfo.DifficultyModifier *= 1.33;
		GP.ChallengeInfo.SpecialEvent = "";

		if (GP.GetNumTeammatesForMatch() > 0)
		{
			if (Controller.OpenMenu(TeamRoleWindow))
			{
				RoleWindow = UT2K4SP_TeamRoles(Controller.TopPage());
				RoleWindow.OnOkClick = StartMatch;
			}
		}
		else StartMatch();
	}
	else Warn("GP != none");
	return true;
}

/**	Start the selected match */
function StartMatch()
{
	local LevelInfo myLevel;
	GP.Balance -= GP.ChallengeInfo.EntryFee;
	myLevel = PlayerOwner().Level;
	GP.StartNewMatch ( -1, myLevel );
	Controller.CloseAll(false,true);
}

/**
	Cancel button pressed
*/
function bool onCancelClick(GUIComponent Sender)
{
	if (OnCanWindowClose(true)) return Controller.CloseMenu(true);
	else return false;
}

/** Player accepts to pay the penalty */
function OnConfirmCancel(byte bButton)
{
	if (bButton == QBTN_Yes)
	{
		GP.Balance -= CancelPenalty;
		CancelPenalty = 0;
		//PlayerOwner().Level.Game.SavePackage(GP.PackageName);
		Controller.CloseMenu(true);
	}
}

function OnMapSelect(GUIComponent Sender)
{
	if (Sender == cbMap1)
	{
		GP.ActiveMap = MapOptions[0];
		cbMap1.bChecked = true;
		cbMap2.bChecked = false;
	}
	else {
		GP.ActiveMap = MapOptions[1];
		cbMap1.bChecked = false;
		cbMap2.bChecked = true;
	}
	GP.ChallengeInfo.LevelName = GP.ActiveMap.MapName;
	if ((GP.ChallengeInfo.LevelName != "") && (GP.Balance > GP.ChallengeInfo.EntryFee)) btnOk.MenuStateChange(MSAT_Blurry);
		else btnOk.MenuStateChange(MSAT_Disabled);
	return;
}

function OnGameSelect(GUIComponent Sender)
{
	GetGame(cbGameType.GetIndex());
}

function OnWindowClose(optional Bool bCancelled)
{
	GP.bIsChallenge = !bCancelled;
}

function bool OnCanWindowClose(optional Bool bCancelled)
{
	local GUIQuestionPage QPage;
	if (!bCancelled) return true;
	if (CancelPenalty > 0)
	{
		if (Controller.OpenMenu("GUI2K4.GUI2K4QuestionPage"))
		{
			QPage=GUIQuestionPage(Controller.TopPage());
			QPage.SetupQuestion(repl(PenaltyWarning, "%cancelpenalty%", GP.MoneyToString(CancelPenalty)), QBTN_YesNo, QBTN_No);
			QPage.OnButtonClick = OnConfirmCancel;
		}
		return false;
	}
	return true;
}

event SetVisibility(bool bIsVisible)
{
	Super.SetVisibility(bIsVisible);
	if ( imgMap1 != None )
	{
		imgMap1.SetVisibility(imgMap1.Image != None);
		if ( lblNoPreview1 != None ) lblNoPreview1.SetVisibility(imgMap1.Image == None);
	}
	if ( imgMap2 != None )
	{
		imgMap2.SetVisibility(imgMap2.Image != None);
		if ( lblNoPreview2 != None ) lblNoPreview2.SetVisibility(imgMap2.Image == None);
	}
}

protected function SetupNoPreview( GUILabel lbl, GUIImage img )
{
	if ( lbl == None || img == None ) return;
	lbl.WinLeft   = img.WinLeft;
	lbl.WinTop    = img.WinTop;
	lbl.WinWidth  = img.WinWidth;
	lbl.WinHeight = img.WinHeight;
}

defaultproperties
{
	Begin Object Class=GUIButton Name=SPMbtnOk
		Caption="ACCEPT"
		WinWidth=0.222222
		WinHeight=0.044444
		WinLeft=0.710071
		WinTop=0.887731
		OnClick=onOkClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=1
		MenuState=MSAT_Disabled
	End Object
	btnOk=SPMbtnOk

	Begin Object Class=GUIButton Name=SPCbtnCancel
		Caption="REFUSE"
		WinWidth=0.222222
		WinHeight=0.044444
		WinLeft=0.050348
		WinTop=0.887731
		OnClick=onCancelClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=2
	End Object
	btnCancel=SPCbtnCancel

	Begin Object Class=GUILabel Name=SPClblTitle
		Caption="Challenge"
		WinWidth=0.862501
		WinHeight=0.077500
		WinLeft=0.067813
		WinTop=0.017847
		RenderWeight=0.2
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextALign=TXTA_Center
	End Object
	lblTitle=SPClblTitle

	Begin Object class=GUIImage name=SPCimgMapBg1
		WinWidth=0.444444
		WinHeight=0.333333
		WinLeft=0.020833
		WinTop=0.186308
		RenderWeight=0.1
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
	End Object
	imgMapBg1=SPCimgMapBg1

	Begin Object class=GUIImage name=SPCimgMapBg2
		WinWidth=0.444444
		WinHeight=0.333333
		WinLeft=0.534722
		WinTop=0.186308
		RenderWeight=0.1
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
	End Object
	imgMapBg2=SPCimgMapBg2

	Begin Object class=GUIImage name=SPCimgMap1
		WinWidth=0.441667
		WinHeight=0.312500
		WinLeft=0.022222
		WinTop=0.197531
		RenderWeight=0.15
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
	End Object
	imgMap1=SPCimgMap1

	Begin Object class=GUIImage name=SPCimgMap2
		WinWidth=0.441667
		WinHeight=0.312500
		WinLeft=0.536111
		WinTop=0.197531
		RenderWeight=0.15
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
	End Object
	imgMap2=SPCimgMap2

	Begin Object Class=GUILabel Name=SPClblGameDescription
		Caption=""
		WinWidth=0.862501
		WinHeight=0.077500
		WinLeft=0.067813
		WinTop=0.101180
		RenderWeight=0.2
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextALign=TXTA_Center
	End Object
	lblGameDescription=SPClblGameDescription

	Begin Object class=GUIComboBox Name=SPCcbGameType
		Hint="Select the game"
		WinWidth=0.958334
		WinHeight=0.079167
		WinLeft=0.019792
		WinTop=0.097830
		RenderWeight=0.21
		FontScale=FNS_Large
		bReadOnly=true
		TabOrder=0
		OnChange=OnGameSelect
	End Object
	cbGameType=SPCcbGameType

	Begin Object class=GUIGFXButton Name=SPCcbMap1
		Caption=""
		WinWidth=0.444444
		WinHeight=0.062500
		WinLeft=0.020833
		WinTop=0.519641
		bCheckBox=true
		OnChange=OnMapSelect
	End Object
	cbMap1=SPCcbMap1

	Begin Object class=GUIGFXButton Name=SPCcbMap2
		Caption=""
		WinWidth=0.444444
		WinHeight=0.062500
		WinLeft=0.534722
		WinTop=0.519641
		bCheckBox=true
		OnChange=OnMapSelect
	End Object
	cbMap2=SPCcbMap2

	Begin Object class=moComboBox Name=SPCcbEnemyTeam
		Caption="Challenge team:"
		Hint="Challenge this team for a match"
		bAutoSizeCaption=true
		WinWidth=0.952779
		WinHeight=0.059722
		WinLeft=0.024306
		WinTop=0.594780
		bReadOnly=true
		TabOrder=3
	End Object
	cbEnemyTeam=SPCcbEnemyTeam

	Begin Object Class=GUIScrollTextBox Name=SPCsbDetails
		WinWidth=0.958334
		WinHeight=0.211111
		WinLeft=0.019792
		WinTop=0.667977
		RenderWeight=0.2
		TabOrder=1
		bNoTeletype=true
		StyleName="NoBackground"
	End Object
	sbDetails=SPCsbDetails

    Begin Object Class=GUILabel Name=NoPreview
        TextFont="UT2HeaderFont"
        TextAlign=TXTA_Center
        VertAlign=TXTA_Center
        bMultiline=True
        bTransparent=False
        TextColor=(R=247,G=255,B=0,A=255)
        Caption="No Preview Available"
		WinWidth=0.441667
		WinHeight=0.312500
		WinLeft=0.022222
		WinTop=0.197531
    End Object
    lblNoPreview1=NoPreview
    lblNoPreview2=NoPreview

	WinWidth=0.9
	WinHeight=0.9
	WinLeft=0.05
	WinTop=0.05
	OnClose=OnWindowClose
	OnCanClose=OnCanWindowClose

	PlayTitle="Play"
	TeamRoleWindow="GUI2K4.UT2K4SP_TeamRoles"
	YouveBeenChallenged="You've been challenged!"
	PenaltyWarning="When you refuse you will have to pay a penalty of %cancelpenalty%.||Are you sure you want to refuse?"
	ChallengeDescription="To enter this challenge you have to pay the entry fee of %entryfee%.|You can win %PrizeMoney% when you win this match.|Your current balance is %balance%."
	ChallengedDescription="You have been challenged by %enemyteam%.|You can win %PrizeMoney% when you defeat them.|When you don't accept this challenge you have to pay %cancelfee% to the opposing team.|Your current balance is %balance%."
	NotEnoughCash="You do not have enough money to challenge another team."
	DefaultUrl="?TeamScreen=true"
	BasePrizeMoney=400
	VarPrizeMoney=100

	ChallengeGameNames[0]="Team DeathMatch"
	ChallengeGames[0]=(GameType="xGame.xTeamGame",ExtraUrl="",MaxBots=5,MapAcronym="DM",MapPrefix="",BaseGoalScore=20,VarGoalScore=5)
	ChallengeGameNames[1]="1vs1 DeathMatch"
	ChallengeGames[1]=(GameType="xGame.xDeathmatch",ExtraUrl="",MaxBots=1,MapAcronym="DM",MapPrefix="DM-1on1",BaseGoalScore=10,VarGoalScore=5)
	ChallengeGameNames[2]="Mutant"
	ChallengeGames[2]=(GameType="BonusPack.xMutantGame",ExtraUrl="",MaxBots=4,MapAcronym="DM",MapPrefix="",BaseGoalScore=15,VarGoalScore=5)
	ChallengeGameNames[3]="Instagib Team DeathMatch"
	ChallengeGames[3]=(GameType="xGame.xTeamGame",ExtraUrl="?mutator=XGame.MutInstaGib",MaxBots=7,MapAcronym="DM",MapPrefix="",BaseGoalScore=30,VarGoalScore=10)
}

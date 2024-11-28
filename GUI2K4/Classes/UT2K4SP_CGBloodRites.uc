//==============================================================================
// Challenge a team and win a team mate
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SP_CGBloodRites extends LargeWindow;

var automated GUIImage imgMap1, imgMap2, imgMapBg1, imgMapBg2;
var automated GUIButton btnOk, btnCancel, btnNextChar, btnPrevChar;
var automated GUILabel lblTitle, lblNoPreview1, lblNoPreview2, lblSelChar, lblSelCharName, lblBalance;
var automated GUIGFXButton cbMap1, cbMap2;
var automated moComboBox cbEnemyTeam;
var automated GUIScrollTextBox sbDetails;
var automated GUICharacterListTeam clSelChar;
var automated moCheckBox cbInstaGib;

var UT2K4GameProfile GP;

/** the penalty we pay when we cancel the challenge */
var int CancelPenalty;

var array<CacheManager.MapRecord> MapData;
/** The two maps to choose from */
var CacheManager.MapRecord MapOptions[2];

/** set to the preselect/challenged team */
var string ChallengedBy;
/** true if we have been challenged */
var bool bChallenged;

var localized string PenaltyWarning, YouveBeenChallenged, SelectAMap,
	msgChallenge, msgChallenged, msgInstaGib, CantAffordCancel;
/** always append this to the url */
var string DefaultUrl;

/** default gametype = xGame.xTeamGame */
var string ChalGameType;
/** additional URL for an instagib game = ?mutator = xGame.MutInstaGib */
var string MutInstaGib;
/** multiplicator for the cancel fee when challenged */
var float ChalledFeeMultiply;
/** diffirent goal scores for normal and insta game */
var int NormalGoalScore, InstaGoalScore;
/** prefix of the maps to use */
var string MapPreFix;


var int minimalEntryFee;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;

	Super.Initcomponent(MyController, MyOwner);
	GP = UT2K4GameProfile(PlayerOwner().Level.Game.CurrentGameProfile);
	for ( i = 0; i < Controls.Length; i++ )
	{
		Controls[i].bScaleToParent=True;
		Controls[i].bBoundToParent=True;
	}

	class'CacheManager'.static.GetMapList(MapData, MapPreFix);
	for (i = MapData.length-1; i >= 0; i--)
	{
		if (MapData[i].PlayerCountMax < 7)    // remove small maps
		{
			MapData.Remove(i, 1);
			continue;
		}
		if (class'CacheManager'.static.Is2004Content(MapData[i].MapName)) continue;
		if (class'CacheManager'.static.Is2003Content(MapData[i].MapName)) continue;
		if (class'CacheManager'.static.IsBPContent(MapData[i].MapName)) continue;
		MapData.Remove(i, 1); // remove custom maps
	}

	SetupNoPreview(lblNoPreview1, imgMap1);
	SetupNoPreview(lblNoPreview2, imgMap2);

	sbDetails.SetContent(SelectAMap);
	GetRandomMaps();

	lblBalance.Caption = class'UT2K4SPTab_LadderBase'.default.BalanceLabel@GP.MoneyToString(GP.Balance);
}

function int CheapestBot(array<string> list)
{
	local int curmin, i, bp;
	curmin = MaxInt;
	for (i = 0; i < list.length; i++)
	{
		bp = GP.GetBotPrice(list[i],,, true);
		if (bp < curmin) curmin = bp;
	}
	return curmin;
}

function GetRandomMaps()
{
	MapOptions[0] = MapData[rand(MapData.length)];
	do {
		if (MapData.length <= 1)
		{
			Warn("MapData.length <= 1");
			break;
		}
		MapOptions[1] = MapData[rand(MapData.length)];
	} until (MapOptions[0] != MapOptions[1]);

	if (MapOptions[0].ScreenshotRef != "") imgMap1.Image = Material(DynamicLoadObject(MapOptions[0].ScreenshotRef, class'Material'));
	lblNoPreview1.bVisible = MapOptions[0].ScreenshotRef == "";
	cbMap1.Caption = MapOptions[0].MapName;
	if (MapOptions[1].ScreenshotRef != "") imgMap2.Image = Material(DynamicLoadObject(MapOptions[1].ScreenshotRef, class'Material'));
	lblNoPreview2.bVisible = MapOptions[1].ScreenshotRef == "";
	cbMap2.Caption = MapOptions[1].MapName;
}

function setChallengeInfo(int selectedMap)
{
	local string desc, tmp;
	local class<UT2K4TeamRoster> ETI;
	local CacheManager.GameRecord GR;

	GR = class'CacheManager'.static.getGameRecord(ChalGameType);
	ETI = class<UT2K4TeamRoster>(DynamicLoadObject(cbEnemyTeam.GetExtra(), class'Class'));

	GP.ChallengeInfo.LevelName = MapOptions[selectedMap].MapName;
	GP.ChallengeInfo.DifficultyModifier = ETI.default.TeamLevel / 1.2;
	GP.ChallengeInfo.PrizeMoney = 0; // never prize money for a bloodrite
	GP.ChallengeInfo.GameType = ChalGameType;
	if (cbInstaGib.IsChecked()) GP.ChallengeInfo.GoalScore = InstaGoalScore;
	else GP.ChallengeInfo.GoalScore = NormalGoalScore;
	if (bChallenged) GP.ChallengeInfo.EntryFee = 0;
	else GP.ChallengeInfo.EntryFee = GP.GetBotPrice(clSelChar.GetName(),,,true)*class'xGame.BloodRites'.default.ChalFeeMultiply; // 3 times the players value
	GP.ChallengeInfo.URLString = DefaultUrl;
	if (cbInstaGib.IsChecked()) GP.ChallengeInfo.URLString $= MutInstaGib;

	if (bChallenged) desc = msgChallenged;
	else desc = msgChallenge;
	desc = repl(desc, "%map%", MapOptions[selectedMap].MapName);
	desc = repl(desc, "%enemyteam%", cbEnemyTeam.GetText());
	desc = repl(desc, "%selectedchar%", clSelChar.GetName());
	desc = repl(desc, "%entryfee%", GP.MoneyToString(GP.ChallengeInfo.EntryFee));
	if (cbInstaGib.IsChecked()) tmp = msgInstaGib@GR.GameName;
	else tmp = GR.GameName;
	desc = repl(desc, "%gametype%", tmp);
	desc = repl(desc, "%penalty%", GP.MoneyToString(CancelPenalty));
	if ((GP.Balance < GP.getMinimalEntryFeeFor(GP.ChallengeInfo)+GP.MinBalance))
	{
		desc $= "|"$class'UT2K4SP_CGManoEMano'.default.NotEnoughCash;
	}
	sbDetails.SetContent(desc);

	if (bChallenged)
	{
		if (GP.ChallengeInfo.LevelName != "") btnOk.EnableMe();
			else btnOk.DisableMe();
	}
	else if ((GP.ChallengeInfo.LevelName != "") && (GP.Balance > GP.getMinimalEntryFeeFor(GP.ChallengeInfo)+GP.MinBalance)) btnOk.EnableMe();
		else btnOk.DisableMe();
}

/**
	param1 = preselect enemy team, fully qualified classname.
	param2 = not empty -> lock team change controlls
*/
event HandleParameters(string Param1, string Param2)
{
	local int i, cnt, minTeamFee, cbot;
	local class<UT2K4TeamRoster> ET;
	local array<string> ETR;
	local GUIController MyController;

	bChallenged = (Param2 != "");

	// count available team mates
	cnt = 0;
	for (i = 0; i < GP.GetMaxTeamSize(); i++)
	{
		if (GP.PlayerTeam[i] != "")
		{
			if (GP.BotStats[GP.GetBotPosition(GP.PlayerTeam[i])].Health >= 100) cnt++;
		}
	}
	GP.ChallengeInfo.NumBots = min(cnt*2+1, 7); // 7 or the number of available team mates

	// fill enemy team teams
	cbEnemyTeam.MyComboBox.List.Clear();
	minimalEntryFee = MaxInt;
	minTeamFee = GP.getMinimalTeamFee(GP.ChallengeInfo.NumBots / 2);
	for (i = 0; i < GP.TeamStats.length; i++)
	{
		if (GP.TeamStats[i].Matches > 0)
		{
			if (GP.TeamStats[i].Name == "") continue;
			ET = class<UT2K4TeamRoster>(DynamicLoadObject(GP.TeamStats[i].Name, class'Class'));
			if (ET != none)
			{
				if (!GP.GetAltTeamRoster(GP.TeamStats[i].Name, ETR)) ETR = ET.default.RosterNames;
				// we need to keep at least 5 players in a team
				if (bChallenged)
				{
					cbEnemyTeam.AddItem(ET.Default.TeamName@"("$class'UT2K4SPTab_DetailEnemies'.default.StrengthLabels[ET.Default.TeamLevel]$")",,GP.TeamStats[i].Name);
				}
				else if (ETR.Length > 5)
				{
					cbot = CheapestBot(ETR)*class'xGame.BloodRites'.default.ChalFeeMultiply;
					if (!bChallenged) cbot += minTeamFee; // if challenged we don't pay the team
					if (cbot < minimalEntryFee) minimalEntryFee = cbot;

					if (cbot < GP.Balance-GP.MinBalance)
						cbEnemyTeam.AddItem(ET.Default.TeamName@"("$class'UT2K4SPTab_DetailEnemies'.default.StrengthLabels[ET.Default.TeamLevel]$")",,GP.TeamStats[i].Name);
				}
			}
		}
	}
	log("Min. entry fee is"@minimalEntryFee@(minimalEntryFee+GP.MinBalance), GP.LogPrefix);
	cbEnemyTeam.MyComboBox.List.Sort();
	if (cbEnemyTeam.ItemCount() > 0)
	{
		cbEnemyTeam.SetIndex(0);
		UpdateEnemyTeam(cbEnemyTeam);
	}

	// fill other info
	ChallengedBy = Param1;
	if ((ChallengedBy != "") && (cbEnemyTeam.ItemCount() > 0)) cbEnemyTeam.SetIndex(cbEnemyTeam.FindExtra(ChallengedBy));
	if (bChallenged)
	{
		if (cbEnemyTeam.ItemCount() > 0)
		{
			cbEnemyTeam.DisableMe();
			clSelChar.PopulateList(GP.PlayerTeam);
			clSelChar.Find(clSelChar.PlayerList[rand(clSelChar.ItemCount)].DefaultName);
			CancelPenalty = GP.GetBotPrice(clSelChar.GetName())*ChalledFeeMultiply;
			clSelChar.DisableMe();
			btnNextChar.DisableMe();
			btnPrevChar.DisableMe();
			cbInstaGib.Checked(rand(2));
			cbInstaGib.DisableMe();

			UpdateDetails(none);
		}

		if (GP.Balance-GP.MinBalance <= CancelPenalty)
		{
			CancelPenalty = 0;
   			Controller.CloseMenu(true);
		}
	}
	else if (GP.Balance-GP.MinBalance <= minimalEntryFee) {
		MyController = Controller;
		Controller.CloseMenu(true);
		MyController.ShowQuestionDialog(class'UT2K4SP_CGManoEMano'.default.NotEnoughCash);
		return;
	}
}

/**
	Play the selected match, first check if we need to assign team mates
*/
function bool onOkClick(GUIComponent Sender)
{
	local int nummates, healthymates, i, j;
	local GUIQuestionPage QPage;
	local UT2K4SP_TeamRoles RoleWindow;
	local string tmp;

	if (GP.ChallengeInfo.EntryFee > GP.Balance) return false;
	nummates = GP.GetNumTeammatesForMatchInfo(GP.ChallengeInfo);
	if (nummates > 0)
	{
		healthymates = 0;
		for (i = 0; i < GP.GetMaxTeamSize(); i++)
		{
			j = GP.GetBotPosition(GP.PlayerTeam[i]);
			if (j > -1) if (GP.BotStats[j].Health >= 100) healthymates++;
		}

		if (healthymates < nummates)
		{
			if (Controller.OpenMenu(Controller.QuestionMenuClass))
			{
				QPage=GUIQuestionPage(Controller.TopPage());
				QPage.SetupQuestion(repl(repl(class'UT2K4SPTab_LadderBase'.default.NotEnoughPlayers, "%healthy%", healthymates), "%teammates%", nummates), QBTN_Ok);
			}
			return true;
		}

		GP.bIsChallenge = true;
		if (bChallenged) tmp = "true";
		if (Controller.OpenMenu(class'UT2K4SPTab_LadderBase'.default.TeamRoleWindow, tmp))
		{
			RoleWindow = UT2K4SP_TeamRoles(Controller.TopPage());
			RoleWindow.OnOkClick = StartMatch;
		}
	}
	else StartMatch();

	return true;
}

function StartMatch()
{
	GP.EnemyTeam = cbEnemyTeam.GetExtra();
	GP.ChallengeInfo.EnemyTeamName = GP.EnemyTeam;
	// when we are challenged we lose a player to that team (and gain none)
	// Important: this has to be set when we LOSE, not when we win.
	GP.bGotChallenged = bChallenged;
	if (bChallenged) GP.ChallengeInfo.SpecialEvent = "UNTRADE"@GP.ChallengeInfo.EnemyTeamName@clSelChar.GetName();
	// when we challenge we get a player (we have to give up one (with reward) when we already have 5)
	else GP.ChallengeInfo.SpecialEvent = "TRADE"@GP.ChallengeInfo.EnemyTeamName@clSelChar.GetName();
	GP.ChallengeVariable = clSelChar.GetName();
	Log("Special event set to:"@GP.ChallengeInfo.SpecialEvent);
	class'xGame.BloodRites'.static.StartChallenge(GP, PlayerOwner().Level);
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
		setChallengeInfo(0);
	}
	else {
		GP.ActiveMap = MapOptions[1];
		cbMap1.bChecked = false;
		cbMap2.bChecked = true;
		setChallengeInfo(1);
	}
	return;
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
	    if (CancelPenalty > GP.Balance)
	    {
	    	if (Controller.OpenMenu("GUI2K4.GUI2K4QuestionPage"))
			{
				QPage=GUIQuestionPage(Controller.TopPage());
				QPage.SetupQuestion(repl(repl(CantAffordCancel, "%cancelpenalty%", GP.MoneyToString(CancelPenalty)), "%balance%", GP.MoneyToString(GP.Balance)), QBTN_Ok, QBTN_Ok);
			}
	    }
		else if (Controller.OpenMenu("GUI2K4.GUI2K4QuestionPage"))
		{
			QPage=GUIQuestionPage(Controller.TopPage());
			QPage.SetupQuestion(repl(PenaltyWarning, "%cancelpenalty%", GP.MoneyToString(CancelPenalty)), QBTN_YesNo, QBTN_No);
			QPage.OnButtonClick = OnConfirmCancel;
		}
		return false;
	}
	return true;
}

function UpdateDetails(GUIComponent Sender)
{
	lblSelCharName.Caption = clSelChar.GetName();
	if (cbMap1.bChecked) setChallengeInfo(0);
	else if (cbMap2.bChecked) setChallengeInfo(1);
}

function UpdateEnemyTeam(GUIComponent Sender)
{
	local array<string> Roster;
	local class<UT2K4TeamRoster> ETI;
	if (!GP.GetAltTeamRoster(cbEnemyTeam.GetExtra(), Roster))
	{
		ETI = class<UT2K4TeamRoster>(DynamicLoadObject(cbEnemyTeam.GetExtra(), class'Class'));
		Roster = ETI.default.RosterNames;
	}
	if (!bChallenged)
	{
		Roster.Remove(0, 1);
		clSelChar.PopulateList(Roster);
		clSelChar.SetIndex(Max(clSelChar.Index, 0));
	}
	UpdateDetails(Sender);
}

function bool onSelectChar(GUIComponent Sender)
{
	if ( Sender == btnPrevChar )
	{
		clSelChar.ScrollLeft();
	}
	else if ( Sender == btnNextChar )
	{
		clSelChar.ScrollRight();
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

function bool XButtonClicked( GUIComponent Sender )
{
	Controller.CloseMenu(true);
	return true;
}

DefaultProperties
{
	Begin Object Class=GUIButton Name=SPMbtnOk
		Caption="ACCEPT"
		WinWidth=0.222222
		WinHeight=0.050694
		WinLeft=0.741321
		WinTop=0.912731
		OnClick=onOkClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=1
		MenuState=MSAT_Disabled
		bBoundToParent=true
	End Object
	btnOk=SPMbtnOk

	Begin Object Class=GUIButton Name=SPCbtnCancel
		Caption="REFUSE"
		WinWidth=0.222222
		WinHeight=0.050694
		WinLeft=0.037848
		WinTop=0.912731
		OnClick=onCancelClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=2
		bBoundToParent=true
	End Object
	btnCancel=SPCbtnCancel

	Begin Object Class=GUILabel Name=SPClblTitle
		Caption="Bloodrites"
		WinWidth=0.862501
		WinHeight=0.077500
		WinLeft=0.067813
		WinTop=0.026180
		RenderWeight=0.2
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextALign=TXTA_Center
		bBoundToParent=true
	End Object
	lblTitle=SPClblTitle

	Begin Object class=GUIImage name=SPCimgMapBg1
		WinWidth=0.444444
		WinHeight=0.333333
		WinLeft=0.020833
		WinTop=0.111308
		RenderWeight=0.1
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
		bBoundToParent=true
	End Object
	imgMapBg1=SPCimgMapBg1

	Begin Object class=GUIImage name=SPCimgMapBg2
		WinWidth=0.444444
		WinHeight=0.333333
		WinLeft=0.534722
		WinTop=0.111308
		RenderWeight=0.1
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
		bBoundToParent=true
	End Object
	imgMapBg2=SPCimgMapBg2

	Begin Object class=GUIImage name=SPCimgMap1
		WinWidth=0.441667
		WinHeight=0.312500
		WinLeft=0.022222
		WinTop=0.122531
		RenderWeight=0.15
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
		bBoundToParent=true
	End Object
	imgMap1=SPCimgMap1

	Begin Object class=GUIImage name=SPCimgMap2
		WinWidth=0.441667
		WinHeight=0.312500
		WinLeft=0.536111
		WinTop=0.122531
		RenderWeight=0.15
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
		bBoundToParent=true
	End Object
	imgMap2=SPCimgMap2

	Begin Object class=GUIGFXButton Name=SPCcbMap1
		Caption=""
		WinWidth=0.444444
		WinHeight=0.062500
		WinLeft=0.020833
		WinTop=0.452974
		bCheckBox=true
		OnChange=OnMapSelect
		bBoundToParent=true
	End Object
	cbMap1=SPCcbMap1

	Begin Object class=GUIGFXButton Name=SPCcbMap2
		Caption=""
		WinWidth=0.444444
		WinHeight=0.062500
		WinLeft=0.534722
		WinTop=0.452974
		bCheckBox=true
		OnChange=OnMapSelect
		bBoundToParent=true
	End Object
	cbMap2=SPCcbMap2

	Begin Object class=moComboBox Name=SPCcbEnemyTeam
		Caption="Challenge team:"
		Hint="Challenge this team for a match"
		bAutoSizeCaption=true
		WinWidth=0.696528
		WinHeight=0.053472
		WinLeft=0.280556
		WinTop=0.528114
		bReadOnly=true
		TabOrder=3
		OnChange=UpdateEnemyTeam
		bBoundToParent=true
	End Object
	cbEnemyTeam=SPCcbEnemyTeam

	Begin Object class=moCheckBox name=SPCcbInstaGib
		Caption="InstaGib Game"
		Hint="In this an InstaGib Match"
		bAutoSizeCaption=true
		WinWidth=0.696528
		WinHeight=0.053472
		WinLeft=0.280556
		WinTop=0.586447
		TabOrder=4
		OnChange=UpdateEnemyTeam
		bBoundToParent=true
	End Object
	cbInstaGib=SPCcbInstaGib

	Begin Object Class=GUIScrollTextBox Name=SPCsbDetails
		WinWidth=0.689583
		WinHeight=0.243611
		WinLeft=0.282292
		WinTop=0.657976
		RenderWeight=0.2
		TabOrder=1
		bNoTeletype=true
		StyleName="NoBackground"
		bBoundToParent=true
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
		bBoundToParent=true
    End Object
    lblNoPreview1=NoPreview
    lblNoPreview2=NoPreview

    // Character selection
    Begin Object class=GUILabel name=CGBlblSelChar
        TextAlign=TXTA_Center
        Caption="Team Mate"
        StyleName="TextLabel"
		WinWidth=0.197917
		WinHeight=0.043750
		WinLeft=0.047222
		WinTop=0.530864
		bBoundToParent=true
    End Object
    lblSelChar=CGBlblSelChar

    Begin Object Class=GUIButton Name=CGBbtnPrevChar
		Hint="Select the team mate you want"
		StyleName="ArrowLeft"
		WinWidth=0.048750
		WinHeight=0.073750
		WinLeft=0.042500
		WinTop=0.673333
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Down
		OnClick=onSelectChar
		bBoundToParent=true
	End Object
	btnPrevChar=CGBbtnPrevChar

	Begin Object Class=GUIButton Name=CGBbtnNextChar
		Hint="Select the team mate you want"
		StyleName="ArrowRight"
		WinWidth=0.048750
		WinHeight=0.073750
		WinLeft=0.203437
		WinTop=0.673333
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Up
		OnClick=onSelectChar
		bBoundToParent=true
	End Object
	btnNextChar=CGBbtnNextChar

	Begin Object class=GUICharacterListTeam Name=CGBclSelChar
		Hint="Challenge for this team mate"
		WinWidth=0.118444
		WinHeight=0.268750
		WinLeft=0.088462
		WinTop=0.576250
		bCenterInBounds=true
		FixedItemsPerPage=1
		StyleName="CharButton"
		TabOrder=0
		OnChange=UpdateDetails
		bBoundToParent=true
	End Object
	clSelChar=CGBclSelChar

	Begin Object class=GUILabel name=CGBlblSelCharName
        TextAlign=TXTA_Center
        Caption=""
        StyleName="TextLabel"
		WinWidth=0.222917
		WinHeight=0.043750
		WinLeft=0.037222
		WinTop=0.847530
		bBoundToParent=true
    End Object
    lblSelCharName=CGBlblSelCharName

    Begin Object Class=GUILabel Name=SPLlblBalance
		WinWidth=0.450000
		WinHeight=0.041250
		WinLeft=0.532360
		WinTop=0.042037
		Caption=""
		StyleName="TextLabel"
		TextAlign=TXTA_Right
		RenderWeight=0.25
		bBoundToParent=true
	End Object
	lblBalance=SPLlblBalance

	DefaultLeft=0.05
	DefaultTop=0.05
	DefaultWidth=0.9
	DefaultHeight=0.9

	WinLeft=0.05
	WinTop=0.05
	WinWidth=0.9
	WinHeight=0.9

	OnClose=OnWindowClose
	OnCanClose=OnCanWindowClose

	PenaltyWarning="When you refuse you will have to pay a penalty of %cancelpenalty%.||Are you sure you want to refuse?"
	CantAffordCancel="You can't afford to refuse this challenge.|You have to pay %cancelpenalty% to refuse, but you only have %balance%."
	SelectAMap="Please select an area for the challenge."
	msgChallenge="You have selected to challenge %enemyteam% for their team mate %selectedchar%. In order to challenge %enemyteam% for one of their team members you will have to pay %entryfee%. The %gametype% will take place in %map%."
	msgChallenged="You have been challenged by %enemyteam%. They are challenging you for your team member %selectedchar%. The %gametype% will take place in %map%.|When you refuse you will have to pay them %penalty%"
    msgInstaGib="InstaGib"

	DefaultUrl="?TeamScreen=true"
	ChalGameType="xGame.xTeamGame"
	MutInstaGib="?mutator=xGame.MutInstaGib"
	InstaGoalScore=35
	NormalGoalScore=25
	MapPreFix="DM"
	ChalledFeeMultiply=2
}

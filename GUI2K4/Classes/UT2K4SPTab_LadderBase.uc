//==============================================================================
// Base class for the ladders (Qualify, Team Qualify and Tournament)
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SPTab_LadderBase extends UT2K4SPTab_Base abstract;

#exec OBJ LOAD FILE=UCGeneric.utx

var automated GUISectionBackground sbgMatch, sbgDetail;
var automated GUIImage imgMatchShot;
var automated GUIScrollTextBox sbDetails;
var automated GUILabel lblMatchPrice, lblMatchEntryFee, lblBalance, lblNoMoney;
var automated GUIButton btnChallengeMap;
/** colors used for the entry fee */
var color clEntryMatch, clEntryFail;

var localized string PrizeMoney, EntryFee, NotEnoughPlayers, BalanceLabel;

var CacheManager.MapRecord ActiveMap;

/**
	This window will be displayed before each team match to assign the roles of the team mates
*/
var string TeamRoleWindow;
var UT2K4SP_TeamRoles RoleWindow;
/** The map challenge window */
var string MapChallengeWindow;

var UT2K4LadderButton SelectedMatch;

/** used for button animation */
struct AnimData
{
	var float X;
	var float Y;
};
/** duration of the button animation */
var float AnimTime;

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);
	if (bShow)
	{
		MainWindow.btnPlay.Caption = CaptionPlay;
		btnPlayEnabled(SelectedMatch != none);
		UpdateBalance();
	}
}

/**
	Move the match information components to a new location
*/
function moveMatchInfo(optional float topDelta, optional float leftDelta)
{
	sbgMatch.WinTop += topDelta;
	sbgMatch.WinLeft += leftDelta;
	imgMatchShot.WinTop += topDelta;
	imgMatchShot.WinLeft += leftDelta;
	lblMatchPrice.WinTop += topDelta;
	lblMatchPrice.WinLeft += leftDelta;
	sbgDetail.WinTop += topDelta;
	sbgDetail.WinLeft += leftDelta;
	sbDetails.WinTop += topDelta;
	sbDetails.WinLeft += leftDelta;
	lblMatchEntryFee.WinTop += topDelta;
	lblMatchEntryFee.WinLeft += leftDelta;
	lblNoMoney.WinTop += topDelta;
	lblNoMoney.WinLeft += leftDelta;
	btnChallengeMap.WinTop += topDelta;
	btnChallengeMap.WinLeft += leftDelta;
}

/**
	Create the ladder buttons
	Should be called during InitComponent()
*/
function array<UT2K4LadderButton> CreateVButtons(int ladderId, float LadderTop, float LadderLeft, float LadderHeight, optional bool bHidden)
{
	local int i, ls;
	local float Spacing, CurrentTop;
	local array<UT2K4LadderButton> buttonArray;
	local UT2K4LadderButton btn;
	local GUIImage img;

	if (GP == none)
	{
		Log("Warning Will Robinson, no loaded game profile", LogPrefix);
		return buttonArray;
	}
	ls = GP.LengthOfLadder(LadderId);
	buttonArray.length = ls;

	Spacing = LadderHeight/(ls-1)-class'UT2K4LadderVButton'.default.WinHeight;
	CurrentTop = LadderTop;
	for (i = ls-1; i >= 0; i--)
	{
		btn = new class'UT2K4LadderVButton';
		btn.WinLeft = LadderLeft;
		btn.WinTop = CurrentTop;
		btn.WinHeight = btn.WinHeight;
		btn.WinWidth = btn.WinWidth;
		CurrentTop = btn.WinTop+btn.WinHeight+Spacing;
		btn.OnClick=onMatchClick;
		btn.OnDblClick=onMatchDblClick;
		btn.TabOrder = Controls.length+1;
		btn.bBoundToParent = true;
		buttonArray[i] = btn;
		AppendComponent(btn);
		btn.bVisible = !bHidden;

		if (i > 0)
		{
			img = new class'GUIImage';
			img.WinTop = btn.WinTop+btn.WinHeight;
			img.WinHeight = CurrentTop-img.WinTop;
			img.WinWidth = 0.003906;
			img.WinLeft = btn.WinWidth/2-img.WinWidth+btn.WinLeft;
			img.Image = btn.PBNormal;
			img.ImageStyle=ISTY_Scaled;
			img.bBoundToParent = true;
			btn.ProgresBar = img;
			AppendComponent(img);
			img.bVisible = !bHidden;
		}

		updateButton(Btn, ladderId, i);
	}
	return buttonArray;
}

/**
	Create the ladder buttons, horizontal layout
	Should be called during InitComponent()
*/
function array<UT2K4LadderButton> CreateHButtons(int ladderId, float LadderTop, float LadderLeft, float LadderWidth, optional bool bHidden)
{
	local int i, ls;
	local float Spacing, CurrentLeft;
	local array<UT2K4LadderButton> buttonArray;
	local UT2K4LadderButton btn;
	local GUIImage img;

	if (GP == none)
	{
		Log("Warning Will Robinson, no loaded game profile", LogPrefix);
		return buttonArray;
	}
	ls = GP.LengthOfLadder(LadderId);
	buttonArray.length = ls;

	Spacing = LadderWidth/(ls-1)-class'UT2K4LadderHButton'.default.WinWidth;
	CurrentLeft = LadderLeft;
	for (i = 0; i < ls; i++)
	{
		if (i > 0)
		{
			img = new class'GUIImage';
			img.WinLeft = btn.WinLeft+btn.WinWidth;
			img.WinWidth = CurrentLeft-img.WinLeft;
			img.WinHeight = 0.003906;
			img.WinTop = btn.WinHeight/2-img.WinHeight+btn.WinTop;
			img.Image = btn.PBNormal;
			img.ImageStyle = ISTY_Scaled;
			img.bBoundToParent = true;
			AppendComponent(img);
			img.bVisible = !bHidden;
		}

		btn = new class'UT2K4LadderHButton';
		btn.WinLeft = CurrentLeft;
		btn.WinTop = LadderTop;
		btn.WinHeight = btn.WinHeight;
		btn.WinWidth = btn.WinWidth;
		CurrentLeft = btn.WinLeft+btn.WinWidth+Spacing;
		btn.OnClick = onMatchClick;
		btn.OnDblClick = onMatchDblClick;
		btn.ProgresBar = img;
		btn.TabOrder = Controls.length+1;
		btn.bBoundToParent = true;
		buttonArray[i] = btn;
		AppendComponent(btn);
		btn.bVisible = !bHidden;

		updateButton(Btn, ladderId, i);
	}
	return buttonArray;
}

/**
	Update a single button.
	buttun style is not updated, this takes quite some time, call SetState
	when the button is shown
*/
function updateButton(UT2K4LadderButton btn, int ladderId, int matchId)
{
	if (Btn.MatchInfo == None)
	{
		Btn.MatchInfo = UT2K4MatchInfo(GP.GetMatchInfo(ladderId, matchId));
		Btn.MyMapRecord = class'CacheManager'.static.getMapRecord(Btn.MatchInfo.LevelName);
		Btn.MatchIndex = matchId;
		Btn.LadderIndex = ladderId;
	}
}

function bool onMatchClick(GUIComponent Sender)
{
	local UT2K4LadderButton tmp;

	if ((UT2K4LadderButton(Sender) != none) && (GP != None))
	{
		tmp = UT2K4LadderButton(GP.NextMatchObject);
		if (tmp != None)		// Reset to non active state
			tmp.SetState(GetLadderProgress(tmp.LadderIndex));

		SelectedMatch = UT2K4LadderButton(Sender);

		GP.CurrentLadder = SelectedMatch.LadderIndex;
		GP.CurrentMenuRung = SelectedMatch.MatchIndex;
		GP.NextMatchObject = SelectedMatch;
		showMatchDetails(UT2K4LadderButton(Sender).MatchInfo);
		SelectedMatch.StyleName = "LadderButtonActive";
		SelectedMatch.Style = Controller.GetStyle(SelectedMatch.StyleName,SelectedMatch.FontScale);

		return true;
	}
	Warn(""$(UT2K4LadderButton(Sender) != none)@(GP != None));
	return false;
}

function int GetLadderProgress(int ladderindex)
{
	if (ladderindex >= 10) return GP.CustomLadders[ladderindex-10].progress;
	return GP.LadderProgress[LadderIndex];
}

/**
	Double click -> play match
*/
function bool onMatchDblClick(GUIComponent Sender)
{
	if (UT2K4LadderButton(Sender) != none)
	{
		return onPlayClick();
	}
	return false;
}

/**
	Show the match details
*/
function showMatchDetails(UT2K4MatchInfo mi)
{
	local Material Screenshot;
	local int minentryfee;

	if ( MI == none ) return;

	minentryfee = GP.getMinimalEntryFeeFor(mi);
	if (SelectedMatch != none)
	{
		lblNoMoney.bVisible = (minentryfee > GP.Balance) || (minentryfee < 0);
		if (lblNoMoney.bVisible) lblNoMoney.Caption = getMatchCaption(mi);
		btnPlayEnabled(!lblNoMoney.bVisible);
	}
	else btnPlayEnabled(false);

	ActiveMap = class'CacheManager'.static.getMapRecord(MI.LevelName);
	MainWindow.LastLadderPage = Self; // we have the last active match

	if (ActiveMap.ScreenshotRef != "")
		Screenshot = Material(DynamicLoadObject(ActiveMap.ScreenshotRef, class'Material'));
	if (Screenshot==None) Screenshot = Material'UCGeneric.SolidColours.Black';

	imgMatchShot.Image = Screenshot;

	// Set Scroll Content
	sbDetails.setContent(GetMapDecription(ActiveMap));

	// Set MapName Label
	if ( MI.MenuName != "" )   // replace mapname with menuname if exists
		sbgDetail.Caption = MI.MenuName;
	else
	{
		if (ActiveMap.FriendlyName != "") sbgDetail.Caption = ActiveMap.FriendlyName;
		else sbgDetail.Caption = ActiveMap.MapName;
	}

	if (MI.PrizeMoney != 0)
		lblMatchPrice.Caption = PrizeMoney@GP.MoneyToString(MI.PrizeMoney);
		else lblMatchPrice.Caption = "";

	// show/hide entry fee label
	if (MI.EntryFee != 0)
	{
		lblMatchEntryFee.Caption = EntryFee@GP.MoneyToString(MI.EntryFee);
		if (MI.EntryFee <= GP.Balance)
			lblMatchEntryFee.TextColor = clEntryMatch;
			else lblMatchEntryFee.TextColor = clEntryFail;
	}
	else lblMatchEntryFee.Caption = "";

	// show hide map challenge button
	btnChallengeMap.bVisible = GP.HasAltLevel(GP.CurrentLadder, GP.CurrentMenuRung);
	minentryfee = GP.getMinimalEntryFeeFor(mi, true);
	if (MI.PrizeMoney * GP.MapChallengeCost < GP.Balance-minentryfee) btnChallengeMap.EnableMe();
		else btnChallengeMap.DisableMe();
}

function string getMatchCaption(UT2K4MatchInfo mi)
{
	local int nummates, healthymates, i, j;
	nummates = GP.GetNumTeammatesForMatch();
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
			return repl(repl(NotEnoughPlayers, "%healthy%", healthymates), "%teammates%", nummates);
		}
	}
	return default.lblNoMoney.Caption;
}

/** return the map description */
function string GetMapDecription(CacheManager.MapRecord MR)
{
	local DecoText DT;
	local string Package, Item;
	if ( MR.Description == "" && class'CacheManager'.static.Is2003Content(MR.MapName) && MR.TextName != "" )
	{
		if ( !Divide(MR.TextName, ".", Package, Item) )
		{
			Package = "XMaps";
			Item = MR.TextName;
		}
		DT = class'xUtil'.static.LoadDecoText( Package, Item );
		if (DT == none) return "";
		return JoinArray(DT.Rows, "|");
	}
	return MR.Description;
}

/**
	Play the selected match, first check if we need to assign team mates
*/
function bool onPlayClick()
{
	local int nummates, healthymates, i, j;
	local GUIQuestionPage QPage;

	if ((SelectedMatch != none) && (GP != none))
	{
		if (!MainWindow.HandleRequirements(SelectedMatch.MatchInfo.Requirements)) return true;
		GP.bIsChallenge = false;
		if (SelectedMatch.MatchInfo.EntryFee > GP.Balance) return false;
		nummates = GP.GetNumTeammatesForMatch();
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
					QPage.SetupQuestion(repl(repl(NotEnoughPlayers, "%healthy%", healthymates), "%teammates%", nummates), QBTN_Ok);
				}
				return true;
			}

			if (Controller.OpenMenu(TeamRoleWindow))
			{
				RoleWindow = UT2K4SP_TeamRoles(Controller.TopPage());
				RoleWindow.OnOkClick = StartMatch;
			}
		}
		else StartMatch();
	}
	return true;
}

/**
	Start the selected match
*/
function StartMatch()
{
	local LevelInfo myLevel;
	GP.Balance -= SelectedMatch.MatchInfo.EntryFee;
	GP.ActiveMap = ActiveMap;
	myLevel = PlayerOwner().Level;
	GP.bIsChallenge = false;
	GP.StartNewMatch ( GP.CurrentLadder, myLevel );
	Controller.CloseAll(false,true);
}

/**
	Select next match
	should be overwritten in subclasses
*/
function selectNextMatch();

/** initialize buttons for animation */
function array<AnimData> InitAnimData(array<UT2K4LadderButton> buttons)
{
	local array<AnimData> res;
	local int i;
	res.length = buttons.length;
	for (i = 0; i < buttons.length; i++)
	{
		res[i].X = buttons[i].WinLeft;
		res[i].Y = buttons[i].WinTop;
	}
	DoAnimate(buttons, res, true);
	return res;
}

/** animate the button */
function DoAnimate(array<UT2K4LadderButton> buttons, array<AnimData> data, optional bool reset)
{
	local int i;
	if (reset)
	{
		for (i = 1; i < buttons.length; i++)
		{

			buttons[i].KillAnimation(); // to stop the current animation
			buttons[i].OnArrival = none;
			buttons[i].WinLeft = data[0].X;
			buttons[i].WinTop  = data[0].Y;
			if (buttons[i].ProgresBar != none) buttons[i].ProgresBar.bVisible = false;
		}
		return;
	}
	for (i = 1; i < buttons.length; i++)
	{
		buttons[i].OnArrival = LadderButtonOnArrival;
		if (buttons[i].ProgresBar != none) buttons[i].ProgresBar.bVisible = false;
		buttons[i].Animate(data[i].X, data[i].Y, AnimTime*(float(i)/float(buttons.length)));
	}
}

/** to show the progress bar again */
function LadderButtonOnArrival(GUIComponent Sender, EAnimationType Type)
{
	if (UT2K4LadderButton(Sender) == none) return;
	if (UT2K4LadderButton(Sender).ProgresBar != none) UT2K4LadderButton(Sender).ProgresBar.bVisible = true;
}

function bool OnMapChallenge(GUIComponent Sender)
{
	local bool res;
	res = Controller.OpenMenu(MapChallengeWindow);
	if (res)
	{
		UT2K4SP_MapChallenge(Controller.ActivePage).MapSelectionUpdate = MapChallengeUpdate;
	}
	return res;
}

function MapChallengeUpdate()
{
	if (SelectedMatch != none)
	{
		SelectedMatch.MatchInfo = UT2K4MatchInfo(GP.GetMatchInfo(SelectedMatch.ladderIndex, SelectedMatch.matchIndex));
		SelectedMatch.MyMapRecord = class'CacheManager'.static.getMapRecord(SelectedMatch.MatchInfo.LevelName);
		SelectedMatch.SetState(GP.LadderProgress[SelectedMatch.LadderIndex]);
		showMatchDetails(SelectedMatch.MatchInfo);
		UpdateBalance();
	}
}

/** update the balance label */
function UpdateBalance()
{
	lblBalance.Caption = BalanceLabel@GP.MoneyToString(GP.Balance);
}

defaultproperties
{
	Begin Object class=AltSectionBackground Name=SPPsbgMatch
		WinWidth=0.470000
		WinHeight=0.376633
		WinLeft=0.515000
		WinTop=0.074329
		Caption="Selected match"
		bBoundToParent=true
    End Object
    sbgMatch=SPPsbgMatch

	Begin Object class=GUIImage Name=SPLimgMatchShot
		WinWidth=0.443266
		WinHeight=0.286939
		WinLeft=0.529005
		WinTop=0.128434
		Image=material'UCGeneric.SolidColours.Black'
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
		RenderWeight=0.2
		bBoundToParent=true
	End Object
	imgMatchShot=SPLimgMatchShot

	Begin Object Class=GUILabel Name=SPLlblMatchPrice
		WinWidth=0.450000
		WinHeight=0.041250
		WinLeft=0.524999
		WinTop=0.374077
		TextAlign=TXTA_Center
		RenderWeight=0.25
		//StyleName="TextLabel"
		TextColor=(R=207,G=185,B=103,A=255)
		ShadowOffsetX=1
		ShadowOffsetY=1
		bBoundToParent=true
	End Object
	lblMatchPrice=SPLlblMatchPrice

	Begin Object Class=GUILabel Name=SPLlblMatchEntryFee
		WinWidth=0.450000
		WinHeight=0.041250
		WinLeft=0.524999
		WinTop=0.340417
		TextAlign=TXTA_Center
		RenderWeight=0.25
		//StyleName="TextLabel"
		TextColor=(R=185,G=207,B=103,A=255)
		ShadowOffsetX=1
		ShadowOffsetY=1
		bBoundToParent=true
	End Object
	lblMatchEntryFee=SPLlblMatchEntryFee

	Begin Object Class=GUILabel Name=SPLlblNoMoney
		WinWidth=0.442347
		WinHeight=0.208418
		WinLeft=0.530101
		WinTop=0.128946
		TextAlign=TXTA_Center
		RenderWeight=0.25
		TextColor=(R=255,G=96,B=96,A=255)
		ShadowOffsetX=1
		ShadowOffsetY=1
		bBoundToParent=true
		Caption="You do not have enough money to enter this match."
		bMultiLine=true
		bVisible=false
	End Object
	lblNoMoney=SPLlblNoMoney

	Begin Object class=AltSectionBackground Name=SPPsbgDetail
		WinWidth=0.470000
		WinHeight=0.450000
		WinLeft=0.515000
		WinTop=0.473463
		Caption="Details"
		bBoundToParent=true
    End Object
    sbgDetail=SPPsbgDetail

	Begin Object Class=GUIScrollTextBox Name=SPPsbDetails
		WinWidth=0.439796
		WinHeight=0.333750
		WinLeft=0.531378
		WinTop=0.532179
		CharDelay=0.0025
		EOLDelay=0.5
		RenderWeight=0.2
		TabOrder=1
		StyleName="NoBackground"
		bBoundToParent=true
	End Object
	sbDetails=SPPsbDetails

	Begin Object Class=GUILabel Name=SPLlblBalance
		WinWidth=0.450000
		WinHeight=0.041250
		WinLeft=0.546249
		WinTop=0.005000
		Caption=""
		StyleName="TextLabel"
		//FontScale=FNS_Large
		TextAlign=TXTA_Right
		RenderWeight=0.25
		ShadowOffsetX=1.5
		ShadowOffsetY=1.5
		ShadowColor=(R=0,G=0,B=0,A=64)
		bBoundToParent=true
	End Object
	lblBalance=SPLlblBalance

	Begin Object Class=GUIButton Name=SPLbtnChallengeMap
		WinWidth=0.371556
		WinHeight=0.047628
		WinLeft=0.565433
		WinTop=0.868232
		Caption="CHANGE ARENA"
		Hint="Select an alternative arena for this match"
		TabOrder=2
		RenderWeight=0.3
		bVisible=false
		OnClick=OnMapChallenge
		bBoundToParent=true
	End Object
	btnChallengeMap=SPLbtnChallengeMap

	TeamRoleWindow="GUI2K4.UT2K4SP_TeamRoles"
	PropagateVisibility=false
	PrizeMoney="Prize money:"
	EntryFee="Entry fee:"
	BalanceLabel="Balance:"
	clEntryMatch=(R=185,G=207,B=103,A=255)
	clEntryFail=(R=255,G=96,B=96,A=255)
	NotEnoughPlayers="You do not have enough healthy team mates for this match.|%teammates% healthy team mates are required, you only have %healthy%."
	AnimTime=1
	MapChallengeWindow="GUI2K4.UT2K4SP_MapChallenge"
}

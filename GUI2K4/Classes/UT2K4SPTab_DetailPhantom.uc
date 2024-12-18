//==============================================================================
// Overview of phantom matches played
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SPTab_DetailPhantom extends UT2K4SPTab_Base;

var automated MultiSelectListBox lbMatches;
var automated GUISectionBackground sbgMatchesBg;

var localized string VS, msgMatchInfo, msgGameTime, msgScore;

var string TeamDetailPage;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
	local UT2K4MatchInfo MI;
	Super.Initcomponent(MyController, MyOwner);
	//lbMatches.List.OnDrawItem = DrawMatchItem;
	lbMatches.List.TextAlign = TXTA_Center;
	lbMatches.List.OnClickSound = CS_None;
	lbMatches.List.bHotTrack = true;
	lbMatches.List.OnTrack = OnPhantomTrack;
	lbMatches.List.OnClick = none;
	for (i = 0; i < GP.PhantomMatches.Length; i++)
	{
		lbMatches.List.Add(getTeamName(GP.TeamStats[GP.PhantomMatches[i].Team1].Name)@VS@getTeamName(GP.TeamStats[GP.PhantomMatches[i].Team2].Name));
		MI = UT2K4MatchInfo(GP.getMatchInfo(GP.PhantomMatches[i].LadderId, GP.PhantomMatches[i].MatchId));
		lbMatches.List.Add(repl(repl(msgMatchInfo, "%gametype%", GP.GetLadderDescription(GP.PhantomMatches[i].LadderId, GP.PhantomMatches[i].MatchId)), "%map%", MI.LevelName)); // TODO: match description
		lbMatches.List.Add(repl(repl(msgScore, "%team1%", int(round(GP.PhantomMatches[i].ScoreTeam1))), "%team2%", int(round(GP.PhantomMatches[i].ScoreTeam2))));
		lbMatches.List.Add(repl(msgGameTime, "%gametime%", (GP.PhantomMatches[i].GameTime/60)));
		lbMatches.List.Add(" ");
	}
}

function string getTeamName(string TeamClass)
{
	local class<UT2K4TeamRoster> ETI;
	ETI = class<UT2K4TeamRoster>(DynamicLoadObject(TeamClass, class'Class'));
	if (ETI == none)
	{
		Warn(TeamClass@"is not a valid subclass of UT2K4TeamRoster");
		return "????";
	}
	return ETI.default.TeamName;
}

function cmMatchesOnClick(GUIContextMenu Sender, int ClickIndex)
{
	local int tindex;
	if (ClickIndex == 0)
	{
		if (Controller.MouseX > lbMatches.ActualLeft()+(lbMatches.ActualWidth() / 2)) tindex = GP.PhantomMatches[lbMatches.List.Index / 5].Team2;
			else tindex = GP.PhantomMatches[lbMatches.List.Index / 5].Team1;
		Controller.OpenMenu(TeamDetailPage, GP.TeamStats[tindex].Name);
	}
}

function OnPhantomTrack(GUIComponent Sender, int LastIndex)
{
	local int rindex, i, j;
	rindex = lbMatches.list.Index / 5;
	for (j = 0; j < (MultiSelectList(lbMatches.list).MElements.length / 5); j++)
	{
		for (i = 0; (i < 4) && ((j*5+i) < MultiSelectList(lbMatches.list).MElements.length); i++) // select per 4 lines
		{
			MultiSelectList(lbMatches.list).MElements[j*5+i].bSelected = (rindex == j);
		}
	}
}

defaultproperties
{
	Begin Object class=GUIContextMenu Name=SPDPcmMatches
		ContextItems(0)="Team details"
		OnSelect=cmMatchesOnClick
	End Object

	Begin Object Class=MultiSelectListBox Name=SPDPlbMatches
		WinWidth=0.764337
		WinHeight=0.686097
		WinLeft=0.117832
		WinTop=0.179697
		bVisibleWhenEmpty=True
		RenderWeight=0.2
		FontScale=FNS_Medium
		OnClickSound=CS_None
		bBoundToParent=true
		StyleName="NoBackground"
	End Object
	lbMatches=SPDPlbMatches
	ContextMenu=SPDPcmMatches

	Begin Object class=GUISectionBackground Name=SPDPimgMatchesBg
		WinWidth=0.8
		WinHeight=0.8
		WinLeft=0.1
		WinTop=0.1
		bBoundToParent=true
		Caption="Other Tournament Matches Played During Your Match"
	End Object
	sbgMatchesBg=SPDPimgMatchesBg

	TeamDetailPage="GUI2K4.UT2K4SP_DetailsTeam"

	PanelCaption="Other Tournament Matches"
	VS="vs"
	msgMatchInfo="%gametype% in %map%"
	msgGameTime="Game time: %gametime% minutes"
	msgScore="Score: %team1% - %team2%"
}

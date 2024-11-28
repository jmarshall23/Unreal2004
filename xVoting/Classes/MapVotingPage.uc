//====================================================================
//  xVoting.MapVotingPage
//  Map Voting page.
//
//  Written by Bruce Bickar
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class MapVotingPage extends VotingPage;

var automated MapVoteMultiColumnListBox      lb_MapListBox;
var automated MapVoteCountMultiColumnListBox lb_VoteCountListBox;
var automated moComboBox                     co_GameType;
var automated GUILabel                       l_Mode;
var automated GUIImage                       i_MapListBackground, i_MapCountListBackground;

// Localization
var localized string lmsgMapVotingDisabled, lmsgReplicationNotFinished, lmsgMapDisabled,
                     lmsgTotalMaps, lmsgMode[8];
//------------------------------------------------------------------------------------------------
function InternalOnOpen()
{
	local int i, d;

    if( MVRI == none || (MVRI != none && !MVRI.bMapVote) )
    {
		Controller.OpenMenu("GUI2K4.GUI2K4QuestionPage");
		GUIQuestionPage(Controller.TopPage()).SetupQuestion(lmsgMapVotingDisabled, QBTN_Ok, QBTN_Ok);
		GUIQuestionPage(Controller.TopPage()).OnButtonClick = OnOkButtonClick;
		return;
    }

	// check if all maps and gametypes have replicated
    if( MVRI.GameConfig.Length < MVRI.GameConfigCount || MVRI.MapList.Length < MVRI.MapCount )
    {
		Controller.OpenMenu("GUI2K4.GUI2K4QuestionPage");
		GUIQuestionPage(Controller.TopPage()).SetupQuestion(lmsgReplicationNotFinished, QBTN_Ok, QBTN_Ok);
		GUIQuestionPage(Controller.TopPage()).OnButtonClick = OnOkButtonClick;
		return;
    }

    for( i=0; i<MVRI.GameConfig.Length; i++ )
    	co_GameType.AddItem( MVRI.GameConfig[i].GameName, none, string(i));
    co_GameType.MyComboBox.List.SortList();
    d = co_GameType.MyComboBox.List.FindExtra(string(MVRI.CurrentGameConfig));
    if( d > -1 )
	   	co_GameType.SetIndex(d);

	l_Mode.Caption = lmsgMode[MVRI.Mode];

   	lb_MapListBox.LoadList(MVRI);
   	MapVoteCountMultiColumnList(lb_VoteCountListBox.List).LoadList(MVRI);

    lb_VoteCountListBox.List.OnDblClick = MapListDblClick;
    lb_VoteCountListBox.List.bDropTarget = True;

    lb_MapListBox.List.OnDblClick = MapListDblClick;
    lb_MaplistBox.List.bDropSource = True;

    co_GameType.OnChange = GameTypeChanged;
    f_Chat.OnSubmit = Submit;
}
//------------------------------------------------------------------------------------------------
function Submit()
{
    SendVote(none);
}
//------------------------------------------------------------------------------------------------
function GameTypeChanged(GUIComponent Sender)
{
	local int GameTypeIndex;

	GameTypeIndex = int(co_GameType.GetExtra());
	if( GameTypeIndex > -1 )
	{
		lb_MapListBox.ChangeGameType( GameTypeIndex );
	    lb_MapListBox.List.OnDblClick = MapListDblClick;
	}
}
//------------------------------------------------------------------------------------------------
function OnOkButtonClick(byte bButton) // triggered by th GUIQuestionPage Ok Button
{
	Controller.CloseMenu(true);
}
//------------------------------------------------------------------------------------------------
function UpdateMapVoteCount(int UpdatedIndex, bool bRemoved)
{
	MapVoteCountMultiColumnList(lb_VoteCountListBox.List).UpdatedVoteCount(UpdatedIndex, bRemoved);
}
//------------------------------------------------------------------------------------------------
function bool MapListDblClick(GUIComponent Sender)
{
    SendVote(Sender);
    return true;
}
//------------------------------------------------------------------------------------------------
function SendVote(GUIComponent Sender)
{
    local int MapIndex,GameConfigIndex;

	if( Sender == lb_VoteCountListBox.List )
	{
		MapIndex = MapVoteCountMultiColumnList(lb_VoteCountListBox.List).GetSelectedMapIndex();
		if( MapIndex > -1)
	    {
		    GameConfigIndex = MapVoteCountMultiColumnList(lb_VoteCountListBox.List).GetSelectedGameConfigIndex();
		    if(MVRI.MapList[MapIndex].bEnabled || PlayerOwner().PlayerReplicationInfo.bAdmin)
		        MVRI.SendMapVote(MapIndex,GameConfigIndex);
		    else
				PlayerOwner().ClientMessage(lmsgMapDisabled);
		}
	}
	else
	{
    	MapIndex = MapVoteMultiColumnList(lb_MapListBox.List).GetSelectedMapIndex();
		if( MapIndex > -1)
	    {
		    GameConfigIndex = int(co_GameType.GetExtra());
		    if(MVRI.MapList[MapIndex].bEnabled || PlayerOwner().PlayerReplicationInfo.bAdmin)
		        MVRI.SendMapVote(MapIndex,GameConfigIndex);
		    else
				PlayerOwner().ClientMessage(lmsgMapDisabled);
		}
    }
}
//------------------------------------------------------------------------------------------------
defaultproperties
{
   	Begin Object Class=GUIImage Name=MapCountListBackground
		WinWidth=0.762535
		WinHeight=0.190957
		WinLeft=0.195849
		WinTop=0.177930
		Image=Material'2K4Menus.NewControls.NewFooter'
		ImageStyle=ISTY_Stretched
		bScaleToParent=True
	End Object
	i_MapCountListBackground=MapCountListBackground

    Begin Object Class=MapVoteCountMultiColumnListBox Name=VoteCountListBox
		WinWidth=0.756285
		WinHeight=0.223770
		WinLeft=0.197412
		WinTop=0.148763
        bVisibleWhenEmpty=true
        FontScale=FNS_Medium
        //StyleName="ServerBrowserGrid"
        bScaleToParent=True
    End Object
    lb_VoteCountListBox = VoteCountListBox

    Begin Object class=moComboBox Name=GameTypeCombo
		WinWidth=0.757809
		WinHeight=0.037500
		WinLeft=0.199219
		WinTop=0.334309
		Caption="Filter Game Type:"
        StyleName="SquareButton"
        CaptionWidth=0.35
		bScaleToParent=True
    End Object
    co_GameType = GameTypeCombo

   	Begin Object Class=GUIImage Name=MapListBackground
		WinWidth=0.768394
		WinHeight=0.279042
		WinLeft=0.193407
		WinTop=0.402270
		Image=Material'2K4Menus.NewControls.NewFooter'
		ImageStyle=ISTY_Stretched
		bScaleToParent=True
	End Object
	i_MapListBackground=MapListBackground

    Begin Object Class=MapVoteMultiColumnListBox Name=MapListBox
		WinWidth=0.765269
		WinHeight=0.316542
		WinLeft=0.194970
		WinTop=0.371020
        bVisibleWhenEmpty=true
        //StyleName="ServerBrowserGrid"
        bScaleToParent=True
    End Object
    lb_MapListBox = MapListBox

    Begin Object class=GUILabel Name=ModeLabel
        Caption=""
        TextALign=TXTA_Left
        TextFont="UT2SmallFont"
        TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.761363
		WinHeight=0.037245
		WinLeft=0.130517
		WinTop=0.621020
		bScaleToParent=True
    End Object
    l_Mode = ModeLabel

    OnOpen=InternalOnOpen;

	lmsgMapVotingDisabled="Sorry, Map Voting has been disabled by the server administrator."
	lmsgReplicationNotFinished="Map data download in progress. Please try again later."
	lmsgMapDisabled="The selected Map is disabled."
	lmsgTotalMaps="%mapcount% Total Maps"
	lmsgMode(0)="Majority Mode"
	lmsgMode(1)="Majority & Elimination Mode"
	lmsgMode(2)="Score Mode"
	lmsgMode(3)="Score & Elimination Mode"
	lmsgMode(4)="Majority & Accumulation Mode"
	lmsgMode(5)="Majority & Accumulation & Elimination Mode"
	lmsgMode(6)="Score & Accumulation Mode"
	lmsgMode(7)="Score & Accumulation & Elimination Mode"
	WindowName="Map Voting"
}


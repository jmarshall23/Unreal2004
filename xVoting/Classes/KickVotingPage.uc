//====================================================================
//  xVoting.KickVotingPage
//  Kick Voting page.
//
//  Written by Bruce Bickar
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class KickVotingPage extends VotingPage;

var automated KickVoteMultiColumnListBox lb_PlayerListBox;
var automated GUILabel         l_PlayerListTitle;
var automated GUIImage         i_PlayerListBackground;

// Localization
var localized string lmsgKickVotingDisabled;
//------------------------------------------------------------------------------------------------
function InternalOnOpen()
{
    if( MVRI == none || (MVRI != none && !MVRI.bKickVote) )
    {
		Controller.OpenMenu("GUI2K4.GUI2K4QuestionPage");
		GUIQuestionPage(Controller.TopPage()).SetupQuestion(lmsgKickVotingDisabled, QBTN_Ok, QBTN_Ok);
		GUIQuestionPage(Controller.TopPage()).OnButtonClick = OnOkButtonClick;
		return;
    }
    lb_PlayerListBox.List.OnDblClick = PlayerListDblClick;
    KickVoteMultiColumnList(lb_PlayerListBox.List).LoadPlayerList(MVRI);
    f_Chat.OnSubmit = SendKickVote;
}
//------------------------------------------------------------------------------------------------
function OnOkButtonClick(byte bButton) // triggered by th GUIQuestionPage Ok Button
{
	Controller.CloseAll(true,true);
}
//------------------------------------------------------------------------------------------------
function UpdateKickVoteCount(VotingHandler.KickVoteScore KVCData)
{
	KickVoteMultiColumnList(lb_PlayerListBox.List).UpdatedVoteCount(KVCData.PlayerID, KVCData.KickVoteCount);
}
//------------------------------------------------------------------------------------------------
function bool PlayerListDblClick(GUIComponent Sender)
{
	SendKickVote();
    return true;
}
//------------------------------------------------------------------------------------------------
function SendKickVote()
{
    local int PlayerID;

    PlayerID = KickVoteMultiColumnList(lb_PlayerListBox.List).GetSelectedPlayerID();
    if( PlayerID > -1 )
        MVRI.SendKickVote(PlayerID);
}
//------------------------------------------------------------------------------------------------
defaultproperties
{
    OnOpen=InternalOnOpen;

   	Begin Object Class=GUIImage Name=PlayerListBackground
		WinWidth=0.593787
		WinHeight=0.562833
		WinLeft=0.253662
		WinTop=0.192513
		Image=Material'2K4Menus.NewControls.NewFooter'
		ImageStyle=ISTY_Stretched
		bScaleToParent=True
	End Object
	i_PlayerListBackground=PlayerListBackground

    Begin Object Class=KickVoteMultiColumnListBox Name=PlayerListBoxControl
		WinWidth=0.473047
		WinHeight=0.481758
		WinLeft=0.254141
		WinTop=0.162239
        bVisibleWhenEmpty=true
        StyleName="ServerBrowserGrid"
    End Object
    lb_PlayerListBox = PlayerListBoxControl

	lmsgKickVotingDisabled="Sorry, Kick Voting has been disabled by the server administrator."
	WindowName="Kick Voting"
}


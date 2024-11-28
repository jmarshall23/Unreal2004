//==============================================================================
//	Created on: 08/29/2003
//	This panel contains in-game voice chat controls, such as banning controls, etc.
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
// TODO: Allow players to restrict speech based on type (taunt, order, etc.)
//==============================================================================
class UT2K4Tab_MidGameVoiceChat extends MidGamePanel;

struct ChatItem
{
	var int PlayerID;
	var bool bNoText;
	var bool bNoSpeech;
	var bool bNoVoice;
	var bool bBanned;
	var bool bDirty;
};

var array<ChatItem> ChatList;

var automated GUISectionBackground sb_Players, sb_Specs, sb_Options;
var automated GUIListBox lb_Players, lb_Specs;
var automated GUIList    li_Players, li_Specs;
var automated moCheckbox ch_NoVoiceChat, ch_NoSpeech, ch_NoText, ch_Ban;
var automated GUIButton  b_Reset;

var() int SelectIndex;
var() localized string ApplySuccessText, ApplyFailText;

var() editconst bool bTeamGame;

var	int RedTeamIndex, BlueTeamIndex;
var localized string RedTeamDesc, BlueTeamDesc;

function GameReplicationInfo GRI()
{
	return PlayerOwner().GameReplicationInfo;
}

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);

	// Tie lists together with buttons
	li_Players 	= lb_Players.List;
	li_Specs 	= lb_Specs.List;

	li_Players.bInitializeList 	= false;
	li_Specs.bInitializeList 	= false;

	sb_Players.ManageComponent( lb_Players );
	sb_Specs.ManageComponent( lb_Specs );

	sb_Options.ManageComponent( ch_NoText );
	sb_Options.ManageComponent( ch_NoSpeech );
	sb_Options.ManageComponent( ch_NoVoiceChat );
	sb_Options.ManageComponent( ch_Ban );

	AssociateButtons();
}

function ShowPanel( bool bShow )
{
	Super.ShowPanel(bShow);
	if ( !bShow )
	{
		ClearIndexes(None);
		SaveRestrictions();
	}
}

event Closed( GUIComponent Sender, bool bCancelled )
{
	SaveRestrictions();
	Super.Closed(Sender,bCancelled);
}

// =====================================================================================================================
// =====================================================================================================================
//  GUI Interface
// =====================================================================================================================
// =====================================================================================================================

function bool PreDraw(Canvas Canvas)
{
	if ( GRI() != None )
	{
		bTeamGame = GRI().bTeamGame;
		FillPlayerLists();
	}

	return false;
}

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_Reset )
	{
		ResetRestrictions();
		ChatList.Remove(0,ChatList.Length);
	}

	UpdateButtonStates();
	return true;
}

// Called when an player's name is clicked on
function ListChange( GUIComponent Sender )
{
	local int ID;
	local GUIList List;

	List = GUIListBox(Sender).List;
	if ( List == None )
		return;

	ClearIndexes( List );

	// grab the associated PlayerID from the selected player
	id = int(List.GetExtra());
	if ( PlayerIDIsMine(id) )
	{
		SelectedSelf();
		return;
	}
	SelectIndex = FindChatListIndex(id);
	LoadRestrictions(SelectIndex);
}

function InternalOnChange(GUIComponent Sender)
{
	local bool bResult;

	if ( !ValidIndex(SelectIndex) )
		return;

	bResult = moCheckbox(Sender).IsChecked();
	ChatList[SelectIndex].bDirty = true;
	switch (Sender)
	{
		case ch_NoText:
			ChatList[SelectIndex].bNoText = bResult;
			break;

		case ch_NoSpeech:
			ChatList[SelectIndex].bNoSpeech = bResult;
			break;

		case ch_NoVoiceChat:
			ChatList[SelectIndex].bNoVoice = bResult;
			break;

		case ch_Ban:
			ChatList[SelectIndex].bBanned = bResult;
			break;
	}

	if ( ChatList[SelectIndex].bDirty && ApplyRestriction(SelectIndex) )
	{
		ModifiedChatRestriction(Self, ChatList[SelectIndex].PlayerID);
		ChatList[SelectIndex].bDirty = False;
	}

	UpdateButtonStates();
}

// =====================================================================================================================
// =====================================================================================================================
//  Implementation
// =====================================================================================================================
// =====================================================================================================================

// Get a list of all players, and put them into the appropriate list
function FillPlayerLists()
{
	local string idx;

	if ( GRI() == None )
		return;

	// if an item was selected, remember the selected PlayerID while we clear and refill the lists
	if ( li_Players.IsValid() )
		idx = li_Players.GetExtra();
	else if ( li_Specs.IsValid() )
		idx = "";

	// Disable all list notification while we're clearing and refilling the lists
	li_Players.bNotify = false;
	li_Specs.bNotify = false;

	ClearLists();
	PopulateLists( GRI() );

	// if we had a PlayerID selected, attempt to reselect that PlayerID in any list
	if ( idx != "" )
	{
		if ( li_Players.Find(idx,false,true) != "" )
		{
			if ( !PlayerIDIsMine(idx) )
				li_Players.CheckLinkedObjects(li_Players);
		}
		else if ( li_Specs.Find(idx,false,true) != "" )
		{
			if ( !PlayerIDIsMine(idx) )
			li_Specs.CheckLinkedObjects(li_Specs);
		}
	}

	li_Players.bNotify = true;
	li_Specs.bNotify = true;
}

function PopulateLists(GameReplicationInfo GRI)
{
	local int i;
	local PlayerReplicationInfo PRI;

	if (bTeamGame)
	{
		li_Players.Add(RedTeamDesc,,,true);
		li_Players.Add(BlueTeamDesc,,,True);
		BlueTeamIndex=1;
	}

	for ( i = 0; i < GRI.PRIArray.Length; i++ )
	{
		PRI = GRI.PRIArray[i];
		if ( PRI == None || PRI.bBot || xPlayerReplicationInfo(PRI) == none || (bTeamGame && PRI.Team==None && !PRI.bOnlySpectator) )
			continue;

		// If this is the first time seeing this playerid, request the ban/ignore info from our ChatManager
		if ( FindChatListIndex(PRI.PlayerID) == -1 )
			AddPlayerInfo(PRI.PlayerID);

		if ( PRI.bOnlySpectator )
			li_Specs.Add(PRI.PlayerName,,string(PRI.PlayerID));
		else
			AddtoPlayers(PRI);
	}
}

function AddToPlayers(PlayerReplicationInfo PRI)
{
	if (bTeamGame )
	{
		if ( PRI.Team.TeamIndex==0 )	// Red Team
		{
			if ( RedTeamIndex == -1 )
			{
				li_Players.Add(RedTeamDesc,,,true);
				RedTeamIndex = 0;
			}

			if ( BlueTeamIndex < 0 )
				li_Players.Add(PRI.PlayerName,,string(PRI.PlayerID));
			else
			{
				li_Players.Insert(BlueTeamIndex,PRI.PlayerName,,string(PRI.PlayerID));
				BlueTeamIndex++;
			}
		}
		else
		{
			if ( BlueTeamIndex == -1 )
			{
				BlueTeamIndex = li_Players.ItemCount;
				li_Players.Add(BlueTeamDesc,,,true);
			}

			li_Players.Add(PRI.PlayerName,,string(PRI.PlayerID));
		}
	}
	else
		li_Players.Add( PRI.PlayerName,,string(PRI.PlayerID) );
}

// When a list item is selected, clear the indexes of the other lists
function ClearIndexes( GUIList List )
{
	if ( List != li_Players)
		li_Players.SilentSetIndex(-1);

	if ( List != li_Specs )
		li_Specs.SilentSetIndex(-1);

	if ( List == None )
		SelectedSelf();
}

// =====================================================================================================================
// =====================================================================================================================
//  Chat Manager Interface
// =====================================================================================================================
// =====================================================================================================================

// Set the checkboxes to the values associated with the given player
function LoadRestrictions(int i)
{
	if ( !ValidIndex(i) )
	{
		ch_NoText.SetComponentValue(False, True);
		ch_NoSpeech.SetComponentValue(False, True);
		ch_NoVoiceChat.SetComponentValue(False, True);
		ch_Ban.SetComponentValue(False, True);
	}
	else
	{
		ch_NoText.SetComponentValue(ChatList[i].bNoText, True);
		ch_NoSpeech.SetComponentValue(ChatList[i].bNoSpeech, True);
		ch_NoVoiceChat.SetComponentValue(ChatList[i].bNoVoice, True);
		ch_Ban.SetComponentValue(ChatList[i].bBanned, True);
	}

	UpdateButtonStates();
}

// Called when a new playerID is found while filling the lists
// Request stored chat restrictions for this PlayerID from the ChatManager
function int AddPlayerInfo(int PlayerID)
{
	local int i;
	local PlayerController PC;
	local byte Restriction;

	PC = PlayerOwner();
	if ( PC.ChatManager == None )
		return -1;

	// Verify that we don't already have this player ID in the list
	i = FindChatListIndex( PlayerID );
	if ( i == -1 )
	{
		i = ChatList.Length;
		ChatList.Length = i+1;
	}

	ChatList[i].PlayerID = PlayerID;

	// Query the ChatManager for the settings for this player
	Restriction = PC.ChatManager.GetPlayerRestriction(PlayerID);
	UnpackRestriction(i, Restriction);
	return i;
}

function PackRestriction(int i, out byte Restriction)
{
	Restriction = 0;
	if ( ValidIndex(i) )
	{
		if ( ChatList[i].bNoText )
			Restriction = Restriction | 1;
		if ( ChatList[i].bNoSpeech )
			Restriction = Restriction | 2;
		if ( ChatList[i].bNoVoice )
			Restriction = Restriction | 4;
		if ( ChatList[i].bBanned )
			Restriction = Restriction | 8;
	}
}

function UnpackRestriction(int i, byte Restriction)
{
	if ( ValidIndex(i) )
	{
		ChatList[i].bNoText = bool(Restriction & 1);
		ChatList[i].bNoSpeech = bool(Restriction & 2);
		ChatList[i].bNoVoice = bool(Restriction & 4);
		ChatList[i].bBanned = bool(Restriction & 8);
		Chatlist[i].bDirty = false;
	}
}

function bool ApplyRestriction(int i)
{
	local byte Restriction;

	// Send restrictions to player's chat manager
	if ( ValidIndex(i) )
	{
		PackRestriction(i, Restriction);
		return PlayerOwner().ChatBan(ChatList[i].PlayerID, Restriction);
	}

	return false;
}

function SaveRestrictions()
{
	local int i;

	for ( i = 0; i < ChatList.Length; i++ )
	{
		if ( Chatlist[i].bDirty && ApplyRestriction(i) )
		{
			ModifiedChatRestriction(Self, Chatlist[i].PlayerID);
			Chatlist[i].bDirty = false;
		}
	}
}

function ResetRestrictions()
{
	local int i;
	local byte Restriction;

	for ( i = 0; i < ChatList.Length; i++ )
	{
		PackRestriction(i, Restriction);
		if ( Restriction != 0 )
			PlayerOwner().ChatBan(ChatList[i].PlayerID, 0);
	}

	PlayerOwner().ChatManager.ClearConfig();
}

function UpdateChatRestriction( int PlayerID )
{
	local int i;

	Super.UpdateChatRestriction(PlayerID);

	i = FindChatListIndex(PlayerID);
	if ( ValidIndex(i) )
		UnpackRestriction( i, PlayerOwner().ChatManager.GetPlayerRestriction(PlayerID) );

	UpdateButtonStates();
}

// =====================================================================================================================
// =====================================================================================================================
//  Utility / Helper functions
// =====================================================================================================================
// =====================================================================================================================

function UpdateButtonStates()
{
	local int i;
	local PlayerController PC;

	PC = PlayerOwner();

	for ( i = 0; i < ChatList.Length; i++ )
	{
		if ( Chatlist[i].bDirty )
		{
			EnableComponent(b_Reset);
			return;
		}
	}

	if ( PC != None && PC.ChatManager != None && PC.ChatManager.Count() > 0 )
	{
		EnableComponent(b_Reset);
		return;
	}

	DisableComponent(b_Reset);
}

function ClearLists()
{
	if ( li_Players.ItemCount > 0 )
		li_Players.Clear();

	if ( li_Specs.ItemCount > 0 )
		li_Specs.Clear();
}

// Disable/enable these components based on whether or not a list has a valid index
function AssociateButtons()
{
	LinkList(li_Players);
	LinkList(li_Specs);
}

function LinkList( GUIList List )
{
	if ( List == None )
		return;

	List.AddLinkObject( ch_NoVoiceChat );
	List.AddLinkObject( ch_NoSpeech );
	List.AddLinkObject( ch_NoText );
	List.AddLinkObject( ch_Ban );
}

function int FindChatListIndex(int PlayerID)
{
	local int i;

	for ( i = 0; i < ChatList.Length; i++ )
		if ( PlayerID == ChatList[i].PlayerID )
			return i;

	return -1;
}

function bool ValidIndex(int i)
{
	return i >= 0 && i < ChatList.Length;
}

function SelectedSelf()
{
	ch_NoText.SetComponentValue(False, True);
	ch_NoSpeech.SetComponentValue(False, True);
	ch_NoVoiceChat.SetComponentValue(False, True);
	ch_Ban.SetComponentValue(False, True);

	DisableComponent(ch_NoText);
	DisableComponent(ch_NoSpeech);
	DisableComponent(ch_NoVoiceChat);
	DisableComponent(ch_Ban);
}

// =====================================================================================================================
// =====================================================================================================================
//  Defaults
// =====================================================================================================================
// =====================================================================================================================

DefaultProperties
{
	Begin Object Class=AltSectionBackground Name=PlayersBackground
        Caption="Players"
        LeftPadding=0.00000
        RightPadding=0.00000
        TopPadding=0.00000
        BottomPadding=0.00000
		WinWidth=0.462019
		WinHeight=0.899506
		WinLeft=0.019250
		WinTop=0.030325
        bBoundToParent=True
        bScaleToParent=True
	End Object
	sb_Players=PlayersBackground

	Begin Object Class=AltSectionBackground Name=SpecBackground
		Caption="Spectators"
        LeftPadding=0.00000
        RightPadding=0.00000
        TopPadding=0.00000
        BottomPadding=0.00000
		WinWidth=0.462019
		WinHeight=0.468385
		WinLeft=0.512544
		WinTop=0.030325
        bBoundToParent=True
        bScaleToParent=True
	End Object
	sb_Specs=SpecBackground

	Begin Object Class=AltSectionBackground Name=OptionBackground
		Caption="Voice Options"
		WinWidth=0.462019
		WinHeight=0.394391
		WinLeft=0.512544
		WinTop=0.508063
		TopPadding=0.04
		BottomPadding=0.0
        bBoundToParent=True
        bScaleToParent=True
	End Object
	sb_Options=OptionBackground

	Begin Object Class=GUIListBox Name=PlayersList
		WinWidth=0.431250
		WinHeight=0.518750
		WinLeft=0.056250
		WinTop=0.041667
		OnChange=ListChange
		bInitializeList=False
		TabOrder=0
		StyleName="NoBackground"
	End Object
	lb_Players=PlayersList

	Begin Object Class=GUIListBox Name=SpecList
		WinWidth=0.431250
		WinHeight=0.518750
		WinLeft=0.531250
		WinTop=0.041667
		OnChange=ListChange
		bInitializeList=False
		TabOrder=1
		StyleName="NoBackground"
	End Object
	lb_Specs=SpecList

	Begin Object Class=moCheckBox Name=NoText
		WinWidth=0.338524
		WinHeight=0.049840
		WinLeft=0.647884
		WinTop=0.620670
		Caption="Ignore Text"
		Hint="Do not receive any text messages from this player"
		OnChange=InternalOnChange
		TabOrder=2
		MenuState=MSAT_Disabled
	End Object
	ch_NoText=NoText

	Begin Object Class=moCheckBox Name=NoSpeech
		WinWidth=0.338524
		WinHeight=0.049840
		WinLeft=0.647884
		WinTop=0.685424
		Caption="Ignore Speech"
		Hint="Do not receive any speech messages, such as \"Incoming!\" from this player"
		OnChange=InternalOnChange
		TabOrder=3
		MenuState=MSAT_Disabled
	End Object
	ch_NoSpeech=NoSpeech

	Begin Object Class=moCheckBox Name=NoVoiceChat
		WinWidth=0.338524
		WinHeight=0.049840
		WinLeft=0.647884
		WinTop=0.750178
		Caption="Ignore Voice Chat"
		Hint="Do not receive any voice chat messages from this player, in any voice chat room"
		OnChange=InternalOnChange
		TabOrder=4
		MenuState=MSAT_Disabled
	End Object
	ch_NoVoiceChat=NoVoiceChat

	Begin Object Class=moCheckBox Name=BanPlayer
		WinWidth=0.338524
		WinHeight=0.049840
		WinLeft=0.647884
		WinTop=0.814932
		Caption="Ban Player"
		Hint="Ban this player from your personal voice chat channel"
		OnChange=InternalOnChange
		TabOrder=5
		MenuState=MSAT_Disabled
	End Object
	ch_Ban=BanPlayer

	Begin Object Class=GUIButton Name=ResetButton
		WinWidth=0.120067
		WinHeight=0.051836
		WinLeft=0.682745
		WinTop=0.899668
		Caption="Reset"
		Hint="Reset & reload all player chat restrictions"
		OnClick=InternalOnClick
		TabOrder=7
		bStandardized=true
		MenuState=MSAT_Disabled
	End Object
	b_Reset=ResetButton

	OnPreDraw=PreDraw
	ApplySuccessText="Changes were saved successfully!"
	ApplyFailText="Changes could not be saved!"

	RedTeamDesc="RED TEAM"
	BlueTeamDesc="BLUE TEAM"

	RedTeamIndex=-1
	BlueTeamIndex=-1
}


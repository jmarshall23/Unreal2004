//==============================================================================
//	Login & Midgame menu - appears when player joins a server for the first time in a while and also on Esc press
//				Has tabs for seeing server rules & map rotation, team scores, and game controls
//
//	Created by Matt Oelfke
//	(c) 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4PlayerLoginMenu extends FloatingWindow;

var(MidGame) array<GUITabItem> Panels;
var(MidGame) GUITabItem        SPRulesPanel;
var(MidGame) GUITabItem        IARulesPanel;

var(MidGame) automated GUITabControl c_Main;

function InitComponent(GUIController MyController, GUIComponent MyComponent)
{
	local PlayerController PC;

	Super.InitComponent(MyController, MyComponent);

	// rjp -- when playing SinglePlayer or InstantAction game, remove tabs which only apply to multiplayer
	PC = PlayerOwner();
	if ( PC != None && PC.Level.NetMode == NM_StandAlone )
		RemoveMultiplayerTabs(PC.Level.Game);
	// -- rjp

	if ( Panels.Length > 0 )
		AddPanels();

	SetTitle();
	T_WindowTitle.DockedTabs = c_Main;
}

function bool FloatingPreDraw( Canvas C )
{
	if (PlayerOwner().GameReplicationInfo!=None)
		SetVisibility(true);
	else
		SetVisibility(false);

	return false;
}


function InternalOnClose(optional Bool bCanceled)
{
	local PlayerController PC;

	PC = PlayerOwner();

	// Turn pause off if currently paused
	if(PC != None && PC.Level.Pauser != None)
		PC.SetPause(false);

	Super.OnClose(bCanceled);
}

function AddPanels()
{
	local int i;
	local MidGamePanel Panel;

	for ( i = 0; i < Panels.Length; i++ )
	{
		Panel = MidGamePanel(c_Main.AddTabItem(Panels[i]));
		if ( Panel != None )
			Panel.ModifiedChatRestriction = UpdateChatRestriction;
	}
}

// Called via delegate by a MidGamePanel when a player chat restriction has been updated
// this notifies all other panels about the change
function UpdateChatRestriction( MidGamePanel Sender, int PlayerID )
{
	local int i;

	if ( Sender == None )
		return;

	for ( i = 0; i < c_Main.TabStack.Length; i++ )
	{
		if ( c_Main.TabStack[i] != None && MidGamePanel(c_Main.TabStack[i].MyPanel) != None &&
			c_Main.TabStack[i].MyPanel != Sender )
			MidGamePanel(c_Main.TabStack[i].MyPanel).UpdateChatRestriction(PlayerID);
	}
}

function SetTitle()
{
	local PlayerController PC;

	PC = PlayerOwner();
	if ( PC.Level.NetMode == NM_StandAlone || PC.GameReplicationInfo == None || PC.GameReplicationInfo.ServerName == "" )
		WindowName = PC.Level.GetURLMap();
	else WindowName = PC.GameReplicationInfo.ServerName;

	t_WindowTitle.SetCaption(WindowName);
}

function RemoveMultiplayerTabs(GameInfo Game)
{

	if (Game.CurrentGameProfile != none)
		Panels[2] = SPRulesPanel; //there's no map rotation in a single player tournament

	Panels.Remove(3,1);
	Panels.Remove(1,1);
}

event bool NotifyLevelChange()
{
	bPersistent = false;
	LevelChanged();
	return true;
}

defaultproperties
{
    Begin Object Class=GUITabControl Name=LoginMenuTC
		WinWidth=0.974999
		WinHeight=0.044644
		WinLeft=0.012500
		WinTop=0.060215
        TabHeight=0.0375
        bAcceptsInput=true
        bDockPanels=true
        bScaleToParent=true
        bFillSpace=True
        BackgroundStyleName="TabBackground"
    End Object
    c_Main=LoginMenuTC

	SPRulesPanel=(ClassName="GUI2K4.UT2K4Tab_ServerInfo",Caption="Rules",Hint="Game settings")
	Panels(0)=(ClassName="GUI2K4.UT2K4Tab_PlayerLoginControls",Caption="Game",Hint="Game Controls")
	Panels(1)=(ClassName="GUI2K4.UT2K4Tab_ServerMOTD",Caption="MOTD",Hint="Message of the Day")
	Panels(2)=(ClassName="GUI2K4.UT2K4Tab_MidGameRulesCombo",Caption="Server Info",Hint="Current map rotation and game settings")
	Panels(3)=(ClassName="GUI2K4.UT2K4Tab_MidGameVoiceChat",Caption="Communication",Hint="Manage communication with other players")
	Panels(4)=(ClassName="GUI2K4.UT2K4Tab_MidGameHelp",Caption="Help",Hint="Helpful hints")

	OnClose=InternalOnClose

	bRenderWorld=true
    bRequire640x480=true
    bAllowedAsLast=true
/*
    DefaultLeft=0.0
    DefaultTop=0.0
    DefaultWidth=1.0
    DefaultHeight=1.0
    WinLeft=0.0
    WinTop=0.0
    WinWidth=1.0
    WinHeight=1.0
*/
	DefaultWidth=0.779688
	DefaultHeight=0.847083
	DefaultLeft=0.110313
	DefaultTop=0.057916
	WinWidth=0.779688
	WinHeight=0.847083
	WinLeft=0.110313
	WinTop=0.057916

	bResizeWidthAllowed=False
	bResizeHeightAllowed=False
	bMoveAllowed=False
	bPersistent=True
}

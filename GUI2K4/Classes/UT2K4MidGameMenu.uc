//-----------------------------------------------------------
//
//-----------------------------------------------------------

class UT2K4MidGameMenu extends UT2K4GUIPage;

#EXEC OBJ LOAD FILE=2K4Menus.utx

var bool bIgnoreEsc;

var     localized string LeaveMPButtonText;
var     localized string LeaveSPButtonText;
var     localized string LeaveEntryButtonText;

var bool bPerButtonSizes;
var GUIButton SizingButton;
var Automated GUIImage MyHeader;
var Automated GUIButton bContinue, bQuit, bForfit, bSettings,
						bChangeTeam, bAdd2Favorites, bServerBrowser,
						bVoting, bMapVoting, bKickVoting, bMatchSetup;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local PlayerController PC;

	Super.InitComponent(MyController, MyOwner);
	PC = PlayerOwner();

	if (PC.GameReplicationInfo == None || !PC.GameReplicationInfo.bTeamGame || PC.GameReplicationInfo.bNoTeamChanges )
		RemoveComponent(bChangeTeam);

//	if (PC.Level.NetMode != NM_Client || CurrentServerIsInFavorites())
//		RemoveComponent(bAdd2Favorites);

	// Set 'leave' button text depending on if we are SP or MP
	if( PC.Level.NetMode != NM_StandAlone )
		bForfit.Caption =  LeaveMPButtonText;
	else
		bForfit.Caption =  LeaveSPButtonText;

	// disable voting in single player mode
    if( PC.Level.NetMode == NM_StandAlone )
	{
		RemoveComponent(bVoting);
		RemoveComponent(bMapVoting);
		RemoveComponent(bKickVoting);
		RemoveComponent(bMatchSetup);
	}

	GetSizingButton();
}

function GetSizingButton()
{
	local int i;

	for (i = 0; i < Components.Length; i++)
	{
		if (GUIButton(Components[i]) != None)
		{
			if (SizingButton == None || Len(GUIButton(Components[i]).Caption) > Len(SizingButton.Caption))
				SizingButton = GUIButton(Components[i]);
		}
	}
}

function bool InternalOnPreDraw(Canvas Canvas)
{
	local int i, X, Y;
    local float XL,YL;

    SizingButton.Style.TextSize(Canvas, SizingButton.MenuState, SizingButton.Caption, XL, YL, SizingButton.FontScale);

	XL += 16;
	YL += 8;

	bQuit.WinWidth = XL;
	bQuit.WinLeft = Canvas.ClipX - bQuit.WinWidth;
	bQuit.WinTop = 0;
	bQuit.WinHeight = YL;

	for (i = 0; i < Components.Length; i++)
	{
		if (GUIButton(Components[i]) != None && Components[i] != bQuit)
		{
			if (bPerButtonSizes)
			{
				Components[i].Style.TextSize(Canvas, Components[i].MenuState, GUIButton(Components[i]).Caption, XL, YL, Components[i].FontScale);
				YL += 8;
			}

			if (X + XL > bQuit.WinLeft)
			{
				X = 0;
				Y += YL;
			}

			Components[i].WinTop = Y;
			Components[i].WinLeft = X;

			Components[i].WinWidth = XL;
			Components[i].WinHeight = YL;

			X += XL;
		}
	}

	MyHeader.WinHeight = Y + YL + 2;
	return false;
}


function bool InternalOnKeyEvent(out byte Key, out byte State, float delta)
{
	// Swallow first escape key event (key up from key down that opened menu)
	if(bIgnoreEsc && Key == 0x1B)
	{
		bIgnoreEsc = false;
		return true;
	}

	return false;
}

/*
// See if we already have this server in our favorites
function bool CurrentServerIsInFavorites()
{
	local string address, ipString, portString;
	local int colonPos, portNum, i;

	// Get current network address
	address = PlayerOwner().GetServerNetworkAddress();

	if(address == "")
		return true; // slightly hacky - dont want to add "none"!

	// Parse text to find IP and possibly port number
	colonPos = InStr(address, ":");
	if(colonPos < 0)
	{
		// No colon - assume port 7777
		ipString = address;
		portNum = 7777;
	}
	else
	{	// Parse out port number
		ipString = Left(address, colonPos);
		portString = Mid(address, colonPos+1);
		portNum = int(portString);
	}

	for(i=0; i<class'UT2K4Browser_ServerListPageFavorites'.default.Favorites.Length; i++ )
	{
		if(	class'UT2K4Browser_ServerListPageFavorites'.default.Favorites[i].IP == ipString &&
			class'UT2K4Browser_ServerListPageFavorites'.default.Favorites[i].Port == portNum )
			return true;
	}

	return false;
}
*/
function InternalOnClose(optional Bool bCanceled)
{
	local PlayerController pc;

	pc = PlayerOwner();

	// Turn pause off if currently paused
	if(pc != None && pc.Level.Pauser != None)
		pc.SetPause(false);

	Super.OnClose(bCanceled);
}

function bool InternalOnClick(GUIComponent Sender)
{

	if(Sender==bQuit) // QUIT
	{
		Controller.OpenMenu(Controller.GetQuitPage());
	}
	else if(Sender==bForfit) // LEAVE/DISCONNECT
	{
		PlayerOwner().ConsoleCommand( "DISCONNECT" );
	    if ( PlayerOwner().Level.Game.CurrentGameProfile != None )
		{
			PlayerOwner().Level.Game.CurrentGameProfile.ContinueSinglePlayerGame(PlayerOwner().Level, true);  // replace menu
		}
		else
			Controller.CloseMenu();
	}
	else if(Sender==bContinue) // CONTINUE
	{
		Controller.CloseMenu(); // Close _all_ menus
	}
	else if(Sender==bSettings) // SETTINGS
	{
		Controller.OpenMenu(Controller.GetSettingsPage());
	}
	else if(Sender==bChangeTeam) // CHANGE TEAM
	{
        PlayerOwner().SwitchTeam();
		Controller.CloseMenu();
	}
	else if(Sender==bAdd2Favorites) // ADD FAVORITE
	{
		PlayerOwner().ConsoleCommand( "ADDCURRENTTOFAVORITES" );
		Controller.CloseMenu();
	}
	else if(Sender==bServerBrowser) // SERVER BROWSER
	{
		Controller.OpenMenu(Controller.GetServerBrowserPage());
	}
	else if(Sender==bVoting) // VOTING
	{
		// if drop down menu is not visible then make it visible otherwise hide it
        if(bMapVoting.bVisible == false)
		{
			bMapVoting.bVisible = true;
			bMapVoting.WinLeft = bVoting.WinLeft;
			bMapVoting.WinWidth = bVoting.WinWidth;
			bMapVoting.WinTop = bVoting.WinTop + bVoting.WinHeight;
			bMapVoting.WinHeight = bVoting.WinHeight;

			bKickVoting.bVisible = true;
			bKickVoting.WinLeft = bVoting.WinLeft;
			bKickVoting.WinWidth = bVoting.WinWidth;
			bKickVoting.WinTop = bVoting.WinTop + bVoting.WinHeight * 2;
			bKickVoting.WinHeight = bVoting.WinHeight;

			bMatchSetup.bVisible = true;
			bMatchSetup.WinLeft = bVoting.WinLeft;
			bMatchSetup.WinWidth = bVoting.WinWidth;
			bMatchSetup.WinTop = bVoting.WinTop + bVoting.WinHeight * 3;
			bMatchSetup.WinHeight = bVoting.WinHeight;
		}
		else
		{
			bMapVoting.bVisible = false;
			bKickVoting.bVisible = false;
			bMatchSetup.bVisible = false;
		}
	}
	else if(Sender==bMapVoting)
	{
		Controller.OpenMenu(Controller.MapVotingMenu);
	}
	else if(Sender==bKickVoting)
	{
		Controller.OpenMenu(Controller.KickVotingMenu);
	}
	else if(Sender==bMatchSetup)
	{
		Controller.OpenMenu(Controller.MatchSetupMenu);
	}

	return true;
}

function InternalOnMouseRelease(GUIComponent Sender)
{
	if (Sender == Self)
		Controller.CloseMenu();
}

DefaultProperties
{
	Begin Object class=GUIImage name=MGHeader
		WinWidth=1
		WinHeight=64
		WinLeft=0
		WinTop=0
		Image=Material'2K4Menus.Controls.header'
		ImageStyle=ISTY_Stretched
        ImageRenderStyle=MSTY_Normal
		bVisible=true
	End Object
	MyHeader=MGHeader

	Begin Object Class=GUIButton Name=ContMatchButton
		Caption="CONTINUE"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=0
	End Object
    bContinue=ContMatchButton

	Begin Object Class=GUIButton Name=QuitGameButton
		Caption="EXIT UT2004"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=8
	End Object
	bQuit=QuitGameButton

	Begin Object Class=GUIButton Name=LeaveMatchButton
		Caption=""
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=1
	End Object
	bForfit=LeaveMatchButton

	Begin Object Class=GUIButton Name=SettingsButton
		Caption="SETTINGS"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=2
	End Object
	bSettings=SettingsButton

	Begin Object Class=GUIButton Name=ChangeTeamButton
		Caption="CHANGE TEAM"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=3
	End Object
	bChangeTeam=ChangeTeamButton

	Begin Object Class=GUIButton Name=AddFavoriteButton
		Caption="ADD FAVORITE"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=4
	End Object
	bAdd2Favorites=AddFavoriteButton

	Begin Object Class=GUIButton Name=BrowserButton
		Caption="SERVER BROWSER"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=5
	End Object
	bServerBrowser=BrowserButton

	Begin Object Class=GUIButton Name=VotingButton
		Caption="Voting . . . "
		StyleName="SquareButton"
		OnClick=InternalOnClick
		TabOrder=6
	End Object
	bVoting=VotingButton

	Begin Object Class=GUIButton Name=MapVotingButton
		Caption="Map Voting"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		bVisible=False
		bTabStop=False
		RenderWeight=0.6
	End Object
	bMapVoting=MapVotingButton

	Begin Object Class=GUIButton Name=KickVotingButton
		Caption="Kick Voting"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		bVisible=False
		bTabStop=False
		RenderWeight=0.6
	End Object
	bKickVoting=KickVotingButton

	Begin Object Class=GUIButton Name=MatchSetupButton
		Caption="Match Setup"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		bVisible=False
		bTabStop=False
		RenderWeight=0.6
	End Object
	bMatchSetup=MatchSetupButton

	OnKeyEvent=InternalOnKeyEvent
	OnClose=InternalOnClose
    OnPreDraw=InternalOnPreDraw

 	WinWidth=1.0
	WinHeight=1.0
	WinTop=0.0
	WinLeft=0.0

	bIgnoreEsc=true
	bAllowedAsLast=true

	LeaveMPButtonText="DISCONNECT"
	LeaveSPButtonText="FORFEIT"
	LeaveEntryButtonText="SERVER BROWSER"

	OnMouseRelease=InternalOnMouseRelease

}
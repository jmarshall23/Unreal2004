// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class ProInstantAction extends UT2K3GUIPage
	Config(ProUI);

#exec OBJ LOAD FILE=InterfaceContent.utx

var Pro_InstantActionMain 		pMain;
var Pro_PlayerSettings			pPlayerSetup;

var GUITabControl				MyTabs;

var localized string			MainTabLabel,
								MainTabHint,
								PlayerTabLabel,
								PlayerTabHint;


var config string GameRules;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);

	MyTabs = GUITabControl(Controls[1]);
	GUITitleBar(Controls[0]).DockedTabs = MyTabs;

	pMain 		 = Pro_InstantActionMain(MyTabs.AddTab(MainTabLabel,"proui.Pro_InstantActionMain",,MainTabHint,true) );
	pPlayerSetup = Pro_PlayerSettings(MyTabs.AddTab(PlayerTabLabel,"proui.Pro_PlayerSettings",,PlayerTabHint));
	MyTabs.bDockPanels=true;

}

function bool BackButtonClick(GUIComponent Sender)
{
	pPlayerSetup.InternalApply(none);
	Controller.CloseMenu(true);
	return true;
}

function bool PlayButtonClick(GUIComponent Sender)
{
	local string FullURL, GameURL, FirstMap, GameType;

	pPlayerSetup.InternalApply(none);

	GameURL = pMain.Play() $ GameRules;

	FirstMap = pMain.MyMapList.List.Get();
	GameType = "?Game="$pMain.GetGameClass();

	FullURL = FirstMap$GameType$GameURL;


	log("["$FirstMap$"] ["$GameType$"]"@FullURL);

	log("UT2InstantActionPage::PlayButtonClick - Sending [open"@FUllURL$"] to the console");
	Console(Controller.Master.Console).DelayedConsoleCommand("start"@FullURL);
	Controller.CloseAll(false);
	return true;
}


function TabChange(GUIComponent Sender)
{
	if (GUITabButton(Sender)==none)
		return;

	GUITitleBar(Controls[0]).Caption = GUITitleBar(default.Controls[0]).Caption@"|"@GUITabButton(Sender).Caption;
}

event ChangeHint(string NewHint)
{
	GUITitleBar(Controls[2]).Caption = NewHint;
}


function InternalOnClose(optional Bool bCanceled)
{
	// Destroy spinning player model actor
	if(pPlayerSetup.SpinnyDude != None)
		pPlayerSetup.SpinnyDude.Destroy();

	Super.OnClose(bCanceled);
}

defaultproperties
{
	Begin Object class=GUITitleBar name=IAPageHeader
		Caption="Instant Action"
		StyleName="Header"
		WinWidth=1
		WinHeight=46.000000
		WinLeft=0
		WinTop=0.036406
		Effect=material'CO_Final'
	End Object
	Controls(0)=GUITitleBar'IAPageHeader'

	Begin Object Class=GUITabControl Name=IAPageTabs
		WinWidth=1.0
		WinLeft=0
		WinTop=0.25
		WinHeight=48
		TabHeight=0.04
		OnChange=TabChange;
		bAcceptsInput=true
	End Object
	Controls(1)=GUITabControl'IAPageTabs'

	Begin Object class=GUITitleBar name=IAPageFooter
		WinWidth=0.76
		WinHeight=0.055
		WinLeft=0.12
		WinTop=0.93
		bUseTextHeight=false
		StyleName="Footer"
		Justification=TXTA_Center
	End Object
	Controls(2)=GUITitleBar'IAPageFooter'

	Begin Object Class=GUIButton Name=IAPagePlayButton
		Caption="PLAY"
		Hint="Start a Match With These Settings"
		OnClick=PlayButtonClick
		StyleName="SquareMenuButton"
		WinWidth=0.12
		WinHeight=0.055
		WinLeft=0.88
		WinTop=0.93
	End Object
	Controls(3)=GUIButton'IAPagePlayButton'

	Begin Object Class=GUIButton Name=IAPageBackButton
		Caption="BACK"
		Hint="Return to Previous Menu"
		OnClick=BackButtonClick
		StyleName="SquareMenuButton"
		WinWidth=0.12
		WinHeight=0.055
		WinLeft=0
		WinTop=0.93
	End Object
	Controls(4)=GUIButton'IAPageBackButton'
	OnClose=InternalOnClose

	Background=Material'InterfaceContent.Backgrounds.bg09'
	WinWidth=1.0
	WinHeight=1.0
	WinTop=0.0
	WinLeft=0.0

	MainTabLabel="Select Map"
	PlayerTabLabel="Player"

	MainTabHint="Choose the starting map and game type to play..."
	PlayerTabHint="Configure your UT2003 Avatar..."
}

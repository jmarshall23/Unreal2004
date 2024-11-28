// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class ProServerBrowser extends UT2K3GUIPage;


#exec OBJ LOAD FILE=InterfaceContent.utx

var Browser_Page LANPage;

var bool bCreatedQueryTabs;
var bool bCreatedStandardTabs;

// Filtering options
var() config bool bOnlyShowStandard;
var() config bool bOnlyShowNonPassword;
var() config bool bDontShowFull;
var() config bool bDontShowEmpty;
var() config bool bDontShowWithBots;
var() config string DesiredMutator;
var() config string DesiredMutator2;
var() config string CustomQuery;
var() config int	MinGamespeed, MaxGamespeed;

var() config enum EViewMutatorMode
{
	VMM_AnyMutators,
	VMM_NoMutators,
	VMM_ThisMutator,
	VMM_NotThisMutator
} ViewMutatorMode, ViewMutator2Mode;

var() config enum EStatsServerView
{
	SSV_Any,
    SSV_OnlyStatsEnabled,
    SSV_NoStatsEnabled,
} StatsServerView;

var() config enum EWeaponStayServerView
{
	WSSV_Any,
    WSSV_OnlyWeaponStay,
    WSSV_NoWeaponStay,
} WeaponStayServerView;

var() config enum ETranslocServerView
{
	TSV_Any,
    TSV_OnlyTransloc,
    TSV_NoTransloc,
} TranslocServerView;


function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);
	GUITitleBar(Controls[0]).DockedTabs = GUITabControl(Controls[1]);

	// delegates
	OnClose = InternalOnClose;
    LANPage.Browser = None;
    GUITabControl(Controls[1]).AddTab(LANPage.PageCaption,"",LANPage);

	GUIButton(GUIPanel(LANPage.Controls[1]).Controls[3]).bVisible = false;
	GUIButton(GUIPanel(LANPage.Controls[1]).Controls[4]).bVisible = false;
}

function InternalOnClose(optional Bool bCanceled)
{
	LANPage.OnCloseBrowser();
	Super.OnClose(bCanceled);
}

defaultproperties
{
	Begin Object Class=Browser_ServerListPageLAN Name=ProLANPage
		PageCaption="LAN"
	End Object
	LANPage=ProLANPage

	Begin Object class=GUITitleBar name=ProServerBrowserHeader
		Caption="Server Browser"
		StyleName="Header"
		WinWidth=1
		WinHeight=46.000000
		WinLeft=0
		WinTop=0.036406
	End Object
	Controls(0)=GUITitleBar'ProServerBrowserHeader'

	Begin Object Class=GUITabControl Name=ProServerBrowserTabs
		WinWidth=1.0
		WinLeft=0
		WinTop=0.25
		WinHeight=48
		TabHeight=0.04
		bFillSpace=False
		bAcceptsInput=true
		bDockPanels=true
	End Object
	Controls(1)=GUITabControl'ProServerBrowserTabs'

	Background=Material'InterfaceContent.Backgrounds.bg10'
	WinWidth=1.0
	WinHeight=1.0
	WinTop=0.0
	WinLeft=0.0
	bCheckResolution=true
	bCreatedQueryTabs=False
	bPersistent=true

	//Filtering
	StatsServerView=SSV_Any
	ViewMutatorMode=VMM_AnyMutators
	WeaponStayServerView=WSSV_Any
	TranslocServerView=TSV_Any
	bOnlyShowStandard=false
	bOnlyShowNonPassword=false
	bDontShowFull=false
	bDontShowEmpty=false
	bDontShowWithBots=false;
	DesiredMutator=""
	CustomQuery=""
	MinGamespeed=0
	MaxGamespeed=200
}

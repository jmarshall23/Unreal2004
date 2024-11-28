//==============================================================================
// Tournament details
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4SP_Details extends UT2K4MainPage config;

var automated GUIButton	btnClose, btnExport;
var automated moCheckBox cbAlwaysShow;

var localized string PageCaption, ProfileExported;

/** profile exporter to use */
var globalconfig string ProfileExporter;

var UT2K4GameProfile GP;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local UT2K4SPTab_DetailEnemies detab;
	Super.Initcomponent(MyController, MyOwner);
	GP = UT2K4GameProfile(PlayerOwner().Level.Game.CurrentGameProfile);
	t_Header.SetCaption(PageCaption);
	c_Tabs.AddTab(PanelCaption[0], PanelClass[0], , PanelHint[0], true); // last match
	if (GP.PhantomMatches.length > 0) c_Tabs.AddTab(PanelCaption[2], PanelClass[2], , PanelHint[2], false); // phantom
	detab = UT2K4SPTab_DetailEnemies(c_Tabs.AddTab(PanelCaption[1], PanelClass[1], , PanelHint[1], false)); // enemies
	if (detab.TeamStats.length == 0) c_Tabs.RemoveTab(, detab.MyButton);

	cbAlwaysShow.Checked(GP.bShowDetails);
}

function bool InternalOnPreDraw(Canvas Canvas)
{
	local float XL,YL;
	btnExport.Style.TextSize(Canvas, btnExport.MenuState, btnExport.Caption, XL, YL, btnExport.FontScale);

	// Automatically size the buttons based on the size of their captions
	btnClose.WinWidth = XL+32;
	btnClose.WinLeft = Canvas.ClipX-btnClose.WinWidth;
	btnExport.WinWidth = XL+32;
	btnExport.WinLeft = btnClose.WinLeft-btnExport.WinWidth;
	return false;
}

function bool btnCloseOnClick(GUIComponent Sender)
{
	return Controller.CloseMenu(true);
}

/**	Clicked a TabButton */
function InternalOnChange(GUIComponent Sender)
{
	if (GUITabButton(Sender)==none)	return;
	t_Header.SetCaption(PageCaption@"|"@GUITabButton(Sender).Caption);
}

function cbAlwaysShowOnChange(GUIComponent Sender)
{
	GP.bShowDetails = cbAlwaysShow.IsChecked();
	return;
}

function bool btnExportOnClick(GUIComponent Sender)
{
	local GUIQuestionPage QPage;
	local class<SPProfileExporter> expclass;
	local SPProfileExporter exporter;

	expclass = class<SPProfileExporter>(DynamicLoadObject(ProfileExporter, class'Class'));
	if (expclass == none)
	{
		Warn("Invalid profile exporter:"@ProfileExporter);
		return true;
	}

	exporter = new expclass;
	exporter.Create(GP, PlayerOwner().Level);
	exporter.ExportProfile();
	if (Controller.OpenMenu(Controller.QuestionMenuClass))
	{
		QPage=GUIQuestionPage(Controller.TopPage());
		QPage.SetupQuestion(QPage.Replace(ProfileExported, "%filename%", exporter.ResultFile), QBTN_Ok);
	}
	return true;
}

function OnDetailClose(optional bool bCanceled)
{
	local UT2K4SP_Main main;
	main = UT2K4SP_Main(Controller.FindMenuByClass(class'UT2K4SP_Main'));
//	main = UT2K4SP_Main(Controller.MenuStack[Controller.MenuStack.length-2]);
	if (main != none)
	{
		if (main.c_Tabs.PendingTab == none) main.c_Tabs.ActiveTab.MyPanel.ShowPanel(true);
	}
}

defaultproperties
{
	OnPreDraw=InternalOnPreDraw
	OnClose=OnDetailClose

	Begin Object class=GUIHeader name=SPDhdrHeader
		RenderWeight=0.3
	End Object

	Begin Object class=ButtonFooter name=SPDftrFooter
		RenderWeight=0.3
	End Object

	Begin Object Class=GUIButton Name=SPDbtnClose
		Caption="CLOSE"
		Hint="Return to the ladder"
		StyleName="FooterButton"
		WinWidth=0.12
		WinHeight=0.040703
		WinTop=0.959479
		WinLeft=0.0
		OnClick=btnCloseOnClick
		TabOrder=0
		RenderWeight=1.0
		bBoundToParent=True
	End Object
	btnClose=SPDbtnClose

	Begin Object Class=GUIButton Name=SPDbtnExport
		Caption="EXPORT"
		Hint="Export the details to a file"
		StyleName="FooterButton"
		WinWidth=0.12
		WinHeight=0.040703
		WinTop=0.959479
		WinLeft=0.88
		OnClick=btnExportOnClick
		TabOrder=1
		RenderWeight=1.0
		bBoundToParent=True
	End Object
	btnExport=SPDbtnExport

	Begin Object class=moCheckBox Name=SPDcbAlwaysShow
		Caption="Show after a match"
		WinWidth=0.289063
		WinHeight=0.030000
		WinLeft=0.006250
		WinTop=0.966146
		TabOrder=2
		RenderWeight=1.0
		bAutoSizeCaption=true
		OnChange=cbAlwaysShowOnChange
	End Object
	cbAlwaysShow=SPDcbAlwaysShow

	t_Header=SPDhdrHeader
	t_Footer=SPDftrFooter

	bPersistent=False
	bDrawFocusedLast=false

	WinWidth=1.0
	WinHeight=1.0
	WinTop=0.0
	WinLeft=0.0
	ProfileExporter="GUI2K4.SPProfileExporter"

	PageCaption="Tournament details"
	ProfileExported="Profile details exported to:|%filename%"

	PanelClass(0)="GUI2K4.UT2K4SPTab_DetailMatch"
	PanelCaption(0)="Last Match Played"
	PanelHint(0)="Overview of the last match played"
	PanelClass(1)="GUI2K4.UT2K4SPTab_DetailEnemies"
	PanelCaption(1)="Opponent Teams"
	PanelHint(1)="Overview of the teams you've played against"
	PanelClass(2)="GUI2K4.UT2K4SPTab_DetailPhantom"
	PanelCaption(2)="Other Tournament Matches"
	PanelHint(2)="Overview of matches played by other teams"
}

// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class MyTestPage extends TestPageBase;

#exec OBJ LOAD FILE=InterfaceContent.utx

var Automated GUIHeader TabHeader;
var Automated GUITabControl TabC;
var Automated GUITitleBar TabFooter;
var Automated GUIButton BackButton;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{

	Super.Initcomponent(MyController, MyOwner);

	TabHeader.DockedTabs = TabC;
    TabC.AddTab("Component Test","GUI2K4.MyTestPanelA",,"Test of many non-list components");
    TabC.AddTab("List Tests","GUI2K4.MyTestPanelB",,"Test of list components");
    TabC.AddTab("Splitter","GUI2K4.MyTestPanelC",,"Test of the Splitter component");

}

function TabChange(GUIComponent Sender)
{
	if (GUITabButton(Sender)==none)
		return;

	TabHeader.SetCaption("Testing : "$GUITabButton(Sender).Caption);
}

event ChangeHint(string NewHint)
{
	TabFooter.SetCaption(NewHint);
}


function bool ButtonClicked(GUIComponent Sender)
{
	local CacheManager.MapRecord Record;

	Record = class'CacheManager'.static.getMapRecord("CTF-Maul");
	return true;
}

event bool NotifyLevelChange()
{
	Controller.CloseMenu(true);
	return Super.NotifyLevelChange();
}

defaultproperties
{
	Begin Object class=GUIHeader name=MyHeader
		Caption="Settings"
		StyleName="Header"
		WinWidth=1.000000
		WinHeight=36.000000
		WinLeft=0.000000
		WinTop=0.005414
		Effect=material'CO_Final'
	End Object

	Begin Object Class=GUITabControl Name=MyTabs
		WinWidth=1.0
		WinLeft=0
		WinTop=0.25
		WinHeight=48
		TabHeight=0.04
		OnChange=TabChange;
		bAcceptsInput=true
		bDockPanels=true
	End Object

	Begin Object class=GUITitleBar name=MyFooter
		WinWidth=0.880000
		WinHeight=0.055000
		WinLeft=0.120000
		WinTop=0.942397
		bUseTextHeight=false
		StyleName="Footer"
		Justification=TXTA_Center
	End Object

	Begin Object Class=GUIButton Name=MyBackButton
		Caption="BACK"
		StyleName="SquareMenuButton"
		Hint="Return to Previous Menu"
		WinWidth=0.12
		WinHeight=0.055
		WinLeft=0
		WinTop=0.942397
		OnClick=ButtonClicked
	End Object

	TabHeader=MyHeader
	TabC=MyTabs
	TabFooter=MyFooter
	BackButton=MyBackButton

	Background=Material'InterfaceContent.Backgrounds.bg11'
	WinWidth=1.0
	WinHeight=1.0
	WinTop=0.0
	WinLeft=0.0

}

//==============================================================================
// Page for the additional custom ladder pages
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SPTab_ExtraLadder extends UT2K4SPTab_Base;

var automated GUITabControl tcTabs;
var string CustomLadderTab;

function InitComponent(GUIController pMyController, GUIComponent MyOwner)
{
	Super.Initcomponent(pMyController, MyOwner);
	bInit = true;
}

function ShowPanel(bool bShow)
{
	local int i;
	local UT2K4SPTab_CustomLadder tsl;
	if (bShow && bInit)
	{
		bInit = false;
		for (i = 0; i < GP.UT2K4GameLadder.default.AdditionalLadders.length; i++)
		{
			Log("Custom Ladder:"@string(GP.UT2K4GameLadder.default.AdditionalLadders[i]));
			GP.RegisterCustomLadder(string(GP.UT2K4GameLadder.default.AdditionalLadders[i]));
			tsl = UT2K4SPTab_CustomLadder(tcTabs.AddTab(GP.UT2K4GameLadder.default.AdditionalLadders[i].default.LadderName, CustomLadderTab,,,false));
			tsl.CustomLadderClass = string(GP.UT2K4GameLadder.default.AdditionalLadders[i]);
			tsl.LadderId = 10+i;
			tsl.EntryLabels = GP.UT2K4GameLadder.default.AdditionalLadders[i].default.EntryLabels;
			tsl.InitCustom();
		}
	}
	Super.ShowPanel(bShow);
}

function InternalOnChange(GUIComponent Sender);

function bool onPlayClick()
{
	if (UT2K4SPTab_Base(tcTabs.ActiveTab.MyPanel) != none)
	{
		return UT2K4SPTab_Base(tcTabs.ActiveTab.MyPanel).onPlayClick();
	}
	return false;
}

defaultproperties
{
	Begin Object Class=GUITabControl Name=PageTabs
		WinWidth=1.0
		WinLeft=0
		WinTop=0.0
		WinHeight=0.04
		TabHeight=0.036
		TabOrder=3
		RenderWeight=0.49
		bFillSpace=False
		bAcceptsInput=True
		bDockPanels=True
		OnChange=InternalOnChange
		BackgroundStyleName="TabBackground"
		bBoundToParent=true
    End Object
	tcTabs=PageTabs

	PanelCaption="Additional"
	CustomLadderTab="GUI2K4.UT2K4SPTab_CustomLadder"
}

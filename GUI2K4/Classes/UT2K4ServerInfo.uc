//==============================================================================
//	Created on: 08/15/2003
//	Description
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4ServerInfo extends UT2K4GUIPage;

var automated GUIImage      i_Background;
var automated GUITabControl c_Tabs;
var automated GUITitleBar   t_Header;
var automated GUIFooter     t_Footer;
var automated GUIButton     b_Close;


var array<string>               PanelClass;
var localized array<string>     PanelCaption;
var localized array<string>     PanelHint;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
    Super.InitComponent(MyController, MyOwner);

	if ( PlayerOwner() != None && PlayerOwner().GameReplicationInfo != None )
	    SetTitle();

    c_Tabs.MyFooter = t_Footer;
	for ( i = 0; i < PanelClass.Length && i < PanelCaption.Length && i < PanelHint.Length; i++ )
	    c_Tabs.AddTab(PanelCaption[i],PanelClass[i],,PanelHint[i]);
}

function bool ButtonClicked(GUIComponent Sender)
{
    Controller.CloseMenu(true);
    return true;
}

event ChangeHint(string NewHint)
{
    t_Footer.SetCaption(NewHint);
}

function SetTitle()
{
	t_Header.SetCaption(PlayerOwner().GameReplicationInfo.ServerName);
}

defaultproperties
{
    Begin Object Class=GUIImage name=ServerInfoBackground
        bAcceptsInput=false
        bNeverFocus=true
        Image=Material'InterfaceContent.Menu.BorderBoxD'
        ImageStyle=ISTY_Stretched
        WinWidth=1
        WinLeft=0
        WinHeight=1
        WinTop=0
        bBoundToParent=true
        bScaleToParent=true
    End Object
    i_Background=ServerInfoBackground

    Begin Object class=GUITitleBar name=ServerInfoHeader
        Caption=""
        StyleName="Header"
        Justification=TXTA_Center
        WinWidth=1
        WinHeight=0.1
        WinLeft=0
        WinTop=0
        bBoundToParent=true
        bScaleToParent=true
        Effect=material'CO_Final'
    End Object
    t_Header=ServerInfoHeader

    Begin Object class=GUIFooter name=ServerInfoFooter
        WinWidth=1
        WinHeight=0.075
        WinLeft=0
        WinTop=0.925
        StyleName="Footer"
//        Justification=TXTA_Center
        bBoundToParent=true
        bScaleToParent=true
    End Object
    t_Footer=ServerInfoFooter

    Begin Object Class=GUIButton Name=ServerBackButton
        Caption="Close"
        Hint="Close this menu"
        WinWidth=0.120000
        WinHeight=0.055000
        WinLeft=0.848750
        WinTop=0.934167
        bBoundToParent=true
        bScaleToParent=true
        OnClick=ButtonClicked
        StyleName="SquareButton"
        RenderWeight=0.51
    End Object
    b_Close=ServerBackButton

    Begin Object Class=GUITabControl Name=ServerInfoTabs
        WinWidth=1.0
        WinLeft=0
        WinTop=0.1
        WinHeight=0.06
        TabHeight=0.04
        bAcceptsInput=true
        bDockPanels=true
        bBoundToParent=true
        bScaleToParent=true
        bFillSpace=True
    End Object
    c_Tabs=ServerInfoTabs

    bAllowedAsLast=true
    WinLeft=0.2
    WinWidth=0.6
    WinTop=0.1
    WinHeight=0.8

	PanelClass(0)="GUI2K4.UT2K4Tab_ServerMOTD"
	PanelClass(1)="GUI2K4.UT2K4Tab_ServerInfo"
	PanelClass(2)="GUI2K4.UT2K4Tab_ServerMapList"

	PanelCaption(0)="MOTD"
	PanelCaption(1)="Rules"
	PanelCaption(2)="Maps"

    PanelHint(0)="Message of the Day"
    PanelHint(1)="Game Rules"
    PanelHint(2)="Map Rotation"
}
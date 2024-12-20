//==============================================================================
//  Server version of IAMultiColumnRulesPanel.
//  Only shows Voting and Server settings.
//
//  Created by Bruce Bickar
//  � 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Tab_ServerRulesPanel extends IAMultiColumnRulesPanel
	config;

var config bool bLANServer;

var automated moCheckbox     ch_Webadmin, ch_LANServer;
var automated moNumericEdit  nu_Port;

var int Port;
var bool bWebadmin;

//------------------------------------------------------------------------------------------------
function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
    Super.InitComponent(MyController, MyOwner);

	// Shrink the list to make room for additional options
	lb_Rules.WinHeight=0.875008;

	// move checkbox over to the left
	ch_Advanced.WinWidth=0.331250;
	ch_Advanced.WinHeight=0.043750;
	ch_Advanced.WinLeft=0.073126;
	ch_Advanced.WinTop=0.890000;

	RemoveComponent(b_Symbols);

	i_BK.SetPosition(0.000505,0.014733,0.996997,0.853123);
	lb_Rules.SetPosition(0.024912,0.066344,0.950175,0.735499);

}
//------------------------------------------------------------------------------------------------
protected function bool ShouldDisplayRule(int Index)
{
	if ( GamePI.Settings[Index].bAdvanced && !Controller.bExpertMode )
		return false;

    // Only multiplayer-specific setting on this page
    return GamePI.Settings[Index].bMPOnly;
}
//------------------------------------------------------------------------------------------------
// rjp --
protected function SetGamePI()
{
	GamePI = p_Anchor.RuleInfo;
	GamePI.Sort(6);
}

function Change(GUIComponent Sender)
{
	if ( Sender == ch_Webadmin )
	{
		bWebadmin = ch_Webadmin.IsChecked();
		UpdatePortState();
	}

	else if ( Sender == ch_LANServer )
	{
		bLANServer = ch_LANServer.IsChecked();
		SaveConfig();
	}

	else if ( Sender == nu_Port )
		Port = nu_Port.GetValue();
}

function string Play()
{
	local bool b;

	if ( bWebadmin != class'UWeb.WebServer'.default.bEnabled )
	{
		class'UWeb.WebServer'.default.bEnabled = bWebadmin;
		b = True;
	}

	if ( Port != class'UWeb.WebServer'.default.ListenPort )
	{
		class'UWeb.Webserver'.default.ListenPort = Port;
		b = True;
	}

	if ( b )
		class'UWeb.Webserver'.static.StaticSaveConfig();

	if ( bLANServer )
		return " -lanplay";

	return "";
}

function InternalOnLoadINI(GUIComponent Sender, string S)
{
	switch (Sender)
	{
		case ch_Webadmin:
			bWebadmin = class'UWeb.WebServer'.default.bEnabled;
			ch_Webadmin.Checked(bWebAdmin);
			UpdatePortState();
			break;

		case ch_LANServer:
			ch_LANServer.Checked(bLANServer);
			break;

		case nu_Port:
			Port = class'UWeb.WebServer'.default.ListenPort;
			nu_Port.SetValue(Port);
			break;
	}
}

function UpdateBotSetting(int i)
{
}
function UpdateSymbolButton()
{
}

function UpdatePortState()
{
	if ( bWebAdmin )
		EnableComponent(nu_Port);
	else DisableComponent(nu_Port);
}

event Closed(GUIComponent Sender, bool bCancelled)  // Called when the Menu Owner is closed
{
	local bool b;

	Super.Closed(Sender, bCancelled);

	if ( bWebadmin != class'UWeb.WebServer'.default.bEnabled )
	{
		class'UWeb.WebServer'.default.bEnabled = bWebadmin;
		b = True;
	}

	if ( Port != class'UWeb.WebServer'.default.ListenPort )
	{
		class'UWeb.Webserver'.default.ListenPort = Port;
		b = True;
	}

	if ( b )
		class'UWeb.Webserver'.static.StaticSaveConfig();
}
// -- rjp
defaultproperties
{
	Begin Object Class=moCheckbox Name=LANServer
		Caption="LAN Server"
		Hint="Optimizes various engine and network settings for LAN-based play.  Enabling this option when running an internet server will cause EXTREME lag during the match!"
		WinWidth=0.331250
		WinHeight=0.043750
		WinLeft=0.073126
		WinTop=0.953334
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=Change
		TabOrder=3
		bAutoSizeCaption=True
	End Object
	ch_LANServer=LANServer

	Begin Object Class=moCheckbox Name=EnableWebadmin
		Caption="Enable WebAdmin"
		Hint="Enables remote web-based administration of the server"
		WinWidth=0.295063
		WinHeight=0.043750
		WinLeft=0.586874
		WinTop=0.893334
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=Change
		TabOrder=4
		bAutoSizeCaption=True
	End Object
	ch_Webadmin=EnableWebAdmin

	Begin Object Class=moNumericEdit Name=WebadminPort
		Caption="WebAdmin Port"
		Hint="Select which port should be used to connect to the remote web-based administration"
		WinWidth=0.295063
		WinHeight=0.043750
		WinLeft=0.586874
		WinTop=0.953334
		MinValue=1
		MaxValue=65536
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=Change
		CaptionWidth=0.7
		ComponentWidth=0.3
		TabOrder=5
		bAutoSizeCaption=True
	End Object
	nu_Port=WebadminPort
}


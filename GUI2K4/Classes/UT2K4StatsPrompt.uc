//==============================================================================
//	Created on: 09/01/2003
//	Menu which appears when attempting to connect to a stats-enabled server when
//  client doesn't have stats enabled
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4StatsPrompt extends UT2StatsPrompt;

var automated GUIImage i_Background, i_PageBG;
var automated GUIButton b_OK, b_Cancel;
var automated GUILabel l_Title, l_Message;

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_OK )
	{
		if ( Controller.OpenMenu(Controller.GetSettingsPage(), class'UT2K4SettingsPage'.default.PanelCaption[3]) )
		{
			if ( UT2K4SettingsPage(Controller.ActivePage) != None && UT2K4SettingsPage(Controller.ActivePage).tp_Game != None )
					UT2K4SettingsPage(Controller.ActivePage).tp_Game.ch_TrackStats.Checked(True);
		}
	}

	else if ( Sender == b_Cancel )
		Controller.ReplaceMenu( Controller.GetServerBrowserPage(),,,True );

	return true;
}

function ReOpen()
{
	if(Len(PlayerOwner().StatsUserName) >= 4 && Len(PlayerOwner().StatsPassword) >= 6)
		OnClose();
}

defaultproperties
{
    Begin Object Class=GUIImage Name=menuBackground
        Image=material'2K4Menus.Controls.menuBackground'
        ImageStyle=ISTY_Scaled
        ImageRenderStyle=MSTY_Normal
        ImageColor=(R=255,G=255,B=255,A=255)
        WinWidth=1
        WinHeight=1.000000
        WinLeft=0
        WinTop=0
        RenderWeight=0.0001
        X1=0
        Y1=0
        X2=1024
        Y2=768
    End Object
    i_PageBG=MenuBackground


	Begin Object class=GUIImage name=PasswordBackground
		WinWidth=1.000000
		WinHeight=251.000000
		WinLeft=0.000000
		WinTop=0.000000
		Image=Material'2K4Menus.Controls.editbox_b'
		ImageStyle=ISTY_Stretched
        ImageRenderStyle=MSTY_Normal
        DropShadow=Material'2K4Menus.Controls.shadow'
        DropShadowX=0
        DropShadowY=10
		bBoundToParent=true
		bScaleToParent=true
	End Object
	i_Background=PasswordBackground

	Begin Object Class=GUIButton Name=YesButton
		Caption="YES"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.125000
		WinTop=0.81
		bBoundToParent=true
		OnClick=InternalOnClick
	End Object
	b_OK=YesButton

	Begin Object Class=GUIButton Name=NoButton
		Caption="NO"
		WinWidth=0.2
		WinHeight=0.04
		WinLeft=0.65
		WinTop=0.81
		bBoundToParent=true
		OnClick=InternalOnClick
	End Object
	b_Cancel=NoButton

	Begin Object class=GUILabel Name=PromptHeader
		Caption="This server has UT2004 stats ENABLED!"
		TextAlign=TXTA_Center
		StyleName="TextLabel"
		FontScale=FNS_Large
		bMultiLine=True
		WinWidth=1.000000
		WinHeight=0.051563
		WinLeft=0.000000
		WinTop=0.354166
	End Object
	l_Title=PromptHeader

	Begin Object class=GUILabel Name=PromptDesc
		Caption="You will only be able to join this server by turning on \"Track Stats\" and setting a unique Stats Username and Password. Currently you will only be able to connect to servers with stats DISABLED.||Would you like to configure your Stats Username and Password now?"
		TextAlign=TXTA_Center
		StyleName="TextLabel"
		bMultiLine=True
		FontScale=FNS_Medium
		WinWidth=1.000000
		WinHeight=0.256251
		WinLeft=0.000000
		WinTop=0.422917
	End Object
	l_Message=PromptDesc

	WinLeft=0
	WinTop=0.325
	WinWidth=1
	WinHeight=0.325

//	OnReOpen=ReOpen
	bAlwaysAutomate=True
}

//==============================================================================
//	Created on: 08/13/2003
//	Eventually this panel and UT2K4Tab_IAMaplist will be combined to allow server admins to switch
//  maps using context menus during online matches
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Tab_ServerMapList extends MidGamePanel;

var() bool bClean;
var automated GUIListBoxBase lb_Maps;
var automated GUIImage i_BG, i_BG2;
var automated GUILabel l_Title;

var() localized string DefaultText;
var() bool bReceivedMaps;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
 	Super.InitComponent(MyController, MyOwner);

   	bClean = true;
   	if ( GUIScrollTextBox(lb_Maps) != None )
	   	GUIScrollTextBox(lb_Maps).SetContent(DefaultText);
}

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);

	if ( bShow && !bReceivedMaps )
	{
		SetTimer(3.0,True);
		Timer();
	}
}

function Timer()
{
	if ( bReceivedMaps || xPlayer(PlayerOwner()) == None )
	{
		KillTimer();
		return;
	}

	xPlayer(PlayerOwner()).ProcessMapName = ProcessMapName;
	xPlayer(PlayerOwner()).ServerRequestMapList();
}

function ProcessMapName(string NewMap)
{
	bReceivedMaps = True;
	if (NewMap=="")
	{
		bClean = true;
		GUIScrollTextBox(lb_Maps).SetContent(DefaultText);
	}
	else
	{
		if (bClean)
			GUIScrollTextBox(lb_Maps).SetContent(NewMap);
		else
			GUIScrollTextBox(lb_Maps).AddText(NewMap);

		bClean = false;
	}
}

defaultproperties
{
	Begin Object Class=GUIScrollTextBox Name=InfoText
		WinWidth=1.000000
		WinHeight=0.834375
		WinLeft=0.000000
		WinTop=0.143750
		CharDelay=0.0025
		EOLDelay=0
		StyleName="NoBackground"
        bNoTeletype=true
        bNeverFocus=true
        TextAlign=TXTA_Center
        bBoundToParent=true
        bScaleToParent=true
	End Object
	lb_Maps=InfoText

	Begin Object class=GUIImage Name=ServerInfoBK1
		WinWidth=0.418437
		WinHeight=0.016522
		WinLeft=0.021641
		WinTop=0.070779
		Image=Material'InterfaceContent.Menu.BorderBoxD'
		ImageColor=(R=255,G=255,B=255,A=160);
		ImageRenderStyle=MSTY_Alpha
		ImageStyle=ISTY_Stretched
        bBoundToParent=true
        bScaleToParent=true
	End Object
	i_BG=ServerInfoBK1

	Begin Object class=GUIImage Name=ServerInfoBK2
		WinWidth=0.395000
		WinHeight=0.016522
		WinLeft=0.576329
		WinTop=0.070779
		Image=Material'InterfaceContent.Menu.BorderBoxD'
		ImageColor=(R=255,G=255,B=255,A=160);
		ImageRenderStyle=MSTY_Alpha
		ImageStyle=ISTY_Stretched
        bBoundToParent=true
        bScaleToParent=true
	End Object
	i_BG2=ServerInfoBK2

	Begin Object class=GUILabel Name=ServerInfoLabel
		Caption="Maps"
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=1.000000
		WinHeight=32.000000
		WinLeft=0.000000
		WinTop=0.045312
        bBoundToParent=true
        bScaleToParent=true
	End Object
	l_Title=ServerInfoLabel

    DefaultText="Receiving Map Rotation from Server..."
}

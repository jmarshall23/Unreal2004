//==============================================================================
//  Created on: 11/12/2003
//  Contains controls for administering maps & maplists
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class AdminPanelMaps extends AdminPanelBase;

var automated GUIListBoxBase lb_Maps;
var bool bReceivedMaps;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);

	lb_Maps.NotifyContextSelect = HandleContextSelect;
	lb_Maps.ContextMenu.ContextItems.Remove(1, lb_Maps.ContextMenu.ContextItems.Length - 1);
}

function ShowPanel()
{
	Super.ShowPanel();
	if ( !bReceivedMaps )
		RefreshMaplist();
}

function RefreshMaplist()
{
	bReceivedMaps = False;
	SetTimer(3.0,True);
	Timer();
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
	GUIList(lb_Maps.MyList).Add(NewMap);
}

function bool HandleContextSelect(GUIContextMenu Sender, int Index)
{
	local string MapName;

	if ( Sender != None )
	{
		MapName = GUIList(lb_Maps.MyList).Get();
		if (MapName != "")
			Console(Controller.Master.Console).DelayedConsoleCommand("open"@MapName);

		Controller.CloseAll(False,True);
	}

	return true;
}

defaultproperties
{
	Begin Object class=MaplistBox Name=Maplist
		WinWidth=1.000000
		WinHeight=0.834375
		WinLeft=0.000000
		WinTop=0.143750
		bVisibleWhenEmpty=True
		bBoundToParent=True
		bScaleToParent=True
		StyleName="NoBackground"
	End Object
	lb_Maps=Maplist

	PanelCaption="Maps"
}

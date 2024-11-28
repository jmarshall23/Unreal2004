//==============================================================================
//	Created on: 09/12/2003
//	This tab is used in instant action games.
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Tab_IAMaplist extends UT2K4Tab_ServerMapList;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);

	lb_Maps.NotifyContextSelect = HandleContextSelect;
	lb_Maps.ContextMenu.ContextItems.Remove(1, lb_Maps.ContextMenu.ContextItems.Length - 1);
}

function bool HandleContextSelect(GUIContextMenu Sender, int Index)
{
	local string MapName;

	if ( Sender != None )
	{
		MapName = GUIList(lb_Maps.MyList).Get();
		if (MapName != "")
			Console(Controller.Master.Console).DelayedConsoleCommand("open"@MapName);

		Controller.CloseAll(False,true);
	}

	return true;
}

function ProcessMapName(string NewMap)
{
	bReceivedMaps = True;
	GUIList(lb_Maps.MyList).Add(NewMap);
}

DefaultProperties
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
}

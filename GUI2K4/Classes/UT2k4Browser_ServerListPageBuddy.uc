//====================================================================
//  Written by Ron Prestenback
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class UT2K4Browser_ServerListPageBuddy extends UT2K4Browser_ServerListPageMS;

// Actual list of buddies
var() config float			BuddySplitterPosition;
var() config array<String> 	Buddies;
var() config string			BuddyListBoxClass;

var GUISplitter					sp_Buddy;
var UT2K4Browser_BuddyListBox 	lb_Buddy;
var UT2K4Browser_BuddyList 		li_Buddy;

var localized string AddBuddyCaption, AddBuddyLabel;
var localized string RemoveBuddyCaption;
var localized string BuddyNameCaption;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;

	Super.InitComponent(MyController, MyOwner);
	lb_Buddy.SetAnchor(Self);
	li_Buddy = UT2K4Browser_BuddyList(lb_Buddy.List);
	li_Buddy.OnChange = BuddyListChanged;
	li_Buddy.OnRightClick = InternalOnRightClick;

	lb_Buddy.TabOrder = 0;
	lb_Server.TabOrder = 1;
	lb_Rules.TabOrder = 2;
	lb_Players.TabOrder = 3;

	for ( i = 0; i < Buddies.Length; i++ )
		li_Buddy.AddedItem();
}

event Opened( GUIComponent Sender )
{
	Super.Opened(Sender);

	Controller.AddBuddy = AddBuddy;
}

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);

	if (bShow && bInit)
	{
		sp_Buddy.SplitterUpdatePositions();
		bInit = False;
	}
}

function Refresh()
{
	local int i;

	Super.Refresh();

	// Construct query containing all buddy names
	for(i=0; i<Buddies.Length; i++)
		AddQueryTerm("buddy", Buddies[i], QT_Equals);

	// Run query
	Browser.Uplink().StartQuery(CTM_Query);

	SetFooterCaption(StartQueryString);
	KillTimer(); // Stop it going back to ready from a previous timer!
}

function BuddyListChanged(GUIComponent Sender)
{
	// Add code here to highlight server for this buddy
}

function AddBuddy(optional string NewBuddy)
{
	if ( Controller.OpenMenu(Controller.RequestDataMenu, AddBuddyLabel, BuddyNameCaption) )
	{
		Controller.ActivePage.SetDataString(NewBuddy);
		Controller.ActivePage.OnClose = BuddyPageClosed;
	}
}

function BuddyPageClosed( bool bCancelled )
{
	local string s;

	if ( bCancelled )
		return;

	s = Controller.ActivePage.GetDataString();
	if ( s != "" )
	{
		if ( FindBuddyIndex(s) != -1 )
			return;

		Buddies[Buddies.Length] = s;
		li_Buddy.AddedItem();
		SaveConfig();
	}
}

function InternalOnCreateComponent(GUIComponent NewComp, GUIComponent Sender)
{
	if (GUISplitter(NewComp) != None)
	{
		// This splitter already has a panel
		if (GUISplitter(Sender).Panels[0] != None)
		{
			// This splitter is the main splitter
			if (UT2K4Browser_ServerListPageBuddy(Sender.MenuOwner) != None)
			{
				sp_Buddy = GUISplitter(NewComp);
				sp_Buddy.DefaultPanels[0] = "GUI2K4.UT2K4Browser_ServerListBox";
				sp_Buddy.DefaultPanels[1] = "XInterface.GUISplitter";
				sp_Buddy.WinTop=0;
				sp_Buddy.WinLeft=0;
				sp_Buddy.WinWidth=1.0;
				sp_Buddy.WinHeight=1.0;
				sp_Buddy.bNeverFocus=True;
				sp_Buddy.bAcceptsInput=True;
				sp_Buddy.RenderWeight=0;
				sp_Buddy.OnCreateComponent=InternalOnCreateComponent;
				sp_Buddy.OnLoadIni=InternalOnLoadIni;
				sp_Buddy.OnReleaseSplitter=InternalReleaseSplitter;
				sp_Buddy.SplitOrientation=SPLIT_Vertical;
			}

			// This is the second panel of sp_Buddy splitter
			else Super.InternalOnCreateComponent(NewComp, Sender);
		}

		else
			Super.InternalOnCreateComponent(NewComp, Sender);
	}

	else if (UT2K4Browser_BuddyListBox(NewComp) != None)
	{
		lb_Buddy = UT2K4Browser_BuddyListBox(NewComp);
	}

	else Super.InternalOnCreateComponent(NewComp, Sender);
}

function InternalOnLoadIni(GUIComponent Sender, string S)
{
	if (Sender == sp_Buddy)
		sp_Buddy.SplitPosition = BuddySplitterPosition;

	else Super.InternalOnLoadIni(Sender, S);
}

function InternalReleaseSplitter(GUIComponent Sender, float NewPos)
{
	if (Sender == sp_Buddy)
	{
		BuddySplitterPosition = NewPos;
		SaveConfig();
	}

	else Super.InternalReleaseSplitter(Sender, NewPos);
}

function int FindBuddyIndex( string BuddyName )
{
	local int i;

	for ( i = 0; i < Buddies.Length; i++ )
		if ( Buddies[i] ~= BuddyName )
			return i;

	return -1;
}

function ContextSelect( GUIContextMenu Sender, int Index )
{
	if ( !NotifyContextSelect(Sender, Index) )
	{
		switch ( Index )
		{
		case 0: AddBuddy(); break;
		case 1:
			if ( li_Buddy.IsValid() )
			{
				Buddies.Remove(li_Buddy.Index, 1);
				li_Buddy.RemovedCurrent();
				SaveConfig();
			}

			break;
		}
	}
}

defaultproperties
{
	BuddyListBoxClass="GUI2K4.UT2K4Browser_BuddyListBox"

	Begin Object Class=GUISplitter Name=HorzSplitter
		WinWidth=1.000000
		WinHeight=1.0
		WinLeft=0.000000
		WinTop=0.000000
		bBoundToParent=True
		bScaleToParent=True
		bNeverFocus=True
		bAcceptsInput=True
		DefaultPanels(0)="GUI2K4.UT2K4Browser_BuddyListBox"
		DefaultPanels(1)="XInterface.GUISplitter"
		SplitOrientation=SPLIT_Horizontal
		SplitAreaSize=8
		RenderWeight=1
		IniOption="@Internal"
		OnCreateComponent=InternalOnCreateComponent
		OnLoadIni=InternalOnLoadIni
		OnReleaseSplitter=InternalReleaseSplitter
	End Object

	Begin Object Class=GUIContextMenu Name=RCMenu
		ContextItems(0)="Add Buddy"
		ContextItems(1)="Remove Buddy"
		OnSelect=ContextSelect
	End Object

	ContextMenu=RCMenu
	sp_Main=HorzSplitter
	BuddySplitterPosition=0.597582
	MainSplitterPosition=0.184326
	DetailSplitterPosition=0.319135
	HeaderColumnSizes(0)=(ColumnSizes=(0.096562,0.493471,0.206944,0.102535,0.150000))
	HeaderColumnSizes(1)=(ColumnSizes=(0.498144,0.500000))
	HeaderColumnSizes(2)=(ColumnSizes=(0.473428,0.185665,0.226824,0.220000))
	PanelCaption="Buddy Browser"

	AddBuddyLabel="Add Buddy"
	AddBuddyCaption="ADD BUDDY"
	RemoveBuddyCaption="REMOVE BUDDY"
	BuddyNameCaption="Buddy Name: "
}

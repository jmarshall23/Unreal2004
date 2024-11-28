//==============================================================================
//  Created on: 12/11/2003
//  Description
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4Tab_MutatorBase extends UT2K4GameTabBase;

var config 	string 	LastActiveMutators;

var automated GUISectionBackground  sb_Avail, sb_Active, sb_Description;
var automated GUIListBox			lb_Avail, lb_Active;
var automated GUIScrollTextBox		lb_MutDesc;
var automated GUIButton				b_Config, b_Add,b_AddAll,b_Remove,b_RemoveAll;
//var automated GUILabel				l_AvailTitle,l_ActiveTitle;
//var automated GUIImage				i_Shadow, i_Shadow2, i_Shadow3;

var CacheManager.GameRecord           CurrentGame;
var	array<CacheManager.MutatorRecord> MutatorList;
var string				MutConfigMenu;

var localized string GroupConflictText, ThisText, TheseText, ContextItems[3];

delegate OnChangeMutators( string ActiveMutatorString );

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);

	// Setup the lists
	if (lb_Avail.List != None)
	{
		lb_Avail.List.OnDragDrop = RemoveMutatorDrop;
		lb_Avail.List.bInitializeList = False;
		lb_Avail.List.bDropSource = True;
		lb_Avail.List.bDropTarget = True;
	}

	if (lb_Active.List != None)
	{
		lb_Active.List.OnDragDrop = AddMutatorDrop;
		lb_Active.List.bInitializeList = False;
		lb_Active.List.bDropSource = True;
		lb_Active.List.bDropTarget = True;
	}


	// Setup the listbox context menus
	if ( lb_Avail.ContextMenu != None )
	{
		lb_Avail.ContextMenu.OnSelect = ContextClick;
		lb_Avail.ContextMenu.OnOpen = lb_Avail.MyOpen;
		lb_Avail.ContextMenu.OnClose = lb_Avail.MyClose;
	}

	if ( lb_Active.ContextMenu != None )
	{
		lb_Active.ContextMenu.OnSelect = ContextClick;
		lb_Active.ContextMenu.OnOpen = lb_Active.MyOpen;
		lb_Active.ContextMenu.OnClose = lb_Active.MyClose;
	}

	lb_Avail.List.OnDblClick=AvailDblClick;
	lb_Active.List.OnDblClick=SelectedDblClick;

	// Setup the listbox notification for disabling buttons
	lb_Avail.List.CheckLinkedObjects = InternalCheckObj;
	lb_Avail.List.AddLinkObject( b_Add, True );

	lb_Active.List.CheckLinkedObjects = InternalCheckObj;
	lb_Active.List.AddLinkObject( b_Remove, True );

	// Setup initial button states manually
	lb_Active.List.DisableLinkedObjects();
	lb_Avail.List.DisableLinkedObjects();
	InternalCheckObj(None);

	sb_Avail.ManageComponent(lb_Avail);
	sb_Active.ManageComponent(lb_Active);
	sb_Description.ManageComponent(lb_MutDesc);
}

function ShowPanel( bool bShow )
{
	local string s;

	Super.ShowPanel(bShow);

	if ( bShow && bInit )
		bInit = False;

	if ( !bShow && !bInit )
	{
		s = BuildActiveMutatorString();
		if ( s != LastActiveMutators )
		{
			LastActiveMutators = s;
			OnChangeMutators( LastActiveMutators );
			SaveConfig();
		}
	}
}

// Called when a new gametype has been selected - remove any mutators which affect physics if
// this gametype doesn't allow them
function SetCurrentGame( CacheManager.GameRecord CurrentGame )
{
	local int i;
	local string m, t;
	local class<GameInfo> GameClass;

	if ( MutatorList.Length > 0 )
		m = BuildActiveMutatorString();
	else m = LastActiveMutators;

	class'CacheManager'.static.GetMutatorList(MutatorList);
	GameClass = class<GameInfo>(DynamicLoadObject(CurrentGame.ClassName, class'Class'));
	if ( GameClass != None )
	{
		for ( i = MutatorList.Length - 1; i >= 0; i-- )
			if ( !GameClass.static.AllowMutator(MutatorList[i].ClassName) )
				MutatorList.Remove(i,1);
	}

	// Disable the list's OnChange() delegate
	lb_Active.List.bNotify = False;
	lb_Avail.List.bNotify = False;

	lb_Active.List.Clear();
	lb_Avail.List.Clear();

	for (i=0;i<MutatorList.Length;i++)
		lb_Avail.List.Add(MutatorList[i].FriendlyName,,MutatorList[i].Description);

	t = NextMutatorInString(m);
	while (t!="")
	{
		SelectMutator(t);
		t = NextMutatorInString(m);
	}

	lb_Active.List.bNotify = True;
	lb_Avail.List.bNotify = True;

	lb_Active.List.CheckLinkedObjects(lb_Active.List);
	lb_Avail.List.CheckLinkedObjects(lb_Avail.List);
}

// When a listitem has changed, update the menustate of all linked objects
// (disable add button if no mutators to add, etc.)
function InternalCheckObj( GUIListBase List )
{
	if ( List != None )
	{
		if ( List.IsValid() )
		{
			List.EnableLinkedObjects();
			if ( List.ItemCount > 0 && Controller.bCurMenuInitialized )
				lb_MutDesc.SetContent(GUIList(List).GetExtra());
		}
		else List.DisableLinkedObjects();
	}

	if ( lb_Active.List.ItemCount > 0 )
	{
		EnableComponent(b_Config);
		EnableComponent(b_RemoveAll);
	}
	else
	{
		DisableComponent(b_Config);
		DisableComponent(b_RemoveAll);
	}

	if ( lb_Avail.List.ItemCount > 0 )
		EnableComponent(b_AddAll);
	else DisableComponent(b_AddAll);
}

// Play is called when the play button is pressed.  It saves any relevant data and then
// returns any additions to the URL
function string Play()
{
	local string URL;
	local bool b;

	if ( !class'LevelInfo'.static.IsDemoBuild() )
	{
		URL = BuildActiveMutatorString();

		b = URL != LastActiveMutators || URL != default.LastActiveMutators;

		if ( URL != "")
		{
			LastActiveMutators = URL;
			URL = "?Mutator=" $ URL;
		}
		else
			LastActiveMutators="";

		if ( b )
			SaveConfig();
	}

	return url;
}

// Builds a comma-delimited string consisting of the selected mutators
function string BuildActiveMutatorString()
{
	local string Str, Result;
	local int i;

	for ( i = 0; i < lb_Active.List.ItemCount; i++ )
	{
		Str = ResolveMutator(lb_Active.List.GetItemAtIndex(i));
		if ( Str != "" )
		{
			if ( Result != "" )
				Result $= ",";

			Result $= Str;
		}
	}

	return Result;
}

// Finds the classname for a mutator based on its friendly name
function string ResolveMutator(string FriendlyName)
{
	local int i;

	for (i=0;i<MutatorList.Length;i++)
		if (MutatorList[i].FriendlyName~=FriendlyName)
			return MutatorList[i].ClassName;

	return "";
}

// Returns the next mutator in a comma-delimited list
function string NextMutatorInString(out string mut)
{
	local string t;
	local int p;

	if (Mut=="")
		return "";

	p = Instr(Mut,",");
	if (p<0)
		p = Len(Mut);

	EatStr(t, Mut, p);
	Mut = Mid(Mut, 1);
	return t;
}

// Adds a mutator by classname
function SelectMutator(string mutclass)
{
	local int i;

	for (i = 0; i < MutatorList.Length; i++)
		if ( MutatorList[i].ClassName ~= mutclass && lb_Avail.List.Find(MutatorList[i].FriendlyName) != "" )
		{
			AddMutator(none);
			return;
		}
}


function bool AvailDBLClick(GUIComponent Sender)
{
	AddMutator(Sender);
	return true;
}

function bool SelectedDBLClick(GUIComponent Sender)
{

	RemoveMutator(Sender);
	return true;
}

// Called when the Configure Mutator button is clicked - opens the custom mutator configuration screen
function bool MutConfigClick(GUIComponent Sender)
{
	local array<string> MutClassNames;
	local int i;

	MutClassNames.Length = lb_Active.List.ItemCount;
	for (i = 0; i < lb_Active.List.ItemCount; i++)
		MutClassNames[i] = ResolveMutator(lb_Active.List.GetItemAtIndex(i));

	if (Controller.OpenMenu(MutConfigMenu))
	{
		MutatorConfigMenu(Controller.ActivePage).ActiveMuts = MutClassNames;
		MutatorConfigMenu(Controller.ActivePage).Initialize();
	}

	return true;
}

function ListChange(GUIComponent Sender)
{
	if (Sender == lb_Avail)
	{
		lb_Active.List.SilentSetIndex(-1);
		InternalCheckObj(lb_Active.List);
	}

	else if (Sender == lb_Active)
	{
		lb_Avail.List.SilentSetIndex(-1);
		InternalCheckObj(lb_Avail.List);
	}

	else lb_MutDesc.SetContent("");
}

// Called each time a mutator is added - ensures that only one mutator from each group is active
function bool AddingGroup(string Group)
{
	local int i;
	local GUIListElem Element;
	local bool bConflict;

	if (Group == "")
		return false;

	for ( i = lb_Active.List.ItemCount - 1; i >= 0; i-- )
	{
		if ( GetGroupFor(lb_Active.List.GetItemAtIndex(i)) ~= Group)
		{
			lb_Active.List.GetAtIndex( i, Element.item, Element.ExtraData, Element.ExtraStrData );
			lb_Avail.List.AddElement(Element);
			lb_Active.List.RemoveSilent( i, 1);
			SetFooterCaption(GroupConflictText, 2.0);
			bConflict = True;
		}
	}
	return bConflict;
}


function string GetGroupFor(string FriendlyName)
{
	local int i;
	for (i=0;i<MutatorList.Length;i++)
		if (MutatorList[i].FriendlyName ~= FriendlyName)
			return MutatorList[i].GroupName;

	return "";
}

function bool AddMutator(GUIComponent Sender)
{
	local int i, j;
	local string gname;
	local array<GUIListElem> PendingElements;
	local array<string> Groups;

	if ( !lb_Avail.List.IsValid() )
		return true;

	PendingElements = lb_Avail.List.GetPendingElements(True);

	lb_Avail.List.bNotify = False;
	for ( i = PendingElements.Length - 1; i >= 0 ; i-- )
	{
		gName = GetGroupFor(PendingElements[i].Item);

		// Remove any elements from the pending list that are in the same group - the first one of the group stays
		for ( j = 0; j < Groups.Length; j++ )
		{
			if ( Groups[j] ~= gName )
			{
				PendingElements.Remove(i, 1);
				break;
			}
		}

		if ( j == Groups.Length )
			Groups[j] = gName;

		lb_Avail.List.RemoveElement(PendingElements[i],, True);
		AddingGroup(gName);
		lb_Active.List.AddElement( PendingElements[i] );
	}

	lb_Avail.List.bNotify = True;
	lb_Avail.List.ClearPendingElements();
	lb_Avail.List.SetIndex(lb_Avail.List.Index);	// to check if button states should change

	if ( lb_Avail.List.bSorted )
		lb_Avail.List.Sort();

	return true;
}

function bool AddMutatorDrop(GUIComponent Sender)
{
	local array<GUIListElem> PendingElements;
	local array<string> Groups;
	local string gName;
	local int i, j;

	if ( Controller == None || Controller.DropSource != lb_Avail.List )
		return false;

	if ( !lb_Avail.List.IsValid() )
		return true;

	PendingElements = lb_Avail.List.GetPendingElements();
	for ( i = PendingElements.Length - 1; i >= 0 ; i-- )
	{
		gName = GetGroupFor(PendingElements[i].Item);

		// Remove any elements from the pending list that are in the same group - the first one of the group stays
		for ( j = 0; j < Groups.Length; j++ )
		{
			if ( Groups[j] ~= gName )
			{
				RemovePendingMutator(PendingElements[i]);
				PendingElements.Remove(i, 1);
				break;
			}
		}

		if ( j == Groups.Length )
			Groups[j] = gName;

		AddingGroup(gName);
	}

	return lb_Active.List.InternalOnDragDrop(Sender);
}

function bool RemoveMutatorDrop( GUIComponent Sender )
{
	if ( Controller == None || Controller.DropSource != lb_Active.List )
		return false;

	return lb_Avail.List.InternalOnDragDrop(Sender);
}

function RemovePendingMutator( GUIListElem Elem, optional GUIList List )
{
	local int i, Index;


	if ( List == None )
		List = lb_Avail.List;

	Index = List.FindIndex(Elem.item);
	if ( Index == -1 )
	{
		log("RemovePendingMutator() not executed.  Item not found in available list:"$Elem.Item@Elem.ExtraStrData);
		return;
	}

	for (i = 0; i < List.SelectedItems.Length; i++)
	{
		if (List.SelectedItems[i] == Index)
		{
			List.SelectedItems.Remove(i,1);
			break;
		}
	}

	for ( i = 0; i < List.SelectedElements.Length; i++ )
	{
		if ( List.SelectedElements[i] == Elem )
		{
			List.SelectedElements.Remove(i, 1);
			return;
		}
	}
}

function bool RemoveMutator(GUIComponent Sender)
{
	local int i;
	local array<GUIListElem> PendingElements;

	if ( !lb_Active.List.IsValid() )
		return true;

	PendingElements = lb_Active.List.GetPendingElements(True);
	lb_Active.List.bNotify = False;
	for ( i = 0; i < PendingElements.Length; i++ )
	{
		lb_Active.List.RemoveElement(PendingElements[i],, True);
		lb_Avail.List.AddElement(PendingElements[i]);
	}

	lb_Active.List.bNotify = True;
	lb_Active.List.ClearPendingElements();
	return true;
}

function bool AddAllMutators(GUIComponent Sender)
{
	local int i;
	local array<GUIListElem> Elem;

	Elem = lb_Avail.List.Elements;
	lb_Avail.List.Clear();

	for ( i = 0; i < Elem.Length; i++ )
	{
		AddingGroup(GetGroupFor(Elem[i].Item));
		lb_Active.List.AddElement(Elem[i]);
	}

	return true;
}

function bool RemoveAllMutators(GUIComponent Sender)
{
	lb_Avail.List.LoadFrom(lb_Active.List,false);
	lb_Active.List.Clear();
	return true;
}

function ContextClick(GUIContextMenu Sender, int Index)
{
	local array<string> MutClassNames, MutNames;
	local int i;


	if ( Sender == lb_Avail.ContextMenu )
	{
		if ( Index == 1 )
		{
			MutNames = lb_Avail.List.GetPendingItems(True);
			for (i = 0; i < MutNames.Length; i++)
				MutClassNames[i] = ResolveMutator(MutNames[i]);

			if (Controller.OpenMenu(MutConfigMenu))
			{
				MutatorConfigMenu(Controller.ActivePage).ActiveMuts = MutClassNames;
				MutatorConfigMenu(Controller.ActivePage).Initialize();
			}
		}

		else if (Index == 0)
			AddMutator(None);
	}

	else if ( Sender == lb_Active.ContextMenu )
	{
		if ( Index == 1 )
		{
			MutNames = lb_Active.List.GetPendingItems(True);
			for (i = 0; i < MutNames.Length; i++)
				MutClassNames[i] = ResolveMutator(MutNames[i]);

			if (Controller.OpenMenu(MutConfigMenu))
			{
				MutatorConfigMenu(Controller.ActivePage).ActiveMuts = MutClassNames;
				MutatorConfigMenu(Controller.ActivePage).Initialize();
			}
		}

		else if ( Index == 0 )
			RemoveMutator(None);
	}
}

function bool ContextMenuOpen(GUIComponent Sender, GUIContextMenu Menu, GUIComponent InMenuOwner)
{
	local array<string> Items;

	if ( GUIListBox(InMenuOwner) != None && GUIList(Sender) != None )
	{
		Items = GUIList(Sender).GetPendingItems(True);
		if ( Items.Length == 1 )
		{
			if ( InMenuOwner == lb_Avail )
				Menu.ContextItems[0] = Repl(ContextItems[0], "%text%", ThisText);
			else Menu.ContextItems[0] = Repl(ContextItems[1], "%text%", ThisText);

			Menu.ContextItems[1] = Repl(ContextItems[2], "%text%", ThisText);
		}
		else
		{
			if ( InMenuOwner == lb_Avail )
				Menu.ContextItems[0] = Repl(ContextItems[0], "%text%", TheseText);
			else Menu.ContextItems[0] = Repl(ContextItems[1], "%text%", TheseText);

			Menu.ContextItems[1] = Repl(ContextItems[2], "%text%", TheseText);
		}

		return true;
	}

	return false;
}


defaultproperties
{
	Begin Object Class=GUIContextMenu Name=RCMenu
	End Object

	ContextItems(0)="Add %text%"
	ContextItems(1)="Remove %text%"
	ContextItems(2)="Configure %text%"

	MutConfigMenu="GUI2K4.MutatorConfigMenu"

	ThisText="This Mutator"
	TheseText="These Mutators"

	GroupConflictText="Unable to add multiple mutators from the same mutator group!"

	Begin Object Class=GUISectionBackground Name=AvailBackground
		WinWidth=0.380859
		WinHeight=0.547697
		WinLeft=0.025156
		WinTop=0.036614
		Caption="Available Mutators"
		LeftPadding=0.0025
		RightPadding=0.0025
		TopPadding=0.0025
		BottomPadding=0.0025
	End Object
	sb_Avail=AvailBackground

	Begin Object Class=GUISectionBackground Name=ActiveBackground
		WinWidth=0.380859
		WinHeight=0.547697
		WinLeft=0.586876
		WinTop=0.036614
		Caption="Active Mutators"
		LeftPadding=0.0025
		RightPadding=0.0025
		TopPadding=0.0025
		BottomPadding=0.0025
	End Object
	sb_Active=ActiveBackground

	Begin Object Class=GUISectionBackground Name=DescriptionBackground
		WinWidth=0.942969
		WinHeight=0.291796
		WinLeft=0.025976
		WinTop=0.610678
		Caption="Mutator Details"
		LeftPadding=0.0025
		RightPadding=0.0025
		TopPadding=0.0025
		BottomPadding=0.0025
	End Object
	sb_Description=DescriptionBackground

	Begin Object Class=GUIListBox Name=IAMutatorAvailList
		WinWidth=0.378955
		WinHeight=0.501446
		WinLeft=0.026108
		WinTop=0.144937
		bVisibleWhenEmpty=true
		OnChange=ListChange
		bSorted=True
		Hint="These are the available mutators."
		TabOrder=0
		ContextMenu=RCMenu
		HandleContextMenuOpen=ContextMenuOpen
		StyleName="NoBackground"
	End Object
    lb_Avail=IAMutatorAvailList

	Begin Object Class=GUIListBox Name=IAMutatorSelectedList
		WinWidth=0.378955
		WinLeft=0.584376
		WinHeight=0.501446
		WinTop=0.144937
		bVisibleWhenEmpty=true
		OnChange=ListChange
		bSorted=True
		Hint="These are the current selected mutators."
		TabOrder=5
		ContextMenu=RCMenu
		StyleName="NoBackground"
	End Object
    lb_Active=IAMutatorSelectedList

	Begin Object Class=GUIScrollTextBox Name=IAMutatorScroll
		WinWidth=0.938254
		WinHeight=0.244296
		WinLeft=0.028333
		WinTop=0.648595
		CharDelay=0.0025
		EOLDelay=0.5
		bNeverFocus=true
		StyleName="NoBackground"
		bTabStop=False
		bVisibleWhenEmpty=True
	End Object
    lb_MutDesc=IAMutatorScroll

	Begin Object Class=GUIButton Name=IAMutatorConfig
		Caption="Configure Mutators"
		Hint="Configure the selected mutators"
		WinWidth=0.239063
		WinHeight=0.054648
		WinLeft=0.729492
		WinTop=0.933490
		bVisible=false
		OnClick=MutConfigClick
		TabOrder=6
	End Object
    b_Config=IAMutatorConfig

	Begin Object Class=GUIButton Name=IAMutatorAdd
		Caption="Add"
		Hint="Adds the selection to the list of mutators to play with."
		WinWidth=0.145000
		WinHeight=0.050000
		WinLeft=0.425
		WinTop=0.194114
		OnClickSound=CS_Up
		OnClick=AddMutator
		TabOrder=1
	End Object
    b_Add=IAMutatorAdd

	Begin Object Class=GUIButton Name=IAMutatorRemove
		Caption="Remove"
		Hint="Removes the selection from the list of mutators to play with."
		WinWidth=0.145000
		WinHeight=0.050000
		WinLeft=0.425
		WinTop=0.424322
		OnClickSound=CS_Down
		OnClick=RemoveMutator
		TabOrder=4
	End Object
	b_Remove=IAMutatorRemove

	Begin Object Class=GUIButton Name=IAMutatorAll
		Caption="Add All"
		Hint="Adds all mutators to the list of mutators to play with."
		WinWidth=0.145000
		WinHeight=0.050000
		WinLeft=0.425
		WinTop=0.259218
		OnClickSound=CS_Up
		OnClick=AddAllMutators
		TabOrder=2
	End Object
	b_AddAll=IAMutatorAll

	Begin Object Class=GUIButton Name=IAMutatorClear
		Caption="Remove All"
		Hint="Removes all mutators from the list of mutators to play with."
		WinWidth=0.145000
		WinHeight=0.050000
		WinLeft=0.425
		WinTop=0.360259
		OnClick=RemoveAllMutators
		OnClickSound=CS_Down
		TabOrder=3
	End Object
	b_RemoveAll=IAMutatorClear

	WinTop=0.15
	WinLeft=0
	WinWidth=1
	WinHeight=0.77
	bAcceptsInput=false
}

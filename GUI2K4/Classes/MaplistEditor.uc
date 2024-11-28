//==============================================================================
//  Created on: 12/18/2003
//  Description
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class MaplistEditor extends FloatingWindow;

var() editconst noexport array<MaplistRecord.MapItem> Maps;
var() editconst noexport CacheManager.GameRecord CurrentGameType;
var() editconst noexport MaplistManager MapHandler;
var() editconst noexport int            GameIndex, RecordIndex;

// Maplist controls
var automated GUITreeListBox	lb_ActiveMaps, lb_AllMaps;
var automated GUIButton			b_Add, b_AddAll, b_Remove, b_RemoveAll, b_MoveUp, b_MoveDown,
								b_New, b_Delete, b_Rename;
var automated GUIEditBox        ed_MapName;
var automated GUIComboBox       co_Maplist;

var automated GUISectionBackground sb_MapList, sb_Avail, sb_Active;

var() editconst noexport GUITreeList li_Active, li_Avail;
// Set by tabpanel that opened this page.
var UT2K4Tab_MainSP MainPanel;
var bool bOnlyShowCustom;
var bool bOnlyShowOfficial;

var() localized string InvalidMaplistClassText, NewMaplistPageCaption, MaplistEditCaption, RenameMaplistPageCaption,
                       LinkText, AutoSelectText;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
    Super.InitComponent(MyController, MyOwner);

	li_Active = lb_ActiveMaps.List;
	li_Avail = lb_AllMaps.List;

	if (li_Avail != None)
	{
//;;	li_Avail.bDropSource = True;
//;;	li_Avail.bDropTarget = True;
		li_Avail.OnDblClick = ModifyMapList;
		li_Avail.OnDragDrop = RemoveDragDrop;
		li_Avail.AddLinkObject( b_Add, True );
		li_Avail.bSorted = True;
		li_Avail.CheckLinkedObjects = InternalCheckLinkedObj;
	}

	if (li_Active != None)
	{
//;;	li_Active.bDropSource = True;
//;;	li_Active.bDropTarget = True;
		li_Active.OnDblClick = ModifyMapList;
		li_Active.OnDragDrop = AddDragDrop;
		li_Active.AddLinkObject( b_Remove, true );
		li_Active.AddLinkObject( b_MoveUp, True );
		li_Active.AddLinkObject( b_MoveDown, True );
		li_Active.bSorted = False;
		li_Active.CheckLinkedObjects = InternalCheckLinkedObj;

		li_Active.bGroupItems = False;
	}

	sb_Avail.ManageComponent(lb_AllMaps);
	sb_Active.ManageComponent(lb_ActiveMaps);

    co_Maplist.List.bInitializeList = False;
    SetupSizingCaption();
}

function SetupSizingCaption()
{
	local string str;

	str = b_New.Caption;
	if ( Len(b_Rename.Caption) > Len(str) )
		str = b_Rename.Caption;

	if ( Len(b_Delete.Caption) > Len(str) )
		str = b_Delete.Caption;

	b_New.SizingCaption = str;
	b_Rename.SizingCaption = str;
	b_Delete.SizingCaption = str;
}

function Initialize( MaplistManager InHandler )
{
	MapHandler = InHandler;

	//  Find the cache record for the current gametype
	if ( InitGameType() )
	{
	    ReloadAvailable();

	    // Refresh the custom maplist combo
	    RefreshMaplistNames(); // this will cause the active list to be reloaded
	}
}

// Called when a new gametype is selected
function bool InitGameType()
{
    local int i;
    local array<CacheManager.GameRecord> Games;

	// Get a list of all gametypes.
    class'CacheManager'.static.GetGameTypeList(Games);
	for (i = 0; i < Games.Length; i++)
    {
        if (Games[i].ClassName ~= Controller.LastGameType)
        {
            CurrentGameType = Games[i];
            GameIndex = MapHandler.GetGameIndex(CurrentGameType.ClassName);
            break;
        }
    }

    return i < Games.Length;
}

// Query the CacheManager for the maps that correspond to this gametype, then fill the 'available' list
function ReloadAvailable()
{
	local int i, j;
	local array<string> CustomLinkSetups;

	if ( MapHandler.GetAvailableMaps(GameIndex, Maps) )
	{
		li_Avail.bNotify = False;
		li_Avail.Clear();

		for ( i = 0; i < Maps.Length; i++ )
		{
			if ( class'CacheManager'.static.IsDefaultContent(Maps[i].MapName) )
			{
				if ( bOnlyShowCustom )
					continue;
			}
			else if ( bOnlyShowOfficial )
				continue;

			if ( Maps[i].Options.Length > 0 )
			{
				// Add the "auto link setup" item
				li_Avail.AddItem( AutoSelectText @ LinkText, Maps[i].MapName $ "?LinkSetup=Random", Maps[i].MapName );

				// Now add all custom link setups
				for ( j = 0; j < Maps[i].Options.Length; j++ )
					li_Avail.AddItem(Maps[i].Options[j].Value @ LinkText, Maps[i].MapName $ "?LinkSetup=" $ Maps[i].Options[j].Value, Maps[i].MapName );
			}
			else li_Avail.AddItem( Maps[i].MapName, Maps[i].MapName );

			if ( CurrentGameType.MapPrefix == "ONS" )
			{
				CustomLinkSetups = GetPerObjectNames( Maps[i].MapName, "ONSPowerLinkCustomSetup" );
				for ( j = 0; j < CustomLinkSetups.Length; j++ )
					li_Avail.AddItem( CustomLinkSetups[j] @ LinkText, Maps[i].MapName $ "?LinkSetup=" $ CustomLinkSetups[j], Maps[i].MapName );
			}
		}
	}

	if ( li_Avail.bSorted )
		li_Avail.Sort();

	li_Avail.bNotify = True;
}

singular function ReloadActive()
{
	local int i;
	local array<string> NewActiveMaps;

	// disable OnChange() calls
	li_Active.bNotify = False;
	li_Avail.bNotify = False;

	// Move all maps that are in 'active' list back to the 'avail' list
//	li_Avail.LoadFrom(li_Active);
	li_Active.Clear();

	// Update the current custom maplist index
	RecordIndex = MapHandler.GetRecordIndex(GameIndex, co_Maplist.GetText());

	// Get the list of active maps from the MaplistManager
	NewActiveMaps = MapHandler.GetMapList(GameIndex, RecordIndex);

	for ( i = 0; i < NewActiveMaps.Length; i++ )
		AddMap(NewActiveMaps[i]);

	li_Active.bNotify = True;
	li_Avail.bNotify = True;
}

// Fill the custom maplist selection combo with the custom maplists for this gametype
function RefreshMaplistNames(optional string CurrentMaplist)
{
	local int i, Index, Current;
	local array<string> Ar;

	Index = MapHandler.GetGameIndex(CurrentGameType.ClassName);
	Ar = MapHandler.GetMapListNames(Index);

	Current = MapHandler.GetRecordIndex(Index, CurrentMaplist);
	if ( Current == -1 )
		Current = MapHandler.GetActiveList(Index);

	// Disable OnChange() calls
	co_Maplist.List.bNotify = False;
	co_Maplist.List.Clear();

	for ( i = 0; i < Ar.Length; i++ )
		co_Maplist.AddItem(Ar[i]);

	co_Maplist.List.bNotify = True;
	CurrentMaplist = MapHandler.GetMaplistTitle(Index, Current);

	co_Maplist.SetText(CurrentMaplist, True);
}

// =====================================================================================================================
// =====================================================================================================================
//  Utility functions - handles all special stuff that should happen whenever events are received on the page
// =====================================================================================================================
// =====================================================================================================================
event Closed(GUIComponent Sender, bool bCancelled)
{
	local int i;

	// Save and apply any changes that were made to the maplist
	StoreMaplists();

	//set the selected map on the main panel's maplist to the first map in this maplist so that if the user
	//starts the game immediately after closing this page, he will always start on the first map in the list
	if (MainPanel != None)
	{
		i = MainPanel.li_Maps.FindIndexByValue(li_Active.GetValueAtIndex(0));
		if (i != -1)
			MainPanel.li_Maps.SetIndex(i);
	}

	Super.Closed(Sender, bCancelled);
}

// Mapname has value only when initializing list
function bool AddMap(optional string MapName)
{
	local int i, j;
	local array<int> Indexes;
	local array<GUITreeNode> PendingElements;
	local MaplistRecord.MapItem Item;


	if ( MapName != "" )
	{
		li_Avail.bNotify = False;

		Indexes = li_Avail.GetIndexList();

		// TODO This will only work reliably if there are no additional command line paramters on the map's name
		for ( i = 0; i < Indexes.Length; i++ )
			if ( li_Avail.ValidSelectionAt(Indexes[i]) && MapName ~= li_Avail.GetValueAtIndex(Indexes[i]) )
				break;

		if ( i < Indexes.Length )
		{
			PendingElements[0] = li_Avail.Elements[Indexes[i]];
//;;		li_Avail.RemoveItemAt(Indexes[i],True);  un-hack

			// unhack --
			i = li_Active.FindIndexByValue(PendingElements[0].Value);
			if  ( i == -1 )
			{
				class'MaplistRecord'.static.CreateMapItem(PendingElements[0].Value, Item);
				for ( i = 0; i < Item.Options.Length; i++ )
				{
					if ( Item.Options[i].Key ~= "LinkSetup" )
					{
						li_Active.AddItem(Item.MapName @ "-" @ Item.Options[i].Value, Item.FullURL,,True);
						break;
					}
				}

				if ( i == Item.Options.Length )
				{
					if ( CurrentGameType.MapPrefix ~= "ONS" )
						li_Active.AddItem(Item.MapName @ "-" @ AutoSelectText, Item.MapName,,True);
					else
						li_Active.AddItem( Item.MapName, Item.MapName, , True);
//;;			li_Active.AddItem( PendingElements[0].Value, PendingElements[0].Value, PendingElements[0].ParentCaption, True );
				}
			}
			else li_Active.SilentSetIndex(i);
			// -- unhack

			// do not call call MapHandler.AddMap() since MapName only has a value when adding maps FROM maphandler
		}
		else
		{
			log("AddMap() didn't find map named"@MapName,'MapHandler');
			MapHandler.RemoveMap(GameIndex,RecordIndex,MapName);
		}
	}

	else
	{
		if ( !li_Avail.IsValid() )
			return false;

		li_Avail.bNotify = False;
		PendingElements = li_Avail.GetPendingElements(True);
		for ( i = 0; i < PendingElements.Length; i++ )
		{
			if ( li_Avail.ValidSelection() )
			{
				j = li_Active.FindIndexByValue(PendingElements[i].Value);//;;--
				if ( j == -1 )//;;--
				{
					class'MaplistRecord'.static.CreateMapItem(PendingElements[i].Value, Item);//;;--
					for ( j = 0; j < Item.Options.Length; j++ )//;;--
					{
						if ( Item.Options[j].Key ~= "LinkSetup" )//;;--
						{
							li_Active.AddItem(Item.MapName @ "-" @ Item.Options[j].Value, Item.FullURL,,True);//;;--
							break;//;;--
						}
					}
					if ( j == Item.Options.Length )
					{
						li_Active.AddItem( Item.MapName, Item.MapName, , True);
					}
				}
				else li_Active.SilentSetIndex(j);//;;--
//;;			li_Avail.RemoveElement(PendingElements[i],,True);
//;;			li_Active.AddItem(PendingElements[i].Caption, PendingElements[i].Value, PendingElements[i].ParentCaption, True);

				MapHandler.AddMap(GameIndex, RecordIndex, PendingElements[i].Value);
			}
		}
	}

	li_Avail.bNotify = True;
	li_Avail.ClearPendingElements();
	li_Avail.SetIndex(li_Avail.Index);

	return true;
}

function bool RemoveMap()
{
	local int i;
	local array<GUITreeNode> PendingElements;

	if ( !li_Active.IsValid() )
		return false;

	li_Active.bNotify = False;
	PendingElements = li_Active.GetPendingElements( True );
	for ( i = 0; i < PendingElements.Length; i++ )
	{
		if ( li_Active.ValidSelection() )
		{
			li_Active.RemoveElement(PendingElements[i],,True);
//;;		li_Avail.AddElement(PendingElements[i]);
			MapHandler.RemoveMap( GameIndex, RecordIndex, PendingElements[i].Value );
		}
	}

	li_Active.bNotify = True;
	li_Active.ClearPendingElements();
	li_Active.SetIndex(li_Active.Index);

	return true;
}

function bool ButtonPreDraw(Canvas C)
{
	local float X,W,BW,L;

	W = sb_MapList.ActualWidth() - sb_MapList.ImageOffset[0] - sb_MapList.ImageOffset[2];
	X = W - sb_MapList.ImageOffset[2];

	BW = b_New.ActualWidth() + b_Rename.ActualWidth() + b_Delete.ActualWidth() + 6;

	L = X - BW;
	b_New.WinLeft=L;
	L+= b_New.ActualWidth()+3;
	b_Rename.WinLeft=L;
	L+= b_Rename.ActualWidth()+3;
	b_Delete.WinLeft=L;

    b_New.WinTop=0.180846;
    b_Rename.WinTop=0.180846;
    b_Delete.WinTop=0.180846;

	co_MapList.WinLeft = sb_MapList.ActualLeft() + sb_MapList.ImageOffset[0];
	co_MapList.WinTop=0.125467;
	co_MapList.WinWidth=0.802485;

	W = (sb_Avail.WinWidth / 2)-(sb_Avail.WinWidth*0.02);
	b_AddAll.WinLeft = sb_Avail.WinLeft;
	b_AddAll.WinWidth = W;
	b_Add.WinLeft = sb_Avail.WinLeft + sb_Avail.WinWidth - W;
	b_Add.WinWidth = w;

	W = (sb_Active.WinWidth / 2)-(sb_Active.WinWidth*0.02);
	b_MoveUp.WinLeft = sb_Active.WinLeft;
	b_MoveUp.WinWidth = w;
	b_Remove.WinLeft = sb_Active.WinLeft;
	b_Remove.WinWidth = w;
	b_MoveDown.WinLeft = sb_Active.WinLeft + sb_Active.WinWidth - w;
	b_MoveDown.WinWidth = w;
	b_RemoveAll.WinLeft = sb_Active.WinLeft + sb_Active.WinWidth - w;
	b_RemoveAll.WinWidth = w;

	return false;
}

function RenameMaplist( optional bool bCancelled )
{
	local string str;

	str = Controller.ActivePage.GetDataString();
	if ( !bCancelled && str != "" )
	{
		UpdateCustomMaplist(str);

		// Reload the maplist names in case we renamed the maplist
		RefreshMaplistNames(str);
	}
}

function CreateNewMaplist( optional bool bCancelled )
{
	local string str, warning;
	local array<string> Ar;

	str = Controller.ActivePage.GetDataString();

	if ( !bCancelled && str != "" )
	{
		// Build an array of strings containing the active maps
		if ( MapHandler.GetDefaultMaps(CurrentGameType.MapListClassName, Ar) && Ar.Length > 0 )
		{
			// Since we are creating a new list, instead of changing this one, reset the old one
			RecordIndex = MapHandler.AddList(CurrentGameType.ClassName, str, Ar);

			// Reload maplist names, set the editbox's text to the new maplist's title
			RefreshMaplistNames(Str);
		}
		else
		{
			warning = Repl(InvalidMaplistClassText, "%name%", str);
			warning = Repl(warning, "%game%", CurrentGameType.ClassName);
			warning = Repl(warning, "%mapclass%", CurrentGameType.MaplistClassName);
			warn( warning );
		}
	}
}

// =====================================================================================================================
// =====================================================================================================================
//  OnClick's
// =====================================================================================================================
// =====================================================================================================================

// Called when one of the buttons between the maplists are clicked on
singular function bool ModifyMapList(GUIComponent Sender)
{
	local int Index, NewIndex;
	local string Str;

	if ( Sender == lb_AllMaps || Sender == li_Avail )
	{
		if ( li_Avail.ValidSelection() )
			AddMap();
		else li_Avail.InternalDblClick(li_Avail);

		return true;
	}

	if ( Sender == lb_ActiveMaps || Sender == li_Active )
	{
		if ( li_Active.ValidSelection() )
			RemoveMap();
		else li_Active.InternalDblClick(li_Active);

		return true;
	}

	if ( GUIButton(Sender) == None )
		return false;

	switch ( Sender )
	{
	case b_Add:
		return AddMap();

	case b_AddAll:
		if (lb_AllMaps.ItemCount()==0)
			return true;

		// watch out for low flying hacks

		li_Avail.bNotify = False;//;;--
		li_Active.bNotify = False;//;;--

		for ( Index = 0; Index < li_Avail.ItemCount; Index++ )
		{
			if ( li_Avail.ValidSelectionAt(Index) )
			{
				li_Avail.SilentSetIndex(Index); //;;--
				AddMap();//;;--
//;;			MapHandler.AddMap( GameIndex, RecordIndex, li_Avail.GetValueAtIndex(Index) );
			}
		}

		li_Avail.bNotify = True; //;;--
		li_Active.bNotify = True;//;;--

//;;	li_Active.LoadFrom(li_Avail);
//;;	li_Avail.Clear();

		return true;

	case b_Remove:
		return RemoveMap();

	case b_RemoveAll:
		if ( lb_ActiveMaps.ItemCount()==0 )
			return true;

		li_Avail.bNotify = False;//;;--
		li_Active.bNotify = False;//;;--

		for ( Index = 0; Index < li_Active.ItemCount; Index++ )
			if ( li_Active.ValidSelectionAt(Index) )
				MapHandler.RemoveMap( GameIndex, RecordIndex, li_Active.GetValueAtIndex(Index) );

		li_Active.Clear(); //;;--

		li_Avail.bNotify = True;
		li_Active.bNotify = True;
//;;		li_Avail.LoadFrom(li_Active);
//;;		li_Active.Clear();

		return true;

	case b_MoveUp:
		if ( !li_Active.IsValid() )
			return true;

		Index = li_Active.Index;
		Str = li_Active.GetValue();
		if ( Index > 0 && li_Active.Swap(index,index-1) )
		{
			NewIndex = li_Active.FindIndexByValue(str);
			li_Active.SetIndex(NewIndex);

			MapHandler.ShiftMap(GameIndex, RecordIndex, Str, -1);
		}

		return true;

	case b_MoveDown:
		if ( !li_Active.IsValid() )
			return true;

		Index = li_Active.Index;
		Str = li_Active.GetValue();

		if ( Index < li_Active.ItemCount - 1 && li_Active.Swap(Index, Index+1) )
		{
			NewIndex = li_Active.FindIndexByValue(Str);

			li_Active.SetIndex(NewIndex);
			MapHandler.ShiftMap(GameIndex, RecordIndex, Str, 1);
		}

		return true;
	}

	return false;
}

// Called when one of the custom maplist management buttons are clicked
function bool CustomMaplistClick(GUIComponent Sender)
{
	local string Str;

	switch ( Sender )
	{
		case b_New:
			if ( Controller.OpenMenu(Controller.RequestDataMenu, NewMaplistPageCaption, MaplistEditCaption) )
				Controller.ActivePage.OnClose = CreateNewMaplist;
			break;

		case b_Delete:

			// Remove the currently loaded maplist
			RecordIndex = MapHandler.RemoveList(GameIndex, RecordIndex);

			// Get the name of the now-current maplist
			Str = MapHandler.GetMapListTitle(GameIndex, RecordIndex);

			// Reload maplist names, set the editbox's text to the new maplist's title
			RefreshMaplistNames(Str);
			ReloadAvailable();
			ReloadActive();
			break;

		case b_Rename:
			if ( Controller.OpenMenu(Controller.RequestDataMenu, RenameMaplistPageCaption, MaplistEditCaption) )
			{
				Controller.ActivePage.SetDataString( co_Maplist.Get() );
				Controller.ActivePage.OnClose = RenameMaplist;
			}
			break;
	}

	return true;
}
// =====================================================================================================================
// =====================================================================================================================
//  OnChange's
// =====================================================================================================================
// =====================================================================================================================

// Called when we've selected a new item in the custom maplist combo, or typed in a new name
function MaplistSelectChange(GUIComponent Sender)
{
	ReloadActive();
}

// =====================================================================================================================
// =====================================================================================================================
//  Misc. Events
// =====================================================================================================================
// =====================================================================================================================

// Called when a map is dragged from the inactive list to the active list
function bool AddDragDrop(GUIComponent Sender)
{
	local array<GUITreeNode> Ar;
	local int i, idx;

	if ( Sender == li_Active )
	{
		idx = li_Active.DropIndex;
		if ( !li_Active.IsValidIndex(idx) )
			idx = li_Active.ItemCount;

		if ( Controller.DropSource == li_Avail )
			Ar = li_Avail.GetPendingElements();

		else if ( Controller.DropSource == li_Active )
		{

			Ar = li_Active.GetPendingElements();
			for ( i = 0; i < Ar.Length; i++ )
			{
				if ( Ar[i].Value == "" )
					continue;

				MapHandler.RemoveMap(GameIndex, RecordIndex, Ar[i].Value);
				li_Active.RemoveElement(Ar[i]);
			}
		}
		// Always insert in reverse order to maintain correct map order
		for ( i = Ar.Length - 1; i >= 0; i-- )
		{
			if ( Ar[i].Value == "" )
			{
				Ar.Remove(i,1);
				continue;
			}

			MapHandler.InsertMap(GameIndex, RecordIndex, Ar[i].Value, idx);
		}

		li_Active.InternalOnDragDrop(li_Active);
		return true;
	}

	return false;
}

// Called when maps are dragged from the active list to the inactive list.
function bool RemoveDragDrop(GUIComponent Sender)
{
	local array<GUITreeNode> Ar;
	local int i;

	if ( Sender == li_Avail )
	{
		if ( Controller.DropSource != li_Active )
			return false;

		Ar = li_Active.GetPendingElements();
		for (i = 0; i < Ar.Length; i++)
		{
			if ( Ar[i].Value == "" )
				continue;

			MapHandler.RemoveMap(GameIndex, RecordIndex, Ar[i].Value);
		}

		return li_Avail.InternalOnDragDrop(Sender);
	}

	return false;
}

function AddMapOption( string MapName, string OptionName, optional string Value )
{

}

// This function overrides GUIList default behavior because we only want to disable the AddAll and RemoveAll
// if those lists are empty
function InternalCheckLinkedObj( GUIListBase List )
{
	if ( List.IsValid() )
		List.EnableLinkedObjects();
	else List.DisableLinkedObjects();

	if ( li_Avail.ItemCount > 0 )
		EnableComponent(b_AddAll);
	else DisableComponent(b_AddAll);

	if ( li_Active.ItemCount > 0 )
		EnableComponent(b_RemoveAll);
	else DisableComponent(b_RemoveAll);

	if ( li_Active.Index == 0 )
		DisableComponent(b_MoveUp);
	else if ( li_Active.Index == li_Active.ItemCount - 1 )
		DisableComponent(b_MoveDown);
}

function InternalOnCreateComponent(GUIComponent NewComp, GUIComponent Sender)
{
	if ( moButton(Sender) != None && GUILabel(NewComp) != None )
	{
		GUILabel(NewComp).TextColor = WhiteColor;
		moButton(Sender).InternalOnCreateComponent(NewComp, Sender);
	}

	Super.InternalOnCreateComponent(NewComp,Sender);
}

// =====================================================================================================================
// =====================================================================================================================
//  Custom Maplist Interface
// =====================================================================================================================
// =====================================================================================================================

function StoreMaplists()
{
	local int i, idx;

	// Apply the currently selected maplist to the real game's maplist
	MapHandler.ApplyMaplist(GameIndex, RecordIndex);
	for ( i = 0; i < co_Maplist.ItemCount(); i++ )
	{
		// Now save all the rest of the maplists
		idx = MapHandler.GetRecordIndex(GameIndex, co_Maplist.List.GetItemAtIndex(i));
		if ( idx != RecordIndex )
			MapHandler.SaveMapList(GameIndex,idx);
	}
}

// Pass in NewName to rename the list
function UpdateCustomMaplist(optional string NewName)
{
	if ( NewName != "" )
		RecordIndex = MapHandler.RenameList(GameIndex, RecordIndex, NewName);

	// If we're saving this gametype's active list, be sure to apply the changes to the real maplist
	MapHandler.SaveMapList(GameIndex, RecordIndex);
}

function string GetMapPrefix()
{
    return CurrentGameType.MapPrefix;
}

function string GetMapListClass()
{
    return CurrentGameType.MapListClassName;
}

function int FindCacheRecordIndex(string MapName)
{
    local int i;

    for (i = 0; i < Maps.Length; i++)
        if (Maps[i].MapName == MapName)
            return i;

    return -1;
}

event Free()
{
	MapHandler = None;
	MainPanel = None;
	Super.Free();
}

DefaultProperties
{
	bCaptureInput=True

// =====================================================================================================================
// =====================================================================================================================
//  Maplist Components
// =====================================================================================================================
// =====================================================================================================================
	Begin Object Class=GUISectionBackground Name=ActiveBackground
		Caption="Active Maps"
		bBoundToParent=True
		bScaleToParent=True
		WinWidth=0.465432
		WinHeight=0.564917
		WinTop=0.261835
		WinLeft=0.511243
		BottomPadding=0.0
		bFillClient=True
	End Object
	sb_Active=ActiveBackground

	Begin Object class=AltSectionBackground name=MapListSectionBackground
		WinWidth=0.943100
		WinHeight=0.190595
		WinLeft=0.023646
		WinTop=0.055162
	End Object
	sb_MapList=MapListSectionBackground

	Begin Object Class=GUISectionBackground Name=InactiveBackground
	    Caption="Inactive Maps"
		WinWidth=0.465432
		WinHeight=0.564917
		WinLeft=0.034914
		WinTop=0.261835
	    bBoundToParent=True
	    bScaleToParent=True
	    BottomPadding=0.0
	    bFillClient=True
	End Object
	sb_Avail=InactiveBackground

	Begin Object Class=GUIComboBox Name=SelectMaplistCombo
		Hint="Load a existing custom maplist"
		OnChange=MaplistSelectChange
		WinWidth=0.441384
		WinHeight=0.045083
		WinLeft=0.471550
		WinTop=0.109808
		TabOrder=0
		bReadOnly=True
	End Object
    co_Maplist=SelectMaplistCombo

	Begin Object Class=GUIButton Name=NewMaplistButton
		Caption="NEW"
		Hint="Create a new custom maplist"
		OnClick=CustomMaplistClick
		WinWidth=0.123020
		WinHeight=0.056312
		WinLeft=0.060671
		WinTop=0.102551
		TabOrder=1
		bAutoSize=True
	End Object
    b_New=NewMaplistButton

	Begin Object Class=GUIButton Name=RenameMaplistButton
		Caption="RENAME"
		Hint="Rename the currently selected maplist"
		OnClick=CustomMaplistClick
		WinWidth=0.123020
		WinHeight=0.056312
		WinLeft=0.189348
		WinTop=0.102551
		TabOrder=2
		bAutoSize=True
	End Object
    b_Rename=RenameMaplistButton

	Begin Object Class=GUIButton Name=DeleteMaplistButton
		Caption="DELETE"
		Hint="Delete the currently selected maplist.  If this is the last maplist for this gametype, a new default maplist will be generated."
		OnClick=CustomMaplistClick
		WinWidth=0.123020
		WinHeight=0.056312
		WinLeft=0.318024
		WinTop=0.102551
		TabOrder=3
		bAutoSize=True
		OnPreDraw=ButtonPreDraw
	End Object
    b_Delete=DeleteMaplistButton

	Begin Object Class=GUITreeListBox Name=InactiveMaps
		WinWidth=0.380394
		WinHeight=0.662671
		WinLeft=0.113794
		WinTop=0.138078
		bVisibleWhenEmpty=true
		bSorted=True
		TabOrder=4
	End Object
	lb_AllMaps=InactiveMaps

	Begin Object Class=GUIButton Name=AddAllButton
		Caption="Add All"
		Hint="Add all maps to your map list"
		WinWidth=0.190232
		WinLeft=0.045006
		WinTop=0.852538
		WinHeight=0.056312
		bScaleToParent=true
		OnClick=ModifyMapList
		OnClickSound=CS_Up
		TabOrder=5
	End Object
	b_AddAll=AddAllButton

	Begin Object Class=GUIButton Name=AddButton
		Caption="Add"
		Hint="Add the selected maps to your map list"
		WinWidth=0.203807
		WinLeft=0.263743
		WinTop=0.852538
		WinHeight=0.056312
		bScaleToParent=true
		OnClick=ModifyMapList
		OnClickSound=CS_Up
		TabOrder=6
		bRepeatClick=True
	End Object
	b_Add=AddButton

	Begin Object Class=GUITreeListBox Name=ActiveMaps
		WinWidth=0.368359
		WinLeft=0.605861
		WinTop=0.108021
		bVisibleWhenEmpty=true
		TabOrder=7
	End Object
	lb_ActiveMaps=ActiveMaps

	Begin Object Class=GUIButton Name=MoveDownButton
		Caption="Down"
		Hint="Move this map lower down in the list"
		WinWidth=0.191554
		WinLeft=0.543747
		WinTop=0.852538
		WinHeight=0.056312
		OnClick=ModifyMapList
		bScaleToParent=true
		OnClickSound=CS_Down
		TabOrder=8
		bRepeatClick=True
	End Object
	b_MoveDown=MoveDownButton

	Begin Object Class=GUIButton Name=MoveUpButton
		Caption="Up"
		Hint="Move this map higher up in the list"
		WinWidth=0.191554
		WinLeft=0.772577
		WinHeight=0.056312
		bScaleToParent=true
		WinTop=0.852538
		OnClick=ModifyMapList
		OnClickSound=CS_Up
		TabOrder=9
		bRepeatClick=True
	End Object
	b_MoveUp=MoveUpButton

	Begin Object Class=GUIButton Name=RemoveButton
		Caption="Remove"
		Hint="Remove the selected maps from your map list"
		WinWidth=0.191554
		WinLeft=0.543747
		WinTop=0.898111
		WinHeight=0.056312
		OnClick=ModifyMapList
		OnClickSound=CS_Down
		TabOrder=10
		AutoSizePadding=(HorzPerc=0.5,VertPerc=0.0)
		bScaleToParent=true
		bRepeatClick=True
	End Object
	b_Remove=RemoveButton

	Begin Object Class=GUIButton Name=RemoveAllButton
		Caption="Remove All"
		Hint="Remove all maps from your map list"
		WinWidth=0.191554
		WinLeft=0.772577
		WinTop=0.898111
		WinHeight=0.056312
		OnClick=ModifyMapList
		OnClickSound=CS_Down
		TabOrder=11
		bScaleToParent=true
	End Object
	b_RemoveAll=RemoveAllButton

    InvalidMaplistClassText="Could not create new custom maplist %name% because %game% has an invalid maplist class: '%mapclass%'!"
    NewMaplistPageCaption="Create custom maplist"
	MaplistEditCaption="Maplist name: "
	RenameMaplistPageCaption="Rename maplist"
	WindowName="Maplist Configuration"

	AutoSelectText="Random"
	LinkText="Link Setup"

	InactiveFadeColor=(R=80,G=80,B=80,A=255)

	DefaultWidth=0.930313
	DefaultHeight=0.931305
	DefaultLeft=0.038125
	DefaultTop=0.021680


	WinWidth=0.930313
	WinHeight=0.931305
	WinLeft=0.038125
	WinTop=0.021680

	MinPageWidth=0.930313
	MinPageHeight=0.931305
	bResizeWidthAllowed=False
	bResizeHeightAllowed=False
}

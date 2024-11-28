//==============================================================================
//  Created on: 12/11/2003
//  Instant Action version of maplist tab
//  This version displays single maplist with preview images
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4Tab_MainSP extends UT2K4Tab_MainBase;

// Preview controls
var automated GUISectionBackground sb_Selection, sb_Preview, sb_Options;
var automated AltSectionBackground asb_Scroll;

var automated GUIScrollTextBox  lb_MapDesc;
var automated GUITreeListBox    lb_Maps;
var() editconst noexport GUITreeList       li_Maps;
var automated moButton	        b_Maplist;
var automated moButton          b_Tutorial;
var automated GUIImage          i_MapPreview, i_DescBack;
var automated GUILabel          l_MapAuthor, l_MapPlayers, l_NoPreview;

var() localized string MapCaption;

var config string LastSelectedMap; // Used to keep track of the map which was selected the last time we were in the menus

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
	local array<CacheManager.MapRecord> TutMaps;

    Super.InitComponent(MyController, MyOwner);

	if ( lb_Maps != None )
		li_Maps = lb_Maps.List;

	if ( li_Maps != None )
	{
	    li_Maps.OnDblClick = MapListDblClick;
	    li_Maps.bSorted = True;
	    lb_Maps.NotifyContextSelect = HandleContextSelect;
	}

    class'CacheManager'.static.GetMaplist(TutMaps, "TUT");

    TutorialMaps.Length = TutMaps.Length;
    for ( i = 0; i < TutMaps.Length; i++ )
    	TutorialMaps[i] = TutMaps[i].MapName;

	lb_Maps.bBoundToParent=false;
	lb_Maps.bScaleToParent=false;

	sb_Selection.ManageComponent(lb_Maps);

	asb_Scroll.ManageComponent(lb_MapDesc);

    sb_Options.ManageComponent(ch_OfficialMapsOnly);
    sb_Options.ManageComponent(b_Maplist);
    sb_Options.ManageComponent(b_Tutorial);

    InitMapHandler();
}


// Called when a new gametype is selected
function InitGameType()
{
    local int i;
    local array<CacheManager.GameRecord> Games;
    local bool bReloadMaps;

	// Get a list of all gametypes.
    class'CacheManager'.static.GetGameTypeList(Games);
	for (i = 0; i < Games.Length; i++)
    {
        if (Games[i].ClassName ~= Controller.LastGameType)
        {
        	bReloadMaps = CurrentGameType.MapPrefix != Games[i].MapPrefix;
            CurrentGameType = Games[i];
            break;
        }
    }

    if ( i == Games.Length )
    	return;

	// Update the gametype label's text
    SetGameTypeCaption();

    // Should the tutorial button be enabled?
    CheckGameTutorial();

    // Load Maps for the new gametype, but only if it uses a different maplist
    if (bReloadMaps)
   		InitMaps();

    // Set the selected map
    i = li_Maps.FindIndexByValue(LastSelectedMap);
    if ( i == -1 )
    	i = 0;
    li_Maps.SetIndex(i);
    li_Maps.Expand(i);

	// Load the information (screenshot, desc., etc.) for the currently selected map
//    ReadMapInfo(li_Maps.GetParentCaption());
}

// Query the CacheManager for the maps that correspond to this gametype, then fill the main list
function InitMaps( optional string MapPrefix )
{
    local int i, j;
    local bool bTemp;
    local string Package, Item;
    local GUITreeNode StoredItem;
    local DecoText DT;
    local array<string> CustomLinkSetups;

	// Make sure we have a map prefix
	if ( MapPrefix == "" )
		MapPrefix = GetMapPrefix();

	// Temporarily disable notification in all components
    bTemp = Controller.bCurMenuInitialized;
    Controller.bCurMenuInitialized = False;

	if ( li_Maps.IsValid() )
		li_Maps.GetElementAtIndex(li_Maps.Index, StoredItem);

	// Get the list of maps for the current gametype
	class'CacheManager'.static.GetMapList( CacheMaps, MapPrefix );
	if ( MapHandler.GetAvailableMaps(MapHandler.GetGameIndex(CurrentGameType.ClassName), Maps) )
	{
		li_Maps.bNotify = False;
		li_Maps.Clear();

		for ( i = 0; i < Maps.Length; i++ )
		{
			DT = None;
			if ( class'CacheManager'.static.IsDefaultContent(Maps[i].MapName) )
			{
				if ( bOnlyShowCustom )
					continue;
			}
			else if ( bOnlyShowOfficial )
				continue;

			j = FindCacheRecordIndex(Maps[i].MapName);
			if ( class'CacheManager'.static.Is2003Content(Maps[i].MapName) )
			{
				if ( CacheMaps[j].TextName != "" )
				{
					if ( !Divide(CacheMaps[j].TextName, ".", Package, Item) )
				{
						Package = "XMaps";
						Item = CacheMaps[j].TextName;
					}
				}

				DT = class'xUtil'.static.LoadDecoText(Package, Item);
			}

			if ( DT != None )
				li_Maps.AddItem( Maps[i].MapName, Maps[i].MapName, ,,JoinArray(DT.Rows, "|") );
			else li_Maps.AddItem( Maps[i].MapName, Maps[i].MapName, ,,CacheMaps[j].Description );

			// for now, limit this to power link setups only
			if ( CurrentGameType.MapPrefix ~= "ONS" )
			{
				if ( Maps[i].Options.Length > 0 )
				{
					Package = CacheMaps[j].Description;

					// Add the "auto link setup" item
					li_Maps.AddItem( AutoSelectText @ LinkText, Maps[i].MapName $ "?LinkSetup=Random", Maps[i].MapName,,Package );

					// Now add all official link setups
					for ( j = 0; j < Maps[i].Options.Length; j++ )
						li_Maps.AddItem(Maps[i].Options[j].Value @ LinkText, Maps[i].MapName $ "?LinkSetup=" $ Maps[i].Options[j].Value, Maps[i].MapName,,Package );
				}
			}

			if ( CurrentGameType.MapPrefix ~= "ONS" )
			{
				// Now to add the custom setups
				CustomLinkSetups = GetPerObjectNames(Maps[i].MapName, "ONSPowerLinkCustomSetup");
				for ( j = 0; j < CustomLinkSetups.Length; j++ )
					li_Maps.AddItem(CustomLinkSetups[j] @ LinkText, Maps[i].MapName $ "?" $ "LinkSetup=" $ CustomLinkSetups[j], Maps[i].MapName,,Package);
			}
		}
	}

	if ( li_Maps.bSorted )
		li_Maps.Sort();

	if ( StoredItem.Caption != "" )
	{
		i = li_Maps.FindFullIndex(StoredItem.Caption, StoredItem.Value, StoredItem.ParentCaption);
		if ( i != -1 )
			li_Maps.SilentSetIndex(i);
	}

	li_Maps.bNotify = True;

    Controller.bCurMenuInitialized = bTemp;
}

// =====================================================================================================================
// =====================================================================================================================
//  Utility functions - handles all special stuff that should happen whenever events are received on the page
// =====================================================================================================================
// =====================================================================================================================

// Update all components on the preview side with the data from the currently selected map
function ReadMapInfo(string MapName)
{
    local string mDesc;
    local int Index;

    if(MapName == "")
        return;

    if (!Controller.bCurMenuInitialized)
        return;

    Index = FindCacheRecordIndex(MapName);
    if (CacheMaps[Index].FriendlyName != "")
        asb_Scroll.Caption = CacheMaps[Index].FriendlyName;
    else
		asb_Scroll.Caption = MapName;

	UpdateScreenshot(Index);

	// Only show 1 number if min & max are the same
	if ( CacheMaps[Index].PlayerCountMin == CacheMaps[Index].PlayerCountMax )
		l_MapPlayers.Caption = CacheMaps[Index].PlayerCountMin @ PlayerText;
	else l_MapPlayers.Caption = CacheMaps[Index].PlayerCountMin@"-"@CacheMaps[Index].PlayerCountMax@PlayerText;

	mDesc = li_Maps.GetExtra();
    if (mDesc == "")
        mDesc = MessageNoInfo;

	lb_MapDesc.SetContent( mDesc );
    if (CacheMaps[Index].Author != "" && !class'CacheManager'.static.IsDefaultContent(CacheMaps[Index].MapName))
        l_MapAuthor.Caption = AuthorText$":"@CacheMaps[Index].Author;
    else l_MapAuthor.Caption = "";
}

// If this gametype has a tutorial, enable the tutorial button
function CheckGameTutorial()
{
	local int i;

	for ( i = 0; i < TutorialMaps.Length; i++ )
	{
		if ( Mid(TutorialMaps[i], InStr(TutorialMaps[i], "-") + 1) ~= CurrentGameType.GameAcronym )
		{
			EnableComponent(b_Tutorial);
			b_Tutorial.SetComponentValue(TutorialMaps[i],True);
			return;
		}
	}

	DisableComponent(b_Tutorial);
	b_Tutorial.SetComponentValue("",True);
}

function UpdateScreenshot(int Index)
{
	local Material Screenie;

	if ( Index >= 0 && Index < CacheMaps.Length )
	    Screenie = Material(DynamicLoadObject(CacheMaps[Index].ScreenshotRef, class'Material'));

    i_MapPreview.Image = Screenie;
    l_NoPreview.SetVisibility( Screenie == None );
    i_MapPreview.SetVisibility( Screenie != None );
}

event SetVisibility( bool bIsVisible )
{
	Super.SetVisibility(bIsVisible);

	if ( bIsVisible )
	{
	    l_NoPreview.SetVisibility( i_MapPreview.Image == None );
	    i_MapPreview.SetVisibility( i_MapPreview.Image != None );
	}
}

function SetGameTypeCaption()
{
    sb_Selection. Caption = CurrentGameType.GameName@MapCaption;
}

function string Play()
{
	return GetMapURL(li_Maps,-1);
}

function string GetMapURL( GUITreeList List, int Index )
{
	local string URL;

	URL = Super.GetMapURL(List,Index);
	if ( CurrentGameType.MapPrefix ~= "ONS" && InStr(Caps(URL),"?LINKSETUP=") == -1 )
		URL $= "?LinkSetup=Default";

	return URL;
}

// =====================================================================================================================
// =====================================================================================================================
//  OnClick's
// =====================================================================================================================
// =====================================================================================================================

function MaplistConfigClick( GUIComponent Sender )
{
	local MaplistEditor MaplistPage;

	// open maplist config page
	if ( Controller.OpenMenu(MaplistEditorMenu) )
	{
		MaplistPage = MaplistEditor(Controller.ActivePage);
		if ( MaplistPage != None )
		{
			MaplistPage.MainPanel = self;
			MaplistPage.bOnlyShowOfficial = bOnlyShowOfficial;
			MaplistPage.bOnlyShowCustom = bOnlyShowCustom;
			MaplistPage.Initialize(MapHandler);
		}
	}
}

// Called when a double click is received in the main maplist
function bool MapListDblClick(GUIComponent Sender)
{
	if ( li_Maps.ValidSelection() )
		return p_Anchor.InternalOnClick(p_Anchor.b_Primary);
	else
	{
		if ( CurrentGameType.MapPrefix ~= "ONS" )
		{
			if ( !li_Maps.IsToggleClick(li_Maps.Index) )
				return p_Anchor.InternalOnClick(p_Anchor.b_Primary);
		}
		else return li_Maps.InternalDblClick(Sender);
	}

    return true;
}

// Called when the "Watch Tutorial" button is clicked
function TutorialClicked( GUIComponent Sender )
{
	if ( Sender == b_Tutorial )
	{
		Play();
		PlayerOwner().ConsoleCommand("START"@b_Tutorial.GetComponentValue()$"?quickstart=true?TeamScreen=false");
		Controller.CloseAll(False,true);
	}
}

// =====================================================================================================================
// =====================================================================================================================
//  OnChange's
// =====================================================================================================================
// =====================================================================================================================

// Called when user clicks on a new map in the main maplist
function MapListChange(GUIComponent Sender)
{
	local MaplistRecord.MapItem Item;

    if (!Controller.bCurMenuInitialized)
        return;

	if ( Sender == lb_Maps )
	{
		if ( li_Maps.IsValid() )
		{
			EnableComponent(b_Primary);
			EnableComponent(b_Secondary);
		}

		class'MaplistRecord'.static.CreateMapItem(li_Maps.GetValue(), Item);

		LastSelectedMap = Item.FullURL;
		SaveConfig();
		ReadMapInfo(Item.MapName);
	}
}

// =====================================================================================================================
// =====================================================================================================================
//  Misc. Events
// =====================================================================================================================
// =====================================================================================================================

function InternalOnCreateComponent(GUIComponent NewComp, GUIComponent Sender)
{
	if ( moButton(Sender) != None && GUILabel(NewComp) != None )
	{
//		GUILabel(NewComp).TextColor = WhiteColor[3];
		moButton(Sender).InternalOnCreateComponent(NewComp, Sender);
	}
}

function bool HandleContextSelect(GUIContextMenu Sender, int Index)
{
	local string MapName;

	if ( Sender != None )
	{
		switch ( Index )
		{
		case 0:
		case 1:
			MapName = GetMapURL(li_Maps,-1);
			if (MapName != "")
			{
				p_Anchor.PrepareToPlay(MapName, MapName);
				p_Anchor.StartGame(MapName, Index == 1);
			}

			break;

		case 3:
			bOnlyShowOfficial = !bOnlyShowOfficial;
			InitMaps();
			ch_OfficialMapsOnly.SetComponentValue(bOnlyShowOfficial, True);
			break;
		}
	}

	return true;
}

function int FindCacheRecordIndex(string MapName)
{
    local int i;

    for (i = 0; i < CacheMaps.Length; i++)
        if (CacheMaps[i].MapName == MapName)
            return i;

    return -1;
}

defaultproperties
{
    Begin Object class=GUISectionBackground Name=SelectionGroup
		WinWidth=0.482149
		WinHeight=0.603330
		WinLeft=0.016993
		WinTop=0.018125
		Caption="Map Selection"
		bFillClient=true
    End Object
    sb_Selection=SelectionGroup

    Begin Object Class=GUITreeListBox Name=AvailableMaps
		WinWidth=0.422481
		WinHeight=0.449870
		WinLeft=0.045671
		WinTop=0.169272
        bVisibleWhenEmpty=true
        Hint="Click a mapname to see a preview and description.  Double-click to play a match on the map."
        StyleName="NoBackground"
        TabOrder=0
        OnChange=MapListChange
        bBoundToParent=false
        bScaleToParent=false
    End Object
    lb_Maps=AvailableMaps

    Begin Object class=GUISectionBackground Name=PreviewGroup
		WinWidth=0.470899
		WinHeight=0.974305
		WinLeft=0.515743
		WinTop=0.018125
		Caption="Preview"
		bFillClient=true
    End Object
    sb_Preview=PreviewGroup

    Begin Object Class=GUILabel Name=NoPreview
		WinWidth=0.372002
		WinHeight=0.357480
		WinLeft=0.562668
		WinTop=0.107691
        TextFont="UT2HeaderFont"
        TextAlign=TXTA_Center
        VertAlign=TXTA_Center
        bMultiline=True
        bTransparent=False
        TextColor=(R=247,G=255,B=0,A=255)
        Caption="No Preview Available"
    End Object
    l_NoPreview=NoPreview

    Begin Object Class=GUIImage Name=MapPreviewImage
		WinWidth=0.372002
		WinHeight=0.357480
		WinLeft=0.562668
		WinTop=0.107691
        ImageColor=(R=255,G=255,B=255,A=255)
        ImageStyle=ISTY_Scaled
        ImageRenderStyle=MSTY_Normal
        RenderWeight=0.2
    End Object
    i_MapPreview=MapPreviewImage

    Begin Object class=GUILabel Name=MapAuthorLabel
        Caption="Testing"
        TextAlign=TXTA_Center
        StyleName="TextLabel"
		WinWidth=0.453285
		WinHeight=0.032552
		WinLeft=0.522265
		WinTop=0.405278
        RenderWeight=0.3
    End Object
    l_MapAuthor=MapAuthorLabel

    Begin Object class=GUILabel Name=RecommendedPlayers
        Caption="Best for 4 to 8 players"
        TextAlign=TXTA_Center
        StyleName="TextLabel"
		WinWidth=0.445313
		WinHeight=0.032552
		WinLeft=0.521288
		WinTop=0.474166
       RenderWeight=0.3
    End Object
    l_MapPlayers=RecommendedPlayers

	Begin Object class=AltSectionBackground name=ScrollSection
		WinWidth=0.409888
		WinHeight=0.437814
		WinLeft=0.546118
		WinTop=0.525219
		Caption="Map Desc"
		bFillClient=true
	End Object
	asb_Scroll=ScrollSection

    Begin Object Class=GUIScrollTextBox Name=MapDescription
		WinWidth=0.379993
		WinHeight=0.268410
		WinLeft=0.561065
		WinTop=0.628421
        CharDelay=0.0025
        EOLDelay=0.5
        bNeverFocus=true
        StyleName="NoBackground"
        bTabStop=False
    End Object
    lb_MapDesc=MapDescription

	Begin Object Class=GUISectionBackground Name=OptionsGroup
		WinWidth=0.482149
		WinHeight=0.351772
		WinLeft=0.018008
		WinTop=0.642580
		Caption="Options"
		BottomPadding=0.07
	End Object
	sb_Options=OptionsGroup

    Begin Object Class=moCheckbox Name=FilterCheck
    	OnChange=ChangeMapFilter
		WinWidth=0.341797
		WinHeight=0.030035
		WinLeft=0.051758
		WinTop=0.772865
    	Caption="Only Official Maps"
    	Hint="Hides all maps not created by Epic or Digital Extremes."
    	TabOrder=1
    	bAutoSizeCaption=True
    	ComponentWidth=0.9
    	CaptionWidth=0.1
    	bSquare=True
    End Object
	ch_OfficialMapsOnly=FilterCheck

    Begin Object Class=moButton Name=MaplistButton
		WinWidth=0.341797
		WinHeight=0.05
		WinLeft=0.039258
		WinTop=0.888587
    	ButtonCaption="Maplist Configuration"
    	Hint="Modify the maps that should be used in gameplay"
    	OnChange=MaplistConfigClick
    	TabOrder=2
    	ComponentWidth=1.0
    End Object
    b_Maplist=MaplistButton

    Begin Object Class=moButton Name=TutorialButton
    	OnChange=TutorialClicked
		WinWidth=0.348633
		WinHeight=0.05
		WinLeft=0.556953
		WinTop=0.913326
//		OnCreateComponent=InternalOnCreateComponent
    	ButtonCaption="Watch Game Tutorial"
    	ButtonStyleName="SquareButton"
    	Hint="Watch the tutorial for this gametype."
    	ComponentWidth=1.0
    	TabOrder=3
    End Object
    b_Tutorial=TutorialButton

	MapCaption="Maps"

}

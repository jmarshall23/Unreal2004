//==============================================================================
//	Customized GUIPanel containing controls for manipulating custom filters.
//	This panel handles events from its controls, and passes the results to the
//	UT2K4CustomFilterPage for processing
//
//	Created by Ron Prestenback
//	� 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4FilterControlPanel extends GUIPanel;


var automated GUISectionBackground sb_Back;
var automated GUIMultiOptionListBox		lb_Filters;
var GUIMultiOptionList					li_Filters;

var automated moCombobox	co_GameType;
var automated GUIButton		b_CreateF, b_RemoveF, b_Close;

var automated GUIImage		i_BG;
var automated GUILabel		l_FilterNames;

var FilterPageBase p_Anchor;

var localized string CopyText;
var localized string CreateFilterCaption;
var localized string RenameText;
var localized string FilterNameCaption;


function InitComponent(GUIController MyC, GUIComponent MyO)
{
	Super.InitComponent(MyC, MyO);

	sb_Back.ManageComponent(lb_Filters);

	lb_Filters.OnClick = FilterListClicked;
	li_Filters = lb_Filters.List;
	li_Filters.ItemScaling = 0.04;
	li_Filters.ItemPadding = 0.3;

	li_Filters.AddLinkObject(b_RemoveF);

	co_GameType.MyComboBox.MyListBox.List.LoadFrom(UT2K4ServerBrowser(p_Anchor.ParentPage).co_GameType.MyCombobox.MyListBox.List, True);
	co_GameType.SetIndex(UT2K4ServerBrowser(p_Anchor.ParentPage).co_GameType.GetIndex());
	co_GameType.ReadOnly(True);
}

function bool FilterListClicked(GUIComponent Sender)
{
	InternalOnChange(li_Filters);
	return true;
}

function bool InternalOnClick(GUIComponent Sender)
{
	if (GUIButton(Sender) == None)
		return false;

	switch ( GUIButton(Sender).Caption )
	{
		case b_CreateF.Caption:		NewFilter();	break;
		case b_RemoveF.Caption:		RemoveFilter(); break;
	}

	return true;
}

function ContextClick(GUIContextMenu Sender, int ClickIndex)
{
	local int i;

	switch ( ClickIndex )
	{
	case 0:	// Copy
		CopyFilter();
		break;

	case 2:
	case 1:	// Rename
		RenameFilter();
		break;

	case 3:	// Reset
		p_Anchor.ResetFilters();
		break;

	case 4:
		li_Filters.bNotify = False;
		for ( i = li_Filters.ItemCount - 1; i >= 0; i-- )
		{
			if ( p_Anchor.FM.RemoveFilter(li_Filters.Elements[i].Caption) )
				li_Filters.RemoveItem(i);
		}

		li_Filters.bNotify = True;
		InternalOnChange(li_Filters);
		break;
	}
}

// Do not allow the context menu to appear if we weren't clicking on the list
function bool InternalOnRightClick(GUIComponent Sender)
{
	if ( !li_Filters.IsInBounds() || !li_Filters.IsValid() )
		return false;

	return true;
}

function NewFilter()
{
	if ( Controller.OpenMenu(Controller.RequestDataMenu, CreateFilterCaption, FilterNameCaption ) )
		Controller.ActivePage.OnClose = NewFilterClosed;
}

function CopyFilter()
{
	local GUIMenuOption op;

	op = li_Filters.Get();
	if ( op == None || op.Caption == "" )
		return;

	if ( Controller.OpenMenu(Controller.RequestDataMenu, CreateFilterCaption, FilterNameCaption) )
	{
		Controller.ActivePage.SetDataString(CopyText@op.Caption);
		Controller.ActivePage.OnClose = CopyFilterClosed;
	}
}

function RenameFilter()
{
	local GUIMenuOption op;

	op = li_Filters.Get();
	if ( op == None || op.Caption == "" )
		return;

	if (Controller.OpenMenu(Controller.RequestDataMenu, RenameText, FilterNameCaption) )
	{
		Controller.ActivePage.SetDataString(op.Caption);
		Controller.ActivePage.OnClose = RenameFilterClosed;
	}
}

function RemoveFilter()
{
	local GUIMenuOption op;

	op = li_Filters.Get();
	if ( op == None || op.Caption == "" )
		return;

	p_Anchor.RemoveExistingFilter(op.Caption);
}

function NewFilterClosed(optional bool bCancelled)
{
	local string s;

	if ( bCancelled )
		return;

	s = Controller.ActivePage.GetDataString();
	if ( s != "" )
		p_Anchor.AddNewFilter( s );
}

function RenameFilterClosed(optional bool bCancelled)
{
	local string s;

	if ( bCancelled )
		return;

	s = Controller.ActivePage.GetDataString();
	if ( s != "" )
		p_Anchor.RenameFilter(li_Filters.Index, s);
}

function CopyFilterClosed(optional bool bCancelled)
{
	local string s;
	if ( bCancelled )
		return;

	s = Controller.ActivePage.GetDataString();
	if ( s != "" )
		p_Anchor.CopyFilter( li_Filters.Index, s );
}


function InternalOnCreateComponent(GUIComponent NewComp, GUIComponent Sender)
{
	local moCheckbox cb;

	cb = moCheckBox(NewComp);
	if (cb != None)	// GUIMultiOptionList would be sender
	{
		cb.CaptionWidth = 0.4;
		cb.bFlipped = False;
		cb.ComponentJustification = TXTA_Center;
		cb.LabelJustification = TXTA_Center;

	}

	if (Sender == lb_Filters)
		lb_Filters.InternalOnCreateComponent(NewComp, Sender);
}

function InternalOnLoad(GUIComponent Sender, string S)
{
	local int i;

	if (moCheckBox(Sender) != None)
	{
		i = p_Anchor.FM.FindFilterIndex(moCheckBox(Sender).Caption);
		moCheckBox(Sender).Checked( p_Anchor.FM.IsActiveAt(i) );
	}
}

function InternalOnChange(GUIComponent Sender)
{
	if (li_Filters == Sender)
	{
		// Set button states
		if (li_Filters.Index < 0)
			NoItemSelected();

		else NewItemSelected();
	}

	else if (Sender == co_GameType)
	{
		// Update visible filter rules to correspond to currently selected gametype
		p_Anchor.FM.SetFilterInfo(co_GameType.GetExtra());
		p_Anchor.ApplyRules(p_Anchor.Index, True);
	}

	OnChange(Sender);	// Pass it on
}

protected function NoItemSelected()
{
	DisableComponent(b_RemoveF);
}

protected function NewItemSelected()
{
	EnableComponent(b_RemoveF);
}

DefaultProperties
{
	Begin Object Class=GUISectionBackground Name=BackgroundSec
        bFillClient=True
        Caption="Custom Filters"
        BottomPadding=0.100000
        WinTop=0.538846
        WinHeight=0.456507
	End Object
	sb_Back=BackgroundSec

	Begin Object Class=moCombobox Name=GameTypeCombo
		Caption="Game Type"
		Hint="Only rules for this gametype will be shown"
		WinWidth=0.957536
		WinHeight=27.000000
		WinLeft=0.029015
		WinTop=8.096001
		CaptionWidth=0.33
		RenderWeight=1
		bTabStop=True
		TabOrder=0
		ComponentJustification=TXTA_Right
		IniOption="@INTERNAL"
        OnChange=InternalOnChange
	End Object
	co_GameType=GameTypeCombo

	Begin Object Class=GUIMultiOptionListBox Name=FilterListBox
		OnCreateComponent=InternalOnCreateComponent
		OnLoadINI=InternalOnLoad
		OnChange=InternalOnChange
		bScaleToParent=True
		bBoundToParent=True
		WinWidth=1.000000
		WinHeight=0.787241
		WinLeft=0.000000
		WinTop=0.079581
		bVisibleWhenEmpty=True
		TabOrder=1
	End Object
	lb_Filters=FilterListBox

	Begin Object Class=GUIButton Name=RemoveFButton
		Caption="Remove"
		Hint="Permanently delete currently selected filter"
		bScaleToParent=True
		bBoundToParent=True
		WinWidth=0.300000
		WinHeight=0.050000
		WinLeft=0.189031
		WinTop=0.927996
		OnClick=InternalOnClick
		TabOrder=2
		MenuState=MSAT_Disabled
	End Object
	b_RemoveF=RemoveFButton

	Begin Object Class=GUIButton Name=CreateFButton
		Caption="New"
		Hint="Create new custom filter"
		bScaleToParent=True
		bBoundToParent=True
		WinWidth=0.302815
		WinHeight=0.050000
		WinLeft=0.533485
		WinTop=0.927996
		TabOrder=3
		OnClick=InternalOnClick
	End Object
	b_CreateF=CreateFButton

	Begin Object Class=GUIContextMenu Name=RCMenu
		ContextItems(0)="Clone Filter"
		ContextItems(1)="Rename Filter"
		ContextItems(2)="-"
		ContextItems(3)="Reset All Filters"
		ContextItems(4)="Remove All Filters"
		OnSelect=ContextClick
	End Object
	ContextMenu=RCMenu

	OnRightClick=InternalOnRightClick
	CopyText="Copy of"
	CreateFilterCaption="Create Filter"
	RenameText="Rename Filter"
	FilterNameCaption="Filter Name: "
}

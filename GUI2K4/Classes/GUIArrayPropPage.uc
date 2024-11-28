//==============================================================================
//	This page displays all values for array properties received from PlayInfo
//	TODO Add support for Select render type in playinfo
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class GUIArrayPropPage extends GUICustomPropertyPage;

var() string PropName;
var() array<string> PropValue;

var string MOType;

var automated GUIMultiOptionListBox lb_Values;
var() GUIMultiOptionList				li_Values;

var() string              Delim, ButtonStyle;
var protected 	bool	bReadOnly;
var() bool bListInitialized;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);
	li_Values = lb_Values.List;
}

function SetOwner( GUIComponent NewOwner )
{
	local string str;

	Super.SetOwner(NewOwner);

	PropName = Item.DisplayName;
	t_WindowTitle.Caption = PropName;

	str = Item.Value;
	// Remove extra () and ""
	Strip(str, "(");
	Strip(str, ")");

	if ( Delim == "" )
		Delim = ",";

	if (Left(str, 1) == "\"")
		Delim = "\"" $ Delim $ "\"";

	Strip(str, "\"");
	Split(str, Delim, PropValue);
}

function SetReadOnly( bool bValue )
{
	bReadOnly = bValue;
}

function bool GetReadOnly() { return bReadOnly; }

function string GetDataString()
{
	local string Result;

	Result = JoinArray( PropValue, Delim );

	if ( Left(Delim,1) == "\"" )
		Result = "\"" $ Result $ "\"";

	Result = "(" $ Result $ ")";

	return Result;
}

function bool InternalOnPreDraw(Canvas C)
{
	if ( !bListInitialized )
		InitializeList();

	return Super.InternalOnPreDraw(C);
}


// Create buttons and controls for array members
function InitializeList()
{
	local int i;

	if ( !li_Values.bPositioned )
		return;

	bListInitialized = True;
    if (Item.RenderType == PIT_Check)
        MOType = "XInterface.moCheckBox";

    else if (Item.RenderType == PIT_Select)
        MOType = "XInterface.moComboBox";

	Clear();
	for (i = 0; i < PropValue.Length; i++)
		AddListItem(i);

	UpdateListCaptions();
	UpdateListValues();
}

// Creates and sets up the menuoption for one array member
function GUIMenuOption AddListItem(int Index)
{
	local GUIMenuOption mo;

	mo = li_Values.InsertItem( Index, MOType, , string(Index+1) $ ":" );

	mo.CaptionWidth=0.05;
	mo.ComponentWidth=0.95;
	mo.bAutoSizeCaption = True;
	mo.SetReadOnly(bReadOnly);

	SetItemOptions(mo);
	return mo;
}

function Clear()
{
	li_Values.Clear();
}

// Resets the menuoption captions to correspond to the currently displayed members
function UpdateListCaptions()
{
	local int i;

	for (i = 0; i < li_Values.Elements.Length; i++)
		li_Values.Elements[i].SetCaption(i+1 $ ":");
}

// Resets the menuoption values to correspond to the currently displayed array members
function UpdateListValues()
{
	local int i;

	RemapComponents();
	for (i = 0; i < li_Values.Elements.Length && i < PropValue.Length; i++)
		li_Values.Elements[i].SetComponentValue(PropValue[i],True);
}

function InternalOnCreateComponent(GUIComponent NewComp, GUIComponent Sender)
{
	if (GUIMultiOptionList(NewComp) != None)
	{
		GUIMultiOptionList(NewComp).bDrawSelectionBorder = False;
		GUIMultiOptionList(NewComp).ItemPadding = 0.15;

		if (Sender == lb_Values)
			lb_Values.InternalOnCreateComponent(NewComp, Sender);
	}

	else if (GUIButton(NewComp) != None)
	{
		GUIButton(NewComp).StyleName = ButtonStyle;
		GUIButton(NewComp).bAutoSize = True;
	}

	Super.InternalOnCreateComponent(NewComp,Sender);
}

function InternalOnChange(GUIComponent Sender)
{
	if (Sender == li_Values )
	{
		if ( li_Values.IsValid() )
			PropValue[li_Values.Index] = li_Values.Get().GetComponentValue();
	}
}

function int GetMaxValue( string MaxLength )
{
	local int i, maxl;
	local string str;

	if ( MaxLength == "" )
		return 0;

	maxl = int(MaxLength);
	for ( i = 0; i < maxl; i++ )
		str $= "9";

	return int(str);
}

function SetItemOptions( GUIMenuOption mo )
{
	local moNumericEdit nu;
	local moFloatEdit fl;
	local moEditBox ed;

	local string str, str1, str2;

	nu = moNumericEdit(mo);
	fl = moFloatEdit(mo);
	ed = moEditBox(mo);

	if ( ed != None )
	{
		if ( Item.Data != "" )
			ed.MyEditBox.MaxWidth = int(Item.Data);
	}

	else if ( fl != None )
	{
		if ( Item.Data != "" )
		{
			if ( Divide(Item.Data, ";", str, str1) )
			{
				fl.MyNumericEdit.MyEditBox.MaxWidth = int(str);
				if ( Divide(str1, ":", str, str2) )
					fl.Setup(str, str2, fl.Step);
			}
			else fl.Setup(0, GetMaxValue(Item.Data), fl.Step);
		}
	}

	else if ( nu != None )
	{
		if ( Item.Data != "" )
		{
			if ( Divide(Item.Data, ";", str, str1) )
			{
				nu.MyNumericEdit.MyEditBox.MaxWidth = int(str);
				if ( Divide(str1, ":", str, str2) )
					nu.Setup(str, str2, fl.Step);
			}
			else nu.Setup(0, GetMaxValue(Item.Data), nu.Step);
		}
	}
}

DefaultProperties
{
	WinWidth=0.989258
	WinLeft=0.005859
	WinTop=0.2
	WinHeight=0.5

	OnCreateComponent=InternalOnCreateComponent
	Begin Object Class=GUIMultiOptionListBox Name=ValueListBox
		NumColumns=1
		OnChange=InternalOnChange
		OnCreateComponent=InternalOnCreateComponent
		bBoundToParent=True
		bScaleToParent=True
		WinWidth=0.865001
		WinHeight=0.714452
		WinLeft=0.021250
		WinTop=0.140209
		TabOrder=0
		bVisibleWhenEmpty=True
	End Object
	lb_Values=ValueListBox

	ButtonStyle="SquareButton"
	MOType="XInterface.moEditBox"
}

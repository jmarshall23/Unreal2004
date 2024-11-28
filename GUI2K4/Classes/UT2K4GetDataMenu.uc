//==============================================================================
//	Created on: 10/05/2003
//	This menu contains an editbox for entering a single data item
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4GetDataMenu extends UT2K4GenericMessageBox;

var automated GUIButton b_Cancel;
var automated moEditbox ed_Data;

function InitComponent( GUIController MyController, GUIComponent MyOwner )
{
	ed_Data.TabOrder = 0;
	b_OK.TabOrder = 1;
	b_Cancel.TabOrder = 2;

	Super.InitComponent( MyController, MyOwner );

	RemoveComponent(l_Text2);

	b_Ok.bBoundtoParent=true;
	b_Ok.bStandardized=true;
	b_Ok.bScaletoPArent=true;
	b_Cancel.bBoundtoParent=true;
	b_Cancel.bStandardized=true;
	b_Cancel.bScaletoPArent=true;

}

function HandleParameters( string A, string B )
{
	l_Text.Caption = A;
	ed_Data.SetCaption(B);
}

function string GetDataString()
{
	return ed_Data.GetText();
}

function SetDataString( string Str )
{
	Super.SetDataString(Str);
	ed_Data.SetText(Str);
}

function bool InternalOnClick( GUIComponent Sender )
{
	Controller.CloseMenu( Sender == b_Cancel );
	return true;
}

function bool InternalOnKeyEvent( out byte Key, out byte State, float Delta )
{
	if ( State == 3 && Key == 0x0B )	// Escape
		return InternalOnClick(b_Cancel);

	return Super.InternalOnKeyEvent(Key,State,Delta);
}

function bool InternalOnPreDraw( Canvas C )
{
	local bool b;
	local float w,l,t;
	b = super.InternalOnPreDraw(c);

	t = 1 - ( 64/ActualHeight() ) - b_Cancel.WinHeight;
	w = 0.15 + (b_Cancel.WinWidth*2);
	l = 0.5 - (w*0.5);

	b_Cancel.SetPosition(l,t,b_Cancel.WinWidth,b_Cancel.WinHeight);
	l+= 0.15 + b_Cancel.WinWidth;
	b_Ok.SetPosition(l,t,b_Cancel.WinWidth,b_Cancel.WinHeight);

	return b;
}

DefaultProperties
{
	WinWidth=1.000000
	WinHeight=0.423438
	WinLeft=0.000000
	WinTop=0.289583

	Begin Object class=GUILabel Name=DialogText
		TextALign=TXTA_Center
		StyleName="TextButton"
		FontScale=FNS_Large
		WinWidth=1
		WinLeft=0
		WinTop=0.388281
		WinHeight=0.09375
	End Object
    l_Text=DialogText

	Begin Object Class=GUIButton Name=CancelButton
		OnClick=InternalOnClick
		WinWidth=0.131641
		WinHeight=0.047812
		WinLeft=0.573047
		WinTop=0.554167
		Caption="CANCEL"
		Hint="Close this menu, discarding changes."
	End Object
	b_Cancel=CancelButton

	Begin Object class=moEditBox Name=Data
		WinWidth=0.500000
		WinHeight=0.047305
		WinLeft=0.250000
		WinTop=0.487710
		CaptionWidth=0.4
		TabOrder=0
	End Object
	ed_Data=Data
}

//==============================================================================
//	Created on: 09/16/2003
//	Handles opening an IP directly
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Browser_OpenIP extends UT2K4GetDataMenu;

var localized string OKButtonHint;
var localized string CancelButtonHint;
var localized string EditBoxHint;

function InitComponent(GUIController pMyController, GUIComponent MyOwner)
{
	Super.InitComponent(pMyController, MyOwner);

	ed_Data.MyEditBox.OnKeyEvent = InternalOnKeyEvent;
	b_OK.SetHint(OKButtonHint);
	b_Cancel.SetHint(CancelButtonHint);
	ed_Data.SetHint(EditBoxHint);
}

function HandleParameters(string s, string s2)
{
	if (s != "")
		ed_Data.SetText( StripProtocol(s) );
}

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_OK )
		Execute();
	else Controller.CloseMenu(true);

	return true;
}

function Execute()
{
	local string URL;

	URL = ed_Data.GetText();
	if ( URL == "" )
		return;

	URL = StripProtocol(URL);
	if ( InStr( URL, ":" ) == -1 )
		URL $= ":7777";

	ApplyURL( URL );
}

function ApplyURL(string URL )
{
	if ( URL == "" || Left(URL,1) == ":" )
		return;

	PlayerOwner().ClientTravel( URL, TRAVEL_Absolute, false );
	Controller.CloseAll(false,True);
}

function bool InternalOnKeyEvent( out byte Key, out byte State, float Delta )
{
	if ( !Super.InternalOnKeyEvent(Key,State,Delta) )
		return ed_Data.MyEditBox.InternalOnKeyEvent(Key,State,Delta);
}

function string StripProtocol( string s )
{
	local string Protocol;

	Protocol = PlayerOwner().GetURLProtocol();

	ReplaceText(s, Protocol $ "://", "");
	ReplaceText(s, Protocol, "");

	return s;
}

defaultproperties
{
	OKButtonHint="Open a connection to this IP address."
	CancelButtonHint="Close this page without connecting to a server."
	EditBoxHint="Enter the address for the server you'd like to connect to - separate the IP and port with the  :  symbol"

	Begin Object class=GUILabel Name=IPDesc
		Caption="Enter New IP Address"
		TextAlign=TXTA_Center
		StyleName="TextLabel"
		FontScale=FNS_Large
		WinWidth=1
		WinLeft=0
		WinTop=0.4
		WinHeight=32
	End Object

	// EditBox with Limited CharSet for IP address
	Begin Object class=moEditBox Name=IpEntryBox
		Caption="IP Address: "
		LabelJustification=TXTA_Right
		CaptionWidth=0.55
		ComponentWidth=-1
		WinWidth=0.500000
		WinHeight=0.04
		WinLeft=0.160000
		WinTop=0.466667
		TabOrder=0
	End Object

	l_Text=IPDesc
	ed_Data=IPEntryBox
}

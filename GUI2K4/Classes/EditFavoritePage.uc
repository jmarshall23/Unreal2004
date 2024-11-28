//==============================================================================
//	Created on: 09/16/2003
//	Edit the IP and port of an existing favorite
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class EditFavoritePage extends UT2k4Browser_OpenIP;

var automated GUILabel l_name;
var GameInfo.ServerResponseLine Server;

var localized string UnknownText;

function HandleParameters(string ServerIP, string ServerName)
{
	if (ServerIP != "")
		ed_Data.SetText(StripProtocol(ServerIP));

	if ( ServerName == "" )
		ServerName = UnknownText;

	l_Name.Caption = ServerName;
}

function ApplyURL( string URL )
{
	local string IP, port;

	if ( URL == "" )
		return;

	URL = StripProtocol(URL);
	if ( !Divide( URL, ":", IP, Port ) )
	{
		IP = URL;
		Port = "7777";
	}

	Server.IP = IP;
	Server.Port = int(Port);
	Server.QueryPort = Server.Port + 1;
	Server.ServerName = l_name.Caption;
	Controller.CloseMenu(False);
}

defaultproperties
{
	OKButtonHint="Close the page and save the new IP to your favorites list."
	CancelButtonHint="Close the page and discard any changes."
	EditBoxHint="Enter the URL for this favorite - separate IP and port with the   :   symbol"

	Begin Object Class=GUILabel Name=ServerName
		StyleName="TextLabel"
		TextAlign=TXTA_Center
		WinWidth=0.854492
		WinHeight=0.050000
		WinLeft=0.070313
		WinTop=0.299479
	End Object
	l_name=ServerName

	Begin Object class=moEditBox Name=IpEntryBox
		Caption="IP Address: "
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Left
		CaptionWidth=0.35
		ComponentWidth=-1
		WinWidth=0.590820
		WinHeight=0.030000
		WinLeft=0.192383
		WinTop=0.487500
		TabOrder=0
		bAutoSizeCaption=True
	End Object
	ed_Data=IPEntryBox

	UnknownText="Unknown Server"
}

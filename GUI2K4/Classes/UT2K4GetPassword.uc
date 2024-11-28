//==============================================================================
//	Created on: 08/11/2003
//	Menu that pops up when attempting to connect to a password protected server
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4GetPassword extends UT2K4GetDataMenu;

var string	RetryURL;
var localized string IncorrectPassword;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(Mycontroller, MyOwner);
	PlayerOwner().ClearProgressMessages();

//	ed_Data.MyEditBox.OnKeyEvent = InternalOnKeyEvent;
	ed_Data.MyEditBox.bConvertSpaces = true;
}

function HandleParameters(string URL, string FailCode)
{
	RetryURL = URL;
	if ( FailCode ~= "WRONGPW" )
		l_Text.Caption = IncorrectPassword;
}

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_OK  ) // Retry
		RetryPassword();

	else if ( Sender == b_Cancel ) // Fail
		Controller.ReplaceMenu(Controller.GetServerBrowserPage());

	return true;
}

function RetryPassword()
{
	local string 			EntryString;
	local ExtendedConsole	MyConsole;

	EntryString = ed_Data.GetText();
	MyConsole = ExtendedConsole(PlayerOwner().Player.Console);

	if ( MyConsole != None && EntryString != "" )
		SavePassword(MyConsole, EntryString);

	PlayerOwner().ClientTravel(
		Eval( EntryString != "",
			  RetryURL $ "?password=" $ EntryString,
			  RetryURL
			), TRAVEL_Absolute,false);

	Controller.CloseAll(false,True);
}

function SavePassword( ExtendedConsole InConsole, string InPassword )
{
	local int i;

	if ( InConsole != None )
	{
		for (i = 0; i < InConsole.SavedPasswords.Length; i++)
		{
			if ( InConsole.SavedPasswords[i].Server == InConsole.LastConnectedServer )
			{
				InConsole.SavedPasswords[i].Password = InPassword;
				break;
			}
		}

		if ( i == InConsole.SavedPasswords.Length )
		{
			InConsole.SavedPasswords.Length = InConsole.SavedPasswords.Length + 1;
			InConsole.SavedPasswords[i].Server = InConsole.LastConnectedServer;
			InConsole.SavedPasswords[i].Password = InPassword;
		}

		InConsole.SaveConfig();
	}
}

defaultproperties
{
	IncorrectPassword="Incorrect password specified.  Please try again."
/*
    Begin Object Class=GUIImage Name=menuBackground
        Image=material'2K4Menus.Controls.menuBackground2'
        ImageStyle=ISTY_Stretched
        ImageRenderStyle=MSTY_Normal
        ImageColor=(R=255,G=255,B=255,A=255)
        WinWidth=1
        WinHeight=1.000000
        WinLeft=0
        WinTop=0
        RenderWeight=0.000001
    End Object
    i_PageBG=MenuBackground
*/

	Begin Object class=moEditBox Name=GetPassPW
		Caption="Server Password"
		CaptionWidth=0.4
		TabOrder=0
		WinWidth=0.562500
		WinHeight=0.030000
		WinLeft=0.209375
		WinTop=0.485366
	End Object
	ed_Data=GetPassPW

	Begin Object Class=GUIButton Name=GetPassRetry
		Caption="SUBMIT"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		bBoundToParent=true
		WinWidth=0.1475
		WinHeight=0.045
		WinLeft=0.320899
		WinTop=0.730455
		TabOrder=1
	End Object
	b_OK=GetPassRetry

	Begin Object Class=GUIButton Name=GetPassFail
		Caption="CANCEL"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		bBoundToParent=true
		WinWidth=0.1475
		WinHeight=0.045000
		WinLeft=0.586523
		WinTop=0.547122
		TabOrder=2
	End Object
	b_Cancel=GetPassFail

	Begin Object Class=GUILabel Name=GetPassLabel
		Caption="A password is required to play on this server."
		TextAlign=TXTA_Center
		StyleName="TextLabel"
		FontScale=FNS_Large
		WinWidth=0.940430
		WinHeight=0.054688
		WinLeft=0.027930
		WinTop=0.302230
		bBoundToParent=true
	End Object
	l_Text=GetPassLabel

	OpenSound=sound'MenuSounds.SelectDshort'
	bAllowedAsLast=true
}

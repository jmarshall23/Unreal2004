//==============================================================================
//	Created on: 08/29/2003
//	This page pops up when attempting to enter a chatroom which has a password.
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4ChatPassword extends UT2K4GetDataMenu;

var string ChatRoomTitle;
var localized string IncorrectPassword;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(Mycontroller, MyOwner);
	PlayerOwner().ClearProgressMessages();

	ed_Data.MyEditBox.bConvertSpaces = true;
	ed_Data.MaskText(True);
}

function HandleParameters(string Title, string FailCode)
{
	ChatRoomTitle = Title;
	if ( FailCode ~= "WRONGPW" )
		l_Text.Caption = Repl(IncorrectPassword, "%ChatRoom%", ChatRoomTitle);
}

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_OK  ) // Retry
		RetryPassword();

	else if ( Sender == b_Cancel ) // Fail
		Controller.CloseMenu(True);

	return true;
}

function RetryPassword()
{
	local string Password;
	local PlayerController PC;

	Password = GetDataString();
	PC = PlayerOwner();

	if ( Password == "" || PC == None )
		return;

	Controller.CloseAll(false,True);
	PC.Join(ChatRoomTitle,Password);
}

DefaultProperties
{
	IncorrectPassword="Incorrect password specified for channel '%ChatRoom%' ."

	Begin Object class=moEditBox Name=GetPassPW
		WinWidth=0.643751
		WinHeight=0.047305
		WinLeft=0.212500
		WinTop=0.497450
		Caption="Chat Room Password"
		CaptionWidth=0.4
		TabOrder=0
	End Object
	ed_Data=GetPassPW

	Begin Object Class=GUIButton Name=GetPassRetry
		Caption="RETRY"
		StyleName="SquareButton"
		OnClick=InternalOnClick
		bBoundToParent=true
		WinWidth=0.131641
		WinHeight=0.040000
		WinLeft=0.346289
		WinTop=0.561667
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
		WinTop=0.561667
		TabOrder=2
	End Object
	b_Cancel=GetPassFail

	Begin Object Class=GUILabel Name=GetPassLabel
		Caption="A password is required to join this chat room"
		TextAlign=TXTA_Center
		StyleName="TextLabel"
		FontScale=FNS_Large
		WinWidth=0.995117
		WinHeight=0.054688
		WinLeft=0.010742
		WinTop=0.318897
		bBoundToParent=true
		bMultiLine=True
	End Object
	l_Text=GetPassLabel

	OpenSound=sound'MenuSounds.SelectDshort'
	bAllowedAsLast=true
}

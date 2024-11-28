//==============================================================================
//  Created on: 01/02/2004
//  Login panel for Match Setup
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class MatchSetupLoginPanel extends MatchSetupPanelBase
	config(LoginCache);

var() config bool bAutoLogin, bKeepHistory;
var() config array<AutoLoginInfo> LoginHistory;

var() editconst noexport string CurrentIP, CurrentPort;

var automated GUILabel l_Title, l_Status;
var automated moEditBox ed_LoginName;
var automated moEditBox ed_LoginPassword;
var automated GUIButton b_Submit, b_Cancel;

var() localized string NoUsernameSpecified, NoPasswordSpecified, InvalidLoginText, WaitingForLoginText,
                       LoggedText, ButtonLoginText, ButtonLogoutText, PleaseWaitText;

function InitComponent( GUIController C, GUIComponent O )
{
	local PlayerController PC;
	local string str;
	local int i;

	Super.InitComponent(C, O);

	PC = PlayerOwner();

	ed_LoginPassword.MyEditBox.bConvertSpaces = True;
	str = PC.GetServerNetworkAddress();

	if ( str != "" )
	{
		if ( !Divide(str, ":", CurrentIP, CurrentPort) )
		{
			CurrentIP = str;
			CurrentPort = "7777";
		}
	}

	i = FindCredentials(CurrentIP, CurrentPort);
	if ( i != -1 )
	{
		ed_Loginname.SetText(LoginHistory[i].Username);
		ed_LoginPassword.SetText(LoginHistory[i].Password);

		if ( LoginHistory[i].bAutoLogin )
			InternalOnClick(b_Submit);
	}
	else if ( PC.PlayerReplicationInfo != None )
		ed_LoginName.SetText(PC.PlayerReplicationInfo.PlayerName);
}

function Opened(GUIComponent Sender)
{
	Super.Opened(Sender);
	UpdateSubmitButton();
}

function bool InternalOnClick(GUIComponent Sender)
{
	local string uname, upass;

	if ( VRI == None )
		return true;

	if ( Sender == b_Submit )
	{
		if ( b_Submit.Caption == ButtonLoginText )
		{
			uname = ed_LoginName.GetText();
			upass = ed_LoginPassword.GetText();

			if ( uname == "" )
			{
				SetFocus(ed_LoginName);
				UpdateStatus(NoUsernameSpecified);
				return true;
			}

			if ( upass == "" )
			{
				SetFocus(ed_LoginPassword);
				UpdateStatus(NoPasswordSpecified);
				return true;
			}

			SendLogin(uname,upass);
		}

		else if ( b_Submit.Caption == ButtonLogoutText )
		{
			SendLogout();
		}

		return true;
	}

	if ( Sender == b_Cancel )
	{
		Controller.CloseMenu(false);
		return true;
	}



	return true;
}

function bool UserNameKeyEvent( out byte Key, out byte State, float Delta )
{
	if ( State == 3 && (Key == 0x0D || Key == 0x28) ) // enter or down
	{
		ed_LoginPassword.SetFocus(none);
		return true;
	}

	return false;
}

function bool PasswordKeyEvent( out byte Key, out byte State, float Delta )
{
	if ( State == 3 )
	{
		if ( Key == 0x0D ) // enter
			return InternalOnClick(b_Submit);

		else if ( Key == 0x26 ) // up
		{
			ed_LoginName.SetFocus(none);
			return true;
		}
	}

	return false;
}

protected function UpdateStatus(string NewStatusMsg )
{
	l_Status.Caption = NewStatusMsg;
	UpdateSubmitButton();
}

protected function int FindCredentials( coerce string IP, coerce string Port )
{
	local int i;

	for ( i = 0; i < LoginHistory.Length; i++ )
		if ( LoginHistory[i].IP == IP && LoginHistory[i].Port == Port )
			return i;

	return -1;
}

protected function SaveCredentials()
{
	local AutoLoginInfo NewInfo;
	local int i;

	if ( !bKeepHistory )
		return;

	NewInfo.UserName = ed_LoginName.GetText();
	NewInfo.Password = ed_LoginPassword.GetText();
	if ( NewInfo.Password == "" )
		return;

	NewInfo.IP = CurrentIP;
	NewInfo.Port = CurrentPort;

	i = FindCredentials(NewInfo.IP, NewInfo.Port);
	if ( i == -1 )
		i = LoginHistory.Length;

	LoginHistory[i] = NewInfo;
	SaveConfig();
}

function UpdateSubmitButton()
{
	switch( l_Status.Caption )
	{
	case WaitingForLoginText:
		b_Submit.Caption = ButtonLogoutText;
		DisableComponent(b_Submit);
		break;

	case LoggedText:
		b_Submit.Caption = ButtonLogoutText;
		EnableComponent(b_Submit);
		break;

	case LoggedText @ PleaseWaitText:
		b_Submit.Caption = ButtonLogoutText;
		DisableComponent(b_Submit);
		break;

	default:
		EnableComponent(b_Submit);
		b_Submit.Caption = ButtonLoginText;
		break;

	}
}

function SendLogin( string UserName, string Password )
{
	DisableComponent(ed_LoginName);
	DisableComponent(ed_LoginPassword);
	EnableComponent(b_Cancel);

	UpdateStatus(WaitingForLoginText);
	VRI.MatchSetupLogin(UserName, Password);
}

function SendLogout();

function LoggedIn()
{
	Super.LoggedIn();
	DisableComponent(b_Cancel);

	UpdateStatus(LoggedText @ PleaseWaitText);
	SaveCredentials();
	VRI.RequestMatchSettings(True,True);
}

function LoggedOut()
{
	Super.LoggedOut();
	EnableComponent(b_Submit);
	EnableComponent(ed_LoginName);
	EnableComponent(ed_LoginPassword);

	UpdateStatus("");
}

function LoginFailed()
{
	EnableComponent(b_Submit);
	EnableComponent(ed_LoginName);
	EnableComponent(ed_LoginPassword);

	DisableComponent(b_Cancel);
	UpdateStatus(InvalidLoginText);
	SetFocus(ed_LoginPassword);
}

function ReceiveComplete()
{
	Super.ReceiveComplete();
	UpdateStatus(LoggedText);
}

defaultproperties
{
	PanelCaption="Login"
	OnLogIn=LoggedIn
	OnLogout=LoggedOut

	WaitingForLoginText="Please wait while your login credentials are verified..."
	NoUsernameSpecified="In order to login to match setup, you must supply a username."
	NoPasswordSpecified="In order to login to match setup, you must supply a password."
	InvalidLoginText="Login attempt failed - invalid username or password."
	LoggedText="Successfully logged into match setup!"
	PleaseWaitText="Complete data transfer will take a few seconds..."

	ButtonLoginText="Login"
	ButtonLogoutText="Logout"

	Begin Object class=GUILabel Name=TitleLabel
		WinWidth=0.521077
		WinHeight=0.152803
		WinLeft=0.244935
		WinTop=0.012808
		Caption="Match Setup Login"
		TextAlign=TXTA_Center
		RenderWeight=1
		StyleName="TextLabel"
		FontScale=FNS_Large
		bBoundToParent=True
		bScaleToParent=True
	End Object
//	l_Title=TitleLabel

	Begin Object class=GUILabel Name=StatusLabel
	    TextAlign=TXTA_Center
	    bMultiLine=True
	    VertAlign=TXTA_Center
	    FontScale=FNS_Large
	    StyleName="TextLabel"
	    WinTop=0.571450
	    WinLeft=0.167765
	    WinWidth=0.670595
	    WinHeight=0.413253
	    RenderWeight=1.000000
	    bBoundToParent=True
	    bScaleToParent=True
	End Object
	l_Status=StatusLabel

	Begin Object class=moEditBox Name=UserIDEditBox
		WinWidth=0.659385
		WinHeight=0.081981
		WinLeft=0.174240
		WinTop=0.209260
		Caption="UserID"
		TabOrder=1
		CaptionWidth=0.1
		OnKeyEvent=UserNameKeyEvent
		bBoundToParent=True
		bScaleToParent=True
	End Object
	ed_LoginName=UserIDEditBox

	Begin Object class=moEditBox Name=PasswordEditBox
		WinWidth=0.659385
		WinHeight=0.081981
		WinLeft=0.174240
		WinTop=0.326729
		Caption="Password"
		TabOrder=2
		CaptionWidth=0.1
		OnKeyEvent=PasswordKeyEvent
		bBoundToParent=True
		bScaleToParent=True
		bMaskText=True
	End Object
	ed_LoginPassword=PasswordEditBox

	Begin Object class=GUIButton Name=LoginButton
		Caption="Login"
		WinWidth=0.137685
		WinHeight=0.070180
		WinLeft=0.680482
		WinTop=0.477284
		OnClick=InternalOnClick
		TabOrder=3
		RenderWeight=1.0
		bBoundToParent=True
		bScaleToParent=True
	End Object
	b_Submit=LoginButton

	Begin Object class=GUIButton Name=CancelButton
		Caption="Cancel"
		WinWidth=0.139293
		WinHeight=0.076611
		WinLeft=0.513741
		WinTop=0.474198
		OnClick=InternalOnClick
		TabOrder=4
		RenderWeight=1.0
		bBoundToParent=True
		bScaleToParent=True
	End Object
	b_Cancel=CancelButton

	WinTop=0.248697
	WinHeight=0.352864
	WinWidth=1.0
	WinLeft=0.0

	bKeepHistory=True
}

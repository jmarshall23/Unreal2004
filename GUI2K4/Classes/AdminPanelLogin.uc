//==============================================================================
//  Created on: 11/12/2003
//  Default screen that appears until successfully logged in
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class AdminPanelLogin extends AdminPanelBase
	config(LoginCache);

var() config bool bStoreLogins;
var() config array<AutoLoginInfo> LoginHistory;
var() localized string WaitingForLoginText, LoggedText;

var automated moEditBox  ed_LoginName, ed_LoginPassword;
var automated GUIButton  b_Login, b_Logout;
var automated GUILabel   l_Status;

var() editconst noexport string CurrentIP, CurrentPort;

function InitComponent( GUIController C, GUIComponent O )
{
	local PlayerController PC;
	local string str;
	local int i;

	Super.InitComponent(C, O);

	PC = PlayerOwner();
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
			InternalOnClick(b_Login);
	}
}

protected function UpdateStatus(string NewStatusMsg )
{
	l_Status.Caption = NewStatusMsg;
}

function bool InternalOnClick(GUIComponent Sender)
{
	local PlayerController PC;
	local string cmd, uname, upass;

	PC = PlayerOwner();
	if ( PC == None )
		return true;

	if ( Sender == b_Login )
	{
		cmd = "AdminLogin";
		uname = ed_LoginName.GetText();
		upass = ed_LoginPassword.GetText();

		UpdateStatus(WaitingForLoginText);
	}

	else if ( Sender == b_Logout )
		cmd = "AdminLogout";

	if ( uname != "" )
		cmd @= uname;

	if ( upass != "" )
		cmd @= upass;

	AdminCommand(cmd);
	return true;
}

function LoggedIn( string AdminName )
{
	DisableComponent(b_Login);
	DisableComponent(ed_LoginName);
	DisableComponent(ed_LoginPassword);

	EnableComponent(b_Logout);
	UpdateStatus(Repl(LoggedText, "%name%", AdminName));

	SaveCredentials();
}

function LoggedOut()
{
	DisableComponent(b_Logout);
	EnableComponent(b_Login);
	EnableComponent(ed_LoginName);
	EnableComponent(ed_LoginPassword);

	UpdateStatus("");
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

	if ( !bStoreLogins )
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

DefaultProperties
{
	PanelCaption="Login"
	WaitingForLoginText="Please wait while your login credentials are verified..."

	Begin Object Class=GUILabel Name=StatusLabel
		WinWidth=0.992189
		WinHeight=0.407813
		WinLeft=0.005312
		WinTop=0.585417
		StyleName="TextLabel"
		bMultiLine=True
		TextAlign=TXTA_Center
		VertAlign=TXTA_Left
		FontScale=FNS_Large
	End Object
	l_Status=StatusLabel

	Begin Object Class=moEditBox Name=LoginNameEditbox
		WinWidth=0.895312
		WinHeight=0.098438
		WinLeft=0.089063
		WinTop=0.091667
		bScaleToParent=True
		bBoundToParent=True
		Caption="Login Name: "
		Hint="Enter your admin username"
		ComponentWidth=-1
		CaptionWidth=0.2
		bAutoSizeCaption=True
		LabelJustification=TXTA_Right
	End Object
	ed_LoginName=LoginNameEditBox

	Begin Object Class=moEditBox Name=LoginPasswordEditBox
		WinWidth=0.970312
		WinHeight=0.098437
		WinLeft=0.014062
		WinTop=0.236667
		bScaleToParent=True
		bBoundToParent=True
		Caption="Login Password: "
		Hint="Enter your admin password"
		ComponentWidth=-1
		CaptionWidth=0.2
		bAutoSizeCaption=True
		bMaskText=True
		LabelJustification=TXTA_Right
	End Object
	ed_LoginPassword=LoginPasswordEditBox

	Begin Object Class=GUIButton Name=LoginButton
		WinWidth=0.286607
		WinHeight=0.092188
		WinLeft=0.360938
		WinTop=0.418750
		bScaleToParent=True
		bBoundToParent=True
		OnClick=InternalOnClick
		Caption="LOGIN"
	End Object
	b_Login=LoginButton

	Begin Object Class=GUIButton Name=LogoutButton
		WinWidth=0.286607
		WinHeight=0.092188
		WinLeft=0.360938
		WinTop=0.418750
		bScaleToParent=True
		bBoundToParent=True
		OnClick=InternalOnClick
		Caption="LOGOUT"
	End Object
	b_Logout=LogoutButton
}

//==============================================================================
//	Created on: 09/14/2003
//	Configures a keybind and channel name for quickly switching active voice chat room
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class VoiceChatKeyBindPage extends LargeWindow;

var automated GUILabel  l_PageTitle, l_KeyLabel1, l_KeyLabel2, l_Key1, l_Key2;
var automated moEditBox ed_ChannelName;
var automated GUIButton b_OK/*, b_Cancel*/;

var localized string NoneText, AnyKeyText;

var string Channel;
var array<string> Keys, LocalizedKeys;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);

	l_Key1.Caption = NoneText;
	l_Key2.Caption = NoneText;
}

function HandleParameters(string Value, string Nothing)
{
	Channel = Value;
	ed_ChannelName.SetText(Channel);
	GetBinds();
}

function GetBinds()
{
	UpdateLabel(l_Key1, False);
	UpdateLabel(l_Key2, False);

	Controller.GetAssignedKeys("Speak" @ Channel, Keys, LocalizedKeys);

	if ( LocalizedKeys.Length > 0 )
		l_Key1.Caption = LocalizedKeys[0];

	if ( LocalizedKeys.Length > 1 )
		l_Key2.Caption = LocalizedKeys[1];
}

function bool CloseClick(GUIComponent Sender)
{
	Controller.CloseMenu(False);
	return true;
}

function InternalOnChange(GUIComponent Sender)
{
	switch ( Sender )
	{
		case ed_ChannelName:
			Channel = ed_ChannelName.GetText();
			break;
	}
}

function bool KeyClick(GUIComponent Sender)
{
	if ( GUILabel(Sender) != None )
	{
		UpdateLabel( GUILabel(Sender), True );
	    Controller.OnNeedRawKeyPress = RawKeyPress;
	    Controller.Master.bRequireRawJoystick = True;
	    PlayerOwner().ConsoleCommand("toggleime 0");

	    return true;
	}

	return false;
}

function UpdateLabel( GUILabel Label, bool bWaitingForRawInput )
{
	if ( Label == None )
		return;

	if ( bWaitingForRawInput )
	{
		Label.Caption = AnyKeyText;
		Label.FontScale = FNS_Small;
	}
	else
	{
		Label.FontScale = FNS_Medium;
		Label.Caption = NoneText;
	}
}

function bool RawKeyPress(byte NewKey)
{
	local string NewKeyName, LocalizedKeyName;

    Controller.OnNeedRawKeyPress = None;
    Controller.Master.bRequireRawJoystick = False;
    PlayerOwner().ConsoleCommand("toggleime 1");

	if ( NewKey == 0x1B )
	{
		GetBinds();
		return true;
	}

	Controller.KeyNameFromIndex( NewKey, NewKeyName, LocalizedKeyName );

	Controller.SetKeyBind( NewKeyName, "Speak" @ Channel );
	PlayerOwner().ClientPlaySound(Controller.ClickSound);

    GetBinds();
    return true;
}

DefaultProperties
{
	NoneText="None"
	AnyKeyText="Press Any Key|To Bind Command"

	WinLeft=0
	WinTop=0.375
	WinWidth=1
	WinHeight=0.3

	Begin Object Class=GUILabel Name=Title
		WinWidth=0.629687
		WinHeight=0.068164
		WinLeft=0.185352
		WinTop=0.388802
		Caption="Modify Quick Switch KeyBind"
		StyleName="TextLabel"
		FontScale=FNS_Large
	End Object
	l_PageTitle=Title

	Begin Object Class=moEditBox Name=ChannelName
		Caption="Channel Name"
		LabelJustification=TXTA_Center
		bVerticalLayout=True
		WinWidth=0.278125
		WinHeight=0.087695
		WinLeft=0.142383
		WinTop=0.486458
		OnChange=InternalOnChange
	End Object
	ed_ChannelName=ChannelName

	Begin Object Class=GUILabel Name=KeyLabel1
		WinWidth=0.082813
		WinHeight=0.038867
		WinLeft=0.464649
		WinTop=0.487760
		Caption="Key 1"
		StyleName="TextLabel"
	End Object
	l_KeyLabel1=KeyLabel1

	Begin Object Class=GUILabel Name=KeyLabel2
		WinWidth=0.200000
		WinHeight=0.038867
		WinLeft=0.654102
		WinTop=0.487760
		Caption="Key 2"
		StyleName="TextLabel"
	End Object
	l_KeyLabel2=KeyLabel2

	Begin Object Class=GUILabel Name=Key1
		WinWidth=0.163867
		WinHeight=0.082813
		WinLeft=0.463673
		WinTop=0.529427
		bAcceptsInput=True
		OnClick=KeyClick
		StyleName="TextLabel"
		bMultiLine=True
	End Object
	l_Key1=Key1

	Begin Object Class=GUILabel Name=Key2
		WinWidth=0.130664
		WinHeight=0.082813
		WinLeft=0.654102
		WinTop=0.529427
		bAcceptsInput=True
		OnClick=KeyClick
		StyleName="TextLabel"
		bMultiLine=True
	End Object
	l_Key2=Key2

	Begin Object Class=GUIButton Name=OKButton
		WinWidth=0.116992
		WinHeight=0.04
		WinLeft=0.673633
		WinTop=0.616667
		Caption="Apply"
		OnClick=CloseClick
	End Object
	b_OK=OKButton
/*
	Begin Object Class=GUIButton Name=CancelButton
		WinWidth=0.116992
		WinHeight=0.04
		WinLeft=0.795703
		WinTop=0.616667
		Caption="Cancel"
		OnClick=CloseClick
	End Object
	b_Cancel=CancelButton

	Begin Object Class=GUIImage Name=PageBackground

	End Object
	i_PageBackground=PageBackground
*/
}

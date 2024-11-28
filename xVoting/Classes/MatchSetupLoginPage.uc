class MatchSetupLoginPage extends LargeWindow;

var automated GUILabel l_Title;
var automated moEditBox ed_UserID;
var automated moEditBox ed_Password;
var automated GUIButton b_LogIn;
var automated GUIButton b_Cancel;

var VotingReplicationInfo VRI;
//------------------------------------------------------------------------------------------------
function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local VotingReplicationInfo RI;
	Super.InitComponent(Mycontroller, MyOwner);

	ed_Password.MyEditBox.bConvertSpaces = true;

    // find the VotingReplicationInfo
	foreach AllObjects(class 'VotingReplicationInfo', RI)
	{
		if( RI.PlayerOwner != None && RI.PlayerOwner == PlayerOwner())
		{
			VRI = RI;
			break;
		}
	}
	if(VRI == None)
		Controller.CloseAll(false);

	ed_Password.MyEditBox.bMaskText=true;
	ed_UserID.SetComponentValue(PlayerOwner().PlayerReplicationInfo.PlayerName);
}
//------------------------------------------------------------------------------------------------
function bool InternalOnClick(GUIComponent Sender)
{
	if(Sender==b_Login && Len(ed_UserID.GetText())>0 && len(ed_Password.GetText())>0 )
	{
		VRI.MatchSetupLogin(ed_UserID.GetText(), ed_Password.GetText());
		setTimer(1, true);
	}

	if(Sender==b_Cancel)
		Controller.CloseAll(false);

	return true;
}
//------------------------------------------------------------------------------------------------
function timer()
{
	Super.timer();

	if(VRI != None && VRI.bMatchSetupPermitted)
		Controller.CloseMenu(false);
}
//------------------------------------------------------------------------------------------------
function bool UserIDKeyPress(out byte Key, out byte State, float delta)
{
	if((Key == 13) && (State==1)) // Enter Key
	{
		ed_Password.SetFocus(none);
		return true;
	}

	if((Key == 40) && (State==1)) // Up Down
	{
		ed_Password.SetFocus(none);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------------------------
function bool PasswordKeyPress(out byte Key, out byte State, float delta)
{
	if((Key == 13) && (State==1)) // Enter Key
	{
		if(Len(ed_UserID.GetText())>0 && len(ed_Password.GetText())>0 )
		{
			VRI.MatchSetupLogin(ed_UserID.GetText(), ed_Password.GetText());
			setTimer(1, true);
		}
		return true;
	}

	if((Key == 38) && (State==1)) // Up Key
	{
		ed_UserID.SetFocus(none);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------------------------
function Closed(GUIComponent Sender, bool bCancelled)
{
	VRI = None;
	Super.Closed(Sender, bCancelled);
}
//------------------------------------------------------------------------------------------------
event bool NotifyLevelChange()
{
	VRI = None;
	return Super.NotifyLevelChange();
}
//------------------------------------------------------------------------------------------------
defaultproperties
{
	Begin Object class=GUILabel Name=TitleLabel
		WinWidth=0.382813
		WinHeight=0.053125
		WinLeft=0.302813
		WinTop=0.287500
		Caption="Match Setup Login"
		TextALign=TXTA_Center
		TextFont="UT2SmallFont"
		TextColor=(R=0,G=0,B=255,A=255)
		RenderWeight=1
	End Object
	l_Title=TitleLabel

	Begin Object class=moEditBox Name=UserIDEditBox
		WinWidth=0.381250
		WinHeight=0.033750
		WinLeft=0.301250
		WinTop=0.366667
		Caption="UserID"
		TabOrder=1
		CaptionWidth=0.5
		OnKeyEvent=UserIDKeyPress
	End Object
	ed_UserID=UserIDEditBox

	Begin Object class=moEditBox Name=PasswordEditBox
		WinWidth=0.382500
		WinHeight=0.031250
		WinLeft=0.300000
		WinTop=0.431667
		Caption="Password"
		TabOrder=2
		CaptionWidth=0.5
		OnKeyEvent=PasswordKeyPress
	End Object
	ed_Password=PasswordEditBox

	Begin Object class=GUIButton Name=LoginButton
		Caption="Login"
		StyleName="SquareButton"
		WinWidth=0.120000
		WinHeight=0.033203
		WinLeft=0.330000
		WinTop=0.526667
		OnClick=InternalOnClick
		TabOrder=3
		RenderWeight=1.0
	End Object
	b_Login=LoginButton

	Begin Object class=GUIButton Name=CancelButton
		Caption="Cancel"
		StyleName="SquareButton"
		WinWidth=0.120000
		WinHeight=0.033203
		WinLeft=0.536249
		WinTop=0.526667
		OnClick=InternalOnClick
		TabOrder=4
		RenderWeight=1.0
	End Object
	b_Cancel=CancelButton

	OpenSound=sound'MenuSounds.SelectDshort'

	bAllowedAsLast=true
	WinTop=0.248697
	WinHeight=0.352864
	WinWidth=1.0;
	WinLeft=0.0;
}

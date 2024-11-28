//-----------------------------------------------------------
//
//-----------------------------------------------------------
class UT2K4InGameChat extends FloatingWindow;

var automated GUISectionBackground sB_Main;
var automated moEditBox eb_Send;
var automated GUIScrollTextBox lb_Chat;
var() int OldCMC;

var() editinline array<byte> CloseKey;
var() editinlinenotify color TextColor[3];

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
	local PlayerController PC;
	local ExtendedConsole MyConsole;

	super.InitComponent(MyController,MyOwner);

	PC = PlayerOwner();
	TextColor[0] = class'SayMessagePlus'.default.RedTeamColor;
	TextColor[1] = class'SayMessagePlus'.default.BlueTeamColor;
	TextColor[2] = class'SayMessagePlus'.default.DrawColor;

	sb_Main.ManageComponent(lb_Chat);
    eb_Send.MyEditBox.OnKeyEvent = InternalOnKeyEvent;
    lb_Chat.MyScrollText.bNeverFocus=true;

	MyConsole = ExtendedConsole(PC.Player.Console);
	if (MyConsole==None)
		return;

	MyConsole.OnChat = HandleChat;
	for (i=0;i<MyConsole.ChatMessages.Length;i++)
	{
		if ( !MyConsole.bTeamChatOnly ||
		      PC.PlayerReplicationInfo == None ||
			  PC.PlayerReplicationInfo.Team == None ||
			  MyConsole.ChatMessages[i].Team == PC.PlayerReplicationInfo.Team.TeamIndex )
			HandleChat(MyConsole.ChatMessages[i].Message, MyConsole.ChatMessages[i].Team);
	}
}

event Opened(GUIComponent Sender)
{
	local int i;
	local string KeyName;
	local array<string> KeyNames;
	local PlayerController PC;

	Super.Opened(Sender);
	PC = PlayerOwner();

	CloseKey.Remove(0, CloseKey.Length);
    KeyName = PC.ConsoleCommand("BINDINGTOKEY InGameChat");
    Split(KeyName, ",", KeyNames);
    for ( i = 0; i < KeyNames.Length; i++ )
    	CloseKey[CloseKey.Length] = byte(PC.ConsoleCommand("KEYNUMBER"@KeyNames[i]));

    OldCMC = PC.myHud.ConsoleMessageCount;
    PC.myHUD.ConsoleMessageCount = 0;


    // Advance the cursor position to the end of the text
    lb_Chat.MyScrollText.End();

    FocusFirst(None);
}

function Closed(GUIComponent Sender, bool bCancelled)
{
    Super.Closed(Sender, bCancelled);
	PlayerOwner().MyHud.ConsoleMessageCount = OldCMC;
}

function HandleChat(string Msg, int TeamIndex)
{
	local int i;
	local string str;

	i = InStr( Msg, ":" );
	if ( TeamIndex < 2 && i != -1 )
	{
		str = MakeColorCode(TextColor[TeamIndex]) $ Left(Msg, i) $
			  MakeColorCode(TextColor[2]) $ ":" $ Mid(Msg, i+1);
	}
	else str = MakeColorCode(TextColor[2]) $ Msg;
	lb_chat.AddText( str );
}

function bool InternalOnKeyEvent(out byte Key, out byte State, float delta)
{
	local string cmd;
	local int i;

	if ( state == 1 )
	{
		for ( i = 0; i < CloseKey.Length; i++ )
			if ( Key == CloseKey[i] )
			{
				Controller.CloseMenu(false);
				return True;
			}
	}

	if ( state == 3 )
	{
		if ( Key == 0x0D )
	    {
	    	cmd = eb_Send.GetText();
	    	if ( cmd == "" )
	    		return True;

	        if ( Left(cmd,1)=="/" )
	        	cmd = Mid(cmd,1);
	        else if ( Left(cmd,1)=="." )
	        	cmd = "teamsay" @ Mid( cmd, 1 );
	        else
	        	cmd = "say" @ cmd;

	    	PlayerOwner().ConsoleCommand(cmd);
	        eb_Send.SetText("");
	        return true;
	    }
	}

	return eb_Send.MyEditBox.InternalOnKeyEvent(key,state,delta);
}

function InternalOnCreateComponent( GUIComponent NewComp, GUIComponent Sender )
{
	if ( NewComp != eb_Send )
		NewComp.bNeverFocus = True;

	Super.InternalOnCreateComponent(NewComp,Sender);
}

DefaultProperties
{
	Begin Object class=AltSectionBackground name=sbMain
		WinWidth=1.0
		WinHeight=1.0
		WinLeft=0.0
		Wintop=0.0
		LeftPadding=0
		RightPadding=0
		TopPadding=0
		BottomPadding=0
		bFillClient=true
        bBoundToParent=true
        bScaleToParent=true
        bNeverFocus=true
	End Object
	sb_Main=sbMain

	Begin Object Class=GUIScrollTextBox Name=lbChat
		WinWidth=1.000000
		WinHeight=0.558333
		WinLeft=0.000000
		WinTop=0.441667
		CharDelay=0.0025
		EOLDelay=0
		StyleName="NoBackground"
        bNoTeletype=true
        bNeverFocus=true
        TextAlign=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        FontScale=FNS_Small
        Separator="þ"
	End Object
	lb_Chat=lbChat

	Begin Object Class=moEditBox Name=ebSend
		WinWidth=0.818909
		WinHeight=0.035416
		WinLeft=0.099584
		WinTop=0.943855
		bScaleToParent=True
		bBoundToParent=True
		Caption="Say: "
		Hint="Prefix a message with a dot (.) to send a team message or a slash (/) to send a command."
		ComponentWidth=-1
		CaptionWidth=0.1
		bAutoSizeCaption=True
		TabOrder=0
		LabelJustification=TXTA_Left
	End Object
	eb_Send=ebSend

	bRenderWorld=true
    bRequire640x480=false
    bAllowedAsLast=true
	DefaultWidth=0.779688
	DefaultHeight=0.847083
	DefaultLeft=0.110313
	DefaultTop=0.057916
	WinWidth=0.779688
	WinHeight=0.847083
	WinLeft=0.110313
	WinTop=0.057916

	bResizeWidthAllowed=False
	bResizeHeightAllowed=False
	bPersistent=True
 	WindowName="In-Game Chat"
}

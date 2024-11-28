// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4VideoChangeOK extends UT2K4GenericMessageBox;

var() noexport enum EVideoChangeType { VCT_Resolution, VCT_FullScreen, VCT_Device } ChangeType;
var() noexport transient int    Count;
var() noexport transient string	RevertString;
var() localized string	RestoreText, SecondText, SecondsText;
var() string OverrideResNotice;

var automated GUIButton b_Cancel;

function InitComponent(GUIController InController, GUIComponent InOwner)
{
	Super.InitComponent(InController, InOwner);

	OverrideResNotice =
		Localize( "UT2DeferChangeRes", "DialogText.Caption", "XInterface" ) $ "|" $
		Localize( "UT2DeferChangeRes", "DialogText2.Caption", "XInterface" );
}

function Execute(string DesiredRes)
{
	if ( DesiredRes == "" )
	{
		KillTimer();
		if ( Controller.ActivePage == Self )
			Controller.CloseMenu();

		return;
	}

	if ( InStr(DesiredRes, "x16") != -1 || InStr(DesiredRes, "x32") != -1 )
	{
		ChangeType = VCT_Resolution;
		ChangeResolution(DesiredRes);
	}

	else if ( DesiredRes ~= "togglefullscreen" )
	{
		ChangeType = VCT_FullScreen;
		ToggleFullScreen();
	}
	else
	{
		ChangeType = VCT_Device;
		SetDevice(DesiredRes);
	}
}

function ToggleFullScreen()
{
	RevertString = "togglefullscreen";
	PlayerOwner().ConsoleCommand(RevertString);
	StartTimer();
}

function ChangeResolution( string DesiredRes )
{
	local int i;
	local string CurrentRes, NewX, NewY, NewDepth, NewScreen;
	local bool lowres;

	// Create the string we'll use to revert settings if change is undesireable
	CurrentRes	= Controller.GetCurrentRes();
	lowres = bool(PlayerOwner().ConsoleCommand("get ini:Engine.Engine.RenderDevice Use16bit"));

	RevertString = "setres" @ CurrentRes;
	if (lowres)
		RevertString $= "x16";
	else
		RevertString $= "x32";

	if(bool(PlayerOwner().ConsoleCommand("ISFULLSCREEN")))
		RevertString $= "f";
	else
		RevertString $= "w";


	// Apply new resolution and wait for acceptance
	PlayerOwner().ConsoleCommand("set ini:Engine.Engine.RenderDevice Use16bit"@(InStr(DesiredRes,"x16") != -1));

	i = InStr(DesiredRes, "x");

	NewX = Left(DesiredRes, i);
	NewY = Mid( DesiredRes, i + 1 );
	i = InStr( NewY, "x" );
	if ( i != -1 )
	{
		NewDepth = Mid(NewY, i);
		NewY = Left(NewY, i);

		if ( Right(NewDepth,1) ~= "f" || Right(NewDepth,1) ~= "w" )
		{
			NewScreen = Right(NewDepth,1);
			NewDepth = Left(NewDepth, Len(NewDepth)-1);
		}
	}

	if( int(NewX) < 640 || int(NewY) < 480 )
	{
		KillTimer();
		PlayerOwner().ConsoleCommand("TEMPSETRES 640x480" $ NewDepth $ NewScreen);
		if ( Controller.OpenMenu("GUI2K4.UT2K4GenericMessageBox","",OverrideResNotice) )
			Controller.ActivePage.OnClose = DeferChangeOnClose;
	}
	else
	{
		PlayerOwner().ConsoleCommand( "SETRES" @ DesiredRes );
		StartTimer();
	}
}

function SetDevice( string NewRenderDevice )
{
	RevertString = PlayerOwner().ConsoleCommand("get ini:Engine.Engine.RenderDevice Class");
	if ( RevertString ~= NewRenderDevice || !Controller.SetRenderDevice(NewRenderDevice) )
	{
		KillTimer();
		if ( Controller.ActivePage == Self )
			Controller.CloseMenu();

		return;
	}

	StartTimer();
}

function DeferChangeOnClose(optional Bool bCancelled)
{
	StartTimer();
}

function StartTimer()
{
	Count=15;
	SetTimer(1.0,true);
}

event Timer()
{
	Count--;
	l_Text2.Caption = Repl(RestoreText, "%count%", Count);

	if ( Count == 1 )
		l_Text2.Caption = Repl(l_Text2.Caption, "%seconds%", SecondText);
	else l_Text2.Caption = Repl(l_Text2.Caption, "%seconds%", SecondsText);

	if ( Count <= 0 )
		InternalOnClick(b_Cancel);
}

function bool InternalOnClick(GUIComponent Sender)
{
	KillTimer();
	if (Sender==b_Cancel)
	{
		switch (ChangeType)
		{
		case VCT_Resolution:
			PlayerOwner().ConsoleCommand("set ini:Engine.Engine.RenderDevice Use16bit"@(InStr(RevertString,"x16")!=-1));
			PlayerOwner().ConsoleCommand(RevertString);
			break;

		case VCT_FullScreen:
			PlayerOwner().ConsoleCommand(RevertString);
			break;

		case VCT_Device:
			Controller.SetRenderDevice(RevertString);
			break;
		}
	}

	Controller.CloseMenu(Sender == b_Cancel);
	return true;
}


defaultproperties
{
	Begin Object Class=GUIButton Name=bOk
		Caption="Keep Settings"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.175000
		WinTop=0.558334
		bBoundToParent=true
		OnClick=InternalOnClick
	End Object
    b_Ok=bOk

	Begin Object Class=GUIButton Name=bCancel
		Caption="Restore Settings"
		WinWidth=0.2
		WinHeight=0.04
		WinLeft=0.65
		WinTop=0.558334
		bBoundToParent=true
		OnClick=InternalOnClick
	End Object
    b_Cancel=bCancel

	Begin Object class=GUILabel Name=lbText
		Caption="Accept these settings?"
		TextAlign=TXTA_Center
   		StyleName="TextLabel"
   		FontScale=FNS_Large
		WinWidth=1.000000
		WinHeight=0.068750
		WinLeft=0.000000
		WinTop=0.390000
	End Object
    l_Text=lbText

	Begin Object class=GUILabel Name=lbText2
		Caption="(Original settings will be restored in 15 seconds)"
		TextAlign=TXTA_Center
   		StyleName="TextButton"
		WinWidth=1
		WinLeft=0
		WinTop=0.46
		WinHeight=0.045
	End Object
    l_Text2=lbText2

	RestoreText="(Original settings will be restored in %count% %seconds%)"
	SecondText="second"
	SecondsText="seconds"
	InactiveFadeColor=(R=128,G=128,B=128,A=255)
}

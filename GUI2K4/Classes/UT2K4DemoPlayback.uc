//-----------------------------------------------------------
//
//-----------------------------------------------------------
class UT2K4DemoPlayback extends PopupPageBase;

var automated StateButton b_FF, b_PlayPause, b_Stop;
var automated GUILabel lb_MapName, lb_Mod;

var GUIList l_ViewTargets;

var bool bIsClosing, bIsPaused;
var float OriginalGameSpeed;
var int GameSpeedModifier;
var float GameSpeedMods[4];
var float modfade;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	super.InitComponent(MyController, MyOwner);
	i_FrameBG.Image = material'DemoHeaderBar';

	OriginalGameSpeed = PlayerOwner().level.TimeDilation;
	lb_MapName.Caption = PlayerOwner().Level.Title;
	Animate(0,0,0.15);
}

event Free()
{
	l_ViewTargets.Clear();
	super.free();
}

function Arrival(GUIComponent Sender, EAnimationType Type)
{
	WinTop=0.0;
}

function bool StopClick(GUIComponent Sender)
{
	PlayerOwner().Level.TimeDilation = OriginalGameSpeed;
	PlayerOwner().ConsoleCommand("disconnect");
	return true;
}

function bool PlayPauseClick(GUIComponent Sender)
{
	if ( bIsPaused )	// We are paused
	{
		bIsPaused=false;
		b_PlayPause.Images[0]=material'PauseBlurry';
		b_PlayPause.Images[1]=material'PauseWatched';
		b_PlayPause.Images[2]=material'PauseWatched';
		b_PlayPause.Images[3]=material'PausePressed';
		b_PlayPause.Images[4]=material'PauseBlurry';
		PlayerOwner().Level.Pauser = None;

	}
	else
	{
		bIsPaused=true;
		b_PlayPause.Images[0]=material'PlayBlurry';
		b_PlayPause.Images[1]=material'PlayWatched';
		b_PlayPause.Images[2]=material'PlayWatched';
		b_PlayPause.Images[3]=material'PlayPressed';
		b_PlayPause.Images[4]=material'PlayBlurry';
		PlayerOwner().Level.Pauser = PlayerOwner().PlayerReplicationInfo;
	}
	return true;
}
function bool FastForwardClick(GUIComponent Sender)
{
	if (GameSpeedModifier<3)
		GameSpeedModifier++;
	else
		GameSpeedModifier = 0;

	lb_Mod.Caption = "x"$int(GameSpeedMods[GameSpeedModifier]);
	ModFade=255;
	PlayerOwner().Level.TimeDilation = OriginalGameSpeed * GameSpeedMods[GameSpeedModifier];
	return true;
}

function bool ModDraw(canvas C)
{
	if (ModFade>0)
	{
		ModFade -= (255*Controller.RenderDelta);
		if (ModFade>=0)
		{
			lb_Mod.TextColor.A=int(ModFade);
			return false;
		}
	}
	lb_Mod.TextColor.A=0;
	return false;
}



DefaultProperties
{

//	OnCanClose=MyCanClose
	OnArrival=Arrival

	Begin Object class=StateButton name=bStop
		WinWidth=0.040000
		WinHeight=0.800000
		WinLeft=0.013750
		WinTop=0.1
		Images(0)=material'2k4menus.AVPlayer.StopBlurry'
		Images(1)=material'2k4menus.AVPlayer.StopWatched'
		Images(2)=material'2k4menus.AVPlayer.StopWatched'
		Images(3)=material'2k4menus.AVPlayer.StopPressed'
		Images(4)=material'2k4menus.AVPlayer.StopBlurry'
		bBoundToParent=true
		bScaleToParent=true
		TabOrder=0
		OnClick=StopClick
	End Object
	b_Stop=bStop

	Begin Object class=StateButton name=bPlayPause
		WinWidth=0.040000
		WinHeight=0.800000
		WinLeft=0.055000
		WinTop=0.1
		Images(0)=material'2k4menus.AVPlayer.PauseBlurry'
		Images(1)=material'2k4menus.AVPlayer.PauseWatched'
		Images(2)=material'2k4menus.AVPlayer.PauseWatched'
		Images(3)=material'2k4menus.AVPlayer.PausePressed'
		Images(4)=material'2k4menus.AVPlayer.PauseBlurry'
		bBoundToParent=true
		bScaleToParent=true
		TabOrder=1
		OnClick=PlayPauseClick
	End Object
	b_PlayPause=bPlayPause

    	Begin Object class=StateButton name=bFF
		WinWidth=0.04
		WinHeight=0.8
		WinLeft=0.097500
		WinTop=0.1
		Images(0)=material'2k4menus.AVPlayer.NextTrackBlurry'
		Images(1)=material'2k4menus.AVPlayer.NextTrackWatched'
		Images(2)=material'2k4menus.AVPlayer.NextTrackWatched'
		Images(3)=material'2k4menus.AVPlayer.NextTrackPressed'
		Images(4)=material'2k4menus.AVPlayer.NextTrackBlurry'
		bBoundToParent=true
		bScaleToParent=true
		TabOrder=2
		OnClick=FastForwardClick
	End Object
	b_FF=bFF


	Begin Object class=GUILabel name=lbMapName
		WinWidth=0.825
		WinHeight=1
		WinLeft=0.15
		WinTop=0
		FontScale=FNS_Large
		Caption=""
		StyleName="DarkTextLabel"
		bBoundToParent=true
		bScaleToParent=true
		TextAlign=TXTA_Right
	End object
	lb_MapName=lbMapName;

	Begin Object class=GUILabel name=lbMod
		WinWidth=0.825
		WinHeight=1
		WinLeft=0.15
		WinTop=0
		Caption="2X"
		StyleName=""
		TextFont="UT2LargeFont"
		TextColor=(R=14,G=41,B=106,A=0)
		bBoundToParent=true
		bScaleToParent=true
		OnDraw=ModDraw
	End object
	lb_Mod=lbMod;

	WinTop=-0.065;
	WinLeft=0.0
	WinWidth=1.0
	WinHeight=0.065

	GameSpeedMods(0)=1.0
	GameSpeedMods(1)=2.0
	GameSpeedMods(2)=4.0
	GameSpeedMods(3)=8.0

	bAllowedAsLast=true


}

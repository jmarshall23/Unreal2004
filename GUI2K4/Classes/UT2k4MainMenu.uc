// ====================================================================
// 	The Main Menu
//
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
//	Updated by Ron Prestenback
// ====================================================================

class UT2k4MainMenu extends UT2K4GUIPage;

#exec OBJ LOAD FILE=InterfaceContent.utx
#exec OBJ LOAD FIlE=2K4Menus.utx
#exec OBJ LOAD FIlE=2K4MenuSounds.uax
/*
	Variable Name Legend

	l_ 	GUILabel			lb_	GUIListBox
	i_ 	GUIImage			li_	GUIList
	b_	GUIButton			tp_	GUITabPanel
	t_	GUITitleBar			sp_	GUISplitter
	c_	GUITabControl
	p_	GUIPanel

	ch_	moCheckBox
	co_	moComboBox
	nu_	moNumericEdit
	ed_	moEditBox
	fl_	moFloatEdit
	sl_ moSlider
*/

var automated   BackgroundImage i_BkChar,
								i_Background;
var automated	GUIImage 	    i_UT2Logo,
								i_PanHuge,
								i_PanBig,
                                i_PanSmall,
                                i_UT2Shader,
								i_TV;

var automated 	GUIButton	b_SinglePlayer, b_MultiPlayer, b_Host,
							b_InstantAction, b_ModsAndDemo,  b_Settings, b_Quit;

var bool	bAllowClose;

var array<material> CharShots;

var float CharFade, DesiredCharFade;
var float CharFadeTime;

var GUIButton Selected;
var() bool bNoInitDelay;

var() config string MenuSong;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);

	Background=MyController.DefaultPens[0];
	if (PlayerOwner().Level.IsDemoBuild())
	{
		b_SinglePlayer.DisableMe();
		b_MultiPlayer.SetFocus(none);
	}

	i_BkChar.Image = CharShots[rand(CharShots.Length)];
}

event Timer()
{
	bNoInitDelay = true;
    if (!Controller.bQuietMenu)
	    PlayerOwner().PlaySound(SlideInSound,SLOT_None);
    i_TV.Animate(-0.000977, 0.332292, 0.35);
    i_UT2Logo.Animate(0.007226,0.016926,0.35);
    i_UT2Shader.Animate(0.249023,0.180988,0.35);
    i_TV.OnEndAnimation = MenuIn_OnArrival;
    i_UT2Logo.OnEndAnimation = MenuIn_OnArrival;
    i_UT2Shader.OnEndAnimation = MenuIn_OnArrival;
}

function MenuIn_OnArrival(GUIComponent Sender, EAnimationType Type)
{
	Sender.OnArrival = none;
	if ( bAnimating )
		return;

	i_UT2Shader.OnDraw = MyOnDraw;
    DesiredCharFade=255;
    CharFadeTime = 0.75;

    if (!Controller.bQuietMenu)
	    PlayerOwner().PlaySound(FadeInSound);

    b_SinglePlayer.Animate(0.315359,0.368813,0.35);
    b_SinglePlayer.OnArrival = PlayPopSound;
	b_Multiplayer.Animate(0.363246,0.449282,0.40);
    b_Multiplayer.OnArrival = PlayPopSound;
	b_Host.Animate(0.395097,0.534027,0.45);
    b_Host.OnArrival = PlayPopSound;
	b_InstantAction.Animate(0.423640,0.618619,0.5);
    b_InstantAction.OnArrival = PlayPopSound;
    b_ModsAndDemo.Animate(0.433406,0.705859,0.55);
    b_ModsAndDemo.OnArrival = PlayPopSound;
	b_Settings.Animate(0.434477,0.800327,0.6);
    b_Settings.OnArrival = PlayPopSound;
	b_Quit.Animate(0.424711,0.887567,0.65);
    b_Quit.OnArrival = MenuIn_Done;
}

event Opened(GUIComponent Sender)
{
	if ( bDebugging )
		log(Name$".Opened()   Sender:"$Sender,'Debug');

    if ( Sender != None && PlayerOwner().Level.IsPendingConnection() )
    	PlayerOwner().ConsoleCommand("CANCEL");

    Super.Opened(Sender);

    Selected = none;

    // Reset the animations of all components
    i_TV.Animate(-0.000977, 1.668619, 0);
    i_UT2Logo.Animate(0.007226,-0.392579,0);
    i_UT2Shader.Animate(0.249023,-0.105470,0);
    b_SinglePlayer.Animate(1,0.368813,0);
	b_Multiplayer.Animate(1.15,0.449282,0);
	b_Host.Animate(1.3,0.534027,0);
	b_InstantAction.Animate(1.45,0.618619,0);
    b_ModsAndDemo.Animate(1.6,0.705859,0);
	b_Settings.Animate(1.75,0.800327,0);
	b_Quit.Animate(1.9,0.887567,0);
}

function MenuIn_Done(GUIComponent Sender, EAnimationType Type)
{
	Sender.OnArrival = none;
    PlayPopSound(Sender,Type);
}

function PlayPopSound(GUIComponent Sender, EAnimationType Type)
{
    if (!Controller.bQuietMenu)
		PlayerOwner().PlaySound(PopInSound);
}

function bool PanHugeDraw(Canvas Canvas)
{
	i_PanHuge.ImageColor.A  = 40 * (CharFade/255);
	i_PanBig.ImageColor.A   = 75 * (CharFade/255);
	i_PanSmall.ImageColor.A = i_BKChar.ImageColor.A;

    return false;
}

function bool BkCharDraw(Canvas Canvas)
{

	if (CharFadeTime>0)
    {
    	CharFade += (DesiredCharFade - CharFade) * (Controller.RenderDelta / CharFadeTime);
        CharFadeTime -= Controller.RenderDelta;
        if (CharFadeTime<=0.0)
        {
        	CharFade = DesiredCharFade;
            CharFadeTime=0.0;
        }
    }

    i_BKChar.ImageColor.A = int(CharFade);

 	return false;
}

function MainReopened()
{
	if ( !PlayerOwner().Level.IsPendingConnection() )
	{
		i_BkChar.Image = CharShots[rand(CharShots.Length)];
		Opened(none);
		Timer();
	}
}

function InternalOnOpen()
{
    if (bNoInitDelay)
    	Timer();
    else
	    SetTimer(0.5,false);

	Controller.PerformRestore();
    PlayerOwner().ClientSetInitialMusic(MenuSong,MTRAN_Segue);
}

function OnClose(optional Bool bCancelled)
{
}

function bool MyKeyEvent(out byte Key,out byte State,float delta)
{
	if(Key == 0x1B && state == 1)	// Escape pressed
		bAllowClose = true;

	return false;
}

function bool CanClose(optional bool bCancelled)
{
	if(bAllowClose)
		ButtonClick(b_Quit);

	bAllowClose = False;
	return PlayerOwner().Level.IsPendingConnection();
}

function MoveOn()
{
	switch (Selected)
	{
		case b_SinglePlayer:
			Profile("SinglePlayer");
			Controller.OpenMenu(Controller.GetSinglePlayerPage());
			Profile("SinglePlayer");
			return;

		case b_MultiPlayer:
			Profile("ServerBrowser");
			Controller.OpenMenu(Controller.GetServerBrowserPage());
			Profile("ServerBrowser");
			return;

		case b_Host:
			Profile("MPHost");
			Controller.OpenMenu(Controller.GetMultiplayerPage());
			Profile("MPHost");
			return;

		case b_InstantAction:
			Profile("InstantAction");
			Controller.OpenMenu(Controller.GetInstantActionPage());
			Profile("InstantAction");
			return;

		case b_ModsAndDemo:
			Profile("ModsandDemos");
			Controller.OpenMenu(Controller.GetModPage());
			Profile("ModsandDemos");
			return;

		case b_Settings:
			Profile("Settings");
        	Controller.OpenMenu(Controller.GetSettingsPage());
			Profile("Settings");
			return;

		case b_Quit:
			Profile("Quit");
			Controller.OpenMenu(Controller.GetQuitPage());
			Profile("Quit");
			return;

		default:
			StopWatch(True);
	}
}

function bool ButtonClick(GUIComponent Sender)
{
	if (GUIButton(Sender) != None)
		Selected = GUIButton(Sender);

	if (Selected==None)
    	return false;

	InitAnimOut( i_TV, -0.000977, 1.668619, 0.35);
    InitAnimOut(i_UT2Logo, 0.007226,-0.392579,0.35);
    InitAnimOut(i_UT2Shader,0.249023,-0.105470,0.35);
    InitAnimOut(b_SinglePlayer,1,0.368813,0.35);
	InitAnimOut(b_Multiplayer,1.15,0.449282,0.35);
	InitAnimOut(b_Host,1.3,0.534027,0.35);
	InitAnimOut(b_InstantAction,1.45,0.618619,0.35);
    InitAnimOut(b_ModsAndDemo,1.6,0.705859,0.35);
	InitAnimOut(b_Settings,1.75,0.800327,0.35);
	InitAnimOut(b_Quit,1.9,0.887567,0.35);

    DesiredCharFade=0;
    CharFadeTime = 0.35;
    return true;
}

function InitAnimOut( GUIComponent C, float X, float Y, float Z )
{
	if ( C == None )
	{
		Warn("UT2K4MainMenu.InitAnimOut called with null component!");
		return;
	}

	C.Animate(X,Y,Z);
	C.OnEndAnimation = MenuOut_Done;
}

function MenuOut_Done(GUIComponent Sender, EAnimationType Type)
{
	Sender.OnArrival = none;
	if ( bAnimating )
		return;

    MoveOn();
}

function bool MyOnDraw(Canvas Canvas)
{
	local GUIButton FButton;
    local int i,x2;
    local float XL,YL;
	local float DeltaTime;

    if (bAnimating || !Controller.bCurMenuInitialized )
    	return false;

    DeltaTime=Controller.RenderDelta;

    for (i=0;i<Controls.Length;i++)
    {
    	if ( (GUIButton(Controls[i])!=None) )
        {

 			FButton = GUIButton(Controls[i]);
            if (FButton.Tag>0 && FButton.MenuState!=MSAT_Focused)
            {
            	FButton.Tag -= 784*DeltaTime;
                if (FButton.Tag<0)
                	FButton.Tag=0;
            }
			else if (FButton.MenuState==MSAT_Focused)
            	FButton.Tag=200;

            if (FButton.Tag>0)
            {
	            fButton.Style.TextSize(Canvas,MSAT_Focused, FButton.Caption,XL,YL,FButton.FontScale);
	            x2 = FButton.ActualLeft() + XL + 16;
	            Canvas.Style=5;
	            Canvas.SetDrawColor(255,255,255,FButton.Tag);
	            Canvas.SetPos(0,fButton.ActualTop());
				Canvas.DrawTilePartialStretched(material'Highlight',x2,FButton.ActualHeight());
            }
        }
    }

    return false;
}

event bool NotifyLevelChange()
{
	if ( bDebugging )
		log(Name@"NotifyLevelChange  PendingConnection:"$PlayerOwner().Level.IsPendingConnection());

	return PlayerOwner().Level.IsPendingConnection();
}

defaultproperties
{
	bDebugging=True
	OnOpen=InternalOnOpen
	OnCanClose=CanClose
	OnKeyEvent=MyKeyEvent
    OnReopen=MainReopened

	Begin Object Class=BackgroundImage Name=PageBackground
		Image=material'2K4Menus.Controls.mmbgnd'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
        X1=0
        Y1=0
        X2=1024
        Y2=768
	End Object

	Begin Object Class=GUIImage Name=iPanHuge
		Image=material'2K4Menus.MainMenu.PanHuge'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=40)
		WinWidth=1
		WinHeight=0.367969
		WinLeft=0
		WinTop=0.001719
        RenderWeight=0.02
        OnDraw=PanHugeDraw
	End Object

	Begin Object Class=GUIImage Name=iPanBig
		Image=material'2K4Menus.MainMenu.PanBig'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=75)
		WinWidth=1
		WinHeight=0.152422
		WinLeft=0
		WinTop=0.081771
        RenderWeight=0.023
	End Object

	Begin Object Class=GUIImage Name=iPanSmall
		Image=material'2K4Menus.MainMenu.PanSmall'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=255)
		WinWidth=1
		WinHeight=0.04
		WinLeft=0
		WinTop=0.226042
        RenderWeight=0.026
	End Object

	Begin Object Class=BackgroundImage Name=ImgBkChar
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=160)
        OnDraw=BkCharDraw
        RenderWeight=0.04
        X1=0
        Y1=0
        X2=1024
        Y2=768
        Tag=0
	End Object

	Begin Object Class=GUIImage Name=ImgUT2Logo
		Image=material'2K4Menus.MainMenu.2K4Logo'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.638868
		WinHeight=0.392579
		WinLeft=0.007226
		WinTop=0.016926
        RenderWeight=0.05
	End Object

	Begin Object Class=GUIImage Name=ImgUT2Shader
		Image=material'InterfaceContent.Logos.fbSymbolShader'
		ImageStyle=ISTY_Scaled
		WinWidth=0.155273
		WinHeight=0.105470
		WinLeft=0.249023
		WinTop=0.180988
        RenderWeight=0.06
//        OnDraw=MyOnDraw
	End Object

	Begin Object Class=GUIImage Name=ImgTV
//		Image=material'2K4Menus.MainMenu.ActionFB'
		Image=material'ULogo.MainMenu.CornerMenuFinal'
		ImageStyle=ISTY_Scaled
		WinWidth=0.500000
		WinHeight=0.668619
		WinLeft=-0.000977
		WinTop=0.332292
        RenderWeight=5.07
        X1=0
        Y1=1
        X2=512
        Y2=511
	End Object


	Begin Object Class=GUIButton Name=SinglePlayerButton
	    FontScale=FNS_Small
		StyleName="TextButton"
		Caption="Single Player"
        CaptionEffectStylename="TextButtonEffect"
        CaptionAlign=TXTA_Left
		Hint="Play through the Tournament"
		OnClick=ButtonClick
		WinWidth=0.715022
		WinHeight=0.075000
		WinLeft=0.315359
		WinTop=0.368813
		TabOrder=0
		bFocusOnWatch=true
		bUseCaptionHeight=true
	End Object

	Begin Object Class=GUIButton Name=MultiplayerButton
	    FontScale=FNS_Small
		StyleName="TextButton"
		Caption="Join Game"
        CaptionEffectStylename="TextButtonEffect"
        CaptionAlign=TXTA_Left
		Hint="Play with human opponents over a LAN or the internet"
		OnClick=ButtonClick
		WinWidth=0.659899
		WinHeight=0.075000
		WinLeft=0.363246
		WinTop=0.449282
		bFocusOnWatch=true
        TabOrder=1
		bUseCaptionHeight=true
	End Object

	Begin Object Class=GUIButton Name=HostButton
	    FontScale=FNS_Small
		StyleName="TextButton"
		Caption="Host Game"
        CaptionEffectStylename="TextButtonEffect"
        CaptionAlign=TXTA_Left
		Hint="Start a server and invite others to join your game"
		OnClick=ButtonClick
		WinWidth=0.627131
		WinHeight=0.075000
		WinLeft=0.395097
		WinTop=0.534027
		bFocusOnWatch=true
        TabOrder=2
		bUseCaptionHeight=true
	End Object

	Begin Object Class=GUIButton Name=InstantActionButton
	    FontScale=FNS_Small
		StyleName="TextButton"
		Caption="Instant Action"
        CaptionEffectStylename="TextButtonEffect"
        CaptionAlign=TXTA_Left
		Hint="Play a practice match"
		OnClick=ButtonClick
		WinWidth=0.593666
		WinHeight=0.075000
		WinLeft=0.423640
		WinTop=0.618619
		bFocusOnWatch=true
        TabOrder=3
		bUseCaptionHeight=true
	End Object

	Begin Object Class=GUIButton Name=ModsAndDemosButton
	    FontScale=FNS_Small
		StyleName="TextButton"
		Caption="Community"
        CaptionEffectStylename="TextButtonEffect"
        CaptionAlign=TXTA_Left
		Hint="Get the latest news, demos and mods from the UT2004 community"
		OnClick=ButtonClick
		WinWidth=0.574135
		WinHeight=0.075000
		WinLeft=0.433406
		WinTop=0.705859
		bFocusOnWatch=true
        TabOrder=4
		bUseCaptionHeight=true
	End Object

	Begin Object Class=GUIButton Name=SettingsButton
	    FontScale=FNS_Small
		StyleName="TextButton"
		Caption="Settings"
        CaptionEffectStylename="TextButtonEffect"
        CaptionAlign=TXTA_Left
		Hint="Change your controls and settings"
		OnClick=ButtonClick
		WinWidth=0.580478
		WinHeight=0.075000
		WinLeft=0.434477
		WinTop=0.800327
		bFocusOnWatch=true
        TabOrder=5
		bUseCaptionHeight=true
	End Object

	Begin Object Class=GUIButton Name=QuitButton
	    FontScale=FNS_Small
		StyleName="TextButton"
		Caption="Exit UT2004"
        CaptionEffectStylename="TextButtonEffect"
        CaptionAlign=TXTA_Left
		Hint="Exit the game"
		OnClick=ButtonClick
		WinWidth=0.526767
		WinHeight=0.075000
		WinLeft=0.424711
		WinTop=0.887567
		bFocusOnWatch=true
        TabOrder=6
		bUseCaptionHeight=true
	End Object

	i_Background=PageBackground
    i_PanHuge=iPanHuge
    i_PanBig=iPanBig
    i_PanSmall=iPanSmall
	i_UT2Logo=ImgUT2Logo
	i_UT2Shader=ImgUT2Shader
	i_BkChar=ImgBkChar
    i_TV=ImgTV
	b_SinglePlayer=SinglePlayerButton
	b_MultiPlayer=MultiplayerButton
	b_Host=HostButton
	b_InstantAction=InstantActionButton
    b_ModsAndDemo=ModsAndDemosButton
	b_Settings=SettingsButton
	b_Quit=QuitButton

	WinWidth=1.0
	WinHeight=1.0
	WinTop=0.0
	WinLeft=0.0

	bRenderWorld=False
	bAllowClose=False
	bAllowedAsLast=true
	bDisconnectOnOpen=true
 	CharFade=0
    CharShots(0)=material'2K4Menus.MainMenu.Char01'
    CharShots(1)=material'2K4Menus.MainMenu.Char02'
    CharShots(2)=material'2K4Menus.MainMenu.Char03'

    MenuSong="KR-UT2004-Menu"
}

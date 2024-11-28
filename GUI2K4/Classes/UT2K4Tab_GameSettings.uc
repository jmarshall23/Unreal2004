//==============================================================================
//  Contains all client-side (mostly) game configuration properties
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Tab_GameSettings extends Settings_Tabs;

var automated GUISectionBackground i_BG1, i_BG2, i_BG3, i_BG4, i_BG5;
var automated moCheckBox    ch_WeaponBob, ch_AutoSwitch, ch_Speech,
                            ch_Dodging, ch_AutoAim, ch_ClassicTrans, ch_LandShake;
var automated moComboBox    co_GoreLevel;

var GUIComponent LastGameOption;  // Hack

var bool    bBob, bDodge, bAim, bAuto, bClassicTrans, bLandShake, bLandShakeD, bSpeechRec;
var int     iGore;

// From network tab
var localized string    NetSpeedText[4];

var localized string    StatsURL;
var localized string    EpicIDMsg;

var automated GUILabel  l_Warning, l_ID;
var automated GUIButton b_Stats;
var automated moCheckBox    ch_TrackStats, ch_DynNetspeed, ch_Precache;
var automated moComboBox    co_Netspeed;
var automated moEditBox     ed_Name, ed_Password;

var int iNetspeed, iNetSpeedD;
var string sPassword, sName;
var bool bStats, bDynNet, bPrecache;


function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;

    Super.InitComponent(MyController, MyOwner);
    if ( class'GameInfo'.Default.bAlternateMode )
    	RemoveComponent(co_GoreLevel);
    else
    {
    	for (i = 0; i < ArrayCount(class'GameInfo'.default.GoreLevelText); i++)
    		co_GoreLevel.AddItem(class'GameInfo'.default.GoreLevelText[i]);
    }

    LastGameOption = ch_LandShake;

    // Network
    for(i = 0;i < ArrayCount(NetSpeedText);i++)
        co_Netspeed.AddItem(NetSpeedText[i]);

    ed_Name.MyEditBox.bConvertSpaces = true;
    ed_Name.MyEditBox.MaxWidth=14;

    ed_Password.MyEditBox.MaxWidth=14;

    ed_Password.MaskText(true);
    l_ID.Caption = FormatID(PlayerOwner().GetPlayerIDHash());

}

function string FormatID(string id)
{
    id=Caps(id);
    return mid(id,0,8)$"-"$mid(id,8,8)$"-"$mid(id,16,8)$"-"$mid(id,24,8);
}

function ShowPanel(bool bShow)
{
    Super.ShowPanel(bShow);
    if ( bShow )
	{
		if ( bInit )
	    {
            i_BG1.ManageComponent(ch_WeaponBob);
            i_BG1.Managecomponent(ch_AutoSwitch);
            i_BG1.ManageComponent(ch_Dodging);
            i_BG1.ManageComponent(ch_AutoAim);
            i_BG1.ManageComponent(ch_ClassicTrans);
            i_BG1.ManageComponent(ch_LandShake);
	        i_BG1.Managecomponent(co_GoreLevel);

            // No speech recognition except on win32... --ryan.
            if ( (!PlatformIsWindows()) || (PlatformIs64Bit()) )
                ch_Speech.DisableMe();

	    }
    	UpdateStatsItems();
    }
}

function InternalOnLoadINI(GUIComponent Sender, string s)
{
	local int i;
    local PlayerController PC;

    if (GUIMenuOption(Sender) != None)
    {
        PC = PlayerOwner();

        switch (GUIMenuOption(Sender))
        {
            case ch_AutoSwitch:
                bAuto = !PC.bNeverSwitchOnPickup;
                ch_AutoSwitch.Checked(bAuto);
                break;

            case ch_WeaponBob:
            	if ( PC.Pawn != None )
            		bBob = PC.Pawn.bWeaponBob;
            	else bBob = class'Pawn'.default.bWeaponBob;
                ch_WeaponBob.Checked(bBob);
                break;

            case co_GoreLevel:
            	if ( PC.Level.Game != None )
            		iGore = PC.Level.Game.GoreLevel;
                else iGore = class'GameInfo'.default.GoreLevel;
                co_GoreLevel.SetIndex(iGore);
                break;

            case ch_Dodging:
                bDodge = PC.DodgingIsEnabled();
                ch_Dodging.Checked(bDodge);
                break;

            case ch_AutoAim:
                bAim = PC.bAimingHelp;
                ch_AutoAim.Checked(bAim);
                break;

            case ch_ClassicTrans:
            	if ( xPlayer(PC) != None )
            		bClassicTrans = xPlayer(PC).bClassicTrans;
                else bClassicTrans = class'xPlayer'.default.bClassicTrans;
                ch_ClassicTrans.Checked(bClassicTrans);
                break;

			case ch_LandShake:
				bLandShake = PC.bLandingShake;
				ch_LandShake.Checked(bLandShake);
				break;

            case ch_Speech:
            	bSpeechRec = bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager UseSpeechRecognition"));
            	ch_Speech.SetComponentValue(bSpeechRec, True);
            	break;

        // Network
        	case ch_Precache:
        		bPrecache = PC.Level.bDesireSkinPreload;
        		ch_Precache.Checked(bPrecache);
        		break;

            case co_Netspeed:
            	if ( PC.Player != None )
            		i = PC.Player.ConfiguredInternetSpeed;
            	else i = class'Player'.default.ConfiguredInternetSpeed;

                if (i <= 2600)
                    iNetSpeed = 0;

                else if (i <= 5000)
                    iNetSpeed = 1;

                else if (i <= 10000)
                    iNetSpeed = 2;

                else iNetSpeed = 3;

				iNetSpeedD = iNetSpeed;
                co_NetSpeed.SetIndex(iNetSpeed);
                break;

            case ed_Name:
                sName = PC.StatsUserName;
                ed_Name.SetText(sName);
                break;

            case ed_Password:
                sPassword = PC.StatsPassword;
                ed_Password.SetText(sPassword);
                break;

            case ch_TrackStats:
            	bStats = PC.bEnableStatsTracking;
                ch_TrackStats.Checked(bStats);
                UpdateStatsItems();
                break;

            case ch_DynNetspeed:
                bDynNet = PC.bDynamicNetSpeed;
                ch_DynNetspeed.Checked(bDynNet);
                break;

        }
    }
}

function SaveSettings()
{
    local PlayerController PC;
    local bool bSave;

	Super.SaveSettings();
    PC = PlayerOwner();

    if ( bSpeechRec != bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager UseSpeechRecognition")) )
    	PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager UseSpeechRecognition"@bSpeechRec);

    if ( xPlayer(PC) != None && xPlayer(PC).bClassicTrans != bClassicTrans)
    {
        xPlayer(PC).bClassicTrans = bClassicTrans;
        xPlayer(PC).ServerSetClassicTrans(bClassicTrans);
        bSave = True;
    }

    else if (class'XGame.xPlayer'.default.bClassicTrans != bClassicTrans)
    {
        class'XGame.xPlayer'.default.bClassicTrans = bClassicTrans;
        class'XGame.xPlayer'.static.StaticSaveConfig();
    }

	if (PC.bLandingShake != bLandShake)
	{
		PC.bLandingShake = bLandShake;
		bSave = True;
	}

    if (PC.DodgingIsEnabled() != bDodge)
    {
        PC.SetDodging(bDodge);
        bSave = True;
    }

    if (PC.bNeverSwitchOnPickup == bAuto)
    {
        PC.bNeverSwitchOnPickup = !bAuto;
        bSave = True;
    }

    if ( PC.bAimingHelp != bAim )
    {
    	PC.bAimingHelp = bAim;
    	bSave = True;
    }

    if (PC.Pawn != None)
    {

        PC.Pawn.bWeaponBob = bBob;
        PC.Pawn.SaveConfig();
    }
    else if (class'Engine.Pawn'.default.bWeaponBob != bBob)
    {
        class'Engine.Pawn'.default.bWeaponBob = bBob;
        class'Engine.Pawn'.static.StaticSaveConfig();
    }

    if (PC.Level != None && PC.Level.Game != None)
    {
    	if ( PC.Level.Game.GoreLevel != iGore )
    	{
	        PC.Level.Game.GoreLevel = iGore;
	        PC.Level.Game.SaveConfig();
	    }
    }
    else
    {
		if ( class'Engine.GameInfo'.default.GoreLevel != iGore )
		{
	        class'Engine.GameInfo'.default.GoreLevel = iGore;
	        class'Engine.GameInfo'.static.StaticSaveConfig();
	    }
    }

	// Network
	if ( bPrecache != PC.Level.bDesireSkinPreload )
	{
		PC.Level.bDesireSkinPreload = bPrecache;
		PC.Level.SaveConfig();
	}

	if ( iNetSpeed != iNetSpeedD || (class'Player'.default.ConfiguredInternetSpeed == 9636) )
	{
		if ( PC.Player != None )
		{
			switch (iNetSpeed)
			{
				case 0: PC.Player.ConfiguredInternetSpeed = 2600; break;
				case 1: PC.Player.ConfiguredInternetSpeed = 5000; break;
				case 2: PC.Player.ConfiguredInternetSpeed = 10000; break;
				case 3: PC.Player.ConfiguredInternetSpeed = 20000; break;
			}

			PC.Player.SaveConfig();
		}

		else
		{
			switch (iNetSpeed)
			{
				case 0: class'Player'.default.ConfiguredInternetSpeed = 2600; break;
				case 1: class'Player'.default.ConfiguredInternetSpeed = 5000; break;
				case 2: class'Player'.default.ConfiguredInternetSpeed = 10000; break;
				case 3: class'Player'.default.ConfiguredInternetSpeed = 15000; break;
			}

			class'Player'.static.StaticSaveConfig();
		}
	}

	if ( bStats != PC.bEnableStatsTracking )
	{
		PC.bEnableStatsTracking = bStats;
		bSave = True;
	}

	if ( sName != PC.StatsUserName )
	{
		PC.StatsUserName = sName;
		bSave = True;
	}

	if ( PC.StatsPassword != sPassword )
	{
		PC.StatsPassword = sPassword;
		bSave = True;
	}

	if ( PC.bDynamicNetSpeed != bDynNet )
	{
		PC.bDynamicNetSpeed = bDynNet;
		bSave = True;
	}


    if (bSave)
        PC.SaveConfig();
}

function ResetClicked()
{
    local class<Client> ViewportClass;
    local bool bTemp;
    local PlayerController PC;
    local int i;

    Super.ResetClicked();

	PC = PlayerOwner();
    ViewportClass = class<Client>(DynamicLoadObject(GetNativeClassName("Engine.Engine.ViewportManager"), class'Class'));

    ViewportClass.static.ResetConfig("UseSpeechRecognition");
    ViewportClass.static.ResetConfig("ScreenFlashes");
    class'XGame.xPlayer'.static.ResetConfig("bClassicTrans");

    PC.ResetConfig("bNeverSwitchOnPickup");
    PC.ResetConfig("bEnableDodging");
	PC.ResetConfig("bLandingShake");
	PC.ResetConfig("bAimingHelp");

    class'Engine.Pawn'.static.ResetConfig("bWeaponBob");
    class'Engine.GameInfo'.static.ResetConfig("GoreLevel");

    // Network
    class'Engine.Player'.static.ResetConfig("ConfiguredInternetSpeed");
    class'Engine.LevelInfo'.static.ResetConfig("bDesireSkinPreload");
    PC.ResetConfig("bEnableStatsTracking");
    PC.ClearConfig("StatsUserName");
    PC.ClearConfig("StatsPassword");
    PC.ResetConfig("bDynamicNetSpeed");

    bTemp = Controller.bCurMenuInitialized;
    Controller.bCurMenuInitialized = False;

    for (i = 0; i < Components.Length; i++)
        Components[i].LoadINI();

    Controller.bCurMenuInitialized = bTemp;
}

function InternalOnChange(GUIComponent Sender)
{
    local PlayerController PC;

	Super.InternalOnChange(Sender);
    if (GUIMenuOption(Sender) != None)
    {
        PC = PlayerOwner();

        switch (GUIMenuOption(Sender))
        {
            case ch_Speech:
            	bSpeechRec = ch_Speech.IsChecked();
            	break;

            case ch_AutoSwitch:
                bAuto = ch_AutoSwitch.IsChecked();
                break;

            case ch_WeaponBob:
                bBob = ch_WeaponBob.IsChecked();
                break;

            case co_GoreLevel:
                iGore = co_GoreLevel.GetIndex();
                break;

            case ch_Dodging:
                bDodge = ch_Dodging.IsChecked();
                break;

            case ch_AutoAim:
                bAim = ch_AutoAim.IsChecked();
                break;

            case ch_ClassicTrans:
                bClassicTrans = ch_ClassicTrans.IsChecked();
                break;

			case ch_LandShake:
				bLandShake = ch_LandShake.IsChecked();
				break;

		// Network
        	case ch_Precache:
        		bPrecache = ch_Precache.IsChecked();
        		break;

            case co_Netspeed:
                iNetSpeed = co_NetSpeed.GetIndex();
                break;

            case ed_Name:
                sName = ed_Name.GetText();
                break;

            case ed_Password:
                sPassword = ed_Password.GetText();
                break;

            case ch_TrackStats:
            	bStats = ch_TrackStats.IsChecked();
                UpdateStatsItems();
                break;

            case ch_DynNetspeed:
                bDynNet = ch_DynNetspeed.IsChecked();
                break;

        }
    }

    l_Warning.SetVisibility(!ValidStatConfig());
}

function bool ValidStatConfig()
{
    if(bStats)
    {
        if(Len(ed_Name.GetText()) < 4)
            return false;

        if(Len(ed_Password.GetText()) < 6)
            return false;
    }

    return true;
}

function bool OnViewStats(GUIComponent Sender)
{
    PlayerOwner().ConsoleCommand("start"@StatsURL);
    return true;
}

function UpdateStatsItems()
{
	if ( bStats )
	{
		EnableComponent(ed_Name);
		EnableComponent(ed_Password);
		EnableComponent(b_Stats);
	}
	else
	{
		DisableComponent(ed_Name);
		DisableComponent(ed_Password);
		DisableComponent(b_Stats);
	}

	l_Warning.SetVisibility(!ValidStatConfig());
}

defaultproperties
{
    Begin Object class=GUISectionBackground Name=GameBK1
		WinWidth=0.449609
		WinHeight=0.748936
		WinLeft=0.014649
		WinTop=0.033853
        RenderWeight=0.1001
        Caption="Gameplay"
    End Object
    i_BG1=GameBK1

    Begin Object class=GUISectionBackground Name=GameBK2
		WinWidth=0.496484
		WinHeight=0.199610
		WinLeft=0.486328
		WinTop=0.033853
        RenderWeight=0.1002
		Caption="Network"
    End Object
    i_BG2=GameBK2

    Begin Object class=GUISectionBackground Name=GameBK3
		WinWidth=0.496484
		WinHeight=0.308985
		WinLeft=0.486328
		WinTop=0.240491
        RenderWeight=0.1002
		Caption="Stats"
    End Object
    i_BG3=GameBK3

    Begin Object class=GUISectionBackground Name=GameBK4
		WinWidth=0.496484
		WinHeight=0.219141
		WinLeft=0.486328
		WinTop=0.559889
        RenderWeight=0.1002
		Caption="Misc"
    End Object
    i_BG4=GameBK4

    Begin Object class=GUISectionBackground Name=GameBK5
		WinWidth=0.965712
		WinHeight=0.200706
		WinLeft=0.017419
		WinTop=0.791393
        RenderWeight=0.1002
		Caption="Unique ID / Messages"
    End Object
    i_BG5=GameBK5

    Begin Object class=moCheckBox Name=GameWeaponBob
        WinWidth=0.400000
        WinLeft=0.050000
        WinTop=0.290780
        Caption="Weapon Bob"
        INIOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Hint="Prevent your weapon from bobbing up and down while moving"
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
        ComponentWidth=-1
        CaptionWidth=0.8
        RenderWeight=1.04
        TabOrder=1
    End Object
    ch_WeaponBob=GameWeaponBob

    Begin Object class=moComboBox Name=GameGoreLevel
        WinWidth=0.400000
        WinLeft=0.050000
        WinTop=0.415521
        Caption="Gore Level"
        INIOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Hint="Configure the amount of blood and gore you see while playing the game"
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
        ComponentWidth=-1
        CaptionWidth=0.5
        RenderWeight=1.04
        TabOrder=2
        bReadOnly=True
    End Object
    co_GoreLevel=GameGoreLevel

    Begin Object class=moCheckBox Name=GameDodging
        WinWidth=0.400000
        WinLeft=0.050000
        WinTop=0.541563
        Caption="Dodging"
        INIOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Hint="Turn this option off to disable special dodge moves."
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
        ComponentWidth=-1
        CaptionWidth=0.8
        RenderWeight=1.04
        TabOrder=3
    End Object
    ch_Dodging=GameDodging

    Begin Object class=moCheckBox Name=GameAutoAim
        WinWidth=0.400000
        WinLeft=0.050000
        WinTop=0.692344
        Caption="Auto Aim"
        INIOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Hint="Enabling this option will activate computer-assisted aiming in single player games."
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
        ComponentWidth=-1
        CaptionWidth=0.8
        RenderWeight=1.04
        TabOrder=4
    End Object
    ch_AutoAim=GameAutoAim

    Begin Object class=moCheckBox Name=GameClassicTrans
        Caption="High Beacon Trajectory"
        IniOption="@Internal"
        OnChange=InternalOnChange
        OnLoadINI=InternalOnLoadINI
        Hint="Enable to use traditional-style high translocator beacon toss trajectory"
        bSquare=True
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
        ComponentWidth=-1
        CaptionWidth=0.8
        RenderWeight=1.04
        TabOrder=5
    End Object
    ch_ClassicTrans=GameClassicTrans

    Begin Object class=moCheckBox Name=WeaponAutoSwitch
        Caption="Weapon Switch On Pickup"
        Hint="Automatically change weapons when you pick up a better one."
        IniOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        bSquare=true
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
        ComponentWidth=-1
        CaptionWidth=0.8
        RenderWeight=1.04
        TabOrder=6
    End Object
    ch_AutoSwitch=WeaponAutoSwitch

	Begin Object class=moCheckBox Name=LandShaking
		WinWidth=0.266797
		WinLeft=0.705430
		WinTop=0.150261
		Caption="Landing Viewshake"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		Hint="Enable view shaking upon landing."
		CaptionWidth=0.9
		bSquare=True
		bHeightFromComponent=False
		ComponentJustification=TXTA_Right
		LabelJustification=TXTA_Left
		TabOrder=7
	End Object
	ch_LandShake=LandShaking

    // Network
    Begin Object class=moComboBox Name=OnlineNetSpeed
		WinWidth=0.419297
		WinLeft=0.528997
		WinTop=0.122944
        Caption="Connection"
        INIOption="@Internal"
        INIDefault="Cable Modem/DSL"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Hint="How fast is your connection?"
        CaptionWidth=0.55
        ComponentJustification=TXTA_Left
        bReadOnly=true
        bHeightFromComponent=false
        TabOrder=8
    End Object
    co_Netspeed=OnlineNetSpeed

    Begin Object class=moCheckBox Name=NetworkDynamicNetspeed
		WinWidth=0.419297
		WinLeft=0.528997
		WinTop=0.166017
        Caption="Dynamic Netspeed"
        Hint="Netspeed is automatically adjusted based on the speed of your network connection"
        CaptionWidth=0.94
        bSquare=true
        ComponentJustification=TXTA_Left
        IniOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        TabOrder=9
    End Object
    ch_DynNetspeed=NetworkDynamicNetspeed

    Begin Object class=moCheckBox Name=OnlineTrackStats
		WinWidth=0.170273
		WinLeft=0.642597
		WinTop=0.321733
        Caption="Track Stats"
        INIOption="@Internal"
        INIDefault="True"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Hint="Enable this option to join the online ranking system."
        CaptionWidth=0.9
        bSquare=true
        ComponentJustification=TXTA_Left
        TabOrder=10
    End Object
    ch_TrackStats=OnlineTrackStats

    Begin Object class=moEditBox Name=OnlineStatsName
		WinWidth=0.419316
		WinLeft=0.524912
		WinTop=0.373349
        INIOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Caption="UserName"
        Hint="Please select a name to use for UT Stats!"
        CaptionWidth=0.4
        TabOrder=11
    End Object
    ed_Name=OnlineStatsName

    Begin Object class=moEditBox Name=OnlineStatsPW
		WinWidth=0.419316
		WinLeft=0.524912
		WinTop=0.430677
        Caption="Password"
        INIOption="@Internal"
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        Hint="Please select a password that will secure your UT Stats!"
        CaptionWidth=0.4
        TabOrder=12
    End Object
    ed_Password=OnlineStatsPW

    Begin Object class=GUIButton Name=ViewOnlineStats
		WinWidth=0.166055
		WinHeight=0.05
		WinLeft=0.780383
		WinTop=0.469391
        Caption="View Stats"
        Hint="Click to launch the UT stats website."
        OnClick=OnViewStats
        TabOrder=13
    End Object
    b_Stats=ViewOnlineStats

    Begin Object Class=moCheckBox Name=SpeechRecognition
		WinWidth=0.403353
		WinLeft=0.540058
		WinTop=0.654527
    	Caption="Speech Recognition"
    	Hint="Enable speech recognition"
    	IniOption="@Internal"
    	OnLoadIni=InternalOnLoadIni
    	OnChange=InternalOnChange
    	ComponentJustification=TXTA_Left
    	CaptionWidth=0.9
    	TabOrder=14
		bBoundToParent=True
		bScaleToParent=True
    End Object
    ch_Speech=SpeechRecognition


    Begin Object class=moCheckBox Name=PrecacheSkins
		WinWidth=0.403353
		WinLeft=0.540058
		WinTop=0.707553
    	Caption="Preload all player skins"
    	Hint="Preloads all player skins, increasing level load time but reducing hitches during network games.  You must have at least 512 MB of system memory to use this option."
    	CaptionWidth=0.9
    	bSquare=True
    	ComponentJustification=TXTA_Left
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadINI
    	OnChange=InternalOnChange
    	TabOrder=15
    End Object
    ch_Precache=PrecacheSkins

    Begin Object class=GUILabel Name=InvalidWarning
		WinWidth=0.887965
		WinHeight=0.058335
		WinLeft=0.057183
		WinTop=0.916002
        Caption="Your stats username or password is invalid.  Your username must be at least 4 characters long, and your password must be at least 6 characters long."
        TextAlign=TXTA_Center
        bMultiLine=True
        TextColor=(R=255,G=255,B=0,A=255)
        TextFont="UT2SmallFont"
    End Object
    l_Warning=InvalidWarning

    Begin Object class=GUILabel Name=EpicID
		WinWidth=0.888991
		WinHeight=0.067703
		WinLeft=0.054907
		WinTop=0.858220
        Caption="Your Unique id is:"
        TextAlign=TXTA_Center
        bMultiLine=false
        StyleName="TextLabel"
        RenderWeight=0.2
    End Object
    l_ID=EpicID

    NetSpeedText(0)="Modem"
    NetSpeedText(1)="ISDN"
    NetSpeedText(2)="Cable/ADSL"
    NetSpeedText(3)="LAN/T1"

    StatsURL="http://ut2004stats.epicgames.com/"
    EpicIDMsg="Your Unique id is:"

    WinTop=0.15
    WinLeft=0
    WinWidth=1
    WinHeight=0.74
    bAcceptsInput=false

    PanelCaption="Game"
}

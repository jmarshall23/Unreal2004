//====================================================================
//  Base class for all main menu pages.
//
//  Written by Ron Prestenback
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class UT2K4MainPage extends UT2K4GUIPage;

var automated GUITabControl     c_Tabs;

var automated GUIHeader         t_Header;
var automated ButtonFooter      t_Footer;
var automated BackgroundImage   i_Background;

var automated	GUIImage 	    i_bkChar, i_bkScan;

/** file where the higscores are stored */
var string HighScoreFile;
var private globalconfig string TotalUnlockedCharacters;

var array<string>               PanelClass;
var localized array<string>     PanelCaption;
var localized array<string>     PanelHint;



function InitComponent(GUIController MyC, GUIComponent MyO)
{
    Super.InitComponent(MyC, MyO);

    c_Tabs.MyFooter = t_Footer;
    t_Header.DockedTabs = c_Tabs;
}

function InternalOnChange(GUIComponent Sender);
/*
event Opened(GUIComponent Sender)
{
	Super.Opened(Sender);

	if ( bPersistent && c_Tabs != None && c_Tabs.FocusedControl == None )
		c_Tabs.SetFocus(None);
}
*/

function HandleParameters(string Param1, string Param2)
{
	if ( Param1 != "" )
	{
		if ( c_Tabs != none )
			c_Tabs.ActivateTabByName(Param1, True);
	}
}

function bool GetRestoreParams( out string Param1, out string Param2 )
{
	if ( c_Tabs != None && c_Tabs.ActiveTab != None )
	{
		Param1 = c_Tabs.ActiveTab.Caption;
		return True;
	}

	return False;
}

static function bool UnlockCharacter( string CharName )
{
	local int i;
	local array<string> Unlocked;

	if ( CharName == "" )
		return false;

	Split(default.TotalUnlockedCharacters, ";", Unlocked);
	for ( i = 0; i < Unlocked.Length; i++ )
		if ( Unlocked[i] ~= CharName )
			return false;

	Unlocked[Unlocked.Length] = CharName;

	default.TotalUnlockedCharacters = JoinArray(Unlocked, ";", True);
	StaticSaveConfig();
	return true;
}

// Caution: you must verify that the specified CharName is a locked character by default, before calling this function
static function bool IsUnlocked( string CharName )
{
	return CharName != "" && (InStr(";" $ default.TotalUnlockedCharacters $ ";", ";" $ CharName $ ";") != -1);
}

defaultproperties
{
    Begin Object Class=GUITabControl Name=PageTabs
        WinWidth=0.98
        WinLeft=0.01
        WinTop=0.0
        WinHeight=0.04
        TabHeight=0.04
        TabOrder=3
        RenderWeight=0.49
        bFillSpace=False
        bAcceptsInput=True
        bDockPanels=True
        OnChange=InternalOnChange
        BackgroundStyleName="TabBackground"
    End Object

    Begin Object Class=BackgroundImage Name=PageBackground
//        Image=material'2K4Menus.Controls.menuBackground'
        ImageStyle=ISTY_PartialScaled
		Image=Material'2K4Menus.BKRenders.bgndTile'
        X1=0
        Y1=0
        X2=4
        Y2=768
        RenderWeight=0.01
    End Object

    Begin Object Class=BackgroundImage Name=PageScanLine
        ImageStyle=ISTY_Tiled
		Image=Material'2K4Menus.BKRenders.ScanLines'
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=32)
		WinWidth=1.0
		WinHeight=1.0
		WinLeft=0.0
		WinTop=0.0
		X1=0
		Y1=0
		X2=32
		Y2=32
		RenderWeight=0.03
    End Object

    Begin Object Class=GUIImage Name=BkChar
        Image=material'2K4Menus.BKRenders.Char01'
        ImageStyle=ISTY_Scaled
        ImageColor=(R=255,G=255,B=255,A=255)
        X1=0
        Y1=0
        X2=1024
        Y2=768
        WinWidth=1
        WinHeight=1
        WinTop=0
        WinLeft=0
        RenderWeight=0.02
    End Object

    WinWidth=1.0
    WinHeight=1.0
    WinLeft=0.0
    WinTop=0.0

    bRenderWorld=False
    bRequire640x480=True
    bPersistent=True
    bRestorable=True

    Background=none
	i_bkScan=PageScanLine
    i_Background=PageBackground
    i_BkChar=BkChar
    c_Tabs=PageTabs
	HighScoreFile="UT2004HighScores"
}

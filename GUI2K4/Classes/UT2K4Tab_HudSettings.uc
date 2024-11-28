//==============================================================================
//	Created on: 07/10/03
//	HUD Configuration Menu
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4Tab_HudSettings extends Settings_Tabs;

var automated GUISectionBackground i_BG1, i_BG2;
var automated GUIImage	i_Scale, i_PreviewBG, i_Preview;
var automated moSlider	sl_Scale, sl_Opacity, sl_Red, sl_Green, sl_Blue;
var automated moNumericEdit	nu_MsgCount, nu_MsgScale, nu_MsgOffset;
var automated moCheckBox	ch_Visible, ch_Weapons, ch_Personal, ch_Score, ch_WeaponBar,
							ch_Portraits,  ch_VCPortraits, ch_DeathMsgs, ch_EnemyNames, ch_CustomColor;


var automated GUIComboBox co_CustomHUD;
var automated GUIButton b_CustomHUD;

var() bool bVis, bWeapons, bPersonal, bScore, bPortraits, bVCPortraits, bNames, bCustomColor, bNoMsgs, bWeaponBar;
var() int iCount, iScale, iOffset;
var() float fScale, fOpacity;
var() color cCustom;

var() floatbox DefaultBGPos, DefaultHealthPos;

var array<CacheManager.GameRecord> Games;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;

	Super.InitComponent(MyController, MyOwner);

	class'CacheManager'.static.GetGameTypeList( Games );
	for ( i = 0; i < Games.Length; i++ )
	{
		if ( Games[i].HUDMenu != "" )
			co_CustomHUD.AddItem( Games[i].GameName, , string(i) );
	}

	if ( co_CustomHUD.ItemCount() == 0 )
	{
		RemoveComponent(co_CustomHUD);
		RemoveComponent(b_CustomHUD);
	}

	i_BG1.ManageComponent(ch_Visible);
    i_BG1.ManageComponent(ch_EnemyNames);
    i_BG1.ManageComponent(ch_WeaponBar);
    i_BG1.ManageComponent(ch_Weapons);
    i_BG1.ManageComponent(ch_Personal);
    i_BG1.ManageComponent(ch_Score);
    i_BG1.ManageComponent(ch_Portraits);
    i_BG1.ManageComponent(ch_VCPortraits);
    i_BG1.ManageComponent(ch_DeathMsgs);
    i_BG1.ManageComponent(nu_MsgCount);
    i_BG1.ManageComponent(nu_MsgScale);
    i_BG1.ManageComponent(nu_MsgOffset);

    sl_Opacity.MySlider.bDrawPercentSign = True;
    sl_Scale.MySlider.bDrawPercentSign = True;
}

function bool InternalOnClick(GUIComponent Sender)
{
	local int i;

	if ( Sender == b_CustomHUD )
	{
		i = int(co_CustomHUD.GetExtra());

		Controller.OpenMenu(Games[i].HUDMenu,Games[i].ClassName);
	}

	return true;
}

function InternalOnLoadINI(GUIComponent Sender, string s)
{
	local HUD H;

	H = PlayerOwner().myHUD;
	switch (Sender)
	{
    case ch_DeathMsgs:
        bNoMsgs = class'XGame.xDeathMessage'.default.bNoConsoleDeathMessages;
        ch_DeathMsgs.SetComponentValue(bNoMsgs,true);
        break;

	case ch_Visible:
		bVis = H.bHideHUD;
		ch_Visible.SetComponentValue(bVis,true);
		break;

	case ch_Weapons:
		bWeapons = H.bShowWeaponInfo;
		ch_Weapons.SetComponentValue(bWeapons,true);
		break;

	case ch_Personal:
		bPersonal = H.bShowPersonalInfo;
		ch_Personal.SetComponentValue(bPersonal,true);
		break;

	case ch_Score:
		bScore = H.bShowPoints;
		ch_Score.SetComponentValue(bScore,true);
		break;

	case ch_WeaponBar:
		bWeaponBar = H.bShowWeaponBar;
		ch_WeaponBar.SetComponentValue(bWeaponBar,true);
		break;

	case ch_Portraits:
		bPortraits = H.bShowPortrait;
		ch_Portraits.SetComponentValue(bPortraits,true);
		break;

	case ch_EnemyNames:
		bNames = !H.bNoEnemyNames;
		ch_EnemyNames.SetComponentValue(bNames,true);
		break;

	case nu_MsgCount:
		iCount = H.ConsoleMessageCount;
		nu_MsgCount.SetComponentValue(iCount,true);
		break;

	case nu_MsgScale:
		iScale = 8 - H.ConsoleFontSize;
		nu_MsgScale.SetComponentValue(iScale,true);
		break;

	case nu_MsgOffset:
		iOffset = H.MessageFontOffset+4;
		nu_MsgOffset.SetComponentValue(iOffset,true);
		break;

	case ch_CustomColor:
		bCustomColor = UsingCustomColor();
		ch_CustomColor.SetComponentValue(bCustomColor,true);
		InitializeHUDColor();
		break;

	case ch_VCPortraits:
		bVCPortraits = H.bShowPortraitVC;
		ch_VCPortraits.SetComponentValue(bVCPortraits,true);
		break;

	default:
		log(Name@"Unknown component calling LoadINI:"$ GUIMenuOption(Sender).Caption);
		GUIMenuOption(Sender).SetComponentValue(s,true);
	}
}

function InitializeHUDColor()
{
	if (bCustomColor)
		cCustom = class'HudBase'.default.CustomHUDColor;

	else
	{	cCustom = GetDefaultColor();
		sl_Red.DisableMe();
		sl_Blue.DisableMe();
		sl_Green.DisableMe();
	}

	fScale = PlayerOwner().myHUD.HudScale * 100;
	fOpacity = (PlayerOwner().myHUD.HudOpacity / 255) * 100;

	sl_Scale.SetValue(fScale);
	sl_Opacity.SetValue(fOpacity);

	sl_Red.SetValue(cCustom.R);
	sl_Blue.SetValue(cCustom.B);
	sl_Green.SetValue(cCustom.G);


	UpdatePreviewColor();
}

function bool UsingCustomColor()
{
	if ( PlayerOwner() != None && PlayerOwner().myHUD != None && HudBase(PlayerOwner().myHUD) != None )
		return HudBase(PlayerOwner().myHUD).bUsingCustomHUDColor;

	return
	class'HudBase'.default.CustomHUDColor.R != 0 ||
	class'HudBase'.default.CustomHUDColor.B != 0 ||
	class'HudBase'.default.CustomHUDColor.G != 0 ||
	class'HudBase'.default.CustomHUDColor.A != 0;
}

function color GetDefaultColor()
{
	local int i;
	local PlayerController PC;

	PC = PlayerOwner();
	if (PC.PlayerReplicationInfo == None || PC.PlayerReplicationInfo.Team == None)
		i = int(PC.GetUrlOption("Team"));
	else i = PC.PlayerReplicationInfo.Team.TeamIndex;

	if (HudBase(PC.myHUD) != None)
		return HudBase(PC.myHUD).GetTeamColor(i);

	return class'HudBase'.static.GetTeamColor(i);
}

function SaveSettings()
{
	local PlayerController PC;
	local HUD H;
	local bool bSave;

	Super.SaveSettings();
	PC = PlayerOwner();
	H = PC.myHUD;

	if ( H.bHideHud != bVis )
	{
		H.bHideHUD = bVis;
		bSave = True;
	}

	if ( H.bShowWeaponInfo != bWeapons )
	{
		H.bShowWeaponInfo = bWeapons;
		bSave = True;
	}

	if ( H.bShowPersonalInfo != bPersonal )
	{
		H.bShowPersonalInfo = bPersonal;
		bSave = True;
	}

	if ( H.bShowPoints != bScore )
	{
		H.bShowPoints = bScore;
		bSave = True;
	}

	if ( H.bShowWeaponBar != bWeaponBar)
	{
		H.bShowWeaponBar = bWeaponBar;
		bSave = True;
	}

	if ( H.bShowPortrait != bPortraits )
	{
		H.bShowPortrait = bPortraits;
		bSave = True;
	}

	if ( H.bShowPortraitVC != bVCPortraits )
	{
		H.bShowPortraitVC = bVCPortraits;
		bSave = True;
	}

	if ( H.bNoEnemyNames == bNames )
	{
		H.bNoEnemyNames = !bNames;
		bSave = True;
	}

	if ( H.ConsoleMessageCount != iCount )
	{
		H.ConsoleMessageCount = iCount;
		bSave = True;
	}

	if ( H.ConsoleFontSize != Abs(iScale - 8) )
	{
		H.ConsoleFontSize = Abs(iScale - 8);
		bSave = True;
	}

	if ( H.MessageFontOffset != iOffset - 4 )
	{
		H.MessageFontOffset = iOffset - 4;
		bSave = True;
	}

	if ( H.HudScale != fScale / 100.0 )
	{
		H.HudScale = fScale / 100.0;
		bSave = True;
	}

	if ( H.HudOpacity != (fOpacity / 100.0) * 255.0 )
	{
		H.HudOpacity = (fOpacity / 100.0) * 255.0;
		bSave = True;
	}

	if ( HudBase(H) != None )
	{
		if ( SaveCustomHUDColor() || bSave )
			H.SaveConfig();
	}

	else
	{
		if ( bSave )
			H.SaveConfig();

		SaveCustomHUDColor();
	}

    if ( class'XGame.xDeathMessage'.default.bNoConsoleDeathMessages != bNoMsgs )
    {
		class'XGame.xDeathMessage'.default.bNoConsoleDeathMessages = bNoMsgs;
		class'XGame.xDeathMessage'.static.StaticSaveConfig();
	}
}

function ResetClicked()
{
	local int i;

	Super.ResetClicked();

	class'HUD'.static.ResetConfig("bHideHUD");
	class'HUD'.static.ResetConfig("bShowWeaponInfo");
	class'HUD'.static.ResetConfig("bShowPersonalInfo");
	class'HUD'.static.ResetConfig("bShowPoints");
	class'HUD'.static.ResetConfig("bShowWeaponBar");
	class'HUD'.static.ResetConfig("bShowPortrait");
	class'HUD'.static.ResetConfig("bShowPortraitVC");
	class'HUD'.static.ResetConfig("bNoEnemyNames");
	class'HUD'.static.ResetConfig("ConsoleMessageCount");
	class'HUD'.static.ResetConfig("ConsoleFontSize");
	class'HUD'.static.ResetConfig("MessageFontOffset");
	class'HUD'.static.ResetConfig("HudScale");
	class'HUD'.static.ResetConfig("HudOpacity");

	class'HudBase'.static.ResetConfig("CustomHudColor");
    class'XGame.xDeathMessage'.static.ResetConfig("bNoConsoleDeathMessages");

	for (i = 0; i < Components.Length; i++)
		Components[i].LoadINI();
}

function bool SaveCustomHUDColor()
{
	local color Temp;
	local HudBase Base;

	Base = HudBase(PlayerOwner().myHUD);
	if ( Base != None )
	{
		if ( bCustomColor )
		{
			if ( Base.CustomHUDColor != cCustom )
			{
				Base.CustomHUDColor = cCustom;
				Base.SetCustomHUDColor();
				return true;
			}
		}
		else if ( Base.CustomHUDColor != Temp )
		{
			Base.CustomHUDColor = Temp;
			Base.SetCustomHUDColor();
			return true;
		}
	}
	else
	{
		if ( bCustomColor )
		{
			if ( class'HudBase'.default.CustomHUDColor != cCustom )
			{
				class'HudBase'.default.CustomHUDColor = cCustom;
				class'HudBase'.static.StaticSaveConfig();
				return false;
			}
		}
		else if ( class'HudBase'.default.CustomHUDColor != Temp )
		{
			class'HudBase'.default.CustomHUDColor = Temp;
			class'HudBase'.static.StaticSaveConfig();
			return false;
		}
	}

	return false;
}

function InternalOnChange(GUIComponent Sender)
{
	Super.InternalOnChange(Sender);
	switch (Sender)
	{
	case ch_Visible:
		bVis = ch_Visible.IsChecked();
		break;

	case ch_Weapons:
		bWeapons = ch_Weapons.IsChecked();
		break;

	case ch_Personal:
		bPersonal = ch_Personal.IsChecked();
		break;

	case ch_Score:
		bScore = ch_Score.IsChecked();
		break;

	case ch_WeaponBar:
		bWeaponBar = ch_WeaponBar.IsChecked();
		break;

    case ch_DeathMsgs:
        bNoMsgs = ch_DeathMsgs.IsChecked();
        break;

	case ch_Portraits:
		bPortraits = ch_Portraits.IsChecked();
		break;

	case ch_VCPortraits:
		bVCPortraits = ch_VCPortraits.IsChecked();
		break;

	case ch_EnemyNames:
		bNames = ch_EnemyNames.IsChecked();
		break;

	case nu_MsgCount:
		iCount = nu_MsgCount.GetValue();
		break;

	case nu_MsgScale:
		iScale = nu_MsgScale.GetValue();
		break;

	case nu_MsgOffset:
		iOffset = nu_MsgOffset.GetValue();
		break;

	case sl_Scale:
		fScale = sl_Scale.GetValue();
		UpdatePreviewColor();
		break;

	case sl_Opacity:
		fOpacity = sl_Opacity.GetValue();
		UpdatePreviewColor();
		break;

	case ch_CustomColor:
		bCustomColor = ch_CustomColor.IsChecked();
		ChangeCustomStatus();
		UpdatePreviewColor();
		break;

	case sl_Red:
		cCustom.R = sl_Red.GetValue();
		UpdatePreviewColor();
		break;

	case sl_Blue:
		cCustom.B = sl_Blue.GetValue();
		UpdatePreviewColor();
		break;

	case sl_Green:
		cCustom.G = sl_Green.GetValue();
		UpdatePreviewColor();
		break;
	}
}

function ChangeCustomStatus()
{
	if (bCustomColor)
	{
		sl_Red.EnableMe();
		sl_Blue.EnableMe();
		sl_Green.EnableMe();

		cCustom.R = sl_Red.GetValue();
		cCustom.G = sl_Green.GetValue();
		cCustom.B = sl_Blue.GetValue();
	}
	else
	{
		sl_Red.DisableMe();
		sl_Blue.DisableMe();
		sl_Green.DisableMe();

		cCustom = GetDefaultColor();
	}
}

function UpdatePreviewColor()
{
	local float o, s;

	i_PreviewBG.ImageColor = cCustom;

	o = 255.0 * (fOpacity / 100.0);
	i_PreviewBG.ImageColor.A = o;
	i_Preview.ImageColor.A = o;

	s = fScale / 100.0;
	i_PreviewBG.WinWidth = DefaultBGPos.X2 * s;
	i_PreviewBG.WinHeight = DefaultBGPos.Y2 * s;

	i_Preview.WinWidth = DefaultHealthPos.X2 * s;
	i_Preview.WinHeight = DefaultHealthPos.Y2 * s;

	i_Scale.WinWidth =  i_PreviewBG.WinWidth;
	i_Scale.WinHeight = i_PreviewBG.WinHeight;

}

defaultproperties
{
 	Begin Object class=GUISectionBackground Name=GameBK
		WinWidth=0.448633
		WinHeight=0.901485
		WinLeft=0.031797
		WinTop=0.057604
        RenderWeight=0.001
		Caption="Options"
	End Object
	i_BG1=GameBK

	Begin Object class=GUISectionBackground Name=GameBK1
		WinWidth=0.448633
		WinHeight=0.901485
		WinLeft=0.517578
		WinTop=0.060208
        Caption="Visuals"
        RenderWeight=0.001
	End Object
	i_BG2=GameBK1

	Begin Object class=moCheckBox Name=GameHudVisible
		WinWidth=0.196875
		WinLeft=0.379297
		WinTop=0.043906
		Caption="Hide HUD"
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Hide the HUD while playing"
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		bAutoSizeCaption=True
		TabOrder=0
	End Object
	ch_Visible=GameHudVisible

	Begin Object class=moCheckBox Name=GameHudShowEnemyNames
		WinWidth=0.378125
		WinLeft=0.050000
		WinTop=0.848594
		Caption="Show Enemy Names"
		Hint="Display enemies' names above their heads"
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		bAutoSizeCaption=True
		INIOption="@Internal"
		TabOrder=1
	End Object
	ch_EnemyNames=GameHudShowEnemyNames

	Begin Object class=moCheckBox Name=GameHudShowWeaponBar
		WinWidth=0.378125
		WinLeft=0.050000
		WinTop=0.598593
		Caption="Weapon Bar"
		Hint="Select whether the weapons bar should appear on the HUD"
		CaptionWidth=0.9
		ComponentJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		bAutoSizeCaption=True
		TabOrder=2
	End Object
	ch_WeaponBar=GameHudShowWeaponBar

	Begin Object class=moCheckBox Name=GameHudShowWeaponInfo
		WinWidth=0.378125
		WinLeft=0.050000
		WinTop=0.181927
		Caption="Show Weapon Info"
		Hint="Show current weapon ammunition status."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		bAutoSizeCaption=True
		INIOption="@Internal"
		TabOrder=3
	End Object
	ch_Weapons=GameHudShowWeaponInfo

	Begin Object class=moCheckBox Name=GameHudShowPersonalInfo
		WinWidth=0.378125
		WinLeft=0.050000
		WinTop=0.317343
		Caption="Show Personal Info"
		Hint="Display health and armor on the HUD."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		bAutoSizeCaption=True
		INIOption="@Internal"
		TabOrder=4
	End Object
	ch_Personal=GameHudShowPersonalInfo

   	Begin Object class=moCheckBox Name=GameHudShowScore
		WinWidth=0.378125
		WinLeft=0.050000
		WinTop=0.452760
		Caption="Show Score"
		Hint="Check to show scores on the HUD"
		CaptionWidth=0.9
		bSquare=true
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		bAutoSizeCaption=True
		INIOption="@Internal"
		ComponentJustification=TXTA_Left
		TabOrder=5
	End Object
	ch_Score=GameHudShowScore

	Begin Object class=moCheckBox Name=GameHudShowPortraits
		WinWidth=0.378125
		WinLeft=0.050000
		WinTop=0.723594
		Caption="Show Portraits"
		Hint="Display player portraits when text messages are received"
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		bAutoSizeCaption=True
		TabOrder=6
	End Object
	ch_Portraits=GameHudShowPortraits

	Begin Object Class=moCheckBox name=GameHUDShowVCPortraits
		WinWidth=0.378125
		WinLeft=0.050000
		WinTop=0.723594
		Caption="Show VoIP Portraits"
		Hint="Display player portraits when voice chat messages are received"
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		bAutoSizeCaption=True
		TabOrder=7
	End Object
	ch_VCPortraits=GameHUDShowVCPortraits

    Begin Object class=moCheckBox Name=GameDeathMsgs
        WinWidth=0.403711
        WinLeft=0.047460
        WinTop=0.847553
        Caption="No Console Death Messages"
        Hint="Turn off reporting of death messages in console"
        CaptionWidth=0.9
        bSquare=True
        ComponentJustification=TXTA_Left
        OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
        INIOption="@Internal"
        INIDefault="False"
        bAutoSizeCaption=True
        TabOrder=8
    End Object
    ch_DeathMsgs=GameDeathMsgs

	Begin Object class=moNumericEdit Name=GameHudMessageCount
		WinWidth=0.381250
		WinLeft=0.550781
		WinTop=0.196875
		Caption="Max. Chat Count"
		CaptionWidth=0.7
		MinValue=0
		MaxValue=8
		ComponentJustification=TXTA_Left
		Hint="Number of lines of chat to display at once"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		bAutoSizeCaption=True
		TabOrder=9
	End Object
	nu_MsgCount=GameHudMessageCount

	Begin Object class=moNumericEdit Name=GameHudMessageScale
		WinWidth=0.381250
		WinLeft=0.550781
		WinTop=0.321874
		Caption="Chat Font Size"
		CaptionWidth=0.7
		MinValue=0
		MaxValue=8
		ComponentJustification=TXTA_Left
		Hint="Adjust the size of chat messages."
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		bAutoSizeCaption=True
		TabOrder=10
	End Object
	nu_MsgScale=GameHudMessageScale

	Begin Object class=moNumericEdit Name=GameHudMessageOffset
 		WinWidth=0.381250
		WinLeft=0.550781
		WinTop=0.436457
		Caption="Message Font Offset"
		CaptionWidth=0.7
		MinValue=0
		MaxValue=4
		ComponentJustification=TXTA_Left
		Hint="Adjust the size of game messages."
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		bAutoSizeCaption=True
		TabOrder=11
	End Object
	nu_MsgOffset=GameHudMessageOffset

	Begin Object class=moSlider Name=GameHudScale
		WinWidth=0.373749
		WinLeft=0.555313
		WinTop=0.309670
		MinValue=50
		MaxValue=100
		Caption="HUD Scaling"
		LabelColor=(R=255,G=255,B=255,A=255)
		LabelJustification=TXTA_Center
		ComponentJustification=TXTA_Left
		OnChange=InternalOnChange
		Hint="Adjust the size of the HUD"
		bAutoSizeCaption=True
		CaptionWidth=0.45
		ComponentWidth=-1
		TabOrder=12
	End Object
	sl_Scale=GameHudScale


	Begin Object class=moSlider Name=GameHudOpacity
		WinWidth=0.373749
		WinLeft=0.555313
		WinTop=0.361753
		MinValue=51
		MaxValue=100
		Caption="HUD Opacity"
		LabelColor=(R=255,G=255,B=255,A=255)
		LabelJustification=TXTA_Center
		ComponentJustification=TXTA_Left
		OnChange=InternalOnChange
		Hint="Adjust the transparency of the HUD"
		bAutoSizeCaption=True
		TabOrder=13
		CaptionWidth=0.45
		ComponentWidth=-1
	End Object
	sl_Opacity=GameHudOpacity

	Begin Object class=moCheckBox Name=CustomHUDColor
		WinWidth=0.373749
		WinLeft=0.555313
		WinTop=0.481406
		Caption="Custom HUD Color"
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Use configured HUD color instead of team colors"
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		bAutoSizeCaption=True
		TabOrder=14
	End Object
	ch_CustomColor=CustomHUDColor

	Begin Object class=moSlider Name=HudColorR
		WinWidth=0.272187
		WinLeft=0.610000
		WinTop=0.572917
		MinValue=0
		MaxValue=255
		Caption="Red:"
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		CaptionWidth=0.35
		ComponentWidth=-1
		LabelColor=(R=255,G=0,B=0,A=255)
		Hint="Adjust the amount of red in the HUD."
		OnChange=InternalOnChange
       	bIntSlider=true
		bAutoSizeCaption=True
       	TabOrder=15
	End Object
	sl_Red=HudColorR

	Begin Object class=moSlider Name=HudColorB
		WinWidth=0.272187
		WinLeft=0.610000
		WinTop=0.752500
		MinValue=0
		MaxValue=255
		Caption="Blue:"
		LabelColor=(R=0,G=0,B=255,A=255)
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		CaptionWidth=0.35
		ComponentWidth=-1
		OnChange=InternalOnChange
		Hint="Adjust the amount of blue in the HUD."
        bIntSlider=true
		bAutoSizeCaption=True
        TabOrder=16
	End Object
	sl_Blue=HudColorB

	Begin Object class=moSlider Name=HudColorG
		WinWidth=0.272187
		WinLeft=0.610000
		WinTop=0.660417
		MinValue=0
		MaxValue=255
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		CaptionWidth=0.35
		ComponentWidth=-1
		LabelColor=(R=0,G=255,B=0,A=255)
		Caption="Green:"
		Hint="Adjust the amount of green in the HUD."
		OnChange=InternalOnChange
        bIntSlider=true
 		bAutoSizeCaption=True
       TabOrder=17
	End Object
	sl_Green=HudColorG

	Begin Object Class=GUIComboBox Name=CustomHUDSelect
		Hint="To configure settings specific to a particular gametype, select the gametype from the list, then click the button to open the menu."
		WinWidth=0.227863
		WinHeight=0.030000
		WinLeft=0.553579
		WinTop=0.878722
		TabOrder=18
		bReadOnly=True
	End Object
	co_CustomHUD=CustomHUDSelect

	Begin Object Class=GUIButton Name=CustomHUDButton
		Caption="Configure"
		Hint="Opens the custom HUD configuration menu for the specified gametype."
		WinWidth=0.138577
		WinHeight=0.050000
		WinLeft=0.792737
		WinTop=0.869032
		TabOrder=19
		OnClick=InternalOnClick
	End Object
	b_CustomHUD=CustomHUDButton

	DefaultBGPos=(X1=0.749335,X2=0.163437,Y1=0.167488,Y2=0.121797)
	DefaultHealthPos=(X1=0.748164,X2=0.063241,Y1=0.169572,Y2=0.099531)

	Begin Object Class=GUIImage Name=PreviewBK
		Image=Material'2K4Menus.NewControls.NewTabBK'
		WinWidth=0.163437
		WinHeight=0.121797
		WinLeft=0.749335
		WinTop=0.211713
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=255)
		ImageAlign=IMGA_Center
		RenderWeight=1.001
	End Object
	i_Scale=PreviewBK;

	Begin Object Class=GUIImage Name=PreviewBackground
		Image=Material'HUDContent.Generic.HUD'
		WinWidth=0.163437
		WinHeight=0.121797
		WinLeft=0.749335
		WinTop=0.211713
		X1=0
		Y1=110
		X2=166
		Y2=163
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=255)
		ImageAlign=IMGA_Center
		RenderWeight=1.002
	End Object
	i_PreviewBG=PreviewBackground

	Begin Object Class=GUIImage Name=Preview
		Image=Material'HUDContent.Generic.HUD'
		WinWidth=0.063241
		WinHeight=0.099531
		WinLeft=0.749828
		WinTop=0.211559
		X1=74
		Y1=165
		X2=123
		Y2=216
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=255)
		RenderWeight=1.003
		ImageAlign=IMGA_Center
	End Object
	i_Preview=Preview

 	WinTop=0.15
	WinLeft=0
	WinWidth=1
	WinHeight=0.74
	bAcceptsInput=false

	PanelCaption="HUD"
}

//==============================================================================
//	Description
//
//	Created by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Tab_IForceSettings extends Settings_Tabs;

var automated GUISectionBackground i_BG1, i_BG2, i_BG3;
var automated GUILabel	l_IForce;
var automated moCheckbox ch_AutoSlope, ch_InvertMouse,
						ch_MouseSmoothing, ch_Joystick, ch_WeaponEffects,
						ch_PickupEffects, ch_DamageEffects, ch_GUIEffects,
						ch_MouseLag;
var automated moFloatEdit	fl_Sensitivity, fl_MenuSensitivity, fl_MouseAccel,
							fl_SmoothingStrength, fl_DodgeTime;

var automated GUIButton b_Controls, b_Speech;

var bool bAim, bSlope, bInvert, bSmoothing, bJoystick, bWFX, bPFX, bDFX, bGFX, bLag;
var float fSens, fMSens, fAccel, fSmoothing, fDodge;

var config string ControlBindMenu, SpeechBindMenu;

event InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);

    i_BG1.ManageComponent(ch_AutoSlope);
    i_BG1.ManageComponent(ch_InvertMouse);
    i_BG1.ManageComponent(ch_MouseSmoothing);
    i_BG1.ManageComponent(ch_MouseLag);
    i_BG1.ManageComponent(ch_Joystick);
    i_BG1.ManageComponent(b_Controls);
    i_BG1.ManageComponent(b_Speech);

    i_BG3.ManageComponent(fl_Sensitivity);
    i_BG3.ManageComponent(fl_MenuSensitivity);
    i_BG3.ManageComponent(fl_SmoothingStrength);
    i_BG3.ManageComponent(fl_MouseAccel);
    i_BG3.ManageComponent(fl_DodgeTime);

    // Disable force feedback options on non-win32 platforms...  --ryan.
    if ( (!PlatformIsWindows()) || (PlatformIs64Bit()) )
    {
        ch_WeaponEffects.DisableMe();
        ch_PickupEffects.DisableMe();
        ch_DamageEffects.DisableMe();
        ch_GUIEffects.DisableMe();
    }
}


function InternalOnLoadINI(GUIComponent Sender, string s)
{
	local PlayerController PC;

	PC = PlayerOwner();

	switch (Sender)
	{
	case ch_AutoSlope:
		bSlope = PC.bSnapToLevel;
		ch_AutoSlope.SetComponentValue(bSlope,true);
		break;

	case ch_InvertMouse:
		bInvert = class'PlayerInput'.default.bInvertMouse;
		ch_InvertMouse.SetComponentValue(bInvert,true);
		break;

	case ch_MouseSmoothing:
		bSmoothing = class'PlayerInput'.default.MouseSmoothingMode > 0;
		ch_MouseSmoothing.SetComponentValue(bSmoothing,true);
		break;

	case ch_Joystick:
		bJoystick = bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager UseJoystick"));
		ch_Joystick.SetComponentValue(bJoystick,true);
		break;

	case ch_WeaponEffects:
		bWFX = PC.bEnableWeaponForceFeedback;
		ch_WeaponEffects.SetComponentValue(bWFX,true);
		break;

	case ch_PickupEffects:
		bPFX = PC.bEnablePickupForceFeedback;
		ch_PickupEffects.SetComponentValue(bPFX,true);
		break;

	case ch_DamageEffects:
		bDFX = PC.bEnableDamageForceFeedback;
		ch_DamageEffects.SetComponentValue(bDFX,true);
		break;

	case ch_GUIEffects:
		bGFX = PC.bEnableGUIForceFeedback;
		ch_GUIEffects.SetComponentValue(bGFX,true);
		break;

	case ch_MouseLag:
		bLag = bool(PC.ConsoleCommand("get ini:Engine.Engine.RenderDevice ReduceMouseLag"));
		ch_MouseLag.Checked(bLag);
		break;

	case fl_Sensitivity:
		fSens = class'PlayerInput'.default.MouseSensitivity;
		fl_Sensitivity.SetComponentValue(fSens,true);
		break;

	case fl_MenuSensitivity:
		fMSens = Controller.MenuMouseSens;
		fl_MenuSensitivity.SetComponentValue(fMSens,true);
		break;

	case fl_MouseAccel:
		fAccel = class'PlayerInput'.Default.MouseAccelThreshold;
		fl_MouseAccel.SetComponentValue(fAccel,true);
		break;

	case fl_SmoothingStrength:
		fSmoothing = class'PlayerInput'.Default.MouseSmoothingStrength;
		fl_SmoothingStrength.SetComponentValue(fSmoothing,true);
		break;

	case fl_DodgeTime:
		fDodge = class'PlayerInput'.Default.DoubleClickTime;
		fl_DodgeTime.SetComponentValue(fDodge,true);
		break;

	default:
		log(Name@"Unknown component calling LoadINI:"$ GUIMenuOption(Sender).Caption);
		GUIMenuOption(Sender).SetComponentValue(s,true);
	}
}

function SaveSettings()
{
	local PlayerController PC;
	local bool bSave, bInputSave, bIForce;

	Super.SaveSettings();

	PC = PlayerOwner();

	if ( bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager UseJoystick")) != bJoystick )
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager UseJoystick" @ bJoystick);

	if ( bool(PC.ConsoleCommand("get ini:Engine.Engine.RenderDevice ReduceMouseLag")) != bLag )
		PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice ReduceMouseLag"@bLag);

	if ( PC.bSnapToLevel != bSlope )
	{
		PC.bSnapToLevel = bSlope;
		bSave = True;
	}

	if ( PC.bEnableWeaponForceFeedback != bWFX )
	{
		PC.bEnableWeaponForceFeedback = bWFX;
		bSave = True;
		bIForce = True;
	}

	if ( PC.bEnablePickupForceFeedback != bPFX )
	{
		PC.bEnablePickupForceFeedback = bPFX;
		bIForce = True;
		bSave = True;
	}

	if ( PC.bEnableDamageForceFeedback != bDFX )
	{
		PC.bEnableDamageForceFeedback = bDFX;
		bIForce = True;
		bSave = True;
	}

	if ( PC.bEnableGUIForceFeedback != bGFX )
	{
		PC.bEnableGUIForceFeedback = bGFX;
		bIForce = True;
		bSave = True;
	}

	if ( Controller.MenuMouseSens != FMax(0.0, fMSens) )
		Controller.SaveConfig();

	if ( class'PlayerInput'.default.DoubleClickTime != FMax(0.0, fDodge) )
	{
		class'PlayerInput'.default.DoubleClickTime = fDodge;
		bInputSave = True;
	}

	if ( class'PlayerInput'.default.MouseAccelThreshold != FMax(0.0, fAccel) )
	{
		PC.SetMouseAccel(fAccel);
		bInputSave = False;
	}

	if ( class'PlayerInput'.default.MouseSmoothingStrength != FMax(0.0, fSmoothing) )
	{
		PC.ConsoleCommand("SetSmoothingStrength"@fSmoothing);
		bInputSave = False;
	}

	if ( class'PlayerInput'.default.bInvertMouse != bInvert )
	{
		PC.InvertMouse( string(bInvert) );
		bInputSave = False;
	}

	if ( class'PlayerInput'.default.MouseSmoothingMode != byte(bSmoothing) )
	{
		PC.SetMouseSmoothing(int(bSmoothing));
		bInputSave = False;
	}

	if ( class'PlayerInput'.default.MouseSensitivity != FMax(0.0, fSens) )
	{
		PC.SetSensitivity(fSens);
		bInputSave = False;
	}

	if (bInputSave)
		class'PlayerInput'.static.StaticSaveConfig();

	if ( bIForce )
		PC.bForceFeedbackSupported = PC.ForceFeedbackSupported(bGFX || bWFX || bPFX || bDFX);

	if (bSave)
		PC.SaveConfig();
}

function ResetClicked()
{
	local int i;
	local string Str;
	local class					 	ViewportClass;
	local class<RenderDevice>		RenderClass;

	Super.ResetClicked();
	Str = PlayerOwner().ConsoleCommand("get ini:Engine.Engine.ViewportManager Class");
	Str = Mid(Str, InStr(Str, "'") + 1);
	Str = Left(Str, Len(Str) - 1);
	ViewportClass = class(DynamicLoadObject(Str, class'Class'));

	Str = PlayerOwner().ConsoleCommand("get ini:Engine.Engine.RenderDevice Class");
	Str = Mid(Str, InStr(Str, "'") + 1);
	Str = Left(Str, Len(Str) - 1);
	RenderClass = class<RenderDevice>(DynamicLoadObject(Str, class'Class'));

	ViewportClass.static.ResetConfig("UseJoystick");
	RenderClass.static.ResetConfig("ReduceMouseLag");
	Controller.static.ResetConfig("MenuMouseSens");
	class'PlayerController'.static.ResetConfig("bSnapToLevel");
	class'PlayerController'.static.ResetConfig("bEnableWeaponForceFeedback");
	class'PlayerController'.static.ResetConfig("bEnablePickupForceFeedback");
	class'PlayerController'.static.ResetConfig("bEnableDamageForceFeedback");
	class'PlayerController'.static.ResetConfig("bEnableGUIForceFeedback");

	class'PlayerInput'.static.ResetConfig("bInvertMouse");
	class'PlayerInput'.static.ResetConfig("MouseSmoothingMode");
	class'PlayerInput'.static.ResetConfig("MouseSensitivity");
	class'PlayerInput'.static.ResetConfig("MouseSmoothingStrength");
	class'PlayerInput'.static.ResetConfig("DoubleClickTime");
	class'PlayerInput'.static.ResetConfig("MouseAccelThreshold");

	for (i = 0; i < Components.Length; i++)
		Components[i].LoadINI();
}

function InternalOnChange(GUIComponent Sender)
{
	Super.InternalOnChange(Sender);
	switch (Sender)
	{
	case ch_AutoSlope:
		bSlope= ch_AutoSlope.IsChecked();
		break;

	case ch_InvertMouse:
		bInvert = ch_InvertMouse.IsChecked();
		break;

	case ch_MouseSmoothing:
		bSmoothing = ch_MouseSmoothing.IsChecked();
		break;

	case ch_Joystick:
		bJoystick = ch_Joystick.IsChecked();
		break;

	case ch_WeaponEffects:
		bWFX = ch_WeaponEffects.IsChecked();
		break;

	case ch_PickupEffects:
		bPFX = ch_PickupEffects.IsChecked();
		break;

	case ch_DamageEffects:
		bDFX = ch_DamageEffects.IsChecked();
		break;

	case ch_GUIEffects:
		bGFX = ch_GUIEffects.IsChecked();
		break;

	case ch_MouseLag:
		bLag = ch_MouseLag.IsChecked();
		break;

	case fl_Sensitivity:
		fSens = fl_Sensitivity.GetValue();
		break;

	case fl_MenuSensitivity:
		Controller.MenuMouseSens = fl_MenuSensitivity.GetValue();
		break;

	case fl_MouseAccel:
		fAccel = fl_MouseAccel.GetValue();
		break;

	case fl_SmoothingStrength:
		fSmoothing = fl_SmoothingStrength.GetValue();
		break;

	case fl_DodgeTime:
		fDodge = fl_DodgeTime.GetValue();
		break;
	}
}

function bool InternalOnClick(GUIComponent Sender)
{
	local GUITabControl C;
	local int i;

	if ( Sender == b_Controls )
	{
		Controller.OpenMenu(ControlBindMenu);
	}

	else if ( Sender == b_Speech )
	{
		// Hack - need to update the players character and voice options before opening the speechbind menu
		C = GUITabControl(MenuOwner);
		if ( C != None )
		{
			for ( i = 0; i < C.TabStack.Length; i++ )
			{
				if ( C.TabStack[i] != None && UT2K4Tab_PlayerSettings(C.TabStack[i].MyPanel) != None )
				{
					UT2K4Tab_PlayerSettings(C.TabStack[i].MyPanel).SaveSettings();
					break;
				}
			}
		}

		Controller.OpenMenu(SpeechBindMenu);
	}

	return true;
}

defaultproperties
{
	Begin Object class=GUISectionBackground Name=InputBK1
		WinWidth=0.381328
		WinHeight=0.655039
		WinLeft=0.021641
		WinTop=0.028176
		Caption="Options"
	End Object
	i_BG1=InputBK1

	Begin Object class=GUISectionBackground Name=InputBK2
		WinWidth=0.957500
		WinHeight=0.240977
		WinLeft=0.021641
		WinTop=0.730000
		Caption="TouchSense Force Feedback"
	End Object
	i_BG2=InputBK2

	Begin Object class=GUISectionBackground Name=InputBK3
		WinWidth=0.527812
		WinHeight=0.655039
		WinLeft=0.451289
		WinTop=0.028176
		Caption="Fine Tuning"
	End Object
	i_BG3=InputBK3


	Begin Object Class=GUIButton Name=ControlBindButton
	    Caption="Configure Controls"
	    SizingCaption="XXXXXXXXXX"
		WinWidth=0.153281
		WinHeight=0.043750
		WinLeft=0.130000
		WinTop=0.018333
	    TabOrder=0
	    OnClick=InternalOnClick
	    Hint="Configure controls and keybinds"
	End Object
	b_Controls=ControlBindButton

	Begin Object Class=GUIButton Name=SpeechBindButton
	    Caption="Speech Binder"
	    SizingCaption="XXXXXXXXXX"
		WinWidth=0.153281
		WinHeight=0.043750
		WinLeft=0.670000
		WinTop=0.018333
	    TabOrder=1
	    OnClick=InternalOnClick
	    Hint="Configure custom keybinds for in-game messages"
	End Object
	b_Speech=SpeechBindButton


	Begin Object class=moCheckBox NAme=InputAutoSlope
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.060937
		WinTop=0.105365
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Auto Slope"
		Hint="When enabled, your view will automatically pitch up/down when on a slope."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=2
	End Object
	ch_AutoSlope=InputAutoSlope

	Begin Object class=moCheckBox NAme=InputInvertMouse
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.060938
		WinTop=0.188698
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Invert Mouse"
		Hint="When enabled, the Y axis of your mouse will be inverted."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=3
	End Object
	ch_InvertMouse=InputInvertMouse

	Begin Object class=moCheckBox NAme=InputMouseSmoothing
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.060938
		WinTop=0.324167
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Mouse Smoothing"
		Hint="Enable this option to automatically smooth out movements in your mouse."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=4
	End Object
	ch_MouseSmoothing=InputMouseSmoothing

	Begin Object class=moCheckBox NAme=InputMouseLag
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.060938
		WinTop=0.405000
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Reduce Mouse Lag"
		Hint="Enable this option will reduce the amount of lag in your mouse."
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
		TabOrder=5
	End Object
	ch_MouseLag=InputMouseLag


	Begin Object class=moCheckBox NAme=InputUseJoystick
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.060938
		WinTop=0.582083
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Enable Joystick"
		Hint="Enable this option to enable joystick support."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=6
	End Object
	ch_Joystick=InputUseJoystick


	Begin Object class=moFloatEdit Name=InputMouseSensitivity
		WinWidth=0.421680
		WinHeight=0.045352
		WinLeft=0.502344
		WinTop=0.105365
		MinValue=1.0
		MaxValue=25.0
        Step=0.25
		Caption="Mouse Sensitivity (Game)"
		CaptionWidth=0.725
		ComponentJustification=TXTA_Left
		Hint="Adjust mouse sensitivity"
 		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
       bHeightFromComponent=false
       TabOrder=7
	End Object
	fl_Sensitivity=InputMouseSensitivity

	Begin Object class=moFloatEdit Name=InputMenuSensitivity
		WinWidth=0.421875
		WinLeft=0.502344
		WinTop=0.188698
		WinHeight=0.045352
		bHeightFromComponent=false
		MinValue=1.0
		MaxValue=6.0
        Step=0.25
		Caption="Mouse Sensitivity (Menus)"
		CaptionWidth=0.725
		ComponentJustification=TXTA_Left
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Adjust mouse speed within the menus"
		TabOrder=8
	End Object
	fl_MenuSensitivity=InputMenuSensitivity

	Begin Object class=moFloatEdit Name=InputMouseSmoothStr
		WinWidth=0.421875
		WinLeft=0.502344
		WinTop=0.324167
		WinHeight=0.045352
		bHeightFromComponent=false
		MinValue=0.0
		MaxValue=1.0
        Step=0.05
		Caption="Mouse Smoothing Strength"
		CaptionWidth=0.725
		ComponentJustification=TXTA_Left
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Adjust the amount of smoothing that is applied to mouse movements"
		TabOrder=9
	End Object
	fl_SmoothingStrength=InputMouseSmoothStr

	Begin Object class=moFloatEdit Name=InputMouseAccel
		WinWidth=0.421875
		WinLeft=0.502344
		WinTop=0.405000
		WinHeight=0.045352
		bHeightFromComponent=false
		MinValue=0.0
		MaxValue=100.0
        Step=5
		Caption="Mouse Accel. Threshold"
		CaptionWidth=0.725
		ComponentJustification=TXTA_Left
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Adjust to determine the amount of movement needed before acceleration is applied"
		TabOrder=10
	End Object
	fl_MouseAccel=InputMouseAccel

	Begin Object class=moFloatEdit Name=InputDodgeTime
		WinWidth=0.421875
		WinLeft=0.502344
		WinTop=0.582083
		WinHeight=0.045352
		bHeightFromComponent=false
		MinValue=0.0
		MaxValue=1.0
        Step=0.05
		Caption="Dodge Double-Click Time"
		CaptionWidth=0.725
		ComponentJustification=TXTA_Left
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Determines how fast you have to double click to dodge"
		TabOrder=11
	End Object
	fl_DodgeTime=InputDodgeTime

	Begin Object class=moCheckBox NAme=InputIFWeaponEffects
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.815333
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Weapon Effects"
		Hint="Turn this option On/Off to feel the weapons you fire."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=12
	End Object
	ch_WeaponEffects=InputIFWeaponEffects

	Begin Object class=moCheckBox NAme=InputIFPickupEffects
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.906333
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Pickup Effects"
		Hint="Turn this option On/Off to feel the items you pick up."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=13
	End Object
	ch_PickupEffects=InputIFPickupEffects

	Begin Object class=moCheckBox NAme=InputIFDamageEffects
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.563867
		WinTop=0.815333
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Damage Effects"
		Hint="Turn this option On/Off to feel the damage you take."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=14
	End Object
	ch_DamageEffects=InputIFDamageEffects

	Begin Object class=moCheckBox NAme=InputIFGUIEffects
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.563867
		WinTop=0.906333
		ComponentClassName="XInterface.GUICheckBoxButton"
		Caption="Vehicle Effects"
		Hint="Turn this option On/Off to feel the vehicle effects."
		CaptionWidth=0.9
		bSquare=true
		IniOption="@Internal"
		OnLoadIni=InternalOnLoadINI
		OnChange=InternalOnChange
		ComponentJustification=TXTA_Left
		TabOrder=15
	End Object
	ch_GUIEffects=InputIFGUIEffects

	WinTop=0.15
	WinLeft=0
	WinWidth=1
	WinHeight=0.74
	bAcceptsInput=false

	PanelCaption="Input"
    PropagateVisibility=false
    ControlBindMenu="GUI2K4.ControlBinder"
    SpeechBindMenu="GUI2K4.SpeechBinder"
}

//==============================================================================
//	Created on: 08/08/2003
//	Description
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4BotConfigPage extends LockedFloatingWindow;

var localized string NoInformation, NoPref, DefaultString;
var GUIImage BotPortrait;
var GUILabel BotName;

var int ConfigIndex;
var xUtil.PlayerRecord ThisBot;
var bool bIgnoreChange;

var automated GUIImage i_Portrait;
var automated moSlider sl_Agg, sl_Acc, sl_Com, sl_Str, sl_Tac, sl_Rea, sl_Jumpy;
var automated moComboBox co_Weapon, co_Voice, co_Orders;

var	automated GUISectionBackground sb_PicBK;

var class<CustomBotConfig> BotConfigClass;
var array<CacheManager.WeaponRecord> Records;
var localized string ResetString;
var localized string AttributesString;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;

	Super.InitComponent(MyController, MyOwner);

	sb_PicBK.ManageComponent(i_Portrait);

	sb_Main.SetPosition(0.350547,0.078391,0.565507,0.600586);

	sb_Main.Caption=AttributesString;

    sb_Main.ManageComponent(sl_Agg);
    sb_Main.ManageComponent(sl_Acc);
    sb_Main.ManageComponent(sl_Com);
    sb_Main.ManageComponent(sl_Str);
    sb_Main.ManageComponent(sl_Tac);
    sb_Main.ManageComponent(sl_Rea);
    sb_Main.ManageComponent(sl_Jumpy);
    sb_Main.ManageComponent(co_Weapon);
    sb_Main.ManageComponent(co_Voice);
    sb_Main.ManageComponent(co_Orders);


	class'CacheManager'.static.GetWeaponList( Records );
    co_Weapon.AddItem(NoPref);
    for (i=0;i<Records.Length;i++)
    	co_Weapon.AddItem(Records[i].FriendlyName,,Records[i].ClassName);

    for ( i = 0; i < ArrayCount(class'GameProfile'.default.PositionName); i++ )
    	co_Orders.AddItem(class'GameProfile'.default.PositionName[i]);

    co_Weapon.Onchange=ComboBoxChange;

	sl_Agg.MySlider.OnDrawCaption=AggDC;
	sl_Acc.MySlider.OnDrawCaption=AccDC;
	sl_Com.MySlider.OnDrawCaption=ComDC;
	sl_Str.MySlider.OnDrawCaption=StrDC;
	sl_Tac.MySlider.OnDrawCaption=TacDC;
	sl_Rea.MySlider.OnDrawCaption=ReaDC;
	sl_Jumpy.MySlider.OnDrawCaption=JumpyDC;

	b_OK.OnClick = OkClick;

	b_Cancel.Caption=ResetString;
	b_Cancel.OnClick=ResetClick;

}

function SetupBotInfo(Material Portrait, string DecoTextName, xUtil.PlayerRecord PRE)
{
	local int i;
	local array<string> VoicePackClasses;
	local class<xVoicePack> Pack;

	bIgnoreChange = true;

    ThisBot = PRE;
    PlayerOwner().GetAllInt("XGame.xVoicePack", VoicePackClasses);

    co_Voice.MyComboBox.List.Clear();
    co_Voice.AddItem(DefaultString);
    for ( i = 0; i < VoicePackClasses.Length; i++ )
    {
    	Pack = class<xVoicePack>(DynamicLoadObject( VoicePackClasses[i], class'Class'));
    	if ( Pack != None )
    	{
    		// Only show voices that correspond to this gender
    		if ( class'TeamVoicePack'.static.VoiceMatchesGender(Pack.default.VoiceGender, ThisBot.Sex) )
	    		co_Voice.AddItem( Pack.default.VoicePackName, Pack, VoicePackClasses[i] );
	    }
    }

	// Setup the Portrait from here
	i_Portrait.Image = PRE.Portrait;
	// Setup the decotext from here
	sb_PicBK.Caption = PRE.DefaultName;

    ConfigIndex = BotConfigClass.static.IndexFor(PRE.DefaultName);

    if (ConfigIndex>=0)
    {
    	sl_Agg.SetValue(BotConfigClass.default.ConfigArray[ConfigIndex].Aggressiveness);
    	sl_Acc.SetValue(BotConfigClass.default.ConfigArray[ConfigIndex].Accuracy);
    	sl_Com.SetValue(BotConfigClass.default.ConfigArray[ConfigIndex].CombatStyle);
    	sl_Str.SetValue(BotConfigClass.default.ConfigArray[ConfigIndex].StrafingAbility);
    	sl_Tac.SetValue(BotConfigClass.default.ConfigArray[ConfigIndex].Tactics);
        sl_Rea.SetValue(BotConfigClass.default.ConfigArray[ConfigIndex].ReactionTime);
        sl_Jumpy.SetValue(BotConfigClass.default.ConfigArray[ConfigIndex].Jumpiness);

        co_Weapon.Find(BotConfigClass.default.ConfigArray[ConfigIndex].FavoriteWeapon,,True);
        co_Voice.Find(BotConfigClass.default.ConfigArray[ConfigIndex].CustomVoice,,True);
        co_Orders.Find(class'GameProfile'.static.TextPositionDescription(BotConfigClass.default.ConfigArray[ConfigIndex].CustomOrders));

    }
    else
	{
    	sl_Agg.SetValue(float(PRE.Aggressiveness));
    	sl_Acc.SetValue(float(PRE.Accuracy));
    	sl_Com.SetValue(float(PRE.CombatStyle));
    	sl_Str.SetValue(float(PRE.StrafingAbility));
    	sl_Tac.SetValue(float(PRE.Tactics));
    	sl_Rea.SetValue(float(PRE.ReactionTime));
        sl_Jumpy.SetValue(float(PRE.Jumpiness));

        co_Weapon.Find(PRE.FavoriteWeapon,,True);
        if ( PRE.VoiceClassName != "" )
	        co_Voice.Find(PRE.VoiceClassName,,True);

	    co_Orders.SetIndex(0);
    }

    bIgnoreChange=false;

}

function bool OkClick(GUIComponent Sender)
{
	BotConfigClass.static.StaticSaveConfig();
	Controller.CloseMenu(false);
	return true;
}

function bool ResetClick(GUIComponent Sender)
{
	bIgnoreChange = true;

	if ( ConfigIndex >= 0 )
	{
		class'CustomBotConfig'.default.ConfigArray.Remove(ConfigIndex,1);
		class'CustomBotConfig'.static.StaticSaveConfig();
	}

	ConfigIndex = -1;
   	sl_Agg.SetValue(float(ThisBot.Aggressiveness));
   	sl_Acc.SetValue(float(ThisBot.Accuracy));
   	sl_Com.SetValue(float(ThisBot.CombatStyle));
   	sl_Str.SetValue(float(ThisBot.StrafingAbility));
   	sl_Tac.SetValue(float(ThisBot.Tactics));
   	sl_Rea.SetValue(float(ThisBot.ReactionTime));
   	sl_Jumpy.SetValue(float(ThisBot.Jumpiness));
	co_Weapon.Find(ThisBot.FavoriteWeapon,false,True);

	if ( ThisBot.VoiceClassName != "" )
		co_Voice.Find(ThisBot.VoiceClassName,false,True);

	else co_Voice.SetIndex(0);

	co_Orders.SetIndex(0);
    bIgnorechange = false;

	return true;
}


function string DoPerc(GUISlider Control)
{
	local float r,v,vmin;

    vmin = Control.MinValue;
    r = Control.MaxValue - vmin;
    v = Control.Value - vmin;

    return string(int(v/r*100));
}



function string AggDC()
{
	return DoPerc(sl_Agg.MySlider) $ "%";
}

function string AccDC()
{
	return sl_Acc.GetComponentValue();
}

function string ComDC()
{
	return DoPerc(sl_Com.MySlider) $"%";
}

function string StrDC()
{
	return sl_Str.GetComponentValue();
}

function string TacDC()
{
	return sl_Tac.GetComponentValue();
}

function string ReaDC()
{
	return sl_Rea.GetComponentValue();
}

function string JumpyDC()
{
	return DoPerc(sl_Jumpy.MySlider) $ "%";
}

function SetDefaults()
{
	BotConfigClass.default.ConfigArray[ConfigIndex].CharacterName = ThisBot.DefaultName;
	BotConfigClass.default.ConfigArray[ConfigIndex].PlayerName = ThisBot.DefaultName;
    BotConfigClass.default.ConfigArray[ConfigIndex].FavoriteWeapon = ThisBot.FavoriteWeapon;
    BotConfigClass.default.ConfigArray[ConfigIndex].Aggressiveness = float(ThisBot.Aggressiveness);
    BotConfigClass.default.ConfigArray[ConfigIndex].Accuracy = float(ThisBot.Accuracy);
    BotConfigClass.default.ConfigArray[ConfigIndex].CombatStyle = float(ThisBot.CombatStyle);
    BotConfigClass.default.ConfigArray[ConfigIndex].StrafingAbility = float(ThisBot.StrafingAbility);
    BotConfigClass.default.ConfigArray[ConfigIndex].Tactics = float(ThisBot.Tactics);
    BotConfigClass.default.ConfigArray[ConfigIndex].ReactionTime = float(ThisBot.ReactionTime);
    BotConfigClass.default.ConfigArray[ConfigIndex].Jumpiness = float(ThisBot.Jumpiness);
    BotConfigClass.default.ConfigArray[ConfigIndex].CustomVoice = ThisBot.VoiceClassName;
    BotConfigClass.default.ConfigArray[ConfigIndex].CustomOrders = POS_Auto;
}

function SliderChange(GUIComponent Sender)
{
	local moSlider S;

	if ( moSlider(Sender) != None )
		S = moSlider(Sender);

    if ( bIgnoreChange || S == None )
    	return;

	ValidateIndex();
	if (S == sl_Agg)
      BotConfigClass.default.ConfigArray[ConfigIndex].Aggressiveness = S.GetValue();

	else if (S == sl_Acc)
      BotConfigClass.default.ConfigArray[ConfigIndex].Accuracy = S.GetValue();

	else if (S == sl_Com)
      BotConfigClass.default.ConfigArray[ConfigIndex].CombatStyle = S.GetValue();

	else if (S == sl_Str)
      BotConfigClass.default.ConfigArray[ConfigIndex].StrafingAbility = S.GetValue();

	else if (S == sl_Tac)
      BotConfigClass.default.ConfigArray[ConfigIndex].Tactics = S.GetValue();

	else if (S == sl_Rea)
      BotConfigClass.default.ConfigArray[ConfigIndex].ReactionTime = S.GetValue();

    else if ( S == sl_Jumpy )
      BotConfigClass.default.ConfigArray[ConfigIndex].Jumpiness = S.GetValue();
}

function ComboBoxChange(GUIComponent Sender)
{
	if (bIgnorechange || moComboBox(Sender) == None)
    	return;

	ValidateIndex();
	if ( Sender == co_Weapon )
	    BotConfigClass.default.ConfigArray[ConfigIndex].FavoriteWeapon = co_Weapon.GetExtra();

	else if ( Sender == co_Voice )
		BotConfigClass.default.ConfigArray[ConfigIndex].CustomVoice = co_Voice.GetExtra();

	else if ( Sender == co_Orders )
		BotConfigClass.default.ConfigArray[ConfigIndex].CustomOrders = class'GameProfile'.static.EnumPositionDescription(co_Orders.GetText());
}

function ValidateIndex()
{
	// Look to see if this is a new entry
    if (ConfigIndex==-1)
    {
    	ConfigIndex = BotConfigClass.Default.ConfigArray.Length;
		BotConfigClass.Default.ConfigArray.Length = ConfigIndex+1;
        SetDefaults();
    }
}

defaultproperties
{
	Begin Object class=GUISectionBackground name=PicBK
		WinWidth=0.290820
		WinHeight=0.638294
		WinLeft=0.026150
		WinTop=0.078391
	End Object
	sb_PicBK=PicBK

	Begin Object class=GUIImage Name=imgBotPic
		WinWidth=0.246875
		WinHeight=0.822510
		WinLeft=0.079861
		WinTop=0.116031
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
		RenderWeight=0.11
		bScaleToParent=True
		bBoundToParent=True
	End Object
	i_Portrait=imgBotPic

	Begin Object class=moSlider Name=BotAggrSlider
		Caption="Aggressiveness"
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.107618
		MinValue=0
		MaxValue=1
		Hint="Configures the aggressiveness rating of this bot."
		LabelStyleName="TextLabel"
		SliderCaptionStyleName="TextLabel"
        OnChange=SliderChange
        TabOrder=0
        bHeightFromComponent=False
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moSlider Name=BotAccuracySlider
		Caption="Accuracy"
		SliderCaptionStyleName="TextLabel"
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.177603
		MinValue=-2
		MaxValue=2
		Hint="Configures the accuracy rating of this bot."
        OnChange=SliderChange
        bHeightFromComponent=False
		LabelStyleName="TextLabel"
		TabOrder=1
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moSlider Name=BotCStyleSlider
		Caption="Combat Style"
		SliderCaptionStyleName="TextLabel"
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.247588
		MinValue=0
		MaxValue=1
		Hint="Adjusts the combat style of this bot."
        OnChange=SliderChange
        bHeightFromComponent=False
		LabelStyleName="TextLabel"
		TabOrder=2
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moSlider Name=BotStrafeSlider
		Caption="Strafing Ability"
		SliderCaptionStyleName="TextLabel"
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.317573
		MinValue=-2
		MaxValue=2
		Hint="Adjusts the strafing ability of this bot."
        OnChange=SliderChange
        bHeightFromComponent=False
		LabelStyleName="TextLabel"
		TabOrder=3
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moSlider Name=BotTacticsSlider
		Caption="Tactics"
		SliderCaptionStyleName="TextLabel"
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.387558
		MinValue=-2
		MaxValue=2
		Hint="Adjusts the team-play awareness ability of this bot."
        OnChange=SliderChange
        bHeightFromComponent=False
		LabelStyleName="TextLabel"
		TabOrder=4
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moSlider Name=BotReactionSlider
		Caption="Reaction Time"
		SliderCaptionStyleName="TextLabel"
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.457542
		MinValue=-2
		MaxValue=2
		Hint="Adjusts the reaction speed of this bot."
        OnChange=SliderChange
        bHeightFromComponent=False
		LabelStyleName="TextLabel"
		TabOrder=5
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moSlider Name=BotJumpy
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.527528
		Caption="Jumpiness"
		Hint="Controls whether this bot jumps a lot during the game."
		MinValue=0
		MaxValue=1
        OnChange=SliderChange
		LabelStyleName="TextLabel"
		SliderCaptionStyleName="TextLabel"
		TabOrder=6
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moComboBox Name=BotWeapon
		WinWidth=0.598438
		WinHeight=0.055469
		WinLeft=0.345313
		WinTop=0.647967
		Caption="Preferred Weapon"
		Hint="Select which weapon this bot should prefer."
		CaptionWidth=0.5
        bReadOnly=true
		ComponentJustification=TXTA_Left
        bHeightFromComponent=False
		LabelStyleName="TextLabel"
		TabOrder=7
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moComboBox Name=BotVoice
		WinWidth=0.598438
		WinHeight=0.055469
		WinLeft=0.345313
		WinTop=0.718011
		Caption="Voice"
		Hint="Choose which voice this bot uses."
		CaptionWidth=0.5
		bReadOnly=True
		ComponentJustification=TXTA_Left
		LabelStyleName="TextLabel"
		OnChange=ComboBoxChange
		TabOrder=8
		bScaleToParent=True
		bBoundToParent=True
	End Object

	Begin Object class=moComboBox Name=BotOrders
		WinWidth=0.598438
		WinHeight=0.055469
		WinLeft=0.345313
		WinTop=0.791159
		Caption="Orders"
		Hint="Choose which role this bot will play in the game."
		CaptionWidth=0.5
		bReadOnly=True
		ComponentJustification=TXTA_Left
		LabelStyleName="TextLabel"
		OnChange=ComboBoxChange
		TabOrder=9
		bScaleToParent=True
		bBoundToParent=True
	End Object

    sl_Agg=BotAggrSlider
    sl_Acc=BotAccuracySlider
    sl_Com=BotCStyleSlider
    sl_Str=BotStrafeSlider
    sl_Tac=BotTacticsSlider
    sl_Rea=BotReactionSlider
    sl_Jumpy=BotJumpy
    co_Weapon=BotWeapon
    co_Voice=BotVoice
    co_Orders=BotOrders

	WinWidth=0.921875
	WinHeight=0.759115
	WinLeft=0.043945
	WinTop=0.123958

	DefaultString="Default"
	NoPref="No Preference"

	BotConfigClass=class'CustomBotConfig'
	NoInformation="No Information Available!"
	ResetString="Reset"
	AttributesString="Attributes"
}

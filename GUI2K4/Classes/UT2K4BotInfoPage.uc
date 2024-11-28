//==============================================================================
//	UT2K4 version of UT2BotInfoPage
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4BotInfoPage extends LockedFloatingWindow;
var localized string NoInformation, AggressionCaption, AccuracyCaption, AgilityCaption, TacticsCaption;

var automated GUIImage 			i_Portrait;
var automated GUIProgressBar 	pb_Accuracy, pb_Agility, pb_Tactics, pb_Aggression;
var automated GUIScrollTextBox	lb_Deco;

var	automated GUISectionBackground sb_PicBK;
var automated altSectionBackground sb_HistBK;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);

	sb_Main.SetPosition(0.363243,0.057558,0.539140,0.336132);

	sb_PicBK.ManageComponent(i_Portrait);

	sb_HistBK.Managecomponent(lb_Deco);

	sb_Main.Managecomponent(pb_Accuracy);
	sb_Main.Managecomponent(pb_Agility);
	sb_Main.Managecomponent(pb_Tactics);
	sb_Main.Managecomponent(pb_Aggression);

	pb_Accuracy.Caption = AccuracyCaption;
	pb_Agility.Caption = AgilityCaption;
	pb_Tactics.Caption = TacticsCaption;
	pb_Aggression.Caption = AggressionCaption;

	b_Cancel.SetVisibility(false);
}

function SetupBotInfo(Material Portrait, string DecoTextName, xUtil.PlayerRecord PRE)
{
	local DecoText BotDeco;
	local int i;
	local string FavWeap, Package, TextName;

	// Setup the Portrait from here
	i_Portrait.Image = PRE.Portrait;
	if (DecoTextName == "")
		DecoTextName = PRE.TextName;

	if (InStr(DecoTextName, ".") != -1)
		Divide(DecoTextName, ".", Package, TextName);
	else TextName = DecoTextName;

	if (DecoTextName != "")
		BotDeco = class'xUtil'.static.LoadDecoText(Package, TextName);

	sb_PicBK.Caption = PRE.DefaultName;

	i = class'CustomBotConfig'.static.IndexFor(PRE.DefaultName);
	if (i != -1)
	{
		FavWeap = class'CustomBotConfig'.static.GetFavoriteWeaponFor(class'CustomBotConfig'.default.ConfigArray[i]);
		pb_Aggression.Value=class'CustomBotConfig'.static.AggressivenessRating(class'CustomBotConfig'.default.ConfigArray[i]);
		pb_Agility.Value=class'CustomBotConfig'.static.AgilityRating(class'CustomBotConfig'.default.ConfigArray[i]);
		pb_Tactics.Value=class'CustomBotConfig'.static.TacticsRating(class'CustomBotConfig'.default.ConfigArray[i]);
		pb_Accuracy.Value=class'CustomBotConfig'.static.AccuracyRating(class'CustomBotConfig'.default.ConfigArray[i]);
	}
	else
	{
		FavWeap = class'xUtil'.static.GetFavoriteWeaponFor(PRE);
		pb_Aggression.Value=class'XUtil'.static.AggressivenessRating(PRE);
		pb_Agility.Value=class'XUtil'.static.AgilityRating(PRE);
		pb_Tactics.Value=class'XUtil'.static.TacticsRating(PRE);
		pb_Accuracy.Value=class'XUtil'.static.AccuracyRating(PRE);
	}

	sb_Main.Caption = FavWeap;
	if (BotDeco != None)
		lb_Deco.SetContent( JoinArray(BotDeco.Rows, "|"), "|" );

	sb_HistBK.Caption = PRE.Species.default.SpeciesName;
}



defaultproperties
{
	Begin Object class=GUISectionBackground name=PicBK
		WinWidth=0.290820
		WinHeight=0.661731
		WinLeft=0.026150
		WinTop=0.057558
	End Object
	sb_PicBK=PicBK

	Begin Object class=AltSectionBackground name=HistBk
		WinWidth=0.546522
		WinHeight=0.269553
		WinLeft=0.357891
		WinTop=0.515790
		LeftPadding=0.01
		RightPadding=0.01
	End Object
	sb_HistBK=HistBK

	Begin Object class=GUIImage Name=imgBotPic
		WinWidth=0.246875
		WinHeight=0.866809
		WinLeft=0.079861
		WinTop=0.097923
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
		RenderWeight=1.01
	End Object

	Begin Object class=GUIProgressBar Name=myPB
		BarColor=(R=255,G=155,B=255,A=255)
		Value=50.0
		WinHeight=0.04
		FontName="UT2SmallFont"
		RenderWeight=1.2
		StyleName="TextLabel"
		bShowLow=false
		bShowHigh=false
		bShowValue=false
		BarBack=Material'2K4Menus.Newcontrols.NewStatusBar'
		BarTop=Material'2K4Menus.Newcontrols.NewStatusFill'

	End Object

	Begin Object class=GUIScrollTextBox Name=DecoDescription
		WinWidth=0.570936
		WinHeight=0.269553
		WinLeft=0.353008
		WinTop=0.613447
		CharDelay=0.0025
		EOLDelay=0.5
		bNeverFocus=true
	End Object

    AggressionCaption="Aggressiveness"
    AgilityCaption="Agility"
    AccuracyCaption="Accuracy"
    TacticsCaption="Tactics"

	i_Portrait=imgBotPic
	lb_Deco = DecoDescription

	pb_Aggression=myPB
	pb_Agility=myPB
	pb_Tactics=myPB
	pb_Accuracy=myPB

	WinWidth=0.902344
	WinHeight=0.759115
	WinLeft=0.045898
	WinTop=0.100228

	NoInformation="No Information Available!"
}

// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class InvasionWaveConfig extends UT2K3GUIPage
	DependsOn(Invasion);

var bool bInitialized;
var moNumericEdit WaveNo;
var GUISlider MySkill;
var moNumericEdit MyTime;

var Invasion.WaveInfo Waves[16];	// Defaults


function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);

    WaveNo = moNumericEdit(Controls[4]);
    MySkill = GUISlider(Controls[6]);
    MyTime = moNumericEdit(Controls[7]);

    GUIEditBox(WaveNo.MyNumericEdit.Controls[0]).bReadOnly = true;

    bInitialized = true;
    WaveNo.SetValue(1);
}

function WaveChanged(GUIComponent Sender)
{
	local int i;
    local byte Wave;
    local int wMask;

	if (!bInitialized)
    	return;

/*
struct WaveInfo
{
	var int WaveMask;	// bit fields for which monsterclasses
	var int WaveMaxMonsters;
	var float WaveMonsterInterval;
	var float WaveDifficulty;
	var float WaveDuration;
	var class<Monster> WaveFallbackMonster;
};
*/

	bInitialized = false;

    Wave = WaveNo.GetValue()-1;

    MySkill.SetValue(class'Invasion'.default.Waves[Wave].WaveDifficulty);
    MyTime.SetValue(class'Invasion'.default.Waves[Wave].WaveDuration);

    wMask = class'Invasion'.default.Waves[Wave].WaveMask;
    for (i=8;i<24;i++)
    	moCheckBox(Controls[i]).Checked(bool(wMask & moCheckBox(Controls[i]).Tag));

    bInitialized = true;

}

function SkillChanged(GUIComponent Sender)
{
	if (!bInitialized)
    	return;

	class'Invasion'.default.Waves[WaveNo.GetValue()-1].WaveDifficulty = MySkill.Value;
    class'Invasion'.static.staticsaveconfig();

}

function DurationChanged(GUIComponent Sender)
{
	if (!bInitialized)
    	return;

	class'Invasion'.default.Waves[WaveNo.GetValue()-1].WaveDuration = MyTime.GetValue();
    class'Invasion'.static.staticsaveconfig();

}

function CheckChanged(GUIComponent Sender)
{
	local int mask,mod;

    if (!bInitialized || moCheckBox(Sender)==None)
    	return;

	Mask = 0;
	for (Mod=8;Mod<24;Mod++)
    	if (moCheckBox(Controls[Mod]).IsChecked())
        	Mask += Controls[Mod].Tag;

    class'Invasion'.default.Waves[WaveNo.GetValue()-1].WaveMask = Mask;
    class'Invasion'.static.staticsaveconfig();
}

function bool CloseClicked(GUIComponent Sender)
{
	Controller.CloseMenu();
    return true;
}

function bool ResetClicked(GUIComponent Sender)
{
	local int i;

    for (i=0;i<16;i++)
		class'Invasion'.default.Waves[i] = Waves[i];

    class'Invasion'.static.staticsaveconfig();
    WaveNo.SetValue(1);

    return true;
}


defaultproperties
{
	Begin Object Class=GUIImage name=WCFGBackground
		bAcceptsInput=false
		bNeverFocus=true
        Image=Material'InterfaceContent.Menu.BorderBoxD'
        ImageStyle=ISTY_Stretched
        WinWidth=1
        WinLeft=0
        WinHeight=1
        WinTop=0
        bBoundToParent=true
        bScaleToParent=true
	End Object
	Controls(0)=GUIImage'WCFGBackground'

	Begin Object Class=GUIImage name=WCFGBackground1
		bAcceptsInput=false
		bNeverFocus=true
        Image=Material'InterfaceContent.Menu.BorderBoxD'
        ImageStyle=ISTY_Stretched
        WinWidth=1
        WinLeft=0
        WinHeight=1
        WinTop=0
        bBoundToParent=true
        bScaleToParent=true
	End Object
	Controls(1)=GUIImage'WCFGBackground1'

	Begin Object Class=GUIImage name=WCFGBackground2
		bAcceptsInput=false
		bNeverFocus=true
        Image=Material'InterfaceContent.Menu.BorderBoxD'
        ImageStyle=ISTY_Stretched
        WinWidth=1
        WinLeft=0
        WinHeight=1
        WinTop=0
        bBoundToParent=true
        bScaleToParent=true
	End Object
	Controls(2)=GUIImage'WCFGBackground2'

	Begin Object class=GUILabel Name=WCFGLabel1
		Caption="Configure each wave..."
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.496094
		WinHeight=32.000000
		WinLeft=0.004883
		WinTop=0.028385
        bBoundToParent=true
	End Object
	Controls(3)=GUILabel'WCFGLabel1'

	Begin Object class=moNumericEdit Name=WCFGWaveNo
		WinWidth=0.333008
		WinHeight=0.060000
		WinLeft=0.332031
		WinTop=0.139323
		Caption="Wave No"
		CaptionWidth=0.6
		MinValue=1
		MaxValue=16
		ComponentJustification=TXTA_Left
        bBoundToParent=true
		Hint="Select which wave to adjust."
        bScaleToParent=true
        OnChange=WaveChanged
	End Object
	Controls(4)=moNumericEdit'WCFGWaveNo'

	Begin Object Class=GUIImage name=WCFGBackground3
		bAcceptsInput=false
		bNeverFocus=true
        Image=Material'InterfaceContent.Menu.BorderBoxD'
        ImageStyle=ISTY_Stretched
		WinWidth=0.944336
		WinHeight=0.637695
		WinLeft=0.030273
		WinTop=0.230468
        bBoundToParent=true
        bScaleToParent=true
	End Object
	Controls(5)=GUIImage'WCFGBackground3'

	Begin Object class=GUISlider Name=WCFGDifficulty
		WinWidth=0.363477
		WinLeft=0.100000
		WinTop=0.307812
		MinValue=0
		MaxValue=3
        bBoundToParent=true
        bScaleToParent=true
		Hint="How hard should the monsters on this wave be."
        OnChange=SkillChanged;
	End Object
	Controls(6)=GUISlider'WCFGDifficulty'

	Begin Object class=moNumericEdit Name=WCFGDuration
		WinWidth=0.363477
		WinHeight=0.069766
		WinLeft=0.563867
		WinTop=0.288281
		Caption="Duration"
		CaptionWidth=0.5
		MinValue=1
		MaxValue=255
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
		Hint="How long should each wave last."
        OnChange=DurationChanged;
	End Object
	Controls(7)=moNumericEdit'WCFGDuration'

// Creatures

	Begin Object class=moCheckBox Name=WCFGCreat01
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.408854
		Caption="Pupae"
		Hint="The Skaarj Pupae."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=1
        OnChange=CheckChanged
	End Object
	Controls(8)=moCheckBox'WCFGCreat01'

	Begin Object class=moCheckBox Name=WCFGCreat02
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.408854
		Caption="Razor Fly"
		Hint="The Razor Fly."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=2
        OnChange=CheckChanged
	End Object
	Controls(9)=moCheckBox'WCFGCreat02'

	Begin Object class=moCheckBox Name=WCFGCreat03
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.467917
		Caption="Manta"
		Hint="The Manta."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=4
        OnChange=CheckChanged
	End Object
	Controls(10)=moCheckBox'WCFGCreat03'

	Begin Object class=moCheckBox Name=WCFGCreat04
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.467917
		Caption="Krall"
		Hint="The Krall."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=8
        OnChange=CheckChanged
	End Object
	Controls(11)=moCheckBox'WCFGCreat04'

	Begin Object class=moCheckBox Name=WCFGCreat05
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.526042
		Caption="Elite Krall"
		Hint="The Elite Krall."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=16
        OnChange=CheckChanged
	End Object
	Controls(12)=moCheckBox'WCFGCreat05'

	Begin Object class=moCheckBox Name=WCFGCreat06
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.526042
		Caption="Gasbag"
		Hint="The Gasbag."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=32
        OnChange=CheckChanged
	End Object
	Controls(13)=moCheckBox'WCFGCreat06'

	Begin Object class=moCheckBox Name=WCFGCreat07
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.582552
		Caption="Brute"
		Hint="The Brute."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=64
        OnChange=CheckChanged
	End Object
	Controls(14)=moCheckBox'WCFGCreat07'

	Begin Object class=moCheckBox Name=WCFGCreat08
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.582552
		Caption="Skaarj"
		Hint="The Skaarj."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=128
        OnChange=CheckChanged
	End Object
	Controls(15)=moCheckBox'WCFGCreat08'

	Begin Object class=moCheckBox Name=WCFGCreat09
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.639063
		Caption="Behemoth"
		Hint="The Behemoth."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=256
        OnChange=CheckChanged
	End Object
	Controls(16)=moCheckBox'WCFGCreat09'

	Begin Object class=moCheckBox Name=WCFGCreat10
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.639063
		Caption="Ice Skaarj"
		Hint="Ice Skaarj."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=512
        OnChange=CheckChanged
	End Object
	Controls(17)=moCheckBox'WCFGCreat10'

	Begin Object class=moCheckBox Name=WCFGCreat11
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.695573
		Caption="Fire Skaarj"
		Hint="The Fire Skaarj."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=1024
        OnChange=CheckChanged
	End Object
	Controls(18)=moCheckBox'WCFGCreat11'

	Begin Object class=moCheckBox Name=WCFGCreat12
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.695573
		Caption="Warlord"
		Hint="The Warlord."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=2048
        OnChange=CheckChanged
	End Object
	Controls(19)=moCheckBox'WCFGCreat12'

	Begin Object class=moCheckBox Name=WCFGCreat13
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.752083
		Caption="Pupae"
		Hint="The Skaarj Pupae."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=4096
        OnChange=CheckChanged
	End Object
	Controls(20)=moCheckBox'WCFGCreat13'

	Begin Object class=moCheckBox Name=WCFGCreat14
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.752083
		Caption="Pupae"
		Hint="The Skaarj Pupae."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=8192
        OnChange=CheckChanged
	End Object
	Controls(21)=moCheckBox'WCFGCreat14'

	Begin Object class=moCheckBox Name=WCFGCreat15
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.100000
		WinTop=0.808594
		Caption="Razor Fly"
		Hint="The Razor Fly."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=16384
        OnChange=CheckChanged
	End Object
	Controls(22)=moCheckBox'WCFGCreat15'

	Begin Object class=moCheckBox Name=WCFGCreat16
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.539453
		WinTop=0.808594
		Caption="Razor Fly"
		Hint="The Razor Fly."
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        bBoundToParent=true
        bScaleToParent=true
        Tag=32768
        OnChange=CheckChanged
	End Object
	Controls(23)=moCheckBox'WCFGCreat16'

	Begin Object class=GUILabel Name=WCFGLabel2
		Caption="Difficulty"
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.152344
		WinHeight=32.000000
		WinLeft=0.126953
		WinTop=0.249739
        bBoundToParent=true
	End Object
	Controls(24)=GUILabel'WCFGLabel2'

	Begin Object Class=GUIButton Name=WCFGClose
		Caption="Close"
		StyleName="SquareMenuButton"
		WinWidth=0.152344
		WinHeight=32.000000
		WinLeft=0.805664
		WinTop=0.876042
		OnClick=CloseClicked
        bBoundToParent=true
        bScaleToParent=true
	End Object
	Controls(25)=GUIButton'WCFGClose'

	Begin Object Class=GUIButton Name=WCFGReset
		Caption="Reset"
		StyleName="SquareMenuButton"
		WinWidth=0.152344
		WinHeight=32.000000
		WinLeft=0.605664
		WinTop=0.876042
		OnClick=ResetClicked
        bBoundToParent=true
        bScaleToParent=true
	End Object
	Controls(26)=GUIButton'WCFGReset'

	WinTop=0.20
	WinHeight=0.65;
	WinWidth=0.5;
	WinLeft=0.25;

	Waves(0)=(WaveMask=20491,WaveMaxMonsters=16,WaveDuration=90,WaveDifficulty=0.0)
	Waves(1)=(WaveMask=60,WaveMaxMonsters=12,WaveDuration=90,WaveDifficulty=0.0)
	Waves(2)=(WaveMask=105,WaveMaxMonsters=12,WaveDuration=90,WaveDifficulty=0.5)
	Waves(3)=(WaveMask=186,WaveMaxMonsters=12,WaveDuration=90,WaveDifficulty=0.5)
	Waves(4)=(WaveMask=225,WaveMaxMonsters=12,WaveDuration=90,WaveDifficulty=0.5)
	Waves(5)=(WaveMask=966,WaveMaxMonsters=12,WaveDuration=90,WaveDifficulty=1.0)
	Waves(6)=(WaveMask=4771,WaveMaxMonsters=12,WaveDuration=120,WaveDifficulty=1.0)
	Waves(7)=(WaveMask=917,WaveMaxMonsters=12,WaveDuration=120,WaveDifficulty=1.0)
	Waves(8)=(WaveMask=1689,WaveMaxMonsters=12,WaveDuration=120,WaveDifficulty=1.5)
	Waves(9)=(WaveMask=18260,WaveMaxMonsters=12,WaveDuration=120,WaveDifficulty=1.5)
	Waves(10)=(WaveMask=14340,WaveMaxMonsters=12,WaveDuration=180,WaveDifficulty=1.5)
	Waves(11)=(WaveMask=4021,WaveMaxMonsters=12,WaveDuration=180,WaveDifficulty=2.0)
	Waves(12)=(WaveMask=3729,WaveMaxMonsters=12,WaveDuration=180,WaveDifficulty=2.0)
	Waves(13)=(WaveMask=3972,WaveMaxMonsters=12,WaveDuration=180,WaveDifficulty=2.0)
	Waves(14)=(WaveMask=3712,WaveMaxMonsters=12,WaveDuration=180,WaveDifficulty=2.0)
	Waves(15)=(WaveMask=2048,WaveMaxMonsters=8,WaveDuration=255,WaveDifficulty=2.0)


}

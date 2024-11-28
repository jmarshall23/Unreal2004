// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class MyTestPanelA extends TestPanelBase;

var Automated moCheckBox CheckTest;
var Automated moEditBox EditTest;
var Automated moFloatEdit FloatTest;
var Automated moNumericEdit NumEditTest;
var Automated GUILabel lbSliderTest;
var Automated GUISlider SliderTest;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController,MyOwner);
    SliderTest.SetFriendlyLabel(lbSliderTest);
}

defaultproperties
{

	Begin Object class=moCheckBox Name=cTwo
		WinWidth=0.5
		WinHeight=0.050000
		WinLeft=0.25
		WinTop=0.2
		Caption="moCheckBox Test"
		CaptionWidth=0.9
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=1
        Hint="This is a check Box"
	End Object

	Begin Object class=moEditBox Name=cThree
		WinWidth=0.5
		WinHeight=0.050000
		WinLeft=0.25
		WinTop=0.3
		Caption="moEditBox Test"
		CaptionWidth=0.4
        TabOrder=2
        Hint="This is an Edit Box"
	End Object

	Begin Object class=moNumericEdit Name=cFive
		WinWidth=0.5
		WinHeight=0.050000
		WinLeft=0.25
		WinTop=0.4
		Caption="moNumericEdit Test"
		CaptionWidth=0.6
		MinValue=1
		MaxValue=16
        TabOrder=4
        Hint="This is an INT numeric Edit box"
	End Object

	Begin Object class=moFloatEdit Name=cFour
		WinWidth=0.5
		WinHeight=0.050000
		WinLeft=0.25
		WinTop=0.5
		MinValue=0.0
		MaxValue=1.0
        Step=0.05
		Caption="moFloatEdit Test"
		CaptionWidth=0.725
		ComponentJustification=TXTA_Left
        TabOrder=3
        Hint="This is a FLOAT numeric Edit Box";
	End Object

    Begin Object class=GUILabel Name=laSix
		WinWidth=0.226563
		WinHeight=0.050000
		WinLeft=0.375000
		WinTop=0.654545
        TextAlign=TXTA_Center
		Caption="Slider Test"
    End Object

	Begin Object class=GUISlider Name=cSix
		WinWidth=0.250000
		WinLeft=0.367188
		WinTop=0.713997
		MinValue=0
		MaxValue=1
		Hint="This is a Slider Test."
        TabOrder=5
	End Object

	CheckTest=cTwo
	EditTest=cThree
	FloatTest=cFour
	NumEditTest=cFive
	lbSliderTest=laSix
	SliderTest=cSix

	Background=Material'InterfaceContent.Backgrounds.bg11'

	WinWidth=1.000000
	WinHeight=0.807813
	WinLeft=0.000000
	WinTop=55.980499
}

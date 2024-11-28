// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class Pro_VideoSettings extends GUITabPanel;

#exec OBJ LOAD FILE=InterfaceContent.utx

var		bool ShowSShot;

struct DisplayMode
{
	var int	Width,
			Height;
};

var DisplayMode DisplayModes[9];

var localized string	BitDepthText[2];

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
	Super.Initcomponent(MyController, MyOwner);

	for (i=0;i<Controls.Length;i++)
		Controls[i].OnChange=InternalOnChange;

	moComboBox(Controls[1]).AddItem(BitDepthText[0]);
	moComboBox(Controls[1]).AddItem(BitDepthText[1]);
	moComboBox(Controls[1]).ReadOnly(true);

	CheckSupportedResolutions();

	Controls[6].FriendlyLabel  = GUILabel(Controls[5]);
	Controls[8].FriendlyLabel  = GUILabel(Controls[7]);
	Controls[10].FriendlyLabel = GUILabel(Controls[9]);

}

function CheckSupportedResolutions()
{
	local int		Index;
	local int		BitDepth;
	local string	CurrentSelection;

	CurrentSelection = moComboBox(Controls[0]).MyComboBox.Edit.GetText();
	if(moComboBox(Controls[0]).ItemCount() > 0)
	moComboBox(Controls[0]).RemoveItem(0,moComboBox(Controls[0]).ItemCount());

	if(moComboBox(Controls[1]).GetText()==BitDepthText[0])
		BitDepth = 16;
	else
		BitDepth = 32;

	for(Index = 0;Index < ArrayCount(DisplayModes);Index++)
	{
		if(!moCheckBox(Controls[2]).IsChecked() ||
			PlayerOwner().ConsoleCommand(
				"SupportedResolution"$
				" WIDTH="$DisplayModes[Index].Width$
				" HEIGHT="$DisplayModes[Index].Height$
				" BITDEPTH="$BitDepth) == "1")
		{
			moComboBox(Controls[0]).AddItem(DisplayModes[Index].Width$"x"$DisplayModes[Index].Height);
		}
	}

	moComboBox(Controls[0]).SetText(CurrentSelection);
}

function Refresh()
{
	InternalOnLoadINI(Controls[0],"");
	InternalOnLoadINI(Controls[1],"");
	InternalOnLoadINI(Controls[2],"");
}

function InternalOnLoadINI(GUIComponent Sender, string s)
{
	local string temp;

	if (Sender==Controls[0])
	{
		// Resolution
		if(Controller.GameResolution != "")
			moComboBox(Controls[0]).SetText(Controller.GameResolution);
		else
			moComboBox(Controls[0]).SetText(Controller.GetCurrentRes());
	}
	if (Sender==Controls[1])
	{
		moComboBox(Sender).SetText(BitDepthText[1]);
		CheckSupportedResolutions();
	}
	else if (Sender==Controls[2])
	{
		Temp = Sender.PlayerOwner().ConsoleCommand("ISFULLSCREEN");
		moCheckBox(Sender).Checked(bool(Temp));
		CheckSupportedResolutions();
	}
}

function string InternalOnSaveINI(GUIComponent Sender); 		// Do the actual work here

function InternalOnChange(GUIComponent Sender)
{
	if (!Controller.bCurMenuInitialized)
		return;

	if (Sender==Controls[0] || Sender==Controls[1] || Sender==Controls[2] )
	{
		Controls[3].bVisible=true;

		if(Sender != Controls[0])
			CheckSupportedResolutions();
	}

	else if (Sender==Controls[6])
		PlayerOwner().ConsoleCommand("GAMMA"@GUISlider(Controls[6]).Value);

	else if (Sender==Controls[8])
		PlayerOwner().ConsoleCommand("BRIGHTNESS"@GUISlider(Controls[8]).Value);

	else if (Sender==Controls[10])
		PlayerOwner().ConsoleCommand("CONTRAST"@GUISlider(Controls[10]).Value);
}

function bool ApplyChanges(GUIComponent Sender)
{
	local string DesiredRes;

	DesiredRes = moComboBox(Controls[0]).MyComboBox.Edit.GetText();

	DesiredRes=DesiredRes$"x32";

	if (moCheckBox(Controls[2]).IsChecked())
		DesiredRes=DesiredRes$"f";
	else
		DesiredRes=DesiredRes$"w";

	Controls[3].bVisible=false;

	if ( Controller.OpenMenu("xinterface.UT2VideoChangeOK") )
		UT2VideoChangeOK(Controller.TopPage()).Execute(DesiredRes);

	return true;
}

function bool InternalOnClick(GUIComponent Sender)
{
	showsshot = !showsshot;

	Controls[12].bVisible = !ShowSShot;
	Controls[13].bVisible = !ShowSShot;

	if (showsshot)
		GUIImage(Controls[11]).Image = material'GammaSet1';
	else
		GUIImage(Controls[11]).Image = material'GammaSet0';

	return true;
}

defaultproperties
{
	Begin Object class=moComboBox Name=ProVideoResolution
		WinWidth=0.390000
		WinHeight=0.050000
		WinLeft=0.124258
		WinTop=0.047396
		Caption="Resolution"
		INIOption="@INTERNAL"
		INIDefault="640x480"
		OnLoadINI=InternalOnLoadINI
		OnSaveINI=InternalOnSaveINI
		Hint="Select the video resolution at which you wish to play."
		bReadOnly=true
		CaptionWidth=0.375
	End Object
	Controls(0)=moComboBox'ProVideoResolution'

	Begin Object class=moComboBox Name=ProVideoColorDepth
		WinWidth=0.390000
		WinHeight=0.050000
		WinLeft=0.121484
		WinTop=0.152345
		Caption="Color Depth"
		INIOption="@Internal"
		INIDefault="false"
		OnLoadINI=InternalOnLoadINI
		OnSaveINI=InternalOnSaveINI
		Hint="Select the maximum number of colors to display at one time."
		CaptionWidth=0.375
        bVisible=false
	End Object
	Controls(1)=moComboBox'ProVideoColorDepth'

	Begin Object class=moCheckBox Name=ProVideoFullScreen
		WinWidth=0.350000
		WinHeight=0.040000
		WinLeft=0.667226
		WinTop=0.047396
		Caption="Full Screen"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		OnSaveINI=InternalOnSaveINI
		Hint="Check this box to run the game full screen."
		bSquare=true
		ComponentJustification=TXTA_Left
		CaptionWidth=0.375
	End Object
	Controls(2)=moCheckBox'ProVideoFullScreen'

	Begin Object class=GUIButton Name=ProVideoApply
		WinWidth=0.25000
		WinHeight=0.050000
		WinLeft=0.667226
		WinTop=0.152345
		Caption="Apply Changes"
		Hint="Apply all changes to your video settings."
		bVisible=false;
		OnClick=ApplyChanges
	End Object
	Controls(3)=GUIButton'ProVideoApply'


	Begin Object class=GUIImage Name=ProGammaBK
		WinWidth=0.957500
		WinHeight=0.697273
		WinLeft=0.021641
		WinTop=0.280365
		Image=Material'InterfaceContent.Menu.BorderBoxD'
		ImageColor=(R=255,G=255,B=255,A=160);
		ImageRenderStyle=MSTY_Alpha
		ImageStyle=ISTY_Stretched
	End Object
	Controls(4)=GUIImage'ProGammaBK'


	Begin Object class=GUILabel Name=ProGammaLabel
		Caption="Gamma"
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.250000
		WinHeight=32.000000
		WinLeft=0.060547
		WinTop=0.341146
		StyleName="TextLabel"
	End Object
	Controls(5)=GUILabel'ProGammaLabel'

	Begin Object class=GUISlider Name=ProGammaSlider
		WinWidth=0.250000
		WinLeft=0.062500
		WinTop=0.402345
		MinValue=0.5
		MaxValue=1.0
		INIOption="ini:Engine.Engine.ViewportManager Gamma"
		INIDefault="0.8"
		Hint="Use the slider to adjust the Gamma to suit your monitor."
	End Object
	Controls(6)=GUISlider'ProGammaSlider'


	Begin Object class=GUILabel Name=ProBrightnessLabel
		Caption="Brightness"
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.250000
		WinHeight=32.000000
		WinLeft=0.061524
		WinTop=0.555990
		StyleName="TextLabel"
	End Object
	Controls(7)=GUILabel'ProBrightnessLabel'

	Begin Object class=GUISlider Name=ProBrightnessSlider
		WinWidth=0.250000
		WinLeft=0.062500
		WinTop=0.623699
		MinValue=0
		MaxValue=1
		INIOption="ini:Engine.Engine.ViewportManager Brightness"
		INIDefault="0.8"
		Hint="Use the slider to adjust the Brightness to suit your monitor."
	End Object
	Controls(8)=GUISlider'ProBrightnessSlider'

	Begin Object class=GUILabel Name=ProContrastLabel
		Caption="Contrast"
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.250000
		WinHeight=32.000000
		WinLeft=0.059570
		WinTop=0.790365
		StyleName="TextLabel"
	End Object
	Controls(9)=GUILabel'ProContrastLabel'

	Begin Object class=GUISlider Name=ProContrastSlider
		WinWidth=0.250000
		WinLeft=0.062500
		WinTop=0.851565
		MinValue=0
		MaxValue=1
		INIOption="ini:Engine.Engine.ViewportManager Contrast"
		INIDEfault="0.8"
		Hint="Use the slider to adjust the Contrast to suit your monitor."
	End Object
	Controls(10)=GUISlider'ProContrastSlider'

	Begin Object class=GUIImage Name=ProGammaBar
		WinWidth=0.400000
		WinHeight=0.500000
		WinLeft=0.454102
		WinTop=0.377604
		Image=material'GammaSet1'
		ImageColor=(R=255,G=255,B=255,A=255);
	End Object
	Controls(11)=GUIImage'ProGammaBar'

	Begin Object class=GUILabel Name=ProBrightDesc1
		Caption="Adjust the Gamma Setting so that the"
		TextALign=TXTA_Center
		TextColor=(R=230,G=200,B=0,A=255)
		TextFont="UT2MenuFont"
		WinWidth=1.000000
		WinHeight=32.000000
		WinLeft=0.151367
		WinTop=0.8
		bVisible=false
	End Object
	Controls(12)=GUILabel'ProBrightDesc1'

	Begin Object class=GUILabel Name=ProBrightDesc2
		Caption="Square is completely black."
		TextALign=TXTA_Center
		TextFont="UT2MenuFont"
		TextColor=(R=230,G=200,B=0,A=255)
		WinWidth=1.000000
		WinHeight=32.000000
		WinLeft=0.136719
		WinTop=0.870000
		bVisible=false
	End Object
	Controls(13)=GUILabel'ProBrightDesc2'

	Begin Object class=GUIButton Name=ProVideoLeft
		WinWidth=0.043555
		WinHeight=0.084414
		WinLeft=0.397656
		WinTop=0.573959
		bNeverFocus=true
		bRepeatClick=true
		OnClick=InternalOnClick
		bVisible=false
		StyleName="ArrowLeft"
	End Object
	Controls(14)=GUIButton'ProVideoLeft'

	Begin Object class=GUIButton Name=ProVideoRight
		WinWidth=0.043555
		WinHeight=0.084414
		WinLeft=0.864063
		WinTop=0.573959
		StyleName="ArrowRight"
		bNeverFocus=true
		bRepeatClick=true
		OnClick=InternalOnClick
		bVisible=false
	End Object
	Controls(15)=GUIButton'ProVideoRight'


	WinTop=0.15
	WinLeft=0
	WinWidth=1
	WinHeight=0.74
	bAcceptsInput=false

	DisplayModes(0)=(Width=320,Height=240)
	DisplayModes(1)=(Width=512,Height=384)
	DisplayModes(2)=(Width=640,Height=480)
	DisplayModes(3)=(Width=800,Height=600)
	DisplayModes(4)=(Width=1024,Height=768)
	DisplayModes(5)=(Width=1152,Height=864)
	DisplayModes(6)=(Width=1280,Height=960)
	DisplayModes(7)=(Width=1280,Height=1024)
	DisplayModes(8)=(Width=1600,Height=1200)

	BitDepthText(0)="16-bit Color"
	BitDepthText(1)="32-bit Color"
}
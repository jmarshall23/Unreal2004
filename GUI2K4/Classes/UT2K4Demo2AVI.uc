//-----------------------------------------------------------
//
//-----------------------------------------------------------
class UT2K4Demo2AVI extends LockedFloatingWindow;

var automated GUILabel lb_SavePos;
var automated moEditBox  eb_Filename;
var automated moComboBox co_Resolution;
var automated moSlider   so_Quality;

var string demoname;

function InitComponent(GUIController Controller, GUIComponent Owner)
{
	super.InitComponent(Controller, Owner);
	sb_Main.bFillClient=true;
	sb_Main.TopPadding=0.05;
	sb_Main.SetPosition(0.033750,0.100000,0.650859,0.344726);
	sb_Main.ManageComponent(eb_Filename);
	sb_Main.Managecomponent(co_Resolution);
	sb_Main.ManageComponent(so_Quality);

	co_Resolution.AddItem("160x120");
	co_Resolution.AddItem("320x240");
	co_Resolution.AddItem("640x480");
	co_Resolution.AddItem("800x600");
	co_Resolution.AddItem("1280x720");
	co_Resolution.SetIndex(1);
	so_Quality.SetValue(1);

	b_Ok.OnCLick=okClick;

}

event HandleParameters(string Param1, string Param2)
{
	local string s;
	local int p;

	DemoName=Param1;

	p = instr(Caps(Param1),".DEMO4");
	if (p>=0)
		s = left(Param1,p);

	s = s$".AVI";
	eb_Filename.SetText(s);
}

function bool OkClick(GUIComponent Sender)
{
	local string s;
 	local int p;
 	local int x,y;

 	s = Caps(co_Resolution.GetText());
 	p = instr(s,"X");
 	x=320;
 	y=240;
 	if (p>=0)
 	{
	 	x = int(Left(s,p));
	 	y = int(right(s,len(s)-p-1));
	}

	PlayerOwner().ConsoleCommand("demodump DEMO="$DemoName@"FILENAME="$eb_Filename.GetText()@"QUALITY="$so_Quality.GetValue()@"FPS=30");
	return true;
}

defaultproperties
{

	Begin Object class=GUILabel name=lbSavePos
		WinWidth=0.764286
		WinHeight=0.061864
		WinLeft=0.117857
		WinTop=0.715625
		Caption="AVI's saved to ..\\UserMovies"
		bBoundToParent=true
		bScaleToParent=true
		StyleName="ServerBrowserGrid"
		TextAlign=TXTA_Center
	End Object
	lb_SavePos=lbSavePos

	Begin Object Class=moEditBox Name=ebFilename
		WinWidth=0.895312
		WinHeight=0.098438
		WinLeft=0.089063
		WinTop=0.091667
		bScaleToParent=True
		bBoundToParent=True
		Caption="Filename: "
		Hint="The name of the AVI file to create"
		ComponentWidth=-1
		CaptionWidth=0.5
		bAutoSizeCaption=True
		bStandardized=true
	End Object
	eb_Filename=ebFilename

	Begin Object class=moComboBox Name=coResolution
		WinWidth=0.500000
		WinHeight=0.060000
		WinLeft=0.031250
		WinTop=0.079339
		Caption="Resolution"
		CaptionWidth=0.5
		bStandardized=true
		ComponentJustification=TXTA_Left
        TabOrder=0
        Hint="The resolution of the final movie."
	End Object
	co_Resolution=coResolution

	Begin Object class=moSlider Name=soQuality
		Caption="Quality"
		WinWidth=0.598438
		WinHeight=0.037500
		WinLeft=0.345313
		WinTop=0.107618
		MinValue=0
		bStandardized=true
		MaxValue=1
		Hint="The quality level of the compression used"
		LabelStyleName="TextLabel"
		SliderCaptionStyleName="TextLabel"
        TabOrder=0
        bHeightFromComponent=False
		bScaleToParent=True
		bBoundToParent=True
	End Object
	so_Quality=soQuality

	WinWidth=0.7;
	WinHeight=0.5;
	WinLeft=0.15;
	WinTop=0.25;
	DefaultWidth=0.7;
	DefaultHeight=0.5;
	DefaultLeft=0.15;
	DefaultTop=0.25;
	Windowname="Output to AVI..."
}

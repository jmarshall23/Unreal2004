// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class MyTestPanelC extends TestPanelBase;

var Automated GUISplitter MainSplitter;


defaultproperties
{
	Begin Object class=GUISplitter Name=cOne
		WinWidth=1
		WinHeight=1
		WinLeft=0
		WinTop=0
        DefaultPanels(0)="GUI2K4.MySubTestPanelA"
        DefaultPanels(1)="GUI2K4.MySubTestPanelB"
        SplitOrientation=SPLIT_Vertical
        MaxPercentage=0.8
	End Object

    MainSplitter=cOne

	Background=Material'InterfaceContent.Backgrounds.bg11'

	WinWidth=1.000000
	WinHeight=0.807813
	WinLeft=0.000000
	WinTop=55.980499
}

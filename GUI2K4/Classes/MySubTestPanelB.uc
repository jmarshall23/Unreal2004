// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class MySubTestPanelB extends TestPanelBase;

var Automated GUIListBox ListBoxTest;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i,c;

	Super.Initcomponent(MyController, MyOwner);

    c = rand(75)+25;
    for (i=0;i<c;i++)
    	ListBoxTest.List.Add("All Work & No Play Makes Me Sad");

}
defaultproperties
{
	Begin Object class=GUIListBox Name=cOne
		WinWidth=1
		WinHeight=1
		WinLeft=0
		WinTop=0
        bVisibleWhenEmpty=true
	End Object

	ListBoxTest=cOne

	Background=Material'InterfaceContent.Backgrounds.bg11'

	WinWidth=1.000000
	WinHeight=0.807813
	WinLeft=0.000000
	WinTop=55.980499
}

// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class MyTestPanelB extends TestPanelBase;

var Automated moComboBox ComboTest;
var Automated GUILabel lbListBoxTest;
var Automated GUIListBox ListBoxTest;
var Automated GUILabel lbScrollTextBox;
var Automated GUIScrollTextBox ScrollTextBoxTest;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i,c;
    local string t;

	Super.Initcomponent(MyController, MyOwner);

    c = rand(30)+5;
    for (i=0;i<c;i++)
    	ComboTest.AddItem("Test "$Rand(100));


    c = rand(75)+25;
    for (i=0;i<c;i++)
    	ListBoxTest.List.Add("Testing "$Rand(100));

	ListBoxTest.SetFriendlyLabel(lbListBoxTest);


    c = rand(75)+25;
    for (i=0;i<c;i++)
    {
    	if (t!="")
        	t = T $"|";

    	t = t$"All Work & No Play Makes Me Sad";
    }

    ScrollTextBoxTest.SetContent(t);
	ScrollTextBoxTest.SetFriendlyLabel(lbScrollTextBox);

}
defaultproperties
{
	Begin Object class=moComboBox Name=caOne
		WinWidth=0.500000
		WinHeight=0.060000
		WinLeft=0.031250
		WinTop=0.079339
		Caption="moComboBox Test"
		CaptionWidth=0.5
		ComponentJustification=TXTA_Left
        TabOrder=0
        Hint="This is a combo box"
	End Object

    Begin Object class=GUILabel Name=laTwo
		WinWidth=0.156250
		WinHeight=0.050000
		WinLeft=0.031250
		WinTop=0.200000
		Caption="ListBox Test"
    End Object

	Begin Object class=GUIListBox Name=caTwo
		WinWidth=0.445313
		WinHeight=0.706250
		WinLeft=0.031250
		WinTop=0.251653
        TabOrder=1
        bVisibleWhenEmpty=true
	End Object

    Begin Object class=GUILabel Name=laThree
		WinWidth=0.257813
		WinHeight=0.050000
		WinLeft=0.515625
		WinTop=0.200000
		Caption="Scrolling Text Test"
    End Object

	Begin Object class=GUIScrollTextBox Name=caThree
		WinWidth=0.445313
		WinHeight=0.706250
		WinLeft=0.515625
		WinTop=0.251653
        TabOrder=2
        bVisibleWhenEmpty=true
       	CharDelay=0.05
	End Object


	ComboTest=caOne
    lbListBoxTest=laTwo
    ListBoxTest=caTwo
	lbScrollTextBox=laThree
	ScrollTextBoxTest=caThree

	Background=Material'InterfaceContent.Backgrounds.bg11'

	WinWidth=1.000000
	WinHeight=0.807813
	WinLeft=0.000000
	WinTop=55.980499
}

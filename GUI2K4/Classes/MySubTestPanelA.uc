// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class MySubTestPanelA extends TestPanelBase;

var Automated GUIMultiColumnListBox MultiColumnListBoxTest;

defaultproperties
{
	Begin Object class=GUIContextMenu Name=cTestMenu
		ContextItems(0)="Test 0"
        ContextItems(1)="Test 1"
        ContextItems(2)="Fuck YOU"
        ContextItems(3)="ABCDEFGHIJKLM"
        ContextItems(4)="NOPQR"
    End Object

	Begin Object class=GUIMultiColumnListBox Name=cOne
		WinWidth=1
		WinHeight=1
		WinLeft=0
		WinTop=0
        bVisibleWhenEmpty=true
        DefaultListClass="GUI2K4.MyTestMultiColumnList"
        ContextMenu=cTestMenu
	End Object

    MultiColumnListBoxTest=cOne

	Background=Material'InterfaceContent.Backgrounds.bg11'

	WinWidth=1.000000
	WinHeight=0.807813
	WinLeft=0.000000
	WinTop=55.980499
}

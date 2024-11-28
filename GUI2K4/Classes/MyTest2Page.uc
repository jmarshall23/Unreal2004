// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class MyTest2Page extends TestPageBase;

var automated GUIImage i_Background;
var automated GUIEditbox ed_Test;

defaultproperties
{
	WinTop=0.0
	WinLeft=0.0
	WinHeight=1.0
	WinWidth=1.0


	Begin Object Class=GUIEditbox Name=TestEdit
		WinWidth=0.2
		WinHeight=0.2
		WinTop=0.2
		WinLeft=0.2
	End Object
	ed_Test=TestEdit

	Begin Object Class=GUIImage Name=PageBackground
		Image=Material'2K4Menus.Controls.popupBorder_b'
		WinTop=0.2
		WinLeft=0.2
		WinHeight=0.6
		WinWidth=0.6
	End Object
	i_Background=PageBackground

	Begin Object Class=GUIImage Name=PageFill
	End Object

	Background=Material'2K4Menus.Controls.menuBackground2'
}

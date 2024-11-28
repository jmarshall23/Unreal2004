// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4PerformWarn extends UT2K4GenericMessageBox;

var automated moCheckbox ch_NeverShowAgain;

function HandleParameters( string Param1, string Param2 )
{
	local float f;

	f = float(Param1);
	if ( f != 0.0 )
		SetTimer(f);
}

function Timer()
{
	Controller.CloseMenu(false);
}

function bool InternalOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(false);
	return true;
}

function CheckBoxClick(GUIComponent Sender)
{
	class'Settings_Tabs'.default.bExpert = ch_NeverShowAgain.IsChecked();
	class'Settings_Tabs'.static.StaticSaveConfig();
}

function InternalOnLoadIni( GUIComponent Sender, string Value )
{
	ch_NeverShowAgain.Checked(class'Settings_Tabs'.default.bExpert);
}

defaultproperties
{
	Begin Object Class=GUIButton Name=OkButton
		Caption="OK"
		WinWidth=0.121875
		WinHeight=0.040000
		WinLeft=0.439063
		WinTop=0.550000
		OnClick=InternalOnClick
		TabOrder=0
	End Object
    b_Ok=OKButton

	Begin Object class=GUILabel Name=DialogText
		Caption="WARNING"
		TextALign=TXTA_Center
		StyleName="TextLabel"
        FontScale=FNS_Large
		WinWidth=1
		WinLeft=0
		WinTop=0.4
		WinHeight=0.04
	End Object
    l_Text=DialogText

	Begin Object class=GUILabel Name=DialogText2
		Caption="The change you are making may adversely affect your performance."
		TextAlign=TXTA_Center
		StyleName="TextLabel"
		WinWidth=1
		WinLeft=0
		WinTop=0.45
		WinHeight=0.04
	End Object
    l_Text2=DialogText2

    Begin Object Class=moCheckBox Name=HideCheckbox
		WinWidth=0.370000
		WinHeight=0.030000
		WinLeft=0.312500
		WinTop=0.499479
    	Caption="  do not display this warning again"
    	Hint="Check this to disable showing warning messages when adjusting properties in the Settings menu"
    	IniOption="@Internal"
    	OnLoadIni=InternalOnLoadIni
    	OnChange=CheckBoxClick
    	TabOrder=1
    	CaptionWidth=0.93
    	ComponentWidth=0.07
    	bFlipped=True
    	ComponentJustification=TXTA_Left
    	LabelJustification=TXTA_Left
    	FontScale=FNS_Small
    End Object
    ch_NeverShowAgain=HideCheckbox

	OpenSound=sound'MenuSounds.SelectDshort'
}

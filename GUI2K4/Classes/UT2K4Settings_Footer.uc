// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4Settings_Footer extends ButtonFooter;

var automated GUIButton b_Back, b_Defaults;
var UT2K4SettingsPage SettingsPage;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController,MyOwner);
    SettingsPage = UT2K4SettingsPage(MyOwner);
}

function bool InternalOnClick(GUIComponent Sender)
{
	if (Sender==b_Back)
    	SettingsPage.BackButtonClicked();

    else if (Sender==b_Defaults)
    	SettingsPage.DefaultsButtonClicked();

	return true;
}

defaultproperties
{
    Begin Object Class=GUIButton Name=DefaultB
        Caption="DEFAULTS"
        Hint="Reset all settings on this page to their default values"
        WinWidth=0.114648
        WinLeft=0.885352
		WinHeight=0.036482
		WinTop=0.085678
        RenderWeight=2
        TabOrder=0
        bBoundToParent=True
        StyleName="FooterButton"
		OnClick=InternalOnClick
    End Object
    b_Defaults=DefaultB

    Begin Object Class=GUIButton Name=BackB
        Caption="BACK"
        Hint="Return to the previous menu"
		WinHeight=0.036482
		WinTop=0.085678
        RenderWeight=2
        TabOrder=1
        bBoundToParent=True
        StyleName="FooterButton"
		OnClick=InternalOnClick
    End Object
	b_Back=BackB
}

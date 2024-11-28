//====================================================================
//  UT2K4 footer panel
//
//  Written by Ron Prestenback
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class UT2K4Browser_Footer extends ButtonFooter;

var automated GUIImage          i_Status;
var automated moCheckBox        ch_Standard;
var automated GUITitleBar       t_StatusBar;
var automated GUIButton b_Join, b_Spectate, b_Back, b_Refresh, b_Filter;

var UT2K4ServerBrowser p_Anchor;

function bool InternalOnClick(GUIComponent Sender)
{
    if (GUIButton(Sender) == None)
        return false;

    if (Sender == b_Back)
    {
        Controller.CloseMenu(False);
        return true;
    }

    if ( Sender == b_Join )
    {
    	p_Anchor.JoinClicked();
    	return true;
    }

    if ( Sender == b_Spectate )
    {
    	p_Anchor.SpectateClicked();
    	return true;
    }

    if ( Sender == b_Refresh )
    {
    	p_Anchor.RefreshClicked();
    	return true;
    }

    if ( Sender == b_Filter )
    {
    	p_Anchor.FilterClicked();
    	return true;
    }


    return false;
}

function UpdateActiveButtons(UT2K4Browser_Page CurrentPanel)
{
    if (CurrentPanel == None)
        return;

	UpdateButtonState( b_Join,     CurrentPanel.IsJoinAvailable( b_Join.Caption ) );
	UpdateButtonState( b_Refresh,  CurrentPanel.IsRefreshAvailable( b_Refresh.Caption ) );
	UpdateButtonState( b_Spectate, CurrentPanel.IsSpectateAvailable( b_Spectate.Caption ) );
	UpdateButtonState( b_Filter,   CurrentPanel.IsFilterAvailable( b_Filter.Caption ) );

	if ( b_Filter.MenuState == MSAT_Disabled )
		ch_Standard.Hide();
	else ch_Standard.Show();
}


function UpdateButtonState( GUIButton But, bool Active )
{
	if ( Active )
		EnableComponent(But);
	else DisableComponent(But);
}

function PositionButtons( Canvas C )
{
	local bool b;

	b                 = b_Filter.bVisible;
	b_Filter.bVisible = false;

	super.PositionButtons(C);

	b_Filter.bVisible = b;
	b_Filter.WinLeft  = GetMargin();
}

function float GetButtonLeft()
{
	local bool bWasVisible;
	local float Result;

	bWasVisible = b_Filter.bVisible;
	b_Filter.bVisible = False;

	Result = Super.GetButtonLeft();
	b_Filter.bVisible = bWasVisible;

	return Result;
}

defaultproperties
{
    bFullHeight=False
    ButtonHeight=0.04
    Justification=TXTA_Right
	WinWidth=1.000000
	WinHeight=0.062422
	WinLeft=0.000000
	WinTop=0.936927

    Begin Object class=GUITitleBar name=BrowserStatus
		WinWidth=0.761055
		WinHeight=0.390234
		WinLeft=0.238945
		WinTop=0.030495
        StyleName="TextLabel"
        Caption=""
        bUseTextHeight=false
        Justification=TXTA_Right
        bBoundToParent=True
        bScaleToParent=True
        FontScale=FNS_Small
    End Object
    t_StatusBar=BrowserStatus

	Begin Object class=moCheckBox Name=OnlyStandardCheckBox
		bStandardized=False
		WinWidth=0.218945
		WinHeight=0.308203
		WinLeft=0.020000
		WinTop=0.093073
		Caption="Standard Servers Only"
		Hint="Only display standard servers (no mutators) in the server browser.  This option overrides filter settings."
		CaptionWidth=0.9
		TabOrder=5
        bBoundToParent=True
        bScaleToParent=True
        FontScale=FNS_Small
	End Object
	ch_Standard=OnlyStandardCheckBox

	Begin Object Class=GUIButton Name=BrowserFilter
		Caption="FILTERS"
		Hint="Filters allow more control over which servers will appear in the server browser lists."
		WinHeight=0.036482
		WinTop=0.036482
		bAutoSize=True
		WinLeft=0.0
		bBoundToParent=True
		OnClick=InternalOnClick
		RenderWeight=2
        StyleName="FooterButton"
		TabOrder=0
	End Object
	b_Filter=BrowserFilter

    Begin Object Class=GUIButton Name=BrowserBack
        Caption="BACK"
        Hint="Return to the previous menu"
		WinHeight=0.036482
		WinTop=0.085678
        RenderWeight=2
        TabOrder=4
        bBoundToParent=True
        StyleName="FooterButton"
		OnClick=InternalOnClick
    End Object
    b_Back=BrowserBack

    Begin Object Class=GUIButton Name=BrowserRefresh
        Caption="REFRESH"
        WinWidth=0.114648
        WinLeft=0.885352
		WinHeight=0.036482
		WinTop=0.085678
        RenderWeight=2
        TabOrder=3
        bBoundToParent=True
        MenuState=MSAT_Disabled
        StyleName="FooterButton"
		OnClick=InternalOnClick
    End Object
    b_Refresh=BrowserRefresh

    Begin Object Class=GUIButton Name=BrowserJoin
        Caption="JOIN"
		WinWidth=124.000000
		WinLeft=611.000000
		WinHeight=0.036482
		WinTop=0.085678
        TabOrder=2
        RenderWeight=2
        bBoundToParent=True
        MenuState=MSAT_Disabled
        StyleName="FooterButton"
		OnClick=InternalOnClick
    End Object
    b_Join=BrowserJoin

    Begin Object Class=GUIButton Name=BrowserSpec
        Caption="SPECTATE"
        WinWidth=0.114648
		WinHeight=0.036482
		WinTop=0.085678
        WinLeft=0.771094
        RenderWeight=2
        TabOrder=1
        bBoundToParent=True
        MenuState=MSAT_Disabled
        StyleName="FooterButton"
		OnClick=InternalOnClick
    End Object
    b_Spectate=BrowserSpec
}

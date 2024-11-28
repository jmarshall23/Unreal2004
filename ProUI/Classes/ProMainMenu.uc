// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class ProMainMenu extends UT2K3GUIPage
	Config(ProUI);

#exec OBJ LOAD FILE=InterfaceContent.utx

var bool			AllowClose;
var config  string	AlternateBackground;
var config  string  ProLogo;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local material m;
	Super.InitComponent(MyController, MyOwner);

    if (AlternateBackground!="")
    {
    	m = Material(DynamicLoadObject(AlternateBackground, class'Material'));
        if (m!=None)
        	Background = m;
	}

    if (ProLogo!="")
    {
    	m = Material(DynamicLoadObject(ProLogo, class'Material'));
        if (m!=None)
        {
        	GUIImage(Controls[6]).Image =m;
            GUIImage(Controls[6]).bVisible=true;
        }
	}
}

function bool MyKeyEvent(out byte Key,out byte State,float delta)
{
	if(Key == 0x1B && State == 1)	// Escape pressed
	{
		AllowClose = true;
		return true;
	}
	else
		return false;
}

function bool CanClose(optional Bool bCanceled)
{
	if(AllowClose)
		Controller.OpenMenu("xinterface.UT2QuitPage");

	return false;
}


function bool ButtonClick(GUIComponent Sender)
{
	local material m;

	if ( Sender==Controls[2] )
		Controller.OpenMenu("proui.ProInstantAction");
	if ( Sender==Controls[3] )
		Controller.OpenMenu("proui.ProServerBrowser");
	if ( Sender==Controls[4] )
		Controller.OpenMenu("proui.ProSettingsPage");
	if (Sender==Controls[5] )
		Controller.OpenMenu("xinterface.UT2QuitPage");

    if (Sender != Controls[5] && AlternateBackground!="")
    {
    	m = Material(DynamicLoadObject(AlternateBackground, class'Material'));
        if (m!=None)
			Controller.ActivePage.Background = m;
	}

	return true;
}

defaultproperties
{
	Begin Object Class=GUIImage Name=ProImgUT2Logo
		Image=material'InterfaceContent.Logos.Logo'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.800000
		WinHeight=0.500000
		WinLeft=0.100000
		WinTop=-0.033854
	End Object
	Controls(0)=GUIImage'ProImgUT2Logo'

	Begin Object Class=GUIImage Name=ProImgUT2Shader
		Image=material'InterfaceContent.Logos.fbSymbolShader'
		ImageStyle=ISTY_Scaled
		WinWidth=0.198242
		WinHeight=0.132813
		WinLeft=0.399414
		WinTop=0.223958
//		bVisible=false
	End Object
	Controls(1)=GUIImage'ProImgUT2Shader'

	Begin Object Class=GUIButton Name=ProWarmup
		StyleName="TextButton"
		Caption="WARM UP"
		Hint="Stretch those muscles before you play"
		OnClick=ButtonClick
		WinWidth=0.414063
		WinHeight=0.075000
		WinLeft=0.531250
		WinTop=0.511719
		bFocusOnWatch=true
	End Object
	Controls(2)=GUIButton'ProWarmup'

	Begin Object Class=GUIButton Name=ProPlay
		StyleName="TextButton"
		Caption="PLAY"
		Hint="Play the next round"
		OnClick=ButtonClick
		WinWidth=0.414063
		WinHeight=0.075000
		WinLeft=0.531250
		WinTop=0.610418
		bFocusOnWatch=true
	End Object
	Controls(3)=GUIButton'ProPlay'


	Begin Object Class=GUIButton Name=ProSettingsButton
		StyleName="TextButton"
		Caption="SETTINGS"
		Hint="Change Your Controls and Settings"
		OnClick=ButtonClick
		WinWidth=0.414063
		WinHeight=0.075000
		WinLeft=0.531250
		WinTop=0.733595
		bFocusOnWatch=true
	End Object
	Controls(4)=GUIButton'ProSettingsButton'

	Begin Object Class=GUIButton Name=ProQuitButton
		Caption="QUIT"
		Hint="Exit Unreal Tournament 2003"
		OnClick=ButtonClick
		StyleName="SquareMenuButton"
		WinWidth=0.205078
		WinHeight=0.042773
		WinLeft=0.634373
		WinTop=0.868225
		bFocusOnWatch=true
	End Object
	Controls(5)=GUIButton'ProQuitButton'

	Begin Object Class=GUIImage Name=ProLogoImg
		Image=material'InterfaceContent.Logos.Logo'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Alpha
		ImageColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.42
		WinHeight=0.42
		WinLeft=0.060938
		WinTop=0.497396
	End Object
	Controls(6)=GUIImage'ProLogoImg'

	Begin Object class=GUIImage Name=ProMainBK6
		WinWidth=0.434062
		WinHeight=0.526132
		WinLeft=0.521641
		WinTop=0.446354
		Image=Material'InterfaceContent.Menu.BorderBoxD'
		ImageColor=(R=255,G=255,B=255,A=160);
		ImageRenderStyle=MSTY_Alpha
		ImageStyle=ISTY_Stretched
	End Object
	Controls(7)=GUIImage'ProMainBK6'

	Begin Object class=GUILabel Name=ProBrandingBk
		Caption="Pro-Tournament Version"
		TextALign=TXTA_Center
		TextColor=(R=0,G=0,B=0,A=255)
		WinWidth=0.429688
		WinHeight=32.000000
		WinLeft=0.525039
		WinTop=0.400417
	End Object
	Controls(8)=GUILabel'ProBrandingBk'

	Begin Object class=GUILabel Name=ProBranding
		Caption="Pro-Tournament Version"
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=0,A=255)
		WinWidth=0.429688
		WinHeight=32.000000
		WinLeft=0.524414
		WinTop=0.400000
	End Object
	Controls(9)=GUILabel'ProBranding'

	Background=Material'InterfaceContent.Backgrounds.bg10'
	WinWidth=1.0
	WinHeight=1.0
	WinTop=0.0
	WinLeft=0.0
	OnCanClose=CanClose
	OnKeyEvent=MyKeyEvent
	AllowClose=False
	bAllowedAsLast=true

}

// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4Tab_ServerInfo extends MidGamePanel;

var bool bClean;
var automated GUIScrollTextBox lb_Text;
var automated GUIImage i_BG, i_BG2;
var automated GUILabel l_Title;

var localized string DefaultText;

var bool bReceivedRules;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
 	Super.InitComponent(MyController, MyOwner);

   	bClean = true;
   	lb_Text.SetContent(DefaultText);
}

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);
	if ( bShow && !bReceivedRules )
	{
		SetTimer(3.0, True);
		Timer();
    }
}

function Timer()
{
	if ( bReceivedRules || xPlayer(PlayerOwner()) == None )
	{
		KillTimer();
		return;
	}

    XPlayer(PlayerOwner()).ProcessRule = ProcessRule;
    XPlayer(PlayerOwner()).ServerRequestRules();
}

function ProcessRule(string NewRule)
{
	bReceivedRules = True;
	if (NewRule=="")
    {
    	bClean = true;
    	lb_Text.SetContent(DefaultText);
    }
    else
    {
    	if (bClean)
        	lb_Text.SetContent(NewRule);
        else
        	lb_Text.AddText(NewRule);

        bClean = false;
    }
}

defaultproperties
{
	Begin Object Class=GUIScrollTextBox Name=InfoText
		WinWidth=1.000000
		WinHeight=0.866016
		WinLeft=0.000000
		WinTop=0.143750
		CharDelay=0.0025
		EOLDelay=0
		StyleName="NoBackground"
        bNoTeletype=true
        bNeverFocus=true
        TextAlign=TXTA_Center
        bBoundToParent=true
        bScaleToParent=true
	End Object
	lb_Text=InfoText

	Begin Object class=GUIImage Name=ServerInfoBK1
		WinWidth=0.418437
		WinHeight=0.016522
		WinLeft=0.021641
		WinTop=0.070779
		Image=Material'InterfaceContent.Menu.BorderBoxD'
		ImageColor=(R=255,G=255,B=255,A=160);
		ImageRenderStyle=MSTY_Alpha
		ImageStyle=ISTY_Stretched
        bBoundToParent=true
        bScaleToParent=true
	End Object
	i_BG=ServerInfoBK1

	Begin Object class=GUIImage Name=ServerInfoBK2
		WinWidth=0.395000
		WinHeight=0.016522
		WinLeft=0.576329
		WinTop=0.070779
		Image=Material'InterfaceContent.Menu.BorderBoxD'
		ImageColor=(R=255,G=255,B=255,A=160);
		ImageRenderStyle=MSTY_Alpha
		ImageStyle=ISTY_Stretched
        bBoundToParent=true
        bScaleToParent=true
	End Object
	i_BG2=ServerInfoBK2

	Begin Object class=GUILabel Name=ServerInfoLabel
		Caption="Rules"
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=1.000000
		WinHeight=32.000000
		WinLeft=0.000000
		WinTop=0.042708
        bBoundToParent=true
        bScaleToParent=true
	End Object
	l_Title=ServerInfoLabel
    DefaultText="Receiving Rules from Server...||This feature requires that the server be running the latest patch"
}

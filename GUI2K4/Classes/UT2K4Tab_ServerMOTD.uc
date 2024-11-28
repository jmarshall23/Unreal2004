// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4Tab_ServerMOTD extends MidGamePanel;

var automated GUISectionBackground sb_MOTD, sb_Admin;
var automated GUIScrollTextBox lb_Text;
var automated GUILabel l_AdminName, l_Email;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	super.Initcomponent(MyController, MyOwner);
	sb_MOTD.ManageComponent(lb_Text);
}

function bool InternalOnPreDraw(Canvas C)
{
	//Moved here from InitComponent() in case player hasn't received GameReplicationInfo yet
	if (PlayerOwner().GameReplicationInfo != None)
	{
		lb_Text.AddText(PlayerOwner().GameReplicationInfo.MessageOfTheDay);

		l_AdminName.Caption = PlayerOwner().GameReplicationInfo.AdminName;
		l_Email.Caption = PlayerOwner().GameReplicationInfo.AdminEmail;
	    OnPreDraw = None;
	}

	return false;
}

defaultproperties
{

	Begin Object Class=AltSectionBackground Name=sbMOTD
        Caption="Message of the Day"
        LeftPadding=0.00000
        RightPadding=0.00000
        TopPadding=0.00000
        BottomPadding=0.00000
		WinWidth=0.922427
		WinHeight=0.644637
		WinLeft=0.035693
		WinTop=0.030325
        bBoundToParent=True
        bScaleToParent=True
	End Object
	sb_MOTD=sbMOTD

	Begin Object Class=AltSectionBackground Name=sbAdmin
        Caption="Your Admin is"
        LeftPadding=0.00000
        RightPadding=0.00000
        TopPadding=0.00000
        BottomPadding=0.00000
		WinWidth=0.922427
		WinHeight=0.258224
		WinLeft=0.035693
		WinTop=0.678274
        bBoundToParent=True
        bScaleToParent=True
	End Object
	sb_Admin=sbAdmin

	Begin Object Class=GUIScrollTextBox Name=MOTDText
		WinWidth=1.000000
		WinHeight=0.558333
		WinLeft=0.000000
		WinTop=0.441667
		CharDelay=0.0025
		EOLDelay=0
		StyleName="NoBackground"
        bNoTeletype=true
        bNeverFocus=true
        TextAlign=TXTA_Center
        bBoundToParent=true
        bScaleToParent=true
	End Object
	lb_Text=MOTDText

	Begin Object class=GUILabel Name=lEmail
		Caption=""
		FontScale=FNS_Small
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.893120
		WinHeight=0.069115
		WinLeft=0.049329
		WinTop=0.801416
        bBoundToParent=true
        bScaleToParent=true
	End Object
	l_Email=lEmail

	Begin Object class=GUILabel Name=lAdminName
		Caption=""
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		TextFont="UT2HeaderFont"
		WinWidth=0.901341
		WinHeight=0.069115
		WinLeft=0.049329
		WinTop=0.747420
        bBoundToParent=true
        bScaleToParent=true
	End Object
	l_AdminName=lAdminName

    WinWidth=1
    WinHeight=0.7
    OnPreDraw=InternalOnPreDraw

}

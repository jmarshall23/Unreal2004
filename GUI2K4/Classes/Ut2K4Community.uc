// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class Ut2K4Community extends ModsAndDemosTabs;

var automated GUIScrollTextBox 	CommunityNews;
var bool 						GotNews;
var localized string 			DefaultNews;
var MasterServerClient			MSC;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);
	CommunityNews.SetContent(DefaultNews);
	CommunityNews.MyScrollText.bClickText=true;
	CommunityNews.MyScrollText.OnDblClick=LaunchURL;
}

function bool LaunchURL(GUIComponent Sender)
{
    local string ClickString;

    ClickString = StripColorCodes(CommunityNews.MyScrollText.ClickedString);
   	Controller.LaunchURL(ClickString);
    return true;
}


defaultproperties
{
	Begin Object class=GUIScrollTextBox Name=lbCommunityNews
		WinWidth=0.96
		WinHeight=0.96
		WinLeft=0.02
		WinTop=0.02
        TabOrder=0
        bVisibleWhenEmpty=true
        bNoTeletype=true
	End Object
    CommunityNews=lbCommunityNews
	Tag=0

    DefaultNews="Thank you for purchasing Unreal Tournament 2004||Attempting to retrieve the latest news from the Master Server, please stand by..."

}

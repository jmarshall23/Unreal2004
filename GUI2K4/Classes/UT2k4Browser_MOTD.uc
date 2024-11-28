//====================================================================
//  Online message of the day page.
//
//  Written by Joe Wilcox
//	Updated by Ron Prestenback
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class UT2K4Browser_MOTD extends UT2K4Browser_Page;

var automated GUIScrollTextBox		lb_MOTD;
var automated GUILabel				l_Version;
var automated GUIButton             b_QuickConnect;

var String MOTD;
var bool bUpgrade, bOptionalUpgrade;
var bool GotMOTD;

var float 	ReReadyPause;
var int		RetryCount;
var() config int RetryMax;

var localized string UpgradeCaption;
var localized string VersionString;

var config string QuickConnectMenu;
var localized string ConnectFailed;
var localized string ConnectTimeout;

event Timer()
{
	if (RetryCount++ < RetryMax)
	{
		SetFooterCaption( RetryString, True );
		Browser.Uplink().StartQuery(CTM_GetMOTD);
	}
	else
	{
		SetFooterCaption( ReadyString );
		KillTimer();
	}
}

event Opened(GUIComponent Sender)
{
	l_Version.Caption = VersionString@PlayerOwner().Level.EngineVersion;
	if ( !GotMOTD )
	{
		DisableComponent(b_QuickConnect);
		Refresh();
	}

	Super.Opened(Sender);
}

function ShowPanel( bool bShow )
{
	Super.ShowPanel(bShow);
	if ( bShow && !bInit )
		BindQueryClient(Browser.Uplink());
}

function Refresh()
{
	KillTimer();
	bUpgrade = false;

	CheckJoinButton(False);

	lb_MOTD.Stop();
	RetryCount = 0;
	SetFooterCaption( StartQueryString );
	ResetQueryClient( Browser.Uplink() );
	Browser.Uplink().StartQuery(CTM_GetMOTD);
}

// delegates
function ReceivedMOTD( MasterServerClient.EMOTDResponse Command, string Data )
{
	switch( Command )
	{
	case MR_MOTD:
		GotMOTD = true;
		EnableComponent(b_QuickConnect);
		lb_MOTD.SetContent(Data, Chr(13));
		break;

	case MR_OptionalUpgrade:
		bOptionalUpgrade = True;
		CheckJoinButton(True);
		break;

	case MR_MandatoryUpgrade:
		bUpgrade = true;
		CheckJoinButton(True);
		break;

	case MR_NewServer:
		break;
	case MR_IniSetting:
		break;
	case MR_Command:
		break;
	}
}

function OpenStatusMessage(string Code, optional string data)
{
		if (!Browser.bHideNetworkMessage)
			Controller.OpenMenu(Controller.NetworkMsgMenu,Code,Data);

		Browser.bHideNetworkMessage=true;
		SetFooterCaption(AuthFailString);
		SetTimer(ReReadyPause, false);
}


function QueryComplete( MasterServerClient.EResponseInfo ResponseInfo, int Info )
{
	switch( ResponseInfo )
	{
	case RI_Success:
		if ( Info == 1 )  // CTM_GetMOTD
		{
			SetFooterCaption(QueryCompleteString);

			// Allow a few seconds to display the "Query Complete" message, then set the caption to "Ready"
			RetryCount = RetryMax;
			SetTimer(ReReadyPause, false);

			if( !bUpgrade && !Browser.Verified)
				Browser.MOTDVerified();
		}
		break;

	case RI_AuthenticationFailed:
		OpenStatusMessage("RI_AuthenticationFailed");break;
	case RI_DevClient:
		OpenStatusMessage("RI_DevClient");break;
    case RI_BadClient:
         OpenStatusMessage("RI_BadClient");break;
    case RI_BannedClient:
         OpenStatusMessage("RI_BannedClient",ResponseInfo$Browser.Uplink().OptionalResult);break;
	case RI_ConnectionFailed:
		lb_MOTD.SetContent(ConnectFailed);
		break;
	case RI_ConnectionTimeout:
		lb_MOTD.SetContent(ConnectTimeout);
		break;
	}
}

function JoinClicked()
{
	Browser.Uplink().LaunchAutoUpdate();
}

function bool IsSpectateAvailable( out string ButtonCaption )
{
	ButtonCaption = SpectateCaption;
	return false;
}

function bool IsJoinAvailable( out string ButtonCaption )
{
	ButtonCaption = UpgradeCaption;
	return bUpgrade || bOptionalUpgrade;
}

function bool IsRefreshAvailable( out string ButtonCaption )
{
	ButtonCaption = RefreshCaption;
	return true;
}

function bool InternalOnClick(GUIComponent Sender)
{
	if ( Sender == b_QuickConnect )
	{
		if ( Controller.OpenMenu(QuickConnectMenu) )
			Controller.ActivePage.HandleObject( Browser.Uplink() );


		return true;
	}

	return false;
}

function ResetQueryClient( ServerQueryClient Client )
{
	Super.ResetQueryClient(Client);

	if ( MasterServerClient(Client) != None )
		MasterServerClient(Client).Query.Length = 0;
}

function BindQueryClient( ServerQueryClient Client )
{
	Super.BindQueryClient(Client);
	if ( MasterServerClient(Client) != None )
	{
		MasterServerClient(Client).OnReceivedMOTDData = ReceivedMOTD;
		MasterServerClient(Client).OnQueryFinished    = QueryComplete;
	}
}

defaultproperties
{
	Begin Object class=GUIScrollTextBox Name=MyMOTDText
		TextAlign=TXTA_Left
		WinWidth=1.0
		WinHeight=0.833203
		WinLeft=0.0
		WinTop=0.002679
		CharDelay=0.05
		EOLDelay=0.1
		bNeverFocus=true
		bAcceptsInput=true
		RenderWeight=0.6
		TabOrder=1
		bVisibleWhenEmpty=True
		bNoTeletype=True
		StyleName="ListBox"
	End Object
	lb_MOTD=MyMOTDText

	Begin Object class=GUILabel Name=VersionNum
		Caption=""
		StyleName="TextLabel"
		TextALign=TXTA_Right
		WinWidth=0.202128
		WinHeight=0.040000
		WinLeft=0.793500
		WinTop=-0.043415
		bBoundToParent=false
		bScaleToParent=false
		RenderWeight=20.7
        bTransparent=true
	End Object
	l_Version=VersionNum

	Begin Object Class=GUIButton Name=QuickPlay
		WinWidth=0.161994
		WinHeight=0.079063
		WinLeft=0.425180
		WinTop=0.866146
		TabOrder=2
		Caption="QUICK PLAY"
		Hint="Open a dialog that can help you easily find the best online server based on your criteria"
		OnClick=InternalOnClick
		bAutoSize=True
	End Object
	b_QuickConnect=QuickPlay

	ReReadyPause=3.0
	RetryMax=10
	VersionString="Version"

	PanelCaption="News from Epic"
	UpgradeCaption="UPGRADE"

	QuickConnectMenu="GUI2K4.UT2K4QuickPlay"
	ConnectFailed="The UT2004 master server could not be reached.  Please try again later."
	ConnectTimeout="Your connection to the UT2004 master server has timed out"

}

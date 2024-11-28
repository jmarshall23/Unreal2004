//==============================================================================
//	Written by Ron Prestenback
//  This menu indicates that a non-recoverable network error has occurred, and the player will
//  be kicked back to the main menu when this menu is closed.
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4NetworkStatusMsg extends LockedFloatingWindow;

var automated GUIScrollTextBox stbNetworkMessage;
var localized 	string 	StatusMessages[14];
var localized 	string 	StatusTitle[14];
var localized   string  UnknownError;
var 			string	StatusCodes[14];
var 			bool	bExit;
function InitComponent(GUIController MyController, GUIComponent MyOwner)
{

	Super.InitComponent(Mycontroller, MyOwner);
	PlayerOwner().ClearProgressMessages();

	b_Cancel.SetVisibility(false);
    b_OK.OnClick = InternalOnClick;

	sb_Main.ManageComponent(stbNetworkMessage);
}

event bool NotifyLevelChange()
{
	return false;
}

event HandleParameters(string Param1, string Param2)
{
	local int i;
	local string s;

	for (i=0;i<ArrayCount(StatusCodes);i++)
    	if (StatusCodes[i] ~= Param1)
        {
        	s = StatusMessages[i];
        	i = instr(s,"%data%");
        	if ( i >= 0 )
        		ReplaceText(s, "%data%", Param2);

        	t_WindowTitle.SetCaption(StatusTitle[i]);
        	break;
        }

	if ( i == ArrayCount(StatusCodes) )
	{
		if (Param1=="")
		{
			s = UnknownError;
			if ( Param1 != "" )
				s $= "||" $ Param1;

			if ( Param2 != "" )
				s $= "||" $ Param2;
		}
		else
			s = Param1 $ "||" $ Param2;
	}

	stbNetworkMessage.SetContent(s);
}
function bool FloatingPreDraw( Canvas C )
{
	if (bExit)
	{
		Controller.CloseMenu(False);
		return true;
	}
	return super.FloatingPreDraw(c);
}


defaultproperties
{
	Begin Object Class=GUIScrollTextBox Name=Scroller
		WinWidth=0.925338
		WinHeight=0.790203
		WinLeft=0.033108
		WinTop=0.133333
        bNoTeletype=true
        StyleName="NoBackground"
	End Object
    stbNetworkMessage=Scroller

	WindowName="Network Connection Lost"

    StatusCodes(0)="RI_AuthenticationFailed"
    StatusTitle(0)="Invalid CD-Key"
	StatusMessages(0)="The Master server has determined your CD-Key is either invalid or already in use.  If this problem persists, please contact Atari Technicial support."
    StatusCodes(1)="RI_ConnectionFailed"
    StatusTitle(1)="Connection Failed"
	StatusMessages(1)="A communication link to the Unreal Tournament 2004 master server could not be established.  Please check your connection to the internet and try again."
    StatusCodes(2)="RI_ConnectionTimeOut"
    StatusTitle(2)="Connection Timed Out"
	StatusMessages(2)="Apparently, your communication link to the Unreal Tournament 2004 master server has been interrupted.  Please check your connection to the internet and try again."
    StatusCodes(3)="RI_MustUpgrade"
    StatusTitle(3)="Must Upgrade"
    StatusMessages(3)="Must Upgrade"
    StatusCodes(4)="RI_DevClient"
    StatusTitle(4)="Developer Mode"
	StatusMessages(4)="Client is in Developer Mode!||Your client is currently operating in developer mode and it's access to the master server has been restricted.  Please restart the game and avoid using SET commands that may cause problems.  If the problem persists, please contact Atari Technical Support."
    StatusCodes(5)="RI_BadClient"
    StatusTitle(5)="Modified Client"
	StatusMessages(5)="Modified Client!||Your copy of Unreal Tournament 2004 has in some way been modified.  Because of this, its access to the master server has been restricted.  If this problem persists, please reinstall the game or the latest patch.||This error has been logged at the master server."
    StatusCodes(6)="RI_BannedClient"
    StatusTitle(6)="You have been Banned"
    StatusMessages(6)="Your CD-KEY has been banned by the master server.  this ban will remain in place until %data%"

    StatusCodes(7)="FC_ServerFull"
    StatusTitle(7)="The Server is Full"
    StatusMessages(7)="The server you are trying to connect to is currently full.  Please try this server again later, or you may wish to try connecting as a spectator if the server allows it"
    StatusCodes(8)="FC_LocalBan"
    StatusTitle(8)="You have been banned"
    StatusMessages(8)="The admin of this server has banned you playing here.  If you feel you do not deserve this ban, the admin's contact email is [%data%]"
    StatusCodes(9)="FC_Challege"
    StatusTitle(9)="Incompatible Version"
    StatusMessages(9)="This copy of UT2004 is not compatible with the server you are connecting to."
    StatusCodes(10)="FC_Login"
    StatusTitle(10)="Problem during Login"
    StatusMessages(10)="There was a fatal problem during login that has caused you to be disconnected from the server.||Reason: %data%"

    StatusCodes(11)="AC_Kicked"
    StatusTitle(11)="Kicked"
    StatusMessages(11)="You have been forcibly removed from the game.||Reason: %data%"

    StatusCodes(12)="AC_SessionBan"
    StatusTitle(12)="Session Ban"
    StatusMessages(12)="An admin of this server has banned you for the remainder of the current game.||If you feel you do not deserve this ban, the admin's email is:|[%data%]."

    StatusCodes(13)="AC_Ban"
    StatusTitle(13)="Permanent Ban"
    StatusMessages(13)="You've been permanently banned from playing on this server.||If you feel you do not deserve this ban, the admin's contact email is:|[%data%]."

//    StatusCode(12)="
//    StatusTitle(12)="
//    StatusMessage(12)="
//    StatusCode(13)="
//    StatusTitle(13)="
//    StatusMessage(13)="

	UnknownError="Unknown Network Error"
}

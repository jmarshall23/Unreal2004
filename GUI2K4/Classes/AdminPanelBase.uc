//==============================================================================
//  Created on: 11/12/2003
//  Base class for admin controls
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class AdminPanelBase extends GUIPanel;

var() localized string PanelCaption;
var() noexport  bool bAdvancedAdmin;

function bool IsAdmin()
{
	return PlayerOwner() != None && PlayerOwner().PlayerReplicationInfo != None && PlayerOwner().PlayerReplicationInfo.bAdmin;
}

function AdminCommand( string Command )
{
	if ( PlayerOwner() != None )
		PlayerOwner().AdminCommand(Command);
}

function LoggedIn( string AdminName );
function LoggedOut();

function SetAdvanced( bool bIsAdvanced )
{
	bAdvancedAdmin = bIsAdvanced;
}

function AdminReply(string Reply);
function ShowPanel();

DefaultProperties
{
	WinWidth=1.000000
	WinHeight=0.862502
	WinLeft=0.000000
	WinTop=0.131250
	bScaleToParent=True
	bBoundToParent=True
}

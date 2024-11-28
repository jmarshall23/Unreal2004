//==============================================================================
//  Created on: 11/23/2003
//  Playinfo stuff
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================
class AdminPanelRules extends AdminPanelBase;

var automated RemotePlayInfoPanel p_Main;

DefaultProperties
{
	Begin Object Class=RemotePlayInfoPanel Name=PlayInfoPanel
	End Object
	p_Main=PlayInfoPanel
}

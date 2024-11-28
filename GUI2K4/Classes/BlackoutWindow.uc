//==============================================================================
//	Created on: 10/10/2003
//	Base class for message windows which do not allow the menus beneath it to be viewed
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class BlackoutWindow extends MessageWindow;

DefaultProperties
{
	bRequire640x480=False
	OpenSound=sound'MenuSounds.SelectDshort'
}

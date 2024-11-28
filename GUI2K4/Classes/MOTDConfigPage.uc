//==============================================================================
//  Created on: 11/17/2003
//  Description
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class MOTDConfigPage extends GUIArrayPropPage;

var automated AltSectionBackground sb_Bk1;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);
	sb_Main.bVisible=false;
	sb_Bk1.Managecomponent(lb_Values);
}

function SetOwner(GUIComponent NewOwner)
{
	Super.SetOwner(NewOwner);
	PropValue.Length = 4;
}

function string GetDataString()
{
	return JoinArray(PropValue, "|", True);
}

function SetItemOptions( GUIMenuOption mo )
{
	local moEditBox ed;

	ed = moEditBox(mo);
	if ( ed != None )
		ed.MyEditBox.MaxWidth = 60;
}

DefaultProperties
{
	Begin Object class=AltSectionBackground name=BK1
		WinWidth=0.621875
		WinHeight=0.340625
		WinLeft=0.043750
		WinTop=0.116666
		TopPadding=0.01
		LeftPadding=0.01
		RightPadding=0.01
	End Object
	sb_BK1=bk1

	WinWidth=0.684570
	WinHeight=0.509375
	WinLeft=0.166992
	WinTop=0.218750

	Delim="|"
}

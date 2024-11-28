//==============================================================================
// GUI button for the championchip, also base class for the other ladder buttons
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4LadderButton extends GUIGFXButton;

var UT2K4MatchInfo MatchInfo;
var CacheManager.MapRecord MyMapRecord;
var int MatchIndex;
var int LadderIndex;

/**
	The bar connection from the previous match
	Light up when this match is available
*/
var GUIImage ProgresBar;
var Material PBNormal, PBActive;

function SetState(int Rung);

defaultproperties
{
	Graphic=Material'InterfaceContent.SPMenu.Lock1'
	WinWidth=0.083925
	WinHeight=0.113688
	bVisible=false
	Position=ICP_Scaled
	bRepeatClick=false
}
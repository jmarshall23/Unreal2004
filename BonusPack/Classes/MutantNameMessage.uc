// ====================================================================
//  Class: BonusPack.MutantNameMessage
//
//  Sends Bottom Feeder Message
//
//  Written by James Golding
//  (c) 2002, Epic Games, Inc.  All Rights Reserved
// ====================================================================

class MutantNameMessage extends LocalMessage;

var()	localized String	BottomFeederMesg;

static function string GetString(
    optional int Switch,
    optional PlayerReplicationInfo RelatedPRI_1, 
    optional PlayerReplicationInfo RelatedPRI_2,
    optional Object OptionalObject
    )
{
	if(Switch == 0)
		return RelatedPRI_1.PlayerName;
	else if(Switch == 1)
		return RelatedPRI_1.PlayerName@Default.BottomFeederMesg;
}

static function color GetColor(
    optional int Switch,
    optional PlayerReplicationInfo RelatedPRI_1, 
    optional PlayerReplicationInfo RelatedPRI_2
    )
{
	if ( Switch == 0 )
		return class'PlayerNameMessage'.Default.DrawColor;
	else
		return Default.DrawColor;
}

defaultproperties
{
	bFadeMessage=True
	bIsUnique=True
	FontSize=0
	bBeep=False
    Lifetime=2
	bIsConsoleMessage=false

	BottomFeederMesg="(BOTTOM FEEDER)"

    DrawColor=(R=0,G=255,B=255,A=255)
    DrawPivot=DP_MiddleMiddle
    StackMode=SM_None
    PosX=0.5
    PosY=0.58
}
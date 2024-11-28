class BombTargetMessage extends LocalMessage;

var Localized string TargetMessage;

static function string GetString(
    optional int Switch,
    optional PlayerReplicationInfo RelatedPRI_1, 
    optional PlayerReplicationInfo RelatedPRI_2,
    optional Object OptionalObject
    )
{
    return default.TargetMessage;
}

defaultproperties
{
	bFadeMessage=True
	bIsUnique=True
	FontSize=0
	bBeep=False
    Lifetime=1
	bIsConsoleMessage=false
	TargetMessage="Incoming Pass"

    DrawColor=(R=200,G=200,B=200,A=200)
    DrawPivot=DP_MiddleMiddle
    StackMode=SM_None
    PosX=0.5
    PosY=0.65
}
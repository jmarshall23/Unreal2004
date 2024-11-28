class PlayerNameMessage extends LocalMessage;

static function string GetString(
    optional int Switch,
    optional PlayerReplicationInfo RelatedPRI_1, 
    optional PlayerReplicationInfo RelatedPRI_2,
    optional Object OptionalObject
    )
{
    return RelatedPRI_1.PlayerName;
}

defaultproperties
{
	bFadeMessage=True
	bIsUnique=True
	FontSize=0
	bBeep=False
    Lifetime=1
	bIsConsoleMessage=false

    DrawColor=(R=0,G=200,B=0,A=200)
    DrawPivot=DP_MiddleMiddle
    StackMode=SM_None
    PosX=0.5
    PosY=0.58
}
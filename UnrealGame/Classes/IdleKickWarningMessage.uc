class IdleKickWarningMessage extends LocalMessage;

static function string GetString(
	optional int Switch,
	optional PlayerReplicationInfo RelatedPRI_1, 
	optional PlayerReplicationInfo RelatedPRI_2,
	optional Object OptionalObject
	)
{
	UnrealPlayer(OptionalObject).LastKickWarningTime = UnrealPlayer(OptionalObject).Level.TimeSeconds;
    return class'GameMessage'.Default.KickWarning;
}
defaultproperties
{
	bFadeMessage=true
	bIsUnique=false
	bIsPartiallyUnique=true
	bIsConsoleMessage=False
	Lifetime=1

	DrawColor=(R=255,G=255,B=64,A=255)
	FontSize=1

	StackMode=SM_Down
    PosY=0.242
}
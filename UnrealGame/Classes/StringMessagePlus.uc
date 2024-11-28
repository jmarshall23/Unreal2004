class StringMessagePlus extends LocalMessage;

static function string AssembleString(
	HUD myHUD,
	optional int Switch,
	optional PlayerReplicationInfo RelatedPRI_1, 
	optional String MessageString
	)
{
	return MessageString;
}

defaultproperties
{
    bIsSpecial=false
	DrawColor=(R=255,G=255,B=255,A=255)
}
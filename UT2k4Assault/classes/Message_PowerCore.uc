class Message_PowerCore extends LocalMessage;

static function string GetString(
	optional int Switch,
	optional PlayerReplicationInfo RelatedPRI_1,
	optional PlayerReplicationInfo RelatedPRI_2,
	optional Object OptionalObject
	)
{
	switch( Switch )
	{
		case 0 : return RelatedPRI_1.PlayerName $ class'GameObject_EnergyCore'.default.PlayerDroppedMessage;
		case 1 : return class'GameObject_EnergyCore'.default.DroppedMessage;
		case 2 : return class'GameObject_EnergyCore'.default.EnergyCorePickedUp;
		case 3 : return RelatedPRI_1.PlayerName $ class'GameObject_EnergyCore'.default.PlayerPickedUpEnergyCore;
		case 4 : return class'GameObject_EnergyCore'.default.PlayerCoreReset;
	}
}

static simulated function ClientReceive(
	PlayerController P,
	optional int Switch,
	optional PlayerReplicationInfo RelatedPRI_1,
	optional PlayerReplicationInfo RelatedPRI_2,
	optional Object OptionalObject
	)
{
	super.ClientReceive(P, Switch, RelatedPRI_1, RelatedPRI_2, OptionalObject);
		
	Switch( Switch )
	{
		case 0 :
		case 1 : P.QueueAnnouncement( class'GameObject_EnergyCore'.default.Announcer_EnergyCore_Dropped, 1); break;
		case 2 :
		case 3 : P.QueueAnnouncement( class'GameObject_EnergyCore'.default.Announcer_EnergyCore_PickedUp, 1); break;
		case 4 : P.QueueAnnouncement( class'GameObject_EnergyCore'.default.Announcer_EnergyCore_Reset, 1); break;
	}
}

defaultproperties
{
	bIsConsoleMessage=false
	bFadeMessage=true
	bIsSpecial=true
	bIsUnique=false
	Lifetime=3
	bBeep=false

	DrawColor=(R=255,G=0,B=0)
	FontSize=1

	StackMode=SM_Down
	PosY=0.242
}

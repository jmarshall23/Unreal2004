class DamTypeFlakChunk extends WeaponDamageType
	abstract;

var sound FlakMonkey; //OBSOLETE

static function IncrementKills(Controller Killer)
{
	local xPlayerReplicationInfo xPRI;

	xPRI = xPlayerReplicationInfo(Killer.PlayerReplicationInfo);
	if ( xPRI != None )
	{
		xPRI.flakcount++;
		if ( (xPRI.flakcount == 15) && (UnrealPlayer(Killer) != None) )
			UnrealPlayer(Killer).ClientDelayedAnnouncementNamed('FlackMonkey',15);
	}
}

defaultproperties
{
    DeathString="%o was shredded by %k's flak cannon."
	MaleSuicide="%o was perforated by his own flak."
	FemaleSuicide="%o was perforated by her own flak."

    WeaponClass=class'FlakCannon'
    VehicleMomentumScaling=0.5
    bDelayedDamage=true
    bBulletHit=True
}


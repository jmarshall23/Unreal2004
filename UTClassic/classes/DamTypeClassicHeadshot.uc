class DamTypeClassicHeadshot extends WeaponDamageType
	abstract;

var class<LocalMessage> KillerMessage;
var sound HeadHunter; // OBSOLETE

static function IncrementKills(Controller Killer)
{
	local xPlayerReplicationInfo xPRI;
	
	if ( PlayerController(Killer) == None )
		return;
		
	PlayerController(Killer).ReceiveLocalizedMessage( Default.KillerMessage, 0, Killer.PlayerReplicationInfo, None, None );
	xPRI = xPlayerReplicationInfo(Killer.PlayerReplicationInfo);
	if ( xPRI != None )
	{
		xPRI.headcount++;
		if ( (xPRI.headcount == 15) && (UnrealPlayer(Killer) != None) )
			UnrealPlayer(Killer).ClientDelayedAnnouncementNamed('HeadHunter',15);
	}
}		

defaultproperties
{
    DeathString="%k put a bullet in %o's skull."
	MaleSuicide="%o shot himself in the head."
	FemaleSuicide="%o shot herself in the head."

    WeaponClass=class'ClassicSniperRifle'
    bAlwaysSevers=true
    bSpecial=true
    KillerMessage=class'SpecialKillMessage'
	VehicleDamageScaling=0.65
}

class UTWeaponPickup extends WeaponPickup;

var(WeaponPickup) vector StandUp;	// rotation change used by WeaponLocker
var(WeaponPickup) float LockerOffset;

simulated event ClientTrigger()
{
	bHidden = true;
	if ( EffectIsRelevant(Location, false) && !Level.GetLocalPlayerController().BeyondViewDistance(Location, CullDistance)  )
		spawn(class'WeaponFadeEffect',self);
}
	
function RespawnEffect()
{
	spawn(class'PlayerSpawnEffect');
}

State FadeOut
{
	function Tick(float DeltaTime)
	{
		disable('Tick');
	}

	function BeginState()
	{
		bHidden = true;
		LifeSpan = 1.0;
		bClientTrigger = !bClientTrigger;
		if ( Level.NetMode != NM_DedicatedServer )
			ClientTrigger();
	}
}

defaultproperties
{
    MessageClass=class'PickupMessagePlus'
    Standup=(Z=0.75)
    LockerOffset=+35.0
}
class TransFire extends ProjectileFire;

var() Sound TransFireSound;
var() Sound RecallFireSound;
var() String TransFireForce;
var() String RecallFireForce;

simulated function PlayFiring()
{
    if (!TransLauncher(Weapon).bBeaconDeployed)
    {
        Weapon.PlayAnim(FireAnim, FireAnimRate, TweenTime);
        ClientPlayForceFeedback( TransFireForce );  // jdf
    }
}

function Rotator AdjustAim(Vector Start, float InAimError)
{
    return Instigator.Controller.Rotation;
}

simulated function bool AllowFire()
{
    return ( TransLauncher(Weapon).AmmoChargeF >= 1.0 );
}

function projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local TransBeacon TransBeacon;

    if (TransLauncher(Weapon).TransBeacon == None)
    {
		if ( (Instigator == None) || (Instigator.PlayerReplicationInfo == None) || (Instigator.PlayerReplicationInfo.Team == None) )
			TransBeacon = Weapon.Spawn(class'XWeapons.TransBeacon',,, Start, Dir);
		else if ( Instigator.PlayerReplicationInfo.Team.TeamIndex == 0 )
			TransBeacon = Weapon.Spawn(class'XWeapons.RedBeacon',,, Start, Dir);
		else
			TransBeacon = Weapon.Spawn(class'XWeapons.BlueBeacon',,, Start, Dir);
        TransLauncher(Weapon).TransBeacon = TransBeacon;
        Weapon.PlaySound(TransFireSound,SLOT_Interact,,,,,false);
    }
    else
    {
        TransLauncher(Weapon).ViewPlayer();
        if ( TransLauncher(Weapon).TransBeacon.Disrupted() )
        {
			if( (Instigator != None) && (PlayerController(Instigator.Controller) != None) )
				PlayerController(Instigator.Controller).ClientPlaySound(Sound'WeaponSounds.BSeekLost1');
		}
		else
		{
			TransLauncher(Weapon).TransBeacon.Destroy();
			TransLauncher(Weapon).TransBeacon = None;
			Weapon.PlaySound(RecallFireSound,SLOT_Interact,,,,,false);
		}
    }
    return TransBeacon;
}

defaultproperties
{
    AmmoClass=None
    AmmoPerFire=1
    FireAnimRate=1.5

    ProjectileClass=class'XWeapons.TransBeacon'

    ProjSpawnOffset=(X=25,Y=8,Z=-10)
    FireRate=0.25
    bWaitForRelease=true
    bModeExclusive=false
    FireSound=None
    TransFireSound=Sound'WeaponSounds.Translocator.TranslocatorFire'
    RecallFireSound=Sound'WeaponSounds.Translocator.TranslocatorModuleRegeneration'
    TransFireForce="TranslocatorFire"  // jdf
    RecallFireForce="TranslocatorModuleRegeneration"  // jdf

    BotRefireRate=0.3
    bLeadTarget=false
}

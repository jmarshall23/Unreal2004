class RedeemerFire extends ProjectileFire;

function Projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local Projectile p;

    p = Super.SpawnProjectile(Start,Dir);
    if ( p == None )
        p = Super.SpawnProjectile(Instigator.Location,Dir);
    if ( p == None )
    {
	 	Weapon.Spawn(class'SmallRedeemerExplosion');
		Weapon.HurtRadius(500, 400, class'DamTypeRedeemer', 100000, Instigator.Location);
	}
    return p;
}

function float MaxRange()
{
	return 20000;
}

defaultproperties
{
    AmmoClass=class'RedeemerAmmo'
    AmmoPerFire=1
    FireRate=1.0

    ProjectileClass=class'XWeapons.RedeemerProjectile'
    ProjSpawnOffset=(X=100,Y=0,Z=0)

    FlashEmitterClass=None

    FireSound=Sound'WeaponSounds.redeemer_shoot'
    FireForce="redeemer_shoot"  // jdf

    bSplashDamage=true
    bRecommendSplashDamage=true
    BotRefireRate=0.5
    TransientSoundVolume=+1.0

    ShakeOffsetMag=(X=-20.0,Y=0.00,Z=0.00)
    ShakeOffsetRate=(X=-1000.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

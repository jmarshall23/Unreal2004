class ShockProjFire extends ProjectileFire;

function InitEffects()
{
    Super.InitEffects();
    if ( FlashEmitter != None )
		Weapon.AttachToBone(FlashEmitter, 'tip');
}

// for bot combos
function projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local Projectile p;

    p = Super.SpawnProjectile(Start,Dir);
	if ( (ShockRifle(Instigator.Weapon) != None) && (p != None) )
		ShockRifle(Instigator.Weapon).SetComboTarget(ShockProjectile(P));
	return p;
}

defaultproperties
{
    AmmoClass=class'ShockAmmo'
    AmmoPerFire=1

    FireAnim=AltFire
    FireAnimRate=1.5

    FlashEmitterClass=class'XEffects.ShockProjMuzFlash'

    ProjectileClass=class'XWeapons.ShockProjectile'

    ProjSpawnOffset=(X=24,Y=8,Z=0)

    FireSound=Sound'WeaponSounds.ShockRifle.ShockRifleAltFire'
    FireForce="ShockRifleAltFire"  // jdf
	TransientSoundVolume=+0.4

    FireRate=0.6
    bModeExclusive=true

    bSplashDamage=true
    BotRefireRate=0.35
	WarnTargetPct=+0.5

    ShakeOffsetMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeOffsetRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=0
    ShakeRotMag=(X=60.0,Y=20.0,Z=0.0)
    ShakeRotRate=(X=1000.0,Y=1000.0,Z=0.0)
    ShakeRotTime=2
}

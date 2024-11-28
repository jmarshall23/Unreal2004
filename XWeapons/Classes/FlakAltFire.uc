class FlakAltFire extends ProjectileFire;

function InitEffects()
{
    Super.InitEffects();
    if ( FlashEmitter == None )
		FlashEmitter = Weapon.GetFireMode(0).FlashEmitter;
}

defaultproperties
{
    AmmoClass=class'FlakAmmo'
    AmmoPerFire=1

    FireAnim=AltFire
    FireAnimRate=0.9
    FireEndAnim=None

    ProjectileClass=class'XWeapons.FlakShell'
    ProjSpawnOffset=(X=25,Y=9,Z=-12)

    FlashEmitterClass=None

    FireSound=Sound'WeaponSounds.FlakCannon.FlakCannonAltFire'
    FireForce="FlakCannonAltFire"  // jdf

    FireRate=1.11

    bSplashDamage=true
    bRecommendSplashDamage=true
    BotRefireRate=0.5
    bTossed=true
	WarnTargetPct=+0.9

    ShakeOffsetMag=(X=-20.0,Y=0.00,Z=0.00)
    ShakeOffsetRate=(X=-1000.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

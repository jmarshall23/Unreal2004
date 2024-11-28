class BioFire extends ProjectileFire;

function DrawMuzzleFlash(Canvas Canvas)
{
    if (FlashEmitter != None)
        FlashEmitter.SetRotation(Weapon.Rotation);
    Super.DrawMuzzleFlash(Canvas);
}

function float MaxRange()
{
	return 1500;
}

defaultproperties
{
    AmmoClass=class'BioAmmo'
    AmmoPerFire=1

    FireAnim=Fire
    FireEndAnim=None

    ProjectileClass=class'XWeapons.BioGlob'
    FireRate=0.33

    ProjSpawnOffset=(X=20,Y=9,Z=-6)
    FlashEmitterClass=class'XEffects.BioMuzFlash1st'
    FireSound=Sound'WeaponSounds.BioRifle.BioRifleFire'
    
    FireForce="BioRifleFire"  // jdf
    
    bSplashDamage=true
    bRecommendSplashDamage=true
    BotRefireRate=0.8
    bTossed=true

    ShakeOffsetMag=(X=0.0,Y=0.0,Z=-2.0)
    ShakeOffsetRate=(X=0.0,Y=0.0,Z=1000.0)
    ShakeOffsetTime=1.8
    ShakeRotMag=(X=70.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=1000.0,Y=0.0,Z=0.0)
    ShakeRotTime=1.8
}

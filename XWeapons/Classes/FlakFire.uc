class FlakFire extends ProjectileFire;

function InitEffects()
{
    Super.InitEffects();
    if ( FlashEmitter != None )
		Weapon.AttachToBone(FlashEmitter, 'tip');
}

defaultproperties
{
    AmmoClass=class'FlakAmmo'
    AmmoPerFire=1

    FireAnim=Fire
    FireAnimRate=0.95
    FireEndAnim=None

    ProjectileClass=class'XWeapons.FlakChunk'
    ProjPerFire=9
    ProjSpawnOffset=(X=25,Y=5,Z=-6)

    SpreadStyle=SS_Random
    Spread=1400

    FlashEmitterClass=class'XEffects.FlakMuzFlash1st'

    FireSound=Sound'WeaponSounds.FlakCannon.FlakCannonFire'
    FireForce="FlakCannonFire"  // jdf

    FireRate=0.8947

    BotRefireRate=0.7

    ShakeOffsetMag=(X=-20.0,Y=0.00,Z=0.00)
    ShakeOffsetRate=(X=-1000.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

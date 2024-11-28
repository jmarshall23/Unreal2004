
class RocketFire extends ProjectileFire;

function PlayFireEnd()
{
}

function InitEffects()
{
    Super.InitEffects();
    if ( FlashEmitter != None )
		Weapon.AttachToBone(FlashEmitter, 'tip');
}

function PlayFiring()
{
    Super.PlayFiring();
    RocketLauncher(Weapon).PlayFiring(true);
}

function Projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local Projectile p;
    
    p = RocketLauncher(Weapon).SpawnProjectile(Start, Dir);
    if ( p != None )
		p.Damage *= DamageAtten;
    return p;
}

defaultproperties
{
    AmmoClass=class'RocketAmmo'
    AmmoPerFire=1

    FireAnim=Fire
    FireAnimRate=1.0

    ProjectileClass=class'XWeapons.RocketProj'

    FlashEmitterClass=class'XEffects.RocketMuzFlash1st'

    ProjSpawnOffset=(X=25,Y=6,Z=-6)

    FireSound=Sound'WeaponSounds.RocketLauncher.RocketLauncherFire'
    FireForce="RocketLauncherFire"  // jdf

    FireRate=0.9
    TweenTime=0.0

    bSplashDamage=true
    bRecommendSplashDamage=true
    bSplashJump=true
    BotRefireRate=0.5
	WarnTargetPct=+0.9

    ShakeOffsetMag=(X=-20.0,Y=0.00,Z=0.00)
    ShakeOffsetRate=(X=-1000.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

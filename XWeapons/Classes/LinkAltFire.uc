
class LinkAltFire extends ProjectileFire;

var sound LinkedFireSound;
var string LinkedFireForce;  // jdf

function DrawMuzzleFlash(Canvas Canvas)
{
    if (FlashEmitter != None)
    {
        FlashEmitter.SetRotation(Weapon.Rotation);
        Super.DrawMuzzleFlash(Canvas);
    }
}

simulated function bool AllowFire()
{
    return ( Weapon.AmmoAmount(ThisModeNum) >= 1 );
}

function Projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local LinkProjectile Proj;

    Start += Vector(Dir) * 10.0 * LinkGun(Weapon).Links;
    Proj = Weapon.Spawn(class'XWeapons.LinkProjectile',,, Start, Dir);
    if ( Proj != None )
    {
		Proj.Links = LinkGun(Weapon).Links;
		Proj.LinkAdjust();
	}
    return Proj;
}

function FlashMuzzleFlash()
{
    if (FlashEmitter != None)
    {
        if (LinkGun(Weapon).Links > 0)
            FlashEmitter.Skins[0] = FinalBlend'XEffectMat.LinkMuzProjYellowFB';
        else
            FlashEmitter.Skins[0] = FinalBlend'XEffectMat.LinkMuzProjGreenFB';
    }
    Super.FlashMuzzleFlash();
}

function ServerPlayFiring()
{
    if (LinkGun(Weapon).Links > 0)
        FireSound = LinkedFireSound;
    else
        FireSound = default.FireSound;
    Super.ServerPlayFiring();
}

function PlayFiring()
{
    if (LinkGun(Weapon).Links > 0)
        FireSound = LinkedFireSound;
    else
        FireSound = default.FireSound;
    Super.PlayFiring();
}

defaultproperties
{
    AmmoClass=class'LinkAmmo'
    AmmoPerFire=2

    FireAnim=Fire
    FireAnimRate=0.75
    FireLoopAnim=None
    FireEndAnim=None

    ProjectileClass=class'XWeapons.LinkProjectile'

    ProjSpawnOffset=(X=25,Y=8,Z=-3)

    FireSound=Sound'WeaponSounds.PulseRifle.PulseRifleFire'
    LinkedFireSound=Sound'WeaponSounds.LinkGun.BLinkedFire'
    FireForce="TranslocatorFire"  // jdf
    LinkedFireForce="BLinkedFire"  // jdf

    FlashEmitterClass=class'xEffects.LinkMuzFlashProj1st'

    FireRate=0.2

    bSplashDamage=false
    bRecommendSplashDamage=false
    BotRefireRate=0.99
	WarnTargetPct=+0.1

    ShakeOffsetMag=(X=0.0,Y=1.0,Z=0.0)
    ShakeOffsetRate=(X=0.0,Y=-2000.0,Z=0.0)
    ShakeOffsetTime=4
    ShakeRotMag=(X=40.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=2000.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

//=============================================================================
// FM_LinkTurret_Fire
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FM_LinkTurret_Fire extends ProjectileFire;

var sound	LinkedFireSound;
var string	LinkedFireForce;  // jdf

function DoFireEffect()
{
	local vector	Start, HL, HN;

	Instigator.MakeNoise(1.0);

	Start = ASVehicle(Instigator).GetFireStart();
	ASVehicle(Instigator).CalcWeaponFire( HL, HN );
	SpawnProjectile(Start, Rotator(HL-Start) );
}

function Projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local PROJ_LinkTurret_Plasma Proj;

    Start += Vector(Dir) * 10.0 * Weapon_LinkTurret(Weapon).Links;
    Proj = Weapon.Spawn(class'UT2k4AssaultFull.PROJ_LinkTurret_Plasma',,, Start, Dir);
    if ( Proj != None )
    {
		Proj.Links = Weapon_LinkTurret(Weapon).Links;
		Proj.LinkAdjust();
	}
    return Proj;
}

function ServerPlayFiring()
{
    if ( Weapon_LinkTurret(Weapon).Links > 0 )
        FireSound = LinkedFireSound;
    else
        FireSound = default.FireSound;

    super.ServerPlayFiring();
}

function PlayFiring()
{
    if ( Weapon_LinkTurret(Weapon).Links > 0 )
        FireSound = LinkedFireSound;
    else
        FireSound = default.FireSound;
    super.PlayFiring();
}

simulated function bool AllowFire()
{
    return true;
}

defaultproperties
{
    AmmoClass=class'Ammo_Dummy'
    AmmoPerFire=0

    FireAnim=Fire
    FireAnimRate=1.0
    FireLoopAnim=None
    FireEndAnim=None

    FireSound=Sound'WeaponSounds.PulseRifle.PulseRifleFire'
    LinkedFireSound=Sound'WeaponSounds.LinkGun.BLinkedFire'
    FireForce="TranslocatorFire"  // jdf
    LinkedFireForce="BLinkedFire"  // jdf
  
    FireRate=0.35

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

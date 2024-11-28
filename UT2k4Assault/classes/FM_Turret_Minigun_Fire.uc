//=============================================================================
// FM_Turret_Minigun_Fire
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FM_Turret_Minigun_Fire extends InstantFire;

function DoFireEffect()
{
    local Vector	StartTrace, HL, HN;
    local Rotator	R;
	local Actor		HitActor;

	if ( Instigator == None )
		return;

    Instigator.MakeNoise(1.0);

    StartTrace	= ASVehicle(Instigator).GetFireStart();
	HitActor	= ASVehicle(Instigator).CalcWeaponFire( HL, HN );
    R = Rotator(Normal(HL-StartTrace) + VRand()*FRand()*Spread);
	DoTrace(StartTrace, R);
}

simulated function bool AllowFire()
{
    return true;
}

//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
    AmmoClass=class'Ammo_Dummy'
    AmmoPerFire=0
	DamageType=class'DamTypeMinigunTurretBullet'

    FireAnim=Fire
    FireLoopAnim=None
    FireEndAnim=None

    FireForce="TranslocatorFire"  // jdf

    FlashEmitterClass=None

    TraceRange=11000
    DamageMin=20
    DamageMax=25
    FireRate=0.15
    SpreadStyle=SS_Random
    Spread=0.015

    bSplashDamage=false
    bRecommendSplashDamage=false
    BotRefireRate=0.99
    WarnTargetPct=+0.15

    ShakeOffsetMag=(X=0.0,Y=1.0,Z=0.0)
    ShakeOffsetRate=(X=0.0,Y=-2000.0,Z=0.0)
    ShakeOffsetTime=4
    ShakeRotMag=(X=40.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=2000.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

//=============================================================================
// FM_LinkTurret_AltFire
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FM_LinkTurret_AltFire extends LinkFire;

var	float VehicleDamageMult;

function PlayFiring()
{
	if ( LinkGun(Weapon).Links <= 0 )
		ClientPlayForceFeedback("BLinkGunBeam1");

	super(WeaponFire).PlayFiring();
}

simulated function vector GetFireStart(vector X, vector Y, vector Z)
{
    return ASVehicle(Instigator).GetFireStart();
}

simulated function bool AllowFire()
{
    return true;
}

simulated function bool myHasAmmo( LinkGun LinkGun )
{
	return true;
}

simulated function Rotator	GetPlayerAim( vector StartTrace, float InAimError )
{
	local vector HL, HN;
	ASVehicle(Instigator).CalcWeaponFire( HL, HN );
	return Rotator( HL - StartTrace );
}

simulated function float AdjustLinkDamage( LinkGun LinkGun, Actor Other, float Damage )
{
	Damage = Damage * (1.5*Linkgun.Links+1);

	if ( Other.IsA('Vehicle') )
		Damage *= VehicleDamageMult;

	return Damage;
}

defaultproperties
{
	TraceRange=5000;
	BeamEffectClass=class'FX_LinkTurret_BeamEffect'
	DamageType=class'DamTypeLinkTurretBeam'
	AmmoClass=class'Ammo_Dummy'
	AmmoPerFire=0
	Damage=15
	VehicleDamageMult=2.5
	FireAnim=Fire
	FireEndAnim=None
	FlashEmitterClass=class'xEffects.LinkMuzFlashBeam1st'
}

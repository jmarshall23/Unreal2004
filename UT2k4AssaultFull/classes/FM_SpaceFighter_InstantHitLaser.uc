//=============================================================================
// FM_SpaceFighter_InstantHitLaser
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FM_SpaceFighter_InstantHitLaser extends InstantFire;

var	bool				bSwitch;
var name				FireAnimLeft, FireAnimRight;
var	Sound				FireSounds[2];


event ModeDoFire()
{
	bSwitch = ( (WeaponAttachment(Weapon.ThirdPersonActor).FlashCount % 2) == 1 );
	super.ModeDoFire();
}

function DoFireEffect()
{
    local Vector	StartTrace, HL, HN;
    local Rotator	R;

	if ( Instigator == None || !Instigator.IsA('ASVehicle') )
	{
		super.DoFireEffect();
		return;
	}

	if ( ASVehicle_SpaceFighter_Skaarj(Instigator) != None )	// hack for Skaarj SF damagetype
		DamageType = class'DamTypeSpaceFighterLaser_Skaarj';

    Instigator.MakeNoise(1.0);

    StartTrace	= ASVehicle(Instigator).GetFireStart();
	ASVehicle(Instigator).CalcWeaponFire( HL, HN );

    R = Rotator( Normal(HL-StartTrace) );

	DoTrace(StartTrace, R);
}

function PlayFiring()
{
	if ( Weapon.Mesh != None )
	{
		if ( bSwitch && Weapon.HasAnim(FireAnimRight) )
			FireAnim = FireAnimRight;
		else if ( !bSwitch && Weapon.HasAnim(FireAnimLeft) )
			FireAnim = FireAnimLeft;
	}

	UpdateFireSound();

	super.PlayFiring();
}

simulated function UpdateFireSound()
{
	if ( Instigator.IsA('ASTurret') || Instigator.IsA('ASVehicle_SpaceFighter_Skaarj') )
		FireSound = FireSounds[0];
	else
		FireSound = FireSounds[1];
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
	TraceRange=65536
    AmmoClass=class'Ammo_Dummy'
    AmmoPerFire=0
	AimError=800
	
	DamageType=class'DamTypeSpaceFighterLaser'

	FireAnimLeft=FireL
	FireAnimRight=FireR
    FireAnim=Fire
    FireLoopAnim=None
    FireEndAnim=None
	TweenTime=0.0
	FireAnimRate=0.45

	FireSounds[0]=Sound'ONSVehicleSounds-S.Laser02'
	FireSounds[1]=Sound'AssaultSounds.HnShipFire01'
    FireSound=Sound'WeaponSounds.PulseRifle.PulseRifleFire'
    FireForce="TranslocatorFire"  // jdf

    //FlashEmitterClass=class'xEffects.LinkMuzFlashProj1st'
    FlashEmitterClass=None

    DamageMin=30
    DamageMax=40
    FireRate=0.2
    SpreadStyle=SS_None
    Spread=0.0

    bSplashDamage=false
    bRecommendSplashDamage=false
    BotRefireRate=0.99
	WarnTargetPct=+0.2

    ShakeOffsetMag=(X=0.0,Y=1.0,Z=0.0)
    ShakeOffsetRate=(X=0.0,Y=-2000.0,Z=0.0)
    ShakeOffsetTime=4
    ShakeRotMag=(X=40.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=2000.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

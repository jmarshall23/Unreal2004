//=============================================================================
// PROJ_TurretSkaarjPlasma
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class PROJ_TurretSkaarjPlasma extends Projectile;

var Emitter			FX;
var	class<Emitter>	PlasmaClass;
var FX_PlasmaImpact	FX_Impact;


simulated function PostBeginPlay()
{
	super.PostBeginPlay();

	Velocity		= Speed * Vector(Rotation);

	if ( Level.NetMode != NM_DedicatedServer )
		SetupProjectile();
}

simulated function PostNetBeginPlay()
{
    Super.PostNetBeginPlay();

    Acceleration	= Velocity * 5;
}

simulated function Destroyed()
{
    if ( FX != None )
        FX.Destroy();

	super.Destroyed();
}

simulated function SetupProjectile()
{
	FX = Spawn(PlasmaClass, Self,, Location, Rotation);

	if ( FX != None )
		FX.SetBase( Self );
}

simulated function Explode(vector HitLocation, vector HitNormal)
{
	if ( EffectIsRelevant(Location, false) )
		SpawnExplodeFX(HitLocation, HitNormal);

	PlaySound(Sound'WeaponSounds.BioRifle.BioRifleGoo2');
	Destroy();
}

simulated function SpawnExplodeFX(vector HitLocation, vector HitNormal)
{
	FX_Impact = Spawn(class'FX_PlasmaImpact',,, HitLocation + HitNormal * 2, rotator(HitNormal));
}

simulated function ProcessTouch (Actor Other, vector HitLocation)
{
	if ( Instigator != None && (Other == Instigator) )
		return;

    if (Other == Owner) return;

	if ( !Other.IsA('Projectile') || Other.bProjTarget )
	{
		if ( Role == ROLE_Authority )
		{
			if ( Instigator == None || Instigator.Controller == None )
				Other.SetDelayedDamageInstigatorController( InstigatorController );

			Other.TakeDamage(Damage, Instigator, HitLocation, MomentumTransfer * Normal(Velocity), MyDamageType);
		}

		Explode(HitLocation, -Normal(Velocity));
	}
}


//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
	PlasmaClass=class'FX_SpaceFighter_SkaarjPlasma'

	MyDamageType=class'DamTypeBallTurretPlasma'

	Damage=45

    Speed=8000
    MaxSpeed=100000
    MomentumTransfer=5000

	CollisionRadius=0.0
    CollisionHeight=0.0

	ExploWallOut=0
    LifeSpan=1.4

	DrawType=DT_None

	AmbientSound=Sound'WeaponSounds.ShockRifle.LinkGunProjectile'
    SoundRadius=50
    SoundVolume=255

	ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=30.0
}

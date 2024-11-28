//=============================================================================
// PROJ_LinkTurret_Plasma
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class PROJ_LinkTurret_Plasma extends Projectile;

var FX_LinkTurret_GreenPlasma	Plasma;
var float						VehicleDamageMult;
var int							Links;

replication
{
    unreliable if (bNetInitial && Role == ROLE_Authority)
        Links;
}

simulated function Destroyed()
{
    if ( Plasma != None )
        Plasma.Destroy();

	super.Destroyed();
}

simulated function PostBeginPlay()
{
	local vector dir;

	Dir = Vector(Rotation);
	super.PostBeginPlay();

	if ( Level.NetMode != NM_DedicatedServer )
		Plasma = Spawn(class'FX_LinkTurret_GreenPlasma', Self,, Location - 50*Dir, Rotation);

	if ( Plasma != None )
		Plasma.SetBase( Self );

	Velocity		 = Speed * Vector(Rotation);
}

simulated function LinkAdjust()
{
    local float ls;

    if ( Links > 0 )
    {
        ls = class'LinkFire'.default.LinkScale[ Min(Links, 5) ];

		if ( Plasma != None )
		{
			Plasma.SetSize( 1.f + ls );
			Plasma.SetYellowColor();
		}

        MaxSpeed	= default.MaxSpeed + 350*Links;
        LightHue = 40;
    }
}

simulated function PostNetBeginPlay()
{
    Acceleration	 = Velocity;

    if ( Role < ROLE_Authority )
        LinkAdjust();

    if ( (Level.NetMode != NM_DedicatedServer) && (Level.bDropDetail || (Level.DetailMode == DM_Low)) )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
}

simulated function Explode(vector HitLocation, vector HitNormal)
{
	local FX_PlasmaImpact	FX_Impact;

	// Give a little splash
	if ( Role == Role_Authority )
		HurtRadius(Damage * 0.5 * (1.0 + float(Links)),DamageRadius, MyDamageType, MomentumTransfer * (1.0 + float(Links)*0.15) ,HitLocation);

    if ( EffectIsRelevant(Location, false) )
	{
        if ( Links == 0 )
		{
            FX_Impact = Spawn(class'FX_PlasmaImpact',,, HitLocation + HitNormal * 2, rotator(HitNormal));
			FX_Impact.SetGreenColor();
		}
        else
		{
            FX_Impact = Spawn(class'FX_PlasmaImpact',,, HitLocation + HitNormal * 2, rotator(HitNormal));
			FX_Impact.SetYellowColor();
		}
	}

    PlaySound(Sound'WeaponSounds.BioRifle.BioRifleGoo2');
	Destroy();
}

/* HurtRadius()
 Hurt locally authoritative actors within the radius.
*/
simulated function HurtRadius( float DamageAmount, float DamageRadius, class<DamageType> DamageType, float Momentum, vector HitLocation )
{
	local actor Victims;
	local float damageScale, dist;
	local vector dir;

	if( bHurtEntry )
		return;

	bHurtEntry = true;
	foreach VisibleCollidingActors( class 'Actor', Victims, DamageRadius, HitLocation )
	{
		// don't let blast damage affect fluid - VisibleCollisingActors doesn't really work for them - jag
		if( (Victims != self) && (Hurtwall != Victims) && (Victims.Role == ROLE_Authority) && !Victims.IsA('FluidSurfaceInfo') )
		{
			dir = Victims.Location - HitLocation;
			dist = FMax(1,VSize(dir));
			dir = dir/dist;
			damageScale = 1 - FMax(0,(dist - Victims.CollisionRadius)/DamageRadius);
			if ( Victims.IsA('Vehicle') )
				damageScale *= VehicleDamageMult;

			if ( DamageType.Default.bDelayedDamage && (Instigator == None || Instigator.Controller == None) )
				Victims.SetDelayedDamageInstigatorController(InstigatorController);
			if ( Victims == LastTouched )
				LastTouched = None;
			Victims.TakeDamage
			(
				damageScale * DamageAmount,
				Instigator,
				Victims.Location - 0.5 * (Victims.CollisionHeight + Victims.CollisionRadius) * dir,
				(damageScale * Momentum * dir),
				DamageType
			);
			if (Vehicle(Victims) != None && Vehicle(Victims).Health > 0)
				Vehicle(Victims).DriverRadiusDamage(DamageAmount, DamageRadius, InstigatorController, DamageType, Momentum, HitLocation);
		}
	}
	if ( (LastTouched != None) && (LastTouched != self) && (LastTouched.Role == ROLE_Authority) && !LastTouched.IsA('FluidSurfaceInfo') )
	{
		Victims = LastTouched;
		LastTouched = None;
		dir = Victims.Location - HitLocation;
		dist = FMax(1,VSize(dir));
		dir = dir/dist;
		damageScale = FMax(Victims.CollisionRadius/(Victims.CollisionRadius + Victims.CollisionHeight),1 - FMax(0,(dist - Victims.CollisionRadius)/DamageRadius));
		if ( DamageType.Default.bDelayedDamage && (Instigator == None || Instigator.Controller == None) )
			Victims.SetDelayedDamageInstigatorController(InstigatorController);
		Victims.TakeDamage
		(
			damageScale * DamageAmount,
			Instigator,
			Victims.Location - 0.5 * (Victims.CollisionHeight + Victims.CollisionRadius) * dir,
			(damageScale * Momentum * dir),
			DamageType
		);
		if (Vehicle(Victims) != None && Vehicle(Victims).Health > 0)
			Vehicle(Victims).DriverRadiusDamage(DamageAmount, DamageRadius, InstigatorController, DamageType, Momentum, HitLocation);
	}

	bHurtEntry = false;
}

simulated function ProcessTouch (Actor Other, vector HitLocation)
{
	local float		AdjustedDamage;

	if (Other == Instigator) return;
    if (Other == Owner) return;

	if ( !Other.IsA('Projectile') || Other.bProjTarget )
	{
		if ( Role == ROLE_Authority )
		{
			AdjustedDamage = Damage * (1.0 + float(Links));

			if ( Other.IsA('Vehicle') )
				AdjustedDamage *= VehicleDamageMult;

			if ( Instigator == None || Instigator.Controller == None )
				Other.SetDelayedDamageInstigatorController( InstigatorController );

			Other.TakeDamage(AdjustedDamage,Instigator,HitLocation,MomentumTransfer * Normal(Velocity),MyDamageType);
		}

		Explode(HitLocation, -Normal(Velocity) );
	}
}

defaultproperties
{
    Damage=75
    DamageRadius=300
	VehicleDamageMult=4.0
    MyDamageType=class'DamTypeLinkTurretPlasma'

	Speed=5000
    MaxSpeed=5000
    MomentumTransfer=5000

	ExploWallOut=0
    LifeSpan=4

	bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightRadius=3
    LightBrightness=255
    LightHue=100
    LightSaturation=100

	DrawType=DT_None
    DrawScale3D=(X=2.295,Y=1.53,Z=1.53)

	AmbientSound=Sound'WeaponSounds.ShockRifle.LinkGunProjectile'
    SoundRadius=50
    SoundVolume=255

    ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=30.0
    FluidSurfaceShootStrengthMod=6.f
    MaxEffectDistance=7000.0
}

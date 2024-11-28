//=============================================================================
// FM_SpaceFighter_AltFire
//=============================================================================

class FM_SpaceFighter_AltFire extends ProjectileFire;

function DoFireEffect()
{
	// Get Rocket spawn location
	if ( ASVehicle_SpaceFighter(Instigator) != None )
		ProjSpawnOffset = ASVehicle_SpaceFighter(Instigator).GetRocketSpawnLocation();

	MyDoFireEffect();
}

function MyDoFireEffect()
{
	local Vector Start, X,Y,Z;

	Instigator.MakeNoise(1.0);
	GetAxes(Instigator.Rotation, X, Y, Z);

	Start = MyGetFireStart(X, Y, Z);

	SpawnProjectile(Start, Instigator.Rotation);
}

function Projectile SpawnProjectile(vector Start, rotator Dir)
{
	local PROJ_SpaceFighter_Rocket	P;
	local Vehicle					Target;

	P = PROJ_SpaceFighter_Rocket(super.SpawnProjectile(Start, Dir));

	if ( P != None && ASVehicle_SpaceFighter_Skaarj(Instigator) != None )	// hack for Skaarj SF damagetype
		P.MyDamageType = class'DamTypeSpaceFighterMissileSkaarj';

	if ( P != None && ASVehicle_SpaceFighter(Instigator) != None )
	{
		if ( Instigator.Controller.IsA('PlayerController') )
			Target = ASVehicle_SpaceFighter(Instigator).CurrentTarget;
		else if ( Instigator.Controller.IsA('Bot') && Instigator.Controller.Enemy != None 
			&& (Bot(Instigator.Controller).Skill > FRand() * 5.f) )
		{
			// very basic AI to target enemies...
			Target = Vehicle(Instigator.Controller.Enemy);
		}
	}

	// Check that Target is directly visible
	if ( Target != None && Normal(Target.Location - Instigator.Location) Dot Vector(Instigator.Rotation) > 0 
		&& Weapon.FastTrace( Target.Location, Instigator.Location ) )
	{
		P.HomingTarget = Target;
		Target.NotifyEnemyLockedOn();
	}

	return P;
}

simulated function vector MyGetFireStart(vector X, vector Y, vector Z)
{
    return Instigator.Location + X*ProjSpawnOffset.X + Y*ProjSpawnOffset.Y + Z*ProjSpawnOffset.Z;
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
	bModeExclusive=false
    AmmoClass=class'Ammo_Dummy'
    AmmoPerFire=0

    FireAnim=Fire
    FireAnimRate=1.0

    ProjectileClass=class'UT2k4AssaultFull.PROJ_SpaceFighter_Rocket'
    FlashEmitterClass=None
    ProjSpawnOffset=(X=0,Y=0,Z=-25)

    FireSound=Sound'AssaultSounds.HnShipFire02'
    FireForce="RocketLauncherFire"  // jdf

    FireRate=4.0
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

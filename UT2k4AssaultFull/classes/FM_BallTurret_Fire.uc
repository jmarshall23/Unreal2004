//=============================================================================
// FM_BallTurret_Fire
//=============================================================================

class FM_BallTurret_Fire extends ProjectileFire;

#exec OBJ LOAD FILE=..\Sounds\ONSVehicleSounds-S.uax

var	class<Projectile>	TeamProjectileClasses[2];
var	bool				bSwitch;
var name				FireAnimLeft, FireAnimRight;

event ModeDoFire()
{
	if ( Weapon.ThirdPersonActor != None )
		bSwitch = ( (WeaponAttachment(Weapon.ThirdPersonActor).FlashCount % 2) == 1 );
	else
		bSwitch = !bSwitch;

	super.ModeDoFire();
}

function DoFireEffect()
{
	local Vector	ProjOffset;
	local Vector	Start, X,Y,Z, HL, HN;

	if ( Instigator.IsA('ASVehicle') )
		ProjOffset = ASVehicle(Instigator).VehicleProjSpawnOffset;

	ProjSpawnOffset = ProjOffset;
	if ( bSwitch )
		ProjSpawnOffset.Y = -ProjSpawnOffset.Y;

	Instigator.MakeNoise(1.0);
    Instigator.GetAxes(Instigator.Rotation, X, Y, Z);

	Start = MyGetFireStart(X, Y, Z);

	ASVehicle(Instigator).CalcWeaponFire( HL, HN );
	SpawnProjectile(Start, Rotator(HL - Start));
}

simulated function vector MyGetFireStart(vector X, vector Y, vector Z)
{
    return Instigator.Location + X*ProjSpawnOffset.X + Y*ProjSpawnOffset.Y + Z*ProjSpawnOffset.Z;
}


function projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local Projectile p;

	p = Weapon.Spawn(TeamProjectileClasses[Instigator.GetTeamNum()], Instigator, , Start, Dir);
    if ( p == None )
        return None;

    p.Damage *= DamageAtten;
    return p;
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

	super.PlayFiring();
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

	FireAnimLeft=FireL
	FireAnimRight=FireR
    FireAnim=Fire
    FireLoopAnim=None
    FireEndAnim=None
	TweenTime=0.0
	FireAnimRate=0.45

	TeamProjectileClasses[0]=class'UT2k4AssaultFull.PROJ_TurretSkaarjPlasma_Red'
	TeamProjectileClasses[1]=class'UT2k4AssaultFull.PROJ_TurretSkaarjPlasma'
    ProjSpawnOffset=(X=200,Y=14,Z=-14)

    FireSound=Sound'ONSVehicleSounds-S.Laser02'
    FireForce="TranslocatorFire"  // jdf

    FlashEmitterClass=None

    FireRate=0.25

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

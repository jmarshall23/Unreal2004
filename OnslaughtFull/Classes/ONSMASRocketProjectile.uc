//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSMASRocketProjectile extends Projectile;

#exec OBJ LOAD FILE=..\Sounds\VMVehicleSounds-S.uax

var Actor HomingTarget;
var vector InitialDir;

var Emitter SmokeTrailEffect;
var Effects Corona;

replication
{
    reliable if (bNetInitial && Role == ROLE_Authority)
        InitialDir, HomingTarget;
}

simulated function Destroyed()
{
	if ( SmokeTrailEffect != None )
		SmokeTrailEffect.Kill();
	if ( Corona != None )
		Corona.Destroy();
	Super.Destroyed();
}

simulated function PostBeginPlay()
{
	if (Level.NetMode != NM_DedicatedServer)
	{
		SmokeTrailEffect = Spawn(class'ONSMASRocketTrailEffect',self);
		Corona = Spawn(class'RocketCorona',self);
		Corona.SetDrawScale(4.5 * Corona.default.DrawScale);
	}

	InitialDir = vector(Rotation);
	Velocity = InitialDir * Speed;

	if (PhysicsVolume.bWaterVolume)
		Velocity = 0.6 * Velocity;

	SetTimer(0.1, true);

	Super.PostBeginPlay();
}

simulated function Timer()
{
	local float VelMag;
	local vector ForceDir;

	if (HomingTarget == None)
		return;

	ForceDir = Normal(HomingTarget.Location - Location);
	if (ForceDir dot InitialDir > 0)
	{
	    	// Do normal guidance to target.
	    	VelMag = VSize(Velocity);

	    	ForceDir = Normal(ForceDir * 0.75 * VelMag + Velocity);
		Velocity =  VelMag * ForceDir;
    		Acceleration = 5 * ForceDir;

	    	// Update rocket so it faces in the direction its going.
		SetRotation(rotator(Velocity));
	}
}

simulated function Landed( vector HitNormal )
{
	Explode(Location,HitNormal);
}

simulated function ProcessTouch (Actor Other, Vector HitLocation)
{
	if ( (Other != instigator) && (!Other.IsA('Projectile') || Other.bProjTarget) )
	{
		Explode(HitLocation, vect(0,0,1));
	}
}

function BlowUp(vector HitLocation)
{
	HurtRadius(Damage, DamageRadius, MyDamageType, MomentumTransfer, HitLocation );
	MakeNoise(1.0);
}

simulated function Explode(vector HitLocation, vector HitNormal)
{
	local PlayerController PC;

	PlaySound(sound'WeaponSounds.BExplosion3',,2.5*TransientSoundVolume);

    if ( EffectIsRelevant(Location,false) )
    {
    	Spawn(class'NewExplosionA',,,HitLocation + HitNormal*20,rotator(HitNormal));
    	PC = Level.GetLocalPlayerController();
		if ( (PC.ViewTarget != None) && VSize(PC.ViewTarget.Location - Location) < 5000 )
	        Spawn(class'ExplosionCrap',,, HitLocation + HitNormal*20, rotator(HitNormal));
    }

	BlowUp(HitLocation);
	Destroy();
}

defaultproperties
{
    Speed=2500.0
    MaxSpeed=2500.0
    MomentumTransfer=10000
    Damage=50.0
    DamageRadius=250.0
    AmbientSound=sound'VMVehicleSounds-S.HoverTank.IncomingShell'
    SoundVolume=255
    MyDamageType=class'DamTypeRocket'
    ExplosionDecal=class'ONSRocketScorch'
    RemoteRole=ROLE_SimulatedProxy
    LifeSpan=7.0
    DrawType=DT_StaticMesh
    DrawScale3D=(Y=0.4,Z=0.4)
    StaticMesh=StaticMesh'WeaponStaticMesh.RocketProj'
    AmbientGlow=96
    bUnlit=True
    bBounce=False
    bFixedRotationDir=True
    RotationRate=(Roll=50000)
    DesiredRotation=(Roll=900000)
    ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=100.0
    bCollideWorld=True
    FluidSurfaceShootStrengthMod=10.0
    bNetTemporary=True
    LightType=LT_None
    bDynamicLight=false
}

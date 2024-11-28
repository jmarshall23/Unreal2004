//=============================================================================
// flakshell
//=============================================================================
class flakshell extends Projectile;

#exec TEXTURE  IMPORT NAME=NewFlakSkin FILE=textures\jflakslugel1.bmp GROUP="Skins" DXT=5

var	xemitter trail;
var vector initialDir;
var actor Glow;

simulated function PostBeginPlay()
{
	local Rotator R;
	local PlayerController PC;
	
	if ( !PhysicsVolume.bWaterVolume && (Level.NetMode != NM_DedicatedServer) )
	{
		PC = Level.GetLocalPlayerController();
		if ( (PC.ViewTarget != None) && VSize(PC.ViewTarget.Location - Location) < 6000 )
			Trail = Spawn(class'FlakShellTrail',self);
		Glow = Spawn(class'FlakGlow', self);
	}

	Super.PostBeginPlay();
	Velocity = Vector(Rotation) * Speed;  
	R = Rotation;
	R.Roll = 32768;
	SetRotation(R);
	Velocity.z += TossZ; 
	initialDir = Velocity;
}

simulated function destroyed()
{
	if ( Trail != None ) 
		Trail.mRegen=False;
	if ( glow != None )
		Glow.Destroy();
	Super.Destroyed();
}


simulated function ProcessTouch(Actor Other, Vector HitLocation)
{
	if ( Other != Instigator )
	{
		SpawnEffects(HitLocation, -1 * Normal(Velocity) );
		Explode(HitLocation,Normal(HitLocation-Other.Location));
	}
}

simulated function SpawnEffects( vector HitLocation, vector HitNormal )
{
	local PlayerController PC;

	PlaySound (Sound'WeaponSounds.BExplosion1',,3*TransientSoundVolume);
	if ( EffectIsRelevant(Location,false) )
	{
		PC = Level.GetLocalPlayerController();
		if ( (PC.ViewTarget != None) && VSize(PC.ViewTarget.Location - Location) < 3000 )
			spawn(class'FlakExplosion',,,HitLocation + HitNormal*16 );
		spawn(class'FlashExplosion',,,HitLocation + HitNormal*16 );
		spawn(class'RocketSmokeRing',,,HitLocation + HitNormal*16, rotator(HitNormal) );
		if ( (ExplosionDecal != None) && (Level.NetMode != NM_DedicatedServer) )
			Spawn(ExplosionDecal,self,,HitLocation, rotator(-HitNormal));
	}
}

simulated function Landed( vector HitNormal )
{
	SpawnEffects( Location, HitNormal );
	Explode(Location,HitNormal);
}

simulated function HitWall (vector HitNormal, actor Wall)
{
	Landed(HitNormal);
}

simulated function Explode(vector HitLocation, vector HitNormal)
{
	local vector start;
    local rotator rot;
    local int i;
    local FlakChunk NewChunk;

	start = Location + 10 * HitNormal;
	if ( Role == ROLE_Authority )
	{
		HurtRadius(damage, 220, MyDamageType, MomentumTransfer, HitLocation);	
		for (i=0; i<6; i++)
		{
			rot = Rotation;
			rot.yaw += FRand()*32000-16000;
			rot.pitch += FRand()*32000-16000;
			rot.roll += FRand()*32000-16000;
			NewChunk = Spawn( class 'FlakChunk',, '', Start, rot);
		}
	}
    Destroy();
}

defaultproperties
{
    ExplosionDecal=class'ShockAltDecal'
	TossZ=+225.0
	bProjTarget=True
    speed=1200.000000
    Damage=90.000000
    MomentumTransfer=75000
    bNetTemporary=True
    Physics=PHYS_Falling
    MyDamageType=class'DamTypeFlakShell'
    LifeSpan=6.000000
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'WeaponStaticMesh.FlakShell'
    Skins(0)=texture'NewFlakSkin'
    DrawScale=8.0
    AmbientGlow=100
    AmbientSound=Sound'WeaponSounds.BaseProjectileSounds.BFlakCannonProjectile'
    SoundRadius=100
    SoundVolume=255
    ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=60.0
    CullDistance=+4000.0
}

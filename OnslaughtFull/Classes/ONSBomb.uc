//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSBomb extends Projectile;

var xEmitter Trail;

simulated function Destroyed()
{
    if ( Trail != None )
        Trail.mRegen = false; // stop the emitter from regenerating
	Super.Destroyed();
}

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();

    if ( Level.NetMode != NM_DedicatedServer)
    {
        Trail = Spawn(class'GrenadeSmokeTrail', self,, Location, Rotation);
    }

    if ( Role == ROLE_Authority )
    {
        RandSpin(25000);
    }
}

simulated function Landed( vector HitNormal )
{
    HitWall( HitNormal, None );
}

simulated function HitWall( vector HitNormal, actor Wall )
{
    Explode(Location, vect(0,0,1));
}

simulated function Explode(vector HitLocation, vector HitNormal)
{
    BlowUp(HitLocation);
	PlaySound(sound'WeaponSounds.BExplosion3',,2.5*TransientSoundVolume);
    if ( EffectIsRelevant(Location,false) )
    {
        Spawn(class'ONSBombDropExplosion',,, HitLocation, rotator(vect(0,0,1)));
		Spawn(ExplosionDecal,self,,HitLocation, rotator(-HitNormal));
    }
    Destroy();
}

defaultproperties
{
	TossZ=+0.0
    ExplosionDecal=class'RocketMark'
    MyDamageType=class'DamTypeAssaultGrenade'
    Damage=70
    DamageRadius=600.0
    MomentumTransfer=75000
    ImpactSound=Sound'WeaponSounds.P1GrenFloor1'
    Physics=PHYS_Falling
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'WeaponStaticMesh.GrenadeMesh'
    DrawScale=8.0
    AmbientGlow=100
    bFixedRotationDir=True
    DesiredRotation=(Pitch=12000,Yaw=5666,Roll=2334)
}

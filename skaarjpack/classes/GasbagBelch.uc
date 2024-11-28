//=============================================================================
// rocket.
//=============================================================================
class GasBagBelch extends Projectile;

var	xEmitter SmokeTrail;
var vector Dir;

simulated function Destroyed() 
{
	if ( SmokeTrail != None )
		SmokeTrail.mRegen = False;
	Super.Destroyed();
}

simulated function PostBeginPlay()
{
	if ( Level.NetMode != NM_DedicatedServer)
	{
		if ( !Level.bDropDetail )
			spawn(class'RocketSmokeRing',,,Location, Rotation );
		SmokeTrail = Spawn(class'BelchFlames',self);
	}
	Dir = vector(Rotation);
	Velocity = speed * Dir;
    if ( Level.bDropDetail )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
	Super.PostBeginPlay();
}

simulated function Landed( vector HitNormal )
{
	Explode(Location,HitNormal);
}

simulated function ProcessTouch (Actor Other, Vector HitLocation)
{
	if ( (Other != instigator) && (!Other.IsA('Projectile') || Other.bProjTarget) ) 
		Explode(HitLocation,Vect(0,0,1));
}

function BlowUp(vector HitLocation)
{
	HurtRadius(Damage, DamageRadius, MyDamageType, MomentumTransfer, HitLocation );
	MakeNoise(1.0);
}

simulated function Explode(vector HitLocation, vector HitNormal) 
{
	PlaySound(sound'WeaponSounds.BExplosion3',,2.5*TransientSoundVolume);
	spawn(class'FlakExplosion',,,HitLocation + HitNormal*16 );
	spawn(class'FlashExplosion',,,HitLocation + HitNormal*16 );
	if ( (ExplosionDecal != None) && (Level.NetMode != NM_DedicatedServer) )
		Spawn(ExplosionDecal,self,,Location, rotator(-HitNormal));
 	
	BlowUp(HitLocation);
	Destroy(); 
}


defaultproperties
{
	bHidden=true
    speed=650.0
    MaxSpeed=650.0
    Damage=45.0
    DamageRadius=140.0  
    MomentumTransfer=50000
    MyDamageType=class'DamTypeBelch'
    ExplosionDecal=class'RocketMark'
    RemoteRole=ROLE_SimulatedProxy
    LifeSpan=6.0
    AmbientSound=Sound'WeaponSounds.RocketLauncher.RocketLauncherProjectile'
    SoundVolume=255
    SoundRadius=100
    DrawType=DT_Sprite
    Style=STY_Translucent
    Texture=Texture'XEffectMat.link_muz_red'
     DrawScale=0.3
    AmbientGlow=96
    bUnlit=True
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightBrightness=255
    LightHue=28
    LightRadius=5
    bDynamicLight=true
    bBounce=false
    bFixedRotationDir=True
    RotationRate=(Roll=50000)
    DesiredRotation=(Roll=30000)
    ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=100.0
    bCollideWorld=true
}

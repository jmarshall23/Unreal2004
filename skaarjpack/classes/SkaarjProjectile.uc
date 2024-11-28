class SkaarjProjectile extends Projectile; 

var xEmitter SparkleTrail;

simulated function PostBeginPlay()
{
	Super.PostBeginPlay();

    if ( Level.NetMode != NM_DedicatedServer )
    {
		SparkleTrail = Spawn(class'SkaarjSparkles', self);
		SparkleTrail.Skins[0] = Texture;
	}

	Velocity = Speed * Vector(Rotation); 
}

simulated function Destroyed()
{
    if (SparkleTrail != None)
    {
        SparkleTrail.mStartParticles = 12;
        SparkleTrail.mLifeRange[0] *= 2.0;
        SparkleTrail.mLifeRange[1] *= 2.0;
        SparkleTrail.mRegen = false;
    }
	Super.Destroyed();
}

simulated function DestroyTrails()
{
    if (SparkleTrail != None)
        SparkleTrail.Destroy();
}

simulated function ProcessTouch (Actor Other, vector HitLocation)
{
    local Vector X, RefNormal, RefDir;

	if (Other == Instigator) return;
    if (Other == Owner) return;

    if (Other.IsA('xPawn') && xPawn(Other).CheckReflect(HitLocation, RefNormal, Damage*0.25))
    {
        if (Role == ROLE_Authority)
        {
            X = Normal(Velocity);
            RefDir = X - 2.0*RefNormal*(X dot RefNormal);
            RefDir = RefNormal;
            Spawn(Class, Other,, HitLocation+RefDir*20, Rotator(RefDir));
        }
        DestroyTrails();
        Destroy();
    }
    else if ( !Other.IsA('Projectile') || Other.bProjTarget )
    {
		Explode(HitLocation, Normal(HitLocation-Other.Location));
    }
}

simulated function Explode(vector HitLocation,vector HitNormal)
{
    if ( Role == ROLE_Authority )
        HurtRadius(Damage, DamageRadius, MyDamageType, MomentumTransfer, HitLocation );

   	PlaySound(ImpactSound, SLOT_Misc);
	Destroy();
}

defaultproperties
{
    ExplosionDecal=class'LinkBoltScorch' 
    Speed=1000
    MaxSpeed=1000
    Damage=30
    DamageRadius=150
    MomentumTransfer=70000
    MyDamageType=class'DamTypeShockBall'
    LifeSpan=10.0
    DrawType=DT_Sprite
    Skins(0)=Texture'link_muz_green'
    Texture=Texture'link_muz_green'
    Style=STY_Translucent
    DrawScale=0.2
    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightBrightness=255
    LightHue=100
    LightSaturation=85
    LightRadius=4
    AmbientSound=Sound'WeaponSounds.ShockRifle.ShockRifleProjectile'
    SoundRadius=60
    SoundVolume=255
    ImpactSound=Sound'WeaponSounds.ShockRifle.ShockRifleExplosion'
}

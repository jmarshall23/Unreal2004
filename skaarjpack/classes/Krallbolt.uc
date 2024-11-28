class KrallBolt extends Projectile;

var xEmitter Trail;
var texture TrailTex;

simulated function Destroyed()
{
    if (Trail != None)
        Trail.Destroy();
	Super.Destroyed();
}

simulated function PostBeginPlay()
{
    local Rotator R;

	Super.PostBeginPlay();

    if ( EffectIsRelevant(vect(0,0,0),false) )
    {
		Trail = Spawn(class'LinkProjEffect',self);
		if ( Trail != None ) 
			Trail.Skins[0] = TrailTex;
	}
	
	Velocity = Speed * Vector(Rotation);

    R = Rotation;
    R.Roll = Rand(65536);
    SetRotation(R);
    
	if ( Level.bDropDetail || Level.DetailMode == DM_Low )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
} 

simulated function Explode(vector HitLocation, vector HitNormal)
{
    if ( EffectIsRelevant(Location,false) )
		Spawn(class'LinkProjSparksYellow',,, HitLocation, rotator(HitNormal));
    PlaySound(Sound'WeaponSounds.BioRifle.BioRifleGoo2');
	Destroy();
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
            Spawn(Class, Other,, HitLocation+RefDir*20, Rotator(RefDir));
        }
        Destroy();
    }
    else if ( Other.bProjTarget )
	{
		if ( Role == ROLE_Authority )
			Other.TakeDamage(Damage,Instigator,HitLocation,MomentumTransfer * Normal(Velocity),MyDamageType);
		Explode(HitLocation, vect(0,0,1));
	}
}

defaultproperties
{ 
	TrailTex=Texture'XEffectMat.link_muz_yellow'
    Skins(0)=FinalBlend'XEffectMat.LinkProjYellowFB'
    ExplosionDecal=class'LinkBoltScorch'
    Damage=17
    MyDamageType=class'DamTypeKrallBolt'
    Speed=1500
    MaxSpeed=1500
    MomentumTransfer=25000
    ExploWallOut=0
    LifeSpan=4
    AmbientGlow=217
    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightRadius=3
    LightBrightness=255
    LightHue=40
    LightSaturation=100
    bFixedRotationDir=True
    RotationRate=(Roll=80000)
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'WeaponStaticMesh.LinkProjectile'
    DrawScale3D=(X=2.55,Y=1.7,Z=1.7)
    Style=STY_Additive
    AmbientSound=Sound'WeaponSounds.ShockRifle.LinkGunProjectile'
    SoundRadius=50
    SoundVolume=255
    ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=30.0
    PrePivot=(X=10)
}

//=============================================================================
// LinkProjectile.
//=============================================================================
class LinkProjectile extends Projectile;

#exec OBJ LOAD FILE=XEffectMat.utx

var NewLinkTrail Trail;
var int Links;

replication
{
    unreliable if (bNetInitial && Role == ROLE_Authority)
        Links;
}

simulated function Destroyed()
{
    if (Trail != None)
    {
        Trail.Destroy();
    }
	Super.Destroyed();
}

simulated function PostBeginPlay()
{
    local Rotator R;

	Super.PostBeginPlay();

	Velocity = Vector(Rotation);
    Velocity *= Speed;

    R = Rotation;
    R.Roll = Rand(65536);
    SetRotation(R);
}

simulated function LinkAdjust()
{
    if (Links > 0)
    {
    	if ( Trail != None )
    		Trail.MakeYellow();

        MaxSpeed = default.MaxSpeed + 350*Links;
        Skins[0] = FinalBlend'XEffectMat.LinkProjYellowFB';
        LightHue = 40;
    }
}

simulated function PostNetBeginPlay()
{
	local float dist;
	local PlayerController PC;

    Acceleration = Normal(Velocity) * 3000.0;

	if ( (Level.NetMode != NM_DedicatedServer) && (Level.DetailMode != DM_Low) )
		Trail = Spawn(class'newLinkTrail',self);
	if ( (Trail != None) && (Instigator != None) && Instigator.IsLocallyControlled() )
	{
		if ( Role == ROLE_Authority )
			Trail.Delay(0.1);
		else
		{
			dist = VSize(Location - Instigator.Location);
			if ( dist < 100 )
				Trail.Delay(0.1 - dist/1000);
		}
	}

    if (Role < ROLE_Authority)
        LinkAdjust();
    if ( Level.NetMode == NM_DedicatedServer )
		return;
	if ( Level.bDropDetail || (Level.DetailMode == DM_Low) )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
	else
	{
		PC = Level.GetLocalPlayerController();
		if ( (PC == None) || (Instigator == None) || (PC != Instigator.Controller) )
		{
			bDynamicLight = false;
			LightType = LT_None;
		}
	}
}

simulated function Explode(vector HitLocation, vector HitNormal)
{
    if ( EffectIsRelevant(Location,false) )
	{
        if (Links == 0)
            Spawn(class'LinkProjSparks',,, HitLocation, rotator(HitNormal));
        else
            Spawn(class'LinkProjSparksYellow',,, HitLocation, rotator(HitNormal));
	}
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
            //Log("reflecting off"@Other@X@RefDir);
            Spawn(Class, Other,, HitLocation+RefDir*20, Rotator(RefDir));
        }
        Destroy();
    }
    else if ( !Other.IsA('Projectile') || Other.bProjTarget )
	{
		if ( Role == ROLE_Authority )
		{
			if ( Instigator == None || Instigator.Controller == None )
				Other.SetDelayedDamageInstigatorController( InstigatorController );
			Other.TakeDamage(Damage * (1.0 + float(Links)),Instigator,HitLocation,MomentumTransfer * Normal(Velocity),MyDamageType);
		}
		Explode(HitLocation, vect(0,0,1));
	}
}

defaultproperties
{
    MaxEffectDistance=7000.0
    ExplosionDecal=class'LinkBoltScorch'
    Damage=30
    DamageRadius=0.0
    MyDamageType=class'DamTypeLinkPlasma'
    Speed=1000
    MaxSpeed=4000
    MomentumTransfer=0
    ExploWallOut=0
    LifeSpan=3
    AmbientGlow=217
    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightRadius=3
    LightBrightness=255
    LightHue=100
    LightSaturation=100
    bFixedRotationDir=True
    RotationRate=(Roll=80000)
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'WeaponStaticMesh.LinkProjectile'
    DrawScale3D=(X=2.295,Y=1.53,Z=1.53)
    Style=STY_Additive
    AmbientSound=Sound'WeaponSounds.ShockRifle.LinkGunProjectile'
    SoundRadius=50
    SoundVolume=255
    ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=30.0
    PrePivot=(X=10)
    FluidSurfaceShootStrengthMod=6.f
    CullDistance=+3500.0
}

class BioGlob extends Projectile;

var xEmitter Trail;
var() int BaseDamage;
var() float GloblingSpeed;
var() float RestTime;
var() float TouchDetonationDelay; // gives player a split second to jump to gain extra momentum from blast
var() float DripTime;
var() int MaxGoopLevel;

var int GoopLevel;
var float GoopVolume;
var Vector SurfaceNormal;
var int Rand3;
var bool bCheckedSurface;
var() bool bMergeGlobs;
var bool bDrip;
var bool bOnMover;

var() Sound ExplodeSound;
var AvoidMarker Fear;

replication
{
    reliable if (bNetInitial && Role == ROLE_Authority)
        Rand3;
}

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();

    SetOwner(None);

    LoopAnim('flying', 1.0);

    if (Role == ROLE_Authority)
    {
        Velocity = Vector(Rotation) * Speed;
        Velocity.Z += TossZ;
    }

    if (Role == ROLE_Authority)
         Rand3 = Rand(3);
	if ( Level.bDropDetail )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
}

function AdjustSpeed()
{
	if ( GoopLevel < 1 )
		Velocity = Vector(Rotation) * Speed;
	else
		Velocity = Vector(Rotation) * Speed * (0.4 + GoopLevel)/(1.4*GoopLevel);
	Velocity.Z += TossZ;
}

simulated function PostNetBeginPlay()
{
    if (Role < ROLE_Authority && Physics == PHYS_None)
    {
        Landed(Vector(Rotation));
    }
}

simulated function Destroyed()
{
    if ( !bNoFX && EffectIsRelevant(Location,false) )
    {
        Spawn(class'xEffects.GoopSmoke');
        Spawn(class'xEffects.GoopSparks');
    }
	if ( Fear != None )
		Fear.Destroy();
    if (Trail != None)
        Trail.Destroy();
    Super.Destroyed();
}

simulated function MergeWithGlob(int AdditionalGoopLevel)
{
}

auto state Flying
{
    simulated function Landed( Vector HitNormal )
    {
        local Rotator NewRot;
        local int CoreGoopLevel;

        if ( Level.NetMode != NM_DedicatedServer )
        {
            PlaySound(ImpactSound, SLOT_Misc);
            // explosion effects
        }

        SurfaceNormal = HitNormal;

        // spawn globlings
        CoreGoopLevel = Rand3 + MaxGoopLevel - 3;
        if (GoopLevel > CoreGoopLevel)
        {
            if (Role == ROLE_Authority)
                SplashGlobs(GoopLevel - CoreGoopLevel);
            SetGoopLevel(CoreGoopLevel);
        }
		spawn(class'BioDecal',,,, rotator(-HitNormal));

        bCollideWorld = false;
        SetCollisionSize(GoopVolume*10.0, GoopVolume*10.0);
        bProjTarget = true;

	    NewRot = Rotator(HitNormal);
	    NewRot.Roll += 32768;
        SetRotation(NewRot);
        SetPhysics(PHYS_None);
        bCheckedsurface = false;
        Fear = Spawn(class'AvoidMarker');
        GotoState('OnGround');
    }

    simulated function HitWall( Vector HitNormal, Actor Wall )
    {
        Landed(HitNormal);
		if ( !Wall.bStatic && !Wall.bWorldGeometry )
        {
            bOnMover = true;
            SetBase(Wall);
            if (Base == None)
                BlowUp(Location);
        }
    }

    simulated function ProcessTouch(Actor Other, Vector HitLocation)
    {
        local BioGlob Glob;

        Glob = BioGlob(Other);

        if ( Glob != None )
        {
            if (Glob.Owner == None || (Glob.Owner != Owner && Glob.Owner != self))
            {
                if (bMergeGlobs)
                {
                    Glob.MergeWithGlob(GoopLevel); // balancing on the brink of infinite recursion
                    bNoFX = true;
                    Destroy();
                }
                else
                {
                    BlowUp( HitLocation );
                }
            }
        }
        else if (Other != Instigator && (Other.IsA('Pawn') || Other.IsA('DestroyableObjective') || Other.bProjTarget))
            BlowUp( HitLocation );
		else if ( Other != Instigator && Other.bBlockActors )
			HitWall( Normal(HitLocation-Location), Other );
    }
}

state OnGround
{
    simulated function BeginState()
    {
        PlayAnim('hit');
        SetTimer(RestTime, false);
    }

    simulated function Timer()
    {
        if (bDrip)
        {
            bDrip = false;
            SetCollisionSize(default.CollisionHeight, default.CollisionRadius);
            Velocity = PhysicsVolume.Gravity * 0.2;
            SetPhysics(PHYS_Falling);
            bCollideWorld = true;
            bCheckedsurface = false;
            bProjTarget = false;
            LoopAnim('flying', 1.0);
            GotoState('Flying');
        }
        else
        {
            BlowUp(Location);
        }
    }

    simulated function ProcessTouch(Actor Other, Vector HitLocation)
    {
        if ( Other.IsA('Pawn') && (Other != Base) )
        {
            bDrip = false;
            SetTimer(TouchDetonationDelay, false);
        }
    }

    function TakeDamage( int Damage, Pawn InstigatedBy, Vector HitLocation, Vector Momentum, class<DamageType> DamageType )
    {
        if (DamageType.default.bDetonatesGoop)
        {
            bDrip = false;
            SetTimer(0.1, false);
        }
    }

    simulated function AnimEnd(int Channel)
    {
        local float DotProduct;

        if (!bCheckedSurface)
        {
            DotProduct = SurfaceNormal dot Vect(0,0,-1);
            if (DotProduct > 0.7)
            {
                PlayAnim('Drip', 0.66);
                bDrip = true;
                SetTimer(DripTime, false);
                if (bOnMover)
                    BlowUp(Location);
            }
            else if (DotProduct > -0.5)
            {
                PlayAnim('Slide', 1.0);
                if (bOnMover)
                    BlowUp(Location);
            }
            bCheckedSurface = true;
        }
    }

    simulated function MergeWithGlob(int AdditionalGoopLevel)
    {
        local int NewGoopLevel, ExtraSplash;
        NewGoopLevel = AdditionalGoopLevel + GoopLevel;
        if (NewGoopLevel > MaxGoopLevel)
        {
            Rand3 = (Rand3 + 1) % 3;
            ExtraSplash = Rand3;
            if (Role == ROLE_Authority)
                SplashGlobs(NewGoopLevel - MaxGoopLevel + ExtraSplash);
            NewGoopLevel = MaxGoopLevel - ExtraSplash;
        }
        SetGoopLevel(NewGoopLevel);
        SetCollisionSize(GoopVolume*10.0, GoopVolume*10.0);
        PlaySound(ImpactSound, SLOT_Misc);
        PlayAnim('hit');
        bCheckedSurface = false;
        SetTimer(RestTime, false);
    }

}

function BlowUp(Vector HitLocation)
{
    if (Role == ROLE_Authority)
    {
        Damage = BaseDamage + Damage * GoopLevel;
        DamageRadius = DamageRadius * GoopVolume;
        MomentumTransfer = MomentumTransfer * GoopVolume;
        if (Physics == PHYS_Flying) MomentumTransfer *= 0.5;
        DelayedHurtRadius(Damage, DamageRadius, MyDamageType, MomentumTransfer, HitLocation);
    }

    PlaySound(ExplodeSound, SLOT_Misc);

    Destroy();
    //GotoState('shriveling');
}

singular function SplashGlobs(int NumGloblings)
{
    local int g;
    local BioGlob NewGlob;
    local Vector VNorm;

    for (g=0; g<NumGloblings; g++)
    {
        NewGlob = Spawn(Class, self,, Location+GoopVolume*(CollisionHeight+4.0)*SurfaceNormal);
        if (NewGlob != None)
        {
            NewGlob.Velocity = (GloblingSpeed + FRand()*150.0) * (SurfaceNormal + VRand()*0.8);
            if (Physics == PHYS_Falling)
            {
                VNorm = (Velocity dot SurfaceNormal) * SurfaceNormal;
                NewGlob.Velocity += (-VNorm + (Velocity - VNorm)) * 0.1;
            }
            NewGlob.InstigatorController = InstigatorController;
        }
        //else log("unable to spawn globling");
    }
}

state Shriveling
{
    simulated function BeginState()
    {
        bProjTarget = false;
        PlayAnim('shrivel', 1.0);
    }

    simulated function AnimEnd(int Channel)
    {
        Destroy();
    }

    simulated function ProcessTouch(Actor Other, Vector HitLocation)
    {
    }
}

simulated function SetGoopLevel( int NewGoopLevel )
{
    GoopLevel = NewGoopLevel;
    GoopVolume = sqrt(float(GoopLevel));
    SetDrawScale(GoopVolume*default.DrawScale);
    LightBrightness = Min(100 + 15*GoopLevel, 255);
    LightRadius = 1.7 + 0.2*GoopLevel;
    FluidSurfaceShootStrengthMod=5.f + 0.3 * NewGoopLevel;
}

defaultproperties
{
    GoopLevel=1
    GoopVolume=1.0
    MaxGoopLevel=5
    Speed=2000.0
    TossZ=0.0
    GloblingSpeed=200.0
    BaseDamage=20.0
    Damage=19.0 // full load = 210 damage
    DamageRadius=120.0
    MomentumTransfer=40000
    bMergeGlobs=true
    RestTime=2.25
    DripTime=1.8
    TouchDetonationDelay=0.15
    MyDamageType=class'DamTypeBioGlob'
    Physics=PHYS_Falling
    Mesh=Mesh'XWeapons_rc.GoopMesh'
    Skins(0)=FinalBlend'GoopFB'
    DrawScale=1.2
    AmbientGlow=80
    bProjTarget=false
    CollisionRadius=2
    CollisionHeight=2
    SoundRadius=100
    SoundVolume=255
    LifeSpan=20.0
    bUnlit=true
    RemoteRole=ROLE_SimulatedProxy
    bNetTemporary=false
    ExplodeSound=Sound'WeaponSounds.BioRifle.BioRifleGoo1'
    ImpactSound=Sound'WeaponSounds.BioRifle.BioRifleGoo2'
    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightBrightness=190
    LightHue=82
    LightSaturation=10
    LightRadius=0.6
    bSwitchToZeroCollision=true
    bOnlyDirtyReplication=true
    MaxEffectDistance=7000.0
    bUseCollisionStaticMesh=true
}

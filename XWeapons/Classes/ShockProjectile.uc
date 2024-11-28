//=============================================================================
// ShockProjectile.
//=============================================================================
class ShockProjectile extends Projectile;

var() Sound ComboSound;
var() float ComboDamage;
var() float ComboRadius;
var() float ComboMomentumTransfer;
var ShockBall ShockBallEffect;
var() int ComboAmmoCost;
var class<DamageType> ComboDamageType;

var Pawn ComboTarget;		// for AI use

var Vector tempStartLoc;

simulated event PreBeginPlay()
{
    Super.PreBeginPlay();

    if( Pawn(Owner) != None )
        Instigator = Pawn( Owner );
}

simulated function PostBeginPlay()
{
	Super.PostBeginPlay();

    if ( Level.NetMode != NM_DedicatedServer )
	{
        ShockBallEffect = Spawn(class'ShockBall', self);
        ShockBallEffect.SetBase(self);
	}

	Velocity = Speed * Vector(Rotation); // starts off slower so combo can be done closer

    SetTimer(0.4, false);
    tempStartLoc = Location;
}

simulated function PostNetBeginPlay()
{
	local PlayerController PC;
	
	Super.PostNetBeginPlay();
	
	if ( Level.NetMode == NM_DedicatedServer )
		return;
		
	PC = Level.GetLocalPlayerController();
	if ( (Instigator != None) && (PC == Instigator.Controller) )
		return;
	if ( Level.bDropDetail || (Level.DetailMode == DM_Low) )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
	else if ( (PC == None) || (PC.ViewTarget == None) || (VSize(PC.ViewTarget.Location - Location) > 3000) )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
}

function Timer()
{
    SetCollisionSize(20, 20);
}

simulated function Destroyed()
{
    if (ShockBallEffect != None)
    {
		if ( bNoFX )
			ShockBallEffect.Destroy();
		else
			ShockBallEffect.Kill();
	}
	
	Super.Destroyed();
}

simulated function DestroyTrails()
{
    if (ShockBallEffect != None)
        ShockBallEffect.Destroy();
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
		if ( ShockProjectile(Other) != None )
			ShockProjectile(Other).Explode(HitLocation,Normal(Other.Location - HitLocation));
    }
}

simulated function Explode(vector HitLocation,vector HitNormal)
{
    if ( Role == ROLE_Authority )
    {
        HurtRadius(Damage, DamageRadius, MyDamageType, MomentumTransfer, HitLocation );
    }

   	PlaySound(ImpactSound, SLOT_Misc);
	if ( EffectIsRelevant(Location,false) )
	{
	    Spawn(class'ShockExplosionCore',,, Location);
		if ( !Level.bDropDetail && (Level.DetailMode != DM_Low) )
			Spawn(class'ShockExplosion',,, Location);
	}
    SetCollisionSize(0.0, 0.0);
	Destroy();
}

event TakeDamage( int Damage, Pawn EventInstigator, vector HitLocation, vector Momentum, class<DamageType> DamageType)
{
    if (DamageType == ComboDamageType)
    {
        Instigator = EventInstigator;
        SuperExplosion();
        if( EventInstigator.Weapon != None )
        {
			EventInstigator.Weapon.ConsumeAmmo(0, ComboAmmoCost, true);
            Instigator = EventInstigator;
        }
    }
}

function SuperExplosion()
{
	local actor HitActor;
	local vector HitLocation, HitNormal;

	HurtRadius(ComboDamage, ComboRadius, class'DamTypeShockCombo', ComboMomentumTransfer, Location );

	Spawn(class'ShockCombo');
	if ( (Level.NetMode != NM_DedicatedServer) && EffectIsRelevant(Location,false) )
	{
		HitActor = Trace(HitLocation, HitNormal,Location - Vect(0,0,120), Location,false);
		if ( HitActor != None )
			Spawn(class'ComboDecal',self,,HitLocation, rotator(vect(0,0,-1)));
	}
	PlaySound(ComboSound, SLOT_None,1.0,,800);
    DestroyTrails();
    Destroy();
}

function Monitor(Pawn P)
{
	ComboTarget = P;

	if ( ComboTarget != None )
		GotoState('WaitForCombo');
}

State WaitForCombo
{
	function Tick(float DeltaTime)
	{
		if ( (ComboTarget == None) || ComboTarget.bDeleteMe
			|| (Instigator == None) || (ShockRifle(Instigator.Weapon) == None) )
		{
			GotoState('');
			return;
		}

		if ( (VSize(ComboTarget.Location - Location) <= 0.5 * ComboRadius + ComboTarget.CollisionRadius)
			|| ((Velocity Dot (ComboTarget.Location - Location)) <= 0) )
		{
			ShockRifle(Instigator.Weapon).DoCombo();
			GotoState('');
			return;
		}
	}
}

defaultproperties
{
    ExplosionDecal=class'ShockImpactScorch'
    Speed=1150
    MaxSpeed=1150
    Damage=45
    DamageRadius=150
    MomentumTransfer=70000
    ComboDamage=200
    ComboRadius=275
    ComboMomentumTransfer=150000
    ComboSound=Sound'WeaponSounds.ShockRifle.ShockComboFire'
    ComboDamageType=class'DamTypeShockBeam'
    MyDamageType=class'DamTypeShockBall'
    bNetTemporary=False
    LifeSpan=10.0
    DrawType=DT_Sprite
    Skins(0)=Texture'XEffectMat.Shock.shock_core_low'
    Texture=Texture'XEffectMat.Shock.shock_core_low'
    Style=STY_Translucent
    bAlwaysFaceCamera=true
    DrawScale=0.7
    CollisionRadius=10
    CollisionHeight=10
    bProjTarget=True
    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightBrightness=255
    LightHue=195
    LightSaturation=85
    LightRadius=4
    AmbientSound=Sound'WeaponSounds.ShockRifle.ShockRifleProjectile'
    SoundRadius=100
    SoundVolume=50
    ImpactSound=Sound'WeaponSounds.ShockRifle.ShockRifleExplosion'
    ComboAmmoCost=3
    ForceType=FT_Constant
    ForceScale=5.0
    ForceRadius=40.0
    bSwitchToZeroCollision=true
    bOnlyDirtyReplication=true
    FluidSurfaceShootStrengthMod=8.0
    MaxEffectDistance=7000.0
    CullDistance=+4000.0
}

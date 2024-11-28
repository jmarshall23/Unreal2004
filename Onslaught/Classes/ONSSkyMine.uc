class ONSSkyMine extends ShockProjectile;

var ONSPRVSideGun OwnerGun;
var bool bDoChainReaction;
var float MaxChainReactionDist, ChainReactionDelay;
var class<ShockBeamEffect> BeamEffectClass;
var class<Emitter> ProjectileEffectClass;
var Emitter ProjectileEffect;

simulated function PostBeginPlay()
{
	Super(Projectile).PostBeginPlay();

	if ( Level.NetMode != NM_DedicatedServer )
	{
		ProjectileEffect = spawn(ProjectileEffectClass, self,, Location, Rotation);
    		ProjectileEffect.SetBase(self);
	}

	Velocity = Speed * Vector(Rotation);
}

simulated function PostNetBeginPlay()
{
	Super.PostNetBeginPlay();

	OwnerGun = ONSPRVSideGun(Owner);
	if (OwnerGun != None)
		OwnerGun.Projectiles[OwnerGun.Projectiles.length] = self;
}

simulated function DestroyTrails()
{
	if (ProjectileEffect != None)
		ProjectileEffect.Destroy();
}

simulated function Destroyed()
{
	if (ProjectileEffect != None)
		ProjectileEffect.Destroy();

	Super.Destroyed();
}

function TakeDamage(int Damage, Pawn EventInstigator, vector HitLocation, vector Momentum, class<DamageType> DamageType)
{
	if (DamageType == class'DamTypeShockBeam')
	{
		ComboDamageType = DamageType;
		bDoChainReaction = false;
	}

	Super.TakeDamage(Damage, EventInstigator, HitLocation, Momentum, DamageType);
}

function SuperExplosion()
{
	local actor HitActor;
	local vector HitLocation, HitNormal;

	HurtRadius(ComboDamage, ComboRadius, class'DamTypePRVCombo', ComboMomentumTransfer, Location );

	Spawn(class'ONSPRVComboEffect');
	if ( (Level.NetMode != NM_DedicatedServer) && EffectIsRelevant(Location,false) )
	{
		HitActor = Trace(HitLocation, HitNormal,Location - Vect(0,0,120), Location,false);
		if ( HitActor != None )
			Spawn(class'ComboDecal',self,,HitLocation, rotator(vect(0,0,-1)));
	}
	PlaySound(ComboSound, SLOT_None,1.0,,800);
	DestroyTrails();

	if (bDoChainReaction)
	{
		SetPhysics(PHYS_None);
		SetCollision(false);
		bHidden = true;
		SetTimer(ChainReactionDelay, false);
	}
	else
		Destroy();
}

State WaitForCombo
{
	function Tick(float DeltaTime)
	{
		if (ComboTarget == None || ComboTarget.bDeleteMe || ONSWeaponPawn(Instigator) == None || ONSPRVSideGun(ONSWeaponPawn(Instigator).Gun) == None)
		{
			GotoState('');
			return;
		}

		if ( (VSize(ComboTarget.Location - Location) <= 0.75 * ComboRadius + ComboTarget.CollisionRadius)
			|| ((Velocity Dot (ComboTarget.Location - Location)) <= 0) )
		{
			ONSPRVSideGun(ONSWeaponPawn(Instigator).Gun).DoCombo();
			GotoState('');
			return;
		}
	}
}

function Timer()
{
	local int x;
	local ShockBeamEffect Beam;
	local Projectile ChainTarget;
	local float BestDist;

	if (OwnerGun != None)
	{
		BestDist = MaxChainReactionDist;
		for (x = 0; x < OwnerGun.Projectiles.length; x++)
		{
			if (OwnerGun.Projectiles[x] == None || OwnerGun.Projectiles[x] == self)
			{
				OwnerGun.Projectiles.Remove(x, 1);
				x--;
			}
			else if (VSize(Location - OwnerGun.Projectiles[x].Location) < BestDist)
			{
				ChainTarget = OwnerGun.Projectiles[x];
				BestDist = VSize(Location - OwnerGun.Projectiles[x].Location);
			}
		}

		if (ChainTarget != None)
		{
			Beam = Spawn(BeamEffectClass,,, Location, rotator(ChainTarget.Location - Location));
			Beam.Instigator = None;
			Beam.AimAt(ChainTarget.Location, Normal(ChainTarget.Location - Location));
			ChainTarget.TakeDamage(1, Instigator, ChainTarget.Location, vect(0,0,0), ComboDamageType);
		}
	}

	Destroy();
}

defaultproperties
{
	MyDamageType=class'DamTypeSkyMine'
	ComboDamageType=class'DamTypePRVLaser'
	Damage=25
	MomentumTransfer=25000
	Speed=950
	MaxSpeed=950
	ComboDamage=200
	ComboRadius=525
	bDoChainReaction=true
	MaxChainReactionDist=2500
	BeamEffectClass=class'ShockBeamEffect'
	ChainReactionDelay=0.25
	DrawScale=1.05
	CollisionRadius=20
	CollisionHeight=20
	ProjectileEffectClass=class'ShockBall'
	DrawType=DT_None
	Style=STY_Additive
}


class ONSRVWebProjectile extends Projectile
	native
	nativereplication;

var class<Emitter>	ProjectileEffectClass; // Assumes Emitter 0 is the beam to next projectile.
var Emitter			ProjectileEffect;

var ONSRVWebProjectileLeader	Leader;
var	int							ProjNumber;
var	float						LastTickTime;
var bool						bBeingSucked;

var Actor	StuckActor;
var vector	StuckNormal;

var() int   BeamSubEmitterIndex; // Emitter index of BeamEmitter than connects projectiles.
var() float	ExplodeDelay;
var() sound StuckSound;

var() class<Actor>		ExplodeEffect;
var() sound				ExplodeSound;

var() class<Actor>		ExtraDamageClass; // If we stick to this class, do more damage.
var() float				ExtraDamageMultiplier;

cpptext
{
	virtual INT* GetOptimizedRepList(BYTE* Recent, FPropertyRetirement* Retire, INT* Ptr, UPackageMap* Map, UActorChannel* Channel);
	void TickAuthoritative(FLOAT DeltaSeconds);
}

replication
{
	reliable if (bNetDirty && Role == ROLE_Authority)
		Leader, ProjNumber;
}

simulated function Destroyed()
{
	if (ProjectileEffect != None)
		ProjectileEffect.Destroy();

	if ( Level.NetMode != NM_DedicatedServer && EffectIsRelevant(Location, false) )
	{
		Spawn(ExplodeEffect,,, Location, rotator(StuckNormal));
		PlaySound(ExplodeSound,,2.5*TransientSoundVolume);

		//Spawn(ExplosionDecal,self,, Location, rotator(-StuckNormal));
	}

	Super.Destroyed();
}

simulated function PostBeginPlay()
{
	Super.PostBeginPlay();

	Velocity = Speed * Vector(Rotation);

	if (Level.NetMode != NM_DedicatedServer)
	{
		ProjectileEffect = spawn(ProjectileEffectClass, self,, Location, Rotation);
		ProjectileEffect.SetBase(self);
	}

	// On client - add this projectile in next free slot leaders list of projectiles.
	if( Role < ROLE_Authority )
	{
		if(Leader != None && ProjNumber != -1)
		{
			if(Leader.Projectiles.Length < ProjNumber + 1)
				Leader.Projectiles.Length = ProjNumber + 1;

			Leader.Projectiles[ ProjNumber ] = self;
		}
		else
		{
			bNetNotify = true; // We'll need the PostNetReceive to add this projectile to its leader.
		}
	}
}

simulated event PostNetReceive()
{
	if( Leader != None && ProjNumber != -1 )
	{
		if(Leader.Projectiles.Length < ProjNumber + 1)
			Leader.Projectiles.Length = ProjNumber + 1;

		Leader.Projectiles[ ProjNumber ] = self;

		bNetNotify = false; // Don't need PostNetReceive any more.
	}
}

simulated function ProcessTouch(actor Other, vector HitLocation)
{
	//Don't hit the player that fired me
	if (Other == Instigator || (Vehicle(Instigator) != None && Other == Vehicle(Instigator).Driver))
		return;

	// If we hit some stuff - just blow up straight away.
	if( Other.IsA('Projectile') )
	{
		if(Role == ROLE_Authority)
			Leader.DetonateWeb();
	}
	else
	{
		StuckActor = Other;
		StuckNormal = normal(HitLocation - Other.Location);
		GotoState('Stuck');
	}
}

simulated function HitWall(vector HitNormal, Actor Wall)
{
	StuckActor = Wall;
	StuckNormal = HitNormal;
	GoToState('Stuck');
}

// Server-side only
function Explode(vector HitLocation, vector HitNormal)
{
	BlowUp(HitLocation);
	Destroy();
}

function BlowUp(vector HitLocation)
{
	if (StuckActor != None && ClassIsChildOf(StuckActor.Class, ExtraDamageClass))
		Damage *= ExtraDamageMultiplier;

	HurtRadius(Damage, DamageRadius, MyDamageType, MomentumTransfer, HitLocation);
	MakeNoise(1.0);
}


state Stuck
{
	simulated function BeginState()
	{
		if (Leader != None)
			Leader.NotifyStuck();

		SetPhysics(PHYS_None);

		PlaySound(StuckSound,,2.5*TransientSoundVolume);

		if (StuckActor != None)
		{
			LastTouched = StuckActor;
			//log("Setting Base:"@StuckActor);
			SetBase(StuckActor);
		}

		SetCollision(false, false);
		bCollideWorld = false;

		if (Role == ROLE_Authority)
			SetTimer(ExplodeDelay, false);
	}

	//simulated function EndState()
	//{
	//	SetPhysics(PHYS_Falling);
	//	SetCollision(true, false);
	//	bCollideWorld = true;
	//}

	//simulated function BaseChange()
	//{
	//	if (Physics == PHYS_None && Base == None)
	//		GotoState('');
	//}

	function Timer()
	{
		// Should only happen on Authority, where Leader should always be valid.
		Leader.DetonateWeb();
	}
}

defaultproperties
{
	Speed=1750
	MaxSpeed=2000
	Damage=65
	DamageRadius=150.0
	MyDamageType=class'DamTypeONSWeb'
	MomentumTransfer=10000
	DrawType=DT_None
	ProjectileEffectClass=class'ONSRVWebProjectileEffect'
	bNetTemporary=False
	bUpdateSimulatedPosition=True
	ProjNumber=-1

	ExplodeDelay=3.0
	BeamSubEmitterIndex=2
	StuckSound=sound'ONSVehicleSounds-S.WebLauncher.WebStick'
	ExplodeEffect=class'xEffects.GoopSparks'
	ExplodeSound=sound'WeaponSounds.BioRifle.BioRifleGoo1'

	ExtraDamageClass=class'Onslaught.ONSHoverBike'
	ExtraDamageMultiplier=1.5
}

//=============================================================================
// Krall.
//=============================================================================
class Krall extends Monster;

var bool bAttackSuccess;
var bool bLegless;
var bool bSuperAggressive;
var name MeleeAttack[5];

replication
{
    unreliable if( Role==ROLE_Authority )
		bLegless;
}

function PostBeginPlay()
{
	Super.PostBeginPlay();
	bSuperAggressive = (FRand() < 0.5);
}

function RangedAttack(Actor A)
{
	if ( bShotAnim )
		return;
	if ( bLegless )
		SetAnimAction('Shoot3');
	else if ( Physics == PHYS_Swimming )
		SetAnimAction('SwimFire');
	else if ( VSize(A.Location - Location) < MeleeRange + CollisionRadius + A.CollisionRadius )
	{
		PlaySound(sound'strike1k',SLOT_Talk);
		SetAnimAction(MeleeAttack[Rand(5)]);
	}	
	else
	{
		if ( bSuperAggressive && !Controller.bPreparingMove && Controller.InLatentExecution(Controller.LATENT_MOVETOWARD) )
			return;
		if ( Controller.InLatentExecution(501) ) // LATENT_MOVETO
			return;
		SetAnimAction('Shoot1');
	}
	Controller.bPreparingMove = true;
	Acceleration = vect(0,0,0);
	bShotAnim = true;
}

function StrikeDamageTarget()
{
	if (MeleeDamageTarget(20, 21000 * Normal(Controller.Target.Location - Location)))
		PlaySound(sound'hit2k',SLOT_Interact);
}

function vector GetFireStart(vector X, vector Y, vector Z)
{
    return Location + 0.9*X - 0.5*Y;
}

function SpawnShot()
{
	FireProjectile();
}

function PlayTakeHit(vector HitLocation, int Damage, class<DamageType> DamageType)
{
	local rotator r;

	if ( bLegless )
		return;
		
	if ( (Health > 30) || (Damage < 20) || (HitLocation.Z > Location.Z) )
	{
		Super.PlayTakeHit(HitLocation, Damage, DamageType);
		return;
	}
	r = rotator(Location - HitLocation);
	CreateGib('lthigh',DamageType,r);
	CreateGib('rthigh',DamageType,r);
	
	bWaitForAnim = false;
	SetAnimAction('LegLoss');
}

simulated function PlayDying(class<DamageType> DamageType, vector HitLoc)
{
	Super.PlayDying(DamageType,HitLoc);
    
    if ( bLegless )
		PlayAnim('LeglessDeath',0.05);
}

simulated event SetAnimAction(name NewAction)
{
	local int i;
	
	if ( NewAction == 'LegLoss' )
	{
		bWaitForAnim = false;
		GroundSpeed = 100;
		bCanStrafe = false;
		bMeleeFighter = true;
		bLegless = true;
		SetCollisionSize(CollisionRadius,16);
		PrePivot = vect(0,0,1) * (Default.CollisionHeight - 16);

		for ( i=0; i<3; i++ )
		{
			MovementAnims[i] = 'Drag';
			SwimAnims[i] = 'Drag';
			CrouchAnims[i] = 'Drag';
			WalkAnims[i] = 'Drag';
			AirAnims[i] = 'Drag';
			TakeOffAnims[i] = 'Drag';
			LandAnims[i] = 'Drag';
			DodgeAnims[i] = 'Drag';
		}
		IdleWeaponAnim = 'Drag';
		IdleHeavyAnim = 'Drag';
		IdleRifleAnim = 'Drag';
		IdleRestAnim = 'Drag';
		IdleCrouchAnim = 'Drag';
		IdleSwimAnim = 'Drag';
		AirStillAnim = 'Drag';
		TakeoffStillAnim = 'Drag';
		TurnRightAnim = 'Drag';
		TurnLeftAnim = 'Drag';
		CrouchTurnRightAnim = 'Drag';
		CrouchTurnLeftAnim = 'Drag';
	}
	Super.SetAnimAction(NewAction);
}

function PlayVictory()
{
	if ( bLegless )
		return;
	Controller.bPreparingMove = true;
	Acceleration = vect(0,0,0);
	bShotAnim = true;
    PlaySound(sound'staflp4k',SLOT_Interact);	
	SetAnimAction('Twirl');
	Controller.Destination = Location;
	Controller.GotoState('TacticalMove','WaitForAnim');
}

function ThrowDamageTarget()
{
	bAttackSuccess = MeleeDamageTarget(30, vect(0,0,0));
	if ( bAttackSuccess )
		PlaySound(sound'hit2k',SLOT_Interact);
}

function ThrowTarget() 
{
	if ( bAttackSuccess && (VSize(Controller.Target.Location - Location) < CollisionRadius + Controller.Target.CollisionRadius + 1.5 * MeleeRange) )
	{
		PlaySound(sound'hit2k',SLOT_Interact);
		if (Pawn(Controller.Target) != None)
		{
			Pawn(Controller.Target).AddVelocity( 
				(50000.0 * (Normal(Controller.Target.Location - Location) + vect(0,0,1)))/Controller.Target.Mass);
		}
	}
}

defaultproperties
{
	JumpZ=550
     bCanStrafe=false
     CollisionHeight=+00044.000000
	mesh=KrallM
	skins(0)=jkrall
	skins(1)=jkrall
    AmmunitionClass=class'KrallAmmo'
     HitSound(0)=injur1k
     HitSound(1)=injur2k
     HitSound(2)=injur1k
     HitSound(3)=injur2k
     FireSound=Sound'WeaponSounds.ShockRifleAltFire'
     DeathSound(0)=death1k
     DeathSound(1)=death2k
     DeathSound(2)=death1k
     DeathSound(3)=death2k
     ChallengeSound(0)=chlng1k
     ChallengeSound(1)=chlng2k
     ChallengeSound(2)=chlng1k
     ChallengeSound(3)=chlng2k

    SwimAnims(0)=Swim
    SwimAnims(1)=Swim
    SwimAnims(2)=Swim
    SwimAnims(3)=Swim
    IdleSwimAnim=Swim
    
    WalkAnims(0)=WalkF
    WalkAnims(1)=WalkF
    WalkAnims(2)=WalkF
    WalkAnims(3)=WalkF

    MovementAnims(0)=RunF
    MovementAnims(1)=RunF
    MovementAnims(2)=RunF
    MovementAnims(3)=RunF

	MeleeAttack(0)=Strike1
	MeleeAttack(1)=Strike2
	MeleeAttack(2)=Strike3
	MeleeAttack(3)=Throw
	MeleeAttack(4)=Throw
	
	ScoringValue=2
}

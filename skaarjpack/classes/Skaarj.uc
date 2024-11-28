class Skaarj extends Monster;

var sound FootStep[2];
var name DeathAnim[4];

function PlayVictory()
{
	Controller.bPreparingMove = true;
	Acceleration = vect(0,0,0);
	bShotAnim = true;
    PlaySound(sound'hairflp2sk',SLOT_Interact);	
	SetAnimAction('HairFlip');
	Controller.Destination = Location;
	Controller.GotoState('TacticalMove','WaitForAnim');
}

function bool SameSpeciesAs(Pawn P)
{
	return ( (Monster(P) != None) && (P.IsA('Skaarj') || P.IsA('WarLord')) );
}

function vector GetFireStart(vector X, vector Y, vector Z)
{
    return Location + 0.9 * CollisionRadius * X + 0.9 * CollisionRadius * Y + 0.4 * CollisionHeight * Z;
}

function SpawnTwoShots()
{
	local vector X,Y,Z, FireStart;
	local rotator FireRotation;
	
	GetAxes(Rotation,X,Y,Z);
	FireStart = GetFireStart(X,Y,Z);
	if ( !SavedFireProperties.bInitialized )
	{
		SavedFireProperties.AmmoClass = MyAmmo.Class;
		SavedFireProperties.ProjectileClass = MyAmmo.ProjectileClass;
		SavedFireProperties.WarnTargetPct = MyAmmo.WarnTargetPct;
		SavedFireProperties.MaxRange = MyAmmo.MaxRange;
		SavedFireProperties.bTossed = MyAmmo.bTossed;
		SavedFireProperties.bTrySplash = MyAmmo.bTrySplash;
		SavedFireProperties.bLeadTarget = MyAmmo.bLeadTarget;
		SavedFireProperties.bInstantHit = MyAmmo.bInstantHit;
		SavedFireProperties.bInitialized = true;
	}
	FireRotation = Controller.AdjustAim(SavedFireProperties,FireStart,600);
	Spawn(MyAmmo.ProjectileClass,,,FireStart,FireRotation);
		
	FireStart = FireStart - 1.8 * CollisionRadius * Y;
	FireRotation.Yaw += 400;
	spawn(MyAmmo.ProjectileClass,,,FireStart, FireRotation);
}

simulated function AnimEnd(int Channel)
{
	local name Anim;
	local float frame,rate;
	
	if ( Channel == 0 )
	{
		GetAnimParams(0, Anim,frame,rate);
		if ( Anim == 'looking' )
			IdleWeaponAnim = 'guncheck';
		else if ( (Anim == 'guncheck') && (FRand() < 0.5) )
			IdleWeaponAnim = 'looking';
	}
	Super.AnimEnd(Channel);
}

function RunStep()
{
	PlaySound(FootStep[Rand(2)], SLOT_Interact);
}

function WalkStep()
{
	PlaySound(FootStep[Rand(2)], SLOT_Interact,0.2);
}

simulated function PlayDying(class<DamageType> DamageType, vector HitLoc)
{
	AmbientSound = None;
    bCanTeleport = false; 
    bReplicateMovement = false;
    bTearOff = true;
    bPlayedDeath = true;
		
	HitDamageType = DamageType; // these are replicated to other clients
    TakeHitLocation = HitLoc;
	LifeSpan = RagdollLifeSpan;

    GotoState('Dying');
		
	Velocity += TearOffMomentum;
    BaseEyeHeight = Default.BaseEyeHeight;
    SetPhysics(PHYS_Falling);
    
    if ( (DamageType == class'DamTypeSniperHeadShot')
		|| ((HitLoc.Z > Location.Z + 0.75 * CollisionHeight) && (FRand() > 0.5) 
			&& (DamageType != class'DamTypeAssaultBullet') && (DamageType != class'DamTypeMinigunBullet') && (DamageType != class'DamTypeFlakChunk')) )
    {
		PlayAnim('Death5',1,0.05);
		CreateGib('head',DamageType,Rotation);
		return;
	}
	if ( Velocity.Z > 300 )
	{
		if ( FRand() < 0.5 )
			PlayAnim('Death',1.2,0.05);
		else
			PlayAnim('Death2',1.2,0.05);
		return;
	}
	PlayAnim(DeathAnim[Rand(4)],1.2,0.05);		
}

function SpinDamageTarget()
{
	if (MeleeDamageTarget(20, (30000 * Normal(Controller.Target.Location - Location))) )
		PlaySound(sound'clawhit1s', SLOT_Interact);		
}

function ClawDamageTarget()
{
	if ( MeleeDamageTarget(25, (25000 * Normal(Controller.Target.Location - Location))) )
		PlaySound(sound'clawhit1s', SLOT_Interact);			
}

function RangedAttack(Actor A)
{
	local name Anim;
	local float frame,rate;
	
	if ( bShotAnim )
		return;
	bShotAnim = true;
	if ( Physics == PHYS_Swimming )
		SetAnimAction('SwimFire');
	else if ( VSize(A.Location - Location) < MeleeRange + CollisionRadius + A.CollisionRadius )
	{
		if ( FRand() < 0.7 )
		{
			SetAnimAction('Spin');
			PlaySound(sound'Spin1s', SLOT_Interact);
			Acceleration = AccelRate * Normal(A.Location - Location);
			return;
		}
		SetAnimAction('Claw');	
		PlaySound(sound'Claw2s', SLOT_Interact);
		Controller.bPreparingMove = true;
		Acceleration = vect(0,0,0);
	}	
	else if ( Velocity == vect(0,0,0) )
	{
		SetAnimAction('Firing');
		Controller.bPreparingMove = true;
		Acceleration = vect(0,0,0);
	}
	else
	{
		GetAnimParams(0,Anim,frame,rate);
		if ( Anim == 'RunL' )
			SetAnimAction('StrafeLeftFr');
		else if ( Anim == 'RunR' )
			SetAnimAction('StrafeRightFr');
		else
			SetAnimAction('JogFire');
	}
}

defaultproperties
{
	JumpZ=550
     bCanStrafe=True
    AmmunitionClass=class'SkaarjAmmo'
     Mesh=Skaarjw
     Skins(0)=Material'Skaarjw1'
     HitSound(0)=injur1sk
     HitSound(1)=injur2sk
     HitSound(2)=injur3sk
     HitSound(3)=injur3sk
     DeathSound(0)=death1sk
	 DeathSound(1)=death2sk
     Footstep(0)=walkC
     Footstep(1)=walkC
     Health=150
     MeleeRange=+00060.000000
     GroundSpeed=+00440.000000
     Mass=+00150.000000
     Buoyancy=+00150.000000
     RotationRate=(Pitch=3072,Yaw=60000,Roll=2048)
     CollisionRadius=+00025.000000
     CollisionHeight=+00044.000000
     ChallengeSound(0)=chalnge1s
     ChallengeSound(1)=chalnge3s
     ChallengeSound(2)=roam11s
     ChallengeSound(3)=roam11s
     DeathAnim(0)=Death
     DeathAnim(1)=Death2
     DeathAnim(2)=Death3
     DeathAnim(3)=Death4
 
    MovementAnims(0)=RunF
    MovementAnims(1)=RunR
    MovementAnims(2)=RunR
    MovementAnims(3)=RunL
    
    SwimAnims(0)=Swim
    SwimAnims(1)=Swim
    SwimAnims(2)=Swim
    SwimAnims(3)=Swim

    WalkAnims(0)=WalkF
    WalkAnims(1)=WalkF
    WalkAnims(2)=WalkF
    WalkAnims(3)=WalkF
    
    AirStillAnim=Jump2
    AirAnims(0)=InAir
    AirAnims(1)=InAir
    AirAnims(2)=InAir
    AirAnims(3)=InAir
    TakeoffStillAnim=Jump2
    TakeoffAnims(0)=Jump
    TakeoffAnims(1)=Jump
    TakeoffAnims(2)=Jump
    TakeoffAnims(3)=Jump
    LandAnims(0)=Landed
    LandAnims(1)=Landed
    LandAnims(2)=Landed
    LandAnims(3)=Landed

    DodgeAnims(0)=DodgeF
    DodgeAnims(1)=DodgeB
    DodgeAnims(2)=DodgeL
    DodgeAnims(3)=DodgeR

    TurnRightAnim=Turn
    TurnLeftAnim=Turn
    
    IdleRestAnim=Breath
    IdleCrouchAnim=Crouch
    IdleSwimAnim=Swim
    IdleWeaponAnim=Looking
    IdleHeavyAnim=Idle_Biggun
    IdleRifleAnim=Idle_Rifle
    FireHeavyRapidAnim=Biggun_Burst
    FireHeavyBurstAnim=Biggun_Aimed
    FireRifleRapidAnim=Rifle_Burst
    FireRifleBurstAnim=Rifle_Aimed
 
	ScoringValue=6
}

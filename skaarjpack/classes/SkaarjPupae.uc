class SkaarjPupae extends Monster;

var name DeathAnims[3];
var bool bLunging;

function SetMovementPhysics()
{
	SetPhysics(PHYS_Falling); 
}

simulated function PlayDirectionalDeath(Vector HitLoc)
{
	PlayAnim(DeathAnims[Rand(3)], 0.7, 0.1);
}

simulated function PlayDirectionalHit(Vector HitLoc)
{
	TweenAnim('TakeHit', 0.05);
}

function PlayVictory()
{
	Controller.bPreparingMove = true;
	Acceleration = vect(0,0,0);
	bShotAnim = true;
	PlayAnim('Stab', 1.0, 0.1);
	Controller.Destination = Location;
	Controller.GotoState('TacticalMove','WaitForAnim');
}

singular function Bump(actor Other)
{
	local name Anim;
	local float frame,rate;
	
	if ( bShotAnim && bLunging )
	{
		bLunging = false;
		GetAnimParams(0, Anim,frame,rate);
		if ( Anim == 'Lunge' )
			MeleeDamageTarget(12, (20000.0 * Normal(Controller.Target.Location - Location)));
	}		
	Super.Bump(Other);
}

function RangedAttack(Actor A)
{
	local float Dist;
	
	if ( bShotAnim )
		return;
		
	Dist = VSize(A.Location - Location);
	if ( Dist > 350 )
		return;
	bShotAnim = true;
	PlaySound(ChallengeSound[Rand(4)], SLOT_Interact);
	if ( Dist < MeleeRange + CollisionRadius + A.CollisionRadius )
	{
  		if ( FRand() < 0.5 )
  			SetAnimAction('Bite');
  		else
  			SetAnimAction('Stab');
		MeleeDamageTarget(8, vect(0,0,0));
		Controller.bPreparingMove = true;
		Acceleration = vect(0,0,0);
		return;
	}
	
	// lunge at enemy
	bLunging = true;
	Enable('Bump');
	SetAnimAction('Lunge');
	Velocity = 500 * Normal(A.Location + A.CollisionHeight * vect(0,0,0.75) - Location);
	if ( dist > CollisionRadius + A.CollisionRadius + 35 )
		Velocity.Z += 0.7 * dist;
	SetPhysics(PHYS_Falling);
}

defaultproperties
{
     HitSound(0)=injur1pp
     HitSound(1)=injur1pp
     HitSound(2)=injur2pp
     HitSound(3)=injur2pp

     DeathSound(0)=death1pp
     DeathSound(1)=death1pp
     DeathSound(2)=death1pp
     DeathSound(3)=death1pp
     ChallengeSound(0)=hiss2pp
     ChallengeSound(1)=hiss1pp
     ChallengeSound(2)=roam1pp
     ChallengeSound(3)=hiss3pp
     
     bCrawler=true
	 bAlwaysStrafe=true
	 bCanStrafe=True
     Health=60
     MeleeRange=+0025.000000
     GroundSpeed=+00300.000000
     WaterSpeed=+00300.000000
     JumpZ=+00450.000000
     Mesh=Pupae1
     Skins(0)=JPupae1
     Skins(1)=JPupae1
     CollisionRadius=+00028.000000
     CollisionHeight=+00012.000000
     Mass=+00080.000000
     RotationRate=(Pitch=3072,Yaw=65000,Roll=0)
    
    AirStillAnim=Lunge
    AirAnims(0)=Lunge
    AirAnims(1)=Lunge
    AirAnims(2)=Lunge
    AirAnims(3)=Lunge
    TakeoffStillAnim=Lunge
    TakeoffAnims(0)=Lunge
    TakeoffAnims(1)=Lunge
    TakeoffAnims(2)=Lunge
    TakeoffAnims(3)=Lunge
    LandAnims(0)=Land
    LandAnims(1)=Land
    LandAnims(2)=Land
    LandAnims(3)=Land
    DodgeAnims(0)=Lunge
    DodgeAnims(1)=Lunge
    DodgeAnims(2)=Lunge
    DodgeAnims(3)=Lunge
    DoubleJumpAnims(0)=Lunge
    DoubleJumpAnims(1)=Lunge
    DoubleJumpAnims(2)=Lunge
    DoubleJumpAnims(3)=Lunge
    
    WalkAnims(0)=Crawl
    WalkAnims(1)=Crawl
    WalkAnims(2)=Crawl
    WalkAnims(3)=Crawl

    MovementAnims(0)=Crawl
    MovementAnims(1)=Crawl
    MovementAnims(2)=Crawl
    MovementAnims(3)=Crawl

    SwimAnims(0)=Crawl
    SwimAnims(1)=Crawl
    SwimAnims(2)=Crawl
    SwimAnims(3)=Crawl
    IdleSwimAnim=Crawl
    
    DeathAnims(0)=Dead
    DeathAnims(1)=Dead2
    DeathAnims(2)=Dead3

    TurnRightAnim=Crawl
    TurnLeftAnim=Crawl
}

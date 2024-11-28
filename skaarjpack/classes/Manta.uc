//=============================================================================
// Manta.
//=============================================================================
class Manta extends Monster;

var bool bStinging;


simulated function PostBeginPlay()
{
	Super.PostBeginPlay();
	PlayAnim('Fly');
}

function SetMovementPhysics()
{
	SetPhysics(PHYS_Flying); 
	PlayAnim('Fly');
}

function WingBeat()
{
	PlaySound(sound'fly1m', SLOT_Interact);
}

simulated function AnimEnd(int Channel)
{
	local name Anim;
	local float frame,rate;
	local vector AccelDir;

	if ( bShotAnim )
		bShotAnim = false;
	if ( bVictoryNext && (Physics != PHYS_Falling) )
	{
		bVictoryNext = false;
		PlayVictory();
		return;
	}
	GetAnimParams(0, Anim,frame,rate);
	if ( Anim != 'Fly' )
		TweenAnim('Fly',0.4);
	else if ( (frame > 0.5) && (FRand() < 0.35) )
	{
		AccelDir = Normal(Acceleration);
		if ( AccelDir.Z > 0.5 )
			PlayAnim('Fly');
		else
			TweenAnim('Fly',0.8 + 2*FRand()+FRand());
	}
	else
		PlayAnim('Fly');
}

simulated function PlayDirectionalDeath(Vector HitLoc)
{
	PlayAnim('Death');
}

simulated function PlayDirectionalHit(Vector HitLoc)
{
	TweenAnim('TakeHit', 0.05);
}

function PlayVictory()
{
	SetAnimAction('Whip');
}

function RangedAttack(Actor A)
{
	if ( bShotAnim )
		return;
		
	if ( Location.Z - A.Location.Z + A.CollisionHeight <= 0 )
		return;
	if ( VSize(A.Location - Location) > MeleeRange + CollisionRadius + A.CollisionRadius - FMax(0, 0.7 * A.Velocity Dot Normal(A.Location - Location)) )
		return;
	bShotAnim = true;
	Acceleration = AccelRate * Normal(A.Location - Location + vect(0,0,0.8) * A.CollisionHeight);
	Enable('Bump');
	bStinging = true;
	if (FRand() < 0.5)
	{
		SetAnimAction('Sting');
		PlaySound(sound'whip1m', SLOT_Interact);	 		
	}
	else
	{
 		SetAnimAction('Whip');
 		PlaySound(sound'sting1m', SLOT_Interact); 
 	}	
 }

singular function Bump(actor Other)
{
	local name Anim;
	local float frame,rate;
	
	if ( bShotAnim && bStinging )
	{
		bStinging = false;
		GetAnimParams(0, Anim,frame,rate);
		if ( (Anim == 'Whip') || (Anim == 'Sting') )
			MeleeDamageTarget(18, (20000.0 * Normal(Controller.Target.Location - Location)));
		Velocity *= -0.5;
		Acceleration *= -1;
		if (Acceleration.Z < 0)
			Acceleration.Z *= -1;
	}		
	Super.Bump(Other);
}

defaultproperties
{
	bCanFly=true
	bCanStrafe=false
	bCanDodge=false
	bPhysicsAnimUpdate=false
     MeleeRange=+00200.000000
     WaterSpeed=+00300.000000
     AirSpeed=+00400.000000
     AccelRate=+0800.000000
     Mesh=Manta1
     Skins(0)=JManta1
     Skins(1)=JManta1
     CollisionRadius=+00025.000000
     CollisionHeight=+00012.000000
     Mass=+00080.000000
     Buoyancy=+00080.000000
     RotationRate=(Pitch=16384,Yaw=55000,Roll=15000)

     HitSound(0)=injur1m
     HitSound(1)=injur2m
     HitSound(2)=injur1m
     HitSound(3)=injur2m
     DeathSound(0)=Death2M
     DeathSound(1)=Death2M
     DeathSound(2)=Death2M
     DeathSound(3)=Death2M
     ChallengeSound(0)=call1M
     ChallengeSound(1)=call1M
     ChallengeSound(2)=call2M
     ChallengeSound(3)=call2M
     
    AirStillAnim=Fly
    AirAnims(0)=Fly
    AirAnims(1)=Fly
    AirAnims(2)=Fly
    AirAnims(3)=Fly
    TakeoffStillAnim=Fly
    TakeoffAnims(0)=Fly
    TakeoffAnims(1)=Fly
    TakeoffAnims(2)=Fly
    TakeoffAnims(3)=Fly
    MovementAnims(0)=Fly
    MovementAnims(1)=Fly
    MovementAnims(2)=Fly
    MovementAnims(3)=Fly
    
    WalkAnims(0)=Fly
    WalkAnims(1)=Fly
    WalkAnims(2)=Fly
    WalkAnims(3)=Fly

}

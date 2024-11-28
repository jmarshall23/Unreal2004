class RazorFly extends Monster;

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

singular function Falling()
{
	SetPhysics(PHYS_Flying);
}

simulated function AnimEnd(int Channel)
{
	if ( bShotAnim )
		bShotAnim = false;
	LoopAnim('Fly',1,0.05);
}

simulated function PlayDirectionalDeath(Vector HitLoc)
{
	PlayAnim('Dead');
}

simulated function PlayDirectionalHit(Vector HitLoc)
{
	TweenAnim('TakeHit', 0.05);
}

function RangedAttack(Actor A)
{
	if ( VSize(A.Location - Location) < MeleeRange + CollisionRadius + A.CollisionRadius )
	{
		bShotAnim = true;
		PlayAnim('Shoot1');
		if ( MeleeDamageTarget(10, (15000.0 * Normal(A.Location - Location))) )
			PlaySound(sound'injur1rf', SLOT_Talk); 
			
		Controller.Destination = Location + 110 * (Normal(Location - A.Location) + VRand());
		Controller.Destination.Z = Location.Z + 70;
		Velocity = AirSpeed * normal(Controller.Destination - Location);
		Controller.GotoState('TacticalMove', 'DoMove');
	}
}

defaultproperties
{
	bCanFly=true
	bCanDodge=false
	bPhysicsAnimUpdate=false
	bAlwaysStrafe=true
     Health=35
     bCanStrafe=True
     MeleeRange=+00040.000000
     AirSpeed=+00300.000000
     AccelRate=+00600.000000
     Mesh=FlyM
     Skins(0)=JFly1
     Skins(1)=JFly1
     AmbientSound=buzz3rf
     DrawScale=+1.0
     CollisionRadius=+00018.000000
     CollisionHeight=+0011.000000
     RotationRate=(Pitch=6000,Yaw=65000,Roll=8192)
     
     HitSound(0)=injur1rf
     HitSound(1)=injur2rf
     HitSound(2)=injur1rf
     HitSound(3)=injur2rf
     DeathSound(0)=death1rf
     DeathSound(1)=death1rf
     DeathSound(2)=death1rf
     DeathSound(3)=death1rf
}

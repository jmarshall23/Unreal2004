class GasBag extends Monster;

function RangedAttack(Actor A)
{
	local vector adjust;
	
	if ( bShotAnim )
		return;
	if ( VSize(A.Location - Location) < MeleeRange + CollisionRadius + A.CollisionRadius )
	{
		adjust = vect(0,0,0);
		adjust.Z = Controller.Target.CollisionHeight;
		Acceleration = AccelRate * Normal(Controller.Target.Location - Location + adjust);
		PlaySound(sound'twopunch1g',SLOT_Talk);
		if (FRand() < 0.5)
			SetAnimAction('TwoPunch');
		else
			SetAnimAction('Pound');
	}
	else	
		SetAnimAction('Belch');
	bShotAnim = true;
}

function bool Dodge(eDoubleClickDir DoubleClickMove)
{
	local vector X,Y,Z,duckdir;

    GetAxes(Rotation,X,Y,Z);
	if (DoubleClickMove == DCLICK_Forward)
		duckdir = X;
	else if (DoubleClickMove == DCLICK_Back)
		duckdir = -1*X; 
	else if (DoubleClickMove == DCLICK_Left)
		duckdir = Y; 
	else if (DoubleClickMove == DCLICK_Right)
		duckdir = -1*Y; 

	Controller.Destination = Location + 200 * duckDir;
	Velocity = AirSpeed * duckDir;
	Controller.GotoState('TacticalMove', 'DoMove');
	return true;
}	

function SetMovementPhysics()
{
	SetPhysics(PHYS_Flying); 
}

singular function Falling()
{
	SetPhysics(PHYS_Flying);
}

simulated function PlayDirectionalDeath(Vector HitLoc)
{
	if ( FRand() < 0.5 )
		PlayAnim('Deflate',, 0.1);
	else
		PlayAnim('Dead2',, 0.1);
}

simulated function PlayDirectionalHit(Vector HitLoc)
{
	if ( FRand() < 0.6 )
		TweenAnim('TakeHit', 0.05);
	else
		TweenAnim('Hit2', 0.05);
}

function PlayVictory()
{
    PlaySound(sound'twopunch1g',SLOT_Interact);	
	SetAnimAction('Pound');
}

function SpawnBelch()
{
	FireProjectile();
}

function vector GetFireStart(vector X, vector Y, vector Z)
{
    return Location + 0.5*X;
}

function PunchDamageTarget()
{
	if (MeleeDamageTarget(25, (39000 * Normal(Controller.Target.Location - Location))))
		PlaySound(sound'Hit1g', SLOT_Interact);
}

function PoundDamageTarget()
{
	if (MeleeDamageTarget(35, (24000 * Normal(Controller.Target.Location - Location))))
		PlaySound(sound'Hit1g', SLOT_Interact);
}

defaultproperties
{
	DodgeSkillAdjust=4
	Skins(0)=Gasbag1
	Skins(1)=GasBag2
	bMeleeFighter=false
	  bCanFly=true
      Health=150
      AirSpeed=+00330.000000
      Mesh=GasbagM
      AmbientSound=amb2g
      CollisionRadius=+00047.000000
      CollisionHeight=+00036.000000
      Mass=+00120.000000

     AmmunitionClass=class'GasbagAmmo'
     HitSound(0)=injur1g
     HitSound(1)=injur2g
     HitSound(2)=injur1g
     HitSound(3)=injur2g
     FireSound=yell3g
     DeathSound(0)=death1g
     DeathSound(1)=death1g
     DeathSound(2)=death1g
     DeathSound(3)=death1g
     ChallengeSound(0)=yell2g
     ChallengeSound(1)=yell3g
     ChallengeSound(2)=nearby1g
     ChallengeSound(3)=yell2g
     
    IdleWeaponAnim=Grab
    TurnRightAnim=idle_rest
    TurnLeftAnim=idle_rest

    CrouchAnims(0)=idle_rest
    CrouchAnims(1)=idle_rest
    CrouchAnims(2)=idle_rest
    CrouchAnims(3)=idle_rest
    
    CrouchTurnRightAnim=Float
    CrouchTurnLeftAnim=Float
    
    AirStillAnim=Float
    AirAnims(0)=Float
    AirAnims(1)=Float
    AirAnims(2)=Float
    AirAnims(3)=Float
    TakeoffStillAnim=Float
    TakeoffAnims(0)=Float
    TakeoffAnims(1)=Float
    TakeoffAnims(2)=Float
    TakeoffAnims(3)=Float
    LandAnims(0)=Float
    LandAnims(1)=Float
    LandAnims(2)=Float
    LandAnims(3)=Float
    DodgeAnims(0)=Float
    DodgeAnims(1)=Float
    DodgeAnims(2)=Float
    DodgeAnims(3)=Float
    MovementAnims(0)=Float
    MovementAnims(1)=Float
    MovementAnims(2)=Float
    MovementAnims(3)=Float

	ScoringValue=4
}

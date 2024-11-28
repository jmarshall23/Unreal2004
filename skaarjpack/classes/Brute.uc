//=============================================================================
// Brute.
//=============================================================================
class Brute extends Monster;

#exec OBJ LOAD FILE=SkaarjPackSkins.utx

//-----------------------------------------------------------------------------
// Brute variables.

var bool bLeftShot;

// Sounds
var(Sounds) sound Footstep[2];
var name MeleeAttack[4];


function PlayTakeHit(vector HitLocation, int Damage, class<DamageType> DamageType)
{
	if ( Damage > 15 )
		Super.PlayTakeHit(HitLocation,Damage,DamageType);
}

function PlayVictory()
{
	Controller.bPreparingMove = true;
	Acceleration = vect(0,0,0);
	bShotAnim = true;
	Controller.Destination = Location;
	Controller.GotoState('TacticalMove','WaitForAnim');
	SetAnimAction('StillLook');
}

function RangedAttack(Actor A)
{
	if ( bShotAnim )
		return;
	if ( VSize(A.Location - Location) < MeleeRange + CollisionRadius + A.CollisionRadius )
	{
		PlaySound(sound'pwhip1br',SLOT_Talk);
		SetAnimAction(MeleeAttack[Rand(4)]);
	}	
	else if ( Controller.InLatentExecution(501) ) // LATENT_MOVETO
		return;
	else
		SetAnimAction('StillFire');

	Controller.bPreparingMove = true;
	Acceleration = vect(0,0,0);
	bShotAnim = true;
}

function WhipDamageTarget()
{
	if ( MeleeDamageTarget(35, 40000.0 * Normal(Controller.Target.Location - Location)) )
		PlaySound(sound'pwhip1br', SLOT_Interact);
}
	
function SpawnLeftShot()
{
	bLeftShot = true;
	FireProjectile();
}

function SpawnRightShot()
{
	bLeftShot = false;
	FireProjectile();
}

function vector GetFireStart(vector X, vector Y, vector Z)
{
	if ( bLeftShot )
		return Location + CollisionRadius * ( X + 0.7 * Y + 0.4 * Z);
	else
		return Location + CollisionRadius * ( X - 0.7 * Y + 0.4 * Z);
}

function Step()
{
	PlaySound(FootStep[Rand(2)], SLOT_Interact);
}
	
defaultproperties
{
     footstep(0)=walk1br
     Footstep(1)=walk2br
     Health=220
     bCanStrafe=false
	 bCanDodge=false
     MeleeRange=+00080.000000
     GroundSpeed=+00150.000000
     WaterSpeed=+00100.000000
     JumpZ=100.000000
     Mesh=Brute1
     CollisionRadius=+00047.000000
     CollisionHeight=+00052.000000
     Mass=+00400.000000
	 Buoyancy=+000390.000000
     RotationRate=(Pitch=3072,Yaw=45000,Roll=0)
 	 skins(0)=jbrute1
 	 skins(1)=RedShell
     AmmunitionClass=class'BruteAmmo'

    SwimAnims(0)=WalkF
    SwimAnims(1)=WalkF
    SwimAnims(2)=WalkF
    SwimAnims(3)=WalkF
    IdleSwimAnim=WalkF
    
    WalkAnims(0)=WalkF
    WalkAnims(1)=WalkF
    WalkAnims(2)=WalkF
    WalkAnims(3)=WalkF

    MovementAnims(0)=WalkF
    MovementAnims(1)=WalkF
    MovementAnims(2)=WalkF
    MovementAnims(3)=WalkF

	MeleeAttack(0)=PistolWhip
	MeleeAttack(1)=Punch
	MeleeAttack(2)=PistolWhip
	MeleeAttack(3)=Punch

     HitSound(0)=injur1br
     HitSound(1)=injur2br
     HitSound(2)=injur1br
     HitSound(3)=injur2br
     FireSound=Sound'WeaponSounds.RocketLauncherFire'
     DeathSound(0)=death1k
     DeathSound(1)=death2k
     DeathSound(2)=death1k
     DeathSound(3)=death2k
     ChallengeSound(0)=yell1br
     ChallengeSound(1)=injur2br
     ChallengeSound(2)=nearby2br
     ChallengeSound(3)=yell2br

    IdleWeaponAnim=CockGun
    IdleHeavyAnim=Idle_Rest
    IdleRifleAnim=Idle_Rest
    
    AirStillAnim=Jump
    AirAnims(0)=Jump
    AirAnims(1)=Jump
    AirAnims(2)=Jump
    AirAnims(3)=Jump
    TakeoffStillAnim=Jump
    TakeoffAnims(0)=Jump
    TakeoffAnims(1)=Jump
    TakeoffAnims(2)=Jump
    TakeoffAnims(3)=Jump
    LandAnims(0)=Land
    LandAnims(1)=Land
    LandAnims(2)=Land
    LandAnims(3)=Land
    DodgeAnims(0)=Jump
    DodgeAnims(1)=Jump
    DodgeAnims(2)=Jump
    DodgeAnims(3)=Jump

	ScoringValue=5
	TransientSoundVolume=+0.7
    TransientSoundRadius=800 
}

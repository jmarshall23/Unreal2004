//=============================================================================
// for tutorial
//=============================================================================
class DestroyableTrigger extends Actor;

var() Name DamageTypeName;
var() bool bActive;
var int StartHealth;
var() int Health;

function PostBeginPlay()
{
	Super.PostBeginPlay();
	StartHealth = Health;
}

function SpawnEffects()
{
    Spawn(class'NewExplosionA',,,Location+VRand()*Vect(50,50,50));
    Spawn(class'NewExplosionA',,,Location+VRand()*Vect(50,50,50));
    Spawn(class'WallSparks',,,Location+VRand()*Vect(50,50,50));
    Spawn(class'WallSparks',,,Location+VRand()*Vect(50,50,50));
    Spawn(class'WallSparks',,,Location+VRand()*Vect(50,50,50));
    Spawn(class'WallSparks',,,Location+VRand()*Vect(50,50,50));
    Spawn(class'ExplosionCrap',,, Location, Rotation);
}

function DoHitEffect()
{
    Spawn(class'WallSparks',,,Location+VRand()*Vect(50,50,50));
}

function TakeDamage( int Damage, Pawn instigatedBy, Vector hitlocation, 
						Vector momentum, class<DamageType> damageType)
{
    if( !bActive || (DamageTypeName != 'None' && DamageTypeName != damageType.Name) )
        return;

    if( Health <= 0 )
        return;

    Health -= Damage;

	if ( (Health <= 0) )
	{
		// Broadcast the Trigger message to all matching actors.
		TriggerEvent(Event, self, instigatedBy);
		SpawnEffects();
        SetCollision(false,false);
        bProjTarget = false;
	}
    else
    {
        DoHitEffect();
    }
}

/* Reset() 
reset actor to initial state - used when restarting level without reloading.
*/
function Reset()
{
	SetCollision(true,true);
	bProjTarget = true;
	Health = StartHealth;
}

defaultproperties
{
    bUnlit=false
    bHidden=false
    bStasis=false
    bStatic=false
    bCollideActors=true
    bCollideWorld=true    
    bNetNotify=true
    bBlockKarma=true
    bBlockActors=true
    bProjTarget=true
    bBlockZeroExtentTraces=true
    bBlockNonZeroExtentTraces=true
    bUseCylinderCollision=false    
	bCanBeDamaged=true

    bActive=false
    Health=30
    Mass=100.000000
	NetUpdateFrequency=5
	RemoteRole=ROLE_None
}



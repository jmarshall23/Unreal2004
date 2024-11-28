//=============================================================================
// The IonCannon
//=============================================================================
class IonCannon extends Actor
    placeable;

var() Sound FireSound;
var() Vector MarkLocation;
var() class<IonEffect> IonEffectClass;
var() int Damage; // per wave
var() float MomentumTransfer; // per wave
var() float DamageRadius; // of final wave
var() class<DamageType> DamageType;
var Vector BeamDirection;
var Vector DamageLocation;
var AvoidMarker Fear;

// camera shakes //
var() vector ShakeRotMag;           // how far to rot view
var() vector ShakeRotRate;          // how fast to rot view
var() float  ShakeRotTime;          // how much time to rot the instigator's view
var() vector ShakeOffsetMag;        // max view offset vertically
var() vector ShakeOffsetRate;       // how fast to offset view vertically
var() float  ShakeOffsetTime;       // how much time to offset view

function PostBeginPlay()
{
	local IonCannon C;
	
    Super.PostBeginPlay();
    if ( bDeleteMe )
		return;
		
	ForEach DynamicActors(Class'IonCannon',C)
	{
		if ( C != self )
			C.Destroy();
	}
}

function bool CheckMark(Pawn Aimer, Vector TestMark, bool bFire)
{
    return false;
}

auto state Ready
{
    function bool CheckMark(Pawn Aimer, Vector TestMark, bool bFire)
    {
        local Actor Other;
        local Vector HitLocation, HitNormal,Top;

        if (IsFiring())
            return false;
		
		Top = TestMark;
		Top.Z = Location.Z;
        Other = Trace(HitLocation, HitNormal, Top, TestMark, false);

        if ( Other != None )
            return false;

        if (bFire)
        {
            Instigator = Aimer;
            MarkLocation = TestMark;
            GotoState('FireSequence');
        }

        return true;
    }
}

function RemoveFear()
{
	if ( Fear != None )
		Fear.Destroy();
}

state FireSequence
{
	function RemoveFear();

	function bool IsFiring()
	{
		return true;
	}
	
    function BeginState()
    {
        BeamDirection = vect(0,0,-1);
        DamageLocation = MarkLocation - BeamDirection * 200.0;
    }

    function SpawnEffect()
    {
        local IonEffect IonBeamEffect;
        local Actor Other;
        local Vector HitLocation, HitNormal, Top, CP;

        Other = Trace(HitLocation, HitNormal, MarkLocation + vect(0,0,10000), MarkLocation, false);
 
		Top = MarkLocation;
		Top.Z = FMax(HitLocation.Z,Location.Z);

        IonBeamEffect = Spawn(IonEffectClass,,, Top);
        if (IonBeamEffect != None)
            IonBeamEffect.AimAt(MarkLocation, Vect(0,0,1));
         
        if ( Instigator != None ) 
			CP = Normal(vect(0,0,1) Cross (Location - Instigator.Location)); 
		else
			CP = vect(1,0,0);
			
		CP *= 0.5 * (Location.Z - MarkLocation.Z); 
		
        Other = Trace(HitLocation, HitNormal, Top + CP, MarkLocation, false);
        if ( Other == None )
        {
			IonBeamEffect = Spawn(IonEffectClass,,, Top + CP);
			if (IonBeamEffect != None)
				IonBeamEffect.AimAt(MarkLocation, Vect(0,0,1));
		}
		
        Other = Trace(HitLocation, HitNormal, Top - CP, MarkLocation, false);
        if ( Other == None )
        {
			IonBeamEffect = Spawn(IonEffectClass,,, Top - CP);
			if (IonBeamEffect != None)
				IonBeamEffect.AimAt(MarkLocation, Vect(0,0,1));
		}
    }
    
    function ShakeView()
    {
        local Controller C;
        local PlayerController PC;
        local float Dist, Scale;

        for ( C=Level.ControllerList; C!=None; C=C.NextController )
        {
            PC = PlayerController(C);
            if ( PC != None && PC.ViewTarget != None && PC.ViewTarget.Base != None )
            {
                Dist = VSize(DamageLocation - PC.ViewTarget.Location);
                if ( Dist < DamageRadius * 2.0)
                {
                    if (Dist < DamageRadius)
                        Scale = 1.0;
                    else
                        Scale = (DamageRadius*2.0 - Dist) / (DamageRadius);
                    C.ShakeView(ShakeRotMag*Scale, ShakeRotRate, ShakeRotTime, ShakeOffsetMag*Scale, ShakeOffsetRate, ShakeOffsetTime);
                }
            }
        }
    }

	function PlayGlobalSound(sound S)
	{
		local PlayerController P;

 		ForEach DynamicActors(class'PlayerController', P)
			P.ClientPlaySound(S);
	}	
		
    function EndState()
    {
		if ( (Instigator != None) && (Painter(Instigator.Weapon) != None) )
			Instigator.Weapon.CheckOutOfAmmo();

		if ( Fear != None )
			Fear.Destroy();
	}

Begin:
    if ( Fear == None )
		Fear = Spawn(class'AvoidMarker',,,MarkLocation);
    Fear.SetCollisionSize(DamageRadius,200); 
	if ( (Instigator != None) && (Instigator.PlayerReplicationInfo != None) && (Instigator.PlayerReplicationInfo.Team != None) )
		Fear.TeamNum = Instigator.PlayerReplicationInfo.Team.TeamIndex;
    Fear.StartleBots();
    Sleep(0.5);
    SpawnEffect();
	PlayGlobalSound(FireSound);
    Sleep(0.5);

    ShakeView();
    HurtRadius(Damage, DamageRadius*0.125, DamageType, MomentumTransfer, DamageLocation);
    Sleep(0.5);
	PlayGlobalSound(sound'WeaponSounds.redeemer_explosionsound');
    HurtRadius(Damage, DamageRadius*0.300, DamageType, MomentumTransfer, DamageLocation);
    Sleep(0.2);
    HurtRadius(Damage, DamageRadius*0.475, DamageType, MomentumTransfer, DamageLocation);
    Sleep(0.2);
    HurtRadius(Damage, DamageRadius*0.650, DamageType, MomentumTransfer, DamageLocation);
    Sleep(0.2);
    HurtRadius(Damage, DamageRadius*0.825, DamageType, MomentumTransfer, DamageLocation);
    Sleep(0.2);
    HurtRadius(Damage, DamageRadius*1.000, DamageType, MomentumTransfer, DamageLocation);
    GotoState('Ready');
}

function bool IsFiring()
{
    return false;
}

defaultproperties
{
    Physics=PHYS_None
    DrawType=DT_Mesh
    Mesh=Mesh'Weapons.IonCannon'
    bRotateToDesired=true
    RotationRate=(Pitch=15000,Yaw=15000,Roll=15000)
    bCollideActors=false
    bCollideWorld=false
    RemoteRole=ROLE_None

    FireSound=Sound'WeaponSounds.TAGRifle.IonCannonBlast'
    IonEffectClass=class'xEffects.IonEffect'
    Damage=150.0
    DamageRadius=2000.0
    MomentumTransfer=150000.0
    DamageType=class'DamTypeIonBlast'
    bStatic=false
    bStasis=false
    bHidden=true
    bHiddenEd=false
    TransientSoundVolume=1.0
    TransientSoundRadius=2000.0

    ShakeRotMag=(Z=250)
    ShakeRotRate=(Z=2500)
    ShakeRotTime=6
    ShakeOffsetMag=(Z=10)
    ShakeOffsetRate=(Z=200)
    ShakeOffsetTime=10
}

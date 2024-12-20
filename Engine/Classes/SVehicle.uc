//=============================================================================
// SVehicle
// karma physics subclass of vehicle, including networking support
//=============================================================================

class SVehicle extends Vehicle
	native
	abstract;

cpptext
{
#ifdef WITH_KARMA
	// Actor interface.
	virtual void PostBeginPlay();
	virtual void Destroy();
	virtual void PostNetReceive();
	virtual void PostNetReceiveLocation();
    virtual void PostEditChange();
	virtual void setPhysics(BYTE NewPhysics, AActor *NewFloor, FVector NewFloorV);
	virtual void TickSimulated( FLOAT DeltaSeconds );
	virtual void TickAuthoritative( FLOAT DeltaSeconds );
	virtual void physKarma(FLOAT DeltaTime);
	virtual void preContactUpdate();
	virtual void preKarmaStep(FLOAT DeltaTime);

	// SVehicle interface.
	virtual void UpdateVehicle(FLOAT DeltaTime);
#endif

}

var (SVehicle) editinline export	array<SVehicleWheel>		Wheels; // Wheel data

var	const bool			bVehicleOnGround; // OUTPUT: True if _any_ SVehicleWheel is currently touching the ground (ignores contacts with chassis etc)

//// PHYSICS ////
var (SVehicle) float	VehicleMass;
var (SVehicle) float	FlipTorque, FlipTime;
var float FlipScale, FlipTimeLeft;
var vector FlipAxis;

//// EFFECTS ////
var (SVehicle) class<Actor>	DestroyEffectClass;		// Effect spawned when vehicle is destroyed

// Useful function for plotting data to real-time graph on screen.
native final function GraphData(string DataName, float DataValue);

// You got some new info from the server (ie. VehicleState has some new info).
event VehicleStateReceived();

// Do script car model stuff here - but DONT create/destroy anything.
native event UpdateVehicle( float DeltaTime );

// Do any general vehicle set-up when it gets spawned.
simulated function PostNetBeginPlay()
{
    Super.PostNetBeginPlay();

	// Make sure params are up to date.
	SVehicleUpdateParams();
}

function JumpOffPawn() {}

// Called when a parameter of the overall articulated actor has changed (like PostEditChange)
// The script must then call KUpdateConstraintParams or Actor Karma mutators as appropriate.
simulated event SVehicleUpdateParams()
{
	KSetMass(VehicleMass);
}

function Flip(vector HitNormal, float ForceScale)
{
	if (!bCanFlip)
		return;

	FlipTimeLeft = FlipTime;
	FlipScale = ForceScale;
	FlipAxis = HitNormal;
	FlipAxis.Z = 0;
	enable('Tick');
}

simulated function KApplyForce(out vector Force, out vector Torque)
{
	local float torqueScale;
	local vector worldUp, torqueAxis;

	Super.KApplyForce(Force, Torque);

	if (FlipTimeLeft <= 0)
		return;

	worldUp = vect(0, 0, 1) >> Rotation;

	torqueAxis = Normal(FlipAxis Cross worldUp);

	// Torque scaled by how far over we are.
	// This will be between 0 and PI - so convert to between 0 and 1.
	torqueScale = Acos(worldUp Dot vect(0, 0, 1))/3.1416;

	Torque = FlipScale * FlipTorque * torqueScale * torqueAxis;
}

simulated function Tick(float deltaTime)
{
	Super.Tick(deltaTime);

	if (FlipTimeLeft > 0)
	{
		KWake();
		FlipTimeLeft -= deltaTime;
		if (FlipTimeLeft <= 0 && !bDriving)
			disable('Tick');
	}
}

simulated function Destroyed()
{
	// Trigger any effects for destruction
	if ( DestroyEffectClass != None )
		Spawn(DestroyEffectClass, , , Location, Rotation);

	super.Destroyed();
}

// Includes properties from KActor
defaultproperties
{
	VehicleMass=1.0
    Physics=PHYS_Karma
	bEdShouldSnap=True
	bStatic=False
	bShadowCast=False
	bCollideActors=True
	bCollideWorld=False
    bProjTarget=True
	bBlockActors=True
	bBlockNonZeroExtentTraces=True
	bBlockZeroExtentTraces=True
	bWorldGeometry=False
	bBlockKarma=True
	bAcceptsProjectors=True
	bCanBeBaseForPawns=True
	bAlwaysRelevant=false
	RemoteRole=ROLE_SimulatedProxy
	bNetInitialRotation=True
	bSpecialCalcView=True
	bDramaticLighting=True
	FlipTorque=300
	FlipTime=2
}

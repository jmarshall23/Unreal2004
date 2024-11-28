class ONSChopperCraft extends ONSVehicle
	abstract
	native
	nativereplication;

cpptext
{
	INT* GetOptimizedRepList(BYTE* Recent, FPropertyRetirement* Retire, INT* Ptr, UPackageMap* Map, UActorChannel* Channel);

#ifdef WITH_KARMA
	// Actor interface.
	virtual UBOOL Tick(FLOAT DeltaTime, enum ELevelTick TickType);
	virtual void PostNetReceive();

	// SVehicle interface.
	virtual void UpdateVehicle(FLOAT DeltaTime);

	// SHover interface.
	virtual void PackState();
#endif
}

var()	float				UprightStiffness;
var()	float				UprightDamping;

var()	float				MaxThrustForce;
var()	float				LongDamping;

var()	float				MaxStrafeForce;
var()	float				LatDamping;

var()	float				MaxRiseForce;
var()	float				UpDamping;

var()	float				TurnTorqueFactor;
var()	float				TurnTorqueMax;
var()	float				TurnDamping;
var()	float				MaxYawRate;

var()	float				PitchTorqueFactor;
var()	float				PitchTorqueMax;
var()	float				PitchDamping;

var()	float				RollTorqueTurnFactor;
var()	float				RollTorqueStrafeFactor;
var()	float				RollTorqueMax;
var()	float				RollDamping;

var()	float				StopThreshold;

var()   float               MaxRandForce;
var()   float               RandForceInterval;

// Internal
var		float				CopterMPH;

var		float				TargetHeading;
var		float				TargetPitch;
var     bool                bHeadingInitialized;

var		float				OutputThrust;
var		float				OutputStrafe;
var		float				OutputRise;

var     vector              RandForce;
var     vector              RandTorque;
var     float               AccumulatedTime;

// Replicated
struct native CopterState
{
	var vector				ChassisPosition;
	var Quat				ChassisQuaternion;
	var vector				ChassisLinVel;
	var vector				ChassisAngVel;

	var byte				ServerThrust;
	var	byte				ServerStrafe;
	var	byte				ServerRise;
	var int                 ServerViewPitch;
	var int                 ServerViewYaw;
};

var		CopterState			CopState, OldCopState;
var		KRigidBodyState		ChassisState;
var		bool				bNewCopterState;

replication
{
	reliable if (bNetDirty && Role == ROLE_Authority)
		CopState;
}

simulated event bool KUpdateState(out KRigidBodyState newState)
{
	// This should never get called on the server - but just in case!
	if(Role == ROLE_Authority || !bNewCopterState)
		return false;

	newState = ChassisState;
	bNewCopterState = false;

	return true;
	//return false;
}

simulated event SVehicleUpdateParams()
{
	Super.SVehicleUpdateParams();

	KSetStayUprightParams( UprightStiffness, UprightDamping );
}

simulated function SpecialCalcFirstPersonView(PlayerController PC, out actor ViewActor, out vector CameraLocation, out rotator CameraRotation )
{
	ViewActor = self;

	CameraLocation = Location + (FPCamPos >> Rotation);
}

defaultproperties
{
	bSpecialHUD=True

	bCanStrafe=true
	bCanFly=true
	bZeroPCRotOnEntry=False
	bFollowLookDir=true
	GroundSpeed=+1200.0
	bCanBeBaseForPawns=false
	bPCRelativeFPRotation=false
}

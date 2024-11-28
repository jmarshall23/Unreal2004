//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSGenericSD extends ONSWheeledCraft;

#exec OBJ LOAD FILE=..\sounds\ONSVehicleSounds-S.uax
#exec OBJ LOAD FILE=..\textures\ONSFullTextures.utx

var()	int		CloseSeatRotation;

simulated event Tick(float DeltaSeconds)
{
	local rotator SeatRot;

	Super.Tick(DeltaSeconds);

	if(!bDriving)
		SeatRot.Pitch = CloseSeatRotation;

	SetBoneRotation('Object05', SeatRot, 0, 1.0);
}

static function StaticPrecache(LevelInfo L)
{
    Super.StaticPrecache(L);

	L.AddPrecacheStaticMesh(StaticMesh'ONSDeadVehicles-SM.RVexploded.RVgun');
	L.AddPrecacheStaticMesh(StaticMesh'ONSDeadVehicles-SM.RVexploded.RVrail');
	L.AddPrecacheStaticMesh(StaticMesh'ONSDeadVehicles-SM.RVexploded.Rvtire');
	L.AddPrecacheStaticMesh(StaticMesh'AW-2004Particles.Debris.Veh_Debris2');
	L.AddPrecacheStaticMesh(StaticMesh'AW-2004Particles.Debris.Veh_Debris1');

    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp2_frames');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp1_frames');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.we1_frames');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.SmokeReOrdered');
    L.AddPrecacheMaterial(Material'AW-2004Particles.Fire.NapalmSpot');
    L.AddPrecacheMaterial(Material'EpicParticles.Fire.SprayFire1');
    L.AddPrecacheMaterial(Material'Engine.GRADIENT_Fade');
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh(StaticMesh'ONSDeadVehicles-SM.RVexploded.RVgun');
	Level.AddPrecacheStaticMesh(StaticMesh'ONSDeadVehicles-SM.RVexploded.RVrail');
	Level.AddPrecacheStaticMesh(StaticMesh'ONSDeadVehicles-SM.RVexploded.Rvtire');
	Level.AddPrecacheStaticMesh(StaticMesh'AW-2004Particles.Debris.Veh_Debris2');
	Level.AddPrecacheStaticMesh(StaticMesh'AW-2004Particles.Debris.Veh_Debris1');

    Super.UpdatePrecacheStaticMeshes();
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp2_frames');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp1_frames');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.we1_frames');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.SmokeReOrdered');
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Fire.NapalmSpot');
    Level.AddPrecacheMaterial(Material'EpicParticles.Fire.SprayFire1');
    Level.AddPrecacheMaterial(Material'Engine.GRADIENT_Fade');

	Super.UpdatePrecacheMaterials();
}

defaultproperties
{
	CloseSeatRotation=16384

	Mesh=Mesh'GenericSD.TC'
	VehiclePositionString="indisposed"
	VehicleNameString="TC-1200"
	bReplicateAnimations=True

    RedSkin=Texture'GenericSD.ToiletCar'
    BlueSkin=Texture'GenericSD.ToiletCarBlue'

    DestroyedVehicleMesh=StaticMesh'GenericSD.TCDead'
	DestructionEffectClass=class'Onslaught.ONSSmallVehicleExplosionEffect'
	DisintegrationEffectClass=class'Onslaught.ONSSmallVehicleExplosionEffect'
    DestructionLinearMomentum=(Min=200000,Max=300000)
    DestructionAngularMomentum=(Min=100,Max=150)
    DisintegrationHealth=-25
	ImpactDamageMult=0.0010

	Health=300
	HealthMax=300
	CollisionHeight=+40.0
	CollisionRadius=+100.0
	DriverDamageMult=0.80
	bHasAltFire=false

	DrawScale=1.0
	DrawScale3D=(X=1.0,Y=1.0,Z=1.0)

	FPCamPos=(X=7,Y=0,Z=52)
	TPCamLookat=(X=0,Y=0,Z=0)

	TPCamWorldOffset=(X=0,Y=0,Z=100)
	TPCamDistance=375

	bDoStuntInfo=true
	DaredevilThreshInAirSpin=180.0
	DaredevilThreshInAirPitch=300.0
	DaredevilThreshInAirRoll=300.0
	DaredevilThreshInAirTime=2.0
	DaredevilThreshInAirDistance=24.0

	AirTurnTorque=35.0
	AirPitchTorque=55.0
	AirPitchDamping=35.0
	AirRollTorque=35.0
	AirRollDamping=35.0

	bDrawDriverInTP=True
	bDrawMeshInFP=True
	bHasHandbrake=true
	bAllowBigWheels=true

	DrivePos=(X=9.0,Y=0.0,Z=70.0)

	MaxViewYaw=16000
	MaxViewPitch=16000

	IdleSound=sound'ONSVehicleSounds-S.RV.RVEng01'
	StartUpSound=sound'ONSVehicleSounds-S.RV.RVStart01'
	ShutDownSound=sound'ONSVehicleSounds-S.Laser24'
	EngineRPMSoundRange=9000
	SoundVolume=128
	IdleRPM=500
	RevMeterScale=4000

	StartUpForce="RVStartUp"

	//SteerBoneName="SteeringWheel"
	//SteerBoneAxis=AXIS_Z
	//SteerBoneMaxAngle=90

	EntryPosition=(X=0,Y=0,Z=0)
	EntryRadius=160.0

	ExitPositions(0)=(X=0,Y=-165,Z=100)
	ExitPositions(1)=(X=0,Y=165,Z=100)
	ExitPositions(2)=(X=0,Y=-165,Z=-100)
	ExitPositions(3)=(X=0,Y=165,Z=-100)

	bMakeBrakeLights=false

	DamagedEffectOffset=(X=-32,Y=0,Z=20)
	DamagedEffectScale=1.0

	WheelPenScale=1.0
	WheelPenOffset=0.01
	WheelSoftness=0.015
	WheelRestitution=0.1
	WheelAdhesion=0.0
	WheelLongFrictionFunc=(Points=((InVal=0,OutVal=0.0),(InVal=100.0,OutVal=1.0),(InVal=200.0,OutVal=0.9),(InVal=10000000000.0,OutVal=0.9)))
	WheelLongFrictionScale=0.9
	WheelLatFrictionScale=1.35
	WheelLongSlip=0.001
	WheelLatSlipFunc=(Points=((InVal=0.0,OutVal=0.0),(InVal=30.0,OutVal=0.009),(InVal=45.0,OutVal=0.00),(InVal=10000000000.0,OutVal=0.00)))
	WheelHandbrakeSlip=0.01
	WheelHandbrakeFriction=0.1
	WheelSuspensionTravel=15.0
	WheelSuspensionOffset=-3.0
	WheelSuspensionMaxRenderTravel=15.0
	TurnDamping=35

	HandbrakeThresh=200
	FTScale=0.03
	ChassisTorqueScale=0.1

	MinBrakeFriction=1.5
	MaxBrakeTorque=20.0
	MaxSteerAngleCurve=(Points=((InVal=0,OutVal=25.0),(InVal=1500.0,OutVal=11.0),(InVal=1000000000.0,OutVal=11.0)))
	SteerSpeed=160
	StopThreshold=100
	TorqueCurve=(Points=((InVal=0,OutVal=9.0),(InVal=200,OutVal=10.0),(InVal=1500,OutVal=11.0),(InVal=2800,OutVal=0.0)))
	EngineBrakeFactor=0.0001
	EngineBrakeRPMScale=0.1
	EngineInertia=0.1
	WheelInertia=0.1

	TransRatio=0.15
	GearRatios[0]=-0.5
	GearRatios[1]=0.4
	GearRatios[2]=0.65
	GearRatios[3]=0.85
	GearRatios[4]=1.1
	ChangeUpPoint=2000
	ChangeDownPoint=1000
	LSDFactor=1.0

	VehicleMass=3.5

    Begin Object Class=KarmaParamsRBFull Name=KParams0
		KStartEnabled=True
		KFriction=0.5
		KLinearDamping=0.05
		KAngularDamping=0.05
		KImpactThreshold=700
		bKNonSphericalInertia=True
        bHighDetailOnly=False
        bClientOnly=False
		bKDoubleTickRate=True
		KInertiaTensor(0)=1.0
		KInertiaTensor(1)=0.0
		KInertiaTensor(2)=0.0
		KInertiaTensor(3)=3.0
		KInertiaTensor(4)=0.0
		KInertiaTensor(5)=3.0
		KCOMOffset=(X=0.0,Y=0.0,Z=0.0)
		bDestroyOnWorldPenetrate=True
		bDoSafetime=True
        Name="KParams0"
    End Object
    KParams=KarmaParams'KParams0'

	Begin Object Class=SVehicleWheel Name=RRWheel
		BoneName="Object02"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=7.0,Z=0.0)
		WheelRadius=13
		bPoweredWheel=True
		bHandbrakeWheel=True
		SteerType=VST_Fixed
		//SupportBoneAxis=AXIS_X
		//SupportBoneName="RRearStrut"
	End Object
	Wheels(0)=SVehicleWheel'RRWheel'

	Begin Object Class=SVehicleWheel Name=LRWheel
		BoneName="Object07"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=-7.0,Z=0.0)
		WheelRadius=13
		bPoweredWheel=True
		bHandbrakeWheel=True
		SteerType=VST_Fixed
		//SupportBoneAxis=AXIS_X
		//SupportBoneName="LRearStrut"
	End Object
	Wheels(1)=SVehicleWheel'LRWheel'

	Begin Object Class=SVehicleWheel Name=RFWheel
		BoneName="Object03"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=7.0,Z=0.0)
		WheelRadius=10
		bPoweredWheel=True
		SteerType=VST_Steered
		//SupportBoneAxis=AXIS_X
		//SupportBoneName="RFrontStrut"
	End Object
	Wheels(2)=SVehicleWheel'RFWheel'

	Begin Object Class=SVehicleWheel Name=LFWheel
		BoneName="Object08"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=-7.0,Z=0.0)
		WheelRadius=10
		bPoweredWheel=True
		SteerType=VST_Steered
		//SupportBoneAxis=AXIS_X
		//SupportBoneName="LFrontStrut"
	End Object
	Wheels(3)=SVehicleWheel'LFWheel'

	GroundSpeed=940
	CenterSpringForce="SpringONSSRV"

	HornSounds(0)=sound'ONSVehicleSounds-S.Horn04'
	HornSounds(1)=sound'ONSVehicleSounds-S.Horn05'
}

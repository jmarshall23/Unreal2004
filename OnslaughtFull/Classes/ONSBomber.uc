//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSBomber extends ONSPlaneCraft;

#exec OBJ LOAD FILE=..\Animations\ONSVehicles-A.ukx

var()   float   MaxPitchSpeed;

simulated event DrivingStatusChanged()
{
    if (bDriving)
        Enable('Tick');
    else
        Disable('Tick');
}

simulated function Tick(float DeltaTime)
{
    local float EnginePitch;

    if(Level.NetMode != NM_DedicatedServer)
	{
        EnginePitch = 96.0 + VSize(Velocity)/MaxPitchSpeed * 32.0;
        SoundPitch = FClamp(EnginePitch, 96, 128);
    }

    Super.Tick(DeltaTime);
}

defaultproperties
{
	Mesh=Mesh'ONSFullAnimations.Bomber'

    RedSkin=Shader'ONSFullTextures.BomberGroup.BomberChassisFinalRED'
    BlueSkin=Shader'ONSFullTextures.BomberGroup.BomberChassisFinalBLUE'

	DriverWeapons(0)=(WeaponClass=class'OnslaughtFull.ONSBombDropper',WeaponBone=FrontGunMount);

	Health=600
	HealthMax=600

	IdleSound=sound'ONSVehicleSounds-S.Flying.Flying02'
//	StartUpSound=sound'ONSVehicleSounds-S.AttackCraft.AttackCraftStartUp'
//	ShutDownSound=sound'ONSVehicleSounds-S.AttackCraft.AttackCraftShutDown'
	MaxPitchSpeed=3200
	SoundVolume=255

    Begin Object Class=KarmaParamsRBFull Name=KParams0
        KInertiaTensor(0)=3.5
        KInertiaTensor(1)=0.0
        KInertiaTensor(2)=0.0
        KInertiaTensor(3)=10
        KInertiaTensor(4)=0.0
        KInertiaTensor(5)=13
		KCOMOffset=(X=0.65,Y=0.0,Z=0.0)
		KStartEnabled=True
		KFriction=0.6
		KLinearDamping=0.0
		KAngularDamping=1.5
		bKNonSphericalInertia=True
        bHighDetailOnly=False
        bClientOnly=False
		bKDoubleTickRate=True
		KActorGravScale=2.0
        KMaxSpeed=4000.0
        KImpactThreshold=300
		bDestroyOnWorldPenetrate=True
		bDoSafetime=True
        Name="KParams0"
    End Object
    KParams=KarmaParams'KParams0'

    DrawScale=0.7

    VehicleMass=4

    bDrawDriverInTP=False
	bDrawMeshInFP=True

	LiftCoefficientCurve=(Points=((InVal=-180,OutVal=0.0),(InVal=-10.0,OutVal=0.0),(InVal=0.0,OutVal=0.4),(InVal=6.0,OutVal=0.8),(InVal=10.0,OutVal=1.2),(InVal=12.0,OutVal=1.4),(InVal=20.0,OutVal=0.8),(InVal=60.0,OutVal=0.6),(InVal=90.0,OutVal=0.0),(InVal=180.0,OutVal=0.0)))
	DragCoefficientCurve=(Points=((InVal=-180,OutVal=0.0),(InVal=-90.0,OutVal=1.2),(InVal=-10.0,OutVal=0.1),(InVal=-5.0,OutVal=0.35),(InVal=0.0,OutVal=0.01),(InVal=5.0,OutVal=0.35),(InVal=10.0,OutVal=0.1),(InVal=15.0,OutVal=0.3),(InVal=60.0,OutVal=1.0),(InVal=90.0,OutVal=1.2),(InVal=180.0,OutVal=0.0)))
    AirFactor=0.00005

	HoverForceCurve=(Points=((InVal=0,OutVal=500),(InVal=30,OutVal=700),(InVal=250,OutVal=0)))
	COMHeight=20.0
	MaxThrust=110.0
    PitchTorque=700.0
    BankTorque=700.0
    ThrustAcceleration=60.0
    bHoverOnGround=True

    ImpactDamageThreshold=1000
    ImpactDamageMult=0.03

    MomentumMult=0.05

    HoverSoftness=0.9
	HoverPenScale=1.5
	HoverCheckDist=500

	ThrusterOffsets(0)=(X=200,Y=0,Z=10)
	ThrusterOffsets(1)=(X=-50,Y=300,Z=10)
	ThrusterOffsets(2)=(X=-50,Y=-300,Z=10)

    ExitPositions(0)=(X=0,Y=500,Z=100)
    ExitPositions(1)=(X=0,Y=-500,Z=100)
	ExitPositions(2)=(X=350,Y=0,Z=100)
	ExitPositions(3)=(X=-350,Y=0,Z=100)
	VehiclePositionString="in a DragonFly"
	VehicleNameString="DragonFly"
}

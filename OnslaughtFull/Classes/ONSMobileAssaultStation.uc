//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSMobileAssaultStation extends ONSWheeledCraft;

#exec OBJ LOAD FILE=..\Sounds\MenuSounds.uax
#exec OBJ LOAD FILE=..\Textures\ONSFullTextures.utx

var()       sound   DeploySound;
var()       sound   HideSound;
var()		string	DeployForce;
var()		string	HideForce;
var         EPhysics    ServerPhysics;

var			bool	bDeployed;
var			bool	bOldDeployed;

var			vector  UnDeployedTPCamLookat;
var			vector  UnDeployedTPCamWorldOffset;
var			vector  DeployedTPCamLookat;
var			vector  DeployedTPCamWorldOffset;

var			vector  UnDeployedFPCamPos;
var			vector  DeployedFPCamPos;

replication
{
	unreliable if(Role==ROLE_Authority)
        ServerPhysics, bDeployed;
}

simulated event PostNetReceive()
{
    Super.PostNetReceive();

    if (ServerPhysics != Physics)
    {
        SetPhysics(ServerPhysics);
    }

	if( bDeployed != bOldDeployed )
	{
		if(bDeployed)
		{
			TPCamLookat = DeployedTPCamLookat;
			TPCamWorldOffset = DeployedTPCamWorldOffset;
			FPCamPos = DeployedFPCamPos;
			bEnableProximityViewShake = False;
		}
		else
		{
			TPCamLookat = UnDeployedTPCamLookat;
			TPCamWorldOffset = UnDeployedTPCamWorldOffset;
			FPCamPos = UnDeployedFPCamPos;
			bEnableProximityViewShake = True;
		}

		bOldDeployed = bDeployed;
	}
}

function VehicleFire(bool bWasAltFire)
{
	if (bWasAltFire && PlayerController(Controller) != None)
		PlayerController(Controller).ClientPlaySound(sound'MenuSounds.Denied1');
}

function ChooseFireAt(Actor A)
{
	local Bot B;

	B = Bot(Controller);
	if ( B == None || B.Squad == None || B.Squad.SquadObjective == None
	     || (IsInState('UnDeployed') != B.LineOfSightTo(B.Squad.SquadObjective)) )
		Fire(0);
	else
		AltFire(0);
}

auto state UnDeployed
{
    function VehicleFire(bool bWasAltFire)
    {
    	if (bWasAltFire)
    	{
            if (PlayerController(Controller) != None && VSize(Velocity) > 15.0)
                PlayerController(Controller).ClientPlaySound(sound'MenuSounds.Denied1');
            else
                GotoState('Deploying');
        }
    	else
    		bWeaponIsFiring = True;
    }
}

state Deployed
{
    function VehicleFire(bool bWasAltFire)
    {
    	if (bWasAltFire)
            GotoState('UnDeploying');
    	else
    		bWeaponIsFiring = True;
    }
}

state UnDeploying
{
Begin:
    if (Controller != None)
    {
    	if (PlayerController(Controller) != None)
    	{
	        PlayerController(Controller).ClientPlaySound(HideSound);
        	if (PlayerController(Controller).bEnableGUIForceFeedback)
			PlayerController(Controller).ClientPlayForceFeedback(HideForce);
	}
        Weapons[1].bForceCenterAim = True;
        Weapons[1].PlayAnim('MASMainGunHide');
        sleep(2.3);
        PlayAnim('MASMainGunHide');
        sleep(4.03);
    	SetPhysics(PHYS_Karma);
    	ServerPhysics = PHYS_Karma;
    	bStationary = false;
    	SetActiveWeapon(0);
	TPCamLookat = UnDeployedTPCamLookat;
	TPCamWorldOffset = UnDeployedTPCamWorldOffset;
	FPCamPos = UnDeployedFPCamPos;
	bEnableProximityViewShake = True;
	bDeployed = False;
        GotoState('UnDeployed');
    }
}

state Deploying
{
Begin:
    if (Controller != None)
    {
    	SetPhysics(PHYS_None);
    	ServerPhysics = PHYS_None;
    	bStationary = true;
    	if (PlayerController(Controller) != None)
    	{
	        PlayerController(Controller).ClientPlaySound(DeploySound);
        	if (PlayerController(Controller).bEnableGUIForceFeedback)
			PlayerController(Controller).ClientPlayForceFeedback(DeployForce);
	}
        PlayAnim('MASMainGunDeploy');
        sleep(3.46);
        Weapons[1].PlayAnim('MASMainGunDeploy');
        sleep(2.873);
        Weapons[1].bForceCenterAim = False;
        SetActiveWeapon(1);
	bWeaponisFiring = false; //so bots don't immediately fire until the gun has a chance to move
	TPCamLookat = DeployedTPCamLookat;
	TPCamWorldOffset = DeployedTPCamWorldOffset;
	FPCamPos = DeployedFPCamPos;
	bEnableProximityViewShake = False;
	bDeployed = True;
        GotoState('Deployed');
    }
}

function Died(Controller Killer, class<DamageType> damageType, vector HitLocation)
{
    SetPhysics(PHYS_Karma);

    Super.Died(Killer, damageType, HitLocation);
}

simulated event ClientVehicleExplosion(bool bFinal)
{
	local int SoundNum;
    local Actor DestructionEffect;

    // Explosion effect
	if(ExplosionSounds.Length > 0)
	{
		SoundNum = Rand(ExplosionSounds.Length);
		PlaySound(ExplosionSounds[SoundNum], SLOT_None, ExplosionSoundVolume*TransientSoundVolume,, ExplosionSoundRadius);
	}

	if (bFinal)
    {
        if (Level.NetMode != NM_DedicatedServer)
            DestructionEffect = spawn(DisintegrationEffectClass,,, Location, Rotation);

        GotoState('VehicleDisintegrated');
    }
}

state VehicleDisintegrated
{
    function Died(Controller Killer, class<DamageType> damageType, vector HitLocation)
    {
    }

Begin:
    sleep(0.75);
    Destroy();
}

static function StaticPrecache(LevelInfo L)
{
    Super.StaticPrecache(L);

	L.AddPrecacheStaticMesh(StaticMesh'ParticleMeshes.Complex.ExplosionRing');
	L.AddPrecacheStaticMesh(StaticMesh'ONSFullStaticMeshes.LEVexploded.BayDoor');
	L.AddPrecacheStaticMesh(StaticMesh'ONSFullStaticMeshes.LEVexploded.MainGun');
	L.AddPrecacheStaticMesh(StaticMesh'ONSFullStaticMeshes.LEVexploded.SideFlap');
	L.AddPrecacheStaticMesh(StaticMesh'AW-2004Particles.Debris.Veh_Debris1');

    L.AddPrecacheMaterial(Material'AW-2004Particles.Energy.SparkHead');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp2_frames');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp1_frames');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.we1_frames');
    L.AddPrecacheMaterial(Material'AW-2004Particles.Fire.NapalmSpot');
    L.AddPrecacheMaterial(Material'EpicParticles.Fire.SprayFire1');
    L.AddPrecacheMaterial(Material'ONSFullTextures.MASGroup.LEVcolorRED');
    L.AddPrecacheMaterial(Material'ONSFullTextures.MASGroup.LEVnoColor');
    L.AddPrecacheMaterial(Material'ONSFullTextures.MASGroup.LEVcolorBlue');
    L.AddPrecacheMaterial(Material'VehicleFX.Particles.DustyCloud2');
    L.AddPrecacheMaterial(Material'VMParticleTextures.DirtKICKGROUP.dirtKICKTEX');
    L.AddPrecacheMaterial(Material'Engine.GRADIENT_Fade');
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh(StaticMesh'ParticleMeshes.Complex.ExplosionRing');
	Level.AddPrecacheStaticMesh(StaticMesh'ONSFullStaticMeshes.LEVexploded.BayDoor');
	Level.AddPrecacheStaticMesh(StaticMesh'ONSFullStaticMeshes.LEVexploded.MainGun');
	Level.AddPrecacheStaticMesh(StaticMesh'ONSFullStaticMeshes.LEVexploded.SideFlap');
	Level.AddPrecacheStaticMesh(StaticMesh'AW-2004Particles.Debris.Veh_Debris1');

    Super.UpdatePrecacheStaticMeshes();
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Energy.SparkHead');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp2_frames');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp1_frames');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.we1_frames');
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Fire.NapalmSpot');
    Level.AddPrecacheMaterial(Material'EpicParticles.Fire.SprayFire1');
    Level.AddPrecacheMaterial(Material'ONSFullTextures.MASGroup.LEVcolorRED');
    Level.AddPrecacheMaterial(Material'ONSFullTextures.MASGroup.LEVnoColor');
    Level.AddPrecacheMaterial(Material'ONSFullTextures.MASGroup.LEVcolorBlue');
    Level.AddPrecacheMaterial(Material'VehicleFX.Particles.DustyCloud2');
    Level.AddPrecacheMaterial(Material'VMParticleTextures.DirtKICKGROUP.dirtKICKTEX');
    Level.AddPrecacheMaterial(Material'Engine.GRADIENT_Fade');

	Super.UpdatePrecacheMaterials();
}

DefaultProperties
{
    bReplicateAnimations=True
    ServerPhysics=PHYS_Karma
    bNetNotify=True
	VehiclePositionString="in a Leviathan"
	VehicleNameString="Leviathan"

	Mesh=Mesh'ONSFullAnimations.MASchassis'

    DriverWeapons(0)=(WeaponClass=class'OnslaughtFull.ONSMASRocketPack',WeaponBone=RocketPackAttach);
    DriverWeapons(1)=(WeaponClass=class'OnslaughtFull.ONSMASCannon',WeaponBone=MainGunPostBase);
	PassengerWeapons(0)=(WeaponPawnClass=class'OnslaughtFull.ONSMASSideGunPawn',WeaponBone=RightFrontGunAttach);
	PassengerWeapons(1)=(WeaponPawnClass=class'OnslaughtFull.ONSMASSideGunPawn',WeaponBone=LeftFrontGunAttach);
	PassengerWeapons(2)=(WeaponPawnClass=class'OnslaughtFull.ONSMASSideGunPawn',WeaponBone=RightRearGunAttach);
	PassengerWeapons(3)=(WeaponPawnClass=class'OnslaughtFull.ONSMASSideGunPawn',WeaponBone=LeftRearGunAttach);

	DestroyedVehicleMesh=StaticMesh'ONSFullStaticMeshes.LeviathanDead'
    DestructionEffectClass=class'Onslaught.ONSVehicleExplosionEffect'
	DisintegrationEffectClass=class'OnslaughtFull.ONSVehDeathMAS'
    DestructionLinearMomentum=(Min=250000,Max=400000)
    DestructionAngularMomentum=(Min=100,Max=300)

    RedSkin=Shader'ONSFullTextures.MASGroup.MASRedShad'
    BlueSkin=Shader'ONSFullTextures.MASGroup.MASBlueShad'
	ShadowCullDistance=2000.0

	Health=5000
	HealthMax=5000
    DisintegrationHealth=0
	DriverDamageMult=0.0
	MomentumMult=0.01
	RanOverDamageType=class'DamTypeMASRoadkill'
	CrushedDamageType=class'DamTypeMASPancake'

	FPCamPos=(X=-240,Y=0,Z=350)

	UnDeployedFPCamPos=(X=-240,Y=0,Z=350)
	DeployedFPCamPos=(X=0,Y=0,Z=600)

	TPCamLookat=(X=-200,Y=0,Z=300)
	TPCamWorldOffset=(X=0,Y=0,Z=200)
	bDeployed=false

	UnDeployedTPCamLookat=(X=-200,Y=0,Z=300)
	UnDeployedTPCamWorldOffset=(X=0,Y=0,Z=200)

	DeployedTPCamLookat=(X=100,Y=0,Z=0)
	DeployedTPCamWorldOffset=(X=0,Y=0,Z=800)

	TPCamDistance=780
	TPCamDistRange=(Min=0,Max=2500)

	CollisionHeight=+60.0
	CollisionRadius=+260.0

	bDrawDriverInTP=False
	bDrawMeshInFP=True
	bAllowBigWheels=true
	bHasAltFire=false

	MaxViewYaw=16000
	MaxViewPitch=30000

	DrivePos=(X=16.921,Y=-40.284,Z=65.794)
	DriveRot=(Pitch=0)

	DeploySound=sound'ONSVehicleSounds-S.MAS.MASDeploy01'
	HideSound=sound'ONSVehicleSounds-S.MAS.MASDeploy01'

	IdleSound=sound'ONSVehicleSounds-S.MAS.MASEng01'
	StartUpSound=sound'ONSVehicleSounds-S.MAS.MASStart01'
	ShutDownSound=sound'ONSVehicleSounds-S.MAS.MASStop01'
	EngineRPMSoundRange=8000
    SoundRadius=255
	SoundVolume=255
	IdleRPM=1000
	RevMeterScale=4000

	StartUpForce="MASStartUp"
	ShutDownForce="MASShutDown"
	DeployForce="MASDeploy"
	HideForce="MASDeploy"

	SteerBoneName=""
	SteerBoneAxis=AXIS_Z
	SteerBoneMaxAngle=90

	EntryPosition=(X=0,Y=0,Z=0)
	EntryRadius=500.0

	ExitPositions(0)=(X=0,Y=-365,Z=200)
	ExitPositions(1)=(X=0,Y=365,Z=200)
	ExitPositions(2)=(X=0,Y=-365,Z=-100)
	ExitPositions(3)=(X=0,Y=365,Z=-100)

	WheelPenScale=1.0
	WheelPenOffset=0.01
	WheelSoftness=0.04
	WheelRestitution=0.1
	WheelAdhesion=0.0
	WheelLongFrictionFunc=(Points=((InVal=0,OutVal=0.0),(InVal=100.0,OutVal=1.0),(InVal=200.0,OutVal=0.9),(InVal=10000000000.0,OutVal=0.9)))
	WheelLongFrictionScale=0.9
	WheelLatFrictionScale=1.5
	WheelLongSlip=0.001
	WheelLatSlipFunc=(Points=((InVal=0.0,OutVal=0.0),(InVal=30.0,OutVal=0.009),(InVal=45.0,OutVal=0.00),(InVal=10000000000.0,OutVal=0.00)))
	WheelHandbrakeSlip=0.01
	WheelHandbrakeFriction=0.1
	WheelSuspensionTravel=40.0
	WheelSuspensionOffset=0.0
	WheelSuspensionMaxRenderTravel=40.0

	HandbrakeThresh=200
	FTScale=0.01
	ChassisTorqueScale=0.1

	MinBrakeFriction=4.0
	MaxBrakeTorque=20.0
	MaxSteerAngleCurve=(Points=((InVal=0,OutVal=35.0),(InVal=1500.0,OutVal=25.0),(InVal=1000000000.0,OutVal=20.0)))
	SteerSpeed=110
	StopThreshold=100
	TorqueCurve=(Points=((InVal=0,OutVal=36.0),(InVal=200,OutVal=4.0),(InVal=1500,OutVal=5.5),(InVal=2500,OutVal=0.0)))
	EngineBrakeFactor=0.002
	EngineBrakeRPMScale=0.1
	EngineInertia=0.5
	WheelInertia=0.01

	TransRatio=0.11
//	TransRatio=0.01
	GearRatios[0]=-0.2
	GearRatios[1]=0.2
	NumForwardGears=1
	ChangeUpPoint=2000
	ChangeDownPoint=1000
	LSDFactor=1.0

//	VehicleMass=25.0
	VehicleMass=10.0

	HeadlightCoronaOffset(0)=(X=365,Y=-87,Z=130)
	HeadlightCoronaMaterial=Material'EpicParticles.flashflare1'
	HeadlightCoronaMaxSize=120

	DamagedEffectOffset=(X=300,Y=0,Z=185)
	DamagedEffectScale=2.5

    Begin Object Class=KarmaParamsRBFull Name=KParams0
		KStartEnabled=True
		KFriction=0.5
		KLinearDamping=0.05
		KAngularDamping=0.05
		KImpactThreshold=500
		bKNonSphericalInertia=True
        bHighDetailOnly=False
        bClientOnly=False
		bKDoubleTickRate=True
//        KInertiaTensor(0)=13.260000
//        KInertiaTensor(0)=13.260000
//    	KInertiaTensor(1)=0
//    	KInertiaTensor(2)=0
//    	KInertiaTensor(3)=35.099998
//    	KInertiaTensor(4)=0
//    	KInertiaTensor(5)=43.499996

      KInertiaTensor(0)=1.260000
    	KInertiaTensor(1)=0
    	KInertiaTensor(2)=0
    	KInertiaTensor(3)=3.099998
    	KInertiaTensor(4)=0
    	KInertiaTensor(5)=4.499996

    	KCOMOffset=(X=0,Y=0,Z=0)
		bDestroyOnWorldPenetrate=True
		bDoSafetime=True
        Name="KParams0"
    End Object
    KParams=KarmaParams'KParams0'

	Begin Object Class=SVehicleWheel Name=RightRearTIRE
		BoneName="RightRearTire"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=0.0,Z=0.0)
		WheelRadius=99
		bPoweredWheel=True
		bHandbrakeWheel=True
		SteerType=VST_Fixed
//		SupportBoneName="RightRearStrut"
	End Object
	Wheels(0)=SVehicleWheel'RightRearTIRE'

	Begin Object Class=SVehicleWheel Name=LeftRearTIRE
		BoneName="LeftRearTire"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=0.0,Z=0.0)
		WheelRadius=99
		bPoweredWheel=True
		bHandbrakeWheel=True
		SteerType=VST_Fixed
//		SupportBoneName="LeftRearStrut"
	End Object
	Wheels(1)=SVehicleWheel'LeftRearTIRE'

	Begin Object Class=SVehicleWheel Name=RightFrontTIRE
		BoneName="RightFrontTire"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=0.0,Z=0.0)
		WheelRadius=99
		bPoweredWheel=True
		SteerType=VST_Steered
//		SupportBoneName="RightFrontStrut"
	End Object
	Wheels(2)=SVehicleWheel'RightFrontTIRE'

	Begin Object Class=SVehicleWheel Name=LeftFrontTIRE
		BoneName="LeftFrontTire"
		BoneRollAxis=AXIS_Y
		BoneSteerAxis=AXIS_Z
		BoneOffset=(X=0.0,Y=0.0,Z=0.0)
		WheelRadius=99
		bPoweredWheel=True
		SteerType=VST_Steered
//		SupportBoneName="LeftFrontStrut"
	End Object
	Wheels(3)=SVehicleWheel'LeftFrontTIRE'

	bDriverHoldsFlag=false
	FlagBone=LeftFrontGunAttach
	bKeyVehicle=true
	bNeverReset=true

	bEnableProximityViewShake=true
	bOnlyViewShakeIfDriven=true
	ViewShakeRadius=1000.0
	ViewShakeOffsetMag=(X=0.7,Y=0.0,Z=2.7)
	ViewShakeOffsetFreq=7.0

	HornSounds(0)=sound'ONSVehicleSounds-S.LevHorn01'
	HornSounds(1)=sound'ONSVehicleSounds-S.LevHorn02'

	MaxDesireability=2.0
	ObjectiveGetOutDist=2000.0
}

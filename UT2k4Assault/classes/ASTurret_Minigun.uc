//=============================================================================
// ASTurret_Minigun
//=============================================================================

class ASTurret_Minigun extends ASTurret;

function Pawn CheckForHeadShot(Vector loc, Vector ray, float AdditionalScale)
{
    local vector	X, Y, Z, newray;
	local float		Angle, Side;

    GetAxes(Rotation, X, Y, Z);

    if ( Driver != None )
    {
        // Remove the Z component of the ray
        newray = ray;
        newray.Z = 0;

		Angle = abs(newray dot X);
		Side = NewRay dot Y;
        if (  Angle < 0.7 && Side < 0 && Driver.IsHeadShot(loc, ray, AdditionalScale) )			// left
            return Driver;
        else if (  Angle < 0.82 && Side > 0 && Driver.IsHeadShot(loc, ray, AdditionalScale) )	// right
            return Driver;
    }

    return None;
}


static function StaticPrecache(LevelInfo L)
{
    super.StaticPrecache( L );

	L.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ASMinigun1' );		// Skins
	L.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ASMinigun2' );
	L.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ChaingunTop' );
	//L.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ChaingunTopALPHA' );

	L.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp7_frames' );			// Explosion Effect
	L.AddPrecacheMaterial( Material'EpicParticles.Flares.SoftFlare' );
	L.AddPrecacheMaterial( Material'AW-2004Particles.Fire.MuchSmoke2t' );
	L.AddPrecacheMaterial( Material'AS_FX_TX.Trails.Trail_red' );
	L.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp1_frames' );
	L.AddPrecacheMaterial( Material'EmitterTextures.MultiFrame.rockchunks02' );

	L.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.ASMinigun_Base' );
	L.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.ASMinigun_Swivel' );
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.ASMinigun_Base' );
	Level.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.ASMinigun_Swivel' );

	super.UpdatePrecacheStaticMeshes();
}


simulated function UpdatePrecacheMaterials()
{
	Level.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ASMinigun1' );		// Skins
	Level.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ASMinigun2' );
	Level.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ChaingunTop' );
	//Level.AddPrecacheMaterial( Material'AS_Weapons_TX.Minigun.ChaingunTopALPHA' );

	Level.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp7_frames' );			// Explosion Effect
	Level.AddPrecacheMaterial( Material'EpicParticles.Flares.SoftFlare' );
	Level.AddPrecacheMaterial( Material'AW-2004Particles.Fire.MuchSmoke2t' );
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Trails.Trail_red' );
	Level.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp1_frames' );
	Level.AddPrecacheMaterial( Material'EmitterTextures.MultiFrame.rockchunks02' );

	super.UpdatePrecacheMaterials();
}

defaultproperties
{
	VehicleProjSpawnOffset=(X=160,Y=-15,Z=66)
	RotPitchConstraint=(Min=10000,Max=5000)
	RotationInertia=0.5
	RotationSpeed=10.0
	CamRotationInertia=0.01
	Health=500
	HealthMax=500
	DriverDamageMult=0.45

	bDrawMeshInFP=true
	bDrawDriverInTP=true
	DrivePos=(X=-20,Y=13,Z=81)
	DriveRot=(Pitch=0)

	bRelativeExitPos=true
	EntryPosition=(X=0,Y=0,Z=0)
    EntryRadius=120.f
	ExitPositions(0)=(X=0,Y=+100,Z=100)
	ExitPositions(1)=(X=0,Y=-100,Z=100)

	CamAbsLocation=(Z=50)
	CamRelLocation=(X=100,Z=50)
	CamDistance=(X=-400,Z=50)
	FPCamPos=(X=-25,Y=13,Z=93)

	Mesh=SkeletalMesh'AS_Vehicles_M.minigun_turret'
	DrawScale=0.42
	TurretBaseClass=class'UT2k4Assault.ASTurret_Minigun_Base'
	TurretSwivelClass=class'UT2k4Assault.ASTurret_Minigun_Swivel'
	DefaultWeaponClassName="UT2k4Assault.Weapon_Turret_Minigun"

	DefaultCrosshair=Material'Crosshairs.HUD.Crosshair_Circle1'
	CrosshairScale=0.5
	bCHZeroYOffset=false

    CollisionHeight=80.0
    CollisionRadius=60.0

	bRemoteControlled=false
	bAutoTurret=false
	VehiclePositionString="manning a Minigun Turret"
	VehicleNameString="Minigun Turret"

	SoundVolume=255
	SoundRadius=1024
}

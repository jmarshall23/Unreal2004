//=============================================================================
// ASVehicle_Sentinel_Floor
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class ASVehicle_Sentinel_Floor extends ASVehicle_Sentinel;

static function StaticPrecache(LevelInfo L)
{
    super.StaticPrecache( L );

	L.AddPrecacheMaterial( Material'AS_Weapons_TX.Sentinels.FloorTurret' );		// Skins

	L.AddPrecacheMaterial( Material'AS_FX_TX.Beams.LaserTex' );					// Firing Effect
	L.AddPrecacheMaterial( Material'AS_FX_TX.Flares.Laser_Flare' );

	L.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp7_frames' );			// Explosion Effect
	L.AddPrecacheMaterial( Material'EpicParticles.Flares.SoftFlare' );
	L.AddPrecacheMaterial( Material'AW-2004Particles.Fire.MuchSmoke2t' );
	L.AddPrecacheMaterial( Material'AS_FX_TX.Trails.Trail_red' );
	L.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp1_frames' );
	L.AddPrecacheMaterial( Material'EmitterTextures.MultiFrame.rockchunks02' );

	L.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.FloorTurretSwivel' );
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.FloorTurretSwivel' );

	super.UpdatePrecacheStaticMeshes();
}

simulated function UpdatePrecacheMaterials()
{
	Level.AddPrecacheMaterial( Material'AS_Weapons_TX.Sentinels.FloorTurret' );		// Skins

	Level.AddPrecacheMaterial( Material'AS_FX_TX.Beams.LaserTex' );					// Firing Effect
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Flares.Laser_Flare' );

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
	OpenCloseSound=Sound'AssaultSounds.Sentinel.Ceiling_Open_Close'

    CollisionHeight=0.0
    CollisionRadius=0.0

	VehicleProjSpawnOffset=(X=65,Y=0,Z=0)

	TurretBaseClass=class'UT2k4Assault.ASVehicle_Sentinel_Floor_Base'
	TurretSwivelClass=class'UT2k4Assault.ASVehicle_Sentinel_Floor_Swivel'

	DrawType=DT_Mesh
	Mesh=SkeletalMesh'AS_Vehicles_M.FloorTurretGun'
    DrawScale=0.5
    AmbientGlow=48
	VehicleNameString="Floor Sentinel"
	bCanBeBaseForPawns=false
}

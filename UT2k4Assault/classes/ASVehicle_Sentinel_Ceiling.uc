//=============================================================================
// ASVehicle_Sentinel_Ceiling
//=============================================================================
// Created by Laurent Delayen
// � 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class ASVehicle_Sentinel_Ceiling extends ASVehicle_Sentinel;

static function StaticPrecache(LevelInfo L)
{
    super.StaticPrecache( L );

	L.AddPrecacheMaterial( Material'AS_Weapons_TX.Sentinels.CeilingTurret' );	// Skins

	L.AddPrecacheMaterial( Material'AS_FX_TX.Beams.LaserTex' );					// Firing Effect
	L.AddPrecacheMaterial( Material'AS_FX_TX.Flares.Laser_Flare' );

	L.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp7_frames' );			// Explosion Effect
	L.AddPrecacheMaterial( Material'EpicParticles.Flares.SoftFlare' );
	L.AddPrecacheMaterial( Material'AW-2004Particles.Fire.MuchSmoke2t' );
	L.AddPrecacheMaterial( Material'AS_FX_TX.Trails.Trail_red' );
	L.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp1_frames' );
	L.AddPrecacheMaterial( Material'EmitterTextures.MultiFrame.rockchunks02' );
}

simulated function UpdatePrecacheMaterials()
{
	Level.AddPrecacheMaterial( Material'AS_Weapons_TX.Sentinels.CeilingTurret' );	// Skins

	Level.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_2' );				// Firing Effect
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Flares.Laser_Flare' );

	Level.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp7_frames' );				// Explosion Effect
	Level.AddPrecacheMaterial( Material'EpicParticles.Flares.SoftFlare' );
	Level.AddPrecacheMaterial( Material'AW-2004Particles.Fire.MuchSmoke2t' );
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Trails.Trail_red' );
	Level.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp1_frames' );
	Level.AddPrecacheMaterial( Material'EmitterTextures.MultiFrame.rockchunks02' );

	super.UpdatePrecacheMaterials();
}

defaultproperties
{
	OpenCloseSound=Sound'AssaultSounds.Sentinel.Floor_Open_Close'
	VehicleProjSpawnOffset=(X=95,Y=0,Z=0)
	TurretBaseClass=None
	TurretSwivelClass=class'UT2k4Assault.ASVehicle_Sentinel_Ceiling_Swivel'

    CollisionHeight=0.0
    CollisionRadius=0.0

	DrawType=DT_Mesh
	Mesh=SkeletalMesh'AS_Vehicles_M.CeilingTurretGun'

	VehicleNameString="Ceiling Sentinel"
    DrawScale=0.5
    AmbientGlow=48
}
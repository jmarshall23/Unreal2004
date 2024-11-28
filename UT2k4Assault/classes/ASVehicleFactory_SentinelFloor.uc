//=============================================================================
// ASVehicleFactory_SentinelFloor
// Specific factory for Floor Sentinels
//=============================================================================

class ASVehicleFactory_SentinelFloor extends ASVehicleFactory;

var()	bool	bSpawnCampProtection;	// makes sentinels super strong

function VehicleSpawned()
{
	super.VehicleSpawned();
	ASVehicle_Sentinel(Child).bSpawnCampProtection = bSpawnCampProtection;
}

//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
	bSpawnBuildEffect=false
	bSpawnCampProtection=true
	VehicleLinkHealMult=0.15
	VehicleTeam=1
	VehicleClass=class'UT2k4Assault.ASVehicle_Sentinel_Floor'
	bEdShouldSnap=true

    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'AS_Weapons_SM.FloorTurretStaticEditor'
    DrawScale=0.5
    AmbientGlow=48

    CollisionHeight=0.0
    CollisionRadius=0.0
	PrePivot=(Z=150)
}
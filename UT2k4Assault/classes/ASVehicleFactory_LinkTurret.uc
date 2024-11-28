//=============================================================================
// ASVehicleFactory_LinkTurret
//=============================================================================

class ASVehicleFactory_LinkTurret extends ASVehicleFactory_Turret;

//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
	bSpawnBuildEffect=false
	VehicleTeam=1
	VehicleClass=None
	VehicleClassStr="UT2k4AssaultFull.ASTurret_LinkTurret"

    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'AS_Weapons_SM.LinkTurret_STATIC'
    DrawScale=0.3

	CollisionHeight=116
}
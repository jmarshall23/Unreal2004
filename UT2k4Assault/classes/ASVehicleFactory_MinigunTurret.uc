//=============================================================================
// ASVehicleFactory_MinigunTurret
// Specific factory for Minigun Turret
//=============================================================================

class ASVehicleFactory_MinigunTurret extends ASVehicleFactory;

var() name ObjectiveTag[6];

function VehicleSpawned()
{
	local ASTurret T;
	local int i;
	
	Super.VehicleSpawned();
	
	T = ASTurret(Child);
	if ( T == None )
		return;
		
	for ( i=0; i<6; i++ )
		T.ObjectiveTag[i] = ObjectiveTag[i];
}	

//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
	bSpawnBuildEffect=false
	VehicleClass=class'UT2k4Assault.ASTurret_Minigun'
	bEdShouldSnap=true
	
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'AS_Weapons_SM.Turret.ASMinigun_Editor'
    DrawScale=0.42
    AmbientGlow=96

    CollisionHeight=39.0
    CollisionRadius=60.0
}
//=============================================================================
// ASVehicleFactory_Turret
// Specific factory for Ion Cannon
//=============================================================================

class ASVehicleFactory_IonCannon extends ASVehicleFactory;

#exec OBJ LOAD File=AnnouncerAssault.uax

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
	bSpawnProtected=false
	bVehicleTeamLock=true
	VehicleTeam=1
	Announcement_Destroyed=sound'AnnouncerAssault.Ion_Cannon_Destroyed'
	VehicleClass=class'UT2k4AssaultFull.ASTurret_IonCannon'
	bEdShouldSnap=true

    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'AS_Weapons_SM.IonCannonStatic'
    DrawScale=0.66
    AmbientGlow=96

    CollisionHeight=300.0
    CollisionRadius=200.0
}
//UT2004 superhealth base with new staticmesh
//to automatically convert all SuperHealthChargers in a level to have this mesh, use the editor console command "NewPickupBases"
class NewSuperHealthCharger extends SuperHealthCharger;

defaultproperties
{
	NewStaticMesh=2k4chargerMESHES.HealthChargerMESH-DS
	NewPrePivot=(Z=2.75)
	NewDrawScale=0.7
}

//=============================================================================
// ShieldPack
//=============================================================================
class ShieldPack extends ShieldPickup;

#exec OBJ LOAD FILE=PickupSounds.uax
#exec OBJ LOAD FILE=E_Pickups.usx

static function StaticPrecache(LevelInfo L)
{
	L.AddPrecacheStaticMesh(StaticMesh'E_Pickups.RegShield');
}

defaultproperties
{
    RespawnTime=30.000000
    Physics=PHYS_Rotating
	RotationRate=(Yaw=24000)
    DrawScale=1.0
    PickupSound=sound'PickupSounds.ShieldPack'
    PickupForce="ShieldPack"  // jdf
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'E_Pickups.RegShield'
    CollisionRadius=32.0
    ShieldAmount=50
    Style=STY_AlphaZ
    ScaleGlow=0.6
    bPredictRespawns=true
}

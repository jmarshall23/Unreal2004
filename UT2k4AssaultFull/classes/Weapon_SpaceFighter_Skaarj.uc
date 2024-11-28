//=============================================================================
// Weapon_SpaceFighter_Skaarj
//=============================================================================

class Weapon_SpaceFighter_Skaarj extends Weapon_SpaceFighter
    config(user)
    HideDropDown
	CacheExempt;

#exec OBJ LOAD FILE=..\StaticMeshes\AS_Vehicles_SM.usx


//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
    DrawScale=1.0
	DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'AS_Vehicles_SM.Vehicles.SpaceFighter_Skaarj_FP'
    PlayerViewOffset=(X=50,Y=0,Z=-25)
    SmallViewOffset=(X=50,Y=0,Z=-25)
}

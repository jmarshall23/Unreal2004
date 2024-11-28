//=============================================================================
// SuperShieldPack
//=============================================================================
class SuperShieldPack extends ShieldPickup
	notplaceable;

#exec OBJ LOAD FILE=E_Pickups.usx

static function StaticPrecache(LevelInfo L)
{
	L.AddPrecacheStaticMesh(StaticMesh'E_Pickups.SuperShield');
}

defaultproperties
{
    RespawnTime=60.000000
    PickupMessage="You picked up a Super Shield Pack +"
    Physics=PHYS_Rotating
	RotationRate=(Yaw=24000)
    DrawScale=0.6
    PickupSound=sound'PickupSounds.LargeShieldPickup'
    PickupForce="LargeShieldPickup"  // jdf
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'E_Pickups.SuperShield'
    CollisionRadius=32.0
    ShieldAmount=100
    Style=STY_AlphaZ
    ScaleGlow=0.6
    TransientSoundRadius=450.0
}

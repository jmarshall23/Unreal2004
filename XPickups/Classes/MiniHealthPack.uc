//=============================================================================
// MiniHealthPack
//=============================================================================
class MiniHealthPack extends TournamentHealth;

// todo: need custom sound effect for this!

defaultproperties
{
    MaxDesireability=0.3
    PickupMessage="You picked up a Health Vial +"
    bSuperHeal=true
    Physics=PHYS_Rotating
	RotationRate=(Yaw=24000)
    DrawScale=0.06
    PickupSound=sound'PickupSounds.HealthPack'
    PickupForce="HealthPack"  // jdf
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'XPickups_rc.MiniHealthPack'
    CollisionRadius=24.0
    HealingAmount=5
    Style=STY_AlphaZ
    ScaleGlow=0.6
    CullDistance=+4500.0
}

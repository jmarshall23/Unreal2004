class FlakAmmoPickup extends UTAmmoPickup;

defaultproperties
{
    InventoryType=class'FlakAmmo'

    PickupMessage="You picked up 10 Flak Shells."
    PickupSound=Sound'PickupSounds.FlakAmmoPickup'
    PickupForce="FlakAmmoPickup"  // jdf

    AmmoAmount=10

    MaxDesireability=0.320000
    CollisionHeight=8.250000

    StaticMesh=StaticMesh'WeaponStaticMesh.FlakAmmoPickup'
    DrawType=DT_StaticMesh
    DrawScale=+0.8
    PrePivot=(Z=+6.5)
}

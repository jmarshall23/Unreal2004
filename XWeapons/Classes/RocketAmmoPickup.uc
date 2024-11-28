class RocketAmmoPickup extends UTAmmoPickup;

defaultproperties
{
    InventoryType=class'RocketAmmo'

    PickupMessage="You picked up a rocket pack."
    PickupSound=Sound'PickupSounds.RocketAmmoPickup'
    PickupForce="RocketAmmoPickup"  // jdf

    AmmoAmount=9

    MaxDesireability=0.300000
    CollisionHeight=13.500000

    StaticMesh=StaticMesh'WeaponStaticMesh.RocketAmmoPickup'
    DrawType=DT_StaticMesh
    DrawScale=+0.7
    PrePivot=(Z=+2.5)
}

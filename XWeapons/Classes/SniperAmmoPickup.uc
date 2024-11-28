class SniperAmmoPickup extends UTAmmoPickup;

defaultproperties
{
    InventoryType=class'SniperAmmo'

    PickupMessage="You picked up lightning ammo."
    PickupSound=Sound'PickupSounds.SniperAmmoPickup'
    PickupForce="SniperAmmoPickup"  // jdf

    AmmoAmount=10
    CollisionHeight=19.000000

    StaticMesh=StaticMesh'WeaponStaticMesh.SniperAmmoPickup'
    DrawType=DT_StaticMesh
}

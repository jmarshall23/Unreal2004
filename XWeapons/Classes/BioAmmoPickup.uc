class BioAmmoPickup extends UTAmmoPickup;

#exec OBJ LOAD FILE=PickupSounds.uax

defaultproperties
{
    InventoryType=class'BioAmmo'

    PickupMessage="You picked up some Bio-Rifle ammo"
    PickupSound=Sound'PickupSounds.FlakAmmoPickup'
    PickupForce="FlakAmmoPickup"  // jdf

    AmmoAmount=20

    MaxDesireability=0.320000
    CollisionHeight=8.250000

    StaticMesh=StaticMesh'WeaponStaticMesh.BioAmmoPickup'
    DrawType=DT_StaticMesh
}

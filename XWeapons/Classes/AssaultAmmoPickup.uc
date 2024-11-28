class AssaultAmmoPickup extends UTAmmoPickup;

defaultproperties
{
    InventoryType=class'GrenadeAmmo'

    PickupMessage="You got a box of grenades and bullets."
    PickupSound=Sound'PickupSounds.AssaultAmmoPickup'
    PickupForce="AssaultAmmoPickup"  // jdf

    AmmoAmount=4

    CollisionHeight=12.500000
    MaxDesireability=0.20000

    StaticMesh=StaticMesh'WeaponStaticMesh.AssaultAmmoPickup'
    DrawType=DT_StaticMesh
    TransientSoundVolume=0.4
}

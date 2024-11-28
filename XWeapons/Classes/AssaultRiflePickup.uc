//=============================================================================
// AssaultRiflePickup.
//=============================================================================
class AssaultRiflePickup extends UTWeaponPickup;

defaultproperties
{
    InventoryType=class'AssaultRifle'

    PickupMessage="You got the Assault Rifle."
    PickupSound=Sound'PickupSounds.AssaultRiflePickup'
    PickupForce="AssaultRiflePickup"  // jdf

	MaxDesireability=+0.4

    StaticMesh=staticmesh'NewWeaponPickups.AssaultPickupSM'
    DrawType=DT_StaticMesh
    DrawScale=0.5
    Standup=(Y=0.25,Z=0.0)
}

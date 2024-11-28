//=============================================================================
// SuperShockRiflePickup
//=============================================================================
class SuperShockRiflePickup extends UTWeaponPickup;

defaultproperties
{ 
    InventoryType=class'SuperShockRifle'

    PickupMessage="You got the Super Shock Rifle."
    PickupSound=Sound'PickupSounds.ShockRiflePickup'
    PickupForce="ShockRiflePickup"  // jdf

	MaxDesireability=+0.65

    StaticMesh=StaticMesh'WeaponStaticMesh.ShockRiflePickup'
    DrawType=DT_StaticMesh
    DrawScale=0.5
}

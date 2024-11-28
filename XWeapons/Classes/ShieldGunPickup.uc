//=============================================================================
// ShieldGunPickup.
//=============================================================================
class ShieldGunPickup extends UTWeaponPickup;

defaultproperties
{
    InventoryType=class'ShieldGun'

    PickupMessage="You got the Shield Gun."
    PickupSound=Sound'PickupSounds.ShieldGunPickup'
    PickupForce="ShieldGunPickup"  // jdf

	MaxDesireability=+0.39

    StaticMesh=StaticMesh'WeaponStaticMesh.ShieldGunPickup'
    DrawType=DT_StaticMesh
    DrawScale=0.5
}

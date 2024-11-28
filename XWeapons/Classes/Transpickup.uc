class TransPickup extends UTWeaponPickup
	notplaceable;

defaultproperties
{
    InventoryType=class'Translauncher'

    PickupMessage="You got the Translocator."
    PickupSound=Sound'PickupSounds.SniperRiflePickup'
    PickupForce="SniperRiflePickup"  // jdf

    StaticMesh=StaticMesh'newweaponpickups.translocatorcenter'
    DrawType=DT_StaticMesh
    DrawScale=0.2
}
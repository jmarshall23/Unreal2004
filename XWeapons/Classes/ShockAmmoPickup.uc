class ShockAmmoPickup extends UTAmmoPickup;
	
defaultproperties
{
    InventoryType=class'ShockAmmo'

    PickupMessage="You picked up a Shock Core."
    PickupSound=Sound'PickupSounds.ShockAmmoPickup'
    PickupForce="ShockAmmoPickup"  // jdf

    CollisionHeight=32.000000
    AmmoAmount=10
    DrawScale3D=(X=+0.8.y=+0.8,Z=+0.5)
    PrePivot=(Z=+32.0)

    StaticMesh=StaticMesh'WeaponStaticMesh.ShockAmmoPickup'
    DrawType=DT_StaticMesh
}

class ClassicSniperAmmoPickup extends UTAmmoPickup;

defaultproperties
{
    InventoryType=class'ClassicSniperAmmo'

    PickupMessage="You picked up sniper ammo."
    PickupSound=Sound'PickupSounds.SniperAmmoPickup'
    PickupForce="SniperAmmoPickup"  // jdf

    AmmoAmount=10
    CollisionHeight=16.000000
	PrePivot=(Z=16.0)

    StaticMesh=StaticMesh'NewWeaponStatic.ClassicSniperAmmoM'
    DrawType=DT_StaticMesh
}

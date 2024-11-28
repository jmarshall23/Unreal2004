class LinkAmmoPickup extends UTAmmoPickup;

function PostBeginPlay()
{
	Super.PostBeginPlay();
	
	if ( Level.Game.bAllowVehicles )
		MaxDesireability *= 1.9;
}

defaultproperties
{
    InventoryType=class'LinkAmmo'

    PickupMessage="You picked up link charges."
    PickupSound=Sound'PickupSounds.LinkAmmoPickup'
    PickupForce="LinkAmmoPickup"  // jdf

    AmmoAmount=50

    CollisionHeight=10.500000
    MaxDesireability=0.240000

    StaticMesh=StaticMesh'WeaponStaticMesh.LinkAmmoPickup'
    DrawType=DT_StaticMesh
}

class UTAmmoPickup extends Ammo;

function RespawnEffect()
{
	spawn(class'PlayerSpawnEffect');
}

defaultproperties
{
    MessageClass=class'PickupMessagePlus'
}
class BioRiflePickup extends UTWeaponPickup;

static function StaticPrecache(LevelInfo L)
{
    L.AddPrecacheMaterial(Texture'XEffects.xbiosplat2');
    L.AddPrecacheMaterial(Texture'XEffects.xbiosplat');
    L.AddPrecacheMaterial(Texture'XGameShaders.bio_flash');
    L.AddPrecacheMaterial(Texture'WeaponSkins.BioGoo.BRInnerGoo');
    L.AddPrecacheMaterial(Texture'WeaponSkins.BioGoo.BRInnerBubbles');
	L.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.BioRiflePickup');
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Texture'XEffects.xbiosplat2');
    Level.AddPrecacheMaterial(Texture'XEffects.xbiosplat');
    Level.AddPrecacheMaterial(Texture'XGameShaders.bio_flash');
    Level.AddPrecacheMaterial(Texture'WeaponSkins.BioGoo.BRInnerGoo');
    Level.AddPrecacheMaterial(Texture'WeaponSkins.BioGoo.BRInnerBubbles');

	super.UpdatePrecacheMaterials();
}

defaultproperties
{
    InventoryType=class'BioRifle'

    PickupMessage="You got the Bio-Rifle"
    PickupSound=Sound'PickupSounds.FlakCannonPickup'
    PickupForce="FlakCannonPickup"  // jdf

	MaxDesireability=+0.75

    StaticMesh=StaticMesh'WeaponStaticMesh.BioRiflePickup'
    DrawType=DT_StaticMesh
    DrawScale=0.6
}

//=============================================================================
// RocketLauncherPickup.
//=============================================================================
class RocketLauncherPickup extends UTWeaponPickup;

static function StaticPrecache(LevelInfo L)
{
    L.AddPrecacheMaterial(Material'WeaponSkins.RocketShellTex');
    L.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    L.AddPrecacheMaterial(Material'XEffects.SmokeAlphab_t');
    L.AddPrecacheMaterial(Material'EmitterTextures.rockchunks02');
    L.AddPrecacheMaterial(Material'EmitterTextures.fire3');
    L.AddPrecacheMaterial(Material'EmitterTextures.LargeFlames');
	L.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.rocketproj');
	L.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.RocketLauncherPickup');
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Material'EmitterTextures.fire3');
    Level.AddPrecacheMaterial(Material'EmitterTextures.LargeFlames');
    Level.AddPrecacheMaterial(Material'WeaponSkins.RocketShellTex');
    Level.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    Level.AddPrecacheMaterial(Material'XEffects.SmokeAlphab_t');
    Level.AddPrecacheMaterial(Material'EmitterTextures.rockchunks02');

	super.UpdatePrecacheMaterials();
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.rocketproj');
	Super.UpdatePrecacheStaticMeshes();
}

defaultproperties
{
    InventoryType=class'RocketLauncher'

    PickupMessage="You got the Rocket Launcher."
    PickupSound=Sound'PickupSounds.RocketLauncherPickup'
    PickupForce="RocketLauncherPickup"  // jdf

	MaxDesireability=+0.78

    StaticMesh=StaticMesh'WeaponStaticMesh.RocketLauncherPickup'
    DrawType=DT_StaticMesh
    DrawScale=0.45
}

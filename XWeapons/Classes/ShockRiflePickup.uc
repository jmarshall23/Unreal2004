//=============================================================================
// ShockRiflePickup
//=============================================================================
class ShockRiflePickup extends UTWeaponPickup;

static function StaticPrecache(LevelInfo L)
{
	if ( class'ShockRifle'.Default.bUseOldWeaponMesh )
	{
		L.AddPrecacheMaterial(Texture'WeaponSkins.Skins.ShockTex0');
		L.AddPrecacheMaterial(Texture'WeaponSkins.ShockLaser.lasermist');
	}
    L.AddPrecacheMaterial(Material'XEffects.ShockHeatDecal');
    L.AddPrecacheMaterial(Material'XEffectMat.shock_flash');
    L.AddPrecacheMaterial(Material'XEffectMat.shock_flare_a');
    L.AddPrecacheMaterial(Material'XEffectMat.shock_core');
    L.AddPrecacheMaterial(Material'XEffectMat.purple_line');
    L.AddPrecacheMaterial(Material'XEffectMat.shock_sparkle');
    L.AddPrecacheMaterial(Material'XEffectMat.shock_core_low');
    L.AddPrecacheMaterial(Material'XEffectMat.shock_Energy_green_faded');
    L.AddPrecacheMaterial(Material'XEffectMat.Shock_Elec_a');
    L.AddPrecacheMaterial(Material'XEffectMat.shock_gradient_b');
    L.AddPrecacheMaterial(Material'XEffectMat.Shock_ring_a');
    L.AddPrecacheMaterial(Material'XEffectMat.ShockComboFlash');
    L.AddPrecacheMaterial(Material'XGameShaders.shock_muzflash_1st');
    L.AddPrecacheMaterial(Material'XGameShaders.WeaponShaders.shock_muzflash_3rd');
    L.AddPrecacheMaterial(Material'XWeapons_rc.ShockBeamTex');
    L.AddPrecacheMaterial(Material'XEffects.SaDScorcht');
    L.AddPrecacheMaterial(Material'DeployableTex.C_T_Electricity_SG');
    L.AddPrecacheMaterial(Material'UT2004Weapons.ShockRipple');

	L.AddPrecacheStaticMesh(StaticMesh'Editor.TexPropSphere');
	L.AddPrecacheStaticMesh(StaticMesh'NewWeaponPickups.ShockPickupSM');
}

simulated function UpdatePrecacheMaterials()
{
	if ( class'ShockRifle'.Default.bUseOldWeaponMesh )
	{
		Level.AddPrecacheMaterial(Texture'WeaponSkins.Skins.ShockTex0');
		Level.AddPrecacheMaterial(Texture'WeaponSkins.ShockLaser.lasermist');
	}
    Level.AddPrecacheMaterial(Material'XEffects.ShockHeatDecal');
    Level.AddPrecacheMaterial(Material'XEffectMat.shock_flash');
    Level.AddPrecacheMaterial(Material'XEffectMat.shock_flare_a');
    Level.AddPrecacheMaterial(Material'XEffectMat.shock_core');
    Level.AddPrecacheMaterial(Material'XEffectMat.purple_line');
    Level.AddPrecacheMaterial(Material'XEffectMat.shock_sparkle');
    Level.AddPrecacheMaterial(Material'XEffectMat.shock_core_low');
    Level.AddPrecacheMaterial(Material'XEffectMat.shock_Energy_green_faded');
    Level.AddPrecacheMaterial(Material'XEffectMat.Shock_Elec_a');
    Level.AddPrecacheMaterial(Material'XEffectMat.shock_gradient_b');
    Level.AddPrecacheMaterial(Material'XEffectMat.Shock_ring_a');
    Level.AddPrecacheMaterial(Material'XEffectMat.ShockComboFlash');
    Level.AddPrecacheMaterial(Material'XGameShaders.shock_muzflash_1st');
    Level.AddPrecacheMaterial(Material'XGameShaders.WeaponShaders.shock_muzflash_3rd');
    Level.AddPrecacheMaterial(Material'XWeapons_rc.ShockBeamTex');
    Level.AddPrecacheMaterial(Material'DeployableTex.C_T_Electricity_SG');
    Level.AddPrecacheMaterial(Material'XEffects.SaDScorcht');
    Level.AddPrecacheMaterial(Material'UT2004Weapons.ShockRipple');

	super.UpdatePrecacheMaterials();
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh(StaticMesh'Editor.TexPropSphere');
	Super.UpdatePrecacheStaticMeshes();
}

defaultproperties
{
    InventoryType=class'ShockRifle'

    PickupMessage="You got the Shock Rifle."
    PickupSound=Sound'PickupSounds.ShockRiflePickup'
    PickupForce="ShockRiflePickup"  // jdf

	MaxDesireability=+0.63

    Skins[0]=UT2004Weapons.ShockRifleTex0
    Skins[1]=material'UT2004Weapons.PurpleShockFinal'
    StaticMesh=staticmesh'NewWeaponPickups.ShockPickupSM'
    DrawType=DT_StaticMesh
    DrawScale=0.55
    Standup=(Y=0.25,Z=0.0)
}

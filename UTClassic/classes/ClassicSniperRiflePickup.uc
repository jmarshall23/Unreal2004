class ClassicSniperRiflePickup extends UTWeaponPickup;

#exec OBJ LOAD FILE=NewWeaponStatic.usx

static function StaticPrecache(LevelInfo L)
{
    L.AddPrecacheMaterial(Material'XGameShaders.WeaponEnvShader');
    L.AddPrecacheMaterial(Texture'NewSniperRifle.COGAssaultZoomedCrosshair');
    L.AddPrecacheMaterial(Texture'NewSniperRifle.NewSniper1');
    L.AddPrecacheMaterial(Texture'NewSniperRifle.Sniper2');
	L.AddPrecacheMaterial(Texture'Engine.WhiteTexture');
	L.AddPrecacheStaticMesh(StaticMesh'NewWeaponStatic.newsniperpickup');
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Material'XGameShaders.WeaponEnvShader');
    Level.AddPrecacheMaterial(Texture'NewSniperRifle.COGAssaultZoomedCrosshair');
    Level.AddPrecacheMaterial(Texture'NewSniperRifle.NewSniper1');
    Level.AddPrecacheMaterial(Texture'NewSniperRifle.Sniper2');
	Level.AddPrecacheMaterial(Texture'Engine.WhiteTexture');
	super.UpdatePrecacheMaterials();
}

defaultproperties
{
    InventoryType=class'ClassicSniperRifle'
    PickupMessage="You got the Sniper Rifle."
    PickupSound=Sound'NewWeaponSounds.NewSniper_Load'
    PickupForce="SniperRiflePickup"  

	MaxDesireability=+0.75

    StaticMesh=StaticMesh'NewWeaponStatic.NewSniperPickup'
    DrawType=DT_StaticMesh
    DrawScale=0.21
}



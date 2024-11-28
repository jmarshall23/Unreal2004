class PainterPickup extends UTWeaponPickup;

function PrebeginPlay()
{
	Super.PreBeginPlay();
	if ( Level.Game.IsA('xMutantGame') )
		Destroy();
}

function SetWeaponStay()
{
	bWeaponStay = false;
}

function float GetRespawnTime()
{
	return ReSpawnTime;
}

static function StaticPrecache(LevelInfo L)
{
	L.AddPrecacheMaterial(Material'XEffectMat.painter_beam');
	L.AddPrecacheMaterial(Material'XEffectMat.ion_grey');
	L.AddPrecacheMaterial(Material'XEffectMat.Ion_beam');
	L.AddPrecacheMaterial(Material'EpicParticles.Smokepuff2');
	L.AddPrecacheMaterial(Material'EpicParticles.IonBurn2');
	L.AddPrecacheMaterial(Material'EpicParticles.BurnFlare1');
	L.AddPrecacheMaterial(Material'EpicParticles.WhiteStreak01aw');
	L.AddPrecacheMaterial(Material'EpicParticles.Smokepuff');
	L.AddPrecacheMaterial(Material'EpicParticles.SoftFlare');
	L.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.PainterPickup');
}

simulated function UpdatePrecacheMaterials()
{
	Level.AddPrecacheMaterial(Material'XEffectMat.painter_beam');
	Level.AddPrecacheMaterial(Material'XEffectMat.ion_grey');
	Level.AddPrecacheMaterial(Material'XEffectMat.Ion_beam');
	Level.AddPrecacheMaterial(Material'EpicParticles.Smokepuff2');
	Level.AddPrecacheMaterial(Material'EpicParticles.IonBurn2');
	Level.AddPrecacheMaterial(Material'EpicParticles.BurnFlare1');
	Level.AddPrecacheMaterial(Material'EpicParticles.WhiteStreak01aw');
	Level.AddPrecacheMaterial(Material'EpicParticles.Smokepuff');
	Level.AddPrecacheMaterial(Material'EpicParticles.SoftFlare');

	super.UpdatePrecacheMaterials();
}

defaultproperties
{
    InventoryType=class'Painter'

    PickupMessage="You got the Ion Painter."
    PickupSound=Sound'PickupSounds.LinkgunPickup'
    PickupForce="LinkGunPickup"  // jdf

	MaxDesireability=+1.5

    StaticMesh=StaticMesh'WeaponStaticMesh.PainterPickup'
    DrawType=DT_StaticMesh
    DrawScale=0.6

    RespawnTime=120.0
    bWeaponStay=false
}

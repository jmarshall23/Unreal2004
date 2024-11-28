#exec OBJ LOAD FILE=EpicParticles.utx
#exec OBJ LOAD FILE=2K4HUD.utx

class RedeemerPickup extends UTWeaponPickup;

var material PrecacheHUDTextures[4];

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
	local int i;
	
	for ( i=0; i<4; i++ )
		L.AddPrecacheMaterial(Default.PrecacheHUDTextures[i]);
	L.AddPrecacheMaterial(Material'EpicParticles.Smokepuff2');
	L.AddPrecacheMaterial(Material'EpicParticles.IonBurn');
	L.AddPrecacheMaterial(Material'EpicParticles.IonWave');
	L.AddPrecacheMaterial(Material'EpicParticles.BurnFlare1');
	L.AddPrecacheMaterial(Material'EpicParticles.WhiteStreak01aw');
	L.AddPrecacheMaterial(Material'EpicParticles.Smokepuff');
	L.AddPrecacheMaterial(Material'EpicParticles.SoftFlare');
	L.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.RedeemerPickup');
}

simulated function UpdatePrecacheMaterials()
{
	local int i;
	
	for ( i=0; i<4; i++ )
		Level.AddPrecacheMaterial(Default.PrecacheHUDTextures[i]);
	Level.AddPrecacheMaterial(Material'EpicParticles.Smokepuff2');
	Level.AddPrecacheMaterial(Material'EpicParticles.IonBurn');
	Level.AddPrecacheMaterial(Material'EpicParticles.IonWave');
	Level.AddPrecacheMaterial(Material'EpicParticles.BurnFlare1');
	Level.AddPrecacheMaterial(Material'EpicParticles.WhiteStreak01aw');
	Level.AddPrecacheMaterial(Material'EpicParticles.Smokepuff');
	Level.AddPrecacheMaterial(Material'EpicParticles.SoftFlare');

	super.UpdatePrecacheMaterials();
}

defaultproperties
{
    InventoryType=class'Redeemer'

    PickupMessage="You got the Redeemer."
    PickupSound=Sound'PickupSounds.FlakCannonPickup'
    PickupForce="FlakCannonPickup"

	MaxDesireability=+1.0

    StaticMesh=StaticMesh'WeaponStaticMesh.RedeemerPickup'
    DrawType=DT_StaticMesh
    DrawScale=0.9
    
    RespawnTime=120.0
    bWeaponStay=false
    
    PrecacheHUDTextures(0)=Material'2K4Hud.Redeemerinnerscope'
    PrecacheHUDTextures(1)=Material'2K4Hud.Redeemerouteredge'
    PrecacheHUDTextures(2)=Material'2K4Hud.Redeemerouterscope'
    PrecacheHUDTextures(3)=Material'2K4Hud.Rdm_Altitude'
}

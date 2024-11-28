//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSMASSideGun extends ONSWeapon;

static function StaticPrecache(LevelInfo L)
{
    L.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.PlasmaStar');
    L.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.PlasmaHead');
    L.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.SmokePanels1');
    L.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.PlasmaStar2');
    L.AddPrecacheMaterial(Material'EpicParticles.Flares.FlashFlare1');
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.PlasmaStar');
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.PlasmaHead');
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.SmokePanels1');
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.PlasmaStar2');
    Level.AddPrecacheMaterial(Material'EpicParticles.Flares.FlashFlare1');

    Super.UpdatePrecacheMaterials();
}

DefaultProperties
{
    Mesh=Mesh'ONSFullAnimations.MASPassengerGun'
    YawBone=Object83
    YawStartConstraint=0
    YawEndConstraint=65535
    PitchBone=Object83
    PitchUpLimit=15000
    PitchDownLimit=60000
    bInstantFire=False
    FireInterval=0.15
    AltFireInterval=0.15
    bAmbientFireSound=False
    WeaponFireAttachmentBone=Object85
    GunnerAttachmentBone=Object83
    WeaponFireOffset=20.0
    bAimable=True
    DamageType=class'DamTypePRVLaser'
    DamageMin=25
    DamageMax=25
    DualFireOffset=10
    FireSoundClass=sound'ONSVehicleSounds-S.LaserSounds.Laser17'
    AltFireSoundClass=sound'ONSVehicleSounds-S.LaserSounds.Laser17'
    FireForce="Laser01"
    AltFireForce="Laser01"
    ProjectileClass=class'OnslaughtFull.ONSMASPlasmaProjectile'
    bDoOffsetTrace=True
}

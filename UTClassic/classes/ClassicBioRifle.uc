class ClassicBioRifle extends BioRifle
	HideDropDown
	CacheExempt;

defaultproperties
{
    FireModeClass(0)=ClassicBioFire
    FireModeClass(1)=ClassicBioChargedFire
	PickupClass=class'ClassicBioRiflePickup'
    SelectAnimRate=+1.0
	BringUpTime=+0.45
	MinReloadPct=+1.0
}

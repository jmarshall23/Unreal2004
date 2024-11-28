class ClassicFlakCannon extends FlakCannon
	HideDropDown
	CacheExempt;

defaultproperties
{
    FireModeClass(0)=ClassicFlakFire
    FireModeClass(1)=ClassicFlakAltFire
	PickupClass=class'ClassicFlakCannonPickup'
    SelectAnimRate=+0.75
	BringUpTime=+0.6
	MinReloadPct=+1.0
}

class ClassicMinigun extends Minigun
	HideDropDown
	CacheExempt;

defaultproperties
{
    FireModeClass(0)=ClassicMinigunAltFire
    FireModeClass(1)=ClassicMinigunFire
    PickupClass=class'ClassicMinigunPickup'
    SelectAnimRate=+0.75
	BringUpTime=+0.6
	MinReloadPct=+1.0
}

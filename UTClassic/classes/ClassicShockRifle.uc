class ClassicShockRifle extends ShockRifle
	HideDropDown
	CacheExempt;

defaultproperties
{
    FireModeClass(0)=ClassicShockBeamFire
    FireModeClass(1)=ClassicShockProjFire
    PickupClass=class'ClassicShockRiflePickup'
    SelectAnimRate=+1.0
	BringUpTime=+0.45
	MinReloadPct=+1.0
}

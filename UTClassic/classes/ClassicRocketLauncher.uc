class ClassicRocketLauncher extends RocketLauncher
	HideDropDown
	CacheExempt;


simulated event ClientStartFire(int Mode)
{
	if ( Mode == 0 )
	{
		SetTightSpread(false);
	}
    else if ( FireMode[0].bIsFiring || (FireMode[0].NextFireTime > Level.TimeSeconds) )
    {
		if ( FireMode[0].Load > 0 )
			SetTightSpread(true);
		return;
    }
    Super(Weapon).ClientStartFire(Mode);
}

defaultproperties
{
    FireModeClass(0)=ClassicRocketMultiFire
    FireModeClass(1)=ClassicGrenadeFire
	PickupClass=class'ClassicRocketLauncherPickup'
    SelectAnimRate=+0.75
	BringUpTime=+0.6
	MinReloadPct=+1.0
}


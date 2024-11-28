class ClassicRocketMultifire extends RocketMultifire;

function ModeTick(float dt)
{
    // auto fire if loaded last rocket
    if (HoldTime > 0.0 && Load >= Weapon.AmmoAmount(ThisModeNum) && !bNowWaiting)
    {
        bIsFiring = false;
    }

    Super(ProjectileFire).ModeTick(dt);

    if ( (Load <= 5) && HoldTime >= FireRate*Load)
    {
        if (Instigator.IsLocallyControlled())
        	RocketLauncher(Weapon).PlayLoad(false);
		else
			ServerPlayLoading();
        Load = Load + 1.0;
    }
}

function DoFireEffect()
{
	Weapon.GetFireMode(0).NextFireTime = Level.TimeSeconds + FireRate;
	Weapon.GetFireMode(1).NextFireTime = Level.TimeSeconds + FireRate;

	Super.DoFireEffect();
}

defaultproperties
{
    FireAnimRate=0.76
    FireRate=1.25
    MaxHoldTime=7.7
    MaxLoad=6
}

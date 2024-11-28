class ZoomSuperShockBeamFire extends SuperShockBeamFire
	config;

var config bool bAllowMultiHit;

function bool AllowMultiHit()
{
	return bAllowMultiHit;
}

defaultproperties
{
	bAllowMultiHit=true
    DamageType=class'ZoomSuperShockBeamDamage'
}
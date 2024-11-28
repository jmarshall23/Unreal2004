class FM_Turret_Minigun_AltFire extends WeaponFire;

simulated function bool AllowFire()
{
    return true;
}

defaultproperties
{
    FireRate=0.5
    bModeExclusive=false
}

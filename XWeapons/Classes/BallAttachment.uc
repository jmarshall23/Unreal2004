class BallAttachment extends xWeaponAttachment;

function InitFor(Inventory I)
{
    Super.InitFor(I);
}

simulated event ThirdPersonEffects()
{
    Super.ThirdPersonEffects();
}

defaultproperties
{
    Mesh=mesh'Weapons.BallLauncher_3rd'
    bHeavy=true
    bRapidFire=false
    bAltRapidFire=false
}
 
class SniperAttachment extends xWeaponAttachment;

var() LightningCharge3rd charge;

simulated function Destroyed()
{
    if (charge != None)
        charge.Destroy();

    Super.Destroyed();
}

simulated event ThirdPersonEffects()
{
    if ( Level.NetMode != NM_DedicatedServer )
    {
        if (charge == None)
        {
            charge = Spawn(class'LightningCharge3rd');
            AttachToBone(charge, 'tip');
        }
        WeaponLight();
    }

    Super.ThirdPersonEffects();
}

defaultproperties
{
    bHeavy=false
    bRapidFire=false
    bAltRapidFire=false
    Mesh=mesh'Weapons.Sniper_3rd'

    bDynamicLight=false
    LightType=LT_Steady
    LightEffect=LE_NonIncidence
    LightPeriod=3
    LightBrightness=255
    LightHue=165
    LightSaturation=170
    LightRadius=5.0
}

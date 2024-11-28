class SniperAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Lightning Charges"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=458,Y1=82,X2=491,Y2=133)

    bTryHeadShot=true
    PickupClass=class'SniperAmmoPickup'
    MaxAmmo=40
    InitialAmount=15
}

class FlakAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Flak Shells"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=394,Y1=40,X2=457,Y2=81)

    PickupClass=class'FlakAmmoPickup'
    MaxAmmo=35
    InitialAmount=15
}

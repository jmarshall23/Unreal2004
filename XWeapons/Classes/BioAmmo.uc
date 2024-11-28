class BioAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Bio-Rifle Goop"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=383,Y1=82,X2=412,Y2=129)

    PickupClass=class'BioAmmoPickup'
    MaxAmmo=50
    InitialAmount=20
}

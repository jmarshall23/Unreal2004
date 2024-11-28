class ShockAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Shock Core"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=400,Y1=130,X2=426,Y2=179)

    PickupClass=class'ShockAmmoPickup'
    MaxAmmo=50
    InitialAmount=20
}

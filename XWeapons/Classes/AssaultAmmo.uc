class AssaultAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Bullets"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=336,Y1=82,X2=382,Y2=125)

    PickupClass=class'AssaultAmmoPickup'
    MaxAmmo=200
    InitialAmount=100
}

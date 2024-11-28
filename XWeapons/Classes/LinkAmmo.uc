class LinkAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Link Ammo"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=413,Y1=82,X2=457,Y2=125)

    PickupClass=class'LinkAmmoPickup'
    MaxAmmo=220
    InitialAmount=70
}

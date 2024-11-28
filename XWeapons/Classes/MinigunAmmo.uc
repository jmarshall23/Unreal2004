class MinigunAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Bullets"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=338,Y1=40,X2=393,Y2=79)

    PickupClass=class'MinigunAmmoPickup'
    MaxAmmo=300
    InitialAmount=150
}

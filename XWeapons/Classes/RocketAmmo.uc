class RocketAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Rockets"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=458,Y1=34,X2=511,Y2=78)

    PickupClass=class'RocketAmmoPickup'
    MaxAmmo=30
    InitialAmount=12
}

class ClassicSniperAmmo extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
    ItemName="Sniper Bullets"
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=451,Y1=445,X2=510,Y2=500)

    bTryHeadShot=true
    PickupClass=class'ClassicSniperAmmoPickup'
    MaxAmmo=35
    InitialAmount=15
}

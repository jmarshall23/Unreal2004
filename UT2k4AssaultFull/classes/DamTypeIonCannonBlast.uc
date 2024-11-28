class DamTypeIonCannonBlast extends VehicleDamageType
	abstract;

defaultproperties
{
    DeathString="%o was OBLITERATED by %k"
	MaleSuicide="%o was OBLITERATED"
	FemaleSuicide="%o was OBLITERATED"

    VehicleClass=class'ASTurret_IonCannon'

    bDetonatesGoop=true
    GibModifier=0
    bSuperWeapon=false
    bSkeletize=true
    bArmorStops=false
    DamageOverlayMaterial=Material'UT2004Weapons.ShockHitShader'
    DamageOverlayTime=1.0
	VehicleDamageScaling=2.f
}

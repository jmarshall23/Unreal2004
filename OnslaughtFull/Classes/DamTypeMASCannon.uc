//-----------------------------------------------------------
//
//-----------------------------------------------------------
class DamTypeMASCannon extends VehicleDamageType;

DefaultProperties
{
    DeathString="%o was VAPORIZED by %k!"
	MaleSuicide="%o was VAPORIZED!"
	FemaleSuicide="%o was VAPORIZED!"

    bArmorStops=False
    bNeverSevers=True

    bKUseTearOffMomentum=True
	GibPerterbation=0.5
	GibModifier=2.0
	bLocationalHit=False
	VehicleClass=class'ONSMobileAssaultStation'
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
class DamTypeMASPlasma extends VehicleDamageType
    abstract;

DefaultProperties
{
	DeathString="%k's Leviathan turret plasmanated %o."
	MaleSuicide="%o wasted himself."
	FemaleSuicide="%o wasted herself."

	FlashFog=(X=700.00000,Y=0.000000,Z=0.00000)
	bDetonatesGoop=true
	bDelayedDamage=true
	VehicleClass=class'ONSMASSideGunPawn'
}

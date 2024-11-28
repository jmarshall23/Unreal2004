class DamTypeMinigunTurretBullet extends VehicleDamageType
	abstract;

defaultproperties
{
	bRagdollBullet=true
	DeathString="%o was mowed down by %k's minigun turret."
	MaleSuicide="%o turned the minigun on himself."
	FemaleSuicide="%o turned the minigun on herself."
	FlashFog=(X=600.00000,Y=0.000000,Z=0.00000)
	VehicleClass=class'ASTurret_Minigun'
	KDamageImpulse=2000
	VehicleDamageScaling=0.75
	bBulletHit=True
}

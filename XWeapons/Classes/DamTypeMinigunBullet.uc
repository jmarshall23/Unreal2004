class DamTypeMinigunBullet extends WeaponDamageType
	abstract;

defaultproperties
{
	bRagdollBullet=true
    DeathString="%o was mowed down by %k's minigun."
	MaleSuicide="%o turned the minigun on himself."
	FemaleSuicide="%o turned the minigun on herself."
	FlashFog=(X=600.00000,Y=0.000000,Z=0.00000)
    WeaponClass=class'Minigun'
    KDamageImpulse=2000
    VehicleDamageScaling=0.65
    bBulletHit=True
}


class DamTypeAssaultBullet extends WeaponDamageType
	abstract;

defaultproperties
{
	bRagdollBullet=true
    DeathString="%o was ventilated by %k's assault rifle."
	MaleSuicide="%o assaulted himself."
	FemaleSuicide="%o assaulted herself."
	FlashFog=(X=600.00000,Y=0.000000,Z=0.00000)

    WeaponClass=class'AssaultRifle'
	KDamageImpulse=2000
    VehicleDamageScaling=0.7
    bBulletHit=True
}


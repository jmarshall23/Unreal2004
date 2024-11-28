class DamTypeAssaultGrenade extends WeaponDamageType
	abstract;

defaultproperties
{
    DeathString="%o tried to juggle %k's grenade."
	MaleSuicide="%o jumped on his own grenade."
	FemaleSuicide="%o jumped on her own grenade."

    WeaponClass=class'AssaultRifle'
    bDetonatesGoop=true
    bDelayedDamage=true
}


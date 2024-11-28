class DamTypeTeleFrag extends WeaponDamageType
	abstract;

defaultproperties
{
    DeathString="%o had their atoms scattered by %k."
	MaleSuicide="%o tried to go where no man has gone before."
	FemaleSuicide="%o tried to go where no woman has gone before."

    WeaponClass=class'TransLauncher'

	bAlwaysSevers=true
    bAlwaysGibs=true
    GibPerterbation=1.0

    bLocationalHit=false
}

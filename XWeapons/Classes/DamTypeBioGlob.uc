class DamTypeBioGlob extends WeaponDamageType
	abstract;

defaultproperties
{
    DeathString="%o was slimed by %k's bio-rifle."
	MaleSuicide="%o was slimed by his own goop."
	FemaleSuicide="%o was slimed by her own goop."

    WeaponClass=class'BioRifle'

	bKUseTearOffMomentum=false
    bDetonatesGoop=true
    bDelayedDamage=true
    
    DeathOverlayMaterial=Material'XGameShaders.PlayerShaders.LinkHit'
}


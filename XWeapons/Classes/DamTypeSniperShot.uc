class DamTypeSniperShot extends WeaponDamageType
	abstract;

static function GetHitEffects(out class<xEmitter> HitEffects[4], int VictemHealth )
{
    HitEffects[0] = class'HitSmoke';
}

defaultproperties
{
    DeathString="%o rode %k's lightning."
	MaleSuicide="%o had an electrifying experience."
	FemaleSuicide="%o had an electrifying experience."

    DamageOverlayMaterial=Material'XGameShaders.PlayerShaders.LightningHit'
    DamageOverlayTime=0.9

    WeaponClass=class'SniperRifle'

    GibPerterbation=0.25
    bDetonatesGoop=true

    bCauseConvulsions=true
    VehicleDamageScaling=0.85
}


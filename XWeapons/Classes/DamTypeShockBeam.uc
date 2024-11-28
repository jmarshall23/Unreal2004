class DamTypeShockBeam extends WeaponDamageType
	abstract;

static function GetHitEffects(out class<xEmitter> HitEffects[4], int VictemHealth )
{
    HitEffects[0] = class'HitSmoke';
}

defaultproperties
{
    DeathString="%o was fatally enlightened by %k's shock beam."
	MaleSuicide="%o somehow managed to shoot himself with the shock rifle."
	FemaleSuicide="%o somehow managed to shoot herself with the shock rifle."

    DamageOverlayMaterial=Material'UT2004Weapons.ShockHitShader'
    DamageOverlayTime=0.8

    WeaponClass=class'ShockRifle'

    GibPerterbation=0.75
    bDetonatesGoop=true

    VehicleMomentumScaling=0.50
    VehicleDamageScaling=0.85
}


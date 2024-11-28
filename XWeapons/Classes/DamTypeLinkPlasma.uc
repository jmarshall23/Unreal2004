class DamTypeLinkPlasma extends WeaponDamageType
	abstract;

static function GetHitEffects(out class<xEmitter> HitEffects[4], int VictemHealth )
{
    HitEffects[0] = class'HitSmoke';
}

defaultproperties
{
    DeathString="%o was served an extra helping of %k's plasma."
	MaleSuicide="%o fried himself with his own plasma blast."
	FemaleSuicide="%o fried herself with her own plasma blast."
	FlashFog=(X=700.00000,Y=0.000000,Z=0.00000)
    DamageOverlayMaterial=Material'XGameShaders.PlayerShaders.LinkHit'
    DamageOverlayTime=0.5

    WeaponClass=class'LinkGun'

    bDetonatesGoop=true
    VehicleDamageScaling=0.67
    bDelayedDamage=true
}


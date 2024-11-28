class DamTypeSkaarjProj extends DamageType
	abstract;

static function GetHitEffects(out class<xEmitter> HitEffects[4], int VictemHealth )
{
    HitEffects[0] = class'HitSmoke';
}

defaultproperties
{
    DeathString="%o was scorched by a skaarj."

    bDetonatesGoop=true
    KDamageImpulse=10000
}
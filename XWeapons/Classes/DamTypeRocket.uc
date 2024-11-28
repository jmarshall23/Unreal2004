class DamTypeRocket extends WeaponDamageType
	abstract;

static function GetHitEffects(out class<xEmitter> HitEffects[4], int VictimHealth )
{
    HitEffects[0] = class'HitSmoke';

    if( VictimHealth <= 0 )
        HitEffects[1] = class'HitFlameBig';
    else if ( FRand() < 0.8 )
        HitEffects[1] = class'HitFlame';
}

defaultproperties
{
    DeathString="%o rode %k's rocket into oblivion."
	MaleSuicide="%o fired his rocket prematurely."
	FemaleSuicide="%o fired her rocket prematurely."

    WeaponClass=class'RocketLauncher'
    bDetonatesGoop=true
    KDamageImpulse=20000
    VehicleMomentumScaling=1.3
    bThrowRagdoll=true
	GibPerterbation=0.15
	bFlaming=true
	bDelayedDamage=true
}


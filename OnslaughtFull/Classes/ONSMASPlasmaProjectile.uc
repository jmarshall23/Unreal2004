//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSMASPlasmaProjectile extends ONSPlasmaProjectile;

DefaultProperties
{
    PlasmaEffectClass=class'Onslaught.ONSGreenPlasmaSmallFireEffect'
    HitEffectClass=class'Onslaught.ONSPlasmaHitGreen'
    Damage=30
    DamageRadius=100.0
    Speed=500
    MaxSpeed=15000
    AccelerationMagnitude=20000
    MyDamageType=class'DamTypeMASPlasma'
}

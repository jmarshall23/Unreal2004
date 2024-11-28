class MinigunAltFire extends MinigunFire;

defaultproperties
{
    BarrelRotationsPerSec=1.f

    DamageMin=14
    DamageMax=16
	Spread=0.0300

    FireLoopAnimRate=3.f
    PreFireTime=0.15f
    WindUpTime=0.15f

    FiringSound=Sound'WeaponSounds.MiniGun.MiniAltFireb'
    SmokeEmitterClass=class'xEffects.MinigunAltMuzzleSmoke'
    FiringForce="minialtfireb"  // jdf
    DamageType=class'DamTypeMinigunAlt'
	MinigunSoundVolume=240

    BotRefireRate=0.99

    ShakeOffsetMag=(X=1.0,Y=1.0,Z=1.0)
    ShakeOffsetRate=(X=1000.0,Y=1000.0,Z=1000.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=50.0,Y=50.0,Z=50.0)
    ShakeRotRate=(X=10000.0,Y=10000.0,Z=10000.0)
    ShakeRotTime=2
}

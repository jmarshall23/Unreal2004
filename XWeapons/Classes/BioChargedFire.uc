class BioChargedFire extends ProjectileFire;

var() float GoopUpRate;
var() int MaxGoopLoad;
var() int GoopLoad;
var() Sound HoldSound;

function InitEffects()
{
    Super.InitEffects();
    if( FlashEmitter == None )
        FlashEmitter = Weapon.GetFireMode(0).FlashEmitter;
}

function DrawMuzzleFlash(Canvas Canvas)
{
	if ( FlashEmitter != None )
		FlashEmitter.SetRotation(Weapon.Rotation);
    Super.DrawMuzzleFlash(Canvas);
}

function ModeHoldFire()
{
    if ( Weapon.AmmoAmount(ThisModeNum) > 0 )
    {
        Super.ModeHoldFire();
        GotoState('Hold');
    }
}

function float MaxRange()
{
	return 1500;
}

simulated function bool AllowFire()
{
    return (Weapon.AmmoAmount(ThisModeNum) > 0 || GoopLoad > 0);
}

simulated function PlayStartHold()
{
    Weapon.PlayAnim('AltFire', 1.0 / (GoopUpRate*MaxGoopLoad), 0.1);
}

simulated function PlayFiring()
{
	Super.PlayFiring();
	Weapon.OutOfAmmo();
}

state Hold
{
    simulated function BeginState()
    {
        GoopLoad = 0;
        SetTimer(GoopUpRate, true);
        Weapon.PlayOwnedSound(sound'WeaponSounds.Biorifle_charge',SLOT_Interact,TransientSoundVolume);
        Weapon.ClientPlayForceFeedback( "BioRiflePowerUp" );  // jdf
        Timer();
    }

    simulated function Timer()
    {
		if ( Weapon.AmmoAmount(ThisModeNum) > 0 )
			GoopLoad++;
        Weapon.ConsumeAmmo(ThisModeNum, 1);
        if (GoopLoad == MaxGoopLoad || Weapon.AmmoAmount(ThisModeNum) == 0)
        {
            SetTimer(0.0, false);
			Instigator.AmbientSound = sound'NewWeaponSounds.BioGoopLoop';
			Instigator.SoundRadius = 50;
			Instigator.SoundVolume = 50;
        }
    }

    simulated function EndState()
    {
		if ( (Instigator != None) && (Instigator.AmbientSound == sound'NewWeaponSounds.BioGoopLoop') )
			Instigator.AmbientSound = None;
		Instigator.SoundRadius = Instigator.Default.SoundRadius;
		Instigator.SoundVolume = Instigator.Default.SoundVolume;

        StopForceFeedback( "BioRiflePowerUp" );  // jdf
    }
}

function projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local BioGlob Glob;

    GotoState('');

    if (GoopLoad == 0) return None;

    Glob = Weapon.Spawn(class'BioGlob',,, Start, Dir);
    if ( Glob != None )
    {
		Glob.Damage *= DamageAtten;
		Glob.SetGoopLevel(GoopLoad);
		Glob.AdjustSpeed();
    }
    GoopLoad = 0;
    if ( Weapon.AmmoAmount(ThisModeNum) <= 0 )
        Weapon.OutOfAmmo();
    return Glob;
}

function StartBerserk()
{
	if ( (Level.GRI != None) && (Level.GRI.WeaponBerserk > 1.0) )
		return;
    GoopUpRate = default.GoopUpRate*0.75;
}

function StopBerserk()
{
	if ( (Level.GRI != None) && (Level.GRI.WeaponBerserk > 1.0) )
		return;
    GoopUpRate = default.GoopUpRate;
}

function StartSuperBerserk()
{
    GoopUpRate = default.GoopUpRate/Level.GRI.WeaponBerserk;
}

defaultproperties
{
    AmmoClass=class'BioAmmo'
    AmmoPerFire=0

    FireAnim=Fire
    FireEndAnim=None

    ProjectileClass=class'XWeapons.BioGlob'
    FireRate=0.33
    bFireOnRelease=true
    GoopUpRate=0.25
    MaxGoopLoad=10

    ProjSpawnOffset=(X=20,Y=9,Z=-6)
    FlashEmitterClass=None
    FireSound=Sound'WeaponSounds.BioRifle.BioRifleFire'
    HoldSound=Sound'WeaponSounds.BioRifle.BioRiflePowerUp'

    FireForce="BioRifleFire";  // jdf

    bSplashDamage=true
    bRecommendSplashDamage=true
    BotRefireRate=0.5
    bTossed=true
    WarnTargetPct=+0.8

    ShakeOffsetMag=(X=-4.0,Y=0.0,Z=-4.0)
    ShakeOffsetRate=(X=1000.0,Y=0.0,Z=1000.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=100.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=1000.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

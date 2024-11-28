class ClassicSniperFire extends InstantFire;

var() float HeadShotDamageMult;
var() class<DamageType> DamageTypeHeadShot;
var name FireAnims[3];

function InitEffects()
{
    Super.InitEffects();
    if ( FlashEmitter != None )
		Weapon.AttachToBone(FlashEmitter, 'dummy01');
}

function FlashMuzzleFlash()
{
    local rotator r;

    r.Yaw = 16384;
    Weapon.SetBoneRotation('dummy01', r, 0, 1.f);
    Super.FlashMuzzleFlash();
}

function DoTrace(Vector Start, Rotator Dir)
{
    local Vector X,Y,Z, End, HitLocation, HitNormal, ArcEnd;
    local Actor Other;
    local SniperWallHitEffect S;
    local Pawn HeadShotPawn;

    Weapon.GetViewAxes(X, Y, Z);
    if ( Weapon.WeaponCentered() )
        ArcEnd = (Instigator.Location +
			Weapon.EffectOffset.X * X +
			1.5 * Weapon.EffectOffset.Z * Z);
	else
        ArcEnd = (Instigator.Location +
			Instigator.CalcDrawOffset(Weapon) +
			Weapon.EffectOffset.X * X +
			Weapon.Hand * Weapon.EffectOffset.Y * Y +
			Weapon.EffectOffset.Z * Z);

    X = Vector(Dir);
    End = Start + TraceRange * X;
    Other = Weapon.Trace(HitLocation, HitNormal, End, Start, true);
    if ( (Level.NetMode != NM_Standalone) || (PlayerController(Instigator.Controller) == None) )
		Weapon.Spawn(class'TracerProjectile',Instigator.Controller,,Start,Dir);
    if ( Other != None && (Other != Instigator) )
    {
        if ( !Other.bWorldGeometry )
        {
            if (Vehicle(Other) != None)
                HeadShotPawn = Vehicle(Other).CheckForHeadShot(HitLocation, X, 1.0);

            if (HeadShotPawn != None)
                HeadShotPawn.TakeDamage(DamageMax * HeadShotDamageMult, Instigator, HitLocation, Momentum*X, DamageTypeHeadShot);
 			else if ( (Pawn(Other) != None) && Pawn(Other).IsHeadShot(HitLocation, X, 1.0))
                Other.TakeDamage(DamageMax * HeadShotDamageMult, Instigator, HitLocation, Momentum*X, DamageTypeHeadShot);
            else
                Other.TakeDamage(DamageMax, Instigator, HitLocation, Momentum*X, DamageType);
        }
        else
				HitLocation = HitLocation + 2.0 * HitNormal;
    }
    else
    {
        HitLocation = End;
        HitNormal = Normal(Start - End);
    }

    if ( (HitNormal != Vect(0,0,0)) && (HitScanBlockingVolume(Other) == None) )
    {
		S = Weapon.Spawn(class'SniperWallHitEffect',,, HitLocation, rotator(-1 * HitNormal));
		if ( S != None )
			S.FireStart = Start;
	}
}

function PlayFiring()
{
	Weapon.PlayAnim(FireAnims[Rand(3)], FireAnimRate, TweenTime);
    Weapon.PlayOwnedSound(FireSound,SLOT_Interact,TransientSoundVolume,,,Default.FireAnimRate/FireAnimRate,false);
    ClientPlayForceFeedback(FireForce);
    FireCount++;
}

defaultproperties
{
    AmmoClass=class'ClassicSniperAmmo'
    AmmoPerFire=1
    DamageType=class'DamTypeClassicSniper'
    DamageTypeHeadShot=class'DamTypeClassicHeadShot'
    DamageMin=60
    DamageMax=60
    FireSound=Sound'NewWeaponSounds.NewSniperShot'
    FireForce="NewSniperShot"  // jdf
    TraceRange=17000
    FireRate=1.33
	FireAnimRate=1.5

    FlashEmitterClass=class'XEffects.AssaultMuzFlash1st'

    BotRefireRate=0.4
    AimError=850
	WarnTargetPct=+0.5

    HeadShotDamageMult=2.0

    ShakeOffsetMag=(X=-15.0,Y=0.0,Z=10.0)
    ShakeOffsetRate=(X=-4000.0,Y=0.0,Z=4000.0)
    ShakeOffsetTime=1.6
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2

    FireAnims(0)=Fire1
    FireAnims(1)=Fire2
    FireAnims(2)=Fire3
}


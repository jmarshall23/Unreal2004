class ShockBeamFire extends InstantFire;

var() class<ShockBeamEffect> BeamEffectClass;

#exec OBJ LOAD FILE=..\Sounds\WeaponSounds.uax


function DoFireEffect()
{
    local Vector StartTrace,X,Y,Z;
    local Rotator R, Aim;

    Instigator.MakeNoise(1.0);

    StartTrace = Instigator.Location + Instigator.EyePosition();
    if ( PlayerController(Instigator.Controller) != None )
    {
		// for combos
	   Weapon.GetViewAxes(X,Y,Z);
		StartTrace = StartTrace + X*class'ShockProjFire'.Default.ProjSpawnOffset.X;
		if ( !Weapon.WeaponCentered() )
			StartTrace = StartTrace + Weapon.Hand * Y*class'ShockProjFire'.Default.ProjSpawnOffset.Y + Z*class'ShockProjFire'.Default.ProjSpawnOffset.Z;
	}

    Aim = AdjustAim(StartTrace, AimError);
	R = rotator(vector(Aim) + VRand()*FRand()*Spread);
    DoTrace(StartTrace, R);
}

function InitEffects()
{
	if ( Level.DetailMode == DM_Low )
		FlashEmitterClass = None;
    Super.InitEffects();
    if ( FlashEmitter != None )
		Weapon.AttachToBone(FlashEmitter, 'tip');
}

// for bot combos
function Rotator AdjustAim(Vector Start, float InAimError)
{
	if ( (ShockRifle(Weapon) != None) && (ShockRifle(Weapon).ComboTarget != None) )
		return Rotator(ShockRifle(Weapon).ComboTarget.Location - Start);

	return Super.AdjustAim(Start, InAimError);
}

function SpawnBeamEffect(Vector Start, Rotator Dir, Vector HitLocation, Vector HitNormal, int ReflectNum)
{
    local ShockBeamEffect Beam;

    if (Weapon != None)
    {
        Beam = Weapon.Spawn(BeamEffectClass,,, Start, Dir);
        if (ReflectNum != 0) Beam.Instigator = None; // prevents client side repositioning of beam start
            Beam.AimAt(HitLocation, HitNormal);
    }
}

defaultproperties
{
    TraceRange=17000
	Momentum=+60000.0
    AmmoClass=class'ShockAmmo'
    AmmoPerFire=1
    DamageType=class'DamTypeShockBeam'
    DamageMin=45
    DamageMax=45

    FlashEmitterClass=class'XEffects.ShockBeamMuzFlash'

    FireSound=Sound'WeaponSounds.ShockRifle.ShockRifleFire'
    FireForce="ShockRifleFire"  // jdf

    bReflective=true

    FireRate=0.7
    bModeExclusive=true

    BeamEffectClass=class'ShockBeamEffect'

    BotRefireRate=0.7
    AimError=700

    ShakeOffsetMag=(X=-8.0,Y=0.00,Z=0.00)
    ShakeOffsetRate=(X=-600.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=3.2
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

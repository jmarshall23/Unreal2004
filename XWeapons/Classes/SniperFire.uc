class SniperFire extends InstantFire;

var() class<xEmitter> HitEmitterClass;
var() class<xEmitter> SecHitEmitterClass;
var() int NumArcs;
var() float SecDamageMult;
var() float SecTraceDist;
var() float HeadShotDamageMult;
var() class<DamageType> DamageTypeHeadShot;
var(tweak) float offsetadj;

function DoTrace(Vector Start, Rotator Dir)
{
    local Vector X,Y,Z, End, HitLocation, HitNormal, RefNormal;
    local Actor Other, mainArcHitTarget;
    local int Damage, ReflectNum, arcsRemaining;
    local bool bDoReflect;
    local xEmitter hitEmitter;
    local class<Actor> tmpHitEmitClass;
    local float tmpTraceRange;
    local vector arcEnd, mainArcHit;
    local Pawn HeadShotPawn;
	local vector EffectOffset;
	
	if ( class'PlayerController'.Default.bSmallWeapons )
		EffectOffset = Weapon.SmallEffectOffset;
	else
		EffectOffset = Weapon.EffectOffset;

    Weapon.GetViewAxes(X, Y, Z);
    if ( Weapon.WeaponCentered() )
        arcEnd = (Instigator.Location +
			EffectOffset.X * X +
			1.5 * EffectOffset.Z * Z);
	else if ( Weapon.Hand == 0 )
	{
		if ( class'PlayerController'.Default.bSmallWeapons )
			arcEnd = (Instigator.Location +
				EffectOffset.X * X);
		else
			arcEnd = (Instigator.Location +
				EffectOffset.X * X
				- 0.5 * EffectOffset.Z * Z);
	}
	else
        arcEnd = (Instigator.Location +
			Instigator.CalcDrawOffset(Weapon) +
			EffectOffset.X * X +
			Weapon.Hand * EffectOffset.Y * Y +
			EffectOffset.Z * Z);

    arcsRemaining = NumArcs;

    tmpHitEmitClass = HitEmitterClass;
    tmpTraceRange = TraceRange;

    ReflectNum = 0;
    while (true)
    {
        bDoReflect = false;
        X = Vector(Dir);
        End = Start + tmpTraceRange * X;
        Other = Weapon.Trace(HitLocation, HitNormal, End, Start, true);

        if ( Other != None && (Other != Instigator || ReflectNum > 0) )
        {
            if (bReflective && Other.IsA('xPawn') && xPawn(Other).CheckReflect(HitLocation, RefNormal, DamageMin*0.25))
            {
                bDoReflect = true;
            }
            else if ( Other != mainArcHitTarget )
            {
                if ( !Other.bWorldGeometry )
                {
                    Damage = (DamageMin + Rand(DamageMax - DamageMin)) * DamageAtten;

                    if (Vehicle(Other) != None)
                        HeadShotPawn = Vehicle(Other).CheckForHeadShot(HitLocation, X, 1.0);

                    if (HeadShotPawn != None)
                        HeadShotPawn.TakeDamage(Damage * HeadShotDamageMult, Instigator, HitLocation, Momentum*X, DamageTypeHeadShot);
					else if ( (Pawn(Other) != None) && (arcsRemaining == NumArcs)
						&& Pawn(Other).IsHeadShot(HitLocation, X, 1.0) )
                        Other.TakeDamage(Damage * HeadShotDamageMult, Instigator, HitLocation, Momentum*X, DamageTypeHeadShot);
                    else
                    {
						if ( arcsRemaining < NumArcs )
							Damage *= SecDamageMult;
                        Other.TakeDamage(Damage, Instigator, HitLocation, Momentum*X, DamageType);
					}
                }
                else
					HitLocation = HitLocation + 2.0 * HitNormal;
            }
        }
        else
        {
            HitLocation = End;
            HitNormal = Normal(Start - End);
        }
        if ( Weapon == None )
			return;
        hitEmitter = xEmitter(Weapon.Spawn(tmpHitEmitClass,,, arcEnd, Rotator(HitNormal)));
        if ( hitEmitter != None )
			hitEmitter.mSpawnVecA = HitLocation;
		if ( HitScanBlockingVolume(Other) != None )
			return;

        if( arcsRemaining == NumArcs )
        {
            mainArcHit = HitLocation + (HitNormal * 2.0);
            if ( Other != None && !Other.bWorldGeometry )
                mainArcHitTarget = Other;
        }

        if (bDoReflect && ++ReflectNum < 4)
        {
            //Log("reflecting off"@Other@Start@HitLocation);
            Start = HitLocation;
            Dir = Rotator( X - 2.0*RefNormal*(X dot RefNormal) );
        }
        else if ( arcsRemaining > 0 )
        {
            arcsRemaining--;

            // done parent arc, now move trace point to arc trace hit location and try child arcs from there
            Start = mainArcHit;
            Dir = Rotator(VRand());
            tmpHitEmitClass = SecHitEmitterClass;
            tmpTraceRange = SecTraceDist;
            arcEnd = mainArcHit;
        }
        else
        {
            break;
        }
    }
}

function PlayFiring()
{
	Super.PlayFiring();
    if ( Weapon.AmmoAmount(0) > 0 )
        Weapon.PlayOwnedSound(Sound'WeaponSounds.LightningGunChargeUp', SLOT_Misc,,,,1.1,false);
}

defaultproperties
{
    AmmoClass=class'SniperAmmo'
    AmmoPerFire=1
    DamageType=class'DamTypeSniperShot'
    DamageTypeHeadShot=class'DamTypeSniperHeadShot'
    DamageMin=70
    DamageMax=70
    HitEmitterClass=class'NewLightningBolt'
    SecHitEmitterClass=class'ChildLightningBolt'
    FireSound=Sound'WeaponSounds.BLightningGunFire'
    FireForce="LightningGunFire"  // jdf
    TraceRange=17000
    FireRate=1.6
	FireAnimRate=1.25
    NumArcs=3
    SecDamageMult=0.5
    SecTraceDist=300.0

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
}

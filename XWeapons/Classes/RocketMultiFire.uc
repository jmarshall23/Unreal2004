
class RocketMultiFire extends ProjectileFire;

var() float TightSpread, LooseSpread;
var byte FlockIndex;
var int MaxLoad;

event ModeHoldFire()
{
    if (Instigator.IsLocallyControlled())
		PlayStartHold();
	else
		ServerPlayLoading();
}

simulated function ServerPlayLoading()
{
	RocketLauncher(Weapon).PlayOwnedSound(Sound'WeaponSounds.RocketLauncher.RocketLauncherLoad', SLOT_None,,,,,false);
}

function PlayFireEnd()
{
}

function PlayStartHold()
{
    RocketLauncher(Weapon).PlayLoad(false);
}

function PlayFiring()
{
    if (Load > 1.0)
        FireAnim = 'AltFire';
    else
        FireAnim = 'Fire';
    Super.PlayFiring();
    RocketLauncher(Weapon).PlayFiring((Load == MaxLoad));
	Weapon.OutOfAmmo();
}

event ModeDoFire()
{
    if ( RocketLauncher(Weapon).bTightSpread || ((Bot(Instigator.Controller) != None) && (FRand() < 0.65)) )
    {
        Spread = TightSpread;
		SpreadStyle = SS_Ring;
    }
    else
    {
		SpreadStyle = SS_Line;
        Spread = LooseSpread;
    }
    RocketLauncher(Weapon).bTightSpread = false;
    Super.ModeDoFire();
	NextFireTime = FMax(NextFireTime, Level.TimeSeconds + FireRate);
}

function DoFireEffect()
{
    local Vector StartProj, StartTrace, X,Y,Z;
    local Rotator Aim;
    local Vector HitLocation, HitNormal,FireLocation;
    local Actor Other;
    local int p,q, SpawnCount, i;
	local RocketProj FiredRockets[4];
	local bool bCurl;
	
	if ( (SpreadStyle == SS_Line) || (Load < 2) )
	{
		Super.DoFireEffect();
		return;
	}
	
    Instigator.MakeNoise(1.0);
    Weapon.GetViewAxes(X,Y,Z);

    StartTrace = Instigator.Location + Instigator.EyePosition();
    StartProj = StartTrace + X*ProjSpawnOffset.X + Z*ProjSpawnOffset.Z;
    if ( !Weapon.WeaponCentered() )
	    StartProj = StartProj + Weapon.Hand * Y*ProjSpawnOffset.Y;

    // check if projectile would spawn through a wall and adjust start location accordingly
    Other = Weapon.Trace(HitLocation, HitNormal, StartProj, StartTrace, false);
    if (Other != None)
    {
        StartProj = HitLocation;
    }
    
    Aim = AdjustAim(StartProj, AimError);

    SpawnCount = Max(1, int(Load));

    for ( p=0; p<SpawnCount; p++ )
    {
 		Firelocation = StartProj - 2*((Sin(p*2*PI/MaxLoad)*8 - 7)*Y - (Cos(p*2*PI/MaxLoad)*8 - 7)*Z) - X * 8 * FRand();
        FiredRockets[p] = RocketProj(SpawnProjectile(FireLocation, Aim));
    }
    
    if ( SpawnCount < 2 )
		return;
	
	FlockIndex++;
	if ( FlockIndex == 0 )
		FlockIndex = 1;
		
    // To get crazy flying, we tell each projectile in the flock about the others.
    for ( p = 0; p < SpawnCount; p++ )
    {
		if ( FiredRockets[p] != None )
		{
			FiredRockets[p].bCurl = bCurl;
			FiredRockets[p].FlockIndex = FlockIndex;
			i = 0;
			for ( q=0; q<SpawnCount; q++ )
				if ( (p != q) && (FiredRockets[q] != None) )
				{
					FiredRockets[p].Flock[i] = FiredRockets[q];
					i++;
				}	
			bCurl = !bCurl;
			if ( Level.NetMode != NM_DedicatedServer )
				FiredRockets[p].SetTimer(0.1, true);
		}
	}
}

function ModeTick(float dt)
{
    // auto fire if loaded last rocket
    if (HoldTime > 0.0 && Load >= Weapon.AmmoAmount(ThisModeNum) && !bNowWaiting)
    {
        bIsFiring = false;
    }

    Super.ModeTick(dt);

    if (Load == 1.0 && HoldTime >= FireRate)
    {
        if (Instigator.IsLocallyControlled())
        	RocketLauncher(Weapon).PlayLoad(false);
		else
			ServerPlayLoading();
			
        Load = Load + 1.0;
    }
    else if (Load == 2.0 && HoldTime >= FireRate*2.0)
    {
        Load = Load + 1.0;
    }
}

function InitEffects()
{
    Super.InitEffects();
    if ( FlashEmitter != None )
		Weapon.AttachToBone(FlashEmitter, 'tip');
}

function Projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local Projectile p;    
    
    p = RocketLauncher(Weapon).SpawnProjectile(Start, Dir);
    if ( P != None )
		p.Damage *= DamageAtten;
    return p;
}

defaultproperties
{
    AmmoClass=class'RocketAmmo'
    AmmoPerFire=1

    FireAnim=AltFire
    FireAnimRate=1.0

    ProjectileClass=class'XWeapons.RocketProj'

    FlashEmitterClass=class'XEffects.RocketMuzFlash1st'

    SpreadStyle=SS_Line
    TightSpread=300
    LooseSpread=1000

    ProjSpawnOffset=(X=25,Y=6,Z=-6)

    FireSound=Sound'WeaponSounds.RocketLauncher.RocketLauncherFire'

    FireForce="RocketLauncherFire"  // jdf

    FireRate=0.95
    TweenTime=0.0

    bFireOnRelease=true
    MaxHoldTime=2.3 // FireRate*2 + 0.5

    bSplashDamage=true
    bRecommendSplashDamage=true
    BotRefireRate=0.6
	WarnTargetPct=+0.9
    bSplashJump=true

    ShakeOffsetMag=(X=-20.0,Y=0.00,Z=0.00)
    ShakeOffsetRate=(X=-1000.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
    MaxLoad=3
}

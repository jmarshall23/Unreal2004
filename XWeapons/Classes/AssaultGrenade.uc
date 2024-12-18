class AssaultGrenade extends ProjectileFire;

#exec OBJ LOAD FILE=..\Sounds\NewWeaponSounds.uax

const               mNumGrenades = 8;
var float           mCurrentRoll;
var float           mBlend;
var float           mRollInc;
var float           mNextRoll;
var float           mDrumRotationsPerSec;
var float           mRollPerSec;
var AssaultRifle    mGun;
var int             mCurrentSlot;
var int             mNextEmptySlot;

var() float         mScale;
var() float         mScaleMultiplier;

var() float         mSpeedMin;
var() float         mSpeedMax;
var() float         mHoldSpeedMin;
var() float         mHoldSpeedMax;
var() float         mHoldSpeedGainPerSec;
var() float         mHoldClampMax;
var float ClickTime;

var() float         mWaitTime;

function PostBeginPlay()
{
    Super.PostBeginPlay();
    mRollInc = -1.f * 65536.f / mNumGrenades;
    mRollPerSec = 65536.f * mDrumRotationsPerSec;
    mGun = AssaultRifle(Weapon);
    mHoldClampMax = (mHoldSpeedMax - mHoldSpeedMin) / mHoldSpeedGainPerSec;
    FireRate = mWaitTime + 1.f / (mDrumRotationsPerSec * mNumGrenades);
}

function DrawMuzzleFlash(Canvas Canvas) {}

simulated function bool AllowFire()
{
    if (Super.AllowFire())
        return true;
    else
    {
        if ( (PlayerController(Instigator.Controller) != None) && (Level.TimeSeconds > ClickTime) )
        {
            Instigator.PlaySound(Sound'NewWeaponSounds.newclickgrenade');
	        ClickTime = Level.TimeSeconds + 0.25;
	    }
        return false;
    }
}

function InitEffects()
{
    Super.InitEffects();
    if ( FlashEmitter != None )
		Weapon.AttachToBone(FlashEmitter, 'tip2');
}


function DoFireEffect()
{
    local Vector StartProj, StartTrace, X,Y,Z;
    local Rotator Aim;
    local Vector HitLocation, HitNormal;
    local Actor Other;

    Instigator.MakeNoise(1.0);
    Weapon.GetViewAxes(X,Y,Z);

    StartTrace = Instigator.Location + Instigator.EyePosition();// + X*Instigator.CollisionRadius;
    StartProj = StartTrace + X*ProjSpawnOffset.X;
    if ( AssaultRifle(Weapon).bDualMode )
    {
		AssaultRifle(Weapon).bFireLeft = !AssaultRifle(Weapon).bFireLeft;
		if ( AssaultRifle(Weapon).bFireLeft )
			Y *= -1;
	}
    if ( !Weapon.WeaponCentered() )
	    StartProj = StartProj + Weapon.Hand * Y*ProjSpawnOffset.Y + Z*ProjSpawnOffset.Z;

    // check if projectile would spawn through a wall and adjust start location accordingly
    Other = Weapon.Trace(HitLocation, HitNormal, StartProj, StartTrace, false);
    if (Other != None)
    {
        StartProj = HitLocation;
    }

    Aim = AdjustAim(StartProj, AimError);
    SpawnProjectile(StartProj, Aim);
}

function projectile SpawnProjectile(Vector Start, Rotator Dir)
{
    local Grenade g;
    local vector X, Y, Z;
    local float pawnSpeed;

    g = Weapon.Spawn(class'Grenade', instigator,, Start, Dir);
    if (g != None)
    {
        Weapon.GetViewAxes(X,Y,Z);
        pawnSpeed = X dot Instigator.Velocity;

		if ( Bot(Instigator.Controller) != None )
			g.Speed = mHoldSpeedMax;
		else
			g.Speed = mHoldSpeedMin + HoldTime*mHoldSpeedGainPerSec;
		g.Speed = FClamp(g.Speed, mHoldSpeedMin, mHoldSpeedMax);
        g.Speed = pawnSpeed + g.Speed;
        g.Velocity = g.Speed * Vector(Dir);

        g.Damage *= DamageAtten;
    }
    return g;
}

simulated function ReturnToIdle()
{
    UpdateRoll(FireRate);
    GotoState('Idle');
}

// Client-side only: update the first person drum rotation
simulated function bool UpdateRoll(float dt)
{
    local rotator r;
    local bool bDone;
    local float diff;
    local float inc;

    diff = mCurrentRoll - mNextRoll;
    inc = dt*mRollPerSec;
    if (inc >= diff)
    {
        inc = diff;
        bDone = true;
    }

    mCurrentRoll -= inc;
    mCurrentRoll = mCurrentRoll % 65536.f;
    r.Roll = int(mCurrentRoll);

    mGun.SetBoneRotation('Bone_Drum', r, 0, mBlend);

    return bDone;
}

simulated function int WrapPostIncr(out int count)
{
    local int oldcount;
    oldcount = count;
    if (++count >= mNumGrenades)
        count = 0;
    return oldcount;
}

function PlayPreFire()   {}
function PlayStartHold() {}
function PlayFiring()    {}
function PlayFireEnd()   {}

auto state Idle
{
    function StopFiring()
    {
        local rotator r;

        if (Instigator.Weapon != Weapon)
            return;

        r.Roll = Rand(65536);
        Weapon.SetBoneRotation('Bone_Flash02', r, 0, 1.f);

        mNextRoll = mCurrentRoll + mRollInc;
        mGun.PlayAnim(FireAnim, FireAnimRate, 0.0);

        if ( (Level.NetMode != NM_Client) && (Instigator.DrivenVehicle == None) )
            Weapon.PlaySound(FireSound,SLOT_Interact,TransientSoundVolume,,512.0,,false);

        ClientPlayForceFeedback(FireForce);  // jdf

        GotoState('Wait');

        Super.StopFiring();
    }
}

state Wait
{
    function BeginState()
    {
        SetTimer(mWaitTime, false);
    }

    function Timer()
    {
		GotoState('LoadNext'); //goto idle if out of ammo?
    }
}

state LoadNext
{
    function BeginState()
    {
        if (Level.NetMode != NM_Client)
            Weapon.PlaySound(ReloadSound,SLOT_None,,,512.0,,false);
        ClientPlayForceFeedback(ReloadForce);
    }

    function ModeTick(float dt)
    {
		if ( Weapon.Mesh != Weapon.OldMesh )
			GotoState('Idle');
        else if (UpdateRoll(dt))
            GotoState('Idle');
    }
}

function StartBerserk()
{
    mHoldSpeedGainPerSec = default.mHoldSpeedGainPerSec * 1.5;
    mHoldClampMax = (mHoldSpeedMax - mHoldSpeedMin) / mHoldSpeedGainPerSec;
}

function StopBerserk()
{
    mHoldSpeedGainPerSec = default.mHoldSpeedGainPerSec;
    mHoldClampMax = (mHoldSpeedMax - mHoldSpeedMin) / mHoldSpeedGainPerSec;
}

function StartSuperBerserk()
{
    mHoldSpeedGainPerSec = default.mHoldSpeedGainPerSec * Level.GRI.WeaponBerserk;
    mHoldClampMax = (mHoldSpeedMax - mHoldSpeedMin) / mHoldSpeedGainPerSec;
    FireRate = mWaitTime + 1.f / (mDrumRotationsPerSec * mNumGrenades) / Level.GRI.WeaponBerserk;
}

defaultproperties
{
    mWaitTime=0.5
    mHoldSpeedMin=850
    mHoldSpeedMax=1600
    mHoldSpeedGainPerSec=750
    mSpeedMin=250.f
    mSpeedMax=3000.f
    mScaleMultiplier=0.9f
    mScale=1.f
    mBlend=1.f
    mDrumRotationsPerSec=0.4f

    AmmoClass=class'GrenadeAmmo'
    AmmoPerFire=1

    FireAnim=Fire
    FireAnimRate=0.5
    FireEndAnim=None
    FireLoopAnim=None

    FlashEmitterClass=class'XEffects.AssaultMuzFlash1st'

    ProjectileClass=class'XWeapons.Grenade'

    ProjSpawnOffset=(X=25,Y=10,Z=-7)

    FireSound=Sound'NewWeaponSounds.NewGrenadeShoot'
    ReloadSound=Sound'WeaponSounds.BReload9'
    FireForce="AssaultRifleAltFire"
    ReloadForce="BReload9"

    PreFireTime=0.0
    FireRate=1.0
    MaxHoldTime=0
    bModeExclusive=true
    bFireOnRelease=true

    bSplashDamage=true
    bRecommendSplashDamage=true
    BotRefireRate=0.25
    bTossed=true

    ShakeOffsetMag=(X=-20.0,Y=0.00,Z=0.00)
    ShakeOffsetRate=(X=-1000.0,Y=0.0,Z=0.0)
    ShakeOffsetTime=2
    ShakeRotMag=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotRate=(X=0.0,Y=0.0,Z=0.0)
    ShakeRotTime=2
}

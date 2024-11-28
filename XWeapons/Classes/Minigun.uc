//=============================================================================
// Minigun
//=============================================================================
class Minigun extends Weapon
    config(user);

#EXEC OBJ LOAD FILE=InterfaceContent.utx

var     float           CurrentRoll;
var     float           RollSpeed;
//var     float           FireTime;
var() xEmitter          ShellCaseEmitter;
var() vector            AttachLoc;
var() rotator           AttachRot;
var   int               CurrentMode;
var() float             GearRatio;
var() float             GearOffset;
var() float             Blend;

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();

    if ( Level.NetMode == NM_DedicatedServer )
        return;

    ShellCaseEmitter = spawn(class'ShellSpewer');
	if ( ShellCaseEmitter != None )
	{
		ShellCaseEmitter.Trigger(Self, Instigator); //turn off
		AttachToBone(ShellCaseEmitter, 'shell');
	}
}

function DropFrom(vector StartLocation)
{
	Super.DropFrom(StartLocation);
}

simulated function OutOfAmmo()
{
    if ( (Instigator == None) || !Instigator.IsLocallyControlled() || HasAmmo() )
        return;

	Instigator.AmbientSound = None;
	Instigator.SoundVolume = Instigator.default.SoundVolume;
    DoAutoSwitch();
}

simulated function Destroyed()
{
    if (ShellCaseEmitter != None)
    {
        ShellCaseEmitter.Destroy();
        ShellCaseEmitter = None;
    }

    Super.Destroyed();
}

/* BotFire()
called by NPC firing weapon. Weapon chooses appropriate firing Mode to use (typically no change)
bFinished should only be true if called from the Finished() function
FiringMode can be passed in to specify a firing Mode (used by scripted sequences)
*/
function bool BotFire(bool bFinished, optional name FiringMode)
{
    local int newmode;
    local Controller C;

    C = Instigator.Controller;
	newMode = BestMode();

	if ( newMode == 0 )
	{
		C.bFire = 1;
		C.bAltFire = 0;
	}
	else
	{
		C.bFire = 0;
		C.bAltFire = 1;
	}

	if ( bFinished )
		return true;

    if ( FireMode[BotMode].bIsFiring && (NewMode != BotMode) )
		StopFire(BotMode);

    if ( !ReadyToFire(newMode) || ClientState != WS_ReadyToFire )
		return false;

    BotMode = NewMode;
    StartFire(NewMode);
    return true;
}

// AI Interface
function float GetAIRating()
{
	local Bot B;
	
	B = Bot(Instigator.Controller);
	if ( (B== None) || (B.Enemy == None) )
		return AIRating;
		
	if ( !B.EnemyVisible() )
		return AIRating - 0.15;

	return AIRating * FMin(Pawn(Owner).DamageScaling, 1.5);
}

/* BestMode()
choose between regular or alt-fire
*/
function byte BestMode()
{
	local float EnemyDist;
	local bot B;

	B = Bot(Instigator.Controller);
	if ( (B == None) || (B.Enemy == None) )
		return 0;

	if ( FireMode[0].bIsFiring )
		return 0;
	else if ( FireMode[1].bIsFiring )
		return 1;
	EnemyDist = VSize(B.Enemy.Location - Instigator.Location);
	if ( EnemyDist < 2000 )
		return 0;
	return 1;
}
// end AI Interface

simulated function SpawnShells(float amountPerSec)
{
    if(ShellCaseEmitter == None || !FirstPersonView())
        return;
	if ( Bot(Instigator.Controller) != None )
	{
		ShellCaseEmitter.Destroy();
		return;
	}

	ShellCaseEmitter.mRegenRange[0] = amountPerSec;
	ShellCaseEmitter.mRegenRange[1] = amountPerSec;
    ShellCaseEmitter.Trigger(self, Instigator);
}

simulated function bool FirstPersonView()
{
    return (Instigator.IsLocallyControlled() && (PlayerController(Instigator.Controller) != None) && !PlayerController(Instigator.Controller).bBehindView);
}

// Prevents wrong anims from playing
simulated function AnimEnd(int channel)
{
}

// Client-side only: update the first person barrel rotation
simulated function UpdateRoll(float dt, float speed, int mode)
{
    local rotator r;

    if (Level.NetMode == NM_DedicatedServer)
        return;

    if (mode == CurrentMode) // to limit to one mode
    {
       // log(self$" updateroll (mode="$mode$") speed="$speed);

        RollSpeed = speed;
        CurrentRoll += dt*RollSpeed;
        CurrentRoll = CurrentRoll % 65536.f;
        r.Roll = int(CurrentRoll);
        SetBoneRotation('Bone Barrels', r, 0, Blend);

        r.Roll = GearOffset + r.Roll*GearRatio;
        SetBoneRotation('Bone gear', r, 0, Blend);
    }
}

simulated function bool StartFire(int mode)
{
    local bool bStart;

	if ( !MinigunFire(FireMode[0]).IsIdle() || !MinigunFire(FireMode[1]).IsIdle() )
		return false;

    bStart = Super.StartFire(mode);
    if (bStart)
        FireMode[mode].StartFiring();

    return bStart;
}

// Allow fire modes to return to idle on weapon switch (server)
simulated function DetachFromPawn(Pawn P)
{
    //log(self$" detach from pawn p="$p);

    ReturnToIdle();

    Super.DetachFromPawn(P);
}

// Allow fire modes to return to idle on weapon switch (client)
simulated function bool PutDown()
{
   // log(self$" putdown");

    ReturnToIdle();

    return Super.PutDown();
}

simulated function ReturnToIdle()
{
    local int mode;

    for (mode=0; mode<NUM_FIRE_MODES; mode++)
    {
        if (FireMode[mode] != None)
        {
            FireMode[mode].GotoState('Idle');
        }
    }
}

defaultproperties
{
	HighDetailOverlay=Material'UT2004Weapons.WeaponSpecMap2'
    ItemName="Minigun"
    Description="The Schultz-Metzger T23-A 23mm rotary cannon is capable of firing both high-velocity caseless ammunition and cased rounds. With an unloaded weight of only 8 kilograms, the T23 is portable and maneuverable, easily worn across the back when employing the optional carrying strap.|The T23-A is the rotary cannon of choice for the discerning soldier."
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=246,Y1=80,X2=332,Y2=106)

    Blend=1.f
    GearRatio=-2.37
    GearOffset=0.0
    AttachLoc=(X=-77,Y=6,Z=4)
    AttachRot=(Pitch=22000,Roll=0,Yaw=-16384)
    FireModeClass(0)=MinigunFire
    FireModeClass(1)=MinigunAltFire
    InventoryGroup=6
    Mesh=mesh'Weapons.Minigun_1st'
    BobDamping=2.25
    PickupClass=class'MinigunPickup'
    EffectOffset=(X=100.0,Y=18.0,Z=-16.0)
    DisplayFOV=60
    PutDownAnim=PutDown
    DrawScale=0.4
    PlayerViewOffset=(X=2,Y=-1,Z=0)
    SmallViewOffset=(X=8,Y=1,Z=-2)
    PlayerViewPivot=(Pitch=0,Roll=0,Yaw=500)
    AttachmentClass=class'MinigunAttachment'
    SoundRadius=400.0
    SelectSound=Sound'WeaponSounds.Minigun.SwitchToMinigun'
	SelectForce="SwitchToMiniGun"

	AIRating=+0.71
	CurrentRating=+0.71

    bDynamicLight=false
    LightType=LT_Pulse
    LightEffect=LE_NonIncidence
    LightPeriod=3
    LightBrightness=255
    LightHue=30
    LightSaturation=150
    LightRadius=5.0

    HudColor=(r=255,g=255,b=255,a=255)
	CustomCrosshair=12
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Circle1"
	CustomCrosshairColor=(r=255,g=255,b=255,a=255)
	Priority=9

	CenteredOffsetY=-6.0
	CenteredYaw=-500
	CenteredRoll=0
}

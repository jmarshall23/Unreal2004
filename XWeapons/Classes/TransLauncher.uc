//=============================================================================
// Translocator Launcher
//=============================================================================
class TransLauncher extends Weapon
    config(user)
    HideDropDown;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

var TransBeacon TransBeacon;
var() float     MaxCamDist;
var() float AmmoChargeF;
var int RepAmmo;
var() float AmmoChargeMax;
var() float AmmoChargeRate;
var globalconfig bool bPrevWeaponSwitch;
var xBombFlag Bomb;
var bool bDrained;
var bool bBeaconDeployed; // meaningful for client
var bool bTeamSet;
var byte ViewBeaconVolume;
var float PreDrainAmmo;
var rotator TranslocRot;
var float TranslocScale, OldTime;

replication
{
    reliable if ( bNetOwner && (ROLE==ROLE_Authority) )
        TransBeacon,RepAmmo;
}

simulated function bool HasAmmo()
{
    return true;
}

// AI Interface...
function GiveTo(Pawn Other, optional Pickup Pickup)
{
	Super.GiveTo(Other, Pickup);

	if ( Bot(Other.Controller) != None )
		Bot(Other.Controller).bHasTranslocator = true;
}

function bool ShouldTranslocatorHop(Bot B)
{
	local float dist;
	local Actor N;
	local bool bHop;

	bHop = B.bTranslocatorHop;
	B.bTranslocatorHop = false;

	if ( bHop && (B.Focus == B.TranslocationTarget) && (B.NextTranslocTime < Level.TimeSeconds)
		 && B.InLatentExecution(B.LATENT_MOVETOWARD) && B.Squad.AllowTranslocationBy(B) )
	{
		if ( (TransBeacon != None) && TransBeacon.IsMonitoring(B.Focus) )
			return false;
		dist = VSize(B.TranslocationTarget.Location - B.Pawn.Location);
		if ( dist < 300 )
		{
			// see if next path is possible
			N = B.AlternateTranslocDest();
			if ( (N == None) || ((vector(B.Rotation) Dot Normal(N.Location - B.Pawn.Location)) < 0.5)  )
			{
				if ( dist < 200 )
				{
					B.TranslocationTarget = None;
					B.RealTranslocationTarget = None;
					return false;
				}
			}
			else
			{
				B.TranslocationTarget = N;
				B.RealTranslocationTarget = B.TranslocationTarget;
				B.Focus = N;
				return true;
			}
		}
		if ( (vector(B.Rotation) Dot Normal(B.TranslocationTarget.Location - B.Pawn.Location)) < 0.5 )
		{
			SetTimer(0.1,false);
			return false;
		}
		return true;
	}
	return false;
}

simulated function Timer()
{
	local Bot B;

	if ( Instigator != None )
	{
		B = Bot(Instigator.Controller);
		if ( (B != None) && (B.TranslocationTarget != None) && (B.bPreparingMove || ShouldTranslocatorHop(B)) )
			FireHack(0);
	}
	Super.Timer();
}

function FireHack(byte Mode)
{
	local Actor TTarget;
	local vector TTargetLoc;

	if ( Mode == 0 )
	{
		if ( TransBeacon != None )
		{
			// this shouldn't happen
			TransBeacon.bNoAI = true;
			TransBeacon.Destroy();
			TransBeacon = None;
		}
		TTarget = Bot(Instigator.Controller).TranslocationTarget;
		if ( TTarget == None )
			return;
		// hack in translocator firing here
        FireMode[0].PlayFiring();
        FireMode[0].FlashMuzzleFlash();
        FireMode[0].StartMuzzleSmoke();
        IncrementFlashCount(0);
		ProjectileFire(FireMode[0]).SpawnProjectile(Instigator.Location,Rot(0,0,0));
		// find correct initial velocity
		TTargetLoc = TTarget.Location;
		if ( JumpSpot(TTarget) != None )
		{
			TTargetLoc.Z += JumpSpot(TTarget).TranslocZOffset;
			if ( (Instigator.Anchor != None) && Instigator.ReachedDestination(Instigator.Anchor) )
			{
				// start from same point as in test
				Transbeacon.SetLocation(TransBeacon.Location + Instigator.Anchor.Location + (Instigator.CollisionHeight - Instigator.Anchor.CollisionHeight) * vect(0,0,1)- Instigator.Location);
			}
		}
		else if ( TTarget.Velocity != vect(0,0,0) )
		{
			TTargetLoc += 0.3 * TTarget.Velocity;
			TTargetLoc.Z = 0.5 * (TTargetLoc.Z + TTarget.Location.Z);
		}
		else if ( (Instigator.Physics == PHYS_Falling)
					&& (Instigator.Location.Z < TTarget.Location.Z)
					&& (Instigator.PhysicsVolume.Gravity.Z > -800) )
			TTargetLoc.Z += 128;

		TransBeacon.Velocity = Bot(Instigator.Controller).AdjustToss(TransBeacon.Speed, TransBeacon.Location, TTargetLoc,false);
		TransBeacon.SetTranslocationTarget(Bot(Instigator.Controller).RealTranslocationTarget);
	}
}

function class<DamageType> GetDamageType()
{
	return class'DamTypeTelefrag';
}

// super desireable for bot waiting to translocate
function float GetAIRating()
{
	local Bot B;

	B = Bot(Instigator.Controller);
	if ( B == None )
		return AIRating;
	if ( B.bPreparingMove && (B.TranslocationTarget != None) )
		return 10;
	if ( B.bTranslocatorHop && ((B.Focus == B.MoveTarget) || ((B.TranslocationTarget != None) && (B.Focus == B.TranslocationTarget))) && B.Squad.AllowTranslocationBy(B) )
	{
		if ( (TransBeacon != None) && TransBeacon.IsMonitoring(Transbeacon.TranslocationTarget) )
		{
			if ( (GameObject(B.Focus) != None) && (B.Focus != Transbeacon.TranslocationTarget) )
			{
				if ( Instigator.Weapon == self )
					SetTimer(0.2,false);
				return 4;
			}
			if  ( (Transbeacon.TranslocationTarget == Instigator.Controller.MoveTarget)
					|| (Transbeacon.TranslocationTarget == Instigator.Controller.RouteGoal)
					|| (Transbeacon.TranslocationTarget == Instigator.Controller.RouteCache[0])
					|| (Transbeacon.TranslocationTarget == Instigator.Controller.RouteCache[1])
					|| (Transbeacon.TranslocationTarget == Instigator.Controller.RouteCache[2])
					|| (Transbeacon.TranslocationTarget == Instigator.Controller.RouteCache[3]) )
				return 4;
		}

		if ( Instigator.Weapon == self )
			SetTimer(0.2,false);
		return 4;
	}
	if ( Instigator.Weapon == self )
	return AIRating;
}

function bool BotFire(bool bFinished, optional name FiringMode)
{
	return false;
}

// End AI interface

function bool ConsumeAmmo(int mode, float load, optional bool bAmountNeededIsMax)
{
	return true;
}

function ReduceAmmo()
{
	enable('Tick');
	bDrained = false;
    AmmoChargeF -= 1;
    RepAmmo -= 1;
    if ( Bot(Instigator.Controller) != None )
    	Bot(Instigator.Controller).TranslocFreq = 3 + FMax(Bot(Instigator.Controller).TranslocFreq,Level.TimeSeconds);
}

simulated function GetAmmoCount(out float MaxAmmoPrimary, out float CurAmmoPrimary)
{
	MaxAmmoPrimary = AmmoChargeMax;
	CurAmmoPrimary = FMax(0,AmmoChargeF);
}

function GiveAmmo(int m, WeaponPickup WP, bool bJustSpawned)
{
    Super.GiveAmmo(m, WP,bJustSpawned);
    AmmoChargeF = Default.AmmoChargeF;
    RepAmmo = int(AmmoChargeF);
}

function DrainCharges()
{
	enable('Tick');
	PreDrainAmmo = AmmoChargeF;
	AmmoChargeF = -1;
	RepAmmo = -1;
	bDrained = true;
    if ( Bot(Instigator.Controller) != None )
    	Bot(Instigator.Controller).NextTranslocTime = Level.TimeSeconds + 3.5;
}

simulated function bool StartFire(int Mode)
{
	if ( !bPrevWeaponSwitch || (Mode == 1) || (Instigator.Controller.bAltFire == 0) || (PlayerController(Instigator.Controller) == None) )
		return Super.StartFire(Mode);
	if ( (OldWeapon != None) && OldWeapon.HasAmmo() )
	    Instigator.PendingWeapon = OldWeapon;
	ClientStopFire(0);
	Instigator.Controller.StopFiring();
	PutDown();
    return false;
}

simulated function Tick(float dt)
{
    if (Role == ROLE_Authority)
    {
		if ( AmmoChargeF >= AmmoChargeMax )
		{
			if ( RepAmmo != int(AmmoChargeF) ) // condition to avoid unnecessary bNetDirty of ammo
				RepAmmo = int(AmmoChargeF);
			disable('Tick');
			return;
		}
		AmmoChargeF += dt*AmmoChargeRate;
		AmmoChargeF = FMin(AmmoChargeF, AmmoChargeMax);
		if ( AmmoChargeF >= 1.5 )
			bDrained = false;
		else if ( bDrained )
		{
			if ( (Bomb == None) && ( Level.Game.IsA('xBombingRun') ) )
				Bomb = xBombFlag(UnrealMPGameInfo(Level.Game).GetGameObject('xBombFlag'));
			if ( (Bomb != None) && (Bomb.Holder != None) )
			{
				bDrained = false;
				AmmoChargeF = 1.5;
                if ( Bot(Instigator.Controller) != None )
    	            Bot(Instigator.Controller).NextTranslocTime = Level.TimeSeconds - 1;
			}
		}
        if ( RepAmmo != int(AmmoChargeF) ) // condition to avoid unnecessary bNetDirty of ammo
			RepAmmo = int(AmmoChargeF);
    }
    else
    {
        // client simulation of the charge bar
        AmmoChargeF = FMin(RepAmmo + AmmoChargeF - int(AmmoChargeF)+dt*AmmoChargeRate, AmmoChargeMax);
    }
}

simulated function DoAutoSwitch()
{
}

simulated function ViewPlayer()
{
    if ( (PlayerController(Instigator.Controller) != None) && PlayerController(Instigator.Controller).ViewTarget == TransBeacon )
    {
        PlayerController(Instigator.Controller).ClientSetViewTarget( Instigator );
        PlayerController(Instigator.Controller).SetViewTarget( Instigator );
        Transbeacon.SetRotation(PlayerController(Instigator.Controller).Rotation);
		Transbeacon.SoundVolume = Transbeacon.default.SoundVolume;
    }
}

simulated function ViewCamera()
{
    if ( TransBeacon!=None )
    {
        if ( Instigator.Controller.IsA('PlayerController') )
        {
            PlayerController(Instigator.Controller).SetViewTarget(TransBeacon);
            PlayerController(Instigator.Controller).ClientSetViewTarget(TransBeacon);
            PlayerController(Instigator.Controller).SetRotation( TransBeacon.Rotation );
			Transbeacon.SoundVolume = ViewBeaconVolume;
        }
    }
}

simulated function Reselect()
{
	if ( (TransBeacon == None) || (Instigator.Controller.IsA('PlayerController') && (PlayerController(Instigator.Controller).ViewTarget == TransBeacon)) )
        ViewPlayer();
    else
        ViewCamera();
}

simulated event RenderOverlays( Canvas Canvas )
{
	local float tileScaleX, tileScaleY, dist, clr;
	local float NewTranslocScale;

	if ( PlayerController(Instigator.Controller).ViewTarget == TransBeacon )
    {
		tileScaleX = Canvas.SizeX / 640.0f;
		tileScaleY = Canvas.SizeY / 480.0f;

        Canvas.DrawColor.R = 255;
		Canvas.DrawColor.G = 255;
		Canvas.DrawColor.B = 255;
		Canvas.DrawColor.A = 255;

        Canvas.Style = 255;
		Canvas.SetPos(0,0);
        Canvas.DrawTile( Material'TransCamFB', Canvas.SizeX, Canvas.SizeY, 0.0, 0.0, 512, 512 ); // !! hardcoded size
        Canvas.SetPos(0,0);

		if ( !Level.IsSoftwareRendering() )
		{
			dist = VSize(TransBeacon.Location - Instigator.Location);
			if ( dist > MaxCamDist )
			{
				clr = 255.0;
			}
			else
			{
				clr = (dist / MaxCamDist);
				clr *= 255.0;
			}
			clr = Clamp( clr, 20.0, 255.0 );
			Canvas.DrawColor.R = clr;
			Canvas.DrawColor.G = clr;
			Canvas.DrawColor.B = clr;
			Canvas.DrawColor.A = 255;
			Canvas.DrawTile( Material'ScreenNoiseFB', Canvas.SizeX, Canvas.SizeY, 0.0, 0.0, 512, 512 ); // !! hardcoded size
		}
	}
    else
    {
		if ( TransBeacon == None )
			NewTranslocScale = 1;
		else
			NewTranslocScale = 0;

		if ( NewTranslocScale != TranslocScale )
		{
			TranslocScale = NewTranslocScale;
			SetBoneScale(0,TranslocScale,'Beacon');
		}
		if ( TranslocScale != 0 )
		{
			TranslocRot.Yaw += 120000 * (Level.TimeSeconds - OldTime);
			OldTime = Level.TimeSeconds;
			SetBoneRotation('Beacon',TranslocRot,0);
		}
		if ( !bTeamSet && (Instigator.PlayerReplicationInfo != None) && (Instigator.PlayerReplicationInfo.Team != None) )
		{
			bTeamSet = true;
			if ( Instigator.PlayerReplicationInfo.Team.TeamIndex == 1 )
				Skins[1] = Material'WeaponSkins.NEWTranslocatorBlue';
		}
        Super.RenderOverlays(Canvas);
    }
}

simulated function bool PutDown()
{
    ViewPlayer();
    return Super.PutDown();
}

simulated function Destroyed()
{
    if (TransBeacon != None)
        TransBeacon.Destroy();
    Super.Destroyed();
}

simulated function float ChargeBar()
{
	return AmmoChargeF - int(AmmoChargeF);
}

defaultproperties
{
    ItemName="Translocator"
    Description="The Translocator was originally designed by Liandri Corporation's R&D sector to facilitate the rapid recall of miners during tunnel collapses. However, rapid deresolution and reconstitution can have several unwelcome effects, including increases in aggression and paranoia.||In order to prolong the careers of today's contenders, limits have been placed on Translocator use in the lower-ranked leagues. The latest iteration of the Translocator features a remotely operated camera, exceptionally useful when scouting out areas of contention.|It should be noted that while viewing the camera's surveillance output, the user is effectively blind to their immediate surroundings."
    IconMaterial=Material'HudContent.Generic.HUD'
	IconCoords=(X1=0,Y1=0,X2=2,Y2=2)

	TranslocScale=+1.0
	bPrevWeaponSwitch=true
	bShowChargingBar=true
    bCanThrow=false
    AmmoChargeF=6.0f
    RepAmmo=6
    AmmoChargeMax=6.0f
    AmmoChargeRate=0.4f
    FireModeClass(0)=TransFire
    FireModeClass(1)=TransRecall
    InventoryGroup=10
    Mesh=mesh'NewWeapons2004.NewTransLauncher_1st'
    BobDamping=1.8
    PickupClass=class'TransPickup'
    EffectOffset=(X=100.0,Y=30.0,Z=-19.0)
    AttachmentClass=class'TransAttachment'
    DisplayFOV=60.0
	ViewBeaconVolume=40

    IdleAnimRate=0.25
    PutDownAnim=PutDown
	SelectAnim=Select

    DrawScale=0.8
    PlayerViewOffset=(X=28.5,Y=12,Z=-12)
    SmallViewOffset=(X=38,Y=16,Z=-16)
    PlayerViewPivot=(Pitch=1000,Roll=0,Yaw=400)
    MaxCamDist=4000.0
    SelectSound=Sound'WeaponSounds.Translocator_change'
	SelectForce="Translocator_change"
	CenteredOffsetY=0

    HudColor=(r=0,g=0,b=255,a=255)
	AIRating=-1.0
	CurrentRating=-1.0

	Priority=1
	CustomCrosshair=2
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Cross3"
	CustomCrosshairColor=(r=0,g=0,b=255,a=255)
	CustomCrosshairScale=1.0

	CenteredRoll=0
	Skins(0)=FinalBlend'EpicParticles.NewTransLaunBoltFB'
    Skins(1)=Material'WeaponSkins.NEWTranslocatorTEX'
    Skins(2)=Material'WeaponSkins.NEWTranslocatorPUCK'
    Skins(3)=FinalBlend'WeaponSkins.NEWTransGlassFB'

}

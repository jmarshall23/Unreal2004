class BallLauncher extends Weapon
    config(user)
    HideDropDown
	CacheExempt;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

var() transient bool launchedBall;
var() transient Pawn PassTarget;
var() Sound PassAmbient;
var() Sound PassTargetLocked;
var() Sound PassTargetLost;
var() String PassTargetLockedForce;
var() String PassTargetLostForce;
var() float HealRate;
var transient float HealAccum;
var transient float SwitchTestTime;

var Actor AITarget;		// used by AI

#exec OBJ LOAD File=IndoorAmbience.uax

replication
{
    reliable if ( Role == ROLE_Authority )
        PassTarget;
}

simulated function OutOfAmmo()
{
}

function SynchronizeWeapon(Weapon ClientWeapon)
{
	if ( Instigator.Controller != None )
		Instigator.Controller.ClientSetWeapon(class);
}

simulated function DrawWeaponInfo(Canvas Canvas)
{
	NewDrawWeaponInfo(Canvas, 0);
}

simulated function NewDrawWeaponInfo(Canvas Canvas, float YPos)
{
    local xPlayer PC;

	PC = xPlayer(Instigator.Controller);
	if ( PC == None )
		return;

    if( PassTarget != None )
        PC.myHUD.SetTargeting( true, PassTarget.Location, 1.0 );

	PC.myHUD.DrawTargeting(Canvas);
    PC.myHUD.SetTargeting(false);
}

simulated function BringUp(optional Weapon PrevWeapon)
{
    Super.BringUp();

    if (Instigator.IsLocallyControlled() && !PrevWeapon.IsA('BallLauncher'))
        Instigator.PendingWeapon = PrevWeapon;

    // prevent shooting ball until button is released
    FireMode[0].bIsFiring = true;
    FireMode[0].bNowWaiting = true;
}

simulated function Weapon NextWeapon(Weapon CurrentChoice, Weapon CurrentWeapon)
{
	if ( CurrentWeapon == self )
		return Instigator.PendingWeapon;
	else
		return Super.NextWeapon(CurrentChoice,CurrentWeapon);
}

simulated function Weapon PrevWeapon(Weapon CurrentChoice, Weapon CurrentWeapon)
{
	if ( CurrentWeapon == self )
		return Instigator.PendingWeapon;
	else
		return Super.PrevWeapon(CurrentChoice,CurrentWeapon);
}

simulated function bool PutDown()
{
    if ( launchedBall || Instigator.PlayerReplicationInfo.HasFlag == None )
    {
        launchedBall = false;
        return Super.PutDown();
    }
	if ( Instigator.PendingWeapon == self )
		Instigator.PendingWeapon = None;
    return( false ); // Never let them put the weapon away.
}

function SetPassTarget( Pawn passTarg )
{
    if ( (PassTarget != None) && (PassTarget != PassTarg) && (PlayerController(PassTarget.Controller) != None) )
		PlayerController(PassTarget.Controller).ClientPlaySound(PassTargetLost);	
    PassTarget = passTarg;
    if ( PassTarget == None )
		Level.Game.GameReplicationInfo.FlagTarget = None;
    else
		Level.Game.GameReplicationInfo.FlagTarget = PassTarget.PlayerReplicationInfo;
    if ( PlayerController(Instigator.Controller) != None )
    {
        if ( passTarg != None )
            PlayerController(Instigator.Controller).ClientPlaySound(PassTargetLocked);
        else
            PlayerController(Instigator.Controller).ClientPlaySound(PassTargetLost);
    }
    if ( (PassTarget != None) && (PlayerController(PassTarget.Controller) != None) )
		PlayerController(PassTarget.Controller).ClientPlaySound(PassTargetLocked);	
}

function ModifyPawn( float dt )
{
    if ( Instigator.Weapon == self && Instigator.PlayerReplicationInfo.HasFlag != None)
    {
	    if ( Instigator.Health < Instigator.HealthMax )
	    {
            HealAccum += HealRate * dt;
            if( HealAccum > 1 )
            {
                Instigator.Health = Min(Instigator.HealthMax, Instigator.Health + HealAccum);
                HealAccum -= int(HealAccum);
            }
	    }
    }

    if ( Instigator.Weapon != self && Instigator.PlayerReplicationInfo.HasFlag != None)
    {
        xBombFlag(Instigator.PlayerReplicationInfo.HasFlag).SetHolder( Instigator.Controller );
    }
}

simulated function Tick(float dt)
{
    if ( (Level.NetMode == NM_Client) || Instigator == None || Instigator.PlayerReplicationInfo == None)
        return;

	if ( Instigator.PlayerReplicationInfo.HasFlag == None )
	{
		if ( Level.TimeSeconds - SwitchTestTime > 0.8 )
		{
			if ( (Instigator.Weapon == self) && ((Instigator.PendingWeapon == None) || (Instigator.PendingWeapon == self)) )
				Instigator.Controller.ClientSwitchToBestWeapon();
			SwitchTestTime = Level.TimeSeconds;
		}

		if ( Instigator.AmbientSound == PassAmbient )
			AmbientSound = None;
		return;
    }

    // safeguard - to be absoulutly certain the ball launcher is up when player has the ball and not up otherwise
    if (Level.TimeSeconds - SwitchTestTime > 0.5)
    {
        if ( (Instigator.Weapon != self) && (Instigator.PendingWeapon != self) )
        {
			if ( PassTarget != None )
				SetPassTarget(None);
            Instigator.Controller.ClientSetWeapon(class'BallLauncher');
        }
        else if ( (PassTarget != None) && ((Passtarget.Controller == None) || (VSize(PassTarget.Location - Instigator.Location) > 6000)) )
			SetPassTarget(None);
        SwitchTestTime = Level.TimeSeconds;
    }

	ModifyPawn(dt);
 }

// todo: disallow switching away from this while this weapon is coming is up!!
simulated function bool StartFire( int modeNum )
{
	local bool bResult;

	bResult = Super.StartFire(modeNum);

	if ( bResult )
	{
		if ( modeNum == 0 )
		{
			launchedBall = true;
			ClientPlayForceFeedback(PassTargetLockedForce);
		}
		else
			ClientPlayForceFeedback(PassTargetLostForce);
	}
	return bResult;
}

simulated function bool HasAmmo()
{
	if ( xBombFlag(Instigator.PlayerReplicationInfo.HasFlag) != None )
		return true;
    return( false ); // Never let them change to this weapon for fun.
}

simulated function Weapon RecommendWeapon( out float rating )
{
    local Weapon Recommended;
    local float oldRating;

	if ( xBombFlag(Instigator.PlayerReplicationInfo.HasFlag) != None )
	{
		rating = 10000000;
		return self;
	}

    if ( inventory != None )
    {
        Recommended = inventory.RecommendWeapon(oldRating);
        if ( Recommended != None )
        {
            rating = oldRating;
            return Recommended;
        }
    }
    // Never return the ball launcher if no bomb!
    return None;
}

function bool CanAttack(Actor Other)
{
	return true;
}

function SetAITarget(Actor T)
{
	AITarget = T;
}

function bool BotFire(bool bFinished, optional name FiringMode)
{
	local xBombFlag Bomb;
	local vector ShootLoc;
	local Bot B;

    Bomb = xBombFlag( Instigator.PlayerReplicationInfo.HasFlag );
	B = Bot(Instigator.Controller);
	if ( !B.bPlannedShot || (Bomb == None) || (AITarget == None) )
	{
		B.bPlannedShot = false;
		return false;
	}

	// find correct initial velocity
	ShootLoc = AITarget.Location;
    Bomb.PassTarget = None;
	if ( Pawn(AITarget) != None )
	{
		ShootLoc = ShootLoc + 0.5 * (VSize(ShootLoc - Bomb.Location)/Bomb.ThrowSpeed) * Pawn(AITarget).Velocity;
		Bomb.PassTarget = Pawn(AITarget);
	}
	else if ( (GameObjective(AITarget) == None) && (AITarget.Location.Z <= Bomb.Location.Z) )
		ShootLoc = ShootLoc - AITarget.CollisionHeight * vect(0,0,1);

	return ShootHoop(B,ShootLoc);
}

function bool ShootHoop(Controller B, Vector ShootLoc)
{
	local xBombFlag Bomb;

    Bomb = xBombFlag( Instigator.PlayerReplicationInfo.HasFlag );

	// shoot hoop
    FireMode[0].PlayFiring();
    FireMode[0].FlashMuzzleFlash();
    FireMode[0].StartMuzzleSmoke();
    IncrementFlashCount(0);
    launchedBall = true;

    Bomb.Throw(Instigator.Location, B.AdjustToss(Bomb.ThrowSpeed, Bomb.Location, ShootLoc,true) ); //fixme experiment with bNormalize==false
	if ( Bot(B) != None )
		Bot(B).bPlannedShot = false;
	AITarget = None;
	return true;
}

defaultproperties
{
	bNoVoluntarySwitch=true
	bNoInstagibReplace=true
    bForceSwitch=true
    bCanThrow=false
    FireModeClass(0)=BallShoot
    FireModeClass(1)=BallTarget
    InventoryGroup=15
    Mesh=mesh'Weapons.BallLauncher_1st'
    ItemName="Ball Launcher"
    BobDamping=2.2
    PickupClass=None
    EffectOffset=(X=30.0,Y=10.0,Z=-10.0)
    DrawScale=0.4
    PutDownAnim=PutDown
    DisplayFOV=60
 	PlayerViewOffset=(X=11,Y=0,Z=0)
    SmallViewOffset=(X=23,Y=6,Z=-6)
    PlayerViewPivot=(Pitch=0,Roll=0,Yaw=0)
    AttachmentClass=class'BallAttachment'
	bNotInPriorityList=true
    PutDownAnimRate=2.5
	PutDownTime=+0.2

	SelectSound=Sound'WeaponSounds.ballgun_change'
    PassAmbient=Sound'IndoorAmbience.Machinery36'
    PassTargetLocked=Sound'WeaponSounds.BLockon1'
    PassTargetLost=Sound'WeaponSounds.BSeekLost1'
    SelectForce="ballgun_change"
    PassTargetLockedForce="LockOn"
    PassTargetLostForce="SeekLost"
    HealRate=5.0

    AIRating=+0.1
    CurrentRating=+0.1

	Priority=17

	CenteredOffsetY=-5.0
	CenteredYaw=-300
	CenteredRoll=5000

	CustomCrosshair=11
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Bracket2"
	CustomCrossHairColor=(R=255,G=255,B=0,A=255)
	CustomCrossHairScale=+1.0
	MinReloadPct=0.0
}

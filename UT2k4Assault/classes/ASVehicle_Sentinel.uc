//=============================================================================
// ASVehicle_Sentinel
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class ASVehicle_Sentinel extends ASTurret
	abstract;

Var bool	bActive, bOldActive;
var bool	bSpawnCampProtection;	// when true, sentinels are more powerful
var Sound	OpenCloseSound;

Replication
{
	reliable if ( (bNetInitial || bNetDirty) && Role==ROLE_Authority )
		bActive;
}

simulated function PostNetBeginPlay()
{
	super.PostNetBeginPlay();

	if ( bSpawnCampProtection )
		RotationRate *= 100;
}

simulated function PostNetReceive()
{
	super.PostNetReceive();

	if ( bActive != bOldActive )
	{
		bOldActive = bActive;

		if ( bActive )
			PlayOpening();
		else 
			PlayClosing();
	}
}

/* awake sleeping sentinel */
function AwakeSentinel()
{
	ASSentinelController(Controller).Awake();
}

function bool Awake() { return false; }
function bool GoToSleep() { return false; }
simulated function PlayClosing();
simulated function PlayOpening();

simulated function PlayFiring(optional float Rate, optional name FiringMode )
{
	PlayAnim('Fire', 0.75);
}

simulated function PlayIdleOpened()
{
	PlayAnim('IdleOpen', 1, 0.0);
	
	if ( Level.NetMode != NM_DedicatedServer )
	{
		if ( TurretBase != None && TurretBase.Mesh != None )
			TurretBase.GotoState('Active');

		if ( TurretSwivel != None && TurretSwivel.Mesh != None )
			TurretSwivel.GotoState('Active');
	}
}

simulated function PlayIdleClosed()
{
	PlayAnim('IdleClosed', 1, 0.0);

	if ( Level.NetMode != NM_DedicatedServer )
	{
		if ( TurretBase != None && TurretBase.Mesh != None )
			TurretBase.GotoState('Sleeping');

		if ( TurretSwivel != None && TurretSwivel.Mesh != None )
			TurretSwivel.GotoState('Sleeping');
	}
}


auto state Sleeping
{
	function TakeDamage( int Damage, Pawn instigatedBy, Vector hitlocation,
					Vector momentum, class<DamageType> damageType)
	{
		if ( Role == Role_Authority )
			AwakeSentinel();
	}

	simulated event AnimEnd( int Channel )
	{
		PlayIdleClosed();
	}

	function bool Awake()
	{
		if ( Role == Role_Authority )
			bActive = true;

		PlayOpening();

		return true;
	}

	simulated function PlayOpening()
	{	
		PlayAnim('Open', 0.33, 0.0);
		
		if ( Level.NetMode != NM_DedicatedServer )
		{
			if ( OpenCloseSound != None )
				PlaySound( OpenCloseSound );

			if ( TurretBase != None && TurretBase.Mesh != None )
				TurretBase.GotoState('Opening');
			
			if ( TurretSwivel != None && TurretSwivel.Mesh != None )
				TurretSwivel.GotoState('Opening');
		}

		GotoState('Opening');
	}

Begin:
	if ( bActive )
		PlayOpening();
	else
		PlayIdleClosed();
}

state Active
{
	simulated event AnimEnd( int Channel )
	{
		PlayIdleOpened();
	}

	function bool GoToSleep()
	{
		if ( Role == Role_Authority )
			bActive = false;

		PlayClosing();

		return true;
	}

	simulated function PlayClosing()
	{
		PlayAnim('Close', 0.33, 0.0);
		
		if ( Level.NetMode != NM_DedicatedServer )
		{
			if ( OpenCloseSound != None )
				PlaySound( OpenCloseSound );

			if ( TurretBase != None && TurretBase.Mesh != None )
				TurretBase.GotoState('Closing');
			
			if ( TurretSwivel != None && TurretSwivel.Mesh != None )
				TurretSwivel.GotoState('Closing');
		}
		GotoState('Closing');
	}

Begin:
	PlayIdleOpened();
}

state Closing
{
	simulated event AnimEnd( int Channel )
	{
		if ( Role == Role_Authority )
			ASSentinelController(Controller).AnimEnded();
		GotoState('Sleeping');
	}

	function TakeDamage( int Damage, Pawn instigatedBy, Vector hitlocation,
						Vector momentum, class<DamageType> damageType);
}

state Opening
{
	simulated event AnimEnd( int Channel )
	{
		if ( Role == Role_Authority )
			ASSentinelController(Controller).AnimEnded();
		GotoState('Active');
	}

	function TakeDamage( int Damage, Pawn instigatedBy, Vector hitlocation,
						Vector momentum, class<DamageType> damageType);
}

defaultproperties
{
	TransientSoundVolume=0.75
	TransientSoundRadius=512
	bNetNotify=true
	Health=1000
	HealthMax=1000
	DefaultWeaponClassName="UT2k4Assault.Weapon_Sentinel"

	bSimulateGravity=false
	Physics=PHYS_Rotating
	AirSpeed=0.0
	WaterSpeed=0.0
	AccelRate=0.0
	JumpZ=0.0
	MaxFallSpeed=0.0

	bIgnoreEncroachers=true
    bCollideWorld=false

	bIgnoreForces=true
	bShouldBaseAtStartup=false
	bAutoTurret=true
	AutoTurretControllerClass=class'UT2k4Assault.ASSentinelController'
	SightRadius=+25000.0
	bNonHumanControl=true
	bDefensive=true
	bStationary=true
	VehicleNameString="Sentinel"

	bNoTeamBeacon=true
}

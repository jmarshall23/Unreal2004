//=============================================================================
// ASOBJ_EnergyCore_Spawn
//=============================================================================
// Created by Laurent Delayen (C) 2003 Epic Games
//=============================================================================

class ASOBJ_EnergyCore_Spawn extends GameObjective
	placeable;

var()	Sound	TakenSound;

var		GameObject_EnergyCore			myFlag;
var		class<GameObject_EnergyCore>	FlagType;


function BeginPlay()
{
	super.BeginPlay(); 
	SpawnFlag();
}

function SpawnFlag()
{
	myFlag = Spawn(FlagType, Self);

	if ( myFlag == None )
	{
		warn(Self$" could not spawn flag of type '"$FlagType$"' at "$location);
		return;
	}
	else
	{
		myFlag.HomeBase = Self;
		ASGameReplicationInfo(Level.Game.GameReplicationInfo).GameObject = myFlag;
	}

	Spawn(class'XGame.xBombBase', Self,, Location-vect(0,0,60), Rotation);
}

function bool BotNearObjective(Bot B)
{
	if  ( !IsActive() || (myFlag == None) || (B==None) )
		return false;

	if ( (MyBaseVolume != None) && myFlag.bHome	&& B.Pawn.IsInVolume(MyBaseVolume) )
		return true;
	
	return ( (VSize(myFlag.Position().Location - B.Pawn.Location) < 2000) && B.LineOfSightTo(myFlag.Position()) );
}

function bool BetterObjectiveThan(GameObjective Best, byte DesiredTeamNum, byte RequesterTeamNum)
{
	if ( !IsActive() || myFlag==None || !myFlag.bHome || (RequesterTeamNum == DesiredTeamNum) )
		return false;
	return true;
}

/* Need a work around for clients...
simulated function bool IsActive()
{
	if ( super.IsActive() )
		return ( myFlag != None && myFlag.bHome );

	return false;
}
*/

function Reset()
{
	super.Reset();

	if ( MyFlag != None )
	{
		if ( MyFlag.Holder != None )
			MyFlag.ClearHolder();

		MyFlag.Destroy();
	}

	SpawnFlag();
}


defaultproperties
{
	ObjectiveTypeIcon=Material'AS_FX_TX.Icons.OBJ_Proximity_FB'
	TakenSound=Sound'GameSounds.CTFAlarm'

	//Announcer_DisabledObjective=AnnouncerAssault.JY_core_acquired
	Announcer_DisabledObjective=None
	Announcer_ObjectiveInfo=AnnouncerAssault.JY_find_core
	Announcer_DefendObjective=AnnouncerAssault.JY_ProtectPowerCore
	ObjectiveName="Energy Core Spawn"
	DefenderTeamIndex=1  

    LightType=LT_SubtlePulse
	LightEffect=LE_QuadraticNonIncidence
	LightRadius=6
    LightBrightness=128	
    LightHue=37
    LightSaturation=255	
    bUnlit=true
    bUseCylinderCollision=true

    FlagType=class'UT2k4Assault.GameObject_EnergyCore'
    DrawType=DT_StaticMesh
    StaticMesh=XGame_rc.BallMesh
    DrawScale=3.000000
	bHidden=true

	bNotBased=true
    CollisionRadius=60.000000
    CollisionHeight=60.000000
    bCollideActors=true
    bBlockActors=false

	bReplicateObjective=true
	bPlayCriticalAssaultAlarm=true
	bAlwaysRelevant=true
	bStatic=false
	bNoDelete=true
	bStasis=false
}

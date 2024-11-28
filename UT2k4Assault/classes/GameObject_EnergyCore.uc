//==============================================================================
// GameObject_EnergyCore
//==============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class GameObject_EnergyCore extends GameObject;

var localized string 	PlayerDroppedMessage, DroppedMessage, EnergyCorePickedUp, PlayerPickedUpEnergyCore, PlayerCoreReset;

var		name	Announcer_EnergyCore_Dropped, Announcer_EnergyCore_PickedUp, Announcer_EnergyCore_Reset;
var		bool	bSoundsPrecached;

function bool CanBePickedUpBy(Pawn P)
{
	return  ( P.GetTeamNum() != Level.Game.GetDefenderNum() && !P.IsA('Vehicle') );
}

function bool ValidHolder(Actor Other)
{
	if ( super.ValidHolder( Other ) && Other.IsA('Pawn') )
		return CanBePickedUpBy( Pawn(Other) );

	return false;
}

function SetHolder(Controller C)
{
	local Controller	Ctrl;
	if ( C == None || C.Pawn == None )
		return;

	for ( Ctrl=Level.ControllerList; Ctrl!=None; Ctrl=Ctrl.nextController )
		if ( PlayerController(Ctrl) != None )
			PlayerController(Ctrl).ReceiveLocalizedMessage(class'Message_PowerCore', 3, C.PlayerReplicationInfo );

	super.SetHolder( C );

	HomeBase.DisableObjective( C.Pawn );
}

function ClearHolder()
{
	super.ClearHolder();
}

function Drop(vector newVel)
{
	local Controller	C;

	if ( Holder != None && Holder.PlayerReplicationInfo != None )
	{
		for ( C=Level.ControllerList; C!=None; C=C.nextController )
			if ( PlayerController(C) != None )
				PlayerController(C).ReceiveLocalizedMessage(class'Message_PowerCore', 0, Holder.PlayerReplicationInfo );
	}
	else
	{
		for ( C=Level.ControllerList; C!=None; C=C.nextController )
			if ( PlayerController(C) != None )
				PlayerController(C).ReceiveLocalizedMessage(class'Message_PowerCore', 1);
	}

	super.Drop( newVel );
}

function HolderDied()
{
	Drop( vect(0,0,0) );
}

function LogReturned()
{
	local Controller C;

	for ( C=Level.ControllerList; C!=None; C=C.nextController )
		if ( PlayerController(C) != None )
			PlayerController(C).ReceiveLocalizedMessage(class'Message_PowerCore', 4);
}


simulated function PrecacheAnnouncer(AnnouncerVoice V, bool bRewardSounds)
{
	if ( !bRewardSounds && !bSoundsPrecached )
	{
		bSoundsPrecached = true;
		V.PrecacheSound( Announcer_EnergyCore_Dropped );
		V.PrecacheSound( Announcer_EnergyCore_PickedUp );
	}
}

defaultproperties
{
	Announcer_EnergyCore_Dropped=JY_PowerCoreDropped
	Announcer_EnergyCore_PickedUp=JY_PowerCorePickedUp
	Announcer_EnergyCore_Reset=JY_PowerCoreReset

	PlayerDroppedMessage=" dropped the Power Core!"
	DroppedMessage="Power Core dropped!"
	EnergyCorePickedUp="Power Core picked up!"
	PlayerPickedUpEnergyCore=" picked up the Power Core!"
	PlayerCoreReset="Power Core reset!"

    bHome=true
	GameObjBone=spine
	GameObjOffset=(X=25,Y=50)
	GameObjRot=(Roll=16384)

    RemoteRole=ROLE_DumbProxy
	NetUpdateFrequency=100
    NetPriority=+00003.000000
	bAlwaysRelevant=true
    
	DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'AS_Decos.HellBenderEngine'
    DrawScale=0.75
	bHidden=false
    bStasis=false
    bStatic=false

    CollisionRadius=24.0
	CollisionHeight=20.0
    bCollideActors=true
    bCollideWorld=true
    bUseCylinderCollision=true
	Mass=100.0
    Buoyancy=20.000000
    PrePivot=(X=2,Y=0,Z=0.5)

	bDynamicLight=true
    LightHue=40
    LightBrightness=200
    bUnlit=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightRadius=6
}
//=============================================================================
// ASTurret_Minigun_Swivel
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class ASTurret_Minigun_Swivel extends ASTurret_Base;


simulated function UpdateSwivelRotation( Rotator TurretRotation )
{
	local Rotator SwivelRotation;

	SwivelRotation			= TurretRotation;
	SwivelRotation.Pitch	= 0;
	SetRotation( SwivelRotation );
}


//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
    StaticMesh=StaticMesh'AS_Weapons_SM.ASMinigun_Swivel'
    DrawScale=0.42

	Physics=Phys_Rotating
	bMovable=true
}
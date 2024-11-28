//=============================================================================
// ASVehicle_Sentinel_Floor_Swivel
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class ASVehicle_Sentinel_Floor_Swivel extends ASTurret_Minigun_Swivel;


//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
    StaticMesh=StaticMesh'AS_Weapons_SM.FloorTurretSwivel'
    DrawScale=0.5

    CollisionHeight=100.0
    CollisionRadius=96.0
	PrePivot=(Z=150)
}
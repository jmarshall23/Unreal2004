//=============================================================================
// Ammo_Dummy
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class Ammo_Dummy extends Ammunition;

#EXEC OBJ LOAD FILE=InterfaceContent.utx

defaultproperties
{
	RemoteRole=Role_None
    ItemName="Dummy"

    PickupClass=None
    MaxAmmo=1
    InitialAmount=1
    bStasis=true
}

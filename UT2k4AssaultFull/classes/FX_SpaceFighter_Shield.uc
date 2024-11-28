//=============================================================================
// FX_SpaceFighter_Shield
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FX_SpaceFighter_Shield extends Actor;


simulated function Tick(float deltaTime)
{
	local float pct;

	if ( Level.NetMode == NM_DedicatedServer )
	{
		Disable('Tick');
		return;
	}

	pct = LifeSpan / default.LifeSpan;
	SetDrawScale( 6 + 8*(1-pct) );
}


defaultproperties
{
	RemoteRole=ROLE_SimulatedProxy
	bNetTemporary=true
	bNetInitialRotation=true

	bReplicateInstigator=true
	bHardAttach=true
	DrawType=DT_StaticMesh
	StaticMesh=StaticMesh'WeaponStaticMesh.Shield'
	Skins(0)=Material'XEffectMat.BlueShell'
	Skins(1)=FinalBlend'XEffectMat.BlueShell'
	DrawScale=12.0
	bUnlit=true
	bOwnerNoSee=true
	AmbientGlow=250
	LifeSpan=0.6
}
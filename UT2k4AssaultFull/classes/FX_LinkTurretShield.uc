//=============================================================================
// FX_LinkTurretShield
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FX_LinkTurretShield extends Actor;

var float	Flash;
var bool	bSetUp;

#exec load obj FILE=JWAssaultMeshes

simulated function PostBeginPlay()
{
	local ColorModifier Alpha;

	super.PostBeginPlay();

	if ( Level.NetMode == NM_DedicatedServer )
		return;

	Alpha = ColorModifier(Level.ObjectPool.AllocateObject(class'ColorModifier'));
	Alpha.Material = Skins[0];
	Alpha.AlphaBlend = true;
	Alpha.RenderTwoSided = true;
	Alpha.Color.G = 255;
	Alpha.Color.A = 255;
	Skins[0] = Alpha;

	//Skins[1] = Alpha;
	//Skins[2] = Alpha;

	UpdateShieldColor();
	bSetUp = true;
}

simulated function Destroyed()
{
	if ( bSetUp )
	{
		Level.ObjectPool.FreeObject(Skins[0]);
		Skins[0] = None;
	}

	super.Destroyed();
}

simulated function SetOpacity( float pct )
{
	if ( !bSetUp )
		return;

	if ( pct < 0.01 )
	{
		bHidden = true;
		return;
	}

	bHidden = false;
	
	ColorModifier(Skins[0]).Color.A = 255 * pct;
}

simulated function DoFlash()
{
	Flash = 255;
}

simulated function Tick(float deltaTime)
{
	if ( Level.NetMode == NM_DedicatedServer )
	{
		Disable('Tick');
		return;
    }

	if ( bHidden || Flash <= 0)
		return;

	Flash -= (256 * deltatime);

	if ( Flash > 0 )
		UpdateShieldColor();
}

simulated function UpdateShieldColor()
{
	ColorModifier(Skins[0]).Color.R = 0;
	ColorModifier(Skins[0]).Color.G = 255 - int(Flash);
	ColorModifier(Skins[0]).Color.B = int(Flash);
}

defaultproperties
{
	LifeSpan=0
	DrawType=DT_StaticMesh
    StaticMesh=staticMesh'LinkTurretShield'
	DrawScale=0.33
    DrawScale3D=(X=1,Y=1.1,Z=1.1)
	PrePivot=(X=109)
    bHidden=false
	bNetTemporary=false
    bReplicateInstigator=true

	RemoteRole=ROLE_None
	bNetInitialRotation=true

	bHardAttach=true
	Skins(0)=AS_FX_TX.WhiteShield_FB
	Skins(1)=AS_FX_TX.WhiteShield_FB
	Skins(2)=AS_FX_TX.WhiteShield_FB
	bUnlit=true
	bOwnerNoSee=true
	AmbientGlow=250
}
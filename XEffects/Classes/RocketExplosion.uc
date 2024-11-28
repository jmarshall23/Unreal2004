//=============================================================================
// RocketExplosion
//=============================================================================
class RocketExplosion extends xEmitter;

#exec OBJ LOAD File=WeaponSounds.uax

#EXEC TEXTURE IMPORT NAME=Rexpt FILE=textures\explochunks.tga GROUP=Skins DXT=5

simulated function PostBeginPlay()
{
	local PlayerController PC;

	PC = Level.GetLocalPlayerController();
	if ( (PC.ViewTarget == None) || (VSize(PC.ViewTarget.Location - Location) > 5000) ) 
	{
		LightType = LT_None;
		bDynamicLight = false;
	}
	else 
	{
		Spawn(class'RocketSmokeRing');
		if ( Level.bDropDetail )
			LightRadius = 7;	
	}
}

defaultproperties 
{
    Style=STY_Additive
    mParticleType=PL_Sprite
    mDirDev=(X=1.0,Y=1.0,Z=1.0)
    mPosDev=(X=0.0,Y=0.0,Z=0.0) 
    mLifeRange(0)=0.5
    mLifeRange(1)=1.0
    mSpeedRange(0)=3.0
    mSpeedRange(1)=10.0
    mSizeRange(0)=100.0
    mSizeRange(1)=200.0
    mMassRange(0)=0.0
    mMassRange(1)=0.0
    mSpinRange(0)=-20.0
    mSpinRange(1)=20.0
    mStartParticles=4
    mMaxParticles=4
    mAttenuate=true
    mRandOrient=true
    mRegen=false
    mRandTextures=false
    skins(0)=Texture'REXpt'
    LifeSpan=2.0
    bForceAffected=false

    bDynamicLight=true
    LightEffect=LE_QuadraticNonIncidence
    LightType=LT_FadeOut
    LightBrightness=255
    LightHue=28
    LightSaturation=90
    LightRadius=9
    LightPeriod=32
    LightCone=128
}
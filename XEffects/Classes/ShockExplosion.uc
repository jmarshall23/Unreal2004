class ShockExplosion extends xEmitter;

simulated event PostBeginPlay()
{
	local PlayerController PC;
	
    Super.PostBeginPlay();

    SetTimer(0.5, false);
    if ( Level.bDropDetail )
	{
		bDynamicLight = false;
		LightType = LT_None;
	}
	else
	{
		PC = Level.GetLocalPlayerController();
		if ( (PC.ViewTarget == None) || (VSize(PC.ViewTarget.Location - Location) > 4000) ) 
		{
			LightType = LT_None;
			bDynamicLight = false;
		}
	}
}

simulated function Timer()
{
    LightType = LT_None;
}

defaultproperties
{
    LifeSpan=2.0
    mParticleType=PT_Mesh
    mMeshNodes(0)=StaticMesh'EffectsSphere144'
    Skins(0)=FinalBlend'XEffectMat.ShockDarkFB'
    mRegen=true
    mStartParticles=1
    mMaxParticles=1
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mLifeRange(0)=0.6
    mLifeRange(1)=0.6
    mSizeRange(0)=0.6
    mSizeRange(1)=0.6
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mGrowthRate=1.2
    mAttenKa=0.3
    mRandOrient=true
    mAttenFunc=ATF_ExpInOut
    mColorRange(0)=(R=100,B=100,G=100)
    mColorRange(1)=(R=100,B=100,G=100)
    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightBrightness=255
    LightHue=195
    LightSaturation=85
    LightRadius=5
    bAttenByLife=true
}

class ShockComboFlash extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties 
{
	bHighDetail=True
    Style=STY_Translucent
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mLifeRange(0)=0.25
    mLifeRange(1)=0.25
    mSizeRange(0)=480.0
    mSizeRange(1)=660.0
    mGrowthRate=500.0
    mSpinRange(0)=0.0
    mSpinRange(1)=0.0
    mStartParticles=1
    mMaxParticles=1
    mAttenuate=True
    Skins(0)=Texture'XEffectMat.ShockComboFlash'
    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightBrightness=255
    LightHue=195
    LightSaturation=180
    LightRadius=10
    mRandOrient=True
    mRegen=False
    LifeSpan=0.3
}

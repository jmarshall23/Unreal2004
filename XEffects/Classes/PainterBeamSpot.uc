class PainterBeamSpot extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Skins=(Texture'XEffectMat.Shock.shock_mark_heat')
    Style=STY_Additive
    mParticleType=PT_Disc
    mStartParticles=0
    mMaxParticles=1
    mLifeRange(0)=5.0
    mLifeRange(1)=5.0
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=0.0
    mSpinRange(1)=0.0
    mSizeRange(0)=30.0
    mSizeRange(1)=30.0
    mGrowthRate=0.0
    mAttenKa=0.0
    mRandOrient=true
    mRegen=true
    mPosRelative=true

    bDynamicLight=true
    LightType=LT_Steady
    LightEffect=LE_QuadraticNonIncidence
    LightBrightness=220
    LightHue=0
    LightSaturation=20
    LightRadius=1
}

class IonCore extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Skins(0)=Texture'XEffectMat.shock_core'
    Style=STY_Additive
    mStartParticles=1
    mMaxParticles=1
    mLifeRange(0)=100.0
    mLifeRange(1)=100.0
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=0.0
    mSpinRange(1)=0.0
    mSizeRange(0)=300.0
    mSizeRange(1)=300.0
    mPosRelative=true
    mAttenuate=false
    LifeSpan=+1.0
}

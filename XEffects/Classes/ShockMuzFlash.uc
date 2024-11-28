class ShockMuzFlash extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    mParticleType=PT_Sprite
    Style=STY_Additive
    Skins(0)=Texture'XEffectMat.shock_sparkle'
    mRandOrient=true
    mSizeRange(0)=50.0
    mSizeRange(1)=50.0
    mGrowthRate=50
    mLifeRange(0)=0.5
    mLifeRange(1)=0.5
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=0
    mSpinRange(1)=0
    mRegen=false
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mStartParticles=1
    mMaxParticles=1
    mColorRange(0)=(R=255,G=255,B=255,A=255)
    mColorRange(1)=(R=255,G=255,B=255,A=255)
    mAttenuate=true
    mAttenKa=0.0
}

class BlueWhisp extends WillowWhisp;

#exec OBJ LOAD File=XMiscEffects.utx

defaultproperties
{
    Skins(0)=Texture'EmitSmoke_t'
    Style=STY_Additive
    mParticleType=PT_Sprite
    mLifeRange(0)=1.25
    mLifeRange(1)=1.25
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=-75.0
    mSpinRange(1)=75.0
    mSizeRange(0)=25.0
    mSizeRange(1)=30.0
    mRegenRange(0)=90.0
    mRegenRange(1)=90.0
    mRandOrient=True
    mRandTextures=True
    mStartParticles=0
    mMaxParticles=150
    mGrowthRate=13.0
    mAttenuate=True
    mAttenFunc=ATF_ExpInOut
    mAttenKa=0.2
    mRegen=True
    mNumTileColumns=4
    mNumTileRows=4
    mMassRange(0)=-0.03
    mMassRange(1)=-0.01
    mColorRange(0)=(R=40,G=40,B=255,A=255)
    mColorRange(1)=(R=40,G=40,B=255,A=255)
}
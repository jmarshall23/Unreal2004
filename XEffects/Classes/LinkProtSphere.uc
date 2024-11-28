class LinkProtSphere extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Style=STY_Translucent
    Skins(0)=Texture'XEffectMat.link_muz_red'
    mRandOrient=True 
    mSizeRange(0)=35.0
    mSizeRange(1)=35.0
    mLifeRange(0)=0.25
    mLifeRange(1)=0.25
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mRegen=True
    mRegenRange(0)=12.0
    mRegenRange(1)=12.0
    mSpinRange(0)=-100.0
    mSpinRange(1)=100.0
    mStartParticles=1
    mMaxParticles=6
    mColorRange(0)=(R=180,G=180,B=180,A=255)
    mColorRange(1)=(R=180,G=180,B=180,A=255)
    mPosRelative=true
    mAttenuate=true
    mAttenKa=0.0
    mParticleType=PT_Disc
}

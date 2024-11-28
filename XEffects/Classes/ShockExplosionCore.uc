class ShockExplosionCore extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
	bHighDetail=True
    Skins(0)=Texture'XEffectMat.shock_core'
    Style=STY_Additive
    mStartParticles=1
    mMaxParticles=1
    mLifeRange(0)=1.0
    mLifeRange(1)=1.0
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=0.0
    mSpinRange(1)=0.0
    mSizeRange(0)=40.0
    mSizeRange(1)=40.0
    mGrowthRate=100.0
    //mColorRange(0)=(B=240,G=120,R=240)
    //mColorRange(1)=(B=240,G=120,R=240)
    mAttenuate=true
    mAttenKa=0.0
    mRegen=false
}

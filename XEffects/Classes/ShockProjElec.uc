class ShockProjElec extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Physics=PHYS_Trailer
    Skins=(Texture'XEffectMat.Shock.shock_flare_a')
    Style=STY_Additive
    mStartParticles=1
    mMaxParticles=5
    mLifeRange(0)=0.5
    mLifeRange(1)=0.5
    mRegenRange(0)=5.0
    mRegenRange(1)=5.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=-120.0
    mSpinRange(1)=120.0
    mSizeRange(0)=75.0
    mSizeRange(1)=75.0
    mColorRange(0)=(B=200,G=200,R=200)
    mColorRange(1)=(B=200,G=200,R=200)
    mRandOrient=True
    mPosRelative=True
}

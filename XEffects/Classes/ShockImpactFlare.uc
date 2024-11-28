class ShockImpactFlare extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Skins=(Texture'XEffectMat.Shock.shock_flare_a')
    Style=STY_Additive
    mParticleType=PT_Disc
    mStartParticles=3
    mMaxParticles=3
    mLifeRange(0)=0.5
    mLifeRange(1)=0.5
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=-100.0
    mSpinRange(1)=100.0
    mSizeRange(0)=50.0
    mSizeRange(1)=60.0
    mGrowthRate=100.0
    mAttenKa=0.0
    mRandOrient=True
    mRegen=false
}

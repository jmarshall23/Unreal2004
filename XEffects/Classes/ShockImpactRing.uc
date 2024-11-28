class ShockImpactRing extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
	bHighDetail=true
    Skins=(Texture'XEffectMat.Shock.shock_ring_b')
    Style=STY_Additive
    mParticleType=PT_Disc
    mStartParticles=1
    mMaxParticles=1
    mLifeRange(0)=0.5
    mLifeRange(1)=0.5
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=-100.0
    mSpinRange(1)=100.0
    mSizeRange(0)=30.0
    mSizeRange(1)=30.0
    mGrowthRate=260.0
    mAttenKa=0.0
    mRandOrient=True
    mRegen=false
}

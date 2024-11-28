class IonShaft extends xEmitter;

defaultproperties
{
    LifeSpan=4.0

    mParticleType=PT_Beam
    mStartParticles=1
    mAttenuate=false
    mAttenKa=0.0
    mSizeRange(0)=50.0
    mSizeRange(1)=90.0
    mRegenDist=256.0
    mLifeRange(0)=1.3
    mMaxParticles=4

    Skins(0)=FinalBlend'XEffectMat.IonParticleBeamFB'
    Style=STY_Translucent
}

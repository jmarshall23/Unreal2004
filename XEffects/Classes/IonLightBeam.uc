class IonLightBeam extends xEmitter;

defaultproperties
{
    LifeSpan=4.0

    mParticleType=PT_Beam
    mStartParticles=1
    mAttenuate=false
    mAttenKa=0.0
    mSizeRange(0)=10.0
    mSizeRange(1)=22.0
    mRegenDist=150.0
    mLifeRange(0)=1.8
    mMaxParticles=1
    mMeshNodes(0)=StaticMesh'ShockCoil'
    mSpinRange(0)=0
    mColorRange(0)=(R=15,G=15,B=15)
    mColorRange(1)=(R=15,G=15,B=15)

    DrawScale=5.0
    Skins(0)=FinalBlend'XEffectMat.IonLightFB'
    bUnlit=true
}


class LinkBeamChild extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    RemoteRole=ROLE_None

    mParticleType=PT_Beam
    mStartParticles=1
    mAttenuate=false
    mSizeRange(0)=6.0 // width
    mRegenDist=75.0 // section length
    mMaxParticles=2 // planes
    //mColorRange(0)=(R=70,B=70,G=70)
    //mColorRange(1)=(R=70,B=70,G=70)
    mColorRange(0)=(R=180,B=180,G=180)
    mColorRange(1)=(R=180,B=180,G=180)

    mSpinRange(0)=45000 // spin
    mAttenKa=0.01
    mWaveFrequency=0.06
    mWaveAmplitude=15.0
    mWaveShift=100000.0
    mBendStrength=3.0
    mWaveLockEnd=true

    Skins(0)=FinalBlend'XEffectMat.LinkBeamGreenFB'
    Style=STY_Additive
    bUnlit=true
}

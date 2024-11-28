//=============================================================================
// ForceRing (ShieldGun).
//=============================================================================
class ForceRing extends xEmitter;

simulated function Fire()
{
    mRegenRange[0] = 24.0;
    mRegenRange[1] = 24.0;
    SetTimer(0.2, false);
}

simulated function Timer()
{
    mRegenRange[0] = 0.0;
    mRegenRange[1] = 0.0;
}

defaultproperties
{
    Skins(0)=Texture'XEffectMat.Shock.shock_ring_b'
    Style=STY_Additive
    mStartParticles=0
    mMaxParticles=10
    mRegen=true
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mLifeRange(0)=0.25
    mLifeRange(1)=0.25
    mSpeedRange(0)=250.0
    mSpeedRange(1)=250.0
    mSizeRange(0)=1.0
    mSizeRange(1)=1.0
    mGrowthRate=200.0
    mSpinRange(0)=0.0
    mSpinRange(1)=0.0
    mParticleType=PT_Disc
    mPosRelative=true
    mAttenKa=0.0
    mColorRange(0)=(R=32,G=255,B=32,A=255)
    mColorRange(1)=(R=32,G=255,B=32,A=255)
}

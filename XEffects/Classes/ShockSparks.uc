//=============================================================================
// ShockSparks.
//=============================================================================
class ShockSparks extends WallSparks;

defaultproperties
{  
    mParticleType=PT_Line
    mMassRange(0)=0.00000
    mMassRange(1)=0.00000
    mDelayRange(0)=0.000000
    mDelayRange(1)=0.00000
    mStartParticles=70
    mDirDev=(X=0.600000,Y=0.600000,Z=0.60000)
    mPosDev=(X=10.00000,Y=10.00000,Z=0.00000)
    mLifeRange(0)=0.20000
    mLifeRange(1)=0.500000
    mSpeedRange(0)=1000.000000
    mSpeedRange(1)=2000.000000
    mSizeRange(0)=3.500000
    mSizeRange(1)=6.500000
    mMaxParticles=70
    mAttenuate=True
    mGrowthRate=-1.0
    mCollision=False
    bForceAffected=False
}
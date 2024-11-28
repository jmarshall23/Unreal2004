//=============================================================================
// ShieldSparks.
//=============================================================================
class ShieldSparks extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Style=STY_Additive
    mParticleType=PT_Line
    mDirDev=(X=0.9,Y=0.9,Z=0.9)
    mPosDev=(X=6.0,Y=6.0,Z=6.0)
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mDelayRange(0)=0.0
    mDelayRange(1)=0.1
    mLifeRange(0)=0.4
    mLifeRange(1)=0.6
    mSpeedRange(0)=150.0
    mSpeedRange(1)=300.0
    mSizeRange(0)=2.5
    mSizeRange(1)=2.5
    mMassRange(0)=1.5
    mMassRange(1)=2.5
    mStartParticles=0
    mMaxParticles=20
    mGrowthRate=-4.0
    mAttenuate=true
    mRegen=true
    Skins(0)=Texture'XEffectMat.ShieldSpark'
    mColorRange(0)=(R=255,G=255,B=255,A=255)
    mColorRange(1)=(R=255,G=255,B=255,A=255)
    bForceAffected=False
    mAttenKa=0.0
    mAirResistance=0.0
    mSpawnVecB=(X=2.0,Y=0.0,Z=0.03)
}

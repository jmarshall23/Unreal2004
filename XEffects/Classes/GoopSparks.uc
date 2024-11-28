class GoopSparks extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Style=STY_Additive
    Skins(0)=Texture'XEffectMat.Link.link_spark_green'
    mParticleType=PT_Line
    mDirDev=(X=0.8,Y=0.8,Z=0.8)
    mPosDev=(X=0.0,Y=0.0,Z=0.0)
    mLifeRange(0)=0.5
    mLifeRange(1)=0.8
    mSpeedRange(0)=80.0
    mSpeedRange(1)=240.0
    mSizeRange(0)=4.0
    mSizeRange(1)=4.0
    mMassRange(0)=1.5
    mMassRange(1)=2.0
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mStartParticles=10
    mMaxParticles=10
    ScaleGlow=2.0
    mGrowthRate=-2.0
    mRegen=false
    mAttenuate=true
    mAttenKa=0.0
    mAirResistance=2.0      
    mColorRange(0)=(R=160,B=180,G=160)
    mColorRange(1)=(R=160,B=180,G=160)
    mSpawnVecB=(X=8.0,Y=0.0,Z=0.04)
    RemoteRole=ROLE_None
}

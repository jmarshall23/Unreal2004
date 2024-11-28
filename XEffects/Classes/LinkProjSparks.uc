class LinkProjSparks extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Skins(0)=Texture'XEffectMat.Link.link_spark_green'
    Style=STY_Translucent
    mParticleType=PT_Line
    mLifeRange(0)=0.5
    mLifeRange(1)=0.5
    mSpeedRange(0)=100.0
    mSpeedRange(1)=175.0
    mMassRange(0)=2.0
    mMassRange(1)=2.0
    mSizeRange(0)=8.0
    mSizeRange(1)=12.0
    mAttenKa=0.1
    mPosDev=(X=10.0,Y=10.0,Z=10.0)
    mDirDev=(X=1.5,Y=1.5,Z=1.5)
    mSpawnVecB=(X=14.0,Z=0.06)
    mRegen=false
    mPosRelative=false
    mColorRange(0)=(R=150,B=150,G=150)
    mColorRange(1)=(R=150,B=150,G=150)
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mStartParticles=8
    mMaxParticles=8
}

class LinkProjSparksYellow extends LinkProjSparks;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Skins(0)=Texture'XEffectMat.Link.link_spark_yellow'
    mLifeRange(0)=0.75
    mLifeRange(1)=0.75
    mSpeedRange(0)=130.0
    mSpeedRange(1)=205.0
    mSizeRange(0)=9.0
    mSizeRange(1)=13.0
    mSpawnVecB=(X=16.0,Z=0.07)
    mStartParticles=8
    mMaxParticles=8
}

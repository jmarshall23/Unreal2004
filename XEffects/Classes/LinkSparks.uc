class LinkSparks extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

var float DesiredRegen;

simulated function SetLinkStatus(int Links, bool bLinking, float ls)
{
    mSizeRange[0] = default.mSizeRange[0] * (ls*1.0 + 1);
    mSizeRange[1] = default.mSizeRange[1] * (ls*1.0 + 1);
    mSpeedRange[0] = default.mSpeedRange[0] * (ls*0.7 + 1);
    mSpeedRange[1] = default.mSpeedRange[1] * (ls*0.7 + 1);
    mLifeRange[0] = default.mLifeRange[0] * (ls + 1);
    mLifeRange[1] = mLifeRange[0];
    DesiredRegen = default.mRegenRange[0] * (ls + 1);
    if (Links == 0)
        Skins[0] = Texture'XEffectMat.Link.link_spark_green';
    else
        Skins[0] = Texture'XEffectMat.Link.link_spark_yellow';
}

defaultproperties
{
    Skins(0)=Texture'XEffectMat.Link.link_spark_green'
    Style=STY_Additive
    mParticleType=PT_Line
    mStartParticles=0
    mLifeRange(0)=0.5
    mLifeRange(1)=0.5
    DesiredRegen=40.0
    mRegenRange(0)=40.0
    mRegenRange(1)=40.0
    mMaxParticles=40.0
    mSpeedRange(0)=-175.0
    mSpeedRange(1)=-225.0
    mMassRange(0)=2.0
    mMassRange(1)=2.0
    mSizeRange(0)=8.0
    mSizeRange(1)=12.0
    mAttenKa=0.1
    mPosDev=(X=8.0,Y=8.0,Z=8.0)
    mDirDev=(X=0.5,Y=1.0,Z=1.0)
    mSpawnVecB=(X=12.0,Z=0.06)
    mColorRange(0)=(R=150,B=150,G=150)
    mColorRange(1)=(R=150,B=150,G=150)

    bDynamicLight=true
    LightType=LT_Steady
    LightRadius=3
    LightHue=100
    LightSaturation=100
    LightBrightness=180
}

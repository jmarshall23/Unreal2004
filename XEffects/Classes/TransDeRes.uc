//=============================================================================
// TransDeRes.
//=============================================================================
class TransDeRes extends xEmitter;

#exec Texture Import File=Textures\GreySpark.tga Name=GreySpark Alpha=1 DXT=5

defaultproperties
{
    bCallPreSpawn=true
    LifeSpan=1.0
    Style=STY_Additive
    mSpawningType=ST_OwnerSkeleton
    mParticleType=PT_Line
    mDirDev=(X=0.0,Y=0.0,Z=0.0)
    mPosDev=(X=6.0,Y=6.0,Z=6.0)
    mDelayRange(0)=0.0
    mDelayRange(1)=0.1
    mLifeRange(0)=0.3
    mLifeRange(1)=0.3
    mSpeedRange(0)=100.0
    mSpeedRange(1)=200.0
    mSizeRange(0)=2.5
    mSizeRange(1)=1.5
    mMassRange(0)=0.0
    mMassRange(1)=0.0
    mStartParticles=10
    mMaxParticles=150
    mRegenRange(0)=550
    mRegenRange(1)=550
    DrawScale=1.0
    ScaleGlow=1.0
    mGrowthRate=-4.0
    mAttenuate=true
    mRegen=true
    Skins(0)=Texture'GreySpark'
    CollisionRadius=0.0
    CollisionHeight=0.0
    mColorRange(0)=(R=235,G=15,B=15,A=255)
    mColorRange(1)=(R=235,G=15,B=15,A=255)
    bForceAffected=False
    mAttenKa=0.1
    mAirResistance=0.0
    mSpawnVecB=(X=4.0,Y=0.0,Z=0.07)
    bAttenByLife=true
}
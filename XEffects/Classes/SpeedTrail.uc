class SpeedTrail extends xEmitter;

#exec TEXTURE  IMPORT NAME=SpeedTrailTex FILE=textures\speedtrail.tga

defaultproperties
{
    RemoteRole=ROLE_SimulatedProxy
    bNetTemporary=false
    Physics=PHYS_None
    Skins(0)=Texture'SpeedTrailTex'
    Style=STY_Additive
    mParticleType=PT_Stream
    mSpawningType=ST_Sphere
    mLifeRange(0)=0.65
    mLifeRange(1)=0.65
    mRegenRange(0)=10.0
    mRegenRange(1)=10.0
    mSpeedRange(0)=-20.0
    mSpeedRange(1)=-20.0
    mAirResistance=0.0
    mMassRange(0)=-0.1
    mMassRange(1)=-0.1
    mSpinRange(0)=0.0
    mSpinRange(1)=0.0
    mSizeRange(0)=12.0
    mSizeRange(1)=12.0
    mGrowthRate=-12.0
    mAttenKa=0.0
    mPosDev=(X=2.0,Y=2.0,Z=2.0)
    mDirDev=(X=0.5,Y=0.5,Z=0.5)
    //mColorRange(0)=(R=0)
    //mColorRange(1)=(R=0)
    mStartParticles=0
    mMaxParticles=50
    mPosRelative=false
    LifeSpan=+60.0
}

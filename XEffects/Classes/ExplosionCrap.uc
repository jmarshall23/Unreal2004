//=============================================================================
// ExplosionCrap.
class ExplosionCrap extends WallSparks;

#exec TEXTURE IMPORT NAME=pcl_Crapa FILE=MODELS\Crap.tga GROUP=Skins Alpha=1  DXT=5


defaultproperties
{  
	bHighDetail=true
    mAirResistance=0.3
    mParticleType=PT_Sprite  
    mGrowthRate=0.0
    Style=STY_Alpha
    mMassRange(0)=2.0000
    mMassRange(1)=3.50000
    mDelayRange(0)=0.00000
    mDelayRange(1)=0.00 
    mStartParticles=150
    mDirDev=(X=0.400000,Y=0.40000,Z=0.4000)
    mPosDev=(X=30.00000,Y=30.00000,Z=30.00000)
    mLifeRange(0)=1.20000
    mLifeRange(1)=2.400000 
    mSpeedRange(0)=200.000000
    mSpeedRange(1)=700.000000
    mSizeRange(0)=2.00000
    mSizeRange(1)=6.00000
    mAttenuate=True
    mSpinRange(0)=-80.0 
    mSpinRange(1)=80.0
    mMaxParticles=150
    mColorRange(0)=(R=255,G=255,B=255,A=255)
    mColorRange(1)=(R=255,G=255,B=255,A=255) 
    //Skins(0)=Texture'pcl_Crapa'

    mNumTileColumns=4
    mNumTileRows=4
    Skins(0)=Texture'EmitterTextures.MultiFrame.rockchunks02'

    mCollision=False
    mRandOrient=True
    bForceAffected=False
    RemoteRole=ROLE_None
}
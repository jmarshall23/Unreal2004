//=============================================================================
// WallSparks.
//=============================================================================
class WallSparks extends xEmitter;

#exec TEXTURE IMPORT NAME=pcl_Spark FILE=TEXTURES\Spark.tga GROUP=Skins ALPHA=1 DXT=5

defaultproperties
{
	 Style=STY_Additive
     mParticleType=PT_Line
     mDirDev=(X=0.6,Y=0.6,Z=0.6)
     mPosDev=(X=0.8,Y=0.8,Z=0.8)
     mDelayRange(0)=0.0
     mDelayRange(1)=0.0
     mLifeRange(0)=0.5
     mLifeRange(1)=0.8
     mSpeedRange(0)=50.0
     mSpeedRange(1)=300.0
     mSizeRange(0)=3.0
     mSizeRange(1)=2.0
     mMassRange(0)=1.5
     mMassRange(1)=2.5
     mRegenRange(0)=0.0
     mRegenRange(1)=0.0
     mRegenDist=0.0
     mStartParticles=8
     mMaxParticles=8
     DrawScale=1.0
     ScaleGlow=2.0
	 mGrowthRate=-4.0
	 mAttenuate=true
	 mRegen=false;
     Skins(0)=Texture'pcl_Spark'
     CollisionRadius=0.0
     CollisionHeight=0.0
	 mColorRange(0)=(R=255,G=255,B=255,A=255)
     mColorRange(1)=(R=255,G=255,B=255,A=255)
	 bForceAffected=False
     mAttenKa=0.0
	 mAirResistance=0.0
     mSpawnVecB=(X=5.0,Y=0.0,Z=0.03)
}
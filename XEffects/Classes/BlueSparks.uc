//=============================================================================
// BlueSparks.
//=============================================================================
class BlueSparks extends xEmitter;

#exec TEXTURE IMPORT NAME=pcl_BlueSpark FILE=TEXTURES\BlueSpark.tga GROUP=Skins ALPHA=1 DXT=5

defaultproperties
{
	bHighDetail=True
	 Style=STY_Additive
     mParticleType=PT_Line
     mDirDev=(X=0.4,Y=0.4,Z=0.4)
     mPosDev=(X=0.2,Y=0.2,Z=0.2)
     mDelayRange(0)=0.0
     mDelayRange(1)=0.0
     mLifeRange(0)=0.8
     mLifeRange(1)=1.2
     mSpeedRange(0)=250.0
     mSpeedRange(1)=350.0
     mSizeRange(0)=1.0
     mSizeRange(1)=2.0
     mMassRange(0)=1.5
     mMassRange(1)=2.5
     mRegenRange(0)=0.0
     mRegenRange(1)=0.0
     mRegenDist=0.0
     mStartParticles=20
     mMaxParticles=10
     DrawScale=1.0
     ScaleGlow=2.0
	 mGrowthRate=-1.0
	 mAttenuate=true
	 mRegen=false;
     Skins(0)=Texture'pcl_BlueSpark'
     CollisionRadius=0.0
     CollisionHeight=0.0
	 mColorRange(0)=(R=255,G=255,B=255,A=255)
     mColorRange(1)=(R=255,G=255,B=255,A=255)
	 bForceAffected=False
     mAttenKa=0.1
	 mAirResistance=0.2
     mSpawnVecB=(X=2.0,Y=0.0,Z=0.06)
}
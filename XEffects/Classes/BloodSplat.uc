//=============================================================================
// BloodSplat.
//=============================================================================
class BloodSplat extends xEmitter;

#exec TEXTURE IMPORT NAME=pcl_Blooda FILE=TEXTURES\Blooda.tga GROUP=Skins Alpha=1 DXT=5

defaultproperties
{
	 Style=STY_Alpha
     mParticleType=PT_Sprite
     mDirDev=(X=0.300000,Y=0.300000,Z=0.300000)
     mPosDev=(X=1.800000,Y=1.800000,Z=1.800000)
     mDelayRange(0)=0.000000
     mDelayRange(1)=0.00000
     mLifeRange(0)=0.50000
     mLifeRange(1)=1.000000
     mSpeedRange(0)=100.000000
     mSpeedRange(1)=280.000000 
     mSizeRange(0)=2.500000
     mSizeRange(1)=6.500000
     mMassRange(0)=0.800000
     mMassRange(1)=1.400000
     mRegenRange(0)=0.000000
     mRegenRange(1)=0.000000
     mRegenDist=0.000000
     mStartParticles=14
     DrawScale=1.000000
     ScaleGlow=2.000000
	 mGrowthRate=0.0
	 mAttenuate=True
	 mRegen=False
	 mRandTextures=True
	 mRandOrient=True
     Skins(0)=Texture'pcl_Blooda'
     CollisionRadius=0.000000
     CollisionHeight=0.000000
	 mColorRange(0)=(R=255,G=255,B=255,A=255)
     mColorRange(1)=(R=255,G=255,B=255,A=255)
	 mCollision=False
	 bForceAffected=False
}
//=============================================================================
// Rain.
//=============================================================================
class Rain extends xEmitter;

#exec TEXTURE IMPORT NAME=pcl_rain FILE=TEXTURES\rain.tga GROUP=Skins ALPHA=1 DXT=5

defaultproperties
{
	 Style=STY_Translucent
     mParticleType=PL_Line
     mDirDev=(X=0.00000,Y=0.00000,Z=0.00000)
     mPosDev=(X=1000.800000,Y=1000.800000,Z=300.800000)
     mDelayRange(0)=0.000000
     mDelayRange(1)=0.00000
     mLifeRange(0)=0.50000
     mLifeRange(1)=1.000000
     mSpeedRange(0)=3000.000000
     mSpeedRange(1)=3000.000000
     mSizeRange(0)=3.500000
     mSizeRange(1)=3.500000
     mMassRange(0)=1.700000
     mMassRange(1)=1.700000
     mRegenRange(0)=150.000000
     mRegenRange(1)=150.000000
     mRegenDist=0.000000
     mStartParticles=150
     DrawScale=1.000000
     ScaleGlow=1.000000
	 mGrowthRate=0.0
	 mAttenuate=False
	 mRegen=True;
     Skins(0)=Texture'pcl_rain'
	 mLifeColorMap=None
     CollisionRadius=0.000000
     CollisionHeight=0.000000
	 mColorRange(0)=(R=255,G=255,B=255,A=255)
     mColorRange(1)=(R=255,G=255,B=255,A=255)
	 mCollision=False // temp test
	 LifeSpan=0.0
}
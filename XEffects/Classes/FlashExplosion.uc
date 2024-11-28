//=============================================================================
// FlashExplosion
//=============================================================================
class FlashExplosion extends xEmitter;

#EXEC TEXTURE IMPORT NAME=ExplosionFlashTex FILE=textures\FlashExp.tga  Alpha=1 GROUP=Skins DXT=5

defaultproperties 
{
	 Style=STY_Translucent  
     mParticleType=PL_Sprite
     mDirDev=(X=0.00000,Y=0.00000,Z=0.00000)
     mPosDev=(X=0.00000,Y=0.00000,Z=0.00000) 
     mDelayRange(0)=0.00000
     mDelayRange(1)=0.000
     mLifeRange(0)=0.250000
     mLifeRange(1)=0.2500000
     mSpeedRange(0)=0.000000
     mSpeedRange(1)=0.000000
     mSizeRange(0)=150.000000
     mSizeRange(1)=150.000000 
     mGrowthRate=500
     mMassRange(0)=0.000000
     mMassRange(1)=0.000000
	 mSpinRange(0)=-45.0
	 mSpinRange(1)=45.0
     mRegenDist=0.000000
     mStartParticles=1
     mMaxParticles=1
	 mRandOrient=True
	 mRegen=False
	 mRandTextures=false
     Texture=Texture'ExplosionFlashTex'
	 skins(0)=Texture'ExplosionFlashTex'
	 mAttenuate=True
	 mCollision=False
	 LifeSpan=0.3
}

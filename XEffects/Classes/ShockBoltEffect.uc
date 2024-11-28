//=============================================================================
// ShockBoltEffect
//=============================================================================
class ShockBoltEffect extends xEmitter;

#exec TEXTURE  IMPORT NAME=LsBBT FILE=textures\ShockEffect.tga GROUP="Skins"

defaultproperties
{
     RemoteRole=ROLE_None //SimulatedProxy
	 Physics=PHYS_Trailer
	 mPosRelative=True
     mPosDev=(X=5.00000,Y=5.00000,Z=5.00000)
     DrawScale=1.00000
	 Style=STY_Translucent
     mParticleType=PCL_Burst
     mLifeRange(0)=0.2000
     mLifeRange(1)=0.40000  
	 mRegenRange(0)=30.0000
	 mRegenRange(1)=30.0000
     mSizeRange(0)=40.00000
     mSizeRange(1)=85.00000
     mStartParticles=10
	 mMaxParticles=10
	 mAttenuate=True
	 mRandOrient=True
	 mRegen=True
	 Skins(0)=Texture'LsBBT'
	 mNumTileColumns=1
	 mNumTileRows=1
	 LifeSpan=10.0
}

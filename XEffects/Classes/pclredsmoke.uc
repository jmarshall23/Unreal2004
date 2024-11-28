//=============================================================================
// pclRedSmoke
//=============================================================================
class pclRedSmoke extends xEmitter;

#exec  TEXTURE IMPORT NAME=TrailLifeMap_t FILE=Textures\RocketTrailColorMap.PCX DXT=1 LODSET=3
#exec  TEXTURE IMPORT NAME=WispSmoke_t FILE=Textures\smoke_wisp.tga LODSET=2 DXT=1 LODSET=3

defaultproperties
{
	mParticleType=PT_Sprite 
	Style=STY_Translucent
    mDirDev=(X=0.0,Y=0.0,Z=0.0)
	mDelayRange(0)=0.000000
	mDelayRange(1)=0.00000
	mLifeRange(0)=0.80000
	mLifeRange(1)=1.000
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mSpinRange(0)=-4.0
	mSpinRange(1)=4.0
	mSizeRange(0)=35.000000
	mSizeRange(1)=40.000000
	mRegenRange(0)=80.000000
	mRegenRange(1)=100.000000 
	mRegenDist=0.000000
	mRandOrient=True
	mRandTextures=True
	mStartParticles=1
	mMaxParticles=100
	mGrowthRate=3.0
	mAttenuate=False
	mRegen=True
	Skins(0)=Texture'WispSmoke_t'
	CollisionRadius=0.000000
	CollisionHeight=0.000000
	mNumTileColumns=4
	mNumTileRows=4
	Physics=PHYS_Trailer
	mLifeColorMap=Texture'TrailLifeMap_t'
	LifeSpan=10.0
	bForceAffected=False
}
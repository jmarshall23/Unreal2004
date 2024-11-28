//=============================================================================
// pclSmallSmoke
//=============================================================================
class pclSmallSmoke extends xEmitter;

#exec  TEXTURE IMPORT NAME=EmitSmoke_t FILE=Textures\smoke_a.tga DXT=1 LODSET=3
    
defaultproperties
{
	mParticleType=PCL_Burst
	Style=STY_Translucent
	mDirDev=(X=0.200000,Y=0.200000,Z=0.200000)
	mPosDev=(X=0.10000,Y=0.10000,Z=0.10000)
	mDelayRange(0)=0.000000
	mDelayRange(1)=0.010000
	mLifeRange(0)=1.300000
	mLifeRange(1)=2.400000
	mSpeedRange(0)=10.000000
	mSpeedRange(1)=23.000000
	mSizeRange(0)=20.000000
	mSizeRange(1)=30.000000
	mMassRange(0)=0.000000
	mMassRange(1)=0.000000
	mRegenRange(0)=4.000000
	mRegenRange(1)=7.000000
	mRegenDist=0.000000
	mRegen=True
	mRandOrient=True
	mRandTextures=True
	mAttenuate=True
	mStartParticles=2
	mMaxParticles=20
	DrawScale=1.000000
	ScaleGlow=2.000000
	Skins(0)=Texture'EmitSmoke_t'
	CollisionRadius=0.000000
	CollisionHeight=0.000000
	mColorRange(0)=(R=140,G=100,B=100,A=255)
	mColorRange(1)=(R=180,G=180,B=180,A=255)
	mNumTileColumns=4
	mNumTileRows=4
}
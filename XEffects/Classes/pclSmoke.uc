//=============================================================================
// pclSmoke
//=============================================================================
class pclSmoke extends xEmitter;

#exec  TEXTURE IMPORT NAME=EmitSmoke_t FILE=Textures\smoke_a.tga LODSET=2 DXT=5

defaultproperties
{
	mParticleType=PCL_Burst
	Style=STY_Translucent
	mDirDev=(X=0.000000,Y=0.000000,Z=0.000000)
	mPosDev=(X=0.00000,Y=0.00000,Z=0.00000)
	mDelayRange(0)=0.000000
	mDelayRange(1)=0.000000
	mLifeRange(0)=1.000000
	mLifeRange(1)=1.00000
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mSizeRange(0)=45.000000
	mSizeRange(1)=45.000000
	mMassRange(0)=0.000000
	mMassRange(1)=0.000000
	mRegenRange(0)=140.000000
	mRegenRange(1)=140.000000
	mRegenDist=0.000000
	mRegen=True
	mRandOrient=True
	mRandTextures=True
	mAttenuate=True
	mStartParticles=2
	mMaxParticles=150
	DrawScale=1.000000
	ScaleGlow=2.000000
	Skins(0)=Texture'EmitSmoke_t'
	CollisionRadius=0.000000
	CollisionHeight=0.000000
	mColorRange(0)=(R=120,G=110,B=100,A=255)
	mColorRange(1)=(R=180,G=180,B=180,A=255)
	mNumTileColumns=4
	mNumTileRows=4
}
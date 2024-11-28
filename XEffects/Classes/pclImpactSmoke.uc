//=============================================================================
// pclImpactSmoke
//=============================================================================
class pclImpactSmoke extends xEmitter;


defaultproperties
{
    mParticleType=PCL_Burst
	Style=STY_Translucent
	mMassRange(0)=0.000000
	mMassRange(1)=0.000000
	mRegenRange(0)=140.000000
	mRegenRange(1)=140.000000
	mRegenDist=0.000000
	
	mRandOrient=True
	mRandTextures=True
	mAttenuate=True
	mMaxParticles=16
	DrawScale=1.000000
	ScaleGlow=2.000000
	Skins(0)=Texture'EmitSmoke_t'

	mNumTileColumns=4
	mNumTileRows=4

	mDirDev=(X=0.150000,Y=0.150000,Z=0.150000)
	mPosDev=(X=8.00000,Y=8.00000,Z=8.00000)
	mDelayRange(0)=0.000000
	mDelayRange(1)=0.100000
	mLifeRange(0)=0.500000
	mLifeRange(1)=1.50000 
	mSpeedRange(0)=10.000000
	mSpeedRange(1)=60.000000
	mSizeRange(0)=10.000000
	mSizeRange(1)=25.000000
	mStartParticles=7
	mRegen=False
	mColorRange(0)=(R=20,G=20,B=20,A=255)
	mColorRange(1)=(R=40,G=40,B=40,A=255)
	mSpinRange(0)=-10.0
	mSpinRange(1)=10.0
    CullDistance=+7000.0
}
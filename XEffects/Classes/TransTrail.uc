//=============================================================================
// TransTrail
//=============================================================================
class TransTrail extends pclSmoke;

#exec TEXTURE  IMPORT NAME=TransTrailT FILE=textures\TransTrail.tga GROUP="Skins" MIPS=0

defaultproperties
{ 
	bOwnerNoSee=true
    Style=STY_Additive
    mParticleType=PT_Stream
    mDirDev=(X=0.000000,Y=0.000000,Z=0.000000)
    mPosDev=(X=0.000000,Y=0.000000,Z=0.000000)  
	mLifeRange(0)=1.5
	mLifeRange(1)=1.5 
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mSizeRange(0)=9.000000
	mSizeRange(1)=9.000000
	mRegen=true
	mRegenRange(0)=20.000000 
	mRegenRange(1)=20.000000 
	mRegenDist=0.000000
	mStartParticles=0
	mAttenuate=true
	mMaxParticles=100
	mGrowthRate=6.0
	Skins(0)=Texture'TransTrailT'  
	Physics=PHYS_Trailer
	LifeSpan=0.0
	mNumTileColumns=1
	mNumTileRows=1
    mColorRange(0)=(R=80,G=150,B=255,A=255)
    mColorRange(1)=(R=80,G=150,B=255,A=255)
    mSpawnVecB=(X=10.0,Y=0.0,Z=0.0)
    mAttenKa=0.0
}
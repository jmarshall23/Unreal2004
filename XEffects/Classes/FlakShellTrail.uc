//=============================================================================
// FlakShellTrail
//=============================================================================
class FlakShellTrail extends pclSmoke; 

#exec  TEXTURE IMPORT NAME=FlakMap_t FILE=Textures\FlakColorMap.PCX LODSET=3 DXT=1

defaultproperties
{ 
	Style=STY_Alpha 
    mDirDev=(X=0.100000,Y=0.100000,Z=0.100000) 
    mPosDev=(X=2.000000,Y=2.000000,Z=2.000000)   
	mLifeRange(0)=0.5000
	mLifeRange(1)=0.600 
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mSizeRange(0)=20.000000
	mSizeRange(1)=35.000000
	mRegenRange(0)=50.000000 
	mRegenRange(1)=50.000000 
	mRegenDist=0.000000
	mStartParticles=1
	mAttenuate=True
	mMaxParticles=150
	mGrowthRate=15.0
	Skins(0)=Texture'SmokeAlphab_t'  
	//mLifeColorMap=Texture'FlakMap_t'
	Physics=PHYS_Trailer
	lifespan=5.0
	mNumTileColumns=4
	mNumTileRows=4
}
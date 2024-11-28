//=============================================================================
// pclLightSmoke
//=============================================================================
class pclLightSmoke extends pclSmoke;

#exec  TEXTURE IMPORT NAME=EmitLightSmoke_t FILE=Textures\smokelight_a.tga LODSET=3 DXT=1

defaultproperties
{
	Style=STY_Translucent
    mDirDev=(X=0.600000,Y=0.600000,Z=0.600000)
	mDelayRange(0)=0.000000
	mDelayRange(1)=0.000000
	mLifeRange(0)=0.800000
	mLifeRange(1)=1.40000
	mSpeedRange(0)=80.000000
	mSpeedRange(1)=110.000000
	mSizeRange(0)=70.000000
	mSizeRange(1)=86.000000
	mRegenRange(0)=150.000000
	mRegenRange(1)=150.000000
	mRegenDist=0.000000
	mStartParticles=1
	mMaxParticles=150
	Skins(0)=Texture'EmitLightSmoke_t'
	mColorRange(0)=(R=75,G=75,B=75,A=255)
	mColorRange(1)=(R=110,G=110,B=110,A=255)
}
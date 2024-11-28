// ============================================================
// pclMuzFlashRocket
// 
// 1st person muzzle flash
// ============================================================
 
class pclMuzFlashRocket extends pclMuzFlashA;

#exec TEXTURE IMPORT NAME=MuzFlashRocket_t FILE=Textures\MuzzleFlashRocket.tga GROUP="Skins" LODSET=3 DXT=1
 
  
defaultproperties
{
	Texture=Texture'MuzFlashRocket_t'  
	Skins(0)=Texture'MuzFlashRocket_t'  
	bOnlyOwnerSee=True 
	mRandOrient=True 
	mSizeRange(0)=48.500000  
	mSizeRange(1)=55.000000
	mLifeRange(0)=0.050000
	mLifeRange(1)=0.050000 
	mStartParticles=0
	mMaxParticles=2
	mColorRange(0)=(R=95,G=95,B=95,A=255)
	mColorRange(1)=(R=95,G=95,B=95,A=255)
}
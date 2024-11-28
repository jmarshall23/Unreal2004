// ============================================================
// pclLinkMuzFlash
// 
// Machinegun 1st person muzzle flash
// ============================================================
 
class pclLinkMuzFlash extends pclMuzFlashA;

#exec TEXTURE IMPORT NAME=MuzFlashLink_t FILE=Textures\MuzzleFlashLink.tga GROUP="Skins" LODSET=3 DXT=1
 
  
defaultproperties
{
	Style=STY_Translucent
	Skins(0)=Texture'MuzFlashLink_t' 
	bOnlyOwnerSee=True 
	mRandOrient=True 
	mSizeRange(0)=110.00000  
	mSizeRange(1)=120.00000
	mLifeRange(0)=0.050000
	mLifeRange(1)=0.050000 
	mStartParticles=2
	mMaxParticles=2
	mColorRange(0)=(R=200,G=200,B=200,A=255)
	mColorRange(1)=(R=200,G=200,B=200,A=255)
}
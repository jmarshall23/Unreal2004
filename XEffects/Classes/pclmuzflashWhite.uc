// ============================================================
// pclMuzFlashWhite
// 
// 1st person muzzle flash
// ============================================================
 
class pclMuzFlashWhite extends pclMuzFlashA;

#exec TEXTURE IMPORT NAME=MuzFlashWhite_t FILE=Textures\MuzzleFlashWhite.tga GROUP="Skins" LODSET=3 DXT=1
 
  
defaultproperties
{
	Texture=Texture'MuzFlashWhite_t'  
	Skins(0)=Texture'MuzFlashWhite_t'  
	bOnlyOwnerSee=True 
	mRandOrient=True 
	mSizeRange(0)=37.00000  
	mSizeRange(1)=40.000000
	mLifeRange(0)=0.050000
	mLifeRange(1)=0.050000 
	mStartParticles=0
	mMaxParticles=2
	mColorRange(0)=(R=95,G=95,B=95,A=255)
	mColorRange(1)=(R=95,G=95,B=95,A=255)
}
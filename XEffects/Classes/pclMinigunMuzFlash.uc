// ============================================================
// pclMinigunMuzFlash
// 
// Minigun 1st person muzzle flash
// ============================================================
 
class pclMinigunMuzFlash extends pclMuzFlashA;

#exec TEXTURE IMPORT NAME=MuzFlashA_t FILE=Textures\MuzzleFlashA.tga GROUP="Skins" LODSET=3 DXT=1
 
  
defaultproperties
{
	Style=STY_Translucent
	Texture=Texture'MuzFlashA_t'
	Skins(0)=Texture'MuzFlashA_t'
	mSizeRange(0)=25.00000  
	mSizeRange(1)=40.00000
	mStartParticles=1
	mMaxParticles=1
	mColorRange(0)=(R=255,G=255,B=255,A=255)
	mColorRange(1)=(R=255,G=255,B=255,A=255)
}
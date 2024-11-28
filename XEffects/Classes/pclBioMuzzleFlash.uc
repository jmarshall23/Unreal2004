// ============================================================
// pclBioMuzFlash
// 
// BioRifle 1st person muzzle flash
// ============================================================
 
class pclBioMuzzleFlash extends pclMuzFlashA;

#exec TEXTURE IMPORT NAME=MuzFlashBio_t FILE=Textures\MuzzleFlashBio.tga GROUP="Skins" LODSET=3 DXT=1
 
  
defaultproperties
{
	Skins(0)=Texture'MuzFlashBio_t' 
	bOnlyOwnerSee=True 
	mRandOrient=False
	mSizeRange(0)=46.500000  
	mSizeRange(1)=49.000000
	mLifeRange(0)=0.050000
	mLifeRange(1)=0.060000 
	mStartParticles=2
	mMaxParticles=2
	mColorRange(0)=(R=100,G=100,B=100,A=255)
	mColorRange(1)=(R=100,G=100,B=100,A=255)
}
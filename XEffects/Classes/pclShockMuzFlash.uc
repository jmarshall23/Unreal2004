// ============================================================
// pclShockMuzFlash
// 
// ShockRifle 1st person muzzle flash
// ============================================================
 
class pclShockMuzFlash extends pclMuzFlashA;

#exec TEXTURE IMPORT NAME=MuzShockFlash_t FILE=Textures\Shockmuz.tga GROUP="Skins" DXT=1 LODSET=3
 
    
defaultproperties
{
	Style=STY_Translucent
	Skins(0)=Texture'MuzShockFlash_t'
	mSizeRange(0)=10.500000  
	mSizeRange(1)=11.000000
	mRandOrient=False
	mStartParticles=1
	mMaxParticles=1
	mColorRange(0)=(R=200,G=200,B=200,A=255)
	mColorRange(1)=(R=200,G=200,B=200,A=255)
}
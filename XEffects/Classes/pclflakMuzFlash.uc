// ============================================================
// pclFlakMuzFlash
// 
// Flak Cannon 1st person muzzle flash
// ============================================================
 
class pclFlakMuzFlash extends pclMuzFlashA;

#exec TEXTURE IMPORT NAME=MuzFlashFlak_t FILE=Textures\FlakFlash.tga GROUP="Skins" LODSET=3 DXT=1
 
  
defaultproperties 
{
	Style=STY_Translucent
	Skins(0)=Texture'MuzFlashFlak_t'
	Texture=Texture'MuzFlashFlak_t'
	mSizeRange(0)=85.0
	mSizeRange(1)=110.0
	//mColorRange(0)=(R=255,G=255,B=255,A=255)
	//mColorRange(1)=(R=255,G=255,B=255,A=255)
}
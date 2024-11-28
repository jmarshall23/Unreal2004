//=============================================================================
// BotSmallHit.
//=============================================================================
class BotSmallHit extends BloodSpurt;

#exec OBJ LOAD File=XGameShadersB.utx

#exec TEXTURE IMPORT NAME=BloodSplat1P FILE=TEXTURES\DECALS\BloodSplat1P.tga MODULATED=1 LODSET=2 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP
#exec TEXTURE IMPORT NAME=BloodSplat2P FILE=TEXTURES\DECALS\BloodSplat2P.tga MODULATED=1 LODSET=2 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP
#exec TEXTURE IMPORT NAME=BloodSplat3P FILE=TEXTURES\DECALS\BloodSplat3P.tga MODULATED=1 LODSET=2 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP


defaultproperties  
{
	splats(0)=Texture'BloodSplat1P' 
	splats(1)=Texture'BloodSplat2P'
	splats(2)=Texture'BloodSplat3P'
    BloodDecalClass=class'BloodSplatterPurple'
    Style=STY_Alpha
    mSizeRange(0)=10.00000 
    mSizeRange(1)=15.00000
    mDirDev=(X=0.7,Y=0.7,Z=0.7) 
    mPosDev=(X=5.00000,Y=5.00000,Z=5.00000)
    mLifeRange(0)=0.50000
    mLifeRange(1)=0.900000
    mSpeedRange(0)=20.000000
    mSpeedRange(1)=70.000000 
    mMassRange(0)=0.100000
    mMassRange(1)=0.200000
    mDelayRange(0)=0.0
    mDelayRange(1)=0.1
    mNumTileColumns=1
    mNumTileRows=1
    Skins(0)=XGameShadersB.Blood.BloodPuffOil
}
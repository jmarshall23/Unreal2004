class FlakMuzFlash1st extends MinigunMuzFlash1st;
 
#exec OBJ LOAD FILE=xGameShaders.utx
#exec STATICMESH IMPORT NAME=FlakMuzFlashMesh FILE=Models\FlakMuzFlash.lwo COLLISION=0

defaultproperties
{
    mTileAnimation=false
    mNumTileRows=1
    mNumTileColumns=1
	Skins(0)=FinalBlend'XGameShaders.WeaponShaders.FlakFlashFinal'
	mMeshNodes(0)=StaticMesh'FlakMuzFlashMesh'
    mGrowthRate=7.0
    DrawScale=2.2
}


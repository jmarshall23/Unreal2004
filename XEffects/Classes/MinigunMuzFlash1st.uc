class MinigunMuzFlash1st extends xEmitter;

#exec OBJ LOAD FILE=xGameShaders.utx

#exec STATICMESH IMPORT NAME=MinigunMuzFlash1stMesh FILE=Models\MinigunMuzFlash1st.lwo COLLISION=0

var int mNumPerFlash;

simulated function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles += mNumPerFlash;
}
 
defaultproperties
{
	DrawScale=0.900000
    mTileAnimation=true
    mNumTileRows=2
    mNumTileColumns=2
    mNumPerFlash=5
	mStartParticles=0
	mMaxParticles=5
	mLifeRange(0)=0.10000
	mLifeRange(1)=0.15000
	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mAirResistance=0.000000
	mSizeRange(0)=0.0500000
	mSizeRange(1)=0.080000
	mGrowthRate=3.00000
	mMeshNodes(0)=StaticMesh'MinigunMuzFlash1stMesh'
	mSpawnVecB=(Z=0.000000)
	mParticleType=PT_Mesh
	mPosRelative=True
	mRandOrient=false
	mRandTextures=True
	Skins(0)=FinalBlend'xGameShaders.WeaponShaders.MinigunMuzFlash1stFinal'
	Style=STY_Additive
}

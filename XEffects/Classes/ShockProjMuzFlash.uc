class ShockProjMuzFlash extends xEmitter;

#exec OBJ LOAD FILE=xGameShaders.utx

var int mNumPerFlash;

simulated function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles += mNumPerFlash;
}
 
defaultproperties
{
	DrawScale=1.0
    mTileAnimation=false
    mNumTileRows=2
    mNumTileColumns=2
    mNumPerFlash=3
	mStartParticles=0
	mMaxParticles=6
	mLifeRange(0)=0.2
	mLifeRange(1)=0.2
	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
	mSpeedRange(0)=0.0
	mSpeedRange(1)=0.0
	mAirResistance=0.0
	mSizeRange(0)=0.6
	mSizeRange(1)=0.7
	mGrowthRate=0.0
	mMeshNodes(0)=StaticMesh'MinigunMuzFlash1stMesh'
	mSpawnVecB=(Z=0.000000)
	mParticleType=PT_Mesh
	mPosRelative=True
	mRandOrient=false
	mRandTextures=True
	Skins(0)=FinalBlend'xGameShaders.WeaponShaders.ShockMuzFlash1stFinal'
	Style=STY_Additive
    RemoteRole=ROLE_None
    mAttenKa=0.0
}

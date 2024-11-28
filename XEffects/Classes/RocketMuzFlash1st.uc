class RocketMuzFlash1st extends xEmitter;

#exec OBJ LOAD FILE=xGameShaders.utx

var int mNumPerFlash;

simulated function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles += mNumPerFlash;
}
 
defaultproperties
{
	DrawScale=2.0
    mTileAnimation=true
    mNumTileRows=2
    mNumTileColumns=2
    mNumPerFlash=5
	mStartParticles=0
	mMaxParticles=5
	mLifeRange(0)=0.15000
	mLifeRange(1)=0.15000
	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mAirResistance=0.000000
	mSizeRange(0)=0.4
	mSizeRange(1)=0.4
	mGrowthRate=0.0
	mMeshNodes(0)=StaticMesh'MinigunMuzFlash1stMesh'
	mSpawnVecB=(Z=0.000000)
	mParticleType=PT_Mesh
	mPosRelative=True
	mRandOrient=false
	mRandTextures=True
	Skins(0)=FinalBlend'xGameShaders.WeaponShaders.MinigunMuzFlash1stFinal'
	Style=STY_Additive
    mColorRange(0)=(R=48,G=40,B=40)
    mColorRange(1)=(R=48,G=40,B=40)
}


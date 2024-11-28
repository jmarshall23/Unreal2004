class MinigunMuzFlash3rd extends xEmitter;

// muzzle flash //
#exec STATICMESH IMPORT     NAME=MinigunFlashMesh FILE=Models\MinigunFlash.lwo COLLISION=0
 
var int mNumPerFlash;

function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles = mNumPerFlash;
    mGrowthRate = default.mGrowthRate;
    mLifeRange[0] = default.mLifeRange[0];
    mLifeRange[1] = default.mLifeRange[1];
}

defaultproperties
{
    mNumPerFlash=1
	mStartParticles=0
	mMaxParticles=6
	mLifeRange(0)=0.150000
	mLifeRange(1)=0.200000
	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mAirResistance=0.000000
	mSizeRange(0)=0.0500000
	mSizeRange(1)=0.080000
	mGrowthRate=2.200000
	mMeshNodes(0)=StaticMesh'MinigunFlashMesh'
	mSpawnVecB=(Z=0.000000)
	mParticleType=PT_Mesh
	mPosRelative=True
	mRandOrient=True
	mRandTextures=True
	Skins(0)=FinalBlend'XGameShaders.Minigun.MinigunFlashFinal'
	Style=STY_Additive
}

class ShockProjMuzFlash3rd extends xEmitter;

#exec OBJ LOAD FILE=xGameShaders.utx

var int mNumPerFlash;

simulated function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles += mNumPerFlash;
}
 
defaultproperties
{
    mNumPerFlash=1
	mStartParticles=0
	mMaxParticles=6
	mLifeRange(0)=0.130000
	mLifeRange(1)=0.130000
	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mAirResistance=0.000000
	mSizeRange(0)=0.350000
	mSizeRange(1)=0.400000
	mGrowthRate=3.200000
	mMeshNodes(0)=StaticMesh'MinigunFlashMesh'
	mSpawnVecB=(Z=0.000000)
	mParticleType=PT_Mesh
	mPosRelative=True
	mRandOrient=True
	mRandTextures=True
    mAttenuate=true
    mAttenKa=0.0
	Skins(0)=FinalBlend'XGameShaders.Minigun.ShockMuzFlash3rdFinal'
	Style=STY_Additive
    mColorRange(0)=(R=50,B=255,G=255)
    mColorRange(1)=(R=50,B=255,G=255)
}

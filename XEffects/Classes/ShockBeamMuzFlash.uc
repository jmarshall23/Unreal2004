class ShockBeamMuzFlash extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

var int mNumPerFlash;

simulated function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles += mNumPerFlash;
}
 
defaultproperties
{
	DrawScale=1.0
    mNumPerFlash=1
	mStartParticles=0
	mMaxParticles=5
	mLifeRange(0)=0.14
	mLifeRange(1)=0.14
	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
	mSpeedRange(0)=0.0
	mSpeedRange(1)=0.0
	mAirResistance=0.0
	mSizeRange(0)=0.5
	mSizeRange(1)=0.5
	mGrowthRate=5.0
	mMeshNodes(0)=StaticMesh'WeaponStaticMesh.ShockMuzFlash'
	mSpawnVecB=(Z=0.000000)
	mParticleType=PT_Mesh
	mPosRelative=true
	mRandOrient=false
	//Skins(0)=FinalBlend'xGameShaders.WeaponShaders.ShockMuzFlash1stFinal'
	Skins(0)=FinalBlend'XEffectMat.ShockFlashFB'
	Style=STY_Additive
    mAttenKa=0.0
    //mColorRange(0)=(R=50,B=128,G=0)
    //mColorRange(1)=(R=50,B=128,G=0)
}

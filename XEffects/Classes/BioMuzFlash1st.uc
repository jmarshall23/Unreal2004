class BioMuzFlash1st extends xEmitter;
 
#exec OBJ LOAD FILE=xGameShaders.utx

var int mNumPerFlash;

simulated function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles += mNumPerFlash;
}

defaultproperties
{
    mNumPerFlash=1
    mParticleType=PT_Mesh
    mMeshNodes(0)=StaticMesh'FlakMuzFlashMesh'
    Skins(0)=FinalBlend'XGameShaders.WeaponShaders.BioFlashFinal'
    mNumTileRows=1
    mNumTileColumns=1
    mTileAnimation=false
    mGrowthRate=10.0
    mSizeRange(0)=0.6
    mSizeRange(1)=0.8
    mRandOrient=true
    mColorRange(0)=(R=180,G=180,B=180,A=255)
    mColorRange(1)=(R=180,G=180,B=180,A=255)
    mStartParticles=0
    mMaxParticles=5
    mLifeRange(0)=0.10
    mLifeRange(1)=0.15
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mAirResistance=0.0
    mSpawnVecB=(Z=0.0)
    mPosRelative=True
    bUnlit=true
    bHidden=true
}

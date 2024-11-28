class LinkMuzFlashProj1st extends xEmitter;
 
#exec OBJ LOAD FILE=xGameShaders.utx
#exec OBJ LOAD FILE=XEffectMat.utx

var int mNumPerFlash;

simulated function Trigger(Actor Other, Pawn EventInstigator)
{
    mStartParticles += mNumPerFlash;
}

defaultproperties
{
    mNumPerFlash=2
    mParticleType=PT_Mesh
    mMeshNodes(0)=StaticMesh'LinkMuzFlashMesh'
    Skins(0)=FinalBlend'XEffectMat.LinkMuzProjGreenFB'
    mNumTileRows=2
    mNumTileColumns=2
    mRandTextures=true
    mTileAnimation=false
    mGrowthRate=0.0
    mSizeRange(0)=0.6
    mSizeRange(1)=0.8
    mRandOrient=true
    mColorRange(0)=(R=75,G=75,B=75,A=255)
    mColorRange(1)=(R=75,G=75,B=75,A=255)
    mStartParticles=0
    mMaxParticles=5
    mLifeRange(0)=0.10
    mLifeRange(1)=0.14
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mAirResistance=0.0
    mSpawnVecB=(Z=0.0)
    mPosRelative=true
    bUnlit=true
    bHidden=true
}

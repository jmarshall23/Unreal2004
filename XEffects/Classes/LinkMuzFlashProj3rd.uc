class LinkMuzFlashProj3rd extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

#exec STATICMESH IMPORT NAME=LinkMuzFlashMesh FILE=Models\link_projectile_flash.lwo COLLISION=0

simulated event Trigger( Actor Other, Pawn EventInstigator )
{
    mStartParticles = 1;
}

defaultproperties
{
    Skins(0)=FinalBlend'XEffectMat.LinkMuzProjGreenFB'
    mStartParticles=0
    mMaxParticles=3
    mRegen=true
    mLifeRange(0)=0.08
    mLifeRange(1)=0.12
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mAirResistance=0.0
    mSizeRange(0)=0.6
    mSizeRange(1)=0.6
    mGrowthRate=0.0
    mMeshNodes(0)=StaticMesh'LinkMuzFlashMesh'
    mParticleType=PT_Mesh
    mPosRelative=true
    mRandTextures=true
    mNumTileColumns=2
    mNumTileRows=2
    mColorRange(0)=(R=70,G=70,B=70)
    mColorRange(1)=(R=70,G=70,B=70)
    mAttenKa=0.0
}


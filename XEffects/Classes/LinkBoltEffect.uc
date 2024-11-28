//=============================================================================
// LinkBoltEffect
//=============================================================================
class LinkBoltEffect extends xEmitter;

#exec TEXTURE  IMPORT NAME=LBBT FILE=textures\LinkEffect.tga GROUP="Skins"

defaultproperties
{
    RemoteRole=ROLE_None //SimulatedProxy
    Physics=PHYS_Trailer
    mPosRelative=True
    mPosDev=(X=10.00000,Y=10.00000,Z=10.00000)
    DrawScale=1.00000
    Style=STY_Translucent
    mParticleType=PCL_Burst
    mLifeRange(0)=0.3000
    mLifeRange(1)=0.60000
    mRegenRange(0)=15.0000
    mRegenRange(1)=15.0000
    mSizeRange(0)=60.00000
    mSizeRange(1)=60.00000
    mStartParticles=0
    mMaxParticles=5
    mAttenuate=True
    mRandOrient=True
    mRegen=True
    Texture=Texture'LBBT'
    Skins(0)=Texture'LBBT'
    mNumTileColumns=1
    mNumTileRows=1
    LifeSpan=2.0
}

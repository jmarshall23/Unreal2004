class LightningCharge extends xEmitter;

#exec TEXTURE IMPORT NAME=LightningChargeT FILE=textures\LightningCharge.tga DXT=1

defaultproperties
{
	mNumTileColumns=2
	mNumTileRows=2
	mSizeRange(0)=5.000000
	mSizeRange(1)=5.000000
	mAttenKa=0.000000
	mRandOrient=True
	bDynamicLight=false
    Skins(0)=Material'LightningChargeT'
    Style=STY_Additive
    mStartParticles=0
    mMaxParticles=10
    mRegen=true
    mRegenRange(0)=10.0
    mRegenRange(1)=10.0
    mLifeRange(0)=0.2
    mLifeRange(1)=0.2
    mRegenDist=0.0
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=0.0
    mSpinRange(1)=0.0
    mParticleType=PT_Sprite
    mSpawningType=ST_Sphere
    mPosDev=(X=10.0)
    mPosRelative=true
    RemoteRole=ROLE_None        
    bNetTemporary=false
    RelativeLocation=(X=-30)
}

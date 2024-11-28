class LinkProjEffect extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
	bHighDetail=true
    Physics=PHYS_Trailer
    Skins=(Texture'XEffectMat.Link.link_muz_green')
    Style=STY_Additive
    mStartParticles=0
    mLifeRange(0)=0.7
    mLifeRange(1)=0.7
    mRegenRange(0)=16.0
    mRegenRange(1)=16.0
    mSpeedRange(0)=225.0
    mSpeedRange(1)=225.0
    mSizeRange(0)=18.0
    mSizeRange(1)=18.0
    mGrowthRate=-18.0
    mAirResistance=0.0
    mSpinRange(0)=-200.0
    mSpinRange(1)=200.0
    mAttenKa=0.0
    mDirDev=(Y=0.01,Z=0.01)
    mPosDev=(Y=4.0,Z=4.0)
    mColorRange(0)=(B=170,G=170,R=170)
    mColorRange(1)=(B=170,G=170,R=170)
    mPosRelative=false
    mOwnerVelocityFactor=1.0
}

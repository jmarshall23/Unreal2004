class OffensiveEffect extends xEmitter;

defaultproperties
{
    RemoteRole=ROLE_SimulatedProxy
    Physics=PHYS_Trailer
    bTrailerSameRotation=true
    bReplicateMovement=false
    bNetTemporary=false
    Skins=(FinalBlend'XEffectMat.RedBoltFB')
    Style=STY_Additive
    mParticleType=PT_Mesh
    mStartParticles=0
    mMaxParticles=10
    mLifeRange(0)=1.0
    mLifeRange(1)=1.0
    mRegenRange(0)=3.0
    mRegenRange(1)=3.0
    mPosDev=(X=10.0,Y=10.0,Z=10.0)
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mPosRelative=true
    mAirResistance=0.0
    mRandOrient=true
    mSizeRange(0)=0.7
    mSizeRange(1)=1.1
    mColorRange(0)=(B=150,G=150,R=250)
    mColorRange(1)=(B=150,G=150,R=250)
    mAttenKa=0.5
    mAttenFunc=ATF_ExpInOut
    mMeshNodes(0)=StaticMesh'XEffects.TeleRing'
    LifeSpan=+60.0
}


class TransRecallEffect extends xEmitter;

defaultproperties
{
	Style=STY_Additive
	Skins(0)=Texture'TransTrailT'  
	bForceAffected=false

	mParticleType=PT_Line
	mSpawningType=ST_Explode

	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
    mDelayRange(0)=0.0
	mDelayRange(1)=0.15
	mStartParticles=0
    mMaxParticles=45

    mLifeRange(0)=0.25
	mLifeRange(1)=0.25
	mSpeedRange(0)=-25.0
	mSpeedRange(1)=-25.0
	mSizeRange(0)=0.5
	mSizeRange(1)=0.5
    mGrowthRate=2.5
	mPosDev=(X=7.5,Y=7.5,Z=7.5)
    mSpawnVecB=(X=3.0,Z=0.08)

	mMassRange(0)=0.0
	mMassRange(1)=0.0
	mAirResistance=0.0

	mColorRange(0)=(B=200,G=150,R=150,A=100)
	mColorRange(1)=(B=200,G=150,R=150,A=100)

	mAttenuate=false

    mPosRelative=true
}

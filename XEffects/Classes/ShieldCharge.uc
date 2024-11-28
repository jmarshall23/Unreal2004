class ShieldCharge extends xEmitter;

defaultproperties
{
	mLifeRange(0)=0.10000
	mLifeRange(1)=0.10000
	mSpeedRange(0)=-50.000000
	mSpeedRange(1)=-50.000000

	mSizeRange(0)=0.200000 
	mSizeRange(1)=0.400000 
    mGrowthRate=0.000000
	mPosDev=(X=5.000000,Y=5.000000,Z=5.000000)
    mSpawnVecB=(X=5.000000,Z=0.080000)

	mParticleType=PT_Line
	mSpawningType=ST_Explode

	mStartParticles=0
    mMaxParticles=100

	mRegenRange(0)=50.000000
	mRegenRange(1)=50.000000

	mMassRange(0)=0.000000
	mMassRange(1)=0.000000
	mAirResistance=0.000000

	mColorRange(0)=(B=45,G=220,R=45,A=25)
	mColorRange(1)=(B=65,G=250,R=65,A=200)

	mAttenKa=0.200000
	bForceAffected=False
    Physics=PHYS_Rotating
	Style=STY_Additive
    Skins(0)=Texture'FlakTrailTex'
	bFixedRotationDir=True
	RotationRate=(Yaw=16000)

    mPosRelative=true
    bOnlyOwnerSee=true
}

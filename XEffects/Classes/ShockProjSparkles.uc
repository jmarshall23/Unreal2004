class ShockProjSparkles extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
    Physics=PHYS_Trailer
	Skins(0)=Texture'XEffectMat.shock_sparkle'
	Style=STY_Translucent
	mSpawningType=ST_Explode
	mStartParticles=0
	mMaxParticles=20
	mLifeRange(0)=0.4
	mLifeRange(1)=0.5
	mRegenRange(0)=20.0
	mRegenRange(1)=20.0
	mSpeedRange(0)=50.0
	mSpeedRange(1)=50.0
	mSizeRange(0)=40.0
	mSizeRange(1)=40.0
	mGrowthRate=-20.0
	mPosDev=(X=9.0,Y=9.0,Z=9.0)
    mAttenuate=true
    mAttenKa=0.2
    mPosRelative=true
    mColorRange(0)=(G=160)
    mColorRange(1)=(G=160)
}

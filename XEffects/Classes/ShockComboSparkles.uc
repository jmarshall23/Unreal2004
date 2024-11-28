class ShockComboSparkles extends xEmitter;

#exec OBJ LOAD FILE=XEffectMat.utx

defaultproperties
{
	Skins(0)=Texture'XEffectMat.shock_sparkle'
	Style=STY_Translucent
    LifeSpan=2.0
	mSpawningType=ST_Explode
	mStartParticles=0
	mMaxParticles=25
	mLifeRange(0)=0.75
	mLifeRange(1)=0.75
	mRegenOnTime(0)=0.65
	mRegenOnTime(1)=0.65
	mRegenOffTime(0)=10.0
	mRegenOffTime(1)=10.0
    mRegenPause=true
	mRegenRange(0)=25.0
	mRegenRange(1)=25.0
	mSpeedRange(0)=-250.0
	mSpeedRange(1)=-250.0
	mSizeRange(0)=65.0
	mSizeRange(1)=65.0
	mGrowthRate=-80.0
	mPosDev=(X=120.0,Y=120.0,Z=120.0)
    mAttenuate=true
    mAttenKa=0.3
}

class ImpactDust extends xEmitter;

var() float MaxImpactParticles;

simulated function Kick( Vector position, float atten )
{
	SetLocation( position );
	mLastPos = position;
	mStartParticles = ClampToMaxParticles(mStartParticles + MaxImpactParticles * atten);
}

defaultproperties
{
	Style=STY_Translucent
	mSpawningType=ST_AimedSphere
	mRegen=true;
    mDirDev=(X=1.0,Y=1.0,Z=0.0)
	mPosDev=(X=8.0,Y=8.0,Z=0.0)
	mAirResistance=0.4
	mDelayRange(0)=0.000000
	mDelayRange(1)=0.100000
	mLifeRange(0)=0.800000
	mLifeRange(1)=1.40000
	mSpeedRange(0)=20.000000
	mSpeedRange(1)=30.000000
	mSizeRange(0)=20.000000
	mSizeRange(1)=25.000000
	mRegenRange(0)=0.0
	mRegenRange(1)=0.0
	mMassRange(0)=-0.05
	mMassRange(1)=-0.1
	mNumTileColumns=4
	mNumTileRows=4
	mRegenDist=0.0
	mStartParticles=1
	mMaxParticles=150
	Skins(0)=Texture'EmitLightSmoke_t'
	mColorRange(0)=(R=11,G=16,B=16,A=255)
	mColorRange(1)=(R=22,G=22,B=22,A=255)

	mAttenKa=0.05
	MaxImpactParticles=20.0
}
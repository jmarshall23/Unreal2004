// ============================================================
// HitFlame 
// ============================================================
#exec OBJ LOAD File=GeneralAmbience.uax

class HitFlame extends xEmitter;

state Ticking
{
	simulated function Tick( float dt )
	{
		if( LifeSpan < 2.0 )
		{
			mRegenRange[0] *= LifeSpan * 0.5;
			mRegenRange[1] = mRegenRange[0];
			SoundVolume = byte(float(SoundVolume) * (LifeSpan * 0.5));
		}
	}
}

simulated function timer()
{
	GotoState('Ticking');
}

simulated function PostNetBeginPlay()
{
	SetTimer(LifeSpan - 2.0,false);
	Super.PostNetBeginPlay();
}

defaultproperties
{
    LifeSpan=5.0
	mStartParticles=0
	mNumTileColumns=4
	mNumTileRows=4
	mLifeRange(0)=0.200000
	mLifeRange(1)=0.100000
	mRegenRange(0)=100.000000
	mRegenRange(1)=100.000000
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mMassRange(0)=-2.000000
	mMassRange(1)=-1.000000
	mSpinRange(0)=-15.000000
	mSpinRange(1)=15.000000
	mSizeRange(0)=5.000000
	mGrowthRate=-10.000000
	mAttenKa=0.500000
	mPosDev=(X=3.000000,Y=3.000000,Z=3.000000)
	mAttenFunc=ATF_None
	Texture=S_Emitter
	Skins(0)=Texture'EmitterTextures.MultiFrame.fire3'
	Style=STY_Translucent
	bDynamicLight=false
    AmbientSound=Sound'GeneralAmbience.firefx9'
    SoundRadius=32
    SoundVolume=190
    SoundPitch=64
}
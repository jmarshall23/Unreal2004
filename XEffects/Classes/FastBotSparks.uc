//=============================================================================
// FastBotSparks.
//=============================================================================
class FastBotSparks extends xEmitter;

#exec TEXTURE IMPORT NAME=BotSpark FILE=TEXTURES\BotSpark.tga GROUP=Skins ALPHA=1 DXT=5

state Ticking
{
	simulated function Tick( float dt )
	{
		if( LifeSpan < 1.0 )
		{
			mRegenRange[0] *= LifeSpan;
			mRegenRange[1] = mRegenRange[0];
		}
	}
}

simulated function timer()
{
	GotoState('Ticking');
}

simulated function PostNetBeginPlay()
{
	SetTimer(LifeSpan - 1.0,false);
}

defaultproperties
{
    LifeSpan=0.2
    Style=STY_Additive
    mParticleType=PT_Line
    mDirDev=(X=0.6,Y=0.6,Z=0.6)
    mPosDev=(X=0.8,Y=0.8,Z=0.8)
    mDelayRange(0)=0.0
    mDelayRange(1)=0.05
    mLifeRange(0)=0.1
    mLifeRange(1)=0.2
    mSpeedRange(0)=150.0
    mSpeedRange(1)=200.0
    mSizeRange(0)=2.5
    mSizeRange(1)=1.5
    mMassRange(0)=1.5
    mMassRange(1)=2.5
    mRegenRange(0)=60.0
    mRegenRange(1)=60.0
    mRegenDist=0.0
    mStartParticles=0
    mMaxParticles=20
    DrawScale=1.0
    ScaleGlow=2.0
    mGrowthRate=-4.0
    mAttenuate=true
    mRegen=true;
    Skins(0)=Texture'BotSpark'
    CollisionRadius=0.0
    CollisionHeight=0.0
    mColorRange(0)=(R=255,G=255,B=255,A=255)
    mColorRange(1)=(R=255,G=255,B=255,A=255)
    bForceAffected=False
    mAttenKa=0.0
    mAirResistance=0.0
    mSpawnVecB=(X=2.0,Y=0.0,Z=0.03)
}
// ============================================================
// HitSmoke: 
// ============================================================

class HitSmoke extends xEmitter;

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
	Super.PostNetBeginPlay();
}

defaultproperties
{
    LifeSpan=10.0;
    Style=STY_Subtractive
    mDirDev=(X=0.3,Y=0.3,Z=0.3)
    mPosDev=(X=3.3,Y=3.3,Z=3.3)
	mSizeRange(0)=15.000000
	mSizeRange(1)=20.000000
	mLifeRange(0)=1.0
	mLifeRange(1)=1.1
	mGrowthRate=25.0
	mStartParticles=0
	mMaxParticles=40
	mColorRange(0)=(R=50,G=50,B=50,A=255)
	mColorRange(1)=(R=100,G=100,B=100,A=255)
	mRegen=true
	mRegenRange(0)=50.0
	mRegenRange(1)=50.0
    mMassRange(0)=-0.1
    mMassRange(1)=-0.2
	bForceAffected=false
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    bHidden=true
    Skins(0)=Texture'EmitSmoke_t'
	mNumTileColumns=4
	mNumTileRows=4
    RemoteRole=ROLE_None
}

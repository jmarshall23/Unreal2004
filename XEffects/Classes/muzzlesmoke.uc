// ============================================================
// MuzzleSmoke: 
// ============================================================

class MuzzleSmoke extends pclLightSmoke;

function PostBeginPlay()
{
    disable('Tick');
}

event Trigger( Actor Other, Pawn EventInstigator )
{
    mRegenRange[0] = 30.0;
    mRegenRange[1] = mRegenRange[0];
    enable('Tick');
}

event Tick(float dt)
{
    if (mRegenRange[0] > 1.0)
    {
        mRegenRange[0] = Lerp(dt, mRegenRange[0], 0.0);
    }
    else
    {
        mRegenRange[0] = 0.0;
        disable('Tick');
    }

    mRegenRange[1] = mRegenRange[0];
}

defaultproperties
{
	bHighDetail=true
    mDirDev=(X=0.300000,Y=0.300000,Z=0.300000)
	bOnlyOwnerSee=True
	mSizeRange(0)=5.000000
	mSizeRange(1)=10.000000
	mLifeRange(0)=0.600000
	mLifeRange(1)=1.200000
	mGrowthRate=4.0
	mStartParticles=0
	mMaxParticles=30
	mColorRange(0)=(R=70,G=70,B=70,A=255)
	mColorRange(1)=(R=113,G=113,B=113,A=255)
	mRegen=True
	mRegenRange(0)=0.00000
	mRegenRange(1)=0.00000 
    mMassRange(0)=-0.25
    mMassRange(1)=-0.15
	bForceAffected=false
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    bHidden=true
}

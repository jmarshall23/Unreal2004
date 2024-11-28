class LinkSmoke extends xEmitter;

#EXEC TEXTURE IMPORT NAME=GreenSmokeTex FILE=textures\Greenexplo.tga DXT=5

defaultproperties 
{
	bHighDetail=true
    Skins(0)=Texture'GreenSmokeTex'
    Style=STY_Translucent
    mSpawningType=ST_Explode
    mDirDev=(X=1.5,Y=1.5,Z=1.5)
    mPosDev=(X=20.0,Y=20.0,Z=20.0)      
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mLifeRange(0)=1.2
    mLifeRange(1)=1.5
    mSpeedRange(0)=10.0
    mSpeedRange(1)=30.0
    mSizeRange(0)=30.0
    mSizeRange(1)=60.0
    mGrowthRate=0.0
    mStartParticles=8
    mMaxParticles=8
    mMassRange(0)=-0.05
    mMassRange(1)=-0.15
    mSpinRange(0)=-30.0
    mSpinRange(1)=30.0
    mRandOrient=true
    mRegen=false
}

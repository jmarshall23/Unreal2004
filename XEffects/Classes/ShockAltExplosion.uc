//=============================================================================
// ShockAltExplosion
//=============================================================================
class ShockAltExplosion extends xEmitter;

#EXEC TEXTURE IMPORT NAME=seexpt FILE=textures\ShockExplosion.tga GROUP=Skins DXT=5

defaultproperties 
{
    Skins(0)=Texture'seEXpt'
    Style=STY_Translucent
    mSpawningType=ST_Explode
    mDirDev=(X=1.0,Y=1.0,Z=1.0)
    mPosDev=(X=30.0,Y=30.0,Z=30.0)      
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mLifeRange(0)=1.2
    mLifeRange(1)=1.5
    mSpeedRange(0)=20.0
    mSpeedRange(1)=50.0
    mSizeRange(0)=60.0
    mSizeRange(1)=90.0
    mGrowthRate=20.0
    mStartParticles=8
    mMaxParticles=8
    mMassRange(0)=-0.1
    mMassRange(1)=-0.2
    mSpinRange(0)=-20.0
    mSpinRange(1)=20.0
    mRandOrient=true
    mRegen=false
}
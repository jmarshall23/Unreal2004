class RocketTrailSmoke extends xEmitter;

#exec OBJ LOAD File=XGameShadersB.utx
#exec  TEXTURE IMPORT NAME=WispSmoke_t FILE=Textures\smoke_wisp.tga LODSET=2 DXT=1

defaultproperties
{
    Physics=PHYS_Trailer
    Skins(0)=Texture'XEffects.SmokeAlphab_t'
    Style=STY_Alpha
    mParticleType=PT_Sprite 
    mLifeRange(0)=1.25
    mLifeRange(1)=1.25
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=-75.0
    mSpinRange(1)=75.0
    mSizeRange(0)=15.0
    mSizeRange(1)=20.0
    mRegenRange(0)=90.0
    mRegenRange(1)=90.0
    mRandOrient=True
    mRandTextures=True
    mStartParticles=0
    mMaxParticles=150
    mGrowthRate=13.0
    mAttenuate=True
    mAttenFunc=ATF_ExpInOut
    mAttenKa=0.2
    mRegen=True
    mNumTileColumns=4
    mNumTileRows=4
    mMassRange(0)=-0.03
    mMassRange(1)=-0.01
    mColorRange(1)=(G=210,B=210)
    bForceAffected=false
    CullDistance=+10000.0
}

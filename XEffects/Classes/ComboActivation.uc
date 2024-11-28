class ComboActivation extends xEmitter;

#exec TEXTURE IMPORT NAME=pcl_ball FILE=..\xEffects\TEXTURES\pcl_ball.tga GROUP=Skins ALPHA=1 DXT=5

#exec  TEXTURE IMPORT NAME=RedMarker_t FILE=Textures\RedMarker.TGA LODSET=3 DXT=1
#exec  TEXTURE IMPORT NAME=BlueMarker_t FILE=Textures\BlueMarker.TGA LODSET=3 DXT=1

defaultproperties
{
    RemoteRole=ROLE_SimulatedProxy
    bNetTemporary=true
    LifeSpan=2.0
    Physics=PHYS_Trailer
    Skins=(Texture'XEffects.Skins.pcl_ball')
    Style=STY_Additive
    mSpawningType=ST_Explode
    mParticleType=PT_Sprite
    mStartParticles=0
    mLifeRange(0)=0.75
    mLifeRange(1)=0.75
    mRegen=true
    mRegenPause=true
    mRegenRange(0)=80.0
    mRegenRange(1)=80.0
    mRegenOnTime(0)=0.25
    mRegenOnTime(1)=0.25
    mRegenOffTime(0)=10.0
    mRegenOffTime(1)=10.0
    mSpeedRange(0)=75.0
    mSpeedRange(1)=75.0
    mAirResistance=-4.0
    mSpinRange(0)=300.0
    mSpinRange(1)=300.0
    mSizeRange(0)=3.0
    mSizeRange(1)=3.0
    mGrowthRate=0.0
    mAttenKa=0.0
    mPosDev=(X=10.0,Y=10.0,Z=10.0)
    mColorRange(0)=(R=255,G=0,B=0)
    mColorRange(1)=(R=255,G=255,B=255)
    mPosRelative=true
}

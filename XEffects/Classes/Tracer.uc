//=============================================================================
// Tracer.
//=============================================================================
class Tracer extends xEmitter;

#exec TEXTURE  IMPORT NAME=TracerT FILE=textures\TracerPcl.tga GROUP="Skins" 

defaultproperties
{
	mRegen=true
    mMaxParticles=100
    mStartParticles=0
    mLifeRange(0)=0.700000
    mLifeRange(1)=1.000000
    mRegenRange(0)=0.000000
    mRegenRange(1)=0.000000
    mSpeedRange(0)=5000.000000
    mSpeedRange(1)=5000.000000
    mSizeRange(0)=4.000000
    mSizeRange(1)=4.000000
    mParticleType=PT_Line
    Skins(0)=Texture'XEffects.Skins.TracerT'
    Style=STY_Translucent
    DrawType=DT_Particle
	mAirResistance=0.0
}


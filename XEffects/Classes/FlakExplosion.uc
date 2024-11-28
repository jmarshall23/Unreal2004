//=============================================================================
// FlakExplosion
//=============================================================================
class FlakExplosion extends RocketExplosion;

#EXEC TEXTURE IMPORT NAME=fexpt FILE=textures\explob.tga GROUP=Skins DXT=5 
#exec AUDIO IMPORT FILE="Sounds\explosion4.WAV" NAME="FlakExplosionSnd" GROUP="Effects"

defaultproperties 
{
	 bHighDetail=true
	 Style=STY_Additive 
     mDirDev=(X=2.00000,Y=2.00000,Z=2.00000)
     mPosDev=(X=55.00000,Y=55.00000,Z=55.00000)   
     mDelayRange(0)=0.00000
     mDelayRange(1)=0.000
     mLifeRange(0)=0.90000
     mLifeRange(1)=1.800000
     mSpeedRange(0)=3.000000
     mSpeedRange(1)=20.000000
     mSizeRange(0)=91.000000
     mSizeRange(1)=161.000000
     mStartParticles=5
     mMassRange(0)=-0.200000
     mMassRange(1)=-0.600000
	 mSpinRange(0)=-3.0
	 mSpinRange(1)=3.0
     Texture=Texture'fEXpt'
	 skins(0)=Texture'fEXpt'
	 bDynamicLight=true
	 LightHue=28
     LightSaturation=100
     LightType=LT_FadeOut
     LightEffect=LE_QuadraticNonIncidence
     LightBrightness=255
     LightRadius=8
}
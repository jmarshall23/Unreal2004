// ============================================================
// pclMuzFlashA
// 
// Machinegun 1st person muzzle flash
// ============================================================
 
class pclMuzFlashA extends xEmitter;

#exec TEXTURE IMPORT NAME=MuzFlashA_t FILE=Textures\MuzzleFlashA.tga GROUP="Skins" LODSET=3 DXT=1

event Trigger( Actor Other, Pawn EventInstigator )
{
    mStartParticles = ClampToMaxParticles(100);
} 
  
defaultproperties
{
    DrawScale=0.75
	mParticleType=PT_Sprite
	Style=STY_Additive
	mDelayRange(0)=0.000000
	mDelayRange(1)=0.00000 
	Skins(0)=Texture'MuzFlashA_t'
	bOnlyOwnerSee=True 
	mRandOrient=True 
	mSizeRange(0)=35.00000  
	mSizeRange(1)=70.00000
	mLifeRange(0)=0.050000
	mLifeRange(1)=0.050000 
	mSpeedRange(0)=0.00000
	mSpeedRange(1)=0.00000 
	mSpinRange(0)=0
	mSpinRange(1)=0
	mRegen=True
    mRegenPause=True
	mRegenRange(0)=0.00000
	mRegenRange(1)=0.00000 
	mStartParticles=0
	mMaxParticles=2
	mColorRange(0)=(R=95,G=95,B=95,A=255)
	mColorRange(1)=(R=95,G=95,B=95,A=255)
	mPosRelative=true
	mAttenuate=False
	bForceAffected=False
    bHidden=true
}
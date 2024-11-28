class LinkMuzFlashBeam1st extends xEmitter;

#exec TEXTURE   IMPORT      NAME=LinkMuzFlashTex FILE=textures\muzzleflashlink.tga GROUP=Effects LODSET=3
#exec OBJ LOAD FILE=XEffectMat.utx

event Trigger( Actor Other, Pawn EventInstigator )
{
    mStartParticles += 1;
} 
  
defaultproperties
{
    Style=STY_Translucent
    Skins(0)=Texture'XEffectMat.link_muz_green'
    bOnlyOwnerSee=True 
    mRandOrient=True 
    mSizeRange(0)=24.0
    mSizeRange(1)=24.0
    mLifeRange(0)=0.25
    mLifeRange(1)=0.25
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mRegen=True
    mRegenRange(0)=0.0
    mRegenRange(1)=0.0
    mSpinRange(0)=-100.0
    mSpinRange(1)=100.0
    mStartParticles=0
    mMaxParticles=5
    mColorRange(0)=(R=200,G=200,B=200,A=255)
    mColorRange(1)=(R=200,G=200,B=200,A=255)
    mPosRelative=true
    mAttenuate=true
    mAttenKa=0.0
    bHidden=true
}
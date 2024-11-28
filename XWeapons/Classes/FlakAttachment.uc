class FlakAttachment extends xWeaponAttachment;

var class<FlakMuzFlash3rd>  mMuzFlashClass;
var xEmitter                mMuzFlash3rd;

simulated function Destroyed()
{
    if (mMuzFlash3rd != None)
        mMuzFlash3rd.Destroy();
	Super.Destroyed();
}

simulated event ThirdPersonEffects()
{
    local rotator r;

    if ( Level.NetMode != NM_DedicatedServer && FlashCount > 0 )
	{
		WeaponLight();
        if (mMuzFlash3rd == None)
        {
            mMuzFlash3rd = Spawn(mMuzFlashClass);
            AttachToBone(mMuzFlash3rd, 'tip');
        }
        if (mMuzFlash3rd != None)
        {
            r.Roll = Rand(65536);
            SetBoneRotation('Bone_Flash', r, 0, 1.f);
            mMuzFlash3rd.mStartParticles++;
        }
    }

    Super.ThirdPersonEffects();
}

defaultproperties
{
    Mesh=mesh'Weapons.Flak_3rd'
    mMuzFlashClass=class'XEffects.FlakMuzFlash3rd'
    bHeavy=true
    bRapidFire=false
    bAltRapidFire=false

    bDynamicLight=false
    LightType=LT_Steady
    LightEffect=LE_NonIncidence
    LightBrightness=255
    LightHue=30
    LightSaturation=150
    LightRadius=4.0
}
 
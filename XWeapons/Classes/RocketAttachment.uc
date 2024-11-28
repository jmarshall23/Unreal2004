class RocketAttachment extends xWeaponAttachment;

var class<xEmitter>     MuzFlashClass;
var xEmitter            MuzFlash;

simulated function Destroyed()
{
    if (MuzFlash != None)
        MuzFlash.Destroy();

    Super.Destroyed();
}

simulated event ThirdPersonEffects()
{
    local rotator r;

    if ( Level.NetMode != NM_DedicatedServer && FlashCount > 0 )
	{
        if (MuzFlash == None)
        {
            MuzFlash = Spawn(MuzFlashClass);
            if ( MuzFlash != None )
				AttachToBone(MuzFlash, 'tip');
        }
        if (MuzFlash != None)
        {
            MuzFlash.mStartParticles++;
            r.Roll = Rand(65536);
            SetBoneRotation('Bone_Flash', r, 0, 1.f);
        }
    }

    Super.ThirdPersonEffects();
}

defaultproperties
{
    MuzFlashClass=class'XEffects.RocketMuzFlash3rd'
    Mesh=mesh'Weapons.RocketLauncher_3rd'

    bHeavy=true
    bRapidFire=false
    bAltRapidFire=false
}

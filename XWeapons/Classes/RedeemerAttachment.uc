class RedeemerAttachment extends xWeaponAttachment;

var LinkMuzFlashProj3rd MuzFlash;

simulated function Destroyed()
{
    if (MuzFlash != None)
        MuzFlash.Destroy();

    Super.Destroyed();
}

simulated event ThirdPersonEffects()
{
    local Rotator R;

    if ( Level.NetMode != NM_DedicatedServer && FlashCount > 0 )
	{
        if ( FiringMode == 0 )
        {
            if (MuzFlash == None)
            {
                //MuzFlash = Spawn(class'LinkMuzFlashProj3rd');
                //AttachToBone(MuzFlash, 'tip');
            }
            if (MuzFlash != None)
            {
                MuzFlash.Trigger(self, None);
                R.Roll = Rand(65536);
                SetBoneRotation('bone flash', R, 0, 1.0);
            }
        }
    }

    Super.ThirdPersonEffects();
}

defaultproperties
{
    bHeavy=true
    bRapidFire=false
    bAltRapidFire=false    
    Mesh=mesh'Weapons.Redeemer_3rd'
    DrawType=DT_Mesh
    DrawScale=0.6
    
    RelativeLocation=(X=-35.0,Y=0.0,Z=0.0)
    RelativeRotation=(Pitch=0,Yaw=49152,Roll=32768)    
	CullDistance=5000.0
}

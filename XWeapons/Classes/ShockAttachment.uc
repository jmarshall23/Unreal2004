class ShockAttachment extends xWeaponAttachment;

var class<xEmitter>     MuzFlashClass;
var xEmitter            MuzFlash;

simulated function PostNetBeginPlay()
{
	Super.PostNetBeginPlay();
	if ( (Instigator != None) && (Instigator.PlayerReplicationInfo != None)&& (Instigator.PlayerReplicationInfo.Team != None) )
	{
		if ( Instigator.PlayerReplicationInfo.Team.TeamIndex == 0 )
			Skins[1] = Material'UT2004Weapons.RedShockFinal';
		else if ( Instigator.PlayerReplicationInfo.Team.TeamIndex == 1 )
			Skins[1] = Material'UT2004Weapons.BlueShockFinal';
	}
}
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
		if ( FiringMode == 0 )
			WeaponLight();
        else
        {
            if (MuzFlash == None)
            {
                MuzFlash = Spawn(MuzFlashClass);
                AttachToBone(MuzFlash, 'tip');
            }
            if (MuzFlash != None)
            {
                MuzFlash.mStartParticles++;
                r.Roll = Rand(65536);
                SetBoneRotation('Bone_Flash', r, 0, 1.f);
            }
        }
    }

    Super.ThirdPersonEffects();
}

defaultproperties
{
    MuzFlashClass=class'XEffects.ShockProjMuzFlash3rd'
    bHeavy=false
    bRapidFire=false
    bAltRapidFire=false
    Mesh=mesh'NewWeapons2004.NewShockRifle_3rd'
    Skins[0]=UT2004Weapons.ShockRifleTex0
    Skins[1]=UT2004Weapons.PurpleShockFinal
    
    bDynamicLight=false
    LightType=LT_Steady
    LightEffect=LE_NonIncidence
    LightPeriod=3
    LightBrightness=255
    LightHue=200
    LightSaturation=70
    LightRadius=4.0
    
    RelativeLocation=(X=-3.0,Y=-5.0,Z=-10.0)
    RelativeRotation=(Pitch=32768)
}

//=============================================================================
// Weapon_Sentinel
//=============================================================================

class Weapon_Sentinel extends Weapon
    config(user)
    HideDropDown
	CacheExempt;

simulated function bool HasAmmo()
{
    return true;
}

function byte BestMode()
{
	return 0;
}

//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
    ItemName="Sentinel weapon"

    FireModeClass(0)=FM_Sentinel_Fire
    FireModeClass(1)=FM_Sentinel_Fire

	bCanThrow=false
	bNoInstagibReplace=true

	PickupClass=None
    AttachmentClass=class'WA_Sentinel'

	Priority=1
    InventoryGroup=1

    DrawScale=3.0
	DrawType=DT_None
	Mesh=None
    PlayerViewOffset=(X=0,Y=0,Z=-40)
    SmallViewOffset=(X=0,Y=0,Z=-40)
	CenteredRoll=0
    DisplayFOV=90
	AmbientGlow=64

	EffectOffset=(X=0,Y=0,Z=0)

	AIRating=+0.68
	CurrentRating=+0.68
}

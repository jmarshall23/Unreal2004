//=============================================================================
// Weapon_Turret
//=============================================================================

class Weapon_Turret_Minigun extends Weapon
    config(user)
    HideDropDown
	CacheExempt;

#exec OBJ LOAD FILE=..\Animations\AS_Vehicles_M.ukx

simulated function ClientStartFire(int mode)
{
	local PlayerController PC;
    if ( mode == 1 )
    {
		PC = PlayerController(Instigator.Controller);
		if ( PC.DesiredFOV == PC.DefaultFOV )
			PC.DesiredFOV = ASTurret(Instigator).MinPlayerFOV;
		else
			PC.DesiredFOV = PC.DefaultFOV;
		PlayerController(Instigator.Controller).bAltFire = 0;
    }
    else
        super.ClientStartFire( mode );
}


simulated function PawnUnpossessed()
{
	if ( (Instigator != None) && (PlayerController(Instigator.Controller) != None) )
		PlayerController(Instigator.Controller).DesiredFOV = PlayerController(Instigator.Controller).DefaultFOV;
}

/* BestMode() choose between regular or alt-fire */
function byte BestMode()
{
	return 0;
}

simulated function bool HasAmmo()
{
    return true;
}

//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
	bCanThrow=false
	bNoInstagibReplace=true
    ItemName="Turret weapon"
	DisplayFOV=60.0

	PickupClass=None
    AttachmentClass=class'WA_Turret_Minigun'

    FireModeClass(0)=FM_Turret_Minigun_Fire
    FireModeClass(1)=FM_Turret_Minigun_AltFire

	Priority=1
    InventoryGroup=1

    DrawScale=3.0
	DrawType=DT_None
    PlayerViewOffset=(X=0,Y=0,Z=-40)
    SmallViewOffset=(X=0,Y=0,Z=-40)
	CenteredRoll=0

	EffectOffset=(X=0,Y=0,Z=0)

	AIRating=+0.68
	CurrentRating=+0.68
}

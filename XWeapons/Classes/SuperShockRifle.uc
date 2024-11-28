//=============================================================================
// SuperShockRifle
//=============================================================================
class SuperShockRifle extends ShockRifle
	HideDropDown
	CacheExempt;

#exec OBJ LOAD FILE=..\Sounds\WeaponSounds.uax
#exec OBJ LOAD FILE=XEffectMat.utx

simulated event RenderOverlays( Canvas Canvas )
{
    if ( (Instigator.PlayerReplicationInfo.Team != None) && (Instigator.PlayerReplicationInfo.Team.TeamIndex == 1) )
		ConstantColor'UT2004Weapons.ShockControl'.Color = class'HUD'.Default.BlueColor;
	else
		ConstantColor'UT2004Weapons.ShockControl'.Color = class'HUD'.Default.RedColor;
	Super.RenderOverlays(Canvas);
}

simulated function bool ConsumeAmmo(int Mode, float load, optional bool bAmountNeededIsMax)
{
    return true;
}

simulated function CheckOutOfAmmo()
{
}

function float GetAIRating()
{
	return AIRating;
}

simulated function bool StartFire(int mode)
{
	bWaitForCombo = false;
	return Super.StartFire(mode);
}

function float RangedAttackTime()
{
	return 0;
}

/* BestMode()
choose between regular or alt-fire
*/
function byte BestMode()
{
	return 0;
}

defaultproperties
{
    bCanThrow=false
	AIRating=+1.0
    bNetNotify=false

    FireModeClass(0)=SuperShockBeamFire
    FireModeClass(1)=SuperShockBeamFire
    InventoryGroup=4
    ItemName="Super Shock Rifle"
    PickupClass=class'SuperShockRiflePickup'
    HudColor=(r=128,g=0,b=255,a=255)
	CustomCrosshair=1
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Cross2"
	CustomCrosshairColor=(r=255,g=0,b=255,a=255)
}

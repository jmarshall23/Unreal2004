class Redeemer extends Weapon
    config(user);

function PrebeginPlay()
{
	Super.PreBeginPlay();
}

simulated function SuperMaxOutAmmo()
{}

simulated event ClientStopFire(int Mode)
{
    if (Role < ROLE_Authority)
    {
        StopFire(Mode);
    }
    if ( Mode == 0 )
		ServerStopFire(Mode);
}

simulated event WeaponTick(float dt)
{
	if ( (Instigator.Controller == None) || HasAmmo() )
		return;
	Instigator.Controller.SwitchToBestWeapon();
}


// AI Interface
function float SuggestAttackStyle()
{
    return -1.0;
}

function float SuggestDefenseStyle()
{
    return -1.0;
}

/* BestMode()
choose between regular or alt-fire
*/
function byte BestMode()
{
	return 0;
}

function float GetAIRating()
{
	local Bot B;

	B = Bot(Instigator.Controller);
	if ( B == None )
		return 0.4;

	if ( B.IsShootingObjective() )
		return 1.0;

	if ( (B.Enemy == None) || B.Enemy.bCanFly || VSize(B.Enemy.Location - Instigator.Location) < 2400 )
		return 0.4;

	return AIRating;
}

defaultproperties
{
    FireModeClass(0)=RedeemerFire
    FireModeClass(1)=RedeemerGuidedFire
    InventoryGroup=0
    GroupOffset=1
    Mesh=mesh'Weapons.Redeemer_1st'
    ItemName="Redeemer"
    Description="The first time you witness this miniature nuclear device in action, you'll agree it is the most powerful weapon in the Tournament.|Launch a slow-moving but utterly devastating missile with the primary fire; but make sure you're out of the Redeemer's impressive blast radius before it impacts. The secondary fire allows you to guide the nuke yourself with a rocket's-eye view.||Keep in mind, however, that you are vulnerable to attack when steering the Redeemer's projectile. Due to the extreme bulkiness of its ammo, the Redeemer is exhausted after a single shot."
    BobDamping=1.4
    PickupClass=class'RedeemerPickup'
    EffectOffset=(X=0.0,Y=0.0,Z=-0.0)
    AttachmentClass=class'RedeemerAttachment'
    PutDownAnim=PutDown

    DisplayFOV=60
    DrawScale=1.2
    PlayerViewOffset=(X=14,Y=0,Z=-28)
    SmallViewOffset=(X=26,Y=6,Z=-34)
    PlayerViewPivot=(Pitch=1000,Roll=0,Yaw=-400)
    SelectSound=Sound'WeaponSounds.Redeemer_change'
	IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=4,Y1=350,X2=110,Y2=395)
	SelectForce="SwitchToFlakCannon"

	AIRating=1.5
	CurrentRating=1.5
	SelectAnim=Pickup

	bNotInDemo=true
	DemoReplacement=class'xWeapons.RocketLauncher'

	Priority=16
	CustomCrosshair=13
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Circle2"
	CustomCrosshairColor=(r=255,g=255,b=128,a=255)
	CustomCrosshairScale=+2.0

    SelectAnimRate=0.667
    PutDownAnimRate=1.0
	BringUpTime=+0.675
	PutDownTime=+0.45
}

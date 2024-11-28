//=============================================================================
// Flak Cannon
//=============================================================================
class FlakCannon extends Weapon
    config(user);

#EXEC OBJ LOAD FILE=InterfaceContent.utx

// AI Interface
function float GetAIRating()
{
	local Bot B;
	local float EnemyDist;
	local vector EnemyDir;

	B = Bot(Instigator.Controller);
	if ( B == None )
		return AIRating;
		
	if ( (B.Target != None) && (Pawn(B.Target) == None) && (VSize(B.Target.Location - Instigator.Location) < 1250) )
		return 0.9;
		
	if ( B.Enemy == None )
		return AIRating;

	EnemyDir = B.Enemy.Location - Instigator.Location;
	EnemyDist = VSize(EnemyDir);
	if ( EnemyDist > 750 )
	{
		if ( EnemyDist > 2000 )
		{
			if ( EnemyDist > 3500 )
				return 0.2;
			return (AIRating - 0.3);
		}
		if ( EnemyDir.Z < -0.5 * EnemyDist )
			return (AIRating - 0.3);
	}
	else if ( (B.Enemy.Weapon != None) && B.Enemy.Weapon.bMeleeWeapon )
		return (AIRating + 0.35);
	else if ( EnemyDist < 400 )
		return (AIRating + 0.2);
	return FMax(AIRating + 0.2 - (EnemyDist - 400) * 0.0008, 0.2);
}

/* BestMode()
choose between regular or alt-fire
*/
function byte BestMode()
{
	local vector EnemyDir;
	local float EnemyDist;
	local bot B;

	B = Bot(Instigator.Controller);
	if ( (B == None) || (B.Enemy == None) )
		return 0;

	EnemyDir = B.Enemy.Location - Instigator.Location;
	EnemyDist = VSize(EnemyDir);
	if ( EnemyDist > 750 )
	{
		if ( EnemyDir.Z < -0.5 * EnemyDist )
			return 1;
		return 0;
	}
	else if ( (B.Enemy.Weapon != None) && B.Enemy.Weapon.bMeleeWeapon )
		return 0;
	else if ( (EnemyDist < 400) || (EnemyDir.Z > 30) )
		return 0;
	else if ( FRand() < 0.65 )
		return 1;
	return 0;
}

function float SuggestAttackStyle()
{
	if ( (AIController(Instigator.Controller) != None)
		&& (AIController(Instigator.Controller).Skill < 3) )
		return 0.4;
    return 0.8;
}

function float SuggestDefenseStyle()
{
    return -0.4;
}
// End AI Interface

defaultproperties
{
	HighDetailOverlay=Material'UT2004Weapons.WeaponSpecMap2'
    ItemName="Flak Cannon"
    Description="Trident Defensive Technologies Series 7 Flechette Cannon has been taken to the next step in evolution with the production of the Mk3 \"Negotiator\". The ionized flechettes are capable of delivering second and third-degree burns to organic tissue, cauterizing the wound instantly.||Payload delivery is achieved via one of two methods: ionized flechettes launched in a spread pattern directly from the barrel; or via fragmentation grenades that explode on impact, radiating flechettes in all directions."
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=169,Y1=172,X2=245,Y2=208)

    FireModeClass(0)=FlakFire
    FireModeClass(1)=FlakAltFire
    InventoryGroup=7
    Mesh=mesh'Weapons.Flak_1st'
    BobDamping=1.4
    PickupClass=class'FlakCannonPickup'
    EffectOffset=(X=200.0,Y=32.0,Z=-25.0)
    AttachmentClass=class'FlakAttachment'
    PutDownAnim=PutDown

    DisplayFOV=60
    DrawScale=1.0
    PlayerViewOffset=(X=-7,Y=8,Z=0)
    SmallViewOffset=(X=5,Y=14,Z=-6)
    PlayerViewPivot=(Pitch=0,Roll=200,Yaw=16884)
    SelectSound=Sound'WeaponSounds.FlakCannon.SwitchToFlakCannon'
	SelectAnim=Pickup
	SelectForce="SwitchToFlakCannon"

	AIRating=+0.75
	CurrentRating=+0.75

    bDynamicLight=false
    LightType=LT_Steady
    LightEffect=LE_NonIncidence
    LightBrightness=255
    LightHue=30
    LightSaturation=150
    LightRadius=4.0

    HudColor=(r=255,g=128,b=0,a=255)
	Priority=13
	CustomCrosshair=9
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Triad3"
	CustomCrosshairColor=(r=255,g=128,b=0,a=255)

	CenteredOffsetY=-4.0
	CenteredYaw=-500
	CenteredRoll=3000
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSGrenadeLauncher extends Weapon
	config(User);

#exec OBJ LOAD FILE=HudContent.utx

var array<ONSGrenadeProjectile> Grenades;
var int CurrentGrenades; //should be sync'ed with Grenades.length
var int MaxGrenades;
var color FadedColor;

replication
{
	reliable if (bNetOwner && bNetDirty && ROLE == ROLE_Authority)
		CurrentGrenades;
}

simulated function DrawWeaponInfo(Canvas Canvas)
{
	NewDrawWeaponInfo(Canvas, 0.705*Canvas.ClipY);
}

simulated function NewDrawWeaponInfo(Canvas Canvas, float YPos)
{
	local int i, Half;
	local float ScaleFactor;

	ScaleFactor = 99 * Canvas.ClipX/3200;
	Half = (MaxGrenades + 1) / 2;
	Canvas.Style = ERenderStyle.STY_Alpha;
	Canvas.DrawColor = class'HUD'.Default.WhiteColor;
	for (i = 0; i < Half; i++)
	{
		if (i >= CurrentGrenades)
			Canvas.DrawColor = FadedColor;
		Canvas.SetPos(Canvas.ClipX - (i+1) * ScaleFactor * 1.25, YPos);
		Canvas.DrawTile(Material'HudContent.Generic.HUD', ScaleFactor, ScaleFactor, 324, 325, 54, 54);
	}
	for (i = Half; i < MaxGrenades; i++)
	{
		if (i >= CurrentGrenades)
			Canvas.DrawColor = FadedColor;
		Canvas.SetPos(Canvas.ClipX - (i-Half+1) * ScaleFactor * 1.25, YPos - ScaleFactor);
		Canvas.DrawTile(Material'HudContent.Generic.HUD', ScaleFactor, ScaleFactor, 324, 325, 54, 54);
	}
}

simulated function bool HasAmmo()
{
    if (CurrentGrenades > 0)
    	return true;

    return Super.HasAmmo();
}

simulated function bool CanThrow()
{
	if ( AmmoAmount(0) <= 0 )
		return false;

	return Super.CanThrow();
}

simulated function OutOfAmmo()
{
}

simulated singular function ClientStopFire(int Mode)
{
	if (Mode == 1 && !HasAmmo())
		DoAutoSwitch();

	Super.ClientStopFire(Mode);
}

simulated function Destroyed()
{
	local int x;

	if (Role == ROLE_Authority)
	{
		for (x = 0; x < Grenades.Length; x++)
			if (Grenades[x] != None)
				Grenades[x].Explode(Grenades[x].Location, vect(0,0,1));
		Grenades.Length = 0;
	}

	Super.Destroyed();
}

// AI Interface
function float GetAIRating()
{
	local Bot B;
	local float EnemyDist;
	local vector EnemyDir;

	B = Bot(Instigator.Controller);
	if ( (B == None) || (B.Enemy == None) )
		return AIRating;

	// if retreating, favor this weapon
	EnemyDir = B.Enemy.Location - Instigator.Location;
	EnemyDist = VSize(EnemyDir);
	if ( EnemyDist > 1500 )
		return 0.1;
	if ( B.IsRetreating() )
		return (AIRating + 0.4);
	if ( -1 * EnemyDir.Z > EnemyDist )
		return AIRating + 0.1;
	if ( (B.Enemy.Weapon != None) && B.Enemy.Weapon.bMeleeWeapon )
		return (AIRating + 0.3);
	if ( EnemyDist > 1000 )
		return 0.35;
	return AIRating;
}

/* BestMode()
choose between regular or alt-fire
*/
function byte BestMode()
{
	local int x;

	if (CurrentGrenades >= MaxGrenades || (AmmoAmount(0) <= 0 && FireMode[0].NextFireTime <= Level.TimeSeconds))
		return 1;

	for (x = 0; x < Grenades.length; x++)
		if (Grenades[x] != None && Pawn(Grenades[x].Base) != None)
			return 1;

	return 0;
}

function float SuggestAttackStyle()
{
	local Bot B;
	local float EnemyDist;

	B = Bot(Instigator.Controller);
	if ( (B == None) || (B.Enemy == None) )
		return 0.4;

	EnemyDist = VSize(B.Enemy.Location - Instigator.Location);
	if ( EnemyDist > 1500 )
		return 1.0;
	if ( EnemyDist > 1000 )
		return 0.4;
	return -0.4;
}

function float SuggestDefenseStyle()
{
	local Bot B;

	B = Bot(Instigator.Controller);
	if ( (B == None) || (B.Enemy == None) )
		return 0;

	if ( VSize(B.Enemy.Location - Instigator.Location) < 1600 )
		return -0.6;
	return 0;
}

// End AI Interface

simulated function AnimEnd(int Channel)
{
    local name anim;
    local float frame, rate;
    GetAnimParams(0, anim, frame, rate);

    if (anim == 'AltFire')
        LoopAnim('Hold', 1.0, 0.1);
    else
        Super.AnimEnd(Channel);
}

defaultproperties
{
    ItemName="Grenade Launcher"
    Description="The MGG Grenade Launcher fires magnetic sticky grenades, which will attach to enemy players and vehicles."
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=434,Y1=253,X2=506,Y2=292)

    FireModeClass(0)=ONSGrenadeFire
    FireModeClass(1)=ONSGrenadeAltFire
    InventoryGroup=7
	GroupOffset=1

    Mesh=Mesh'ONSWeapons-A.GrenadeLauncher_1st'

    BobDamping=2.2
    PickupClass=class'ONSGrenadePickup'
    EffectOffset=(X=100.0,Y=32.0,Z=-20.0)
    AttachmentClass=class'ONSGrenadeAttachment'
    PutDownAnim=PutDown
    PutDownAnimRate=2.8
    SelectAnimRate=3.1

    DisplayFOV=45
    DrawScale=1.0
	AmbientGlow=64.0
    PlayerViewOffset=(X=150,Y=40,Z=-46)
    SmallViewOffset=(X=166,Y=48,Z=-54)
    PlayerViewPivot=(Pitch=0,Roll=0,Yaw=0)
    SelectSound=Sound'WeaponSounds.FlakCannon.SwitchToFlakCannon'
	SelectForce="SwitchToFlakCannon"

	AIRating=+0.55
	CurrentRating=+0.55
    HudColor=(r=0,g=0,b=255,a=255)
	Priority=10
	CustomCrosshair=15
	CustomCrosshairTextureName="ONSInterface-TX.grenadeLauncherReticle"
	CustomCrosshairColor=(r=255,g=255,b=255,a=255)

	MaxGrenades=8
	FadedColor=(R=128,G=128,B=128,A=128)
}

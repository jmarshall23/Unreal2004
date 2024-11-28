class Painter extends Weapon
    config(user);

#exec OBJ LOAD FILE=XGameShaders.utx

var(Gfx) float borderX;
var(Gfx) float borderY;

var(Gfx) float focusX;
var(Gfx) float focusY;
var(Gfx) float innerArrowsX;
var(Gfx) float innerArrowsY;
var(Gfx) Color ArrowColor;
var(Gfx) Color FocusColor;
var(Gfx) Color ChargeColor;

var(Gfx) vector RechargeOrigin;
var(Gfx) vector RechargeSize;

var transient float LastFOV;
var bool zoomed;

var Vector EndEffect;
var vector MarkLocation;
var IonCannon FirstCannon;

function bool ConsumeAmmo(int Mode, float load, optional bool bAmountNeededIsMax)
{
	return true;
}

function ReallyConsumeAmmo(int Mode, float load)
{
	Super.ConsumeAmmo(Mode,load);
}

simulated function ClientWeaponThrown()
{
    if( (Instigator != None) && (PlayerController(Instigator.Controller) != None) )
        PlayerController(Instigator.Controller).EndZoom();
    Super.ClientWeaponThrown();
}

// compensate for bright fog
simulated function SetZoomBlendColor(Canvas c)
{
    local Byte    val;
    local Color   clr;
    local Color   fog;

    clr.R = 255;
    clr.G = 255;
    clr.B = 255;
    clr.A = 255;

    if( Instigator.Region.Zone.bDistanceFog )
    {
        fog = Instigator.Region.Zone.DistanceFogColor;
        val = 0;
        val = Max( val, fog.R);
        val = Max( val, fog.G);
        val = Max( val, fog.B);

        if( val > 128 )
        {
            val -= 128;
            clr.R -= val;
            clr.G -= val;
            clr.B -= val;
        }
    }
    c.DrawColor = clr;
}

simulated event RenderOverlays( Canvas Canvas )
{
	local float tileScaleX;
	local float tileScaleY;
	local float bX;
	local float bY;
	local float fX;
	local float fY;
	local float ChargeBar;

	local float barOrgX;
	local float barOrgY;
	local float barSizeX;
	local float barSizeY;

    local PainterFire PainterFire;

	CheckOutOfAmmo();

    //    FireSound=Sound'WeaponSounds.LightningGun.LightningScope'
    if ( LastFOV > PlayerController(Instigator.Controller).DesiredFOV )
    {
        PlaySound(Sound'WeaponSounds.LightningGun.LightningZoomIn', SLOT_Misc,,,,,false);
    }
    else if ( LastFOV < PlayerController(Instigator.Controller).DesiredFOV )
    {
        PlaySound(Sound'WeaponSounds.LightningGun.LightningZoomOut', SLOT_Misc,,,,,false);
    }
    LastFOV = PlayerController(Instigator.Controller).DesiredFOV;

    if ( PlayerController(Instigator.Controller).DesiredFOV == PlayerController(Instigator.Controller).DefaultFOV )
	{
        Super.RenderOverlays(Canvas);
		zoomed=false;
	}
	else
    {
		PainterFire = PainterFire(FireMode[0]);
        if (PainterFire.bIsFiring && PainterFire.bValidMark && Level.TimeSeconds - PainterFire.MarkTime > 0.4)
        {
            ChargeBar = FMin(1.0, ((Level.TimeSeconds - PainterFire.MarkTime) / PainterFire.PaintDuration));
            if (ChargeBar >= 1.0) ChargeBar = 0.0;
        }
        else
            ChargeBar = 0.0;

		tileScaleX = Canvas.SizeX / 640.0f;
		tileScaleY = Canvas.SizeY / 480.0f;

		bX = borderX * tileScaleX;
		bY = borderY * tileScaleY;
		fX = focusX * tileScaleX;
		fY = focusY * tileScaleX;

		barOrgX = RechargeOrigin.X * tileScaleX;
		barOrgY = RechargeOrigin.Y * tileScaleY;

		barSizeX = RechargeSize.X * tileScaleX;
		barSizeY = RechargeSize.Y * tileScaleY;

        SetZoomBlendColor(Canvas);

        Canvas.Style = 255;
		Canvas.SetPos(0,0);
        Canvas.DrawTile( Material'ZoomFB', Canvas.SizeX, Canvas.SizeY, 0.0, 0.0, 512, 512 ); // !! hardcoded size

		// draw border corners
        Canvas.Style = ERenderStyle.STY_Alpha;
		Canvas.SetPos(0,0);
		Canvas.DrawTile( Texture'SniperBorder', bX, bY, 0.0, 0.0, Texture'SniperBorder'.USize, Texture'SniperBorder'.VSize );

		Canvas.SetPos(Canvas.SizeX-bX,0);
		Canvas.DrawTile( Texture'SniperBorder', bX, bY, 0.0, 0.0, -Texture'SniperBorder'.USize, Texture'SniperBorder'.VSize );

		Canvas.SetPos(Canvas.SizeX-bX,Canvas.SizeY-bY);
		Canvas.DrawTile( Texture'SniperBorder', bX, bY, 0.0, 0.0, -Texture'SniperBorder'.USize, -Texture'SniperBorder'.VSize );

		Canvas.SetPos(0,Canvas.SizeY-bY);
		Canvas.DrawTile( Texture'SniperBorder', bX, bY, 0.0, 0.0, Texture'SniperBorder'.USize, -Texture'SniperBorder'.VSize );


		Canvas.DrawColor = FocusColor;
        Canvas.DrawColor.A = 255; // 255 was the original -asp. WTF??!?!?!
		Canvas.Style = ERenderStyle.STY_Alpha;

		Canvas.SetPos((Canvas.SizeX*0.5)-fX,(Canvas.SizeY*0.5)-fY);
		Canvas.DrawTile( Texture'SniperFocus', fX*2.0, fY*2.0, 0.0, 0.0, Texture'SniperFocus'.USize, Texture'SniperFocus'.VSize );

        fX = innerArrowsX * tileScaleX;
		fY = innerArrowsY * tileScaleY;

        Canvas.DrawColor = ArrowColor;
        Canvas.SetPos((Canvas.SizeX*0.5)-fX,(Canvas.SizeY*0.5)-fY);
		Canvas.DrawTile( Texture'SniperArrows', fX*2.0, fY*2.0, 0.0, 0.0, Texture'SniperArrows'.USize, Texture'SniperArrows'.VSize );


		// Draw the Charging meter  -AsP
		Canvas.DrawColor = ChargeColor;
        Canvas.DrawColor.A = 255;

		if(ChargeBar <1)
		    Canvas.DrawColor.R = 255*ChargeBar;
		else
        {
            Canvas.DrawColor.R = 0;
		    Canvas.DrawColor.B = 0;
        }

		if(ChargeBar == 1)
		    Canvas.DrawColor.G = 255;
		else
		    Canvas.DrawColor.G = 0;

		Canvas.Style = ERenderStyle.STY_Alpha;
		Canvas.SetPos( barOrgX, barOrgY );
		Canvas.DrawTile(Texture'Engine.WhiteTexture',barSizeX,barSizeY*ChargeBar, 0.0, 0.0,Texture'Engine.WhiteTexture'.USize,Texture'Engine.WhiteTexture'.VSize*ChargeBar);
		zoomed = true;
	}
}

simulated function ClientStartFire(int mode)
{
    if (mode == 1)
    {
        FireMode[mode].bIsFiring = true;
        if( Instigator.Controller.IsA( 'PlayerController' ) )
            PlayerController(Instigator.Controller).ToggleZoom();
    }
    else
    {
        Super.ClientStartFire(mode);
    }
}

simulated function ClientStopFire(int mode)
{
    if (mode == 1)
    {
        FireMode[mode].bIsFiring = false;
        if( Instigator.Controller.IsA( 'PlayerController' ) )
            PlayerController(Instigator.Controller).StopZoom();
    }
    else
    {
        Super.ClientStopFire(mode);
    }
}

simulated function BringUp(optional Weapon PrevWeapon)
{
    if( Instigator.Controller.IsA( 'PlayerController' ) )
        LastFOV = PlayerController(Instigator.Controller).DesiredFOV;
    Super.BringUp(PrevWeapon);
}

simulated function bool PutDown()
{
    if( Instigator.Controller.IsA( 'PlayerController' ) )
        PlayerController(Instigator.Controller).EndZoom();
    return Super.PutDown();
}


simulated function bool HasAmmo()
{
    return (FireMode[0] != None && AmmoAmount(0) >= 1);
}

function IonCannon CheckMark(vector MarkLocation, bool bFire)
{
	if ( FirstCannon != None )
		return FirstCannon;
	foreach DynamicActors(class'IonCannon', FirstCannon)
		return FirstCannon;
	FirstCannon = spawn(class'IonCannon',,,Location+vect(0,0,2000));
	return FirstCannon;
}

// AI Interface
function float GetAIRating()
{
	local Bot B;
	local vector HitLocation, HitNormal;

	B = Bot(Instigator.Controller);
	if ( B == None )
		return AIRating;
	if ( B.IsShootingObjective() && Trace(HitLocation, HitNormal, B.Target.Location - vect(0,0,2000), B.Target.Location, false) != None )
	{
		MarkLocation = HitLocation;
		if ( CheckMark(MarkLocation, false) != None )
			return AIRating;
	}
	if ( (B.Enemy == None) || (Instigator.Location.Z < B.Enemy.Location.Z) || !B.EnemyVisible() )
		return 0;
	MarkLocation = B.Enemy.Location - B.Enemy.CollisionHeight * vect(0,0,2);
	if ( CheckMark(MarkLocation, false) != None )
		return 2.0;
	if ( TerrainInfo(B.Enemy.Base) == None )
		return 0;
	return 0.1;
}

/* BestMode()
choose between regular or alt-fire
*/
function byte BestMode()
{
	return 0;
}

function float SuggestAttackStyle()
{
    return 0;
}

function float SuggestDefenseStyle()
{
    return 2;
}

function float RangedAttackTime()
{
	return 6;
}

function bool RecommendRangedAttack()
{
	return true;
}

function bool RecommendLongRangedAttack()
{
	return true;
}

// End AI Interface


defaultproperties
{
	HighDetailOverlay=Material'UT2004Weapons.WeaponSpecMap2'
    FireModeClass(0)=PainterFire
    FireModeClass(1)=PainterZoom
    InventoryGroup=0
    DrawScale=1.0
    Mesh=mesh'Weapons.Painter_1st'
    ItemName="Ion Painter"
    Description="The Ion Painter seems innocuous enough at first glance, emitting a harmless low-power laser beam when the primary firing mode is engaged. Several seconds later a multi-gigawatt orbital ion cannon fires on the target, neutralizing any combatants in the vicinity.||The Ion Painter is a remote targeting device used to orient and fire the VAPOR Ion Cannon. The Ion Painter offers increased targeting accuracy via its telescopic sight, easily activated by the secondary fire mode of the weapon.|Once the Ion Painter has been used to designate a target it is highly recommended that the user put considerable distance between themselves and the weapon's area of effect."
    BobDamping=1.575000
    PickupClass=class'PainterPickup'
    EffectOffset=(X=100,Y=25,Z=-3)
    PutDownAnim=PutDown
    DisplayFOV=60
    PlayerViewOffset=(X=25,Y=2,Z=-1)
    SmallViewOffset=(X=37,Y=8,Z=-7)
    PlayerViewPivot=(Pitch=0,Roll=0,Yaw=0)
    AttachmentClass=class'PainterAttachment'
    SelectSound=Sound'WeaponSounds.LinkGun.SwitchToLinkGun'
	IconMaterial=Material'HudContent.Generic.HUD'
	IconCoords=(X1=0,Y1=407,X2=118,Y2=442)
	SelectForce="SwitchToLinkGun"

    AIRating=1.0
    CurrentRating=1.0

    borderX=60.0
    borderY=60.0
    focusX=135
    focusY=105
    innerArrowsX=42.0
    innerArrowsY=42.0
    ChargeColor=(R=255,G=255,B=255,A=255)
    FocusColor=(R=71,G=90,B=126,A=215)
    ArrowColor=(R=200,G=200,B=200,A=255)
    RechargeOrigin=(X=600,Y=330,Z=0)
	RechargeSize=(X=10,Y=-180,Z=0)

	bNotInDemo=true
	DemoReplacement=class'xWeapons.SniperRifle'

	Priority=15

	CenteredOffsetY=-22.0
	CenteredRoll=1000

	CustomCrosshair=13
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Circle2"
	CustomCrosshairColor=(r=255,g=255,b=128,a=255)
	CustomCrosshairScale=+2.0
}

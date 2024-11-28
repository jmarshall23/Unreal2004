//=============================================================================
// Sniper Rifle
//=============================================================================
class SniperRifle extends Weapon
    config(user);

#EXEC OBJ LOAD FILE=InterfaceContent.utx
#EXEC OBJ LOAD FILE=HudContent.utx
#exec OBJ LOAD FILE=XGameShaders.utx

var(Gfx) float testX;
var(Gfx) float testY;

var(Gfx) float borderX;
var(Gfx) float borderY;

var(Gfx) float focusX;
var(Gfx) float focusY;
var(Gfx) float innerArrowsX;
var(Gfx) float innerArrowsY;

var(Gfx) Color ArrowColor;
var(Gfx) Color TargetColor;
var(Gfx) Color NoTargetColor;
var(Gfx) Color FocusColor;
var(Gfx) Color ChargeColor;

var(Gfx) vector RechargeOrigin;
var(Gfx) vector RechargeSize;

var transient float LastFOV;
var() bool zoomed;
var() xEmitter  chargeEmitter;

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();
}

simulated function Destroyed()
{
    if (chargeEmitter != None)
        chargeEmitter.Destroy();

    Super.Destroyed();
}

simulated function ClientWeaponThrown()
{
    if( (Instigator != None) && (PlayerController(Instigator.Controller) != None) )
        PlayerController(Instigator.Controller).EndZoom();
    Super.ClientWeaponThrown();
}

simulated function IncrementFlashCount(int Mode)
{
	if ( Mode == 1 )
		return;
	Super.IncrementFlashCount(Mode);
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

	local float tX;
	local float tY;

	local float barOrgX;
	local float barOrgY;
	local float barSizeX;
	local float barSizeY;

	if ( PlayerController(Instigator.Controller) == None )
	{
        Super.RenderOverlays(Canvas);
		zoomed=false;
		return;
	}

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
		if ( FireMode[0].NextFireTime <= Level.TimeSeconds )
		{
			ChargeBar = 1.0;
		}
		else
		{
			ChargeBar = 1.0 - ((FireMode[0].NextFireTime-Level.TimeSeconds) / FireMode[0].FireRate);
		}

		tileScaleX = Canvas.SizeX / 640.0f;
		tileScaleY = Canvas.SizeY / 480.0f;

		bX = borderX * tileScaleX;
		bY = borderY * tileScaleY;
		fX = 2*focusX * tileScaleX;
		fY = 2*focusY * tileScaleX;

		tX = testX * tileScaleX;
		tY = testY * tileScaleX;

		barOrgX = RechargeOrigin.X * tileScaleX;
		barOrgY = RechargeOrigin.Y * tileScaleY;

		barSizeX = RechargeSize.X * tileScaleX;
		barSizeY = RechargeSize.Y * tileScaleY;

        SetZoomBlendColor(Canvas);

        Canvas.Style = 255;
		Canvas.SetPos(0,0);
        Canvas.DrawTile( Material'ZoomFB', Canvas.SizeX, Canvas.SizeY, 128, 128, 256, 256 ); // !! hardcoded size

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
        if( PlayerController(Instigator.Controller) != None )
            PlayerController(Instigator.Controller).StopZoom();
    }
    else
    {
        Super.ClientStopFire(mode);
    }
}

simulated function BringUp(optional Weapon PrevWeapon)
{
    if ( PlayerController(Instigator.Controller) != None )
    {
        LastFOV = PlayerController(Instigator.Controller).DesiredFOV;
		if ( Instigator.IsLocallyControlled() )
			GotoState('TickEffects');
	}
    Super.BringUp(PrevWeapon);
}

simulated function bool PutDown()
{
    if( Instigator.Controller.IsA( 'PlayerController' ) )
        PlayerController(Instigator.Controller).EndZoom();
    if ( Super.PutDown() )
    {
		GotoState('');
		return true;
	}
	return false;
}

state TickEffects
{
    simulated function Tick( float t )
    {
        if (chargeEmitter == None)
        {
            chargeEmitter = Spawn(class'LightningCharge',self);
            AttachToBone(chargeEmitter, 'tip' );
        }
        chargeEmitter.mRegenPause = ( FireMode[0].NextFireTime > Level.TimeSeconds || AmmoAmount(0) == 0 );
    }
}

// AI Interface
function float SuggestAttackStyle()
{
    return -0.4;
}

function float SuggestDefenseStyle()
{
    return 0.2;
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
	local float ZDiff, dist, Result;

	B = Bot(Instigator.Controller);
	if ( B == None )
		return AIRating;
	if ( B.IsShootingObjective() )
		return AIRating - 0.15;
	if ( B.Enemy == None )
		return AIRating;

	if ( B.Stopped() )
		result = AIRating + 0.1;
	else
		result = AIRating - 0.1;
	if ( Vehicle(B.Enemy) != None )
		result -= 0.2;
	ZDiff = Instigator.Location.Z - B.Enemy.Location.Z;
	if ( ZDiff < -200 )
		result += 0.1;
	dist = VSize(B.Enemy.Location - Instigator.Location);
	if ( dist > 2000 )
	{
		if ( !B.EnemyVisible() )
			result = result - 0.15;
		return ( FMin(2.0,result + (dist - 2000) * 0.0002) );
	}
	if ( !B.EnemyVisible() )
		return AIRating - 0.1;

	return result;
}


function bool RecommendRangedAttack()
{
	local Bot B;

	B = Bot(Instigator.Controller);
	if ( (B == None) || (B.Enemy == None) )
		return true;

	return ( VSize(B.Enemy.Location - Instigator.Location) > 2000 * (1 + FRand()) );
}
// end AI Interface

defaultproperties
{
	HighDetailOverlay=Material'UT2004Weapons.WeaponSpecMap2'
    SelectAnimRate=+1.25
	BringUpTime=+0.36
	MinReloadPct=+0.25

    ItemName="Lightning Gun"
    Description="The Lightning Gun is a high-power energy rifle capable of ablating even the heaviest carapace armor. Acquisition of a target at long range requires a steady hand, but the anti-jitter effect of the optical system reduces the weapon's learning curve significantly. Once the target has been acquired, the operator depresses the trigger, painting a proton 'patch' on the target. Milliseconds later the rifle emits a high voltage arc of electricity, which seeks out the charge differential and annihilates the target."
    IconMaterial=Material'HudContent.Generic.HUD'
    IconCoords=(X1=246,Y1=182,X2=331,Y2=210)

    FireModeClass(0)=SniperFire
    FireModeClass(1)=SniperZoom
    InventoryGroup=9

    Mesh=mesh'Weapons.Sniper_1st'
    BobDamping=2.3
    PickupClass=class'SniperRiflePickup'
    EffectOffset=(X=100,Y=28,Z=-26)
    SmallEffectOffset=(X=92,Y=32,Z=-35)

    DrawScale=0.4
    PlayerViewOffset=(X=0,Y=2.3,Z=0)
    SmallViewOffset=(X=8,Y=6.3,Z=-4)
    PlayerViewPivot=(Pitch=0,Roll=0,Yaw=0) // changing this will mess up the charge emitter loc please

    PutDownAnim=PutDown
    IdleAnim=Idle
    SelectAnim=PickUp
    DisplayFOV=60.0

	borderX=60.0
	borderY=60.0

	focusX=135
	focusY=105

	testX=100
	testY=100

    innerArrowsX=42.0
    innerArrowsY=42.0

	ChargeColor=(R=255,G=255,B=255,A=255)
    FocusColor=(R=71,G=90,B=126,A=215)
    NoTargetColor=(R=200,G=200,B=200,A=255)
    TargetColor=(R=255,G=255,B=255,A=255)
    ArrowColor=(R=255,G=0,B=0,A=255)

    RechargeOrigin=(X=600,Y=330,Z=0)
	RechargeSize=(X=10,Y=-180,Z=0)
	AttachmentClass=class'SniperAttachment'
    SelectSound=Sound'WeaponSounds.LightningGun.SwitchToLightningGun'
	zoomed=false
	SelectForce="SwitchToLightningGun"

    bSniping=true
	AIRating=+0.69
	CurrentRating=+0.69

    bDynamicLight=false
    LightType=LT_Steady
    LightEffect=LE_NonIncidence
    LightPeriod=3
    LightBrightness=255
    LightHue=165
    LightSaturation=170
    LightRadius=5.0

    HudColor=(r=185,g=170,b=255,a=255)
	CustomCrosshair=0
	CustomCrosshairTextureName="Crosshairs.Hud.Crosshair_Cross1"
	CustomCrosshairColor=(r=185,g=170,b=255,a=255)
	Priority=11

	CenteredOffsetY=0
	CenteredYaw=-500
}

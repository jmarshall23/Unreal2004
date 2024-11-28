//=============================================================================
// ASTurret_IonCannon
//=============================================================================

#EXEC OBJ LOAD File=../StaticMeshes/AS_Weapons_SM.usx

class ASTurret_IonCannon extends ASTurret;

var vector	RechargeOrigin, RechargeSize;
var	sound	ChargingSound;

simulated function PlayFiring(optional float Rate, optional name FiringMode)
{
	if ( Level.NetMode != NM_DedicatedServer )
	{
		PlayAnim('Fire', 0.5);
		AmbientSound = None;
		PlaySound(sound'WeaponSounds.BExplosion5');
	}
}

simulated event PlayCharge()
{
	if ( Level.NetMode != NM_DedicatedServer )
	{
		PlayAnim('Charge', 0.08, 0.25);
		AmbientSound	= ChargingSound;
	}

}

function Vehicle FindEntryVehicle(Pawn P)
{
	local vector Dir;

	if ( Bot(P.Controller) != None )
	{
		Dir = Location - P.Location;
		if ( Abs(Dir.Z) < CollisionHeight )
		{
			Dir.Z = 0;
			if (VSize(Dir) < EntryRadius)
				return self;
		}
	}
    return Super.FindEntryVehicle(P);
}

simulated event PlayRelease()
{
	if ( Level.NetMode != NM_DedicatedServer )
	{
		PlayAnim('Idle', 1.0, 0.25);
		AmbientSound = None;
		PlaySound(sound'WeaponSounds.TranslocatorModuleRegeneration', SLOT_Interface,, false,, 4.0);
	}
}

simulated function DrawVehicleHUD( Canvas C, PlayerController PC )
{
	local vehicle	V;
	local vector	ScreenPos;
	local string	VehicleInfoString;

	super.DrawVehicleHUD( C, PC );

	if ( Weapon == None || Weapon.GetFireMode(0) == None )
		return;

	// 1st person view
	if ( !PlayerController(Controller).bBehindView )
	{
		C.Style		= ERenderStyle.STY_Alpha;

		// Draw Weird cam
		C.DrawColor.R = 255;
		C.DrawColor.G = 255;
		C.DrawColor.B = 255;
		C.DrawColor.A = 64;

		C.SetPos(0,0);
		C.DrawTile( Material'TransCamFB', C.SizeX, C.SizeY, 0.0, 0.0, 512, 512 ); // !! hardcoded size
		C.SetPos(0,0);

		//C.DrawTile( Material'ScreenNoiseFB', C.SizeX, C.SizeY, 0.0, 0.0, 512, 512 ); // !! hardcoded size
		C.DrawColor	= class'HUD_Assault'.static.GetTeamColor( Team );

		// Draw Reticle around visible vehicles
		foreach DynamicActors(class'Vehicle', V )
		{
			if ( (V==Self) || (V.Health < 1) || V.bDeleteMe || V.GetTeamNum() == Team || !V.IndependentVehicle() )
				continue;

			if ( !class'HUD_Assault'.static.IsTargetInFrontOfPlayer( C, V, ScreenPos, Location, Rotation ) )
				continue;

			if ( !FastTrace( V.Location, Location ) )
				continue;

			C.Font = class'HudBase'.static.GetConsoleFont( C );
			VehicleInfoString = V.VehicleNameString $ ":" @ int(VSize(Location-V.Location)*0.01875.f) $ class'HUD_Assault'.default.MetersString;
			class'HUD_Assault'.static.Draw_2DCollisionBox( C, V, ScreenPos, VehicleInfoString, 1.5f, true );
		}
	}
}


simulated function DrawWeaponInfo( Canvas C, HUD H )
{
	local float		Charge, tileScaleX, tileScaleY, barOrgX, barOrgY, barSizeX, barSizeY;

	super.DrawWeaponInfo( C, H );

	tileScaleX = C.SizeX / 640.0f;
	tileScaleY = C.SizeY / 480.0f;

	if ( Weapon.GetFireMode(0).NextFireTime > Level.TimeSeconds ) // Cooling
		charge = 1.f - FMin( (Weapon.GetFireMode(0).NextFireTime-Level.TimeSeconds) / Weapon.GetFireMode(0).FireRate, 1.f);
	else // Power Up
		charge = 1.f - FMin(Weapon.GetFireMode(0).HoldTime / Weapon.GetFireMode(0).MaxHoldTime, 1.f);

	if ( charge > 0 )
	{
		barOrgX		= RechargeOrigin.X * tileScaleX;
		barOrgY		= RechargeOrigin.Y * tileScaleY;
		barSizeX	= RechargeSize.X * tileScaleX;
		barSizeY	= RechargeSize.Y * tileScaleY;

		C.DrawColor	= class'UT2k4Assault.HUD_Assault'.static.GetGYRColorRamp( charge );
		C.DrawColor.A	= 96;

		C.Style = ERenderStyle.STY_Alpha;
		C.SetPos( barOrgX, barOrgY );
		C.DrawTile(Texture'Engine.WhiteTexture',barSizeX,barSizeY*charge, 0.0, 0.0,Texture'Engine.WhiteTexture'.USize,Texture'Engine.WhiteTexture'.VSize*charge);
	}
}


// Spawn Explosion FX
simulated function Explode( vector HitLocation, vector HitNormal )
{
	if ( Level.NetMode != NM_DedicatedServer )
		ExplosionEffect = Spawn(class'IonCannonDeathEffect', Self,, HitLocation, Rotation);

	if ( TurretBase != None )
		TurretBase.Destroy();

	if ( TurretSwivel != None )
		TurretSwivel.Destroy();
}

static function StaticPrecache(LevelInfo L)
{
    super.StaticPrecache( L );

	L.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_2' );
	L.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_3' );
	L.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_4' );
	L.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_1' );
	L.AddPrecacheMaterial( Material'AS_Weapons_TX.IonCanon.ASIonCannon1' );
	L.AddPrecacheMaterial( Material'AS_Weapons_TX.IonCanon.ASIonCannon2' );

	// FX
	L.AddPrecacheMaterial( Material'AW-2004Particles.Weapons.HardSpot' );
	L.AddPrecacheMaterial( Material'AW-2004Particles.Energy.AirBlastP' );
	L.AddPrecacheMaterial( Material'AW-2004Particles.Energy.PurpleSwell' );
	L.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp2_framesP' );
	L.AddPrecacheMaterial( Texture'EpicParticles.Flares.SoftFlare' );
	L.AddPrecacheMaterial( Texture'EpicParticles.Beams.WhiteStreak01aw' );
	L.AddPrecacheMaterial( Texture'AW-2004Particles.Energy.EclipseCircle' );
	L.AddPrecacheMaterial( Texture'EpicParticles.Flares.HotSpot' );
	L.AddPrecacheMaterial( Material'AW-2004Particles.Weapons.GrenExpl' );
	L.AddPrecacheMaterial( Material'AS_FX_TX.Flares.Laser_Flare' );
	L.AddPrecacheMaterial( Material'AW-2004Particles.Weapons.PlasmaStar' );

	L.AddPrecacheStaticMesh( StaticMesh'AW-2004Particles.Weapons.PlasmaSphere' );

	L.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.IonBase' );
	L.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.IonSwivel' );
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.IonBase' );
	Level.AddPrecacheStaticMesh( StaticMesh'AS_Weapons_SM.IonSwivel' );

	Level.AddPrecacheStaticMesh( StaticMesh'AW-2004Particles.Weapons.PlasmaSphere' );

	super.UpdatePrecacheStaticMeshes();
}

simulated function UpdatePrecacheMaterials()
{
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_2' );
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_3' );
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_4' );
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Beams.HotBolt_1' );
	Level.AddPrecacheMaterial( Material'AS_Weapons_TX.IonCanon.ASIonCannon1' );
	Level.AddPrecacheMaterial( Material'AS_Weapons_TX.IonCanon.ASIonCannon2' );

	// FX
	Level.AddPrecacheMaterial( Material'AW-2004Particles.Weapons.HardSpot' );
	Level.AddPrecacheMaterial( Material'AW-2004Particles.Energy.AirBlastP' );
	Level.AddPrecacheMaterial( Material'AW-2004Particles.Energy.PurpleSwell' );
	Level.AddPrecacheMaterial( Material'ExplosionTex.Framed.exp2_framesP' );
	Level.AddPrecacheMaterial( Texture'EpicParticles.Flares.SoftFlare' );
	Level.AddPrecacheMaterial( Texture'EpicParticles.Beams.WhiteStreak01aw' );
	Level.AddPrecacheMaterial( Texture'AW-2004Particles.Energy.EclipseCircle' );
	Level.AddPrecacheMaterial( Texture'EpicParticles.Flares.HotSpot' );
	Level.AddPrecacheMaterial( Material'AW-2004Particles.Weapons.GrenExpl' );
	Level.AddPrecacheMaterial( Material'AS_FX_TX.Flares.Laser_Flare' );
	Level.AddPrecacheMaterial( Material'AW-2004Particles.Weapons.PlasmaStar' );

	super.UpdatePrecacheMaterials();
}

defaultproperties
{
	bShowDamageOverlay=false
	RechargeOrigin=(X=620,Y=400,Z=0)
	RechargeSize=(X=10,Y=-100,Z=0)
	ChargingSound=Sound'AssaultSounds.IonPowerUp'

	DefaultWeaponClassName="UT2k4AssaultFull.Weapon_Turret_IonCannon"
	VehicleProjSpawnOffset=(X=925,Y=0,Z=200)
	RotPitchConstraint=(Min=13184,Max=11500)
	RotationInertia=0.9
	RotationSpeed=0.5
	CamRotationInertia=3.0
	Health=2000
	HealthMax=2000
	AmbientGlow=8
	DriverDamageMult=0.0

	bRelativeExitPos=true
	EntryPosition=(X=-125,Y=-200,Z=-200)
    EntryRadius=120.f
	ExitPositions(0)=(X=-125,Y=-200,Z=128)
	ExitPositions(1)=(X=-125,Y=+200,Z=80)

	bCHZeroYOffset=false
	DefaultCrosshair=Material'Crosshairs.HUD.Crosshair_Circle1'
	CrosshairScale=0.5

	CamAbsLocation=(X=0,Y=0,Z=0)
	CamRelLocation=(X=0,Y=0,Z=400)
	CamDistance=(X=-900,Y=0,Z=0)
	FPCamPos=(X=400,Y=0,Z=400)
	bDrawMeshInFP=true

	Mesh=SkeletalMesh'AS_VehiclesFull_M.IonCannon'
	DrawScale=0.66
	TurretBaseClass=class'UT2k4AssaultFull.ASTurret_IonCannon_Base'
	TurretSwivelClass=class'UT2k4AssaultFull.ASTurret_IonCannon_Swivel'

	ShadowMaxTraceDist=+1050
	ShadowCullDistance=4500.0

    CollisionHeight=300.0
    CollisionRadius=200.0

	bRemoteControlled=false
	bAutoTurret=false
	VehiclePositionString="manning an Ion Cannon"
	VehicleNameString="Ion Cannon"

    TransientSoundVolume=1.0
    TransientSoundRadius=5000.0
	SoundVolume=255
	SoundRadius=5000
	SoundPitch=51

	bAlwaysRelevant=true
}

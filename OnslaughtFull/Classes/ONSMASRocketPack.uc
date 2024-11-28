//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSMASRocketPack extends ONSWeapon;

var float MaxLockRange, LockAim;

static function StaticPrecache(LevelInfo L)
{
    L.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    L.AddPrecacheMaterial(Material'AS_FX_TX.Trails.Trail_Blue');
    L.AddPrecacheMaterial(Material'EpicParticles.Flares.FlickerFlare');
    L.AddPrecacheMaterial(Material'WeaponSkins.Skins.RocketShellTex');
    L.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    L.AddPrecacheMaterial(Material'AS_FX_TX.Trails.Trail_Blue');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.SmokeReOrdered');
    L.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp2_frames');
    L.AddPrecacheMaterial(Material'XEffects.Skins.Rexpt');
    L.AddPrecacheMaterial(Material'EmitterTextures.MultiFrame.rockchunks02');

    L.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.RocketProj');
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    Level.AddPrecacheMaterial(Material'AS_FX_TX.Trails.Trail_Blue');
    Level.AddPrecacheMaterial(Material'EpicParticles.Flares.FlickerFlare');
    Level.AddPrecacheMaterial(Material'WeaponSkins.Skins.RocketShellTex');
    Level.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    Level.AddPrecacheMaterial(Material'AS_FX_TX.Trails.Trail_Blue');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.SmokeReOrdered');
    Level.AddPrecacheMaterial(Material'ExplosionTex.Framed.exp2_frames');
    Level.AddPrecacheMaterial(Material'XEffects.Skins.Rexpt');
    Level.AddPrecacheMaterial(Material'EmitterTextures.MultiFrame.rockchunks02');

    Super.UpdatePrecacheMaterials();
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.RocketProj');
	Super.UpdatePrecacheStaticMeshes();
}


state ProjectileFireMode
{
	function Fire(Controller C)
	{
		local ONSMASRocketProjectile R;
		local float BestAim, BestDist;

		R = ONSMASRocketProjectile(SpawnProjectile(ProjectileClass, False));
		if (R != None)
		{
			if (AIController(C) != None)
				R.HomingTarget = C.Enemy;
			else
			{
				BestAim = LockAim;
				R.HomingTarget = C.PickTarget(BestAim, BestDist, vector(WeaponFireRotation), WeaponFireLocation, MaxLockRange);
			}
		}
	}
}


DefaultProperties
{
    Mesh=Mesh'ONSFullAnimations.MASRocketPack'
    YawBone=RocketPivot
    YawStartConstraint=0
    YawEndConstraint=65535
    PitchBone=RocketPacks
    PitchUpLimit=18000
    PitchDownLimit=60000
    FireSoundClass=sound'WeaponSounds.RocketLauncher.RocketLauncherFire'
    FireForce="RocketLauncherFire"
    ProjectileClass=class'OnslaughtFull.ONSMASRocketProjectile'
    FireInterval=0.35
    WeaponFireAttachmentBone=RocketPackFirePoint
    WeaponFireOffset=0.0
    bAimable=True
    CollisionRadius=+60.0
    DualFireOffset=80.0
    bDualIndependantTargeting=True
    LockAim=0.975
    MaxLockRange=30000
    AIInfo(0)=(bLeadTarget=true)
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSHoverTankCannon extends ONSWeapon;

var vector OldDir;
var rotator OldRot;

#exec OBJ LOAD FILE=..\Animations\ONSWeapons-A.ukx

static function StaticPrecache(LevelInfo L)
{
    L.AddPrecacheMaterial(Material'WeaponSkins.RocketShellTex');
    L.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    L.AddPrecacheMaterial(Material'XEffects.SmokeAlphab_t');
    L.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.TankTrail');
    L.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.SmokePanels2');
    L.AddPrecacheMaterial(Material'ONSInterface-TX.tankBarrelAligned');
    L.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.cloudParticleOrange');
    L.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.TankDustKick1');
    L.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.TankDustKick');
    L.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.tankHitRocks');
    L.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.DirtPuffTEX');
}

simulated function UpdatePrecacheMaterials()
{
    Level.AddPrecacheMaterial(Material'WeaponSkins.RocketShellTex');
    Level.AddPrecacheMaterial(Material'XEffects.RocketFlare');
    Level.AddPrecacheMaterial(Material'XEffects.SmokeAlphab_t');
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.TankTrail');
    Level.AddPrecacheMaterial(Material'AW-2004Particles.Weapons.SmokePanels2');
    Level.AddPrecacheMaterial(Material'ONSInterface-TX.tankBarrelAligned');
    Level.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.cloudParticleOrange');
    Level.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.TankDustKick1');
    Level.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.TankDustKick');
    Level.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.tankHitRocks');
    Level.AddPrecacheMaterial(Material'VMParticleTextures.TankFiringP.DirtPuffTEX');

    Super.UpdatePrecacheMaterials();
}

simulated function UpdatePrecacheStaticMeshes()
{
	Level.AddPrecacheStaticMesh(StaticMesh'WeaponStaticMesh.RocketProj');
	Super.UpdatePrecacheStaticMeshes();
}

function byte BestMode()
{
	return 0;
}

simulated function PostBeginPlay()
{
	Super.PostBeginPlay();

	OldDir = Vector(CurrentAim);
}

function Tick(float Delta)
{
	local int i;
	local xPawn P;
	local vector NewDir, PawnDir;
    local coords WeaponBoneCoords;


    Super.Tick(Delta);

	if ( (Role == ROLE_Authority) && (Base != None) )
	{
	    WeaponBoneCoords = GetBoneCoords(YawBone);
		NewDir = WeaponBoneCoords.XAxis;
		if ( (Vehicle(Base).Controller != None) && (NewDir.Z < 0.9) )
		{
			for ( i=0; i<Base.Attached.Length; i++ )
			{
				P = XPawn(Base.Attached[i]);
				if ( (P != None) && (P.Physics != PHYS_None) && (P != Vehicle(Base).Driver) )
				{
					PawnDir = P.Location - WeaponBoneCoords.Origin;
					PawnDir.Z = 0;
					PawnDir = Normal(PawnDir);
					if ( ((PawnDir.X <= NewDir.X) && (PawnDir.X > OldDir.X))
						|| ((PawnDir.X >= NewDir.X) && (PawnDir.X < OldDir.X)) )
					{
						if ( ((PawnDir.Y <= NewDir.Y) && (PawnDir.Y > OldDir.Y))
							|| ((PawnDir.Y >= NewDir.Y) && (PawnDir.X < OldDir.Y)) )
						{
							P.SetPhysics(PHYS_Falling);
							P.Velocity = WeaponBoneCoords.YAxis;
							if ( ((NewDir - OldDir) Dot WeaponBoneCoords.YAxis) < 0 )
								P.Velocity *= -1;
							P.Velocity = 500 * (P.Velocity + 0.3*NewDir);
							P.Velocity.Z = 200;
						}
					}
				}
			}
		}
		OldDir = NewDir;
	}
}

DefaultProperties
{
    Mesh=Mesh'ONSWeapons-A.HoverTankCannon'
    RedSkin=Shader'VMVehicles-TX.HoverTankGroup.HoverTankChassisFinalRED'
    BlueSkin=Shader'VMVehicles-TX.HoverTankGroup.HoverTankChassisFinalBLUE'
    YawBone=TankTurret
    YawStartConstraint=0
    YawEndConstraint=65535
    PitchBone=TankBarrel
    PitchUpLimit=6000
    PitchDownLimit=61500
    FireSoundClass=sound'ONSVehicleSounds-S.TankFire01'
    FireForce="Explosion05"
    ProjectileClass=class'Onslaught.ONSRocketProjectile'
    EffectEmitterClass=class'Onslaught.ONSTankFireEffect'
    FireInterval=2.5
    FireSoundVolume=512
    ShakeRotMag=(Z=250)
    ShakeRotRate=(Z=2500)
    ShakeRotTime=6
    ShakeOffsetMag=(Z=10)
    ShakeOffsetRate=(Z=200)
    ShakeOffsetTime=10
    WeaponFireAttachmentBone=TankBarrel
    WeaponFireOffset=200.0
    bAimable=True
    RotationsPerSecond=0.18
    AIInfo(0)=(bLeadTarget=true,bTrySplash=true,WarnTargetPct=0.75,RefireRate=0.5)
    Spread=0.015
}

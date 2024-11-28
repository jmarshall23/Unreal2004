//=============================================================================
// xHeavyWallHitEffect.
//=============================================================================
class SniperWallHitEffect extends Effects;
 
var sound ImpactSounds[6];
var vector FireStart;

replication
{
	reliable if ( Role == ROLE_Authority )
		FireStart;
}

simulated function SpawnEffects()
{
	local rotator ReverseRot;
	local PlayerController PC;
	local vector Dir, LinePos, LineDir;
	local bool bViewed;
	
	PlaySound(ImpactSounds[Rand(6)],, 2.5*TransientSoundVolume,,200);
	
	PC = Level.GetLocalPlayerController();
	if ( (PC != None) && (PC.ViewTarget != None) && (VSize(PC.Viewtarget.Location - Location) < 3000*PC.FOVBias) )
	{
		Spawn(class'LongBulletDecal');
		bViewed = true;
	}
	
	if ( !PhysicsVolume.bWaterVolume )
	{
		ReverseRot = rotator(-1 * vector(Rotation));
		if ( bViewed )
			Spawn(class'pclImpactSmoke',,,,ReverseRot);
		Spawn(class'WallSparks',,,,ReverseRot);
	}
	
	if ( FireStart != vect(0,0,0) )
	{
		// see if local player controller near bullet, but missed
		if ( (PC != None) && (PC.Pawn != None) )
		{
			Dir = Normal(Location - FireStart);
			LinePos = (FireStart + (Dir dot (PC.Pawn.Location - FireStart)) * Dir);
			LineDir = PC.Pawn.Location - LinePos;
			if ( VSize(LineDir) < 150 )
			{
				SetLocation(LinePos);
				if ( FRand() < 0.5 )
					PlaySound(sound'Impact3Snd',,,,80);
				else
					PlaySound(sound'Impact7Snd',,,,80);
 			}
		}
	}
}

simulated function PostNetBeginPlay()
{
	if ( Role == ROLE_Authority )
	{
		if ( Instigator != None )
			MakeNoise(0.5);
	}
	if ( Level.NetMode != NM_DedicatedServer )
		SpawnEffects();

	Super.PostNetBeginPlay();
}

defaultproperties
{
	DrawType=DT_None
	bHighDetail=false
	Lifespan=+0.2
	RemoteRole=ROLE_SimulatedProxy
	bNetTemporary=true
	ImpactSounds(0)=sound'Impact1Snd'
	ImpactSounds(1)=sound'Impact2Snd'
	ImpactSounds(2)=sound'Impact5Snd'
	ImpactSounds(3)=sound'Impact3'
	ImpactSounds(4)=sound'Impact1'
	ImpactSounds(5)=sound'Impact2'
}

//=============================================================================
// xHeavyWallHitEffect.
//=============================================================================
class xHeavyWallHitEffect extends Effects;

#exec AUDIO IMPORT FILE="Sounds\BulletImpact1.WAV" NAME="Impact1Snd"
#exec AUDIO IMPORT FILE="Sounds\BulletImpact2.WAV" NAME="Impact2Snd"
#exec AUDIO IMPORT FILE="Sounds\BulletImpact3.WAV" NAME="Impact3Snd"
#exec AUDIO IMPORT FILE="Sounds\BulletImpact4.WAV" NAME="Impact4Snd"
#exec AUDIO IMPORT FILE="Sounds\BulletImpact5.WAV" NAME="Impact5Snd"
#exec AUDIO IMPORT FILE="Sounds\BulletImpact6.WAV" NAME="Impact6Snd"
#exec AUDIO IMPORT FILE="Sounds\BulletImpact7.WAV" NAME="Impact7Snd"
#exec AUDIO IMPORT FILE="Sounds\imp01.WAV" NAME="Impact1"    
#exec AUDIO IMPORT FILE="Sounds\imp02.WAV" NAME="Impact2"    
#exec AUDIO IMPORT FILE="Sounds\imp03.WAV" NAME="Impact3"    
 
var sound ImpactSounds[10];

simulated function PostNetBeginPlay()
{
	Super.PostNetBeginPlay();

	if ( Role == ROLE_Authority )
	{
		if ( Instigator != None )
			MakeNoise(0.3);
	}
	if ( Level.NetMode != NM_DedicatedServer )
		SpawnEffects();
}

simulated function SpawnEffects()
{
	local playercontroller P;
	local bool bViewed;
	
	PlaySound(ImpactSounds[Rand(10)]);
	
	P = Level.GetLocalPlayerController();
	if ( (P != None) && (P.ViewTarget != None) && (VSize(P.Viewtarget.Location - Location) < 1600*P.FOVBias) && ((vector(P.Rotation) dot (Location - P.ViewTarget.Location)) > 0) )
	{
		Spawn(class'BulletDecal',self,,Location, rotator(-1 * vector(Rotation)));
		bViewed = true;
	}
	if ( PhysicsVolume.bWaterVolume )
		return;

	if ( (Level.DetailMode == DM_Low) || Level.bDropDetail )
	{
		if ( bViewed && (FRand() < 0.25) )
			Spawn(class'pclImpactSmoke');
		else		
			Spawn(class'WallSparks');
		return;
	}

	if ( bViewed && (FRand() < 0.5) )
		Spawn(class'pclImpactSmoke');
	Spawn(class'WallSparks');
}

defaultproperties
{
	Style=STY_Additive
	Drawtype=DT_None
	RemoteRole=ROLE_None
	LifeSpan=0.1
	ImpactSounds(0)=sound'Impact1Snd'
	ImpactSounds(1)=sound'Impact2Snd'
	ImpactSounds(2)=sound'Impact3Snd'
	ImpactSounds(3)=sound'Impact4Snd'
	ImpactSounds(4)=sound'Impact5Snd'
	ImpactSounds(5)=sound'Impact6Snd'
	ImpactSounds(6)=sound'Impact7Snd'
	ImpactSounds(7)=sound'Impact3'
	ImpactSounds(8)=sound'Impact1'
	ImpactSounds(9)=sound'Impact2'
    CullDistance=+7000.0
}

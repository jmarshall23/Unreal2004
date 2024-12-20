//=============================================================================
// FX_Turret_IonCannon_LaserBeam
//=============================================================================
// Created by Laurent Delayen (C) 2003 Epic Games
//=============================================================================

class FX_Turret_IonCannon_LaserBeam extends Emitter
	notplaceable;

var vector	StartLocation, EndLocation;
var	float	BeamSize;

var	WA_Turret_IonCannon	IonCannonOwner;

replication
{
    unreliable if ( Role == ROLE_Authority && bNetInitial && bNetOwner )
        IonCannonOwner;

    unreliable if ( Role == ROLE_Authority && bNetDirty && !bNetOwner )
        StartLocation, EndLocation;
}

function PostBeginPlay()
{
	super.PostBeginPlay();
	SetWeaponOwner();
}

simulated function Tick(float DeltaTime)
{
	UpdateBeamLocation();

	if ( Level.NetMode != NM_DedicatedServer )
	{
		UpdateLaserBeamFX( DeltaTime );

		SetLocation( StartLocation );
		SetRotation( Rotator(EndLocation - StartLocation) );
	}
}

simulated function SetWeaponOwner()
{
	IonCannonOwner = WA_Turret_IonCannon(Owner);
}

simulated function UpdateBeamLocation()
{
	if ( IonCannonOwner != None && ( Role==Role_Authority || IonCannonOwner.CannonPawn.IsLocallyControlled()) )
		IonCannonOwner.UpdateLaserBeamLocation(StartLocation, EndLocation);
}

simulated function UpdateLaserBeamFX( float DeltaTime )
{
	local float		Dist;

	Dist = VSize( EndLocation - StartLocation );
	BeamEmitter(Emitters[0]).BeamDistanceRange.Min = Dist;
	BeamEmitter(Emitters[0]).BeamDistanceRange.Max = Dist;

	// Beam Growing effect
	if ( DeltaTime < 1  )
		BeamSize = FMin( BeamSize + DeltaTime, 2.f);

	BeamEmitter(Emitters[0]).StartSizeRange.X.Min = BeamSize * 35.f;
	BeamEmitter(Emitters[0]).StartSizeRange.X.Max = BeamSize * 40.f;
}

//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
	NetUpdateFrequency=15
	bSkipActorPropertyReplication=true
	bOnlyDirtyReplication=true
	bAlwaysRelevant=true
    RemoteRole=ROLE_SimulatedProxy
    bNetTemporary=false
    bReplicateInstigator=true

	Begin Object Class=BeamEmitter Name=BeamEmitter2
        BeamDistanceRange=(Min=500.000000,Max=500.000000)
        DetermineEndPointBy=PTEP_Distance
        RotatingSheets=3
        LowFrequencyPoints=2
        HighFrequencyPoints=2
		CoordinateSystem=PTCS_Relative
        UseColorScale=True
        ColorScale(0)=(Color=(B=16,G=16,R=220))
        ColorScale(1)=(RelativeTime=1.000000,Color=(B=16,G=16,R=180))
        Opacity=0.330000
        MaxParticles=1
        StartSizeRange=(X=(Min=30.000000,Max=35.000000))
        InitialParticlesPerSecond=2000.000000
        AutomaticInitialSpawning=False
        Texture=Texture'EpicParticles.Beams.WhiteStreak01aw'
		SecondsBeforeInactive=0
        LifetimeRange=(Min=0.080000,Max=0.160000)
        StartVelocityRange=(X=(Min=0.001000,Max=0.001000))
        Name="BeamEmitter2"
    End Object
    Emitters(0)=BeamEmitter'BeamEmitter2'

    bDirectional=true
    bNoDelete=false
    AutoDestroy=false
}
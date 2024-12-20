class NewTransDeresBlue extends Emitter;

function PostBeginPlay()
{
	if ( Level.NetMode == NM_DedicatedServer )
		LifeSpan = 0.15;
	else if ( Level.GetLocalPlayerController().BeyondViewDistance(Location,0) )
	{
		LifeSpan = 0.01;
		return;
	}
	Super.PostBeginPlay();
}
defaultproperties
{  
    Begin Object Class=SpriteEmitter Name=SpriteEmitter4
        UseDirectionAs=PTDU_Right
        UseColorScale=True
        ColorScale(1)=(RelativeTime=0.100000,Color=(B=255,G=128))
        ColorScale(2)=(RelativeTime=0.750000,Color=(B=255,G=128))
        ColorScale(3)=(RelativeTime=1.000000)
        MaxParticles=27
        RespawnDeadParticles=False
        StartLocationRange=(Z=(Min=-64.000000,Max=64.000000))
        StartLocationShape=PTLS_All
        SphereRadiusRange=(Min=32.000000,Max=32.000000)
        UseRotationFrom=PTRS_Actor
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=20.000000)
        SizeScale(1)=(RelativeTime=0.300000,RelativeSize=8.000000)
        SizeScale(2)=(RelativeTime=1.000000,RelativeSize=0.500000)
        StartSizeRange=(X=(Min=8.000000,Max=12.000000))
        UniformSize=True
        ScaleSizeXByVelocity=True
        ScaleSizeByVelocityMultiplier=(X=0.005000)
        InitialParticlesPerSecond=1000.000000
        AutomaticInitialSpawning=False
        Texture=Texture'AW-2004Particles.Energy.BandFlash'
        LifetimeRange=(Min=0.500000,Max=1.000000)
        StartVelocityRange=(X=(Min=5.000000,Max=8.000000))
        UseVelocityScale=True
        VelocityScale(0)=(RelativeVelocity=(X=1.000000,Y=1.000000,Z=1.000000))
        VelocityScale(1)=(RelativeTime=0.200000,RelativeVelocity=(X=5.000000,Y=5.000000,Z=5.000000))
        VelocityScale(2)=(RelativeTime=0.400000,RelativeVelocity=(X=20.000000,Y=20.000000,Z=20.000000))
        VelocityScale(3)=(RelativeTime=1.000000,RelativeVelocity=(X=200.000000,Y=200.000000,Z=200.000000))
        Name="SpriteEmitter4"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter4'

    RemoteRole=ROLE_SimulatedProxy
    bNetTemporary=true
    bNoDelete=False
    bNetInitialRotation=true
    AutoDestroy=True
    Texture=Texture'AW-2004Particles.Energy.BandFlash'
}
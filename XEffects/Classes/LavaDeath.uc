#exec OBJ LOAD FILE=EpicParticles.utx

class LavaDeath extends Emitter;

defaultproperties
{
    Begin Object Class=SpriteEmitter Name=SpriteEmitter1
        Acceleration=(Z=15.000000)
        UseColorScale=True
        ColorScale(0)=(Color=(B=51,G=152,R=200))
        ColorScale(1)=(RelativeTime=0.300000,Color=(B=48,G=91,R=222))
        ColorScale(2)=(RelativeTime=1.000000)
        MaxParticles=40
        RespawnDeadParticles=False
        StartLocationRange=(X=(Min=-8.000000,Max=8.000000),Y=(Min=-8.000000,Max=8.000000),Z=(Min=-32.000000,Max=32.000000))
        StartLocationShape=PTLS_All
        SphereRadiusRange=(Min=48.000000,Max=48.000000)
        MeshSpawningStaticMesh=StaticMesh'ParticleMeshes.Simple.ParticleBomb'
        MeshScaleRange=(X=(Min=0.500000,Max=0.500000),Y=(Min=0.500000,Max=0.500000),Z=(Min=0.500000,Max=0.500000))
        SpinParticles=True
        SpinsPerSecondRange=(X=(Max=0.100000))
        StartSpinRange=(X=(Max=1.000000))
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=0.700000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=2.000000)
        StartSizeRange=(X=(Min=20.000000,Max=40.000000),Y=(Min=20.000000,Max=20.000000),Z=(Min=20.000000,Max=20.000000))
        UniformSize=True
        Texture=Texture'EpicParticles.Smoke.Smokepuff2'
        LifetimeRange=(Min=0.300000,Max=0.300000)
		SecondsBeforeInactive=0
        Name="SpriteEmitter1"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter1'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter2
        Acceleration=(Z=150.000000)
        UseColorScale=True
        ColorScale(0)=(Color=(G=255,R=255,A=128))
        ColorScale(1)=(RelativeTime=0.300000,Color=(B=47,G=80,R=179,A=255))
        ColorScale(2)=(RelativeTime=0.600000,Color=(A=80))
        ColorScale(3)=(RelativeTime=1.000000)
        MaxParticles=20
        RespawnDeadParticles=False
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Min=48.000000,Max=48.000000)
        SpinParticles=True
        SpinsPerSecondRange=(X=(Max=0.100000))
        StartSpinRange=(X=(Max=1.000000))
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=0.700000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=2.000000)
        StartSizeRange=(X=(Min=20.000000,Max=40.000000),Y=(Min=20.000000,Max=40.000000))
        UniformSize=True
        InitialParticlesPerSecond=80.000000
        AutomaticInitialSpawning=False
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'EpicParticles.Smoke.Smokepuff'
        LifetimeRange=(Min=0.700000,Max=0.700000)
		SecondsBeforeInactive=0
        Name="SpriteEmitter2"
    End Object
    Emitters(1)=SpriteEmitter'SpriteEmitter2'

	bHighDetail=true
	bHardAttach=true
    bNoDelete=false
    RemoteRole=ROLE_None
    bNetTemporary=true
	bHidden=false
	AutoDestroy=true
}
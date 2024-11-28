class IonCannonDeathEffect extends Emitter;

#exec OBJ LOAD FILE=..\Textures\AW-2004Particles.utx

var Sound	ExplosionSound[11];
var byte	bPlayed[11];

auto State Explosion
{

Begin:
	PlayUniqueRandomExplosion();
	PlayUniqueRandomExplosion();
	
	Sleep(0.33);
	PlayUniqueRandomExplosion();
}

simulated function PlayUniqueRandomExplosion()
{
	local int Num;
	
	Num = Rand(42) % 11;
	if ( bPlayed[Num] > 0 )	
	{	// try again if already played this sound...
		PlayUniqueRandomExplosion();
		return;
	}

	PlaySound(ExplosionSound[Num],, 4,, 1600);
	bPlayed[Num] = 1;
}

/*
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
		*/

defaultproperties
{
	ExplosionSound(0)=Sound'ONSVehicleSounds-S.Explosion01'
	ExplosionSound(1)=Sound'ONSVehicleSounds-S.Explosion02'
	ExplosionSound(2)=Sound'ONSVehicleSounds-S.Explosion03'
	ExplosionSound(3)=Sound'ONSVehicleSounds-S.Explosion04'
	ExplosionSound(4)=Sound'ONSVehicleSounds-S.Explosion05'
	ExplosionSound(5)=Sound'ONSVehicleSounds-S.Explosion06'
	ExplosionSound(6)=Sound'ONSVehicleSounds-S.Explosion07'
	ExplosionSound(7)=Sound'ONSVehicleSounds-S.Explosion08'
	ExplosionSound(8)=Sound'ONSVehicleSounds-S.Explosion09'
	ExplosionSound(9)=Sound'ONSVehicleSounds-S.Explosion10'
	ExplosionSound(10)=Sound'ONSVehicleSounds-S.Explosion11'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter1
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        RespawnDeadParticles=False
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        ColorMultiplierRange=(Y=(Min=0.600000,Max=0.600000))
        MaxParticles=1
        StartLocationOffset=(X=150.000000,Z=350.000000)
        SizeScale(1)=(RelativeTime=0.500000,RelativeSize=0.300000)
        SizeScale(2)=(RelativeTime=1.000000,RelativeSize=5.000000)
        InitialParticlesPerSecond=500.000000
        Texture=AW-2004Particles.Energy.PurpleSwell
        LifetimeRange=(Min=0.500000,Max=0.500000)
        Name="SpriteEmitter1"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter1'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter2
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        RespawnDeadParticles=False
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        ColorMultiplierRange=(Y=(Min=0.600000,Max=0.600000))
        Opacity=0.500000
        MaxParticles=30
        StartLocationOffset=(X=150.000000,Z=200.000000)
        StartLocationRange=(X=(Min=-500.000000,Max=500.000000))
        StartLocationShape=PTLS_All
        SphereRadiusRange=(Min=400.000000,Max=500.000000)
        StartSpinRange=(X=(Max=1.000000))
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=3.000000)
        StartSizeRange=(X=(Min=300.000000,Max=400.000000))
        InitialParticlesPerSecond=500.000000
        Texture=ExplosionTex.Framed.exp2_framesP
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.400000,Max=0.400000)
        Name="SpriteEmitter2"
    End Object
    Emitters(1)=SpriteEmitter'SpriteEmitter2'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter4
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        UseColorScale=True
        RespawnDeadParticles=False
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        UseRandomSubdivision=True
        ColorScale(0)=(Color=(B=255,G=255,R=255))
        ColorScale(1)=(RelativeTime=0.800000,Color=(B=255,G=255,R=255))
        ColorScale(2)=(RelativeTime=1.000000)
        MaxParticles=15
	UseRotationFrom=PTRS_Actor
        StartLocationOffset=(X=150.000000,Z=350.000000)
        StartLocationRange=(X=(Min=-600.000000,Max=600.000000),Y=(Min=-100.000000,Max=100.000000),Z=(Min=-100.000000,Max=100.000000))
        StartLocationShape=PTLS_All
        SphereRadiusRange=(Min=600.000000,Max=800.000000)
        StartSpinRange=(X=(Max=1.000000))
        SizeScale(0)=(RelativeSize=0.500000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=3.000000)
        StartSizeRange=(X=(Min=300.000000,Max=400.000000))
        InitialParticlesPerSecond=100.000000
        Texture=ExplosionTex.Framed.exp2_framesP
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.200000,Max=0.400000)
        InitialDelayRange=(Min=0.150000,Max=0.150000)
        Name="SpriteEmitter4"
    End Object
    Emitters(2)=SpriteEmitter'SpriteEmitter4'

    Begin Object Class=BeamEmitter Name=BeamEmitter0
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        BeamDistanceRange=(Min=2000.000000,Max=4000.000000)
        DetermineEndPointBy=PTEP_Distance
        LowFrequencyNoiseRange=(Y=(Min=-50.000000,Max=50.000000),Z=(Min=-50.000000,Max=50.000000))
        HighFrequencyNoiseRange=(Y=(Min=-80.000000,Max=80.000000),Z=(Min=-80.000000,Max=80.000000))
        RespawnDeadParticles=False
        AutomaticInitialSpawning=False
        ColorMultiplierRange=(X=(Min=0.800000,Max=0.800000),Y=(Min=0.500000,Max=0.500000),Z=(Min=0.800000,Max=0.800000))
        Opacity=0.200000
        MaxParticles=50
	UseRotationFrom=PTRS_Actor
        StartLocationOffset=(X=100.000000,Z=336.000000)
        StartSizeRange=(X=(Min=5.000000,Max=200.000000))
        InitialParticlesPerSecond=80.000000
        Texture=AW-2004Particles.Energy.PowerBolt
        LifetimeRange=(Min=0.200000,Max=0.200000)
        InitialDelayRange=(Min=0.300000,Max=0.300000)
        StartVelocityRange=(X=(Min=300.000000,Max=600.000000),Y=(Min=-100.000000,Max=100.000000),Z=(Min=-100.000000,Max=100.000000))
        Name="BeamEmitter0"
    End Object
    Emitters(3)=BeamEmitter'BeamEmitter0'

    Begin Object Class=BeamEmitter Name=BeamEmitter1
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        BeamDistanceRange=(Min=300.000000,Max=600.000000)
        DetermineEndPointBy=PTEP_Distance
        LowFrequencyNoiseRange=(Y=(Min=-20.000000,Max=-100.000000),Z=(Min=-50.000000,Max=50.000000))
        HighFrequencyNoiseRange=(X=(Min=-10.000000,Max=10.000000),Y=(Min=-10.000000,Max=10.000000),Z=(Min=-10.000000,Max=10.000000))
        NoiseDeterminesEndPoint=True
        RespawnDeadParticles=False
        AutomaticInitialSpawning=False
        ColorMultiplierRange=(Y=(Min=0.500000,Max=0.500000))
        MaxParticles=20
	UseRotationFrom=PTRS_Actor
        StartLocationOffset=(X=-860.000000,Y=-140.000000,Z=288.000000)
        StartLocationRange=(X=(Min=-10.000000,Max=5.000000),Y=(Min=-5.000000,Max=5.000000),Z=(Min=-80.000000,Max=10.000000))
        StartLocationShape=PTLS_Sphere
        StartSizeRange=(X=(Min=-25.000000,Max=25.000000),Y=(Max=-100.000000))
        InitialParticlesPerSecond=30.000000
        Texture=EpicParticles.Beams.HotBolt03aw
        LifetimeRange=(Min=0.100000,Max=0.200000)
        InitialDelayRange=(Min=0.300000,Max=0.350000)
        StartVelocityRange=(X=(Min=1.000000,Max=1.000000),Y=(Min=0.200000,Max=0.200000))
        Name="BeamEmitter1"
    End Object
    Emitters(4)=BeamEmitter'BeamEmitter1'

    Begin Object Class=BeamEmitter Name=BeamEmitter3
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        BeamDistanceRange=(Min=300.000000,Max=800.000000)
        DetermineEndPointBy=PTEP_Distance
        LowFrequencyNoiseRange=(Y=(Min=40.000000,Max=150.000000),Z=(Min=-50.000000,Max=50.000000))
        HighFrequencyNoiseRange=(X=(Min=-10.000000,Max=10.000000),Y=(Min=-10.000000,Max=10.000000),Z=(Min=-10.000000,Max=10.000000))
        NoiseDeterminesEndPoint=True
        RespawnDeadParticles=False
        AutomaticInitialSpawning=False
        ColorMultiplierRange=(Y=(Min=0.500000,Max=0.500000))
        MaxParticles=25
	UseRotationFrom=PTRS_Actor
        StartLocationOffset=(X=-780.000000,Y=86.000000,Z=288.000000)
        StartLocationRange=(X=(Min=-10.000000,Max=5.000000),Y=(Min=-5.000000,Max=5.000000),Z=(Min=-80.000000,Max=10.000000))
        StartLocationShape=PTLS_Sphere
        StartSizeRange=(X=(Min=-25.000000,Max=25.000000),Y=(Max=-100.000000))
        InitialParticlesPerSecond=30.000000
        Texture=EpicParticles.Beams.HotBolt03aw
        LifetimeRange=(Min=0.100000,Max=0.150000)
        InitialDelayRange=(Min=0.300000,Max=0.350000)
        StartVelocityRange=(X=(Min=1.000000,Max=1.000000),Y=(Min=-0.300000,Max=-0.300000),Z=(Min=0.100000,Max=0.100000))
        Name="BeamEmitter3"
    End Object
    Emitters(5)=BeamEmitter'BeamEmitter3'

    Begin Object Class=BeamEmitter Name=BeamEmitter10
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        BeamDistanceRange=(Min=900.000000,Max=930.000000)
        DetermineEndPointBy=PTEP_Distance
        BeamTextureUScale=2.000000
        LowFrequencyNoiseRange=(Y=(Min=-50.000000,Max=50.000000),Z=(Min=-20.000000,Max=100.000000))
        HighFrequencyNoiseRange=(Y=(Min=-30.000000,Max=30.000000),Z=(Min=-30.000000,Max=30.000000))
        UseColorScale=True
        RespawnDeadParticles=False
        AutomaticInitialSpawning=False
        ColorScale(0)=(Color=(B=255,G=255,R=255))
        ColorScale(1)=(RelativeTime=1.000000)
        ColorMultiplierRange=(Y=(Min=0.300000,Max=0.700000))
        MaxParticles=20
	UseRotationFrom=PTRS_Actor
        StartLocationOffset=(X=240.000000,Y=-30.000000,Z=220.000000)
        StartLocationRange=(Y=(Min=-50.000000,Max=50.000000),Z=(Min=-50.000000,Max=50.000000))
        StartSizeRange=(X=(Min=10.000000,Max=30.000000))
        InitialParticlesPerSecond=30.000000
        Texture=EpicParticles.Beams.HotBolt03aw
        LifetimeRange=(Min=0.100000,Max=0.200000)
        InitialDelayRange=(Min=0.300000,Max=0.300000)
        StartVelocityRange=(X=(Min=1.000000,Max=1.000000),Z=(Min=-0.300000,Max=-0.300000))
        Name="BeamEmitter10"
    End Object
    Emitters(6)=BeamEmitter'BeamEmitter10'

    Begin Object Class=MeshEmitter Name=MeshEmitter0
		AutoDestroy=true
		SecondsBeforeInactive=0.000000
        StaticMesh=AW-2004Particles.Weapons.PlasmaSphere
        UseParticleColor=True
        UseColorScale=True
        RespawnDeadParticles=False
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        ColorScale(0)=(Color=(B=255,G=255,R=255))
        ColorScale(1)=(RelativeTime=0.700000,Color=(B=255,G=255,R=255))
        ColorScale(2)=(RelativeTime=1.000000)
        Opacity=0.500000
        MaxParticles=1
	UseRotationFrom=PTRS_Actor
        StartLocationOffset=(Z=115.000000)
        SizeScale(0)=(RelativeSize=0.500000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=10.000000)
        StartSizeRange=(X=(Min=2.000000,Max=2.000000))
        InitialParticlesPerSecond=5000.000000
        LifetimeRange=(Min=0.700000,Max=0.700000)
        Name="MeshEmitter0"
    End Object
    Emitters(7)=MeshEmitter'MeshEmitter0'

    AmbientGlow=254
    bDirectional=true
    bNoDelete=false
    AutoDestroy=true
	RemoteRole=ROLE_None
}
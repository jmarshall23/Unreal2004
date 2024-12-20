//=============================================================================
// FX_ExplodingBarrel
//=============================================================================
// Created by Laurent Delayen
// � 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FX_ExplodingBarrel extends Emitter
	notplaceable;

#exec OBJ LOAD FILE=..\Sounds\WeaponSounds.uax


defaultproperties
{
    Begin Object Class=MeshEmitter Name=MeshEmitter2
        StaticMesh=StaticMesh'AS_Decos.ExplodingBarrel'
        UseCollision=True
		DampingFactorRange=(X=(Min=0.500000,Max=0.500000),Y=(Min=0.500000,Max=0.500000),Z=(Min=0.500000,Max=0.500000))
        RespawnDeadParticles=False
		AutoDestroy=true
        SpinParticles=True
        AutomaticInitialSpawning=False
        Acceleration=(Z=-980.000000)
        MaxParticles=1
        //Name="barrel"
        RevolutionsPerSecondRange=(Y=(Max=-0.150000))
        SpinsPerSecondRange=(Y=(Min=0.200000,Max=1.000000))
        InitialParticlesPerSecond=9000.000000
        DrawStyle=PTDS_Regular
        LifetimeRange=(Min=2.000000,Max=2.000000)
		SecondsBeforeInactive=0
        StartVelocityRange=(X=(Min=-500.000000,Max=500.000000),Y=(Min=-500.000000,Max=500.000000),Z=(Min=1500.000000,Max=2000.000000))
        Name="MeshEmitter2"
    End Object
    Emitters(0)=MeshEmitter'MeshEmitter2'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter98
        RespawnDeadParticles=False
		AutoDestroy=true
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        Opacity=0.670000
        MaxParticles=30
        //Name="barrel fire"
        AddLocationFromOtherEmitter=0
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Max=64.000000)
        StartSpinRange=(X=(Max=1.000000))
        SizeScale(0)=(RelativeSize=1.000000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=2.000000)
        StartSizeRange=(X=(Min=25.000000))
        Sounds(0)=(Sound=Sound'WeaponSounds.BaseImpactAndExplosions.BExplosion2',Radius=(Min=512.000000,Max=512.000000),Pitch=(Min=0.700000,Max=1.000000),Volume=(Min=2.000000,Max=2.000000),Probability=(Min=0.200000,Max=0.200000))
        Sounds(1)=(Sound=Sound'WeaponSounds.BaseImpactAndExplosions.BExplosion3',Radius=(Min=512.000000,Max=512.000000),Pitch=(Min=1.000000,Max=1.500000),Volume=(Min=2.000000,Max=2.000000),Probability=(Min=0.200000,Max=0.200000))
        SpawningSound=PTSC_LinearLocal
        SpawningSoundProbability=(Min=1.000000,Max=1.000000)
        InitialParticlesPerSecond=15.000000
        DrawStyle=PTDS_Brighten
        Texture=Texture'ExplosionTex.Framed.exp1_frames'
        TextureUSubdivisions=2
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.500000,Max=1.000000)
		SecondsBeforeInactive=0
        Name="SpriteEmitter98"
    End Object
    Emitters(1)=SpriteEmitter'SpriteEmitter98'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter99
        RespawnDeadParticles=False
		AutoDestroy=true
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        MaxParticles=5
        //Name="barrel explosion"
        AddLocationFromOtherEmitter=0
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Min=64.000000,Max=128.000000)
        StartSpinRange=(X=(Max=1.000000))
        SizeScale(0)=(RelativeSize=1.000000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=3.000000)
        StartSizeRange=(X=(Max=200.000000))
        Sounds(0)=(Sound=Sound'WeaponSounds.BaseImpactAndExplosions.BExplosion1',Radius=(Min=512.000000,Max=512.000000),Pitch=(Min=1.000000,Max=1.000000),Volume=(Min=4.000000,Max=4.000000),Probability=(Min=0.500000,Max=0.500000))
        Sounds(1)=(Sound=Sound'WeaponSounds.BaseImpactAndExplosions.BExplosion5',Radius=(Min=512.000000,Max=512.000000),Pitch=(Min=1.000000,Max=1.000000),Volume=(Min=4.000000,Max=4.000000),Probability=(Min=0.500000,Max=0.500000))
        SpawningSound=PTSC_LinearLocal
        SpawningSoundProbability=(Min=1.000000,Max=1.000000)
        InitialParticlesPerSecond=25.000000
        DrawStyle=PTDS_Brighten
        Texture=Texture'ExplosionTex.Framed.exp2_frames'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.750000,Max=1.000000)
		SecondsBeforeInactive=0
        InitialDelayRange=(Min=2.000000,Max=2.000000)
        Name="SpriteEmitter99"
    End Object
    Emitters(2)=SpriteEmitter'SpriteEmitter99'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter100
        UseColorScale=True
        RespawnDeadParticles=False
		AutoDestroy=true
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        UseRandomSubdivision=True
        ColorScale(1)=(RelativeTime=0.250000,Color=(B=255,G=255,R=255))
        ColorScale(2)=(RelativeTime=0.670000,Color=(B=255,G=255,R=255))
        ColorScale(3)=(RelativeTime=1.000000)
        MaxParticles=15
        //Name="barrel black smoke"
        AddLocationFromOtherEmitter=0
        SpinsPerSecondRange=(X=(Min=0.100000,Max=0.100000))
        StartSpinRange=(X=(Max=1.000000))
        SizeScale(0)=(RelativeSize=2.000000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=4.000000)
        StartSizeRange=(X=(Min=25.000000,Max=50.000000))
        InitialParticlesPerSecond=10.000000
        DrawStyle=PTDS_Darken
        Texture=Texture'AW-2004Particles.Fire.MuchSmoke2t'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=1.500000,Max=1.500000)
		SecondsBeforeInactive=0
        Name="SpriteEmitter100"
    End Object
    Emitters(3)=SpriteEmitter'SpriteEmitter100'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter101
        UseCollision=True
		UseMaxCollisions=True
        DampingFactorRange=(X=(Min=0.500000,Max=0.500000),Y=(Min=0.500000,Max=0.500000),Z=(Min=0.500000,Max=0.500000))
        MaxCollisions=(Min=1.000000,Max=2.000000)
        FadeOut=True
        RespawnDeadParticles=False
		AutoDestroy=true
        SpinParticles=True
        UniformSize=True
        AutomaticInitialSpawning=False
        UseRandomSubdivision=True
        Acceleration=(Z=-950.000000)
        FadeOutStartTime=1.000000
        MaxParticles=20
        //Name="sharpnels"
        AddLocationFromOtherEmitter=0
        RevolutionsPerSecondRange=(Z=(Min=-0.500000,Max=0.500000))
        SpinsPerSecondRange=(X=(Min=0.500000,Max=1.000000))
        StartSpinRange=(X=(Max=1.000000))
        StartSizeRange=(X=(Min=10.000000,Max=50.000000))
        InitialParticlesPerSecond=500.000000
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'VMParticleTextures.VehicleExplosions.GENERICshrapnelTEX'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=2.000000,Max=2.000000)
		SecondsBeforeInactive=0
        InitialDelayRange=(Min=2.000000,Max=2.000000)
        StartVelocityRange=(X=(Min=-4000.000000,Max=4000.000000),Y=(Min=-4000.000000,Max=4000.000000),Z=(Min=-4000.000000,Max=4000.000000))
        VelocityLossRange=(X=(Min=0.900000,Max=0.900000),Y=(Min=0.900000,Max=0.900000))
        Name="SpriteEmitter101"
    End Object
    Emitters(4)=SpriteEmitter'SpriteEmitter101'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter102
        UseColorScale=True
        RespawnDeadParticles=False
		AutoDestroy=true
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        UseRandomSubdivision=True
        ColorScale(0)=(Color=(A=255))
        ColorScale(1)=(RelativeTime=0.500000,Color=(A=255))
        ColorScale(2)=(RelativeTime=1.000000)
        MaxParticles=150
        //Name="sharpnel black smoke"
        AddLocationFromOtherEmitter=4
        SpinsPerSecondRange=(X=(Min=0.010000,Max=0.100000))
        StartSpinRange=(X=(Max=1.000000))
        SizeScale(0)=(RelativeSize=1.000000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=3.000000)
        StartSizeRange=(X=(Min=33.000000,Max=67.000000))
        InitialParticlesPerSecond=400.000000
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'AW-2004Particles.Weapons.SmokePanels2'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=1.000000,Max=2.000000)
		SecondsBeforeInactive=0
        InitialDelayRange=(Min=2.100000,Max=2.100000)
        Name="SpriteEmitter102"
    End Object
    Emitters(5)=SpriteEmitter'SpriteEmitter102'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter103
        RespawnDeadParticles=False
		AutoDestroy=true
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        //Name="base explosion"
        StartLocationOffset=(Z=32.000000)
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Min=32.000000,Max=128.000000)
        StartSpinRange=(X=(Max=1.000000))
        SizeScale(0)=(RelativeSize=1.000000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=2.000000)
        StartSizeRange=(X=(Min=50.000000,Max=150.000000))
        Sounds(0)=(Sound=Sound'WeaponSounds.BaseImpactAndExplosions.BExplosion1',Radius=(Min=512.000000,Max=512.000000),Pitch=(Min=0.500000,Max=1.000000),Volume=(Min=2.000000,Max=2.000000),Probability=(Min=1.000000,Max=1.000000))
        Sounds(1)=(Sound=Sound'WeaponSounds.BaseImpactAndExplosions.BExplosion5',Radius=(Min=512.000000,Max=512.000000),Pitch=(Min=1.000000,Max=2.000000),Volume=(Min=2.000000,Max=2.000000),Probability=(Min=0.500000,Max=0.500000))
        SpawningSound=PTSC_LinearLocal
        SpawningSoundProbability=(Min=1.000000,Max=1.000000)
        InitialParticlesPerSecond=20.000000
        DrawStyle=PTDS_Brighten
        Texture=Texture'ExplosionTex.Framed.exp7_frames'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.500000,Max=1.000000)
		SecondsBeforeInactive=0
        Name="SpriteEmitter103"
    End Object
    Emitters(6)=SpriteEmitter'SpriteEmitter103'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter104
        UseColorScale=True
        RespawnDeadParticles=False
		AutoDestroy=true
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        Acceleration=(Z=-980.000000)
        ColorScale(0)=(Color=(G=108,R=217))
        ColorScale(1)=(RelativeTime=0.500000,Color=(B=79,G=198,R=255))
        ColorScale(2)=(RelativeTime=1.000000,Color=(B=64,G=128,R=255))
        ColorScaleRepeats=8.000000
        Opacity=0.500000
        CoordinateSystem=PTCS_Relative
        MaxParticles=1
        //Name="barrel flare"
        SizeScale(0)=(RelativeSize=0.500000)
        SizeScale(1)=(RelativeTime=0.600000,RelativeSize=1.000000)
        SizeScale(2)=(RelativeTime=1.000000,RelativeSize=0.500000)
        SizeScaleRepeats=8.000000
        StartSizeRange=(X=(Min=350.000000,Max=350.000000))
        InitialParticlesPerSecond=9000.000000
        Texture=Texture'EpicParticles.Flares.SoftFlare'
        LifetimeRange=(Min=2.000000,Max=2.000000)
		SecondsBeforeInactive=0
        AddVelocityFromOtherEmitter=0
        Name="SpriteEmitter104"
    End Object
    Emitters(7)=SpriteEmitter'SpriteEmitter104'

    bNoDelete=false
    AutoDestroy=true
	RemoteRole=Role_DumbProxy
}
class NewMinigunMFlash extends Emitter;

#exec OBJ LOAD FILE="..\Textures\AW-2004Particles.utx"

defaultproperties
{
    Begin Object Class=SpriteEmitter Name=SpriteEmitter4
        RespawnDeadParticles=False
        SpinParticles=True
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        CoordinateSystem=PTCS_Relative
        MaxParticles=3
        StartLocationOffset=(X=8.000000)
        StartSpinRange=(X=(Max=1.000000))
        StartSizeRange=(X=(Min=15.000000,Max=20.000000))
        InitialParticlesPerSecond=5000.000000
        Texture=AW-2004Explosions.Fire.Part_explode2s
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.100000,Max=0.100000)
        Name="SpriteEmitter4"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter4'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter5
        RespawnDeadParticles=False
        SpinParticles=True
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        CoordinateSystem=PTCS_Relative
        MaxParticles=3
        StartLocationOffset=(X=16.000000)
        StartSpinRange=(X=(Max=1.000000))
        StartSizeRange=(X=(Min=10.000000,Max=15.000000))
        InitialParticlesPerSecond=5000.000000
        Texture=AW-2004Explosions.Fire.Part_explode2s
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.100000,Max=0.100000)
        Name="SpriteEmitter5"
    End Object
    Emitters(1)=SpriteEmitter'SpriteEmitter5'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter6
        RespawnDeadParticles=False
        SpinParticles=True
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        CoordinateSystem=PTCS_Relative
        MaxParticles=3
        StartLocationOffset=(X=24.000000)
        StartLocationRange=(X=(Max=8.000000))
        StartSpinRange=(X=(Max=1.000000))
        StartSizeRange=(X=(Min=5.000000,Max=10.000000))
        InitialParticlesPerSecond=5000.000000
        Texture=AW-2004Explosions.Fire.Part_explode2s
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.100000,Max=0.100000)
        Name="SpriteEmitter6"
    End Object
    Emitters(2)=SpriteEmitter'SpriteEmitter6'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter7
        RespawnDeadParticles=False
        SpinParticles=True
        UniformSize=True
        AutomaticInitialSpawning=False
        BlendBetweenSubdivisions=True
        CoordinateSystem=PTCS_Relative
        MaxParticles=4
        StartLocationOffset=(X=32.000000)
        StartLocationRange=(X=(Max=8.000000))
        StartSpinRange=(X=(Max=1.000000))
        StartSizeRange=(X=(Min=2.500000,Max=5.000000))
        InitialParticlesPerSecond=5000.000000
        Texture=AW-2004Explosions.Fire.Part_explode2s
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=0.100000,Max=0.100000)
        Name="SpriteEmitter7"
    End Object
    Emitters(3)=SpriteEmitter'SpriteEmitter7'
    
	bHardAttach=true
    bNoDelete=false
	bBlockActors=false
	bCollideActors=false
	RemoteRole=ROLE_None
	Physics=PHYS_None
}
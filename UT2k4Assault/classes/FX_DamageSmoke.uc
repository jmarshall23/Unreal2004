// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class FX_DamageSmoke extends Emitter;

#exec OBJ LOAD FILE=..\Textures\AW-2004Particles.utx
#exec OBJ LOAD FILE=..\Textures\EpicParticles.utx

defaultproperties
{
	bNoDelete=false
	bBlockActors=False
	RemoteRole=ROLE_None
	Physics=PHYS_None
	bHardAttach=True

	AutoDestroy=1
	AutoReset=0

    Begin Object Class=SpriteEmitter Name=SpriteEmitter0
        UseColorScale=True
        SpinParticles=True
        UseSizeScale=True
        UseRegularSizeScale=False
        UniformSize=True
        AutomaticInitialSpawning=False
        UseRandomSubdivision=True
        AddVelocityFromOwner=True
        Acceleration=(Z=120.000000)
        ColorScale(1)=(RelativeTime=0.200000,Color=(B=128,G=128,R=128,A=255))
        ColorScale(2)=(RelativeTime=0.800000,Color=(B=64,G=64,R=64,A=255))
        ColorScale(3)=(RelativeTime=1.000000,Color=(B=64,G=64,R=64))
        MaxParticles=15
        StartSpinRange=(X=(Min=0.250000,Max=0.750000))
        SizeScale(0)=(RelativeSize=0.100000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=1.000000)
        StartSizeRange=(X=(Min=50.000000,Max=60.000000))
        ParticlesPerSecond=7.500000
        InitialParticlesPerSecond=7.500000
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'AW-2004Particles.Fire.MuchSmoke1'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        LifetimeRange=(Min=2.000000,Max=2.000000)
        //VelocityLossRange=(X=(Min=1.000000,Max=2.000000),Y=(Min=1.000000,Max=2.000000))
        AddVelocityMultiplierRange=(X=(Min=1.0000,Max=1.0000),Y=(Min=1.0000,Max=1.0000),Z=(Min=0.5000000,Max=0.5000000))
		RespawnDeadParticles=False
		LowDetailFactor=1.0
        Name="SpriteEmitter0"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter0'

}
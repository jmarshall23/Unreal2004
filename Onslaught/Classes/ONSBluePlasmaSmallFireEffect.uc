//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSBluePlasmaSmallFireEffect extends Emitter;

simulated function PostNetBeginPlay()
{
	local PlayerController PC;
	
	Super.PostNetBeginPlay();
		
	PC = Level.GetLocalPlayerController();
	if ( (PC.ViewTarget == None) || (VSize(PC.ViewTarget.Location - Location) > 5000) )
		Emitters[2].Disabled = true;
}	

DefaultProperties
{
    Begin Object Class=SpriteEmitter Name=SpriteEmitter34
        UseDirectionAs=PTDU_Scale
        UseColorScale=True
        ColorScale(0)=(Color=(B=255,G=128,A=128))
        ColorScale(1)=(RelativeTime=0.800000,Color=(B=255,G=128))
        ColorScale(2)=(RelativeTime=1.000000,Color=(B=255,G=128,A=128))
        CoordinateSystem=PTCS_Relative
        MaxParticles=2
        StartLocationOffset=(X=92.500000)
        SpinParticles=True
        StartSpinRange=(X=(Max=1.000000))
        StartSizeRange=(X=(Min=35.000000,Max=50.000000))
        UniformSize=True
        InitialParticlesPerSecond=8000.000000
        Texture=Texture'AW-2004Particles.Weapons.PlasmaStar'
        LifetimeRange=(Min=0.200000,Max=0.200000)
        WarmupTicksPerSecond=2.000000
        RelativeWarmupTime=2.000000
        Name="SpriteEmitter34"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter34'
    Begin Object Class=SpriteEmitter Name=SpriteEmitter35
        UseDirectionAs=PTDU_Right
        ColorScale(0)=(Color=(B=255))
        ColorScale(1)=(RelativeTime=1.000000,Color=(B=255))
        CoordinateSystem=PTCS_Relative
        MaxParticles=1
        StartLocationOffset=(X=50.000000)
        StartSizeRange=(X=(Min=-75.000000,Max=-75.000000),Y=(Min=25.000000,Max=25.000000))
        InitialParticlesPerSecond=500.000000
        AutomaticInitialSpawning=False
        Texture=Texture'AW-2004Particles.Weapons.PlasmaHeadBlue'
        LifetimeRange=(Min=0.200000,Max=0.200000)
        StartVelocityRange=(X=(Max=10.000000))
        VelocityLossRange=(X=(Min=1.000000,Max=1.000000))
        WarmupTicksPerSecond=1.000000
        RelativeWarmupTime=2.000000
        Name="SpriteEmitter35"
    End Object
    Emitters(1)=SpriteEmitter'SpriteEmitter35'
    Begin Object Class=SpriteEmitter Name=SpriteEmitter36
        UseColorScale=True
        ColorScale(1)=(RelativeTime=0.100000,Color=(B=255,G=128))
        ColorScale(2)=(RelativeTime=0.500000,Color=(B=255,G=128))
        ColorScale(3)=(RelativeTime=1.000000)
        CoordinateSystem=PTCS_Relative
        MaxParticles=20
        StartLocationOffset=(X=75.000000)
        SpinParticles=True
        SpinsPerSecondRange=(X=(Max=0.200000))
        StartSpinRange=(X=(Max=1.000000))
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=1.000000)
        SizeScale(1)=(RelativeTime=1.000000)
        StartSizeRange=(X=(Min=12.500000,Max=17.500000))
        UniformSize=True
        Texture=Texture'AW-2004Particles.Weapons.SmokePanels1'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        DetailMode=DM_High
        BlendBetweenSubdivisions=True
        LifetimeRange=(Min=0.400000,Max=0.400000)
        StartVelocityRange=(X=(Min=-500.000000,Max=-500.000000))
        WarmupTicksPerSecond=1.000000
        RelativeWarmupTime=2.000000
        Name="SpriteEmitter36"
    End Object
    Emitters(2)=SpriteEmitter'SpriteEmitter36'
    bNoDelete=False
}

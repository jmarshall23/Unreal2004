//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSDirtSlipEffect extends Emitter;

#exec OBJ LOAD FILE=..\Sounds\ONSVehicleSounds-S.uax

var () int		MaxSpritePPS;
var () int		MaxMeshPPS;
var () sound    DirtSlipSound;

simulated function SetDirtColor(color DirtColor)
{
	local color DirtColorZeroAlpha, DirtColorHalfAlpha;

	// Ignore if dust color if black.
	if(DirtColor.R == 0 && DirtColor.G == 0 && DirtColor.B == 0)
		return;

	DirtColor.A = 255;

	DirtColorZeroAlpha = DirtColor;
	DirtColorZeroAlpha.A = 0;

	DirtColorHalfAlpha = DirtColor;
	DirtColorHalfAlpha.A = 128;

	Emitters[0].ColorScale[0].Color = DirtColorZeroAlpha;
	Emitters[0].ColorScale[1].Color = DirtColorHalfAlpha;
	Emitters[0].ColorScale[2].Color = DirtColorHalfAlpha;
	Emitters[0].ColorScale[3].Color = DirtColorZeroAlpha;
}

simulated function UpdateDust(SVehicleWheel t, float DustSlipRate, float DustSlipThresh)
{
	local float SpritePPS, MeshPPS;

	//Log("Material:"$t.GroundMaterial$" OnGround:"$t.bTireOnGround);

	// If wheel is on ground, and slipping above threshold..
	if(t.bWheelOnGround && t.SlipVel > DustSlipThresh)
	{
		SpritePPS = FMin(DustSlipRate * (t.SlipVel - DustSlipThresh), MaxSpritePPS);

		Emitters[0].ParticlesPerSecond = SpritePPS;
		Emitters[0].InitialParticlesPerSecond = SpritePPS;
		Emitters[0].AllParticlesDead = false;

		MeshPPS = FMin(DustSlipRate * (t.SlipVel - DustSlipThresh), MaxMeshPPS);

		Emitters[1].ParticlesPerSecond = MeshPPS;
		Emitters[1].InitialParticlesPerSecond = MeshPPS;
		Emitters[1].AllParticlesDead = false;

		AmbientSound = DirtSlipSound;
	}
	else // ..otherwise, switch off.
	{
		Emitters[0].ParticlesPerSecond = 0;
		Emitters[0].InitialParticlesPerSecond = 0;

		Emitters[1].ParticlesPerSecond = 0;
		Emitters[1].InitialParticlesPerSecond = 0;

		AmbientSound = None;
	}
}

defaultproperties
{
	bNoDelete=false
	MaxSpritePPS=70
	MaxMeshPPS=70
	DirtSlipSound=sound'ONSVehicleSounds-S.PRV.DirtSkid03'
	SoundVolume=40
	SoundRadius=64
	CullDistance=5000.0

    Begin Object Class=SpriteEmitter Name=SpriteEmitter1
        Acceleration=(Z=15.000000)
        UseColorScale=True
        ColorScale(0)=(Color=(B=30,G=90,R=110))
        ColorScale(1)=(RelativeTime=0.100000,Color=(B=34,G=44,R=66,A=128))
        ColorScale(2)=(RelativeTime=0.900000,Color=(B=36,G=49,R=64,A=128))
        ColorScale(3)=(RelativeTime=1.000000,Color=(B=20,G=50,R=80))
        MaxParticles=100
        StartLocationRange=(Y=(Min=-24.000000,Max=24.000000))
        UseRotationFrom=PTRS_Actor
        SpinParticles=True
        SpinsPerSecondRange=(X=(Max=0.025000))
        StartSpinRange=(X=(Max=1.000000))
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=0.250000)
        SizeScale(1)=(RelativeTime=0.500000,RelativeSize=1.000000)
        SizeScale(2)=(RelativeTime=0.900000,RelativeSize=2.000000)
        StartSizeRange=(X=(Min=50.000000,Max=50.000000),Y=(Min=50.000000,Max=50.000000),Z=(Min=50.000000,Max=50.000000))
        UniformSize=True
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'VehicleFX.Particles.DustyCloud2'
        LifetimeRange=(Min=0.500000,Max=1.100000)
		AutomaticInitialSpawning=False
		InitialParticlesPerSecond=0
		ParticlesPersecond=0
		RespawnDeadParticles=false
		SecondsBeforeInactive=0
        Name="SpriteEmitter1"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter1'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter0
        Acceleration=(Z=-550.000000)
        FadeOutStartTime=0.500000
        FadeOut=True
        MaxParticles=150
        StartLocationRange=(X=(Min=-10.000000,Max=10.000000),Y=(Min=-10.000000,Max=10.000000),Z=(Min=-10.000000,Max=10.000000))
        UseRotationFrom=PTRS_Actor
        SpinParticles=True
        SpinsPerSecondRange=(X=(Min=0.500000,Max=2.000000))
        StartSizeRange=(X=(Min=2.000000,Max=8.000000))
        UniformSize=True
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'VMParticleTextures.DirtKICKGROUP.dirtKICKTEX'
        TextureUSubdivisions=16
        TextureVSubdivisions=16
        BlendBetweenSubdivisions=True
        UseRandomSubdivision=True
		AutomaticInitialSpawning=False
		InitialParticlesPerSecond=0
		ParticlesPersecond=0
		RespawnDeadParticles=false
		SecondsBeforeInactive=0
        LifetimeRange=(Min=0.650000,Max=0.650000)
        StartVelocityRange=(X=(Min=-150.000000,Max=-250.000000),Y=(Min=-80.000000,Max=80.000000),Z=(Min=50.000000,Max=250.000000))
        Name="SpriteEmitter0"
    End Object
    Emitters(1)=SpriteEmitter'SpriteEmitter0'


	bBlockActors=False
	RemoteRole=ROLE_None
	Physics=PHYS_None
	bHardAttach=True
}

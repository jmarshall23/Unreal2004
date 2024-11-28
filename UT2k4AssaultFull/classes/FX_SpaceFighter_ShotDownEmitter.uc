class FX_SpaceFighter_ShotDownEmitter extends Emitter
	notplaceable;

#exec OBJ LOAD FILE=EpicParticles.utx
#exec OBJ LOAD FILE=ExplosionTex.utx


defaultproperties
{
    Begin Object Class=SpriteEmitter Name=SpriteEmitter18
        UseColorScale=True
        ColorScale(0)=(Color=(B=176,G=223,R=255,A=255))
        ColorScale(1)=(RelativeTime=0.150000,Color=(B=32,G=166,R=255,A=255))
        ColorScale(2)=(RelativeTime=0.300000,Color=(B=108,G=146,R=183,A=200))
        ColorScale(3)=(RelativeTime=0.750000,Color=(B=80,G=80,R=80,A=128))
        ColorScale(4)=(RelativeTime=1.000000)
        MaxParticles=30
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Min=10.000000,Max=20.000000)
        SpinParticles=True
        StartSpinRange=(X=(Max=1.000000))
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=1.500000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=7.000000)
        StartSizeRange=(X=(Min=20.000000,Max=40.000000))
        UniformSize=True
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'ExplosionTex.Framed.exp2_frames'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        BlendBetweenSubdivisions=True
		SecondsBeforeInactive=0.000000
        LifetimeRange=(Min=1.000000,Max=1.000000)
        Name="SpriteEmitter18"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter18'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter19
        UseColorScale=True
        ColorScale(0)=(Color=(B=200,G=255,R=255,A=255))
        ColorScale(1)=(RelativeTime=0.250000,Color=(B=74,G=169,R=255,A=160))
        ColorScale(2)=(RelativeTime=0.600000,Color=(B=108,G=146,R=183,A=64))
        ColorScale(3)=(RelativeTime=1.000000,Color=(B=80,G=80,R=80))
        MaxParticles=60
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Max=12.000000)
        SpinParticles=True
        StartSpinRange=(X=(Max=1.000000))
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=1.000000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=3.000000)
        StartSizeRange=(X=(Min=15.000000,Max=30.000000))
        UniformSize=True
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'EpicParticles.Smoke.Smokepuff'
		SecondsBeforeInactive=0.000000
        LifetimeRange=(Min=0.400000,Max=0.400000)
        Name="SpriteEmitter19"
    End Object
    Emitters(1)=SpriteEmitter'SpriteEmitter19'

    Begin Object Class=SpriteEmitter Name=SpriteEmitter20
        FadeOutStartTime=0.500000
        FadeOut=True
        CoordinateSystem=PTCS_Relative
        MaxParticles=20
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Min=18.000000,Max=24.000000)
        SpinParticles=True
        SpinsPerSecondRange=(X=(Max=0.100000))
        StartSpinRange=(X=(Max=1.000000))
        StartSizeRange=(X=(Min=2.000000,Max=20.000000))
        UniformSize=True
        Texture=Texture'ExplosionTex.Framed.exp7_frames'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        BlendBetweenSubdivisions=True
		SecondsBeforeInactive=0.000000
        LifetimeRange=(Min=0.500000,Max=1.000000)
        StartVelocityRadialRange=(Max=25.000000)
        GetVelocityDirectionFrom=PTVD_AddRadial
        Name="SpriteEmitter20"
    End Object
    Emitters(2)=SpriteEmitter'SpriteEmitter20'

    AutoDestroy=true
	bStasis=false
    bDirectional=true
    bNoDelete=false
	RemoteRole=ROLE_None
    bHardAttach=true
}

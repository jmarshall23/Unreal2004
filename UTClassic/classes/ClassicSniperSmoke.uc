class ClassicSniperSmoke extends Emitter;

defaultproperties
{
    Begin Object Class=SpriteEmitter Name=SpriteEmitter0
        Acceleration=(Z=20.000000)
        UseColorScale=True
        MaxParticles=5
        StartLocationShape=PTLS_Sphere
        SphereRadiusRange=(Min=14.000000,Max=14.000000)
        RespawnDeadParticles=False
        SpinParticles=True
        RotationOffset=(Yaw=-16384)
        SpinsPerSecondRange=(X=(Max=0.100000))
        StartSpinRange=(X=(Max=1.000000))
        UseSizeScale=True
        UseRegularSizeScale=False
        SizeScale(0)=(RelativeSize=0.200000)
        SizeScale(1)=(RelativeTime=1.000000,RelativeSize=0.500000)
        UniformSize=True
        InitialParticlesPerSecond=500.000000
        AutomaticInitialSpawning=False
        DrawStyle=PTDS_AlphaBlend
        Texture=Texture'ExplosionTex.Framed.SmokeReOrdered'
        TextureUSubdivisions=4
        TextureVSubdivisions=4
        BlendBetweenSubdivisions=True
        UseRandomSubdivision=True
        LifetimeRange=(Min=2.500000,Max=3.00000)
        FadeOutStartTime=2.0000
        FadeOut=True
        Name="SpriteEmitter0"
    End Object
    Emitters(0)=SpriteEmitter'SpriteEmitter0'
   
    bNoDelete=false
    AutoDestroy=true
    RemoteRole=ROLE_None
}
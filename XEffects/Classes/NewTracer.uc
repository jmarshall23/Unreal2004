class NewTracer extends Emitter;

#exec OBJ LOAD FILE=..\Textures\AW-2004Particles.utx

simulated function SpawnParticle( int Amount )
{
	local PlayerController PC;
	local vector Dir, LineDir, LinePos, RealLocation;
	
	Super.SpawnParticle(Amount);
	
    if ( (Instigator == None) || Instigator.IsFirstPerson() )
		return;
   
	// see if local player controller near bullet, but missed
	PC = Level.GetLocalPlayerController();
	if ( (PC != None) && (PC.Pawn != None) )
	{
		Dir.X = Emitters[0].StartVelocityRange.X.Min;
		Dir.Y = Emitters[0].StartVelocityRange.Y.Min;
		Dir.Z = Emitters[0].StartVelocityRange.Z.Min;
		Dir = Normal(Dir);
		LinePos = (Location + (Dir dot (PC.Pawn.Location - Location)) * Dir);
		LineDir = PC.Pawn.Location - LinePos;
		if ( VSize(LineDir) < 150 )
		{
			RealLocation = Location;
			SetLocation(LinePos);
			if ( FRand() < 0.5 )
				PlaySound(sound'Impact3Snd',,,,80);
			else
				PlaySound(sound'Impact7Snd',,,,80);
			SetLocation(RealLocation);
		}
	}
}

defaultproperties
{
	Begin Object Class=SpriteEmitter Name=SpriteEmitter13
		UseDirectionAs=PTDU_Right
		UseSizeScale=True
		UseRegularSizeScale=False
		ScaleSizeXByVelocity=True
		ExtentMultiplier=(X=0.200000)
		ColorMultiplierRange=(Y=(Min=0.800000,Max=0.800000),Z=(Min=0.500000,Max=0.500000))
		MaxParticles=100
		UseRotationFrom=PTRS_None
		UseAbsoluteTimeForSizeScale=True
		SizeScale(1)=(RelativeTime=0.0300000,RelativeSize=1.000000)
		StartSizeRange=(X=(Min=20.000000,Max=20.000000),Y=(Min=10.000000,Max=10.000000))
		ScaleSizeByVelocityMultiplier=(X=0.002000)
		InitialParticlesPerSecond=0.000000
		ParticlesPerSecond=0.0
		RespawnDeadParticles=False
		AutomaticInitialSpawning=False
		Texture=AW-2004Particles.Weapons.TracerShot
		LifetimeRange=(Min=0.100000,Max=0.100000)
		StartVelocityRange=(X=(Min=10000.000000,Max=10000.000000),Y=(Min=0.000000,Max=0.000000),Z=(Min=0.000000,Max=0.000000))
		Name="SpriteEmitter13"
	End Object
	Emitters(0)=SpriteEmitter'SpriteEmitter13'

	bNoDelete=false
	bBlockActors=false
	bCollideActors=false
	RemoteRole=ROLE_None
	Physics=PHYS_None
	bHardAttach=True
}
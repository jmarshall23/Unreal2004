class BombTrail extends SpeedTrail;

simulated function PostBeginPlay()
{
	Super.PostBeginPlay();
	
	if ( Owner != None )
	{
		SetBase(Owner);
		SetRelativeLocation(vect(0,0,0));
	}	
}

DefaultProperties
{
	bHardAttach=true
}
class ClientMover Extends Mover;

function PostBeginPlay()
{
	Super.PostBeginPlay();
	
	if ( Level.NetMode == NM_DedicatedServer )
	{
		GotoState('ServerIdle');
		SetTimer(0,false);
		SetPhysics(PHYS_None);
	}
}

State ServerIdle
{
}

defaultproperties
{
	bAlwaysRelevant=false
	RemoteRole=ROLE_None
	bClientAuthoritative=true
}

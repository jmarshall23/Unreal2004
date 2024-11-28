class UTJumpPad extends JumpPad
	placeable;


event Touch(Actor Other)
{
	if ( (UnrealPawn(Other) == None) || (Other.Physics == PHYS_None) )
		return;

	PendingTouch = Other.PendingTouch;
	Other.PendingTouch = self;
}

event PostTouch(Actor Other)
{
	local Pawn P;
	local Bot B;

	Super.PostTouch(Other);

	P = UnrealPawn(Other);
	if ( P == None )
		return;

	B = Bot(P.Controller);	
	if ( (B != None) && (PhysicsVolume.Gravity.Z > PhysicsVolume.Default.Gravity.Z) )
		B.Focus = B.FaceActor(2);
}	

/*
defaultproperties
{
	JumpSound=sound'
}
*/
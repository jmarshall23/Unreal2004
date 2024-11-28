class RestingFormation extends Info;

var Bot Occupant[16];
var vector Offset[16];
var vector LookDir[16];
var float FormationSize;

function LeaveFormation(Bot B)
{
	if ( Occupant[B.FormationPosition] == B )
		Occupant[B.FormationPosition] = None;
}

function bool SetFormation(Bot B, Int Pos, bool bFullCheck)
{
	local vector HitLocation, HitNormal;
	local actor HitActor, Center;

	Center = SquadAI(Owner).FormationCenter();
	if ( Center == None )
		Center = B.Pawn;
	if ( (Occupant[Pos] == None)
		|| !Occupant[Pos].Formation() )
	{
		if ( bFullCheck )
		{
			// FIXME - check if valid position, with traces
			HitActor = Trace(HitLocation, HitNormal,Center.Location, GetLocationFor(Pos,B),false);
			if ( (HitActor != None) && (HitNormal.Z < MINFLOORZ) )
				return false;
		}
		LeaveFormation(B);
		Occupant[Pos] = B;
		return true;
	}
}

function int RecommendPositionFor(Bot B)
{
	local int i;
	
	i = Rand(15);
	if ( SetFormation(B,i,true) )
		return i;
	for ( i=0; i<16; i++ )
		if ( SetFormation(B,i,true) )
			return i;
	for ( i=0; i<16; i++ )
		if ( SetFormation(B,i,false) )
			return i;
	return Rand(15);
}

function vector GetLocationFor(int Pos, Bot B)
{
	local vector Loc,X,Y,Z;
	local actor Center;

	Center = SquadAI(Owner).FormationCenter();
	if ( Center == None )
		Center = B.Pawn;

	GetAxes(SquadAI(Owner).GetFacingRotation(),X,Y,Z);
	Loc = Center.Location + Offset[Pos].X * X + Offset[Pos].Y * Y;
	// FIXME adjust based on traces, try to make a legal destination
	return Loc;
}

function vector GetViewPointFor(Bot B,int Pos)
{
	local vector ViewPoint;
	local actor Center;
	local int i;
	local actor Best;
	
	if ( B.Pawn.Anchor != None )
	{
		For ( i=0; i<B.Pawn.Anchor.PathList.Length; i++ )
		{
			if ( (B.Pawn.Anchor.PathList[i] != None) && (VSize(B.Pawn.Location - B.Pawn.Anchor.PathList[i].End.Location) > 400)
				&& (FRand() > 0.3) && (VSize(B.FocalPoint -  B.Pawn.Anchor.PathList[i].End.Location) > 100)
				&& FastTrace(B.Pawn.Location, B.Pawn.Anchor.PathList[i].End.Location) )
				Best = B.Pawn.Anchor.PathList[i].End;
		}
	}
	
	if ( Best != None )
		return (Best.Location + 0.5 * B.Pawn.CollisionHeight * vect(0,0,1));
	
	if ( B.Pawn.bStationary || (Vehicle(B.Pawn) != None) )
	{
		ViewPoint = (B.Pawn.Location + 2000 * VRand());
		ViewPoint.Z = B.Pawn.Location.Z;
		if ( ((ViewPoint - B.Pawn.Location) Dot (B.FocalPoint - B.Pawn.Location)) > 0 )
			ViewPoint = 2 * B.Pawn.Location - ViewPoint;
		if ( !FastTrace(ViewPoint, B.Pawn.Location) )
			ViewPoint = B.FocalPoint;
	}
	else
	{
		Center = SquadAI(Owner).FormationCenter();
		if ( Center == None )
			return (B.Pawn.Location + 200*VRand());

		ViewPoint = 2 * B.Pawn.Location - Center.Location;
		ViewPoint.Z = B.Pawn.Location.Z;
	}

	return ViewPoint;
}

defaultproperties
{
	Offset(0)=(x=100.0,y=300.0,z=0.0)
	Offset(1)=(x=300.0,y=0.0,z=0.0)
	Offset(2)=(x=100.0,y=-300.0,z=0.0)
	Offset(3)=(x=-100.0,y=300.0,z=0.0)
	Offset(4)=(x=-100.0,y=-300.0,z=0.0)
	Offset(5)=(x=-400.0,y=0.0,z=0.0)
	Offset(6)=(x=-100.0,y=-150.0,z=0.0)
	Offset(7)=(x=-100.0,y=150.0,z=0.0)
	Offset(8)=(x=-400.0,y=-120.0,z=0.0)
	Offset(9)=(x=-400.0,y=120.0,z=0.0)
	Offset(10)=(x=-300.0,y=300.0,z=0.0)
	Offset(11)=(x=-300.0,y=-300.0,z=0.0)
	Offset(12)=(x=300.0,y=300.0,z=0.0)
	Offset(13)=(x=300.0,y=-300.0,z=0.0)
	Offset(14)=(x=-200.0,y=450.0,z=0.0)
	Offset(15)=(x=-200.0,y=-450.0,z=0.0)
	FormationSize=+750.0

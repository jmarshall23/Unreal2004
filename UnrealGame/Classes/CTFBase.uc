//=============================================================================
// CTFBase.
//=============================================================================
class CTFBase extends GameObjective
	abstract;

var() Sound TakenSound;
var CTFFlag myFlag;
var class<CTFFlag> FlagType;

function BeginPlay()
{
	Super.BeginPlay();
	bHidden = false;

	myFlag = Spawn(FlagType, self);

	if (myFlag==None)
	{
		warn(Self$" could not spawn flag of type '"$FlagType$"' at "$location);
		return;
	}
	else
	{
		myFlag.HomeBase = self;
		myFlag.TeamNum = DefenderTeamIndex;
	}
}

function PostBeginPlay()
{
	local UnrealScriptedSequence W;
//	local NavigationPoint N;

	Super.PostBeginPlay();

	//calculate distance from this base to all nodes - store in BaseDist[DefenderTeamIndex] for each node
	SetBaseDistance(DefenderTeamIndex);

	// calculate visibility to base and defensepoints, for all paths
	SetBaseVisibility(DefenderTeamIndex);
	for ( W=DefenseScripts; W!=None; W=W.NextScript )
		if ( W.myMarker != None )
			W.myMarker.SetBaseVisibility(DefenderTeamIndex);
}

function PlayAlarm()
{
	SetTimer(5.0, false);
	AmbientSound = TakenSound;
}

function Timer()
{
	AmbientSound = None;
}

defaultproperties
{
	bStatic=false
	bStasis=false
	bAlwaysRelevant=true
	SoundRadius=255
	SoundVolume=255
    DrawType=DT_Mesh
    DrawScale=1.300000
    CollisionRadius=60.000000
    CollisionHeight=60.000000
    bCollideActors=True
    bBlockActors=False
	NetUpdateFrequency=8
}

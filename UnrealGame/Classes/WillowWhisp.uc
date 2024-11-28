class WillowWhisp extends xEmitter;

var		vector		WayPoints[11];
var		int			NumPoints;
var		int			Position;
var		vector		Destination;
var		bool		bHeadedRight;
var		float		LifeLeft;

replication
{
	reliable if ( Role == ROLE_Authority )
		NumPoints,WayPoints;
}

function PostBeginPlay()
{
	local int i,start;
	local Controller C;
	local Actor HitActor;
	local Vector HitLocation,HitNormal;
	
	Super.PostBeginPlay();
	
	C = Controller(Owner);
	if ( C.Pawn == None )
		return;
	SetLocation(C.Pawn.Location);

	WayPoints[0] = C.Pawn.Location + C.Pawn.CollisionHeight * Vect(0,0,1) + 200 * vector(C.Rotation);
	HitActor = Trace(HitLocation, HitNormal,WayPoints[0], C.Pawn.Location,false);
	if ( HitActor != None )
		WayPoints[0] = HitLocation;
	NumPoints++;
	
	if ( (C.RouteCache[i] != None) && C.ActorReachable(C.RouteCache[1]) )
		start = 1;
	for ( i=start; i<start+10; i++ )
	{
		if ( C.RouteCache[i] == None )
			break;
		else
		{
			WayPoints[NumPoints] = C.RouteCache[i].Location + C.Pawn.CollisionHeight * Vect(0,0,1);
			NumPoints++;
		}
	}
	Velocity = 500 * Normal(WayPoints[0] - Location) + C.Pawn.Velocity;
}

simulated function PostNetBeginPlay()
{
	if ( (Level.NetMode == NM_Standalone) || (Level.NetMode == NM_Client) )
	{
		bHidden = false;
		StartNextPath();
	}
	else if ( (Level.NetMode == NM_ListenServer) && (Viewport(PlayerController(Owner).Player) != None) )
	{
		bHidden = false;
		RemoteRole = ROLE_None;
		StartNextPath();
	}
	else
		LifeSpan = 0.5;
}

simulated function StartNextPath()
{
	if ( Position >= NumPoints )
	{
		mregen = false;
		LifeSpan = 1.5;
		LifeLeft = 1.5;
		Velocity = vect(0,0,0);
		Acceleration = vect(0,0,0);
		return;
	}
	bHeadedRight = false;
	Destination = WayPoints[Position];
	Acceleration = 1200 * Normal(Destination - Location);
	Velocity *= 0.5;
	Velocity.Z = 0.5 * (Velocity.Z + Acceleration.Z);
	SetRotation(rotator(Acceleration));
	Position++;
}

auto state Pathing
{
	simulated function Tick(float DeltaTime)
	{
		if ( LifeLeft > 0 )
		{
			LifeLeft -= DeltaTime;
			if ( LifeLeft <= 0 )
			{
				Destroy();
				return;
			}
			return;
		}
		Acceleration = 1200 * Normal(Destination - Location);
		Velocity = Velocity + DeltaTime * Acceleration; // force double acceleration
		if ( !bHeadedRight )
			bHeadedRight = ( (Velocity Dot Acceleration) > 0 );
		else if ( Velocity Dot Acceleration < 0 )
			StartNextPath();
		if ( VSize(Destination - Location) < 80 )
			StartNextPath();
	}
}

defaultproperties
{
	LifeSpan=10
	bStasis=false
	bIgnoreOutOfWorld=true
	bCollideWorld=false
	bCollideActors=false
	physics=PHYS_Projectile
	bOnlyOwnerSee=false
	bHidden=true
	bNoDelete=false
	bNetTemporary=true
	RemoteRole=ROLE_SimulatedProxy
	
    Skins(0)=Texture'S_Pawn'
    Style=STY_Alpha
    mParticleType=PT_Sprite 
    mLifeRange(0)=1.25
    mLifeRange(1)=1.25
    mSpeedRange(0)=0.0
    mSpeedRange(1)=0.0
    mSpinRange(0)=-75.0
    mSpinRange(1)=75.0
    mSizeRange(0)=15.0
    mSizeRange(1)=20.0
    mRegenRange(0)=90.0
    mRegenRange(1)=90.0
    mRandOrient=True
    mRandTextures=True
    mStartParticles=0
    mMaxParticles=150
    mGrowthRate=13.0
    mAttenuate=True
    mAttenFunc=ATF_ExpInOut
    mAttenKa=0.2
    mRegen=True
    mNumTileColumns=1
    mNumTileRows=1
    mMassRange(0)=-0.03
    mMassRange(1)=-0.01
    mColorRange(1)=(G=210,B=210)
}
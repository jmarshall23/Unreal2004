class TransbeaconTeleporter extends Keypoint;

var() name JumpSpotTag;
var JumpSpot myJumpSpot;

function PostBeginPlay()
{
	super.PostBeginPlay();
	
	if ( JumpSpotTag != '' )
		ForEach AllActors(class'JumpSpot',myJumpSpot,JumpSpotTag)
			break;
}
	
event Touch(Actor Other)
{
	if ( (TransBeacon(Other) == None) || (myJumpSpot == None) )
		return;
		
	Other.SetLocation(myJumpSpot.Location);
	if ( TransBeacon(Other).Trail != None )
	    TransBeacon(Other).Trail.mRegen = false;
}
		
defaultproperties
{
	 bCollideActors=true
     bStatic=True
     bHidden=True
     SoundVolume=0
     CollisionRadius=+00030.000000
     CollisionHeight=+00030.000000
	 Texture=S_Keypoint
}
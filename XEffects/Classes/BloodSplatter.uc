class BloodSplatter extends xScorch;

var texture Splats[3];

simulated function PostBeginPlay()
{
    ProjTexture = splats[Rand(3)];
    Super.PostBeginPlay();
}

defaultproperties
{
	LifeSpan=5
	splats(0)=Texture'BloodSplat1' 
	splats(1)=Texture'BloodSplat2'
	splats(2)=Texture'BloodSplat3'
	ProjTexture=Texture'BloodSplat1'
	RemoteRole=ROLE_None
    FOV=6
    bClipStaticMesh=True
    CullDistance=+7000.0
}

class BloodSplatterPurple extends xScorch;

var texture Splats[3];

simulated function PostBeginPlay()
{
    ProjTexture = splats[Rand(3)];
    Super.PostBeginPlay();
}

defaultproperties
{
	LifeSpan=5
	splats(0)=Texture'BloodSplat1P' 
	splats(1)=Texture'BloodSplat2P'
	splats(2)=Texture'BloodSplat3P'
	ProjTexture=Texture'BloodSplat1P'
	RemoteRole=ROLE_None
    FOV=6
    bClipStaticMesh=True
    CullDistance=+7000.0
}

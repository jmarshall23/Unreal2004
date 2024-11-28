class BulletDecal extends xScorch;

#exec TEXTURE IMPORT NAME=bulletpock FILE=TEXTURES\pock.tga LODSET=2 MODULATED=1 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

function PostBeginPlay()
{
	if ( FRand() < 0.75 )
		LifeSpan *= 0.5;
	Super.PostBeginPlay();
}

defaultproperties
{
	LifeSpan=3.2
	bHighDetail=true
	DrawScale=+0.18
	ProjTexture=Material'bulletpock'
    RandomOrient=false
	bClipStaticMesh=True
	CullDistance=+3000.0
}
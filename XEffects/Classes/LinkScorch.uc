class LinkScorch extends xScorch;

#exec TEXTURE IMPORT NAME=LBScorcht FILE=TEXTURES\DECALS\LinkBoltDecal.tga LODSET=2 Alpha=1 MODULATED=1 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

defaultproperties
{
	DrawScale=+0.4
	ProjTexture=Material'rocketblastmark'
    LifeSpan=1.0
    CullDistance=+4000.0
}

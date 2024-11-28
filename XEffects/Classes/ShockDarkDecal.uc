class ShockDarkDecal extends xScorch;

#exec TEXTURE IMPORT NAME=SDScorcht FILE=TEXTURES\DECALS\ShockDarkDecal.tga LODSET=2 MODULATED=1 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

defaultproperties
{
	DrawScale=+1.0
	ProjTexture=sdScorcht
    CullDistance=+7000.0
}

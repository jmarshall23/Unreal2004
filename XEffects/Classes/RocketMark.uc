class RocketMark extends xScorch;

#exec TEXTURE IMPORT NAME=rocketblastmark FILE=TEXTURES\DECALS\rocketBlasta.tga LODSET=2 MODULATED=1 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

defaultproperties
{
	DrawScale=+2.0
	ProjTexture=Material'rocketblastmark'
}

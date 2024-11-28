class FlakDirectionalBlast extends xScorch;

#exec TEXTURE IMPORT NAME=flaktdirectionalblast FILE=TEXTURES\DECALS\FlakShellBlast.tga LODSET=2 MODULATED=1 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

defaultproperties
{
	DrawScale=+1.7
	ProjTexture=texture'xEffects.flaktdirectionalblast'
}

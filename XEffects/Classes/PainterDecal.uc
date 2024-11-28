class PainterDecal extends xScorch;

#exec TEXTURE IMPORT NAME=PainterDecalMark FILE=TEXTURES\DECALS\BoltImpact.tga Alpha=1 LODSET=2 DXT=5 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

defaultproperties
{
	ScaleGlow=1.0
	STYLE=STY_Additive
	DrawScale=+1.0 
	ProjTexture=PainterDecalMark
    FrameBufferBlendingOp=PB_Add
	LifeSpan=1.5
}

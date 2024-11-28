class ShockImpactScorch extends xScorch;

#exec TEXTURE IMPORT NAME=ShockHeatDecal FILE=TEXTURES\DECALS\ShockHeatDecal.tga LODSET=2 ADDITIVE=1 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

defaultproperties
{
    FrameBufferBlendingOp=PB_Add
	DrawScale=+0.5
	ProjTexture=Texture'ShockHeatDecal'
    LifeSpan=2.2
    CullDistance=+7000.0
}

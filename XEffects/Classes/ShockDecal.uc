class ShockDecal extends xScorch;

#exec TEXTURE IMPORT NAME=ShockDecalMark FILE=TEXTURES\DECALS\ShockDecal.tga Alpha=1 LODSET=2 DXT=5 UCLAMPMODE=CLAMP VCLAMPMODE=CLAMP

defaultproperties
{
	ProjTexture=ShockDecalMark
    FrameBufferBlendingOp=PB_Add
    CullDistance=+7000.0
}

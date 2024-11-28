#exec OBJ LOAD FILE=VehicleFX.utx

class BulldogHeadlight extends DynamicProjector;

// Empty tick here - do detach/attach in Bulldog tick
function Tick(float Delta)
{

}

defaultproperties
{
	DrawScale=0.65
	bHidden=true
	FrameBufferBlendingOp=PB_Add
    ProjTexture=Texture'VehicleFX.Projected.BullHeadlights'
    FOV=30
    MaxTraceDistance=2048
    bProjectOnUnlit=True
    bGradient=True
    bProjectOnAlpha=True
    bLightChanged=True
    bHardAttach=True
    bProjectActor=True
    bProjectOnParallelBSP=True
	bClipBSP=True
	RemoteRole=ROLE_None
}
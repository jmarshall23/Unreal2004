//=============================================================================
// xDomB.
//=============================================================================
class xDomB extends xDOMLetter;

defaultproperties
{   
    StaticMesh=XGame_rc.DomBMesh
    Skins(0)=Shader'XGameTextures.SuperPickups.DomABGS'
    DrawType=DT_StaticMesh
    DrawScale=0.250000
    bCollideWorld=false
    bCollideActors=false
    bStatic=false
    Physics=PHYS_Rotating
    bStasis=false
    bFixedRotationDir=True
    RotationRate=(Yaw=24000)
}

class GibOrganicRedForearm extends Gib;

defaultproperties
{
    DrawType=DT_StaticMesh
    DrawScale=1.3
    StaticMesh=StaticMesh'GibOrganicForearm'
    Skins=(Texture'GibOrganicRed')

    TrailClass=class'BloodJet'

    CollisionHeight=6.0
    CollisionRadius=6.0
}

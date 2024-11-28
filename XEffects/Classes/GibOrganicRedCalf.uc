class GibOrganicRedCalf extends Gib;

defaultproperties
{
    DrawType=DT_StaticMesh
    DrawScale=0.2
    StaticMesh=StaticMesh'GibOrganicCalf'
    Skins=(Texture'GibOrganicRed')

    TrailClass=class'BloodJet'

    CollisionHeight=6.0
    CollisionRadius=6.0
}

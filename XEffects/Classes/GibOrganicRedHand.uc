class GibOrganicRedHand extends Gib;

defaultproperties
{
    DrawType=DT_StaticMesh
    DrawScale=1.3
    StaticMesh=StaticMesh'GibOrganicHand'
    Skins=(Texture'GibOrganicRed')

    TrailClass=class'BloodJet'

    CollisionHeight=4.0
    CollisionRadius=4.0
}

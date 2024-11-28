class GibOrganicRedTorso extends Gib;

defaultproperties
{
    DrawType=DT_StaticMesh
    DrawScale=0.4
    StaticMesh=StaticMesh'GibOrganicTorso'
    Skins=(Texture'GibOrganicRed')

    TrailClass=class'BloodJet'

    CollisionHeight=10.0
    CollisionRadius=10.0
}

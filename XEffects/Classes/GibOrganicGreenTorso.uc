class GibOrganicGreenTorso extends Gib;

defaultproperties
{
	GibGroupClass=class'xAlienGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.4
    StaticMesh=StaticMesh'GibOrganicTorso'
    Skins=(Texture'GibOrganicGreen')

    TrailClass=class'AlienBloodJet'

    CollisionHeight=10.0
    CollisionRadius=10.0
}

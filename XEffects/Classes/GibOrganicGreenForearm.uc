class GibOrganicGreenForearm extends Gib;

defaultproperties
{
	GibGroupClass=class'xAlienGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=1.3
    StaticMesh=StaticMesh'GibOrganicForearm'
    Skins=(Texture'GibOrganicGreen')

    TrailClass=class'AlienBloodJet'

    CollisionHeight=6.0
    CollisionRadius=6.0
}

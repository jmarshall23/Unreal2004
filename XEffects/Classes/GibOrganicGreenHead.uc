class GibOrganicGreenHead extends GibHead;

defaultproperties
{
	GibGroupClass=class'xAlienGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.3
    StaticMesh=StaticMesh'GibOrganicHead'
    Skins=(Texture'GibOrganicGreen')

    TrailClass=class'AlienBloodJet'

    CollisionHeight=5.0
    CollisionRadius=6.0
}

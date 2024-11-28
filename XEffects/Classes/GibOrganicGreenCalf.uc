class GibOrganicGreenCalf extends Gib;

defaultproperties
{
	GibGroupClass=class'xAlienGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.2
    StaticMesh=StaticMesh'GibOrganicCalf'
    Skins=(Texture'GibOrganicGreen')

    TrailClass=class'AlienBloodJet'

    CollisionHeight=6.0
    CollisionRadius=6.0
}

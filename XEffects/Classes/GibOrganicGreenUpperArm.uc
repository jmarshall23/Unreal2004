class GibOrganicGreenUpperArm extends Gib;

defaultproperties
{
	GibGroupClass=class'xAlienGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.13
    StaticMesh=StaticMesh'GibOrganicUpperArm'
    Skins=(Texture'GibOrganicGreen')

    TrailClass=class'AlienBloodJet'

    CollisionHeight=6.0
    CollisionRadius=6.0
}

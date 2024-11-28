class GibOrganicGreenHand extends Gib;

defaultproperties
{
	GibGroupClass=class'xAlienGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=1.3
    StaticMesh=StaticMesh'GibOrganicHand'
    Skins=(Texture'GibOrganicGreen')

    TrailClass=class'AlienBloodJet'

    CollisionHeight=4.0
    CollisionRadius=4.0
}

class GibBotHead extends GibHead;

defaultproperties
{
	GibGroupClass=class'xBotGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.6
    StaticMesh=StaticMesh'GibBotHead'
    Skins=(Texture'GibBot')

    TrailClass=class'BotSparks'

    CollisionHeight=2.5
    CollisionRadius=5.0

    HitSounds(0)=Sound'WeaponSounds.P1GrenFloor1'
    HitSounds(1)=Sound'WeaponSounds.P1GrenFloor1'
}

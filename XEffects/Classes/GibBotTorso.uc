class GibBotTorso extends Gib;

defaultproperties
{
	GibGroupClass=class'xBotGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.5
    StaticMesh=StaticMesh'GibBotTorso'
    Skins=(Texture'GibBot')

    TrailClass=class'BotSparks'

    CollisionHeight=8.0
    CollisionRadius=8.0

    HitSounds(0)=Sound'WeaponSounds.P1GrenFloor1'
    HitSounds(1)=Sound'WeaponSounds.P1GrenFloor1'
}

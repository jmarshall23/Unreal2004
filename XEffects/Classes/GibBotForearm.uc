class GibBotForearm extends Gib;

defaultproperties
{
	GibGroupClass=class'xBotGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.4
    StaticMesh=StaticMesh'GibBotForearm'
    Skins=(Texture'GibBot')

    TrailClass=class'BotSparks'

    CollisionHeight=2.5
    CollisionRadius=5.0

    HitSounds(0)=Sound'WeaponSounds.P1GrenFloor1'
    HitSounds(1)=Sound'WeaponSounds.P1GrenFloor1'
}

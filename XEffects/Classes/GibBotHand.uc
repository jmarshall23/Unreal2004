class GibBotHand extends Gib;

defaultproperties
{
	GibGroupClass=class'xBotGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.65
    StaticMesh=StaticMesh'GibBotHand'
    Skins=(Texture'GibBot')

    TrailClass=class'BotSparks'

    CollisionHeight=1.5
    CollisionRadius=4.0

    HitSounds(0)=Sound'WeaponSounds.P1GrenFloor1'
    HitSounds(1)=Sound'WeaponSounds.P1GrenFloor1'
}

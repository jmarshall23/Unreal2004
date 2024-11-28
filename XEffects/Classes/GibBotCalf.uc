class GibBotCalf extends Gib;

defaultproperties
{
	GibGroupClass=class'xBotGibGroup'
    DrawType=DT_StaticMesh
    DrawScale=0.4
    StaticMesh=StaticMesh'GibBotCalf'
    Skins=(Texture'GibBot')

    TrailClass=class'BotSparks'

    CollisionHeight=3.0
    CollisionRadius=6.0
    
    HitSounds(0)=Sound'WeaponSounds.P1GrenFloor1'
    HitSounds(1)=Sound'WeaponSounds.P1GrenFloor1'
}

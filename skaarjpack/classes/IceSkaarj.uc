class IceSkaarj extends Skaarj;

event PostBeginPlay()
{
	Super.PostBeginPlay();
	MyAmmo.ProjectileClass = class'IceSkaarjProjectile';
}

defaultproperties
{
	Skins(0)=Skaarjw2
	ScoringValue=6
}
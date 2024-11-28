class FireSkaarj extends Skaarj;

event PostBeginPlay()
{
	Super.PostBeginPlay();
	MyAmmo.ProjectileClass = class'FireSkaarjProjectile';
}

defaultproperties
{
	Skins(0)=Skaarjw3
	ScoringValue=7
}
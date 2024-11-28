class EliteKrall extends Krall;

event PostBeginPlay()
{
	Super.PostBeginPlay();

	MyAmmo.ProjectileClass = class'EliteKrallBolt';
}

defaultproperties
{
	Skins(0)=ekrall
	skins(1)=ekrall
	ScoringValue=3
}
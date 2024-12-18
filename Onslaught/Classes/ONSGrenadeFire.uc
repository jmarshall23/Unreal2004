//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSGrenadeFire extends BioFire;

simulated function bool AllowFire()
{
	if (ONSGrenadeLauncher(Weapon).CurrentGrenades >= ONSGrenadeLauncher(Weapon).MaxGrenades)
		return false;

	return Super.AllowFire();
}

function Projectile SpawnProjectile(Vector Start, Rotator Dir)
{
	local ONSGrenadeProjectile G;

	G = ONSGrenadeProjectile(Super.SpawnProjectile(Start, Dir));
	if (G != None && ONSGrenadeLauncher(Weapon) != None)
	{
		G.SetOwner(Weapon);
		ONSGrenadeLauncher(Weapon).Grenades[ONSGrenadeLauncher(Weapon).Grenades.length] = G;
		ONSGrenadeLauncher(Weapon).CurrentGrenades++;
	}

	return G;
}

DefaultProperties
{
    AmmoClass=class'Onslaught.ONSGrenadeAmmo'
    ProjectileClass=class'Onslaught.ONSGrenadeProjectile'
    FireRate=0.65
    bSplashDamage=false
    bRecommendSplashDamage=false
}

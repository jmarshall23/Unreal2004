//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSBombDropper extends ONSWeapon;

#exec OBJ LOAD FILE=..\Animations\ONSWeapons-A.ukx

//state ProjectileFireMode
//{
//    function SpawnProjectile(class<Projectile> ProjClass)
//    {
//        local Projectile P;
//
//        P = spawn(ProjClass, self, , WeaponFireLocation, WeaponFireRotation);
//
//    	if (P != None)
//    	{
//            P.Velocity = Instigator.Velocity + (vect(0,0,-1000) << Owner.Rotation);
//
//            // Play firing noise
//            PlaySound(FireSoundClass, SLOT_None, FireSoundVolume/255.0,,,, false);
//        }
//    }
//}

DefaultProperties
{
    WeaponFireAttachmentBone=PlasmaGunBarrel
    WeaponFireOffset=0.0
    Mesh=Mesh'ONSWeapons-A.PlasmaGun'
    YawStartConstraint=65535
    YawEndConstraint=-65535
    PitchUpLimit=65535
    PitchDownLimit=-65535
    FireSoundClass=sound'WeaponSounds.RocketLauncher.RocketLauncherFire'
    ProjectileClass=class'OnslaughtFull.ONSBomberRocketProjectile'
    FireInterval=0.4
    bInheritVelocity=True
    bAimable=False
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSMASSideGunPawn extends ONSWeaponPawn;

DefaultProperties
{
	VehiclePositionString="in a Leviathan turret"
	VehicleNameString="Leviathan Turret"
	EntryPosition=(X=0,Y=0,Z=-150)
	EntryRadius=130.0
	ExitPositions(0)=(X=0,Y=-365,Z=200)
	ExitPositions(1)=(X=0,Y=365,Z=200)
	ExitPositions(2)=(X=0,Y=-365,Z=-100)
	ExitPositions(3)=(X=0,Y=365,Z=-100)
	GunClass=class'OnslaughtFull.ONSMASSideGun'
	CameraBone=Object83
	FPCamPos=(X=0,Y=0,Z=0)
	FPCamViewOffset=(X=5,Y=0,Z=35)
	bFPNoZFromCameraPitch=False
	TPCamLookAt=(X=0,Y=0,Z=110)
	TPCamDistance=100
	DriverDamageMult=0.1
	bDrawDriverInTP=False
}

class ONSDaredevilMessage extends LocalMessage;

#exec OBJ LOAD FILE="..\Sounds\announcermale2k4.uax"
#exec OBJ LOAD FILE="..\Sounds\ONSVehicleSounds-S.uax"

var		localized String	StuntInfoString1;
var		localized String	StuntInfoString2;
var		localized String	StuntInfoString3;
var		localized String	StuntInfoString4;
var		localized String	StuntInfoString5;
var		localized String	StuntInfoString6;
var		localized String	StuntInfoString7;
var		localized String	StuntInfoString8;

var		sound				CheerSound;
var		int					CheerPointThresh;

static function string GetString(
								 optional int SwitchNum,
								 optional PlayerReplicationInfo RelatedPRI_1,
								 optional PlayerReplicationInfo RelatedPRI_2,
								 optional Object OptionalObject
								 )
{
	local ONSWheeledCraft Car;

	Car = ONSWheeledCraft(OptionalObject);

	if(SwitchNum == 0)
		return Default.StuntInfoString1$Car.DaredevilPoints$Default.StuntInfoString2$Car.InAirDistance$Default.StuntInfoString3;
	else if(SwitchNum == 1)
		return Default.StuntInfoString4$Car.InAirSpin$Default.StuntInfoString5$Car.InAirPitch$Default.StuntInfoString6$Car.InAirRoll$Default.StuntInfoString7$Car.InAirTime$Default.StuntInfoString8;

}

static simulated function ClientReceive(
										PlayerController P,
										optional int SwitchNum,
										optional PlayerReplicationInfo RelatedPRI_1,
										optional PlayerReplicationInfo RelatedPRI_2,
										optional Object OptionalObject
										)
{
	local ONSWheeledCraft Car;

	Super.ClientReceive(P, SwitchNum, RelatedPRI_1, RelatedPRI_2, OptionalObject);

	if(SwitchNum == 0)
	{
		P.PlayRewardAnnouncement('DareDevil', 1, true);

		// If we got enough points - add a cheer as well!
		Car = ONSWheeledCraft(OptionalObject);

		if(Car.DaredevilPoints >= Default.CheerPointThresh)
			P.ClientPlaySound(Default.CheerSound);
	}
}

defaultproperties
{
	bFadeMessage=True
	bIsSpecial=True
	bIsUnique=False
	Lifetime=9
	bBeep=False
	DrawColor=(R=255,G=0,B=128,A=255)
	FontSize=0

	StuntInfoString1="Daredevil!  Level "
	StuntInfoString2=", "
	StuntInfoString3="m"
	StuntInfoString4="Spin: "
	StuntInfoString5="�, Flip: "
	StuntInfoString6="�, Roll: "
	StuntInfoString7="�, "
	StuntInfoString8=" secs"

	StackMode=SM_Down
	PosY=0.7

	CheerSound=sound'ONSVehicleSounds-S.crowd_cheer'
	CheerPointThresh=20
}

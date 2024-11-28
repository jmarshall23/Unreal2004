class MutVehicleArena extends Mutator
    config;

var config string ArenaVehicleClassName;
var class<SVehicle> ArenaVehicleClass;
var localized string ArenaDisplayText, ArenaDescText;
var array<class<SVehicle> > VehicleClasses;

function PostBeginPlay()
{
	local ONSVehicleFactory Factory;

	ArenaVehicleClass = class<SVehicle>( DynamicLoadObject(ArenaVehicleClassName,class'Class') );

	if(ArenaVehicleClass != None)
	{
		foreach AllActors( class 'ONSVehicleFactory', Factory )
		{
			Factory.VehicleClass = ArenaVehicleClass;
		}
	}

	Super.PostBeginPlay();
}

static function FillPlayInfo(PlayInfo PlayInfo)
{
	local string VehicleOptions;
	local int i;

	Super.FillPlayInfo(PlayInfo);

	for (i=0; i<default.VehicleClasses.Length; i++)
	{
		if (VehicleOptions != "")
			VehicleOptions $= ";";

		VehicleOptions $= default.VehicleClasses[i] $ ";" $ default.VehicleClasses[i].default.VehicleNameString;
	}

	if(class'GUI2K4.UT2K4SP_Main'.default.bEnableTC)
		VehicleOptions $= ";" $ class'OnslaughtFull.ONSGenericSD' $ ";" $ class'OnslaughtFull.ONSGenericSD'.default.VehicleNameString;


	PlayInfo.AddSetting(default.RulesGroup, "ArenaVehicleClassName", default.ArenaDisplayText, 0, 1, "Select", VehicleOptions);
}

static event string GetDescriptionText(string PropName)
{
	if (PropName == "ArenaWeaponClassName")
		return default.ArenaDescText;

	return Super.GetDescriptionText(PropName);
}

defaultproperties
{
	ArenaDisplayText="Arena Vehicle"
	ArenaDescText="Determines which vehicle type will be used in the match."
    IconMaterialName="MutatorArt.nosym"
	GroupName="VehicleArena"
    FriendlyName="Vehicle Arena"
    Description="Replace all vehicles in map with a particular type."
    ArenaVehicleClassName="Onslaught.ONSRV"

	VehicleClasses(0)=class'Onslaught.ONSRV'
	VehicleClasses(1)=class'Onslaught.ONSPRV'
	VehicleClasses(2)=class'Onslaught.ONSAttackCraft'
	VehicleClasses(3)=class'Onslaught.ONSHoverBike'
	VehicleClasses(4)=class'Onslaught.ONSHoverTank'
}

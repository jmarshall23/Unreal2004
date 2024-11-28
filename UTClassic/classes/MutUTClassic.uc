class MutUTClassic extends Mutator;

var class<Weapon> OriginalClasses[8];
var class<Weapon> ReplacementClasses[8];
var config bool bCanDoubleJump;
var config bool bCanWallDodge;
var config bool bCanDodgeDoubleJump;
var config bool bModifyWeaponDamage;
var config bool bClassicTranslocator;
var localized string DJString, WDString, DDJString, DJHelp, WDHelp, DDJHelp, WeapString, WeapHelp, TranslocString, TranslocHelp;

function PostBeginPlay()
{
	local GameRules G;

	Super.PostBeginPlay();
	if ( bModifyWeaponDamage )
	{
		G = spawn(class'UTClassicGameRules');
		if ( Level.Game.GameRulesModifiers == None )
			Level.Game.GameRulesModifiers = G;
		else
			Level.Game.GameRulesModifiers.AddGameRules(G);
	}
	Level.Game.bModViewShake = false;
}

function bool CheckReplacement( Actor Other, out byte bSuperRelevant )
{
	local int i;

	bSuperRelevant = 0;
	if ( xPawn(Other) != None )
	{
		if ( !bCanDoubleJump )
		{
			xPawn(Other).MaxMultiJump = 0;
			xPawn(Other).MultiJumpRemaining = 0;
		}
		xPawn(Other).bCanWallDodge = bCanWallDodge;
		xPawn(Other).bCanDodgeDoubleJump = bCanDodgeDoubleJump;
		xPawn(Other).RequiredEquipment[1] = "UTClassic.ClassicShieldGun";
		if ( bClassicTranslocator && Deathmatch(Level.Game).AllowTransloc() )
			xPawn(Other).RequiredEquipment[2] = "UTClassic.ClassicTranslauncher";
	}
    else if ( xWeaponBase(Other) != None )
    {
		for ( i=0; i<8; i++ )
			if ( xWeaponBase(Other).WeaponType == OriginalClasses[i] )
			{
				xWeaponBase(Other).WeaponType = ReplacementClasses[i];
				return true;
			}
	}
	else if ( SniperAmmoPickup(Other) != None )
	{
		ReplaceWith( Other, "UTClassic.ClassicSniperAmmoPickup");
		return false;
	}
	else if ( bClassicTranslocator && (Translauncher(Other) != None) )
	{
		if ( ClassicTransLauncher(Other) == None )
			return false;
	}
	return true;
}

static function FillPlayInfo(PlayInfo PlayInfo)
{
	Super.FillPlayInfo(PlayInfo);

	PlayInfo.AddSetting(default.RulesGroup, "bCanDoubleJump", default.DJString, 0, 1, "Check");
	PlayInfo.AddSetting(default.RulesGroup, "bCanWallDodge", default.WDString, 0, 1, "Check");
	PlayInfo.AddSetting(default.RulesGroup, "bCanDodgeDoubleJump", default.DDJString, 0, 1, "Check");
	PlayInfo.AddSetting(default.RulesGroup, "bModifyWeaponDamage", default.WeapString, 0, 1, "Check");
	PlayInfo.AddSetting(default.RulesGroup, "bClassicTranslocator", default.TranslocString, 0, 1, "Check");
}

static event string GetDescriptionText(string PropName)
{
	switch (PropName)
	{
		case "bCanDoubleJump":	return default.DJHelp;
		case "bCanWallDodge":			return default.WDHelp;
		case "bCanDodgeDoubleJump":			return default.DDJHelp;
		case "bModifyWeaponDamage":			return default.WeapHelp;
		case "bClassicTranslocator":			return default.TranslocHelp;
	}

	return Super.GetDescriptionText(PropName);
}

defaultproperties
{
	DJString="Allow Double Jumping"
	WDString="Allow Wall Dodging"
	DDJString="Allow Dodge Double Jumping"
	DJHelp="If enabled, players can double jump at the peak of jumps."
	WDHelp="If enabled, players can dodge off walls."
	DDJHelp="If enabled, players can double jump at the peak of dodge jumps."
    IconMaterialName="MutatorArt.nosym"
    GroupName="Arena"
    FriendlyName="UT Classic"
    Description="Classic UT style weapons and movement options."
	WeapString="Modify Weapon Damage"
	WeapHelp="If enabled, weapons do more damage."
	TranslocString="Modify Translocator"
	TranslocHelp="If enabled, translocator recharges faster."

    OriginalClasses(0)=class'FlakCannon'
    OriginalClasses(1)=class'Minigun'
    OriginalClasses(2)=class'ShockRifle'
    OriginalClasses(3)=class'RocketLauncher'
    OriginalClasses(4)=class'SniperRifle'
    OriginalClasses(5)=class'BioRifle'
    OriginalClasses(6)=class'Translauncher'
    OriginalClasses(7)=class'ShieldGun'

    ReplacementClasses(0)=class'ClassicFlakCannon'
    ReplacementClasses(1)=class'ClassicMinigun'
    ReplacementClasses(2)=class'ClassicShockRifle'
    ReplacementClasses(3)=class'ClassicRocketLauncher'
    ReplacementClasses(4)=class'ClassicSniperRifle'
    ReplacementClasses(5)=class'ClassicBioRifle'
    ReplacementClasses(6)=class'ClassicTranslauncher'
    ReplacementClasses(7)=class'ClassicShieldGun'

    bCanDoubleJump=true
    bCanWallDodge=true
    bCanDodgeDoubleJump=true
    bModifyWeaponDamage=true
    bClassicTranslocator=true
}



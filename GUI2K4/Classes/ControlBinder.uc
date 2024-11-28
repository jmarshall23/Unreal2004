//==============================================================================
//  Created on: 11/23/2003
//  Description
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class ControlBinder extends KeyBindMenu;

var localized string BindingLabel[150];

function LoadCommands()
{
	local int i;

	Super.LoadCommands();

	// Update the MultiColumnList's sortdata array to reflect the indexes of our Bindings array
    for (i = 0; i < Bindings.Length; i++)
    	li_Binds.AddedItem();
}

function MapBindings()
{
	LoadCustomBindings();
	Super.MapBindings();
}

protected function LoadCustomBindings()
{
	local int i;
	local array<string> KeyBindClasses;
    local class<GUIUserKeyBinding> CustomKeyBindClass;

    // Load custom keybinds from .int files
    PlayerOwner().GetAllInt("XInterface.GUIUserKeyBinding",KeyBindClasses);
	for (i = 0; i < KeyBindClasses.Length; i++)
	{
		CustomKeyBindClass = class<GUIUserKeyBinding>(DynamicLoadObject(KeyBindClasses[i],class'Class'));
		if (CustomKeyBindClass != None)
			AddCustomBindings( CustomKeyBindClass.default.KeyData );
    }
}

function AddCustomBindings( array<GUIUserKeyBinding.KeyInfo> KeyData )
{
	local int i;

	for ( i = 0; i < KeyData.Length; i++ )
		CreateAliasMapping( KeyData[i].Alias, KeyData[i].KeyLabel, KeyData[i].bIsSection );
}

function ClearBindings()
{
	local int i, max;

	Super.ClearBindings();
	Bindings = default.Bindings;
	max = Min(Bindings.Length, ArrayCount(BindingLabel));
	for ( i = 0; i < max; i++ )
	{
		if ( BindingLabel[i] != "" )
			Bindings[i].KeyLabel = BindingLabel[i];
	}
}

DefaultProperties
{
	PageCaption="Configure Keys"
	Headings(0)="Action"
    Bindings(0)=(bIsSectionLabel=true,KeyLabel="Movement")
    BindingLabel(0)="Movement"
    Bindings(1)=(KeyLabel="Forward",Alias="MoveForward")
    BindingLabel(1)="Forward"
    Bindings(2)=(KeyLabel="Backward",Alias="MoveBackward")
    BindingLabel(2)="Backward"
    Bindings(3)=(KeyLabel="Strafe Left",Alias="StrafeLeft")
    BindingLabel(3)="Strafe Left"
    Bindings(4)=(KeyLabel="Strafe Right",Alias="StrafeRight")
    BindingLabel(4)="Strafe Right"
    Bindings(5)=(KeyLabel="Jump",Alias="Jump")
    BindingLabel(5)="Jump"
    Bindings(6)=(KeyLabel="Walk",Alias="Walking")
    BindingLabel(6)="Walk"
    Bindings(7)=(KeyLabel="Crouch",Alias="Duck")
    BindingLabel(7)="Crouch"
    Bindings(8)=(KeyLabel="Strafe Toggle",Alias="Strafe")
    BindingLabel(8)="Strafe Toggle"

    Bindings(9)=(bIsSectionLabel=true,KeyLabel="Looking")
    BindingLabel(9)="Looking"
    Bindings(10)=(KeyLabel="Turn Left",Alias="TurnLeft")
    BindingLabel(10)="Turn Left"
    Bindings(11)=(KeyLabel="Turn Right",Alias="TurnRight")
    BindingLabel(11)="Turn Right"
    Bindings(12)=(KeyLabel="Look Up",Alias="LookUp")
    BindingLabel(12)="Look Up"
    Bindings(13)=(KeyLabel="Look Down",Alias="LookDown")
    BindingLabel(13)="Look Down"
    Bindings(14)=(KeyLabel="Center View",Alias="CenterView")
    BindingLabel(14)="Center View"
    Bindings(15)=(KeyLabel="Toggle \"BehindView\"",Alias="ToggleBehindView")
    BindingLabel(15)="Toggle \"BehindView\""
    Bindings(16)=(KeyLabel="Toggle Camera Mode",Alias="ToggleFreeCam")
    BindingLabel(16)="Toggle Camera Mode"

    Bindings(17)=(bIsSectionLabel=true,KeyLabel="Weapons")
    BindingLabel(17)="Weapons"
    Bindings(18)=(KeyLabel="Fire",Alias="Fire")
    BindingLabel(18)="Fire"
    Bindings(19)=(KeyLabel="Alt-Fire",Alias="AltFire")
    BindingLabel(19)="Alt-Fire"
    Bindings(20)=(KeyLabel="Throw Weapon",Alias="ThrowWeapon")
    BindingLabel(20)="Throw Weapon"
    Bindings(21)=(KeyLabel="Best Weapon",Alias="SwitchToBestWeapon")
    BindingLabel(21)="Best Weapon"
    Bindings(22)=(KeyLabel="Next Weapon",Alias="NextWeapon")
    BindingLabel(22)="Next Weapon"
    Bindings(23)=(KeyLabel="Prev Weapon",Alias="PrevWeapon")
    BindingLabel(23)="Prev Weapon"
    Bindings(24)=(KeyLabel="Last Weapon",Alias="SwitchToLastWeapon")
    BindingLabel(24)="Last Weapon"
    Bindings(25)=(KeyLabel="Weapon Selection",Alias="")
    BindingLabel(25)="Weapon Selection"
    Bindings(26)=(KeyLabel="Super Weapon",Alias="SwitchWeapon 0")
    BindingLabel(26)="Super Weapon"
    Bindings(27)=(KeyLabel="Shield Gun",Alias="SwitchWeapon 1")
    BindingLabel(27)="Shield Gun"
    Bindings(28)=(KeyLabel="Assault Rifle",Alias="SwitchWeapon 2")
    BindingLabel(28)="Assault Rifle"
	Bindings(29)=(KeyLabel="Bio-Rifle",Alias="SwitchWeapon 3")
    BindingLabel(29)="Bio-Rifle"
    Bindings(30)=(KeyLabel="Shock Rifle",Alias="SwitchWeapon 4")
    BindingLabel(30)="Shock Rifle"
    Bindings(31)=(KeyLabel="Link Gun",Alias="SwitchWeapon 5")
    BindingLabel(31)="Link Gun"
    Bindings(32)=(KeyLabel="Minigun",Alias="SwitchWeapon 6")
    BindingLabel(32)="Minigun"
    Bindings(33)=(KeyLabel="Flak Cannon",Alias="SwitchWeapon 7")
    BindingLabel(33)="Flak Cannon"
    Bindings(34)=(KeyLabel="Rocket Launcher",Alias="SwitchWeapon 8")
    BindingLabel(34)="Rocket Launcher"
    Bindings(35)=(KeyLabel="Lightning Rifle",Alias="SwitchWeapon 9")
    BindingLabel(35)="Lightning Rifle"
    Bindings(36)=(KeyLabel="Translocator",Alias="SwitchWeapon 10")
    BindingLabel(36)="Translocator"

    Bindings(37)=(bIsSectionLabel=true,KeyLabel="Communication")
    BindingLabel(37)="Communication"
    Bindings(38)=(KeyLabel="Say",Alias="Talk")
    BindingLabel(38)="Say"
    Bindings(39)=(KeyLabel="Team Say",Alias="TeamTalk")
    BindingLabel(39)="Team Say"
    Bindings(40)=(KeyLabel="In Game Chat",Alias="InGameChat")
    BindingLabel(40)="In Game Chat"
    Bindings(41)=(KeyLabel="Speech Menu",Alias="SpeechMenuToggle")
    BindingLabel(41)="Speech Menu"
    Bindings(42)=(KeyLabel="Activate Microphone",Alias="VoiceTalk")
    BindingLabel(42)="Activate Microphone"
    Bindings(43)=(KeyLabel="Speak in Public Chatroom",Alias="Speak Public")
    BindingLabel(43)="Speak in Public Chatroom"
    Bindings(44)=(KeyLabel="Speak in local Chatroom",Alias="Speak Local")
    BindingLabel(44)="Speak in local Chatroom"
    Bindings(45)=(KeyLabel="Speak in Team Chatroom",Alias="Speak Team")
    BindingLabel(45)="Speak in Team Chatroom"
    Bindings(46)=(KeyLabel="Toggle Public Chatroom",Alias="ToggleTeamChat")
    BindingLabel(46)="Toggle Public Channel"
    Bindings(47)=(KeyLabel="Toggle Local Chatroom",Alias="ToggleLocalChat")
    BindingLabel(47)="Toggle Local Channel"
    Bindings(48)=(KeyLabel="Toggle Team Chatroom",Alias="ToggleTeamChat")
    BindingLabel(48)="Toggle Team Channel"

    Bindings(49)=(bIsSectionLabel=true,KeyLabel="Taunts")
    BindingLabel(49)="Taunts"
    Bindings(50)=(KeyLabel="Pelvic Thrust",Alias="taunt pthrust")
    BindingLabel(50)="Pelvic Thrust"
    Bindings(51)=(KeyLabel="Ass Smack",Alias="taunt asssmack")
    BindingLabel(51)="Ass Smack"
    Bindings(52)=(KeyLabel="Throat Cut",Alias="taunt throatcut")
    BindingLabel(52)="Throat Cut"
    Bindings(53)=(KeyLabel="Brag",Alias="taunt gesture_point")
    BindingLabel(53)="Brag"

    Bindings(54)=(bIsSectionLabel=true,KeyLabel="Hud")
    BindingLabel(54)="Hud"
    Bindings(55)=(KeyLabel="Grow Hud",Alias="GrowHud")
    BindingLabel(55)="Grow Hud"
    Bindings(56)=(KeyLabel="Shrink Hud",Alias="ShrinkHud")
    BindingLabel(56)="Shrink Hud"
    Bindings(57)=(KeyLabel="Show Radar Map",Alias="ToggleRadarMap")
    BindingLabel(57)="Show Radar Map"
    Bindings(58)=(KeyLabel="ScoreBoard",Alias="ShowScores")
    BindingLabel(58)="ScoreBoard Toggle"
    Bindings(59)=(KeyLabel="ScoreBoard (QuickView)",Alias="ScoreToggle")
    BindingLabel(59)="ScoreBoard"

    Bindings(60)=(bIsSectionLabel=true,KeyLabel="Game")
    BindingLabel(60)="Game"
    Bindings(61)=(KeyLabel="Use",Alias="use")
    BindingLabel(61)="Use"
    Bindings(62)=(KeyLabel="Pause",Alias="Pause")
    BindingLabel(62)="Pause"
    Bindings(63)=(KeyLabel="Screenshot",Alias="shot")
    BindingLabel(63)="Screenshot"
    Bindings(64)=(KeyLabel="Find Red Base",Alias="basepath 0")
    BindingLabel(64)="Find Red Base"
    Bindings(65)=(KeyLabel="Find Blue Base",Alias="basepath 1")
    BindingLabel(65)="Find Blue Base"
    Bindings(66)=(KeyLabel="Next Inventory Item",Alias="InventoryNext")
    BindingLabel(66)="Next Inventory Item"
    Bindings(67)=(KeyLabel="Previous Inventory Item",Alias="InventoryPrevious")
    BindingLabel(67)="Previous Inventory Item"
    Bindings(68)=(KeyLabel="Activate Current Inventory Item",Alias="InventoryActivate")
    BindingLabel(68)="Activate Current Inventory Item"
    Bindings(69)=(KeyLabel="Show Personal Stats",Alias="ShowStats")
    BindingLabel(69)="Show Personal Stats"
    Bindings(70)=(KeyLabel="View Next Player's Stats",Alias="NextStats")
    BindingLabel(70)="View Next Player's Stats"
    Bindings(71)=(KeyLabel="Server Info",Alias="ServerInfo")
    BindingLabel(71)="Server Info"
    Bindings(72)=(KeyLabel="Vehicle Horn",Alias="playvehiclehorn 0")
    BindingLabel(72)="Vehicle Horn"

    Bindings(73)=(bIsSectionLabel=true,KeyLabel="Miscellaneous")
    BindingLabel(73)="Miscellaneous"
    Bindings(74)=(KeyLabel="Menu",Alias="ShowMenu")
    BindingLabel(74)="Menu"
    Bindings(75)=(KeyLabel="Music Player",Alias="MusicMenu")
    BindingLabel(75)="Music Player"
    Bindings(76)=(KeyLabel="Voting Menu",Alias="ShowVoteMenu")
    BindingLabel(76)="Voting Menu"
    Bindings(77)=(KeyLabel="Toggle Console",Alias="ConsoleToggle")
    BindingLabel(77)="Toggle Console"
    Bindings(78)=(KeyLabel="View Connection Status",Alias="Stat Net")
    BindingLabel(78)="View Connection Status"
    Bindings(79)=(KeyLabel="Cancel Pending Connection",Alias="Cancel")
    BindingLabel(79)="Cancel Pending Connection"
}

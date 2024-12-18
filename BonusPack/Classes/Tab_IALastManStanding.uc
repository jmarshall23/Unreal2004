// ====================================================================
//  Class: BonusPack.Tab_IALastManStanding
//
//  Last Man Standing game type config page.
//
//  Written by James Golding

//	Rollback  
//  (c) 2002, Epic Games, Inc.  All Rights Reserved
// ====================================================================

class Tab_IALastManStanding extends Tab_IADeathMatch;

var config bool		LastExtraLives;
var config bool		LastSuperWeaps;
var config bool 	LastAllowPickups;
var config bool 	LastAllowAdrenaline;

var moCheckBox		MyExtraLives;
var moCheckBox		MySuperWeaps;
var moCheckBox		MyAllowPickups;
var moCheckBox		MyAllowAdrenaline;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);

	MyExtraLives=moCheckBox(Controls[14]);
	MySuperWeaps=moCheckBox(Controls[15]);
    MyAllowPickups=moCheckBox(Controls[16]);
    MyAllowAdrenaline=moCheckBox(Controls[17]);

	Controls[2].WinHeight=0.643324; // Make the box a bit bigger.

	MyExtraLives.Checked(LastExtraLives);
	MySuperWeaps.Checked(LastSuperWeaps);
    MyAllowPickups.Checked(LastAllowPickups);
    MyAllowAdrenaline.Checked(LastAllowAdrenaline);

	//MyGoalScore.MenuStateChange(MSAT_Disabled);
    //MyGoalScore.SetValue(0);

}

// Add extra options to URL
function string Play()
{
	local string url;

	LastExtraLives = MyExtraLives.IsChecked();
	LastSuperWeaps = MySuperWeaps.IsChecked();
    LastAllowPickups = MyAllowPickups.IsChecked();
    LastAllowAdrenaline = MyAllowAdrenaline.IsChecked();

	SaveConfig();

	url = Super.Play();
	url = url$"?HealthForKill="$LastExtraLives$"?SuperWeaps="$LastSuperWeaps$"?AllowPickups="$LastAllowPickups$"?AllowAdrenaline="$LastAllowAdrenaline;

	return url;
}

function SuperWeapChange(GUIComponent Sender)
{
	if (MySuperWeaps.IsChecked())
    	MyAllowPickups.Checked(true);

	LastSuperWeaps = MySuperWeaps.IsChecked();
	SaveConfig();

}

function AllowPickupsChange(GUIComponent Sender)
{
	if (!MyAllowPickups.IsChecked())
    	MySuperWeaps.Checked(false);

    LastAllowPickups = MyAllowPickups.IsChecked();
	SaveConfig();

}

function AllowAdrenalineChange(GUIComponent Sender)
{
    LastAllowAdrenaline = MyAllowAdrenaline.IsChecked();
	SaveConfig();
}

function ExtraLivesChange(GUIComponent Sender)
{
	LastExtraLives = MyExtraLives.IsChecked();
	SaveConfig();
}


defaultproperties
{
	Begin Object class=moCheckBox Name=IARulesExtraLives
		WinWidth=0.400000
		WinHeight=0.040000
		WinLeft=0.050000
		WinTop=0.747500
		Caption="Killing Gives Health"
		Hint="When selected, a portion of the player's health is restored with each kill."
		bSquare=true
		ComponentJustification=TXTA_Left
		CaptionWidth=0.9
        OnChange=ExtraLivesChange
	End Object
	Controls(14)=moCheckBox'IARulesExtraLives'

	Begin Object class=moCheckBox Name=IARulesSuperWeaps
		WinWidth=0.400000
		WinHeight=0.040000
		WinLeft=0.542188
		WinTop=0.799166
		Caption="Allow Super Weapons"
		Hint="When selected, allow super weapon pickups."
		bSquare=true
        OnChange=SuperWeapChange;
		ComponentJustification=TXTA_Left
		CaptionWidth=0.9
	End Object
	Controls(15)=moCheckBox'IARulesSuperWeaps'

	Begin Object class=moCheckBox Name=IARulesLMSAllowPickups
		WinWidth=0.400000
		WinHeight=0.040000
		WinLeft=0.542188
		WinTop=0.747083
		Caption="Allow Pickups"
		Hint="When selected, allow normal pickups."
		bSquare=true
        OnChange=AllowPickupsChange
		ComponentJustification=TXTA_Left
		CaptionWidth=0.9
	End Object
	Controls(16)=moCheckBox'IARulesLMSAllowPickups'

	Begin Object class=moCheckBox Name=IARulesLMSAllowAdrenaline
		WinWidth=0.400000
		WinHeight=0.040000
		WinLeft=0.542188
		WinTop=0.851250
		Caption="Allow Adrenaline"
		Hint="When selected, players can use Adrenaline."
		bSquare=true
        OnChange=AllowAdrenalineChange
		ComponentJustification=TXTA_Left
		CaptionWidth=0.9
	End Object
	Controls(17)=moCheckBox'IARulesLMSAllowAdrenaline'

	LastExtraLives=true
	LastSuperWeaps=false

	LastMaxLives = 3
}

// ====================================================================
//  Class: BonusPack.Tab_IAMutant
//
//  Mutant game type config page.
//
//  Written by James Golding

//	Rollback
//  (c) 2002, Epic Games, Inc.  All Rights Reserved
// ====================================================================

class Tab_IAMutant extends Tab_IADeathMatch;

var localized string	TargetScoreString;

var config bool 	LastBottomFeeders;

var moCheckBox		MyBottomFeeders;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);

	MyBottomFeeders = moCheckBox(Controls[16]);

	// Make the boxes a bit bigger.
	Controls[1].WinHeight = 0.538638;
	Controls[2].WinHeight = 0.538638;
	Controls[14].WinTop = 0.800430;

	MyBottomFeeders.Checked(LastBottomFeeders);

	//MyGoalScore.MyLabel.Caption = TargetScoreString;
	//MyGoalScore.SetValue(LastGoalScore);
}

// Add extra options to URL
function string Play()
{
	local string url;

	LastBottomFeeders = MyBottomFeeders.IsChecked();
	SaveConfig();

	url = Super.Play();
	url = url$"?BottomFeeder="$LastBottomFeeders;

	return url;
}

function BottomFeedersChange(GUIComponent Sender)
{
	LastBottomFeeders = MyBottomFeeders.IsChecked();
	SaveConfig();
}

defaultproperties
{
	Begin Object class=moCheckBox Name=IARulesBottomFeeders
		WinWidth=0.400000
		WinHeight=0.040000
		WinLeft=0.050000
		WinTop=0.714166
		Caption="Bottom Feeder"
		Hint="When selected, bottom ranked player(s) can kill anyone."
		bSquare=true
		ComponentJustification=TXTA_Left
		CaptionWidth=0.9
        OnChange=BottomFeedersChange
	End Object
	Controls(16)=moCheckBox'IARulesBottomFeeders'

	TargetScoreString="Score Limit"
	LastGoalScore=20
	LastBottomFeeders=true
}

//==============================================================================
// General message page, to display "welcome" and "congrats" messages
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc. All Rights Reserved
//==============================================================================
class UT2K4SP_Message extends UT2K4GUIPage;

#exec OBJ LOAD File=AnnouncerFemale2K4.uax

var automated GUIImage	imgBackground, imgSponsorBg, imgSponsor;
var automated GUIButton btnOk, btnClock;
var automated GUISubtitleText sbText;

/** subtitle to display, use | as line seperator */
var localized string messages[11];
/**
	subtitle delay per line, use | as line seperator
	note: multiply the timing with 1.1 to get the correct timing
*/
var localized string messageslength[11];
var sound sounds[11];
var string screenshots[11];

/** Used to play back the correct sound */
var int MessageID;

var float oldMusicVol;

event HandleParameters(string Param1, string Param2)
{
	Super.HandleParameters(Param1, Param2);
	btnClock.bVisible = Controller.bModAuthor;
	MessageID = int(Param1);
	OpenSound = sounds[MessageID];
	imgBackground.Image = Material(DynamicLoadObject(screenshots[MessageID], class'Material', true));
	SetTimer(0.1, true); // workaround for not playing sound after a match
	// update music
	oldMusicVol = float(PlayerOwner().ConsoleCommand("get ini:Engine.Engine.AudioDevice MusicVolume"));
	if (oldMusicVol > 0.5)
	{
		PlayerOwner().ConsoleCommand("set ini:Engine.Engine.AudioDevice MusicVolume 0.5");
	}
	else oldMusicVol = -1;
}

function bool NotifyLevelChange()
{
	OpenSound = sounds[MessageID];
	SetTimer(0.1, true);
	LevelChanged();
	return false;
}

event Timer()
{
	local float t;
	if (OpenSound != none)
	{
		t = PlayerOwner().GetSoundDuration(sounds[MessageID])+1.0;
		if (messageslength[MessageID] != "") sbText.SetSubtitles(messages[MessageID],, false,messageslength[MessageID]);
			else sbText.SetSubtitles(messages[MessageID]);
		if (t <= 0)
		{
			btnOk.bVisible = true;
		}
		else {
			TimerInterval = t;
			if (Controller.bModAuthor) StopWatch();
			PlayerOwner().PlayOwnedSound(sounds[MessageID],SLOT_Interface,1.0);
		}
		OpenSound = none;
	}
	else Controller.CloseMenu(false);
}

event ChangeHint(string NewHint);

function bool btnOkOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(false);
	return true;
}

function bool OnClockClick(GUIComponent Sender)
{
	if (!Controller.bModAuthor) return false;
	StopWatch(true);
	StopWatch();
	return true;
}

function RestoreMusicVolume(bool bCancelled)
{
	if (oldMusicVol > -1)
	{
		PlayerOwner().ConsoleCommand("set ini:Engine.Engine.AudioDevice MusicVolume"@oldMusicVol);
	}
}

defaultproperties
{
	Begin Object class=GUIImage name=SPMimgBackground
		WinWidth=1
		WinHeight=1
		WinLeft=0
		WinTop=0
		RenderWeight=0.1
		X1=0
		Y1=0
		X2=1023
		Y2=767
		Image=Material'2K4Menus.Controls.thinpipe_f'
		ImageStyle=ISTY_Scaled
		ImageRenderStyle=MSTY_Normal
		bVisible=true
	End Object
	imgBackground=SPMimgBackground

	Begin Object Class=GUIButton Name=SPMbtnOk
		Caption="OK"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.401563
		WinTop=0.95
		OnClick=btnOkOnClick
		FontScale=FNS_Small
		bVisible=false
	End Object
	btnOk=SPMbtnOk

	Begin Object Class=GUIButton Name=SPMbtnClock
		Caption="Clock"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.8
		WinTop=0.95
		FontScale=FNS_Small
		bVisible=false
		OnClick=OnClockClick
	End Object
	btnClock=SPMbtnClock

	Begin Object Class=GUISubtitleText Name=SPPsbText
		StyleName=""
		WinWidth=0.960000
		WinHeight=0.206250
		WinLeft=0.020000
		WinTop=0.781667
		TextColor=(R=255,G=200,B=0,A=225)
		TextFont="UT2SmallHeaderFont"
		// create an outline
		ShadowColor=(R=0,G=0,B=0,A=196)
		ShadowOffsetX=1
		ShadowOffsetY=1
		HilightColor=(R=255,G=255,B=255,A=128)
		HilightOffsetX=1
		HilightOffsetY=1
		VertAlign=TXTA_Right
	End Object
	sbText=SPPsbText

	OnClose=RestoreMusicVolume

	messages[0]="The Liandri Corporation's Unreal Tournament enters its twelfth year as a legally sanctioned competition.|This year's tournament is the most highly anticipated yet, with teams captained by highly regarded former champions Malcolm, Gorge, and the long absent Xan Kriegor, as well as a dangerous team from the Skaarj Empire.|Before you can enter the Tournament proper, you must first defeat other hopefuls in the qualifying rounds."
	messageslength[0]="6.4119|14.8049|99"
	Sounds[0]=Sound'AnnouncerFemale2K4.SinglePlayer_1'
	Screenshots[0]="LadderShots.TeamDMMoneyShot"

	messages[1]="Your success in the preliminary rounds has convinced your sponsor to fund a team with you as captain.|Once you have drafted your team, you must defeat them in a free-for-all deathmatch to demonstrate your worthiness as leader."
	messageslength[1]="6.4471|99"
	Sounds[1]=Sound'AnnouncerFemale2K4.SinglePlayer_2'
	Screenshots[1]="LadderShots.TeamDMMoneyShot"

	messages[2]="The team has accepted you as their captain.|All that remains before reaching the tournament is to lead them through the team qualifying rounds.|You will be facing Sun Blade, Super Nova, and the new Goliath team of Juggernauts, also seeking to qualify for the Tournament."
	messageslength[2]="2.6257|5.7838|99"
	Sounds[2]=Sound'AnnouncerFemale2K4.SinglePlayer_3'
	Screenshots[2]="LadderShots.TeamDMMoneyShot"

	// DDOM
	messages[3]="Your team has now qualified for the Tournament.|You must advance to the top of all four ladders to qualify for the Tournament Finals.|Initially, only the Double Domination ladder is available to you."
	messageslength[3]="2.9667|5.1766|99"
	Sounds[3]=Sound'AnnouncerFemale2K4.SinglePlayer_4a'
	Screenshots[3]="LadderShots.TeamDMMoneyShot"

	// CTF
	messages[4]="Due to your success in the Double Domination ladder, the Capture the Flag ladder is now available to you."
	messageslength[4]="99"
	Sounds[4]=Sound'AnnouncerFemale2K4.SinglePlayer_4b'
	Screenshots[4]="LadderShots.TeamDMMoneyShot"

	// BR
	messages[5]="Due to your success in the Capture the Flag ladder, the Bombing Run ladder is now available to you."
	messageslength[5]="99"
	Sounds[5]=Sound'AnnouncerFemale2K4.SinglePlayer_4c'
	Screenshots[5]="LadderShots.TeamDMMoneyShot"

	// AS
	messages[6]="Due to your success in the Bombing Run ladder, the Assault ladder is now available to you."
	messageslength[6]="99"
	Sounds[6]=Sound'AnnouncerFemale2K4.SinglePlayer_4d'
	Screenshots[6]="LadderShots.TeamDMMoneyShot"

	// before finals
	messages[7]="The Skaarj disposed of all other competitors on their way to this year's finals.|In reaction to their surprising defeat in the ladder at your hands, they have brought in their Clan Lord to reinforce the team.|The Clan Lord is like no Skaarj you have ever faced."
	messageslength[7]="4.7696|7.5625|99"
	Sounds[7]=Sound'AnnouncerFemale2K4.SinglePlayer_5a'
	Screenshots[7]="LadderShots.TeamDMMoneyShot"

	messages[8]="Malcolm is back, and his return transforms Thunder Crash.|They defeated all the other contestants without their great champion.|His return would seem to make their ultimate victory certain."
	messageslength[8]="3.7642|3.9952|99"
	Sounds[8]=Sound'AnnouncerFemale2K4.SinglePlayer_5b'
	Screenshots[8]="LadderShots.TeamDMMoneyShot"

	messages[9]="The Corrupt have done the bidding of their master, Xan Kriegor, and advanced to the Tournament finals.|Now Xan enters the fray, and your previous success against The Corrupt is rendered meaningless."
	messageslength[9]="5.511|99"
	Sounds[9]=Sound'AnnouncerFemale2K4.SinglePlayer_5c'
	Screenshots[9]="LadderShots.TeamDMMoneyShot"

	// End message
	messages[10]="Victory is yours! Today you enter the immortal pantheon of Tournament Champions."
	messageslength[10]="99"
	Sounds[10]=Sound'AnnouncerFemale2K4.SinglePlayer_6'
	Screenshots[10]="LadderShots.TeamDMMoneyShot"

	WinLeft=0
	WinTop=0
	WinWidth=1
	WinHeight=1
}

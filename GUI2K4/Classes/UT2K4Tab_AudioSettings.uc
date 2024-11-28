//==============================================================================
//	Description
//
//	Created by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Tab_AudioSettings extends Settings_Tabs;

#exec OBJ LOAD FILE=PickupSounds.uax
#exec OBJ LOAD FILE=AnnouncerMale2K4.uax

var array<CacheManager.AnnouncerRecord> Announcers;

var string StatusPreviewSound, RewardPreviewSound;

var localized string	AudioModes[4],
						VoiceModes[4],
						AnnounceModes[3];


var automated GUISectionBackground	i_BG1, i_BG2, i_BG3;
var automated moSlider		sl_MusicVol, sl_EffectsVol, sl_VoiceVol, sl_TTS;
var automated moComboBox	co_Mode, co_Voices, co_Announce, co_RewardAnnouncer, co_StatusAnnouncer;
var automated moCheckbox 	ch_ReverseStereo, ch_MessageBeep, ch_AutoTaunt, ch_TTSIRC, ch_OnlyTeamTTS,
							ch_MatureTaunts, ch_LowDetail, ch_Default, ch_TTS, ch_VoiceChat;
var automated moButton      b_VoiceChat;

var float	fMusic, fEffects, fTTS;
var int		iVoice, iMode, iVoiceMode, iAnnounce;
var string	sStatAnnouncer, sRewAnnouncer;
var bool	bRev, bBeep, bAuto, bMature, bLow, bCompat, b3DSound, bEAX, bDefault, bTTS, bTTSIRC, bOnlyTeamTTS, bVoiceChat;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
    local bool bIsWin32;

	bIsWin32 = ( ( PlatformIsWindows() ) && ( !PlatformIs64Bit() ) );

	Super.InitComponent(MyController, MyOwner);

	if ( bIsWin32 )
	{
		for(i = 0;i < ArrayCount(AudioModes);i++)
			co_Mode.AddItem(AudioModes[i]);
	}
	else
	{
		co_Mode.AddItem("OpenAL");
	}

	for(i = 0;i < ArrayCount(VoiceModes);i++)
		co_Voices.AddItem(VoiceModes[i]);

	for(i = 0;i < ArrayCount(AnnounceModes);i++)
		co_Announce.AddItem(AnnounceModes[i]);

	i_BG2.WinWidth=0.503398;
	i_BG2.WinHeight=0.453045;
	i_BG2.WinLeft=0.004063;
	i_BG2.WinTop=0.540831;

	i_BG3.WinWidth=0.475078;
	i_BG3.WinHeight=0.453045;
	i_BG3.WinLeft=0.518712;
	i_BG3.WinTop=0.540831;

	i_BG1.ManageComponent(sl_MusicVol);
	i_BG1.ManageComponent(sl_EffectsVol);
	i_BG1.ManageComponent(co_Mode);
	i_BG1.ManageComponent(ch_LowDetail);
	i_BG1.ManageComponent(ch_Default);
	i_BG1.ManageComponent(ch_reverseStereo);
	i_BG1.ManageComponent(co_Voices);
	i_BG1.ManageComponent(ch_MatureTaunts);
	i_BG1.ManageComponent(ch_AutoTaunt);
	i_BG1.ManageComponent(ch_MessageBeep);

	i_BG2.ManageComponent(sl_VoiceVol);
	i_BG2.ManageComponent(co_Announce);

//	i_BG3.ManageComponent(sl_TTS);
	i_BG3.ManageComponent(ch_TTS);
	i_BG3.ManageComponent(ch_TTSIRC);
	i_BG3.ManageComponent(ch_OnlyTeamTTS);
	i_BG3.ManageComponent(ch_VoiceChat);
	i_BG3.ManageComponent(b_VoiceChat);

	i_BG2.ManageComponent(co_StatusAnnouncer);
	i_BG2.ManageComponent(co_RewardAnnouncer);

	class'CacheManager'.static.GetAnnouncerList( Announcers );
	for ( i = 0; i < Announcers.Length; i++ )
	{
		if ( Announcers[i].FriendlyName != "" )
		{
			co_StatusAnnouncer.AddItem( Announcers[i].FriendlyName,,string(i) );
	        co_RewardAnnouncer.AddItem( Announcers[i].FriendlyName,,string(i) );
		}
	}

	// !!! FIXME: Might use a preinstalled system OpenAL in the future on
	// !!! FIXME:  Mac or Unix, but for now, we don't...  --ryan.
	if ( !PlatformIsWindows() )
		ch_Default.DisableMe();
}

function ResetClicked()
{
	local class<AudioSubSystem> A;
	local class UnrealPlayerClass;
	local PlayerController PC;
	local int i;

	Super.ResetClicked();

	PC = PlayerOwner();

	A = class<AudioSubSystem>(DynamicLoadObject(GetNativeClassName("Engine.Engine.AudioDevice"), Class'Class'));
	A.static.ResetConfig();

	class'Hud'.static.ResetConfig("bMessageBeep");
	class'LevelInfo'.static.ResetConfig("bLowSoundDetail");

	class'PlayerController'.static.ResetConfig("bAutoTaunt");
	class'PlayerController'.static.ResetConfig("bNoMatureLanguage");
	class'PlayerController'.static.ResetConfig("bNoAutoTaunts");
	class'PlayerController'.static.ResetConfig("bNoVoiceTaunts");
	class'PlayerController'.static.ResetConfig("bNoVoiceMessages");
	class'PlayerController'.static.ResetConfig("AnnouncerLevel");
	class'PlayerController'.static.ResetConfig("bNoTextToSpeechVoiceMessages");
//	class'PlayerController'.static.ResetConfig("TextToSpeechVoiceVolume");
	class'PlayerController'.static.ResetConfig("bOnlySpeakTeamText");
	class'UT2K4IRC_Page'.static.ResetConfig("bIRCTextToSpeechEnabled");

	UnrealPlayerClass = class(DynamicLoadObject("UnrealGame.UnrealPlayer",class'Class'));
	if ( UnrealPlayerClass != None )
	{
		UnrealPlayerClass.static.ResetConfig("CustomRewardAnnouncerPack");
		UnrealPlayerClass.static.ResetConfig("CustomStatusAnnouncerPack");
	}

	for (i = 0; i < Components.Length; i++)
		Components[i].LoadINI();
}

function InternalOnLoadINI(GUIComponent Sender, string s)
{
	local int i;
	local PlayerController PC;
	local bool bIsWin32;

	PC = PlayerOwner();

	switch (Sender)
	{
	case sl_VoiceVol:
		iVoice = PC.AnnouncerVolume;
     	sl_VoiceVol.SetComponentValue(iVoice,true);
		break;

	case sl_MusicVol:
		fMusic = float(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice MusicVolume"));
		sl_MusicVol.SetComponentValue(fMusic,true);
		break;

	case sl_EffectsVol:
		fEffects = float(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice SoundVolume"));
		sl_EffectsVol.SetComponentValue(fEffects,true);
		break;
/*
	case sl_TTS:
		fTTS = PC.TextToSpeechVoiceVolume;
		sl_TTS.SetComponentValue(fTTS,true);
		break;
*/
	case co_Mode:
		iMode = 1;
		bIsWin32 = ( ( PlatformIsWindows() ) && ( !PlatformIs64Bit() ) );
		if ( !bIsWin32 )
		{
			bCompat = False;
			b3DSound = True;
			iMode = 0;
		}
		else
		{
			if ( bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice CompatibilityMode")) )
			{
				bCompat = True;
				iMode = 0;
			}

			if ( bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice Use3DSound")) )
			{
				b3DSound = True;
				iMode = 2;
			}

			if ( bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice UseEAX" )) )
			{
				bEAX = True;
				iMode = 3;
			}
		}
		co_Mode.SilentSetIndex(iMode);
		break;

	case ch_ReverseStereo:
		bRev = bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice ReverseStereo"));
		ch_ReverseStereo.SetComponentValue(bRev,true);
		break;

	case ch_Default:
		bDefault = bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice UseDefaultDriver"));
		ch_Default.SetComponentValue(bDefault,true);
		break;

	case co_Voices:
		if ( PC.bNoAutoTaunts ) iVoiceMode = 1;
		if ( PC.bNoVoiceTaunts ) iVoiceMode = 2;
		if ( PC.bNoVoiceMessages ) iVoiceMode = 3;
		co_Voices.SilentSetIndex(iVoiceMode);
		break;

	case ch_MessageBeep:
		bBeep = class'HUD'.default.bMessageBeep;
		ch_MessageBeep.SetComponentValue(bBeep,true);
		break;

	case ch_AutoTaunt:
		bAuto = PC.bAutoTaunt;
		ch_AutoTaunt.SetComponentValue(bAuto,true);
		break;

	case ch_TTS:
		bTTS = !PC.bNoTextToSpeechVoiceMessages;
		ch_TTS.SetComponentValue(bTTS,true);
		break;

	case ch_OnlyTeamTTS:
		bOnlyTeamTTS = PC.bOnlySpeakTeamText;
		ch_OnlyTeamTTs.SetComponentValue(bOnlyTeamTTS,True);
		break;

	case ch_MatureTaunts:
		bMature = !PC.bNoMatureLanguage;
		ch_MatureTaunts.SetComponentValue(bMature,true);
		break;

	case ch_LowDetail:
		bLow = PC.Level.bLowSoundDetail;

		// Make sure both are the same - LevelInfo.bLowSoundDetail take priority
		if ( bLow != bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice LowQualitySound" )) )
		{
			PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice LowQualitySound"@bLow);
			PC.ConsoleCommand("SOUND_REBOOT");

			// Restart music.
			if( PC.Level.Song != "" && PC.Level.Song != "None" )
				PC.ClientSetMusic( PC.Level.Song, MTRAN_Instant );

			else PC.ClientSetMusic( class'UT2K4MainMenu'.default.MenuSong, MTRAN_Instant );
		}

		ch_LowDetail.SetComponentValue(bLow,true);
		break;

	case co_Announce:
		iAnnounce = PC.AnnouncerLevel;
		co_Announce.SilentSetIndex(iAnnounce);
		break;

	case co_RewardAnnouncer:

		if ( PC.IsA('UnrealPlayer') )
		{
			sStatAnnouncer = PC.GetCustomStatusAnnouncerClass();
			sRewAnnouncer = PC.GetCustomRewardAnnouncerClass();
		}
		else
		{
			sStatAnnouncer = class'UnrealPlayer'.default.CustomStatusAnnouncerPack;
			sRewAnnouncer = class'UnrealPlayer'.default.CustomRewardAnnouncerPack;
		}

		i = FindAnnouncerClassIndex(sStatAnnouncer);
		if ( i != -1 )
		{
			i = co_StatusAnnouncer.FindIndex(Announcers[i].FriendlyName);
			if ( i != -1 )
				co_StatusAnnouncer.SilentSetIndex(i);
		}

		i = FindAnnouncerClassIndex(sRewAnnouncer);
		if (i != -1)
		{
			i = co_RewardAnnouncer.FindIndex(Announcers[i].FriendlyName);
			if ( i != -1 )
				co_RewardAnnouncer.SilentSetIndex(i);
		}

		break;

	case ch_TTSIRC:
		bTTSIRC = class'UT2K4IRC_Page'.default.bIRCTextToSpeechEnabled;
		ch_TTSIRC.SetComponentValue(bTTSIRC,true);
		break;

	case ch_VoiceChat:
		bVoiceChat = bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice UseVoIP"));
		ch_VoiceChat.SetComponentValue(bVoiceChat,true);
		break;

	default:
		log(Name@"Unknown component calling LoadINI:"$ GUIMenuOption(Sender).Caption);
		GUIMenuOption(Sender).SetComponentValue(s,true);
	}
}

function InternalOnChange(GUIComponent Sender)
{
	local PlayerController PC;
	local float AnnouncerVol;
	local Sound snd;
	local int AnnouncerIdx;
	local bool bIsWin32;

	bIsWin32 = ( ( PlatformIsWindows() ) && ( !PlatformIs64Bit() ) );

	Super.InternalOnChange(Sender);
	PC = PlayerOwner();

	switch(Sender)
	{
		case sl_VoiceVol:
			iVoice = sl_VoiceVol.GetValue();
			AnnouncerVol = 2.0 * FClamp(0.1 + iVoice*0.225,0.2,1.0);
			if ( co_StatusAnnouncer == None )
				return;

			snd = sound(co_StatusAnnouncer.GetObject());
			if ( snd == None && Announcers.Length > 0 )
			{
				snd = Sound(DynamicLoadObject(Announcers[AnnouncerIdx].PackageName $ "." $ StatusPreviewSound,class'Sound'));
				if ( snd == none )
					snd = Sound(DynamicLoadObject(Announcers[AnnouncerIdx].FallbackPackage $ "." $ StatusPreviewSound,class'Sound'));

				co_StatusAnnouncer.MyComboBox.List.SetObjectAtIndex(co_StatusAnnouncer.MyComboBox.List.Index,snd);
			}

			if ( snd != None )
				PC.PlaySound(snd,SLOT_Talk,AnnouncerVol);

			break;

		case sl_MusicVol:
			fMusic = sl_MusicVol.GetValue();
			PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice MusicVolume"@fMusic);
			PC.ConsoleCommand("SetMusicVolume"@fMusic);

			if( PC.Level.Song != "" && PC.Level.Song != "None" )
				PC.ClientSetMusic( PC.Level.Song, MTRAN_Instant );
			else PC.ClientSetMusic( class'UT2K4MainMenu'.default.MenuSong, MTRAN_Instant );
			break;

		case sl_EffectsVol:
			fEffects = sl_EffectsVol.GetValue();
			PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice SoundVolume"@fEffects);
			PC.ConsoleCommand("stopsounds");
			PC.PlaySound(sound'PickupSounds.AdrenelinPickup');
			break;
/*
		case sl_TTS:
			fTTS = sl_TTS.GetValue();
		// Do not preview TTS voice volume, since there isn't any way to truly represent the way it will sound in-game
//				PC.TextToSpeech( "Fo Shizzle my nizzle", fTTS );
			break;
*/
		case co_Mode:
			if ( !bIsWin32 )  // Simple OpenAL abstraction...  --ryan.
				break;

			iMode = co_Mode.GetIndex();
			if (iMode > 1)
				ShowPerformanceWarning();

			bCompat = iMode < 1;
			b3DSound = iMode > 1;
			bEAX = iMode > 2;
	        PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice CompatibilityMode"@bCompat);
	        PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice Use3DSound"@b3DSound);
	        PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice UseEAX"@bEAX);
			PC.ConsoleCommand("SOUND_REBOOT");

			// Restart music.
			if( PC.Level.Song != "" && PC.Level.Song != "None" )
				PC.ClientSetMusic( PC.Level.Song, MTRAN_Instant );
			else PC.ClientSetMusic( class'UT2K4MainMenu'.default.MenuSong, MTRAN_Instant );
			break;

		case ch_ReverseStereo:
			bRev = ch_ReverseStereo.IsChecked();
			break;

		case ch_MessageBeep:
			bBeep = ch_MessageBeep.IsChecked();
			break;

		case ch_AutoTaunt:
			bAuto = ch_AutoTaunt.IsChecked();
			break;

		case ch_TTS:
			bTTS = ch_TTS.IsChecked();
			break;

		case ch_MatureTaunts:
			bMature = ch_MatureTaunts.IsChecked();
			break;

		case co_Voices:
			iVoiceMode = co_Voices.GetIndex();
			break;

		case ch_Default:
			bDefault = ch_Default.IsChecked();
			PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice UseDefaultDriver"@bDefault);
			PC.ConsoleCommand("SOUND_REBOOT");
			break;

		case ch_LowDetail:
			bLow = ch_LowDetail.IsChecked();

			PC.Level.bLowSoundDetail = bLow;
			PC.Level.StaticSaveConfig();

			PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice LowQualitySound"@bLow);
			PC.ConsoleCommand("SOUND_REBOOT");

			// Restart music.
			if( PC.Level.Song != "" && PC.Level.Song != "None" )
				PC.ClientSetMusic( PC.Level.Song, MTRAN_Instant );
			else PC.ClientSetMusic( class'UT2K4MainMenu'.default.MenuSong, MTRAN_Instant );
			break;

		case co_Announce:
			iAnnounce = co_Announce.GetIndex();
			break;

		case co_RewardAnnouncer:
			AnnouncerIdx = int(co_RewardAnnouncer.GetExtra());
			sRewAnnouncer = Announcers[AnnouncerIdx].ClassName;

			AnnouncerVol = 2.0 * FClamp(0.1 + iVoice*0.225,0.2,1.0);
			snd = sound(co_RewardAnnouncer.GetObject());
			if ( snd == None )
			{
				snd = Sound(DynamicLoadObject(Announcers[AnnouncerIdx].PackageName $ "." $ RewardPreviewSound,class'Sound'));
				if ( snd == none )
					snd = Sound(DynamicLoadObject(Announcers[AnnouncerIdx].FallbackPackage $ "." $ RewardPreviewSound,class'Sound'));

				co_RewardAnnouncer.MyComboBox.List.SetObjectAtIndex(co_RewardAnnouncer.MyComboBox.List.Index,snd);
			}
			PC.PlaySound(snd,SLOT_Talk,AnnouncerVol);
			break;

		case co_StatusAnnouncer:
			AnnouncerIdx = int(co_StatusAnnouncer.GetExtra());
			sStatAnnouncer = Announcers[AnnouncerIdx].ClassName;

			AnnouncerVol = 2.0 * FClamp(0.1 + iVoice*0.225,0.2,1.0);
			snd = sound(co_StatusAnnouncer.GetObject());
			if ( snd == None )
			{
				snd = Sound(DynamicLoadObject(Announcers[AnnouncerIdx].PackageName $ "." $ StatusPreviewSound,class'Sound'));
				if ( snd == none )
					snd = Sound(DynamicLoadObject(Announcers[AnnouncerIdx].FallbackPackage $ "." $ StatusPreviewSound,class'Sound'));

				co_StatusAnnouncer.MyComboBox.List.SetObjectAtIndex(co_StatusAnnouncer.MyComboBox.List.Index,snd);
			}
			PC.PlaySound(snd,SLOT_Talk,AnnouncerVol);
			break;

		case ch_TTSIRC:
			bTTSIRC = ch_TTSIRC.IsChecked();
			break;

		case ch_VoiceChat:
			bVoiceChat = ch_VoiceChat.IsChecked();
			break;

		case ch_OnlyTeamTTS:
			bOnlyTeamTTS = ch_OnlyTeamTTS.IsChecked();
			break;
	}
}

function SaveSettings()
{
	local PlayerController PC;
	local bool bSave, bReboot;

	Super.SaveSettings();
	PC = PlayerOwner();

	if (PC.AnnouncerLevel != iAnnounce)
	{
		PC.AnnouncerLevel = iAnnounce;
		PC.default.AnnouncerLevel = PC.AnnouncerLevel;
		bSave = True;
	}

	if (PC.AnnouncerVolume != iVoice)
	{
		PC.AnnouncerVolume = iVoice;
		PC.default.AnnouncerVolume = iVoice;
		bSave = True;
	}

/*	if ( PC.TextToSpeechVoiceVolume != fTTS )
	{
		PC.TextToSpeechVoiceVolume = fTTS;
		bSave = True;
	}
*/
	if ( PC.bOnlySpeakTeamText != bOnlyTeamTTS )
	{
		PC.bOnlySpeakTeamText = bOnlyTeamTTS;
		bSave = True;
	}

	if ( PC.bNoTextToSpeechVoiceMessages == bTTS )
	{
		PC.bNoTextToSpeechVoiceMessages = !bTTS;
		bSave = True;
	}

	if ( class'UT2K4IRC_Page'.default.bIRCTextToSpeechEnabled != bTTSIRC )
	{
		class'UT2K4IRC_Page'.default.bIRCTextToSpeechEnabled = bTTSIRC;
		class'UT2K4IRC_Page'.static.StaticSaveConfig();
	}

	if (PC.bNoMatureLanguage == bMature)
	{
		PC.bNoMatureLanguage = !bMature;
		PC.default.bNoMatureLanguage = !bMature;
		bSave = True;
	}

	if (PC.bNoAutoTaunts != iVoiceMode > 0)
	{
		PC.bNoAutoTaunts = iVoiceMode > 0;
		PC.default.bNoAutoTaunts = PC.bNoAutoTaunts;
		bSave = True;
	}

	if (PC.bNoVoiceTaunts != iVoiceMode > 1)
	{
		PC.bNoVoiceTaunts = iVoiceMode > 1;
		PC.default.bNoVoiceTaunts = PC.bNoVoiceTaunts;
		bSave = True;
	}

	if (PC.bNoVoiceMessages != iVoiceMode == 3)
	{
		PC.bNoVoiceMessages = iVoiceMode == 3;
		PC.default.bNoVoiceMessages = PC.bNoVoiceMessages;
		bSave = True;
	}

	if (fMusic != sl_MusicVol.GetValue())
		PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice MusicVolume"@fMusic);

	if (fEffects != sl_EffectsVol.GetValue())
		PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice SoundVolume"@fEffects);

	if (bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice ReverseStereo")) != bRev)
		PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice ReverseStereo"@bRev);

	if (bDefault != bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice UseDefaultDriver")))
	{
		PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice UseDefaultDriver"@bDefault);
		bReboot = True;
	}

	if( PC.MyHud != None )
	{
		if ( PC.myHUD.bMessageBeep != bBeep )
		{
			PC.myHUD.bMessageBeep = bBeep;
			PC.myHUD.SaveConfig();
		}
	}

	else
	{
		if ( class'HUD'.default.bMessageBeep != bBeep )
		{
			class'HUD'.default.bMessageBeep = bBeep;
			class'HUD'.static.StaticSaveConfig();
		}
	}

    if ( !PC.Level.IsDemoBuild() && PC.IsA('UnrealPlayer') )
    {
    	if ( PC.GetCustomStatusAnnouncerClass() != sStatAnnouncer )
    	{
    		PC.SetCustomStatusAnnouncerClass(sStatAnnouncer);
    		bSave = True;
    	}

    	if ( PC.GetCustomRewardAnnouncerClass() != sRewAnnouncer )
    	{
    		PC.SetCustomRewardAnnouncerClass(sRewAnnouncer);
    		bSave = True;
    	}
    }

	if (bVoiceChat != bool(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice UseVoIP")))
	{
		if (bVoiceChat)
			PC.EnableVoiceChat();
		else PC.DisableVoiceChat();

		bReboot = False;
	}

	if (bSave)
		PC.SaveConfig();

	if ( PC.bAutoTaunt != bAuto )
		PC.SetAutoTaunt(bAuto);

    if ( !PC.Level.IsDemoBuild() && !PC.IsA('UnrealPlayer') )
    {
    	if ( !(class'UnrealPlayer'.default.CustomStatusAnnouncerPack ~= sStatAnnouncer) ||
    		 !(class'UnrealPlayer'.default.CustomRewardAnnouncerPack ~= sRewAnnouncer) )
    	{
	    	class'UnrealPlayer'.default.CustomStatusAnnouncerPack = sStatAnnouncer;
	    	class'UnrealPlayer'.default.CustomRewardAnnouncerPack = sRewAnnouncer;
	    	class'UnrealPlayer'.static.StaticSaveConfig();
	    }
    }

	if (bReboot)
		PC.ConsoleCommand("SOUND_REBOOT");
}

function int FindAnnouncerClassIndex( string AnnouncerClass )
{
	local int i;

	for ( i = 0; i < Announcers.Length; i++ )
		if ( Announcers[i].ClassName ~= AnnouncerClass )
			return i;

	return -1;
}

function string GetAnnouncerClass( int Index )
{
	if ( Index < 0 || Index >= Announcers.Length )
		return "";

	return Announcers[Index].ClassName;
}

function string GetAnnouncerPackage( int Index )
{
	if ( Index < 0 || Index >= Announcers.Length )
		return "";

	return Announcers[Index].PackageName;
}

defaultproperties
{
	Begin Object class=GUISectionBackground Name=AudioBK1
		WinWidth=0.987773
		WinHeight=0.502850
		WinLeft=0.004063
		WinTop=0.017393
		Caption="Sound Effects"
		NumColumns=2
		MaxPerColumn=5
	End Object
	i_BG1=AudioBK1

	Begin Object class=GUISectionBackground Name=AudioBK2
		WinWidth=0.987773
		WinHeight=0.517498
		WinLeft=0.004063
		WinTop=0.004372
		Caption="Announcer"
	End Object
	i_BG2=AudioBK2

	Begin Object class=GUISectionBackground Name=AudioBK3
		WinWidth=0.987773
		WinHeight=0.517498
		WinLeft=0.004063
		WinTop=0.004372
		Caption="Text To Speech"
	End Object
	i_BG3=AudioBK3

	Begin Object class=moSlider Name=AudioMusicVolume
		WinWidth=0.450000
		WinLeft=0.018164
		WinTop=0.070522
		Caption="Music Volume"
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		CaptionWidth=0.5
		ComponentWidth=-1
		bAutoSizeCaption=True
		MinValue=0.0
		MaxValue=1.0
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		INIDefault="0.5"
		Hint="Adjusts the volume of the background music."
		TabOrder=0
	End Object
	sl_MusicVol=AudioMusicVolume

	Begin Object class=moComboBox Name=AudioMode
		WinWidth=0.450000
		WinLeft=0.018164
		WinTop=0.149739
		Caption="Audio Mode"
		INIOption="@Internal"
		INIDefault="Software 3D Audio"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Changes the audio system mode."
		CaptionWidth=0.5
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		bReadOnly=true
		TabOrder=1
	End Object
	co_Mode=AudioMode

	Begin Object class=moCheckBox Name=AudioLowDetail
		WinWidth=0.45
		WinLeft=0.018164
		WinTop=0.235052
		Caption="Low Sound Detail"
		INIOption="@Internal"
		INIDefault="False"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Lowers quality of sound."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
		TabOrder=2
	End Object
	ch_LowDetail=AudioLowDetail

	Begin Object class=moCheckBox Name=AudioDefaultDriver
		WinWidth=0.45
		WinLeft=0.018164
		WinTop=0.320365
		Caption="System Driver"
		Hint="Use system installed OpenAL driver"
		INIOption="@Internal"
		INIDefault="False"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		ComponentJustification=TXTA_Left
		TabOrder=3
	End Object
	ch_Default=AudioDefaultDriver

	Begin Object class=moCheckBox Name=AudioReverseStereo
		WinWidth=0.45
		WinLeft=0.018164
		WinTop=0.405678
		Caption="Reverse Stereo"
		Hint="Reverses the left and right audio channels."
		INIOption="@Internal"
		INIDefault="False"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		ComponentJustification=TXTA_Left
		TabOrder=4
	End Object
	ch_ReverseStereo=AudioReverseStereo

	Begin Object class=moSlider Name=AudioEffectsVolumeSlider
		WinWidth=0.450000
		WinLeft=0.524024
		WinTop=0.070522
		MinValue=0.0
		MaxValue=1.0
		Caption="Effects Volume"
		Hint="Adjusts the volume of all in game sound effects."
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		CaptionWidth=0.5
		ComponentWidth=-1
		bAutoSizeCaption=True
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		INIOption="@Internal"
		INIDefault="0.9"
		TabOrder=5
	End Object
	sl_EffectsVol=AudioEffectsVolumeSlider

	Begin Object class=moComboBox Name=AudioPlayVoices
		WinWidth=0.45
		WinLeft=0.524024
		WinTop=0.149739
		Caption="Play Voices"
		Hint="Defines the types of voice messages to play."
		INIOption="@Internal"
		INIDefault="All"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.5
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		bReadOnly=true
		TabOrder=6
		bHeightFromComponent=False
	End Object
	co_Voices=AudioPlayVoices

	Begin Object class=moCheckBox Name=AudioMatureTaunts
		WinWidth=0.45
		WinLeft=0.524024
		WinTop=0.235052
		Caption="Mature Taunts"
		Hint="Enables off-color commentary."
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
		TabOrder=7
	End Object
	ch_MatureTaunts=AudioMatureTaunts

	Begin Object class=moCheckBox Name=AudioAutoTaunt
		WinWidth=0.45
		WinLeft=0.524024
		WinTop=0.320365
		Caption="Auto-Taunt"
		Hint="Enables your in-game player to automatically taunt opponents."
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
		TabOrder=8
	End Object
	ch_AutoTaunt=AudioAutoTaunt

	Begin Object class=moCheckBox Name=AudioMessageBeep
		WinWidth=0.45
		WinLeft=0.524024
		WinTop=0.405678
		Caption="Message Beep"
		Hint="Enables a beep when receiving a text message from other players."
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
		TabOrder=9
	End Object
	ch_MessageBeep=AudioMessageBeep

	Begin Object class=moSlider Name=AudioAnnouncerVolume
		WinWidth=0.470703
		WinLeft=0.011329
		WinTop=0.597866
		Caption="Volume"
		Hint="Adjusts the volume of all in game voice messages."
		MinValue=1
		MaxValue=4
        bIntSlider=true
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		CaptionWidth=0.65
		ComponentWidth=-1
		bAutoSizeCaption=True
		TabOrder=10
	End Object
	sl_VoiceVol=AudioAnnouncerVolume

	Begin Object class=moComboBox Name=AudioAnnounce
		WinWidth=0.470703
		WinLeft=0.011329
		WinTop=0.711079
		Caption="Announcements"
		Hint="Adjusts the amount of in-game announcements."
		INIOption="@Internal"
		INIDefault="All"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.65
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		bReadOnly=true
		TabOrder=11
	End Object
	co_Announce=AudioAnnounce

	Begin Object class=moComboBox Name=AudioStatusAnnouncer
		WinWidth=0.470703
		WinLeft=0.011329
		WinTop=0.801035
		Caption="Status Announcer"
		Hint="The Status announcer relays important information about the game to tournament players and spectators."
		INIOption="@Internal"
		OnChange=InternalOnChange
		CaptionWidth=0.65
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		bReadOnly=true
		TabOrder=12
	End Object
	co_StatusAnnouncer=AudioStatusAnnouncer

	Begin Object class=moComboBox Name=AudioRewardAnnouncer
		WinWidth=0.470703
		WinLeft=0.011329
		WinTop=0.890991
		Caption="Reward Announcer"
		Hint="Each tournament player is linked to a Reward announcer that informs you when you've demonstrated exceptional combat skills."
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.65
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		bReadOnly=true
		TabOrder=13
	End Object
	co_RewardAnnouncer=AudioRewardAnnouncer
/*
	Begin Object class=moSlider Name=AudioTTSVolumeSlider
		WinWidth=0.461134
		WinLeft=0.527734
		WinTop=0.597866
		Caption="Volume"
		Hint="Adjusts the volume of Text-To-Speech messages."
		MinValue=1
		MaxValue=4
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.65
		ComponentWidth=-1
		bAutoSizeCaption=True
		LabelJustification=TXTA_Left
		ComponentJustification=TXTA_Right
		TabOrder=14
	End Object
	sl_TTS=AudioTTSVolumeSlider
*/
	Begin Object class=moCheckBox Name=AudioEnableTTS
		WinWidth=0.461134
		WinLeft=0.527734
		WinTop=0.685037
		Caption="Enable In Game"
		Hint="Enables Text-To-Speech message processing"
		INIOption="@Internal"
		INIDefault="False"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
		TabOrder=15
	End Object
	ch_TTS=AudioEnableTTS

	Begin Object class=moCheckbox Name=IRCTextToSpeech
		WinWidth=0.461134
		WinLeft=0.527734
		WinTop=0.755462
		Caption="Enable on IRC"
		Hint="Enables Text-To-Speech processing in the IRC client (only messages from active tab is processed)"
		IniOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		ComponentJustification=TXTA_Left
		TabOrder=16
	End Object
	ch_TTSIRC=IRCTextToSpeech

	Begin Object CLass=moCheckBox Name=OnlyTeamTTSCheck
		WinWidth=0.461134
		WinLeft=0.527734
		WinTop=0.755462
		Caption="Team Messages Only"
		Hint="If enabled, only team messages will be spoken in team games, unless the match or round is over."
		IniOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		CaptionWidth=0.94
		ComponentJustification=TXTA_Left
		TabOrder=17
	End Object
	ch_OnlyTeamTTS=OnlyTeamTTSCheck

	Begin Object class=moCheckBox Name=EnableVoiceChat
		WinWidth=0.461134
		WinLeft=0.527734
		WinTop=0.834777
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		OnChange=InternalOnChange
		Hint="Enables the voice chat system during online matches."
		Caption="Voice Chat"
		CaptionWidth=0.94
		bSquare=true
		TabOrder=18
		ComponentJustification=TXTA_Left
	End Object
	ch_VoiceChat=EnableVoiceChat

	Begin Object Class=moButton Name=VoiceOptions
		WinWidth=0.461134
		WinLeft=0.527734
		WinHeight=0.05
		WinTop=0.909065
		Caption="Voice Options"
		ButtonCaption="Configure"
		MenuClass="GUI2K4.VoiceChatConfig"
		MenuTitle="Voice Chat Options"
		CaptionWidth=0.5
		TabOrder=19
	End Object
	b_VoiceChat=VoiceOptions

	WinTop=0.15
	WinLeft=0
	WinWidth=1
	WinHeight=0.74
	bAcceptsInput=false

	AudioModes[0]="Safe Mode"
	AudioModes[1]="3D Audio"
	AudioModes[2]="H/W 3D Audio"
	AudioModes[3]="H/W 3D + EAX"

	VoiceModes[0]="All"
	VoiceModes[1]="No auto-taunts"
	VoiceModes[2]="No taunts"
	VoiceModes[3]="None"

	AnnounceModes[0]="None"
	AnnounceModes[1]="Minimal"
	AnnounceModes[2]="All"

	PanelCaption="Audio"

	StatusPreviewSound="1_minute_remains"
	RewardPreviewSound="unstoppable"
}

//==============================================================================
//  Created on: 11/19/2003
//  Configuration page for Voice Chat Options
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class VoiceChatConfig extends GUICustomPropertyPage;

var automated moEditBox     ed_ChatPassword;
var automated GUIButton     b_QuickBinds;
var automated moEditBox     ed_Active;
var automated moSlider      sl_VoiceVol;
var automated moCheckBox    ch_AJPublic, ch_AJLocal, ch_AJTeam, ch_AutoSpeak;
var automated moComboBox    co_Quality, co_LANQuality;

var bool bAJPublic, bAJLocal, bAJTeam, bAutoSpeak;
var string  sPwd, sCodec, sLANCodec, sActive, QuickBindMenu;
var float fVoice;

var class<VoiceChatReplicationInfo> VoiceChatClass;
var string VoiceChatClassName;

var array<string>    InstalledCodecs;

var localized string KeyNameCaption;
var localized string ResetCaption, ResetHint;

function InitComponent( GUIController InController, GUIComponent InOwner )
{
	local int i;
	local string CName, CDesc;
	local class<VoiceChatReplicationInfo> Cls;

	Super.InitComponent( InController, InOwner );

	// Need to load TeamVoiceReplicationInfo to get team channel names
	Cls = class<VoiceChatReplicationInfo>( DynamicLoadObject( VoiceChatClassName, class'Class') );
	if ( Cls != None )
		VoiceChatClass = Cls;

	VoiceChatClass.static.GetInstalledCodecs( InstalledCodecs );
	for ( i = 0; i < InstalledCodecs.Length; i++ )
	{
		VoiceChatClass.static.GetCodecInfo( InstalledCodecs[i], CName, CDesc );
		co_Quality.AddItem( CName,, InstalledCodecs[i] );
		co_LANQuality.AddItem( CName,, InstalledCodecs[i] );
	}

   ed_ChatPassword.MaskText(True);

	b_Cancel.Caption = ResetCaption;
	b_Cancel.Hint = ResetHint;
	b_Cancel.OnClick = ResetClick;

    ed_ChatPassword.MaskText(True);

	sb_Main.ManageComponent(sl_VoiceVol);
    sb_Main.ManageComponent(ch_AJPublic);
    sb_Main.ManageComponent(ch_AJLocal);
    sb_Main.ManageComponent(ch_AJTeam);
	sb_Main.ManageComponent(ch_AutoSpeak);
	sb_Main.ManageComponent(ed_Active);
    sb_Main.ManageComponent(ed_ChatPassword);
    sb_Main.ManageComponent(co_Quality);
    sb_Main.ManageComponent(co_LanQuality);

}


function InternalOnLoadINI(GUIComponent Sender, string s)
{
    local PlayerController PC;

    if (GUIMenuOption(Sender) != None)
    {
        PC = PlayerOwner();

        switch (GUIMenuOption(Sender).Caption)
        {
            case ed_ChatPassword.Caption:
                sPwd = PC.ChatPassword;
                ed_ChatPassword.SetComponentValue(sPwd, True);
                break;

            case sl_VoiceVol.Caption:
            	fVoice = float(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice VoiceVolume"));
            	sl_VoiceVol.SetComponentValue(fVoice, True);
            	break;

			case co_Quality.Caption:
				sCodec = PC.VoiceChatCodec;
				co_Quality.SetExtra(sCodec,true);
				break;

			case co_LANQuality.Caption:
				sLANCodec = PC.VoiceChatLANCodec;
				co_LANQuality.SetExtra(sLANCodec,true);
				break;

			case ch_AJPublic.Caption:
				bAJPublic = bool(PC.AutoJoinMask & 1);
				ch_AJPublic.SetComponentValue(bAJPublic,True);
				break;

			case ch_AJLocal.Caption:
				bAJLocal = bool(PC.AutoJoinMask & 2);
				ch_AJLocal.SetComponentValue(bAJLocal,True);
				break;

			case ch_AJTeam.Caption:
				bAJTeam = bool(PC.AutoJoinMask & 4);
				ch_AJTeam.SetComponentValue(bAJTeam,True);
				break;

			case ch_AutoSpeak.Caption:
				bAutoSpeak = PC.bEnableInitialChatRoom;
				if ( bAutoSpeak )
					EnableComponent(ed_Active);
				else DisableComponent(ed_Active);

				ch_AutoSpeak.SetComponentValue(bAutoSpeak, True);
				break;

			case ed_Active.Caption:
				sActive = PC.DefaultActiveChannel;
				ed_Active.SetComponentValue(sActive, True);
				break;
		}
   }
}

function InternalOnChange(GUIComponent Sender)
{
    local PlayerController PC;

    if (GUIMenuOption(Sender) != None)
    {
        PC = PlayerOwner();

        switch (GUIMenuOption(Sender).Caption)
        {
            case ed_ChatPassword.Caption:
                sPwd = ed_ChatPassword.GetText();
                break;

            case sl_VoiceVol.Caption:
            	fVoice = sl_VoiceVol.GetValue();
            	break;

			case co_Quality.Caption:
				sCodec = co_Quality.GetExtra();
				break;

			case co_LANQuality.Caption:
				sLANCodec = co_LANQuality.GetExtra();
				break;

			case ch_AJPublic.Caption:
				bAJPublic = ch_AJPublic.IsChecked();
				break;

			case ch_AJLocal.Caption:
				bAJLocal = ch_AJLocal.IsChecked();
				break;

			case ch_AJTeam.Caption:
				bAJTeam = ch_AJTeam.IsChecked();
				break;

			case ch_AutoSpeak.Caption:
				bAutoSpeak = ch_AutoSpeak.IsChecked();
				if ( bAutoSpeak )
					EnableComponent(ed_Active);
				else DisableComponent(ed_Active);
				break;

			case ed_Active.Caption:
				sActive = ed_Active.GetText();
				break;
        }
    }
}

function Closed(GUIComponent Sender, bool bCancelled)
{
    local PlayerController PC;
    local bool bSave;

	Super.Closed(Sender,bCancelled);
    PC = PlayerOwner();

	if ( bAJPublic != bool(PC.AutoJoinMask & 1) )
	{
		if ( bAJPublic )
			PC.AutoJoinMask = PC.AutoJoinMask | 1;
		else PC.AutoJoinMask = PC.AutoJoinMask & ~1;
		bSave = True;
	}

	if ( bAJLocal != bool(PC.AutoJoinMask & 2) )
	{
		if ( bAJLocal )
			PC.AutoJoinMask = PC.AutoJoinMask | 2;
		else PC.AutoJoinMask = PC.AutoJoinMask & ~2;
		bSave = True;
	}

	if ( bAJTeam != bool(PC.AutoJoinMask & 4) )
	{
		if ( bAJTeam )
			PC.AutoJoinMask = PC.AutoJoinMask | 4;
		else PC.AutoJoinMask = PC.AutoJoinMask & ~4;
		bSave = True;
	}

    if ( fVoice != float(PC.ConsoleCommand("get ini:Engine.Engine.AudioDevice VoiceVolume")) )
    	PC.ConsoleCommand("set ini:Engine.Engine.AudioDevice VoiceVolume"@fVoice);

    if ( sCodec != PC.VoiceChatCodec )
    {
    	PC.VoiceChatCodec = sCodec;
    	bSave = True;
    }

    if ( sLANCodec != PC.VoiceChatLANCodec )
    {
    	PC.VoiceChatLANCodec = sLANCodec;
    	bSave = True;
    }

    if ( PC.bEnableInitialChatRoom != bAutoSpeak )
    {
    	PC.bEnableInitialChatRoom = bAutoSpeak;
    	bSave = True;
    }

	if ( !(PC.DefaultActiveChannel ~= sActive) )
	{
		PC.DefaultActiveChannel = sActive;
		bSave = True;
	}

    if (PC.ChatPassword != sPwd)
    {
        PC.SetChatPassword(sPwd);
        bSave = False;
    }

    if (bSave)
        PC.SaveConfig();
}

function bool ResetClick(GUIComponent Sender)
{
	local int i;
	local class<AudioSubSystem> AudioClass;

	if ( Sender == b_Cancel ) // Remapped to Reset
	{
		AudioClass = class<AudioSubSystem>(DynamicLoadObject(PlayerOwner().ConsoleCommand("get ini:Engine.Engine.AudioDevice Class"),class'Class'));
	    AudioClass.static.ResetConfig("VoiceVolume");

		class'Engine.PlayerController'.static.ResetConfig("VoiceChatCodec");
		class'Engine.PlayerController'.static.ResetConfig("VoiceChatLANCodec");
	    class'Engine.PlayerController'.static.ResetConfig("AutoJoinMask");
	    class'Engine.PlayerController'.static.ResetConfig("ChatPassword");
	    class'Engine.PlayerController'.static.ResetConfig("DefaultActiveChannel");
	    class'Engine.PlayerController'.static.ResetConfig("bEnableInitialChatRoom");


	    for (i = 0; i < Components.Length; i++)
	        Components[i].LoadINI();

	}
	return true;
}

DefaultProperties
{
	bAcceptsInput=false

    Begin Object Class=moSlider Name=VoiceVolume
		WinWidth=0.408907
		WinLeft=0.518507
		WinTop=0.142484
    	Caption="Voice Chat Volume"
    	Hint="Adjusts the volume of other players' voice chat communication."
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadIni
    	OnChange=InternalOnChange
    	RenderWeight=1.04
	    TabOrder=0
	    MinValue=1.0
	    MaxValue=10.0
		CaptionWidth=0.6
		ComponentWidth=-1
		bAutoSizeCaption=True
    	ComponentJustification=TXTA_Right
    	LabelJustification=TXTA_Left
    End Object
    sl_VoiceVol=VoiceVolume

    Begin Object Class=moCheckBox Name=AutoJoinPublic
		WinWidth=0.826652
		WinLeft=0.086280
		WinTop=0.042496
    	Caption="Autojoin Public Channel"
    	Hint="Automatically join the 'Public' channel upon connecting to a server."
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadIni
    	OnChange=InternalOnChange
    	bSquare=True
    	ComponentJustification=TXTA_Right
    	LabelJustification=TXTA_Left
    	ComponentWidth=-1
    	CaptionWidth=0.94
    	TabOrder=1
   End Object
    ch_AJPublic=AutoJoinPublic

    Begin Object Class=moCheckBox Name=AutoJoinLocal
		WinWidth=0.826652
		WinLeft=0.086280
		WinTop=0.145784
    	Caption="Autojoin Local Channel"
    	Hint="Automatically join the 'Local' channel upon connecting to a server."
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadIni
    	OnChange=InternalOnChange
    	bSquare=True
    	ComponentJustification=TXTA_Right
    	LabelJustification=TXTA_Left
    	ComponentWidth=-1
    	CaptionWidth=0.94
    	TabOrder=2
   End Object
    ch_AJLocal=AutoJoinLocal

    Begin Object Class=moCheckBox Name=AutoJoinTeam
		WinWidth=0.440910
		WinLeft=0.022803
		WinTop=0.226937
    	Caption="Autojoin Team Channel"
    	Hint="Automatically join the 'Team' channel upon connecting to a server."
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadIni
    	OnChange=InternalOnChange
    	bSquare=True
    	ComponentJustification=TXTA_Right
    	LabelJustification=TXTA_Left
    	ComponentWidth=-1
    	CaptionWidth=0.8
    	TabOrder=3
   End Object
    ch_AJTeam=AutoJoinTeam

    Begin Object Class=moCheckBox Name=AutoSpeakCheckbox
		WinWidth=0.442638
		WinHeight=0.060000
		WinLeft=0.039812
		WinTop=0.603526
    	Caption="Auto-select Active Channel"
    	Hint="Automatically set an active channel when you join a server.  The default channel is determined by the gametype, but you can specify your own using the editbox below"
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadIni
    	OnChange=InternalOnChange
    	bSquare=True
    	ComponentJustification=TXTA_Right
    	LabelJustification=TXTA_Left
    	ComponentWidth=-1
    	CaptionWidth=0.8
    	TabOrder=3
		bBoundToParent=True
		bScaleToParent=True
    End Object
    ch_AutoSpeak=AutoSpeakCheckbox


    Begin Object Class=moEditBox Name=ChatPasswordEdit
		WinWidth=0.420403
		WinLeft=0.032569
		WinTop=0.332828
        Hint="Set a password on your personal chat room to limit who is allowed to join"
        Caption="Chat Password"
        IniOption="@Internal"
        OnLoadIni=InternalOnLoadIni
        OnChange=InternalOnChange
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
    	CaptionWidth=0.6
    	ComponentWidth=-1
    	bAutoSizeCaption=True
        TabOrder=4
   End Object
   ed_ChatPassword=ChatPasswordEdit

    Begin Object Class=moEditBox Name=DefaultActiveChannelEditBox
		WinWidth=0.420403
		WinLeft=0.032569
		WinTop=0.757277
        Hint="Enter the name of the channel to speak on by default when you join the server.  To use the default chatroom for whichever gametype you're playing, leave this field empty"
        Caption="Default Channel Name"
        IniOption="@Internal"
        OnLoadIni=InternalOnLoadIni
        OnChange=InternalOnChange
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
    	CaptionWidth=0.6
    	ComponentWidth=-1
        TabOrder=4
	End Object
    ed_Active=DefaultActiveChannelEditBox


    Begin Object class=moComboBox Name=VoiceQuality
		WinWidth=0.408907
		WinLeft=0.523390
		WinTop=0.241391
    	Caption="Internet Quality"
    	Hint="Determines the codec used to transmit voice chat to and from internet servers."
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadINI
    	OnChange=InternalOnChange
    	TabOrder=5
    	bReadOnly=True
    	CaptionWidth=0.6
    	ComponentWidth=-1
    	bAutoSizeCaption=True
        ComponentJustification=TXTA_Right
       LabelJustification=TXTA_Left
    End Object
    co_Quality=VoiceQuality

    Begin Object class=moComboBox Name=VoiceQualityLAN
		WinWidth=0.408907
		WinLeft=0.523390
		WinTop=0.333786
    	Caption="LAN Quality"
    	Hint="Determines the codec used to transmit voice chat to and from LAN servers."
    	IniOption="@Internal"
    	OnLoadINI=InternalOnLoadINI
    	OnChange=InternalOnChange
    	TabOrder=6
    	bReadOnly=True
    	CaptionWidth=0.6
    	ComponentWidth=-1
    	bAutoSizeCaption=True
        ComponentJustification=TXTA_Right
        LabelJustification=TXTA_Left
    End Object
    co_LANQuality=VoiceQualityLAN

	VoiceChatClassName="UnrealGame.TeamVoiceReplicationInfo"
	VoiceChatClass=class'Engine.VoiceChatReplicationInfo'
	ResetCaption="Reset"
	ResetHint="Reset values for all options to their default values"
	WindowName="Voice Chat Configuration"


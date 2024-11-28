/*=============================================================================
	AudioSubsystem.cpp: Unreal audio interface object. [GCN]
	Copyright 1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Brandon Reinhart.
	* Rewritten by Andrew Scheidecker.
=============================================================================*/

/*------------------------------------------------------------------------------------
	Audio includes.
------------------------------------------------------------------------------------*/

#include "AudioPrivate.h"


/*------------------------------------------------------------------------------------
	UGCNAudioSubsystem.
------------------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UGCNAudioSubsystem);

TArray<AXVPB*> DroppedSounds;
void SoundDroppedCallback(void* axvpb)
{
	DroppedSounds.AddItem((AXVPB*)axvpb);
}

INT VolumeFloatToDB(FLOAT Volume)
{
	return Volume * 964 - 904;
}

UGCNAudioSubsystem::UGCNAudioSubsystem()
{
	guard(UGCNAudioSubsystem::UGCNAudioSubsystem);

	unguard;
}

void UGCNAudioSubsystem::StaticConstructor()
{
	guard(UGCNAudioSubsystem::StaticConstructor);

	SoundVolume = 0.5;
	MusicVolume = 0.5;

	ReverseStereo = 0;
	Monaural = 0;

	new(GetClass(),TEXT("ReverseStereo"),RF_Public) UBoolProperty(CPP_PROPERTY(ReverseStereo),TEXT("Options"),0);
	new(GetClass(),TEXT("Monaural"),RF_Public) UBoolProperty(CPP_PROPERTY(Monaural),TEXT("Options"),0);
	new(GetClass(),TEXT("SoundVolume"),RF_Public) UFloatProperty(CPP_PROPERTY(SoundVolume),TEXT("Options"),0);
	new(GetClass(),TEXT("MusicVolume"),RF_Public) UFloatProperty(CPP_PROPERTY(MusicVolume),TEXT("Options"),0);

	unguard;
}

/*------------------------------------------------------------------------------------
	UObject Interface.
------------------------------------------------------------------------------------*/

void UGCNAudioSubsystem::PostEditChange()
{
	guard(UGCNAudioSubsystem::PostEditChange);

	// Validate configurable variables.

	SoundVolume = Clamp(SoundVolume,0.0f,1.0f);
	MusicVolume = Clamp(MusicVolume,0.0f,1.0f);

	SetVolumes();

	unguard;
}

void UGCNAudioSubsystem::Destroy()
{
	guard(UGCNAudioSubsystem::Destroy);

	if(Initialized)
	{
		// Unhook.
		USound::Audio = NULL;
//		UMusic::Audio = NULL;

		SetViewport(NULL);
		ShutupMusic();

		debugf(NAME_Exit,TEXT("GCN audio subsystem shut down."));
	}

	Super::Destroy();

	unguard;
}

void UGCNAudioSubsystem::ShutupMusic()
{
	debugf(TEXT("Stopping music stream..."));
	GUglyHackFlags&=~16;
}

void UGCNAudioSubsystem::ShutdownAfterError()
{
	guard(UGCNAudioSubsystem::ShutdownAfterError);

	if(Initialized)
	{
		// Unhook.
		USound::Audio = NULL;
//		UMusic::Audio = NULL;
	
		debugf(NAME_Exit,TEXT("UGCNAudioSubsystem::ShutdownAfterError"));
	}

	Super::ShutdownAfterError();

	unguard;
}

/*------------------------------------------------------------------------------------
	UAudioSubsystem Interface.
------------------------------------------------------------------------------------*/

UBOOL UGCNAudioSubsystem::Init()
{
	guard(UGCNAudioSubsystem::Init);
#ifndef EMU

	LastTime = appSeconds();

	// Initialize AX
	ARReset();
	ARAMUsed = (INT)ARInit(NULL, 0);

	ARQInit();
	AIInit(NULL);
	
	AXInit();
	MIXInit();


	// Initialize the SPU2 memory cache.
	CleanUp();

	// Initialized!
	USound::Audio = this;
//	UMusic::Audio = this;
	Initialized = 1;

#endif

	debugf(NAME_Init,TEXT("GCN audio subsystem initialized."));

	return 1;

	unguard;
}

void UGCNAudioSubsystem::SetViewport(UViewport* InViewport)
{
	guard(UGCNAudioSubsystem::SetViewport);

	// Stop playing sounds.
	this->StopSound(NULL, NULL);

	if(Viewport != InViewport)
	{
		// Switch viewports.

		Viewport = InViewport;

		if(Viewport)
		{
			// Determine startup parameters.

			if(Viewport->Actor->Song && Viewport->Actor->Transition == MTRAN_None)
				Viewport->Actor->Transition = MTRAN_Instant;

			SetVolumes();
		}

		// Deregsiter everything
		CleanUp();

		if(Viewport)
		{
			// Register everything.
			for(TObjectIterator<USound> SoundIt;SoundIt;++SoundIt)
				RegisterSound(*SoundIt);
		}
	}

	unguard;
}

UViewport* UGCNAudioSubsystem::GetViewport()
{
	guard(UGCNAudioSubsystem::GetViewport);
	return Viewport;
	unguard;
}

void UGCNAudioSubsystem::RegisterSound(USound* Sound)
{
	guard(UGCNAudioSubsystem::RegisterSound);

//	if(Sound->FileType != FName(TEXT("GCN")))
//	{
//		debugf( TEXT("Rejecting non-GCN sound %s."), Sound->GetName() );
//		return;
//	}

	if( !Sound->Handle )
	{
		Sound->Data.Load();
		INT SoundSize = Sound->Data.Num() - 4;
		
		INT Looping;
		appMemcpy( &Looping, &Sound->Data(0), sizeof(INT) );

		FGCNSound* SoundInfo = new(TEXT("FGCNSound"))FGCNSound;
		GCNSounds.AddItem(SoundInfo);

		SoundInfo->SourceSound = Sound;
		SoundInfo->VoicePointer = NULL;

		SoundInfo->VoiceAddr.loopFlag			= Looping;
		SoundInfo->VoiceAddr.format				= AX_PB_FORMAT_PCM8;
		SoundInfo->VoiceAddr.loopAddressHi		= 0;
		SoundInfo->VoiceAddr.loopAddressLo		= ARAMUsed + SoundSize;
		SoundInfo->VoiceAddr.endAddressHi		= 0;
		SoundInfo->VoiceAddr.endAddressLo		= SoundInfo->VoiceAddr.loopAddressLo;
		SoundInfo->VoiceAddr.currentAddressHi	= 0;
		SoundInfo->VoiceAddr.currentAddressLo	= ARAMUsed;

        SoundInfo->VoiceSrc.ratioHi             = 1;
        SoundInfo->VoiceSrc.ratioLo             = 0;
        SoundInfo->VoiceSrc.currentAddressFrac  = 0;
        SoundInfo->VoiceSrc.last_samples[0]     = 0;
        SoundInfo->VoiceSrc.last_samples[1]     = 0;
        SoundInfo->VoiceSrc.last_samples[2]     = 0;
        SoundInfo->VoiceSrc.last_samples[3]     = 0;

		Sound->Handle = (INT)SoundInfo;
		Sound->Data.Unload();
	}
	unguard;
}

void UGCNAudioSubsystem::UnregisterSound(USound* Sound)
{
	guard(UGCNAudioSubsystem::UnregisterSound);

	if( Sound->Handle )
	{
		delete (FGCNSound*)Sound->Handle;
		Sound->Handle = 0;
	}

	unguard;
}

void UGCNAudioSubsystem::CleanUp()
{
	guard(UGCNAudioSubsystem::CleanUp());
	debugf(TEXT("Cleaning up audio memory..."));

	for( INT i=0; i<GCNSounds.Num(); i++ )
		if( GCNSounds(i) )
			UnregisterSound( GCNSounds(i)->SourceSound );

	unguard;
}

//void UGCNAudioSubsystem::UnregisterMusic(UMusic* Music)
//{
//	guard(UGCNAudioSubsystem::UnregisterMusic);
//	unguard;
//}

UBOOL UGCNAudioSubsystem::PlaySound(AActor* Actor,INT Id,USound* Sound,FVector Location,FLOAT Volume,FLOAT Radius,FLOAT Pitch, INT Flags)
{
	guard(UGCNAudioSubsystem::PlaySound);

#ifndef EMU

	check(Radius);
INT ViewportNum = 0; //SLUW (Added this to take out the parameter)

	if(!Viewport || !Sound)
		return 0;
	
	if( !Sound->Handle )
		RegisterSound(Sound);

	if( !Sound->Handle )
		return 0;

	FGCNSound*		SoundInfo = (FGCNSound*)Sound->Handle;
	if( !SoundInfo )
		return 0;

	FLOAT			Priority = Clamp(SoundPriority(Viewport->GetOuterUClient()->Viewports(ViewportNum),Location,Volume,Radius), 0.0f, 1.0f);

	// Find a voice to play the sound in.

	if (SoundInfo->VoicePointer == NULL)
	{
		SoundInfo->VoicePointer = AXAcquireVoice((INT)(Priority * 30) + 1, SoundDroppedCallback, 0);
		if (SoundInfo->VoicePointer == NULL)
			return 0;
	}

	if ( Actor && Viewport && ((Actor == Viewport->Actor->GetViewTarget()) || Actor->IsOwnedBy(Viewport->Actor->ViewTarget)) )
		Flags |= SF_No3D;
		
	// Setup the voice.

	Pitch = Clamp<FLOAT>(Pitch,0.1f,2.0f);

	SoundInfo->Actor = Actor;
	SoundInfo->Location = Location;
	SoundInfo->Radius = Radius;
	SoundInfo->Volume = Volume;
	SoundInfo->Pitch = Pitch;
	SoundInfo->Panning = AUDIO_MIDPAN;
	SoundInfo->ViewportNum = Min<INT>(ViewportNum, 3);
	SoundInfo->Flags = Flags;
	SoundInfo->Id = Id;
	SoundInfo->Paused = 0;

	// Start the voice.
	MIXInitChannel(SoundInfo->VoicePointer, 0, 0, -904, -904, 0, 64, 0);
	AXSetVoiceAddr(SoundInfo->VoicePointer, &SoundInfo->VoiceAddr);                    // input addressing
	AXSetVoiceSrcType(SoundInfo->VoicePointer, AX_SRC_TYPE_LINEAR);    // SRC type
	AXSetVoiceSrc(SoundInfo->VoicePointer, &SoundInfo->VoiceSrc);                      // initial SRC settings
	AXSetVoiceState(SoundInfo->VoicePointer, AX_PB_STATE_RUN);         // set it to run

#endif
	return 1;

	unguard;
}

UBOOL UGCNAudioSubsystem::StopSound( AActor* Actor, USound* Sound )
{
	if ( !Actor && !Sound )
	{
		for (INT i=0; i<GCNSounds.Num(); i++)	
			StopSound(i);
		return true;
	}

	UBOOL Stopped = false;

	// Stop selected sound.
	for (INT i=0; i<GCNSounds.Num(); i++)	
	{
		if ( Sound )
		{
			if ( Actor )
			{
				if ( (Actor == GCNSounds(i)->Actor) && (Sound == GCNSounds(i)->SourceSound) )
				{
					StopSound(i);
					Stopped = true;
				}
			}
			else
			{
				if ( Sound == GCNSounds(i)->SourceSound )
				{
					StopSound(i);
					Stopped = true;
				}
			}
		}
		else if ( Actor )
		{
			if ( Actor == GCNSounds(i)->Actor )
			{
				StopSound(i);
				Stopped = true;
			}
		}
	}

	return false;
}

void UGCNAudioSubsystem::NoteDestroy(AActor* Actor)
{
	guard(UGCNAudioSubsystem::NoteDestroy);
	check(Actor);
	check(Actor->IsValid());

	for( INT i=0; i<GCNSounds.Num(); i++ )
	{
		// Stop the actor's ambient sound, and dereference owned sounds.
		if (GCNSounds(i)->Actor == Actor)
		{
			if(GCNSounds(i)->Id & 14 == SLOT_Ambient * 2)
				StopSound(i);
			else
				GCNSounds(i)->Actor = NULL;
		}
	}

	unguard;
}

void UGCNAudioSubsystem::RenderAudioGeometry(FSceneNode* Frame)
{
	guard(UGCNAudioSubsystem::RenderAudioGeometry);
	unguard;
}

void UGCNAudioSubsystem::Update(FPointRegion Region,FCoords& Coords)
{
	guard(UGCNAudioSubsystem::Update);

#ifndef EMU

	AActor*	ViewActors[4];
	FCoords ViewCoords[4];
	UViewport* Viewports[4];
	FLOAT	DeltaTime,
			AmbientFactor,
			Brightness,
			BestDist=99999999.0f,
			NewDist;
	DWORD	KeyOn[2];
	INT		i, BestViewport;

	if(!Viewport)
		return;

	KeyOn[0] = 0;
	KeyOn[1] = 0;

	// Time passes...
	DeltaTime = appSeconds() - LastTime.GetFloat(); //SLUW (added GetFloat())
	LastTime += DeltaTime;
	DeltaTime = Clamp(DeltaTime,0.0f,1.0f);
	for( i=0;i<4;i++)
	{
		Viewports[i] = i<Viewport->GetOuterUClient()->Viewports.Num() ? Viewport->GetOuterUClient()->Viewports(i) : Viewport;
		ViewActors[i] = Viewports[i]->Actor->GetViewTarget();
		ViewCoords[i] = GMath.ViewCoords / ViewActors[i]->Rotation / ViewActors[i]->Location;
	}
	AmbientFactor = 0.7f;

	// Drop all dropped sounds    SL TODO: Make faster with a map?
	for (i=0; i<DroppedSounds.Num(); i++)
	{
		for (INT v=0; v<GCNSounds.Num(); v++)
		{
			if (GCNSounds(v)->VoicePointer == DroppedSounds(i))
			{
				GCNSounds(v)->VoicePointer = NULL;
				break;
			}
		}
	}
/*
	// Update the music.
	if(CurrentTrack != Viewport->Actor->CdTrack)
	{
		if(CurrentTrack != 255)
			ShutupMusic();

		CurrentTrack = Viewport->Actor->CdTrack;

		if(CurrentTrack != 255)
		{
			debugf(TEXT("Starting music stream..."));
			GUglyHackFlags|=16;

			strcpy(((FStartStreamParms*) MusicRPCBuffer)->Filename,appToAnsi(*FString::Printf(TEXT("\\MUSIC\\TRACK%02i.VGM;1"),CurrentTrack)));
			sceSifCallRpc(&MusicRPC,0,0,MusicRPCBuffer,RPC_BUFFER_SIZE,MusicRPCBuffer,RPC_BUFFER_SIZE,0,0);

			if(Monaural)
			{
				sceSdRemote(1,rSdSetParam,CoreVoiceMask(46) | SD_VP_VOLL,(INT) (MusicVolume * 8192.0f));
				sceSdRemote(1,rSdSetParam,CoreVoiceMask(46) | SD_VP_VOLR,(INT) (MusicVolume * 8192.0f));

				sceSdRemote(1,rSdSetParam,CoreVoiceMask(47) | SD_VP_VOLL,(INT) (MusicVolume * 8192.0f));
				sceSdRemote(1,rSdSetParam,CoreVoiceMask(47) | SD_VP_VOLR,(INT) (MusicVolume * 8192.0f));
			}
			else
			{
				sceSdRemote(1,rSdSetParam,CoreVoiceMask(46) | SD_VP_VOLL,(INT) (MusicVolume * 8192.0f));
				sceSdRemote(1,rSdSetParam,CoreVoiceMask(46) | SD_VP_VOLR,0);

				sceSdRemote(1,rSdSetParam,CoreVoiceMask(47) | SD_VP_VOLL,0);
				sceSdRemote(1,rSdSetParam,CoreVoiceMask(47) | SD_VP_VOLR,(INT) (MusicVolume * 8192.0f));
			}
		}
	}
*/
	// See if any new ambient sounds need to be started.
	guard(StartAmbience);
	for(i = 0;i < Viewport->Actor->GetLevel()->Actors.Num();i++)
	{
		AActor*	Actor = Viewport->Actor->GetLevel()->Actors(i);

		if(Actor && Actor->AmbientSound )
		{
			BestViewport = -1;
			for( INT j=0;j<4;j++ )
			{
				NewDist = FDistSquared(ViewActors[j]->Location,Actor->Location);
				if( BestViewport==-1 || NewDist<BestDist )
				{
					BestViewport = j;
					BestDist = NewDist;
				}
			}

			if( BestDist <= Square(Actor->SoundRadius*GAudioMaxRadiusMultiplier))
			{
				INT	Id = Actor->GetIndex()*16 + SLOT_Ambient*2,
					j;

				for(j = 0;j < GCNSounds.Num();j++)
				{
					if(GCNSounds(j)->Id == Id)
						break;
				}

				if(j == GCNSounds.Num())
					//SL TODO: What Flags?
					PlaySound(Actor,Id,Actor->AmbientSound,Actor->Location,AmbientFactor * Actor->SoundVolume / 255.0f,Actor->SoundRadius*GAudioMaxRadiusMultiplier,Actor->SoundPitch / 64.0f, 0);
			}
		}
	}

	unguard;

	// Update all playing ambient sounds.
	guard(UpdateAmbience);

	for(i = 0; i < GCNSounds.Num(); i++)
	{
		if(GCNSounds(i)->VoicePointer && (GCNSounds(i)->Id & 14) == SLOT_Ambient*2)
		{
			if(GCNSounds(i)->Actor )
			{
				// Find nearest viewport to ambient sound
				BestViewport = -1;
				for( INT j=0;j<4;j++ )
				{
					NewDist = FDistSquared(ViewActors[j]->Location,GCNSounds(i)->Actor->Location);
					if( BestViewport==-1 || NewDist<BestDist )
					{
						BestViewport = j;
						GCNSounds(i)->ViewportNum = BestViewport;
						BestDist = NewDist;
					}
				}
				if( BestDist>Square(GCNSounds(i)->Actor->SoundRadius*GAudioMaxRadiusMultiplier) || GCNSounds(i)->Actor->AmbientSound!=GCNSounds(i)->SourceSound )
				{
					// Ambient sound went out of range.
					StopSound(i);
				}
			}
			else if(!GCNSounds(i)->Actor)
				StopSound(i);
		}
	}

	unguard;

	// Update all active sounds.
	guard(UpdateSounds);

	MIXUpdateSettings();
	for(i = 0; i < GCNSounds.Num(); i++)
	{
		FGCNSound* SoundInfo = GCNSounds(i);
		if(SoundInfo->VoicePointer != NULL)
		{
			if(SoundInfo->VoicePointer->pb.state == AX_PB_STATE_STOP && !SoundInfo->Paused)
			{
				debugf(TEXT("Sound %s is done playing!"), SoundInfo->SourceSound->GetName());
				StopSound(i);
				continue;
			}

			// Update positioning from actor, if available.
			if(SoundInfo->Actor)
				SoundInfo->Location = SoundInfo->Actor->Location;

			// Update the priority.
			FLOAT Priority = Clamp(SoundPriority(Viewports[SoundInfo->ViewportNum],SoundInfo->Location,SoundInfo->Volume,SoundInfo->Radius), 0.0f, 1.0f);
			AXSetVoicePriority(SoundInfo->VoicePointer, (INT)(Priority * 30) + 1);

			// Compute the spatialization.
			FVector	Location(0, 0, 0);
			FLOAT	PanAngle(1.0);

			// Play at listener location if no 3D effects are wanted.
			if (!(SoundInfo->Flags & SF_No3D))
			{
				Location = SoundInfo->Location.TransformPointBy(SoundInfo->ViewportNum?ViewCoords[SoundInfo->ViewportNum]:Coords);
				PanAngle = appAtan2(Location.X,Abs(Location.Z));
			}
			
			// Despatialize sounds when you get real close to them.
			FLOAT	CenterDist = 0.1f*SoundInfo->Radius,
					Size = Location.Size();

			if(Location.SizeSquared() < Square(CenterDist))
				PanAngle *= Size / CenterDist;

			// Compute panning and volume.

			INT	SoundPan = Clamp((INT)(AUDIO_MAXPAN/2 + PanAngle*AUDIO_MAXPAN*7/8/PI),0,AUDIO_MAXPAN);

			FLOAT	Attenuation = 1.0f - Size / SoundInfo->Radius;

			if(Attenuation > 1.0f)
				Attenuation = 1.0f;
			else if(Attenuation < 0.0f)
				Attenuation = 0.0f;

			if(SoundInfo->Volume > 1.0f)
				SoundInfo->Volume = 1.0f;

			FLOAT	Volume = (INT)(SoundInfo->Volume * Attenuation * SoundVolume * 127);

			if(Volume > 1.0f)
				Volume = 1.0f;

			// Calculate left and right panning volume factor.

			INT VolumeDB = VolumeFloatToDB(Volume);
			MIXSetPan(SoundInfo->VoicePointer, SoundPan);
			MIXSetInput(SoundInfo->VoicePointer, VolumeDB);

			// Update the voice.
			if(Monaural)
			{
//				sceSdRemote(1,rSdSetParam,CoreVoiceMask(i) | SD_VP_VOLL,(INT) (Volume * 4096.0f));
//				sceSdRemote(1,rSdSetParam,CoreVoiceMask(i) | SD_VP_VOLR,(INT) (Volume * 4096.0f));
			}
			else
			{
//				sceSdRemote(1,rSdSetParam,CoreVoiceMask(i) | SD_VP_VOLL,(INT) ((BaseVolume - BaseVolume * PanningFactor) * 4096.0f));
//				sceSdRemote(1,rSdSetParam,CoreVoiceMask(i) | SD_VP_VOLR,(INT) ((BaseVolume + BaseVolume * PanningFactor) * 4096.0f));
			}

			// Compute doppler shifting (doesn't account for player's velocity).
			FLOAT	Doppler = 1.0f,
					DopplerSpeed = 9000.0f;

			if(SoundInfo->Actor)
				Doppler = Clamp(1.0f - ((SoundInfo->Actor->Velocity) | (SoundInfo->Actor->Location - ViewActors[SoundInfo->ViewportNum]->Location).SafeNormal())/DopplerSpeed, 0.5f, 2.0f);
//			sceSdRemote(1,rSdSetParam,CoreVoiceMask(i) | SD_VP_PITCH,(INT) ((((float) SoundInfo->Sound->SoundRate) / 48000.0f) * SoundInfo->Pitch * ViewActors[SoundInfo->ViewportNum]->Level->TimeDilation * Doppler * (44100.0f / 48000.0f) * 4096.0f));
		}
	}

	unguard;
#endif
	unguard;
}

void UGCNAudioSubsystem::PostRender( FSceneNode* Frame )
{
	guard(UGCNAudioSubsystem::PostRender);
/*
	INT	i;

	Frame->Viewport->Canvas->Color = FColor(255,255,255);

	if(AudioStats)
	{
		Frame->Viewport->Canvas->CurX = 0;
		Frame->Viewport->Canvas->CurY = 16;
		Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->MedFont,0,TEXT("GCNAudioSubsystem Statistics"));

		for(i = 0;i < 16;i++)
		{
			if(Voices[i].Id)
			{
				// Current Sound.

				Frame->Viewport->Canvas->CurX = 10;
				Frame->Viewport->Canvas->CurY = 32 + i * 32;
//				Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->MedFont,0,TEXT("%2i(%X %X)"),i,sceSdRemote(1,rSdGetParam,CoreVoiceMask(i) | SD_VP_ENVX),sceSdRemote(1,rSdGetParam,CoreVoiceMask(i) | SD_VP_PITCH));
			}
			else
			{
				Frame->Viewport->Canvas->CurX = 10;
				Frame->Viewport->Canvas->CurY = 32 + i * 32;
//				Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->MedFont,0,TEXT("%2i(%X %X)"),i,sceSdRemote(1,rSdGetParam,CoreVoiceMask(i) | SD_VP_ENVX),sceSdRemote(1,rSdGetParam,CoreVoiceMask(i) | SD_VP_PITCH));
			}
		}
	}
*/
	unguard;
}

UBOOL UGCNAudioSubsystem::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UGCNAudioSubsystem::Exec);

#ifndef EMU

	const TCHAR*	Str = Cmd;

	if(ParseCommand(&Str,TEXT("ASTAT")))
	{
		if(ParseCommand(&Str,TEXT("Audio")))
		{
			AudioStats ^= 1;
			return 1;
		}
	}
	else if(ParseCommand(&Str,TEXT("PauseSounds")))
	{
		for(INT Index = 0;Index < GCNSounds.Num();Index++)
		{
			if(GCNSounds(Index)->VoicePointer)
			{
				AXSetVoiceState(GCNSounds(Index)->VoicePointer, AX_PB_STATE_STOP);
				GCNSounds(Index)->Paused = 1;
			}
		}

		return 1;
	}
	else if(ParseCommand(&Str,TEXT("UnPauseSounds")))
	{
		for(INT Index = 0;Index < GCNSounds.Num();Index++)
		{
			if(GCNSounds(Index)->VoicePointer && GCNSounds(Index)->Paused)
			{
				AXSetVoiceState(GCNSounds(Index)->VoicePointer, AX_PB_STATE_RUN);
			}
		}

		return 1;
	}
#endif
	return 0;
	
	unguard;
}

/*------------------------------------------------------------------------------------
	Global Pause/Resume music functions, needed for sync with StaticLoadObject.
------------------------------------------------------------------------------------*/

void appPauseMusic()
{
	guard(UGCNAudioSubsystem::PauseMusic);
//	sceSifCallRpc(&MusicRPC,2,0,MusicRPCBuffer,RPC_BUFFER_SIZE,MusicRPCBuffer,RPC_BUFFER_SIZE,0,0);
	unguard;
}

void appResumeMusic()
{
	guard(UGCNAudioSubsystem::ResumeMusic);
//	sceSifCallRpc(&MusicRPC,3,0,MusicRPCBuffer,RPC_BUFFER_SIZE,MusicRPCBuffer,RPC_BUFFER_SIZE,0,0);
	unguard;
}

/*------------------------------------------------------------------------------------
	Internals.
------------------------------------------------------------------------------------*/

void UGCNAudioSubsystem::SetVolumes()
{
	guard(UGCNAudioSubsystem::SetVolumes);

	if(CurrentSong != "")
	{
		// Update the music volume.

//		sceSdRemote(1,rSdSetParam,CoreVoiceMask(46) | SD_VP_VOLL,(INT) (MusicVolume * 8192.0f));
//		sceSdRemote(1,rSdSetParam,CoreVoiceMask(46) | SD_VP_VOLR,0);

//		sceSdRemote(1,rSdSetParam,CoreVoiceMask(47) | SD_VP_VOLL,0);
//		sceSdRemote(1,rSdSetParam,CoreVoiceMask(47) | SD_VP_VOLR,(INT) (MusicVolume * 8192.0f));
	}

	unguard;
}

void UGCNAudioSubsystem::StopSound( INT i )
{
	guard(UGCNAudioSubsystem::StopSound);
#ifndef EMU

	FGCNSound* SoundInfo = GCNSounds(i);

	MIXReleaseChannel(SoundInfo->VoicePointer);
	AXSetVoiceState(SoundInfo->VoicePointer, AX_PB_STATE_STOP);

	if (SoundInfo->VoicePointer)
	{
		AXFreeVoice(SoundInfo->VoicePointer);
		SoundInfo->VoicePointer = NULL;
	}
	SoundInfo->Actor = NULL;
#endif

	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


/*=============================================================================
	ALAudioSubsystem.cpp: Unreal OpenAL Audio interface object.
	Copyright 1999-2003 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Daniel Vogel.
	* Ported to Linux by Ryan C. Gordon.

	OpenAL is RHS
	Unreal is RHS with Y and Z swapped

=============================================================================*/

/*------------------------------------------------------------------------------------
	Audio includes.
------------------------------------------------------------------------------------*/

#include "ALAudioPrivate.h"

#if SUPPORTS_PRAGMA_PACK
#pragma pack (push,8)
#endif

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

// OpenAL functions.
#define INITGUID 1
#undef DEFINE_GUID
#define OPENAL

#if DYNAMIC_BIND
#define AL_EXT(name) UBOOL UALAudioSubsystem::SUPPORTS##name;
#define ALC_EXT(name) UBOOL UALAudioSubsystem::SUPPORTS##name;
#define AL_PROC(ext,ret,func,parms) ret (CDECL *UALAudioSubsystem::func)parms;
#define ALC_PROC(ext,ret,func,parms) ret (CDECL *UALAudioSubsystem::func)parms;
#include "ALFuncs.h"
#undef AL_EXT
#undef ALC_EXT
#undef AL_PROC
#undef ALC_PROC
#endif

#if SUPPORTS_EAX
// The below two headers are not standard EAX SDK headers!
#include "eax3.h"
#include "eax2.h"
EAXGet UALAudioSubsystem::alEAXGet = NULL;
EAXSet UALAudioSubsystem::alEAXSet = NULL;
#endif

#if !__UNIX__ && !defined(_WIN64)  // !!! FIXME!
#define USE_DIRECTSOUND 1
#endif

#if SUPPORTS_PRAGMA_PACK
#pragma pack (pop)
#endif

/*------------------------------------------------------------------------------------
	UALAudioSubsystem.
------------------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UALAudioSubsystem);

//
// UALAudioSubsystem::UALAudioSubsystem
//
UALAudioSubsystem::UALAudioSubsystem()
{
	guard(UALAudioSubsystem::UALAudioSubsystem);
	unguard;
}

//
// UALAudioSubsystem::StaticConstructor
//
void UALAudioSubsystem::StaticConstructor()
{
	guard(UALAudioSubsystem::StaticConstructor);
	new(GetClass(),TEXT("Use3DSound"			), RF_Public) UBoolProperty	(CPP_PROPERTY(Use3DSound			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("UseEAX"				), RF_Public) UBoolProperty	(CPP_PROPERTY(UseEAX				), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("CompatibilityMode"		), RF_Public) UBoolProperty	(CPP_PROPERTY(UseMMSYSTEM			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("UsePrecache"			), RF_Public) UBoolProperty	(CPP_PROPERTY(UsePrecache			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("ReverseStereo"			), RF_Public) UBoolProperty	(CPP_PROPERTY(ReverseStereo			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("UseDefaultDriver"		), RF_Public) UBoolProperty	(CPP_PROPERTY(UseDefaultDriver		), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("Channels"				), RF_Public) UIntProperty	(CPP_PROPERTY(MaxChannels			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("MaxEAXVersion"			), RF_Public) UIntProperty	(CPP_PROPERTY(MaxEAXVersion			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("DopplerFactor"			), RF_Public) UFloatProperty(CPP_PROPERTY(DopplerFactor			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("RollOff"				), RF_Public) UFloatProperty(CPP_PROPERTY(RollOff				), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("LocalZOffset"			), RF_Public) UFloatProperty(CPP_PROPERTY(LocalZOffset			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("TimeBetweenHWUpdates"	), RF_Public) UFloatProperty(CPP_PROPERTY(TimeBetweenHWUpdates	), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("SpatializedVoiceRadius"), RF_Public) UFloatProperty(CPP_PROPERTY(SpatializedVoiceRadius), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("DisablePitch"			), RF_Public) UBoolProperty	(CPP_PROPERTY(DisablePitch			), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("LowQualitySound"		), RF_Public) UBoolProperty	(CPP_PROPERTY(UseLowQualitySound	), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("UseVAD"				), RF_Public) UBoolProperty	(CPP_PROPERTY(UseVAD				), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("EnhancedDenoiser"		), RF_Public) UBoolProperty	(CPP_PROPERTY(EnhancedDenoiser		), TEXT("ALAudio"), CPF_Config );
	new(GetClass(),TEXT("UseSpatializedVoice"	), RF_Public) UBoolProperty	(CPP_PROPERTY(UseSpatializedVoice	), TEXT("ALAudio"), CPF_Config );
	unguard;
}


/*------------------------------------------------------------------------------------
	UObject Interface.
------------------------------------------------------------------------------------*/

//
// UALAudioSubsystem::PostEditChange
//
void UALAudioSubsystem::PostEditChange()
{
	guard(UALAudioSubsystem::PostEditChange);

	Super::PostEditChange();

	// Adjust volumes.
	for (INT i=0; i<Sources.Num(); i++)
	{
		if ( Sources(i)->Flags & SF_Music )
		{
			Sources(i)->Volume = Clamp( MusicVolume, 0.f, 1.f );
        	alSourcef( Sources(i)->SourceId, AL_GAIN, Sources(i)->Volume );
		}
		else
		if ( Sources(i)->Flags & SF_Voice )			
		{
			Sources(i)->Volume = Clamp( VoiceVolume, 0.f, 1.f );
			alSourcef( Sources(i)->SourceId, AL_GAIN, Sources(i)->Volume );
		}
	}

	unguard;
}

//
//	UALAudioSubsystem::Flush
//
void UALAudioSubsystem::Flush( UBOOL Force )
{
	guard(UALAudioSubsystem::Flush);
	for( INT i=0; i<Sources.Num(); i++ )
		Sources(i)->Stop();
	for( INT i=0; i<Buffers.Num(); i++ )
	{
		if( Force || (Buffers(i)->Sound->VoiceCodec == CODEC_None) )
		{
			delete Buffers(i);
			Buffers.Remove(i--);
		}
	}
	unguard;
}

//
//	UALAudioSubsystem::Shutdown
//
void UALAudioSubsystem::Shutdown()
{
	guard(UALAudioSubsystem::Shutdown);
	if( USound::Audio )
	{
		// Unhook.
		USound::Audio = NULL;

		Flush( 1 );

		for( INT i=0; i<Sources.Num(); i++ )
			delete Sources(i);
		Sources.Empty();

		#if WITH_SPEEX
		delete VoiceModule;
		VoiceModule			= NULL;
		#endif

		delete Listener;
		delete TransientSource;

		Listener			= NULL;
		TransientSource		= NULL;

		alcProcessContext( SoundContext );
		alcMakeContextCurrent( NULL );
		alcDestroyContext( SoundContext );
		alcCloseDevice( SoundDevice );

		SetViewport(NULL);

		if( UTexture::__Client )
			UTexture::__Client->TeardownSR();

		delete CaptureDevice;
		CaptureDevice = NULL;

		if( StreamManager )
		{
			StreamManager->Destroy();
			delete StreamManager;
			StreamManager = NULL;
		}

		appFreeDllHandle( DLLHandle );

		debugf(NAME_Exit,TEXT("OpenAL Audio subsystem shut down."));
	}
	unguard;
}

//
//	UALAudioSubsystem::Destroy
//
void UALAudioSubsystem::Destroy()
{
	guard(UALAudioSubsystem::Destroy);
	Shutdown();
	Super::Destroy();
	unguard;
}

//
// UALAudioSubsystem::ShutdownAfterError
//
void UALAudioSubsystem::ShutdownAfterError()
{
	guard(UALAudioSubsystem::ShutdownAfterError);
	Shutdown();
	Super::ShutdownAfterError();
	unguard;
}

//
//	UALAudioSubsystem::Serialize
//
void UALAudioSubsystem::Serialize( FArchive& Ar )
{
	guard(UALAudioSubsystem::Serialize);
	Super::Serialize(Ar);

	if( !Ar.IsLoading() && !Ar.IsSaving() )
	{
		for( INT i=0; i<Sources.Num(); i++ )
			Ar << Sources(i)->Sound;
		for( INT i=0; i<Buffers.Num(); i++ )
			Ar << Buffers(i)->Sound;
	}

	unguard;
}


/*------------------------------------------------------------------------------------
	UAudioSubsystem Interface.
------------------------------------------------------------------------------------*/

//
// UALAudioSubsystem::Init
//
UBOOL UALAudioSubsystem::Init()
{
	guard(UALAudioSubsystem::Init);

	if( USound::Audio )
		return 1;

#if DYNAMIC_BIND
	guard(DynamicBinding);
	// Find DLL's.
	DLLHandle = NULL;
	if( !UseDefaultDriver )
	{
		DLLHandle = appGetDllHandle( AL_DLL );
		if( !DLLHandle )
			debugf( NAME_Init, TEXT("Couldn't locate %s - trying default next."), AL_DLL );
	}
	if( !DLLHandle )
	{
		DLLHandle = appGetDllHandle( AL_DEFAULT_DLL );
		if( !DLLHandle )
		{
			debugf( NAME_Init, TEXT("Couldn't locate %s - giving up."), AL_DEFAULT_DLL );
			return 0;
		}
	}
	
	// Find functions.
	SUPPORTS_AL = 1;
	FindProcs( 0 );
	if( !SUPPORTS_AL )
		return 0;
	unguard;
#endif

	if( UseEAX )
		Use3DSound = true;

	char *Device = NULL;
	#if USE_DIRECTSOUND
		Device = UseMMSYSTEM ? "MMSYSTEM" : Use3DSound ? "DirectSound3D" : "DirectSound";
	#endif
	guard(alcOpenDevice);
	SoundDevice = alcOpenDevice( (unsigned char *)Device );
	unguard;
	if( !SoundDevice )
	{
		debugf(NAME_Init,TEXT("ALAudio: no OpenAL devices found."));
		return 0;
	}

	ALint Caps[] = { ALC_FREQUENCY, 44100, 0 };
	guard(alcCreateContext);
	SoundContext = alcCreateContext( SoundDevice, Caps );
	unguard;
	if( !SoundContext )
	{
		debugf(NAME_Init, TEXT("ALAudio: context creation failed."));
		return 0;
	}

	guard(alGetError);
	alGetError();  // clear errors in case we didn't reload DLL (MacOSX) --ryan.
	unguard;

        guard(alcMakeContextCurrent);
        alcMakeContextCurrent( SoundContext );
        unguard;
	
	if ( alError(TEXT("Init")) )
	{
		debugf(NAME_Init,TEXT("ALAudio: makecurrent failed."));
		return 0;
	}

	guard(alGetStrings);
	debugf( NAME_Init, TEXT("AL_VENDOR      : %s"), appFromAnsi((ANSICHAR*)alGetString(AL_VENDOR	)));
	debugf( NAME_Init, TEXT("AL_RENDERER    : %s"), appFromAnsi((ANSICHAR*)alGetString(AL_RENDERER	)));
	debugf( NAME_Init, TEXT("AL_VERSION     : %s"), appFromAnsi((ANSICHAR*)alGetString(AL_VERSION	)));
	debugf( NAME_Init, TEXT("AL_EXTENSIONS  : %s"), appFromAnsi((ANSICHAR*)alGetString(AL_EXTENSIONS)));
	unguard;

	FindProcs( 1 );

	// No channels, no sound.
	if ( MaxChannels <= 4 )
		return 0;

	// Adjust global rolloff factor.
	if( RollOff <= 0.0f )
		RollOff = 1.0f;
	
	// Initialize channels.
	guard(InitializingChannels);

	alError(TEXT("Emptying error stack"),0);
	ALuint sid;
	alGenSources( 1, &sid );
	TransientSource = new FALSoundSource(this);
	TransientSource->SourceId = sid;
	if( alError(TEXT("Init (creating transient source)")) )
	{
		debugf(NAME_Error,TEXT("ALAudio: couldn't allocate transient source"));
		return 0;
	}

	for( INT i=0; i<Min(MaxChannels, MAX_AUDIOCHANNELS); i++ )
	{
		alGenSources( 1, &sid );
		if ( !alError(TEXT("Init (creating sources)"), 0 ) )
		{
			Sources.Add(1);
			Sources(i) = new FALSoundSource(this);
			Sources(i)->SourceId = sid;
			// Initialize rolloff so DS3D wrapper can set global rolloff. (HACK)
			alSourcef( sid, AL_ROLLOFF_FACTOR, RollOff );
		}
		else
			break;
	}
	if( !Sources.Num() )
	{
		debugf(NAME_Error,TEXT("ALAudio: couldn't allocate sources"));
		return 0;
	}

	unguard;

	// Use DS3D distance model :(
	alDistanceModel( AL_INVERSE_DISTANCE_CLAMPED );
	alDopplerFactor( DopplerFactor );

	#if SUPPORTS_EAX
		// Check for EAX support.
		guard(CheckingForEAX);
		alEAXGet	= NULL;
		alEAXSet	= NULL;
		EAXVersion	= 0;

		if( ((alIsExtensionPresent((ALubyte*)"EAX3.0") == AL_TRUE) || (alIsExtensionPresent((ALubyte*)"EAX3.0Emulated") == AL_TRUE)) && (MaxEAXVersion >= 3) )
			EAXVersion = 3;
		else if( (alIsExtensionPresent((ALubyte*)"EAX2.0") == AL_TRUE) && (MaxEAXVersion >= 2) )
			EAXVersion = 2;

		if( UseEAX && EAXVersion )
		{
			alEAXSet	= (EAXSet) alGetProcAddress((ALubyte*)"EAXSet");
			alEAXGet	= (EAXGet) alGetProcAddress((ALubyte*)"EAXGet");

			if( alEAXSet && alEAXGet )
			{
				// Set 'plain' EAX preset.
				if( EAXVersion >= 3 )
				{
					ALuint Environment = EAX_ENVIRONMENT_PLAIN;
					alEAXSet(&DSPROPSETID_EAX30_ListenerProperties,DSPROPERTY_EAXLISTENER_ENVIRONMENT,NULL, &Environment, sizeof(ALuint));
				}
				else
				{
					ALuint Environment = EAX20_ENVIRONMENT_PLAIN;
					alEAXSet(&DSPROPSETID_EAX20_ListenerProperties,DSPROPERTY_EAX20LISTENER_ENVIRONMENT,NULL, &Environment, sizeof(ALuint));
				}
				debugf(NAME_Init,TEXT("ALAudio: using EAX"));
			}
			else
			{
				alEAXSet	= NULL;
				alEAXGet	= NULL;
			}
		}
		unguard;
	#endif

	// Init shared audio capture device.
	if( !GIsEditor )
	{
		if( CaptureDevice == NULL )
		{
			CaptureDevice = new FALAudioCaptureDevice();
			if( CaptureDevice->Init() )
			{
				CaptureDevice->Record();
			}
			else
			{
				delete CaptureDevice;
				CaptureDevice = NULL;
			}
		}			
	}
	else
		CaptureDevice = NULL;

	#ifndef WIN32  // !!! FIXME
	alcProcessContext( SoundContext );
	#endif

#if WITH_SPEEX
	// Init voice encoding/ decoding module.
	VoiceModule = new FALVoiceModule( this, CaptureDevice );
#endif

	// Init streaming thread.
	StreamManager = new FALAudioStreamManager();
	StreamManager->Init();

	// Base init.
	UAudioSubsystem::Init();

	// Initialized.
	LastRealtime	= 1;
	LastHWUpdate	= 0;
	LastVoiceTime	= GCurrentTime;
	Listener		= new FListener();
	USound::Audio	= this;

	debugf(NAME_Init,TEXT("ALAudio: subsystem initialized."));
	return 1;

	unguard;
}

//
// UALAudioSubsystem::SetViewport
//
void UALAudioSubsystem::SetViewport(UViewport* InViewport)
{
	guard(UALAudioSubsystem::SetViewport);

	UAudioSubsystem::SetViewport( InViewport );

	// Stop playing sounds.
	for( INT i=0; i<Sources.Num(); i++ )
		if( !(Sources(i)->Flags & SF_Streaming) )
			Sources(i)->Stop();

	if( Viewport != InViewport )
	{
		// Switch viewports.
		Viewport = InViewport;

		// Reset listener.
		if( Listener )
			Listener->Reset();

		Flush( 0 );

		if( Viewport && UsePrecache )
		{
			// Register everything.
			for(TObjectIterator<USound> SoundIt;SoundIt;++SoundIt)
			{
				USound* Sound = *SoundIt;

			    FLOAT TempPitch = 1.f, TempVolume = 1.f;
				Sound = Sound->RenderSoundPlay( &TempPitch, &TempVolume);
                if( !Sound->IsValid() )
                    continue;

				// Don't precache temporary music/streaming objects.
				if( !(Sound->GetFlags() & SF_Streaming) )    
					FALSoundBuffer::Init( Sound, this );
		    }
		}
	}

	unguard;
}
			
//
// UALAudioSubsystem::PlaySoundSource
//
void UALAudioSubsystem::PlaySound( FSoundSource* InSource )
{
	guard(UALAudioSubsystem::PlaySound);

	static INT FreeSlot;

	// Early outs.
	if( !Viewport || !InSource->Sound || !InSource->Sound->IsValid() )
        return;

	// Find matching buffer.
	FALSoundBuffer* Buffer = FALSoundBuffer::Init( InSource->Sound, this );
	if( !Buffer )
		return;

	// Volume mojo.
	InSource->Volume = Clamp<FLOAT>(InSource->Volume,0.0f,1.0f);

	if( InSource->Flags & SF_MusicVolume )
		InSource->Volume *= 1.f;
	else
	if( InSource->Flags & SF_Voice )
		InSource->Volume *= VoiceVolume;
	else
		InSource->Volume *= SoundVolume;
	
	InSource->Pitch  = Clamp<FLOAT>(InSource->Pitch,0.5f,2.0f);
	InSource->Volume = Clamp<FLOAT>(InSource->Volume,0.0f,1.0f);

	if( InSource->Volume == 0.f )
		return;

	InSource->UsedVolume = InSource->FadeMode == FADE_In ? 0.f : InSource->Volume;

	// Allocate a new slot if requested.
	if( (InSource->Id&14)==2*SLOT_None )
		InSource->Id = 16 * --FreeSlot;

	// Find source to play sound in.
	FALSoundSource* Source = TransientSource;

	// Flat copy of source.
	*((FSoundSource*)Source) = *InSource;
	
	// Set remaining parameters.
	Source->Location	= (Source->Flags & SF_No3D) ? FVector(0,LocalZOffset,0) : Source->Location;
	Source->Buffer		= Buffer;

	// Find source to play sound in.
	if( !FindSource() )
		return;
	
	// Convert parameters to OpenAL conventions.
	ALuint	sid			= Source->SourceId;
	
#if SUPPORTS_EAX
	// Set EAX occlusion.
	if( alEAXSet != NULL )
	{
		long Occlusion = Source->Flags & SF_No3D ? 0 : (long) ( 868.589f * log( Max( Source->UsedRadius / Source->Radius, 0.00001f) ) ) ;
		if( EAXVersion >= 3 )
            alEAXSet(&DSPROPSETID_EAX30_BufferProperties, DSPROPERTY_EAXBUFFER_OCCLUSION, sid, &Occlusion, sizeof(Occlusion));
		else
			alEAXSet(&DSPROPSETID_EAX20_BufferProperties, DSPROPERTY_EAX20BUFFER_OCCLUSION, sid, &Occlusion, sizeof(Occlusion));
	}
#endif

	FVector ALLocation;
	ALLocation.X = Source->Location.X;
	ALLocation.Y = Source->Location.Z;
	ALLocation.Z = Source->Location.Y;
	ALLocation *= AUDIO_DISTANCE_FACTOR;

	// Set OpenAL source attributes.
	alSourcef ( sid, AL_GAIN				, Clamp( Source->UsedVolume * (DampenVolume ? VolumeScaleRec : 1.f), 0.0f, 1.0f));
	alSourcefv( sid, AL_POSITION			, (ALfloat*) &ALLocation ); 
	alSourcef ( sid, AL_REFERENCE_DISTANCE	, Source->UsedRadius * AUDIO_DISTANCE_FACTOR );
	alSourcei ( sid, AL_LOOPING				, (Source->Flags & SF_Looping) ? AL_TRUE : AL_FALSE );
	alSourcef ( sid, AL_ROLLOFF_FACTOR		, RollOff ); //!!: wrapper doesn't correctly support per source rolloff

	if( Source->Flags & SF_No3D )
		alSourcei( sid, AL_SOURCE_RELATIVE, AL_TRUE );
	else
		alSourcei( sid, AL_SOURCE_RELATIVE, AL_FALSE );

	alSourcef ( sid, AL_PITCH , DisablePitch ? 1.0f : Source->Pitch );

	// Start playback with buffer 0.
	Buffer->BufferQueueIndex	= 0;
	Buffer->BufferUnqueueIndex	= 0; 

	// Queue all buffers.
	check( Buffer->BufferIds.Num() );	
	if( !(Source->Flags & SF_Voice) )
	{
		if( Buffer->BufferIds.Num() == 1 )
			alSourcei( sid, AL_BUFFER, Buffer->BufferIds(0) );
		else
			alSourceQueueBuffers( sid, Buffer->BufferIds.Num(), &Buffer->BufferIds(0) );
		Source->Play();
	}

	alError( TEXT("PlaySound END") );

	unguard;
}

//
//	UALAudioSubsystem::SeekStream
//
INT UALAudioSubsystem::SeekStream( INT Handle, FLOAT Time )
{
	guard(UALAudioSubsystem::SeekStream);
	for( INT i=0; i<Sources.Num(); i++ )
	{
		FALSoundSource* Source = Sources(i);
		if( Source && Source->Sound && (Source->Sound->GetIndex()==Handle) && Source->Buffer && Source->Buffer->AudioStream )
			return Source->Buffer->AudioStream->Seek( Time );
	}
	return 0;
	unguard;
}

//
//	UALAudioSubsystem::Update
//
UBOOL UALAudioSubsystem::Update( FSceneNode* SceneNode )
{
	guard(UALAudioSubsystem::Update);

	clock(GStats.DWORDStats(AudioStats.STATS_UpdateCycles));

	// Update.
	if( !UAudioSubsystem::Update( SceneNode ) )
	{
		unclock(GStats.DWORDStats(AudioStats.STATS_UpdateCycles));
		return 0;
	}

	// Set Player position and orientation.
	FVector ALOrientation[2];
	ALOrientation[0].X	= Listener->Front.X;
	ALOrientation[0].Y	= Listener->Front.Z;
	ALOrientation[0].Z	= Listener->Front.Y;	
	ALOrientation[1].X	= Listener->Up.X;
	ALOrientation[1].Y	= Listener->Up.Z;
	ALOrientation[1].Z	= Listener->Up.Y;	

    // Make the listener still and the sounds move relatively -- this allows 
    // us to scale the doppler effect on a per-sound basis.
	FVector	ALLocation;
	ALLocation.X = Listener->Location.X;
	ALLocation.Y = Listener->Location.Z;
	ALLocation.Z = Listener->Location.Y;
	ALLocation *= AUDIO_DISTANCE_FACTOR;

	alListenerfv( AL_POSITION	, (ALfloat *) &ALLocation		);
	alListenerfv( AL_ORIENTATION, (ALfloat *) &ALOrientation[0] );

	// Set listener EAX effect.
	SetEAXEffect();

#if WITH_SPEEX
	if( CaptureDevice )
	{
		// Dampen sound volume if recording.
		DampenVolume = Viewport->Actor->bVoiceTalk;

		// Check capture device and encode + send packets if there is enough data.
		VoiceModule->Encode();
	}
#endif

	// Deferred commit (enforce min time between updates).
	if( Viewport->CurrentTime < LastHWUpdate )
		LastHWUpdate = Viewport->CurrentTime;
	if( (Viewport->CurrentTime - LastHWUpdate) >= (TimeBetweenHWUpdates / 1000.f) )
	{
		LastHWUpdate = Viewport->CurrentTime;
		alcProcessContext( SoundContext );
#ifdef WIN32
		alcSuspendContext( SoundContext );
#endif
	}

	unclock(GStats.DWORDStats(AudioStats.STATS_UpdateCycles));
	return 1;

	unguard;
}

//
//	UALAudioSubsystem::PlayVoicePacket.
//
UBOOL UALAudioSubsystem::PlayVoicePacket( FVoiceInfo* VoiceInfo )
{
	guard(UALAudioSubsystem::PlayVoicePacket);
#if WITH_SPEEX
	LastVoiceTime = GCurrentTime;
	check(VoiceModule);
	check(VoiceInfo);
	return VoiceModule->Decode( VoiceInfo );
#else
	return 0;
#endif
	unguard;
}

//
//	UALAudioSubsystem::Exec
//
UBOOL UALAudioSubsystem::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UALAudioSubsystem::Exec);

	if(ParseCommand(&Cmd,TEXT("PAUSESOUNDS")))
	{
		for(INT i=0; i<Sources.Num(); i++)
			Sources(i)->Pause();
		return 1;
	}
	if(ParseCommand(&Cmd,TEXT("AUDIO")))
	{
		if(ParseCommand(&Cmd,TEXT("FLUSH")))
		{
			if (appStrcmp(Cmd,TEXT("")) != 0) 
			{		
				INT Force=appAtoi(Cmd);
				Flush(Force);
				return 1;
			}
			else
				return 0;
		}
		return 0;
	}
#if SAVE_SOUNDS
	else if(ParseCommand(&Cmd,TEXT("SAVESOUNDS")))
	{
		appSaveArrayToFile( RawSoundBeforeDenoiser, TEXT("BeforeDenoiser.raw") );
		appSaveArrayToFile( RawSoundAfterDenoiser, TEXT("AfterDenoiser.raw") );
		return 1;
	}
#endif
	else if(ParseCommand(&Cmd,TEXT("UNPAUSESOUNDS")))
	{
		for(INT i=0; i<Sources.Num(); i++)
			Sources(i)->Play();
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("STOPSOUNDS")) )
	{
		for( INT i=0; i<Sources.Num(); i++ )
		{
			if( !(Sources(i)->Flags & SF_Streaming) )
				Sources(i)->Stop();
		}
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("WEAPONRADIUS")) )
	{
		if (appStrcmp(Cmd,TEXT("")) != 0) 
			GAudioDefaultRadius=appAtof(Cmd);
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("ROLLOFF")) )
	{
		if (appStrcmp(Cmd,TEXT("")) != 0) 
		{		
			RollOff=appAtof(Cmd);
			for( INT i=0; i<Sources.Num(); i++ )
				Sources(i)->Stop();
		}
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("CHECKSOUNDPLAYING")) )
	{
		for(INT i=0; i<Sources.Num(); i++)
		{
			if( Sources(i)->Sound && appStrcmp( Sources(i)->Sound->GetName(), Cmd ) == 0 )
			{
				Ar.Logf(TEXT("1"));
				return 1;
			}
		}	
		Ar.Logf(TEXT("0"));
		return 1;
	}
	else if( ParseCommand(&Cmd,TEXT("SETTEMPMUSICVOLUME")) )
	{
		if (appStrcmp(Cmd,TEXT("")) != 0) 
		{		
			FLOAT Volume=appAtof(Cmd);
			for( INT i=0; i<Sources.Num(); i++ )
				if( Sources(i)->Flags & SF_Music )
					alSourcef( Sources(i)->SourceId, AL_GAIN, Clamp(Volume, 0.0f, 1.0f) );
		}
		return 1;
	}
	else if ( ParseCommand(&Cmd,TEXT("GETDURATION")) )
	{
		if ( *Cmd )
		{
			INT Handle = appAtoi(Cmd);
			if ( Handle != UCONST_INVALIDSONGHANDLE )
			{
				for( INT i=0; i<Sources.Num(); i++ )
				{
					FALSoundSource* Source = Sources(i);
					if( Source && Source->Sound && (Source->Sound->GetIndex()==Handle) && Source->Buffer && Source->Buffer->AudioStream )
					{
						Ar.Logf(TEXT("%i"), (INT)Source->Buffer->AudioStream->GetDuration() );
						return 1;
					}
				}
			}
		}
	}

	return 0;	
	unguard;
}

//
//	UALAudioSubsystem::NoteDestroy
//
void UALAudioSubsystem::NoteDestroy( USound* Sound )
{
	guard(UALAudioSubsystem::NoteDetroy);

	UAudioSubsystem::NoteDestroy( Sound );

	for( INT i=0; i<Sources.Num(); i++ )
		if( Sources(i)->Sound == Sound )
			Sources(i)->Stop();

	for( INT i=0; i<Buffers.Num(); i++ )
	{
		if( Buffers(i)->Sound == Sound )
		{
			delete Buffers(i);
			Buffers.Remove(i--);
		}
	}
	unguard;
}

//
//	UALAudioSubsystem::NoteDestroy
//
void UALAudioSubsystem::NoteDestroy( AActor* Actor )
{
	guard(UALAudioSubsystem::NoteDetroy);
	check(Actor);
	check(Actor->IsValid());

	UAudioSubsystem::NoteDestroy( Actor );

#if WITH_SPEEX
	// Give voice module a chance to detach.
	VoiceModule->NoteDestroy( Actor );
#endif

	// Stop the actor's sound, and dereference owned sounds.
	for( INT i=0; i<Sources.Num(); i++ )
	{
		if(Sources(i)->Actor == Actor)
		{
			if((Sources(i)->Id & 14) == SLOT_Ambient * 2)
				Sources(i)->Stop();
			else 
				Sources(i)->Actor = NULL;
		}
	}

	unguard;
}

//
//	UALAudioSubsystem::SetTransientSource
//
void UALAudioSubsystem::SetTransientSource( FSoundSource* NewTransientSource )
{
	guard(UALAudioSubsystem::SetTransientSource);
	
	for( INT i=0; i<Sources.Num(); i++ )
	{
		if( Sources(i) == NewTransientSource )
		{
			Sources(i) = TransientSource;
			break;
		}
	}

	TransientSource = (FALSoundSource*) NewTransientSource;
	TransientSource->Stop();

	unguard;
}

/*------------------------------------------------------------------------------------
	Internals.
------------------------------------------------------------------------------------*/

//
//	UALAudioSubsystem::FindExt
//
UBOOL UALAudioSubsystem::FindExt( const TCHAR* Name, UBOOL IsALC )
{
	guard(UALAudioSubsystem::FindExt);

	ALboolean Result = AL_FALSE;
	if( IsALC )  // !!! FIXME: May not be NULL device!
	{
		if( alcIsExtensionPresent )
			Result = alcIsExtensionPresent(NULL, (ALubyte *) appToAnsi(Name));
	}
	else
	{
		if( alIsExtensionPresent )
			Result = alIsExtensionPresent((ALubyte *) appToAnsi(Name));
	}

	if( Result )
		debugf( NAME_Init, TEXT("Device supports: %s"), Name );
	return Result;
	unguard;
}

//
//	UALAudioSubsystem::FindProc
//
void UALAudioSubsystem::FindProc( void*& ProcAddress, char* Name, char* SupportName, UBOOL& Supports, UBOOL AllowExt, UBOOL IsALC )
{
	guard(UALAudioSubsystem::FindProc);
	ProcAddress = NULL;

// !!! FIXME: Shouldn't use this for more than al*GetProcAddress, even
// !!! FIXME:  when dynamically loading. All entry points should be
// !!! FIXME:  retrieve via alGetProcAddress (or alcGetProcAddress), since
// !!! FIXME:  they reserve the right to give different values in certain
// !!! FIXME:  cases.  --ryan.
#if DYNAMIC_BIND
	if( !ProcAddress )
		ProcAddress = appGetDllExport( DLLHandle, appFromAnsi(Name) );
#endif
	if( !ProcAddress && Supports && AllowExt )
	{
		if( IsALC )  // !!! FIXME: Might need device in some cases...
			ProcAddress = alcGetProcAddress( NULL, (ALubyte*) Name );
		else
			ProcAddress = alGetProcAddress( (ALubyte*) Name );
	}
	if( !ProcAddress )
	{
		if( Supports )
			debugf( TEXT("   Missing function '%s' for '%s' support"), appFromAnsi(Name), appFromAnsi(SupportName) );
		Supports = 0;
	}
	unguard;
}

//
//	UALAudioSubsystem::FindProcs
//
void UALAudioSubsystem::FindProcs( UBOOL AllowExt )
{
	guard(UALAudioSubsystem::FindProcs);
	#define AL_EXT(name) if( AllowExt ) SUPPORTS##name = FindExt( TEXT(#name)+1, 0 );
	#define ALC_EXT(name) if( AllowExt ) SUPPORTS##name = FindExt( TEXT(#name)+1, 1 );
	#define AL_PROC(ext,ret,func,parms) FindProc( *(void**)&func, #func, #ext, SUPPORTS##ext, AllowExt, 0);
	#define ALC_PROC(ext,ret,func,parms) FindProc( *(void**)&func, #func, #ext, SUPPORTS##ext, AllowExt, 1);
	#include "ALFuncs.h"
	#undef AL_EXT
	#undef ALC_EXT
	#undef AL_PROC
	#undef ALC_PROC
	unguard;
}

//
//	UALAudioSubsystem::SetEAXEffect
//
void UALAudioSubsystem::SetEAXEffect()
{
	guard(UALAudioSubsystem::SetEAXEffect);

#if SUPPORTS_EAX
	// Do nothing if EAX isn't supported.
	if( !UseEAX || !alEAXSet || !alEAXGet )
		return;

	// Check whether update is necessary.
	if( !GIsEditor && (Listener->EAXEffect == Listener->LastEAXEffect) )
		return;

	if( EAXVersion >= 3 )
	{
		if( !Listener->EAXEffect )
		{
			// Set 'plain' EAX preset.
			_EAXLISTENERPROPERTIES EAXProperties;
			appMemzero(&EAXProperties, sizeof(EAXProperties));

			EAXProperties.ulEnvironment				= EAX_ENVIRONMENT_GENERIC;
			EAXProperties.flEnvironmentSize			= 7.5f;
			EAXProperties.flEnvironmentDiffusion	= 1.0;
			EAXProperties.lRoom						= -10000;
			EAXProperties.lRoomHF					= -10000;
			EAXProperties.lRoomLF					= -10000;
			EAXProperties.flDecayTime				= 1.49f;
			EAXProperties.flDecayHFRatio			= 0.83f;
			EAXProperties.flDecayLFRatio			= 1.00f;
			EAXProperties.lReflections				= -2602;
			EAXProperties.flReflectionsDelay		= 0.007f;
			//EAXProperties.vReflectionsPan			= *((EAXVECTOR*)&Listener->ReflectionsPan);
			EAXProperties.lReverb					= 200;
			EAXProperties.flReverbDelay				= 0.011f;
			//EAXProperties.vReverbPan				= *((EAXVECTOR*)&Listener->ReverbPan);
			EAXProperties.flEchoTime				= 0.25f;
			EAXProperties.flEchoDepth				= 0.f;
			EAXProperties.flModulationTime			= 0.25f;
			EAXProperties.flModulationDepth			= 0.f;
			EAXProperties.flAirAbsorptionHF			= 0.f;
			EAXProperties.flHFReference				= 5000.f;
			EAXProperties.flLFReference				= 250.f;
			EAXProperties.flRoomRolloffFactor		= 0.f;
			EAXProperties.ulFlags					= EAXLISTENERFLAGS_DECAYTIMESCALE
													| EAXLISTENERFLAGS_REFLECTIONSSCALE
													| EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE
													| EAXLISTENERFLAGS_REVERBSCALE
													| EAXLISTENERFLAGS_REVERBDELAYSCALE
													| EAXLISTENERFLAGS_REVERBDELAYSCALE
													| EAXLISTENERFLAGS_DECAYHFLIMIT;			

			alEAXSet(&DSPROPSETID_EAX30_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS, NULL, &EAXProperties, sizeof(EAXProperties));
		}
		else
		{
			DWORD Flags = 0;
			if( Listener->EAXEffect->bDecayTimeScale )
				Flags |= EAXLISTENERFLAGS_DECAYTIMESCALE;
			if( Listener->EAXEffect->bReflectionsScale )
				Flags |= EAXLISTENERFLAGS_REFLECTIONSSCALE;
			if( Listener->EAXEffect->bReflectionsDelayScale )
				Flags |= EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE;
			if( Listener->EAXEffect->bReverbScale )
				Flags |= EAXLISTENERFLAGS_REVERBSCALE;
			if( Listener->EAXEffect->bReverbDelayScale )
				Flags |= EAXLISTENERFLAGS_REVERBDELAYSCALE;
			if( Listener->EAXEffect->bReverbDelayScale )
				Flags |= EAXLISTENERFLAGS_REVERBDELAYSCALE;
			if( Listener->EAXEffect->bEchoTimeScale )
				Flags |= EAXLISTENERFLAGS_ECHOTIMESCALE;
			if( Listener->EAXEffect->bModulationTimeScale )
				Flags |= EAXLISTENERFLAGS_MODULATIONTIMESCALE;
			if( Listener->EAXEffect->bDecayHFLimit )
				Flags |= EAXLISTENERFLAGS_DECAYHFLIMIT;

			_EAXLISTENERPROPERTIES EAXProperties;

			EAXProperties.ulEnvironment				= EAX_ENVIRONMENT_GENERIC;
			EAXProperties.flEnvironmentSize			= Listener->EAXEffect->EnvironmentSize;
			EAXProperties.flEnvironmentDiffusion	= Listener->EAXEffect->EnvironmentDiffusion;
			EAXProperties.lRoom						= Listener->EAXEffect->Room;
			EAXProperties.lRoomHF					= Listener->EAXEffect->RoomHF;
			EAXProperties.lRoomLF					= Listener->EAXEffect->RoomLF;
			EAXProperties.flDecayTime				= Listener->EAXEffect->DecayTime;
			EAXProperties.flDecayHFRatio			= Listener->EAXEffect->DecayHFRatio;
			EAXProperties.flDecayLFRatio			= Listener->EAXEffect->DecayLFRatio;
			EAXProperties.lReflections				= Listener->EAXEffect->Reflections;
			EAXProperties.flReflectionsDelay		= Listener->EAXEffect->ReflectionsDelay;
			EAXProperties.vReflectionsPan			= *((EAXVECTOR*)&Listener->EAXEffect->ReflectionsPan);
			EAXProperties.lReverb					= Listener->EAXEffect->Reverb;
			EAXProperties.flReverbDelay				= Listener->EAXEffect->ReverbDelay;
			EAXProperties.vReverbPan				= *((EAXVECTOR*)&Listener->EAXEffect->ReverbPan);
			EAXProperties.flEchoTime				= Listener->EAXEffect->EchoTime;
			EAXProperties.flEchoDepth				= Listener->EAXEffect->EchoDepth;
			EAXProperties.flModulationTime			= Listener->EAXEffect->ModulationTime;
			EAXProperties.flModulationDepth			= Listener->EAXEffect->ModulationDepth;
			EAXProperties.flAirAbsorptionHF			= 0.f; // Listener->EAXEffect->AirAbsorptionHF;
			EAXProperties.flHFReference				= Listener->EAXEffect->HFReference;
			EAXProperties.flLFReference				= Listener->EAXEffect->LFReference;
			EAXProperties.flRoomRolloffFactor		= Listener->EAXEffect->RoomRolloffFactor;
			EAXProperties.ulFlags					= Flags;

			alEAXSet(&DSPROPSETID_EAX30_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS, NULL, &EAXProperties, sizeof(EAXProperties));
		}
	}
	else
	{
		if( !Listener->EAXEffect )
		{
			// Set 'plain' EAX preset.
			_EAX20LISTENERPROPERTIES EAXProperties;
			appMemzero(&EAXProperties, sizeof(EAXProperties));

			EAXProperties.dwEnvironment				= EAX20_ENVIRONMENT_GENERIC;
			EAXProperties.flEnvironmentSize			= 7.5f;
			EAXProperties.flEnvironmentDiffusion	= 1.0;
			EAXProperties.lRoom						= -10000;
			EAXProperties.lRoomHF					= -10000;
			EAXProperties.flDecayTime				= 1.49f;
			EAXProperties.flDecayHFRatio			= 0.83f;
			EAXProperties.lReflections				= -2602;
			EAXProperties.flReflectionsDelay		= 0.007f;
			EAXProperties.lReverb					= 200;
			EAXProperties.flReverbDelay				= 0.011f;
			EAXProperties.flAirAbsorptionHF			= 0.f;
			EAXProperties.flRoomRolloffFactor		= 0.f;
			EAXProperties.dwFlags					= EAX20LISTENERFLAGS_DECAYTIMESCALE
				| EAX20LISTENERFLAGS_REFLECTIONSSCALE
				| EAX20LISTENERFLAGS_REFLECTIONSDELAYSCALE
				| EAX20LISTENERFLAGS_REVERBSCALE
				| EAX20LISTENERFLAGS_REVERBDELAYSCALE
				| EAX20LISTENERFLAGS_REVERBDELAYSCALE
				| EAX20LISTENERFLAGS_DECAYHFLIMIT;			

			alEAXSet(&DSPROPSETID_EAX20_ListenerProperties,DSPROPERTY_EAX20LISTENER_ALLPARAMETERS, NULL, &EAXProperties, sizeof(EAXProperties));
		}
		else
		{
			DWORD Flags = 0;
			if( Listener->EAXEffect->bDecayTimeScale )
				Flags |= EAX20LISTENERFLAGS_DECAYTIMESCALE;
			if( Listener->EAXEffect->bReflectionsScale )
				Flags |= EAX20LISTENERFLAGS_REFLECTIONSSCALE;
			if( Listener->EAXEffect->bReflectionsDelayScale )
				Flags |= EAX20LISTENERFLAGS_REFLECTIONSDELAYSCALE;
			if( Listener->EAXEffect->bReverbScale )
				Flags |= EAX20LISTENERFLAGS_REVERBSCALE;
			if( Listener->EAXEffect->bReverbDelayScale )
				Flags |= EAX20LISTENERFLAGS_REVERBDELAYSCALE;
			if( Listener->EAXEffect->bReverbDelayScale )
				Flags |= EAX20LISTENERFLAGS_REVERBDELAYSCALE;
			if( Listener->EAXEffect->bDecayHFLimit )
				Flags |= EAX20LISTENERFLAGS_DECAYHFLIMIT;

			_EAX20LISTENERPROPERTIES EAXProperties;

			EAXProperties.dwEnvironment				= EAX20_ENVIRONMENT_GENERIC;
			EAXProperties.flEnvironmentSize			= Listener->EAXEffect->EnvironmentSize;
			EAXProperties.flEnvironmentDiffusion	= Listener->EAXEffect->EnvironmentDiffusion;
			EAXProperties.lRoom						= Listener->EAXEffect->Room;
			EAXProperties.lRoomHF					= Listener->EAXEffect->RoomHF;
			EAXProperties.flDecayTime				= Listener->EAXEffect->DecayTime;
			EAXProperties.flDecayHFRatio			= Listener->EAXEffect->DecayHFRatio;
			EAXProperties.lReflections				= Listener->EAXEffect->Reflections;
			EAXProperties.flReflectionsDelay		= Listener->EAXEffect->ReflectionsDelay;
			EAXProperties.lReverb					= Listener->EAXEffect->Reverb;
			EAXProperties.flReverbDelay				= Listener->EAXEffect->ReverbDelay;
			EAXProperties.flAirAbsorptionHF			= 0.f;// Listener->EAXEffect->AirAbsorptionHF;
			EAXProperties.flRoomRolloffFactor		= Listener->EAXEffect->RoomRolloffFactor;
			EAXProperties.dwFlags					= Flags;

			alEAXSet(&DSPROPSETID_EAX20_ListenerProperties,DSPROPERTY_EAX20LISTENER_ALLPARAMETERS, NULL, &EAXProperties, sizeof(EAXProperties));
		}
	}
#endif
	unguard;
}

//
//	alError
//
UBOOL UALAudioSubsystem::alError( TCHAR* Text, UBOOL Log )
{
	ALint Error = alGetError();
	if ( Error == AL_NO_ERROR )
		return false;
	else
	{
		do 
		{		
			if ( Log )
			{
				switch ( Error )
				{
				case AL_INVALID_NAME:
					debugf(TEXT("ALAudio: AL_INVALID_NAME in %s"), Text);
					break;
				#if __WIN32__  // !!! FIXME: This isn't in the Linux reference implemention? --ryan.
				case AL_INVALID_ENUM:
					debugf(TEXT("ALAudio: AL_INVALID_ENUM in %s"), Text);
					break;
				#endif
				case AL_INVALID_VALUE:
					debugf(TEXT("ALAudio: AL_INVALID_VALUE in %s"), Text);
					break;
				#if __WIN32__  // !!! FIXME: This isn't in the Linux reference implemention? --ryan.
				case AL_INVALID_OPERATION:
					debugf(TEXT("ALAudio: AL_INVALID_OPERATION in %s"), Text);
					break;
				#endif
				case AL_OUT_OF_MEMORY:
					debugf(TEXT("ALAudio: AL_OUT_OF_MEMORY in %s"), Text);
					break;
				default:
					debugf(TEXT("ALAudio: Unknown error in %s"), Text);
				}
			}
		}
		while( (Error = alGetError()) != AL_NO_ERROR );
		return true;
	}
}


/*------------------------------------------------------------------------------------
	FALSoundSource.
------------------------------------------------------------------------------------*/

//
//	FALSoundSource::FALSoundSource
//
FALSoundSource::FALSoundSource( UALAudioSubsystem* InAudioSubsystem )
: FSoundSource( InAudioSubsystem )
{
	guard(FALSoundSource::FALSoundSource());
	Paused		= 0;
	SourceId	= -1;
	Buffer		= NULL;
	unguard;
}

//
//	FALSoundSource::~FALSoundSource
//
FALSoundSource::~FALSoundSource()
{
	guard(FALSoundSource::~FALSoundSource());
	Stop();
	UALAudioSubsystem::alDeleteSources( 1, &SourceId );
	unguard;
}

//
//	FALSoundSource::Update
//
UBOOL FALSoundSource::Update( FLOAT DeltaTime )
{
	guard(FALSoundSource::Update);

	// USoundSource::UpdateBase has already been called by UAudioSystem::Update at this point.

	if( !Initialized || Paused )
		return 0;

	UALAudioSubsystem* AudioSubsystem = (UALAudioSubsystem*) this->AudioSubsystem;
	if( (Flags & SF_UpdatePitch) && !AudioSubsystem->DisablePitch )
		UALAudioSubsystem::alSourcef( SourceId, AL_PITCH, Clamp<FLOAT>( Pitch, 0.5f, 2.0f) );
		
	FLOAT OneOverVoiceVolume = 1.f;
	if( (GCurrentTime - AudioSubsystem->LastVoiceTime < 0.2) && !(Flags & SF_Voice) )
		OneOverVoiceVolume = AudioSubsystem->VoiceVolume > 1.f ? 1.f / AudioSubsystem->VoiceVolume : 1.f;

	UALAudioSubsystem::alSourcef( SourceId, AL_GAIN, Clamp(OneOverVoiceVolume * UsedVolume * (AudioSubsystem->DampenVolume ? AudioSubsystem->VolumeScaleRec : 1.f), 0.0f, 1.0f) );

	if( !(Flags & SF_No3D) )
	{
		FVector ALLocation;
		ALLocation.X = Location.X;
		ALLocation.Y = Location.Z;
		ALLocation.Z = Location.Y;
		ALLocation *= AUDIO_DISTANCE_FACTOR;

		UALAudioSubsystem::alSourcefv( SourceId, AL_POSITION			, (ALfloat *) &ALLocation );
		UALAudioSubsystem::alSourcef ( SourceId, AL_REFERENCE_DISTANCE	, UsedRadius * AUDIO_DISTANCE_FACTOR );

#if SUPPORTS_EAX
		if( AudioSubsystem->alEAXSet != NULL )
		{
			long Occlusion = (long) (868.589f * log( Max( UsedRadius / Radius, 0.00001f) ) ) ;
			if( AudioSubsystem->EAXVersion >= 3 )
		        UALAudioSubsystem::alEAXSet(&DSPROPSETID_EAX30_BufferProperties, DSPROPERTY_EAXBUFFER_OCCLUSION, SourceId, &Occlusion, sizeof(Occlusion));
			else
				UALAudioSubsystem::alEAXSet(&DSPROPSETID_EAX20_BufferProperties, DSPROPERTY_EAX20BUFFER_OCCLUSION, SourceId, &Occlusion, sizeof(Occlusion));
		}
#endif
	}

	// Streaming sounds need a bit of special attention.
	if( (Flags & SF_Streaming) && Buffer->AudioStream )
	{
		// Retrieve Source status information.
		INT BuffersProcessed,
			BuffersQueued,
			BuffersInFlight,
			SourceState;

		UALAudioSubsystem::alGetSourcei( SourceId, AL_SOURCE_STATE		, &SourceState		);
		UALAudioSubsystem::alGetSourcei( SourceId, AL_BUFFERS_PROCESSED, &BuffersProcessed	);
		UALAudioSubsystem::alGetSourcei( SourceId, AL_BUFFERS_QUEUED	, &BuffersQueued	);

		UBOOL Alive = Buffer->AudioStream->QueryStream( BuffersInFlight );

		// Queue incoming packet, space permitting.
		if( BuffersProcessed && !BuffersInFlight )
		{
			INT& BufferQueueIndex	= Buffer->BufferQueueIndex;
			INT& BufferUnqueueIndex = Buffer->BufferUnqueueIndex;

			// Unqueue, update and queue again.
			UALAudioSubsystem::alSourceUnqueueBuffers( SourceId, 1, &Buffer->BufferIds(BufferUnqueueIndex) );
			UALAudioSubsystem::alBufferData( Buffer->BufferIds(BufferQueueIndex), Buffer->AudioStream->GetFormat(), Buffer->AudioStreamData, Buffer->AudioStream->GetChunkSize(), Buffer->AudioStream->GetRate() );
			UALAudioSubsystem::alSourceQueueBuffers( SourceId, 1, &Buffer->BufferIds(BufferQueueIndex) );
			
			Buffer->AudioStream->RequestChunks( 1, Buffer->AudioStreamData );

			++BufferQueueIndex	 %= Buffer->BufferIds.Num();
			++BufferUnqueueIndex %= Buffer->BufferIds.Num();

			// Start playing again.
			if( SourceState == AL_STOPPED )
				Play();
		}
		else 
		if( !Alive )
		{
			AudioSubsystem->StreamManager->DestroyStream( Buffer->AudioStream, 0 );
			Buffer->AudioStream		= NULL;
			delete Buffer->AudioStreamData;
			Buffer->AudioStreamData = NULL;

			// Let PlayerController know that stream finished.
			UViewport* Viewport = AudioSubsystem->Viewport;
			if( Viewport && Viewport->Actor && !(Buffer->Sound->GetFlags() & SF_Music) )
				Viewport->Actor->eventStreamFinished( Buffer->Sound->GetIndex(), STREAMFINISH_EOF );
		}
		else
		{
			// Start playing again so buffers can become processed.
			if( SourceState == AL_STOPPED )
				Play();
		}
	}


	return 1;
	unguard;
}

//
//	FALSoundSource::Play
//
void FALSoundSource::Play()
{
	guard(FALSoundSource::Play);
	if( Initialized )
	{
		UALAudioSubsystem::alSourcePlay( SourceId );
		Paused	= 0;
		Playing = 1;
	}
	unguard;
}

//
//	FALSoundSource::Stop
//
void FALSoundSource::Stop()
{
	guard(FALSoundSource::Stop);
	
	if( Initialized )
	{	
		UALAudioSubsystem::alSourceStop( SourceId );
		UALAudioSubsystem::alSourcei( SourceId, AL_BUFFER, 0 );
		Buffer		= NULL;
		Initialized = 0;
		Paused		= 0;
		Playing		= 0;
		Priority	= 0;	
		Sound		= NULL;
		Flags		= 0;
	}
	unguard;
}

//
//	FALSoundSource::Pause
//
void FALSoundSource::Pause()
{
	guard(FALSoundSource::Pause);
	if( Initialized )
	{
		UALAudioSubsystem::alSourcePause( SourceId );
		Paused = 1;
	}
	unguard;
}

//
//	FALSoundSource::IsStopped
//
UBOOL FALSoundSource::IsStopped()
{
	guard(FALSoundSource::IsStopped);
	if( Initialized )
	{
		ALint State;
		UALAudioSubsystem::alGetSourcei( SourceId, AL_SOURCE_STATE, &State );
		return State == AL_STOPPED;
	}
	else
		return 0;
	unguard;
}

//
//	FALSoundSource::IsFinished
//
UBOOL FALSoundSource::IsFinished( FLOAT DeltaTime )
{
	guard(FALSoundSource::IsFinished);
	if( Initialized && Buffer )
	{
		if( (Flags & SF_Streaming) && !Buffer->AudioStream )
			return 1;
		if( IsStopped() )
		{
			InactiveTime += DeltaTime;
			if( Flags & SF_Voice )
			{
				if( InactiveTime > 0.5f )
					return 1;
				else
					return 0;
			}
			else if( Flags & SF_Queueing )
				return 0;
			else
				return 1;
		}
	}
	InactiveTime = 0.f;
	return 0;
	unguard;
}

/*------------------------------------------------------------------------------------
	FALSoundBuffer.
------------------------------------------------------------------------------------*/

//
//	FALSoundBuffer::FALSoundBuffer
//
FALSoundBuffer::FALSoundBuffer( UALAudioSubsystem* InAudioSubsystem )
{
	guard(FALSoundBuffer::FALSoundBuffer);
	AudioSubsystem		= InAudioSubsystem;
	AudioStream			= NULL;
	AudioStreamData		= NULL;
	Sound				= NULL;
	BufferQueueIndex	= 0;
	BufferUnqueueIndex	= 0;
	unguard;
}

//
//	FALSoundBuffer::~FALSoundBuffer
//
FALSoundBuffer::~FALSoundBuffer()
{
	guard(FALSoundBuffer::~FALSoundBuffer);
	Reset();
	unguard;
}

//
//	FALSoundBuffer::Init
//
FALSoundBuffer* FALSoundBuffer::Init( USound* Sound, UALAudioSubsystem* AudioSubsystem )
{
	guard(FALSoundBuffer::Init);
	if( !Sound->GetHandle() )
	{
		FALSoundBuffer* Buffer = new FALSoundBuffer( AudioSubsystem );

		if( Sound->GetFlags() & SF_Streaming )
		{
			// By default music is looping.
			EAudioStreamType Type = ST_OggLooping;

			// Determine format of stream by extension.
			if( !(Sound->GetFlags() & SF_Music) )
			{
				FString Extension = FString(Sound->GetFilename()).Right(3);

				if( appStricmp( *Extension, TEXT("mp3") ) == 0 )
					Type = ST_MP3;
				else
				if( appStricmp( *Extension, TEXT("ogg") ) == 0 )
					Type = ST_Ogg;
				else
				{
					// Fail if neither .mp3 nor .ogg
					delete Buffer;
					return NULL;
				}
			}
			
			// Create stream.
			BYTE* Data = new BYTE[MAX_STREAM_CHUNKSIZE * STREAM_NUMCHUNKS];
			Buffer->AudioStream = AudioSubsystem->StreamManager->CreateStream( Sound->GetFilename(), STREAM_NUMCHUNKS, Data, Type, Sound->InitialSeekTime );
	
			// Clean up on error.
			if( !Buffer->AudioStream )
			{
				UViewport* Viewport = AudioSubsystem->Viewport;
				if( Viewport && Viewport->Actor && !(Sound->GetFlags() & SF_Music) )
					Viewport->Actor->eventStreamFinished( Sound->GetIndex(), STREAMFINISH_Error );
				Sound->SetHandle(0);
				delete Buffer;
				return NULL;
			}

			// Set duration.
			Sound->SetDuration( Buffer->AudioStream->GetDuration() );
	
			// Create AL buffers.
			ALuint bid[STREAM_NUMCHUNKS];
			UALAudioSubsystem::alGenBuffers( STREAM_NUMCHUNKS, bid );
			UALAudioSubsystem::alError(TEXT("RegisterSound (generating streaming buffers)"));

			// Register Buffer and fill it with data.
			Sound->SetHandle( Buffer );
			for( INT i=0; i<STREAM_NUMCHUNKS; i++ )
			{
				UALAudioSubsystem::alBufferData( bid[i], Buffer->AudioStream->GetFormat(), Data + i * Buffer->AudioStream->GetChunkSize(), Buffer->AudioStream->GetChunkSize(), Buffer->AudioStream->GetRate() );
				Buffer->BufferIds.AddItem( bid[i] );
			}
			AudioSubsystem->Buffers.AddItem( Buffer );
			Buffer->Sound = Sound;
			delete Data;

			// Request more chunks.
			Buffer->AudioStreamData = new BYTE[Buffer->AudioStream->GetChunkSize()];
			Buffer->AudioStream->RequestChunks( 1, Buffer->AudioStreamData );
		}
		else
		if( Sound->GetFlags() & SF_Voice )
		{
			ALuint bid[NUM_VOICE_BUFFERS];
			UALAudioSubsystem::alGenBuffers( NUM_VOICE_BUFFERS, bid );
			UALAudioSubsystem::alError(TEXT("RegisterSound (generating voice buffers)"));

			INT		FrameSize	= UAudioSubsystem::CodecFrameSize[CODEC_TO_INDEX(Sound->VoiceCodec)];
			SHORT*	Dummy		= new SHORT[FrameSize];
			appMemzero( Dummy, FrameSize * sizeof(SHORT) );

			for( INT i=0; i<NUM_VOICE_BUFFERS; i++ )
			{
				Buffer->BufferIds.AddItem( bid[i] );
				UALAudioSubsystem::alBufferData( bid[i], AL_FORMAT_MONO16, Dummy, FrameSize * sizeof(SHORT), UAudioSubsystem::CodecFrequency[CODEC_TO_INDEX(Sound->VoiceCodec)] );
			}
			delete [] Dummy;

			AudioSubsystem->Buffers.AddItem( Buffer );

			Sound->SetHandle( Buffer );
			Buffer->Sound = Sound;
		}
		else
		{
			// Load the data.
			Sound->GetData().Load();
			if( Sound->GetData().Num() == 0 )
			{
				debugf(TEXT("%s"), Sound->GetPathName());
				check(1);
			}
			check(Sound->GetData().Num()>0);
	
			FWaveModInfo WaveInfo;
			if (!WaveInfo.ReadWaveInfo(Sound->GetData()))
			{
				debugf( NAME_Warning, TEXT("%s is not a WAVE file?"), Sound->GetPathName() );
				Sound->GetData().Unload();
				Sound->SetHandle(0);
				delete Buffer;
				return NULL;
			}

			//INT Flags = WaveInfo.SampleLoopsNum ? SF_Looping : 0;

			ALint Format;
			if ( *WaveInfo.pBitsPerSample == 8 )
			{
				// debugf( NAME_Warning, TEXT("8 bit sound detected [%s]"), Sound->GetPathName() );
				if (*WaveInfo.pChannels == 1)
					Format = AL_FORMAT_MONO8;
				else
					Format = AL_FORMAT_STEREO8;
			}
			else
			{
				if (*WaveInfo.pChannels == 1)
					Format = AL_FORMAT_MONO16;
				else
					Format = AL_FORMAT_STEREO16;				
			}

			if ( *WaveInfo.pChannels != 1 )
			{
				debugf( NAME_Warning, TEXT("Shouldn't use stereo sound: %s"), Sound->GetPathName() );
				Sound->GetData().Unload();
				Sound->SetHandle(0);
				delete Buffer;
				return NULL;
			}

			ALuint bid;
			UALAudioSubsystem::alGenBuffers( 1, &bid );
			UALAudioSubsystem::alError(TEXT("RegisterSound (generating buffer)"));
		
			UALAudioSubsystem::alBufferData( bid, Format, WaveInfo.SampleDataStart, WaveInfo.SampleDataSize, *WaveInfo.pSamplesPerSec );
	
			// Unload the data.
			Sound->GetData().Unload();

			if( UALAudioSubsystem::alError(TEXT("RegisterSound (creating buffer)")) )
			{			
				Sound->SetHandle(0);
				delete Buffer;
				return NULL;
			}
			else
			{
				// Initialization succeeded.
				Sound->SetHandle( Buffer );
				Buffer->BufferIds.AddItem( bid );
				Buffer->Sound = Sound;
				AudioSubsystem->Buffers.AddItem( Buffer );
			}
		}

		return Buffer;
	} 
	else
		return (FALSoundBuffer*) Sound->GetHandle();

	unguard;
}

//
//	FALSoundBuffer::Reset
//
void FALSoundBuffer::Reset()
{
	guard(FALSoundBuffer::Reset());
	
	// Stop sources using this buffer.
	if( BufferIds.Num() )
		for( INT i=0; i<AudioSubsystem->Sources.Num(); i++ )
			if( AudioSubsystem->Sources(i)->Buffer == this )
				AudioSubsystem->Sources(i)->Stop();

	// Delete AL buffers.
	for( INT i=0; i<BufferIds.Num(); i++ )	
		UALAudioSubsystem::alDeleteBuffers( 1, &BufferIds(i) );
	BufferIds.Empty();
	
	// Unregister handle.
	if( Sound )
		Sound->SetHandle(0);	
	Sound = NULL;

	// Stop stream.
	if( AudioStream )
	{
		AudioSubsystem->StreamManager->DestroyStream( AudioStream, 0 );
		AudioStream = NULL;
	}

	delete AudioStreamData;
	AudioStreamData = NULL;

	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


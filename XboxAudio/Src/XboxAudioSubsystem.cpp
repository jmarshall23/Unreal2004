/*=======================================s======================================
	XBoxAudioSubsystem.cpp: Unreal XBox Audio interface object.
	Copyright 1999-2001 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Daniel Vogel.
=============================================================================*/

/*------------------------------------------------------------------------------------
	Audio includes.
------------------------------------------------------------------------------------*/

#include "XBoxAudioPrivate.h"
#include <math.h>

#define USE_DEFERRED 1
#if USE_DEFERRED
#define COMMIT_MODE DS3D_DEFERRED
#else
#define COMMIT_MODE DS3D_IMMEDIATE
#endif

/*------------------------------------------------------------------------------------
	UXBoxAudioSubsystem.
------------------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UXBoxAudioSubsystem);


UXBoxAudioSubsystem::UXBoxAudioSubsystem()
{
	guard(UXBoxAudioSubsystem::UXBoxAudioSubsystem);
	LastTime	= appSeconds();
	unguard;
}


void UXBoxAudioSubsystem::StaticConstructor()
{
	guard(UXBoxAudioSubsystem::StaticConstructor);
	new(GetClass(),TEXT("UsePrecache"),     RF_Public)UBoolProperty  (CPP_PROPERTY(UsePrecache    ), TEXT("XBoxAudio"), CPF_Config );
	new(GetClass(),TEXT("ReverseStereo"),   RF_Public)UBoolProperty  (CPP_PROPERTY(ReverseStereo  ), TEXT("XBoxAudio"), CPF_Config );
	new(GetClass(),TEXT("Channels"),        RF_Public)UIntProperty   (CPP_PROPERTY(MaxChannels    ), TEXT("XBoxAudio"), CPF_Config );
	new(GetClass(),TEXT("MusicVolume"),     RF_Public)UFloatProperty (CPP_PROPERTY(MusicVolume    ), TEXT("XBoxAudio"), CPF_Config );
	new(GetClass(),TEXT("SoundVolume"),     RF_Public)UFloatProperty (CPP_PROPERTY(SoundVolume    ), TEXT("XBoxAudio"), CPF_Config );
	new(GetClass(),TEXT("VoiceVolume"),     RF_Public)UFloatProperty (CPP_PROPERTY(VoiceVolume    ), TEXT("XBoxAudio"), CPF_Config ); // gam
	unguard;
}


/*------------------------------------------------------------------------------------
	UObject Interface.
------------------------------------------------------------------------------------*/

void UXBoxAudioSubsystem::PostEditChange()
{
	guard(UXBoxAudioSubsystem::PostEditChange);
	appErrorf(TEXT("PostEditChange"));
	unguard;
}


void UXBoxAudioSubsystem::Destroy()
{
	guard(UXBoxAudioSubsystem::Destroy);
	if(Initialized)
	{
		// Unhook.
		USound::Audio = NULL;

		// Stop sounds and mark buffers for deletion.
		for (INT i=0; i<Sources.Num(); i++)
			StopSound(i);
		Sources.Empty();

		// Unregister sounds.
		if (Viewport)
		{
			for(TObjectIterator<USound> SoundIt;SoundIt;++SoundIt)
				UnregisterSound(*SoundIt);
		}

		// Deferred buffer deletion.
		for( INT iBuffer=0; iBuffer<StoppedBuffers.Num(); iBuffer )
		{
			StoppedBuffers(iBuffer)->Release();
			StoppedBuffers.Remove(iBuffer);
		}

		// DSBuffer is 'released' in StopSound.
		Buffers.Empty();

		// Release DirectSound interface.
		DirectSound8->Release();
		DirectSound8 = NULL;
		
		SetViewport(NULL);

		// Close voice chat socket.
		closesocket(VoiceSocket);

		debugf(NAME_Exit,TEXT("XBox Audio subsystem shut down."));
	}
	Super::Destroy();
	unguard;
}


void UXBoxAudioSubsystem::ShutdownAfterError()
{
	guard(UXBoxAudioSubsystem::ShutdownAfterError);

	if(Initialized)
	{
		// Unhook.
		USound::Audio = NULL;
		debugf(NAME_Exit,TEXT("UXBoxAudioSubsystem::ShutdownAfterError"));
	}

	Super::ShutdownAfterError();

	unguard;
}


void UXBoxAudioSubsystem::Serialize( FArchive& Ar )
{
	guard(UALAudioSubsystem::Serialize);
	Super::Serialize(Ar);
	if( !Ar.IsLoading() && !Ar.IsSaving() )
	{
		for( INT i=0; i<Sources.Num(); i++ )
			Ar << Sources(i).Sound;
	}
	unguard;
}


/*------------------------------------------------------------------------------------
	UAudioSubsystem Interface.
------------------------------------------------------------------------------------*/

//!!vogel: UC E3 HACK
extern UBOOL InitSockets( FString& Error );
extern void IpSetBytes( in_addr& Addr, BYTE Ip1, BYTE Ip2, BYTE Ip3, BYTE Ip4 );
UBOOL UXBoxAudioSubsystem::Init()
{
	guard(UXBoxAudioSubsystem::Init);

	if (Initialized)
		return 1;

	HRESULT hr;
	
	if ( FAILED ( hr = DirectSoundCreate( NULL, &DirectSound8, NULL ) ) )
	{
		XBoxSoundError( hr );
		return 0;
	}

	DSEFFECTIMAGELOC dsImageLoc;
	dsImageLoc.dwI3DL2ReverbIndex	= DSFX_IMAGELOC_UNUSED;
	dsImageLoc.dwCrosstalkIndex		= Graph5_XTalk;
	if( FAILED(hr=XAudioDownloadEffectsImage( "d:\\xbox.bin", &dsImageLoc, XAUDIO_DOWNLOADFX_EXTERNFILE, &DSPImage )) )
	{
		XBoxSoundError( hr );
		return 0;
	}

	// Initialize Channels.
	Sources.Empty(Min(MaxChannels, MAX_AUDIOCHANNELS));
	Sources.AddZeroed(Min(MaxChannels, MAX_AUDIOCHANNELS));
	
	// Initialize streams.
	Streams.Empty( MAX_AUDIOSTREAMS );
	Streams.AddZeroed( MAX_AUDIOSTREAMS );

	// Initialize variables.
	Initialized					= 1;
	LastRealtime				= 0;
	OldListener					= NULL;

	// Initialize voice chat.
#if USE_VOICE_CHAT
	FString Error;
	InitSockets( Error );

	XNADDR xna;
	while( XNetGetTitleXnAddr( &xna ) == XNET_GET_XNADDR_PENDING );
	LocalIpAddress = xna.ina.s_addr;

	debugf(TEXT("LocalIpAddress = %i"), LocalIpAddress);

	// Network stuff initialization
	VoiceSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( INVALID_SOCKET ==  VoiceSocket )
	{
		hr = WSAGetLastError();
		appErrorf(TEXT("Couldn't open voice socket [%i]"), hr );
	}

	SOCKADDR_IN VoiceAddr;
	VoiceAddr.sin_family		= AF_INET;
	VoiceAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	VoiceAddr.sin_port			= htons( VOICE_PORT );

	if( bind(VoiceSocket, (const sockaddr*)(&VoiceAddr), sizeof( SOCKADDR_IN )) == SOCKET_ERROR )
	{
	   hr = WSAGetLastError();
	   appErrorf(TEXT("Couldn't bind to voice socket") );
	   return hr;
	}

	DWORD NonBlocking = 1;
	if( ioctlsocket(VoiceSocket, FIONBIO, &NonBlocking) == SOCKET_ERROR )
	{
	   hr = WSAGetLastError();
	   appErrorf(TEXT("Couldn't change socket to non blocking."));
	}

	VoiceManager = new CVoiceManager( this );
	VoiceManager->EnterChatSession();
#endif

	// Initialize stats.
    XBoxAudioStats.Init();

	// Initialized.
	USound::Audio = this;

	debugf(NAME_Init,TEXT("XBoxAudio: subsystem initialized."));
	return 1;

	unguard;
}


void UXBoxAudioSubsystem::SetViewport(UViewport* InViewport)
{
	guard(UXBoxAudioSubsystem::SetViewport);

	// Stop playing sounds.
	for(INT i=0; i<Sources.Num(); i++)
		if(Sources(i).Started && !(Sources(i).Flags & SF_Music))
			StopSound(i);

	if(Viewport != InViewport)
	{
		// Switch viewports.
		Viewport = InViewport;
		
		// Set listener region to 0.
		appMemzero( &RegionListener, sizeof(RegionListener) );		

		if(Viewport)
		{
			// needed for doppler effect & HRTF
			DirectSound8->SetDistanceFactor( 0.01875f, COMMIT_MODE );
			
			// Global rolloff factor.
			DirectSound8->SetRolloffFactor( 0.5f, COMMIT_MODE );

			// Full vs light is ~ 70kb.
			DirectSoundUseFullHRTF();

			// Update the voice masks.
			VoiceManager->UpdateVoiceMasks();
		}

		if( Viewport && UsePrecache )
		{
			// Register everything.
			for(TObjectIterator<USound> SoundIt;SoundIt;++SoundIt)
			{
			    // gam ---
			    FLOAT TempPitch = 1.f, TempVolume = 1.f;
			    SoundIt->RenderSoundPlay( &TempPitch, &TempVolume);
                if( !SoundIt->IsValid() )
                    continue;
				// --- gam

				RegisterSound(*SoundIt);
			}
		}
	}

	unguard;
}


UViewport* UXBoxAudioSubsystem::GetViewport()
{
	guard(UXBoxAudioSubsystem::GetViewport);
	return Viewport;
	unguard;
}


void UXBoxAudioSubsystem::RegisterSound(USound* Sound)
{
	guard(UXBoxAudioSubsystem::RegisterSound);

	checkSlow(Sound);

	if( !Sound->GetHandle() ) // gam
	{
		// Avoid recursion as USound->Load calls RegisterSound.
		Sound->SetHandle( 0xDEADBEEF ); // sjs
		
		//debugf( NAME_DevSound, TEXT("Register sound: %s"), Sound->GetPathName() );
		if ( Sound->GetFlags() & SF_Streaming ) // gam
		{
			appErrorf(TEXT("Streaming"));
		}
		else
		{
			// Load the data. Unloaded in UnregisterSound.
			Sound->GetData().Load(); // gam
			check(Sound->GetData().Num()>0); // gam

			debugf( NAME_DevSound, TEXT("Register sound: %s (%i)"), Sound->GetPathName(), Sound->GetData().Num() ); // gam
	
			FWaveModInfo WaveInfo;
			WaveInfo.ReadWaveInfo(Sound->GetData()); // gam

			INT i = Buffers.AddZeroed(1);
			
			Buffers(i).wfx.wFormatTag		= WAVE_FORMAT_PCM;
			Buffers(i).wfx.nChannels		= *WaveInfo.pChannels;
			Buffers(i).wfx.nSamplesPerSec	= *WaveInfo.pSamplesPerSec;
			Buffers(i).wfx.nAvgBytesPerSec	= *WaveInfo.pAvgBytesPerSec;
			Buffers(i).wfx.nBlockAlign		= *WaveInfo.pBlockAlign;
			Buffers(i).wfx.wBitsPerSample	= *WaveInfo.pBitsPerSample; 
			Buffers(i).wfx.cbSize			= 0;

			Buffers(i).Rate					= *WaveInfo.pSamplesPerSec;
			Buffers(i).Sound				= Sound;
			Buffers(i).Flags				= WaveInfo.SampleLoopsNum ? SF_Looping : 0;
			Buffers(i).Size					= WaveInfo.SampleDataSize;
			Buffers(i).Data					= WaveInfo.SampleDataStart;

			Sound->SetHandle (i+1); // gam
		}
	} 
	unguard;
}


void UXBoxAudioSubsystem::UnregisterSound(USound* Sound)
{
	guard(UXBoxAudioSubsystem::UnregisterSound);

	for( INT i=0; i<Sources.Num(); i++ )
		if( Sources(i).Sound == Sound )
			StopSound(i);

	if (Sound->GetHandle()) // gam
	{
		debugf( NAME_DevSound, TEXT("Unregister sound: %s"), Sound->GetPathName() );
		INT i = Sound->GetHandle()-1; // gam
		Sound->GetData().Unload(); // gam
		appMemzero( &Buffers(i), sizeof(XBBuffer) );
	}

	unguard;
}


UBOOL UXBoxAudioSubsystem::PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch, INT Flags, FLOAT FadeDuration, FLOAT InPriority )
{
	guard(UXBoxAudioSubsystem::PlaySound);

	static INT FreeSlot;
	check(Radius);

    // gam ---

    Sound->RenderSoundPlay( &Volume, &Pitch );

	if( !Viewport || !Sound)
		return 0;
    if( !Sound->IsValid() )
		return 0;
	if( !Sound->GetHandle() )
		RegisterSound(Sound);
	if( !Sound->GetHandle() )
		return 0;

	clock(GStats.DWORDStats(XBoxAudioStats.STATS_PlaySoundCycles));
	GStats.DWORDStats(XBoxAudioStats.STATS_PlaySoundCalls)++;

	// Global volume.
	Volume *= SoundVolume;

	// Compute priority.
	FLOAT Priority;
	
	if( InPriority && ((Id & 14) == SLOT_Ambient * 2) )
	{
		// Ambientsounds have their priority calculated in Update.
		Priority = InPriority;
	}
	else
	{
		// Calculate occlusion before priority.
		FLOAT OcclusionRadius = Radius;
		if( !(Flags & SF_No3D) && Actor && Actor->GetLevel() )
		{
			guard(SoundOcclusion);
			clock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));
			if( !Actor->GetLevel()->IsAudibleAt( Location, LastPosition, Actor, (ESoundOcclusion) Actor->SoundOcclusion ) )
			{
				FPointRegion RegionSource	= Actor->GetLevel()->Model->PointRegion( Actor->Level, Location );
				OcclusionRadius = Radius * Actor->GetLevel()->CalculateRadiusMultiplier( RegionSource.ZoneNumber, RegionListener.ZoneNumber );
				OcclusionRadius *= OCCLUSION_FACTOR;
			}
			unclock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));
			unguard;
		}
		Priority = SoundPriority( Viewport, Location, Volume, OcclusionRadius, Flags );
	}
	
	FLOAT BestPriority	= Priority;

	// Allocate a new slot if requested.
	if( (Id&14)==2*SLOT_None )
		Id = 16 * --FreeSlot;

	INT Index = -1;

	// Find a voice to play the sound in.
	for(INT i=0; i<Sources.Num(); i++ )
	{
		if( (Sources(i).Id&~1)==(Id&~1) )
		{
			// Skip if not interruptable.
			if( Id&1 )
				return 0;

			// Stop the sound.
			Index = i;
			break;
		}
		else if( (Sources(i).Priority * PLAYING_PRIORITY_MULTIPLIER ) < BestPriority )
		{
			Index = i;
			BestPriority = Sources(i).Priority;
		}
	}

	// If no sound, or its priority is overruled, don't play it.
	if( Index==-1 )
	{
		unclock(GStats.DWORDStats(XBoxAudioStats.STATS_PlaySoundCycles));
		return 0;
	}

	StopSound(Index);
	
	Pitch  = Clamp<FLOAT>(Pitch,0.5f,2.0f);
	Volume = Clamp<FLOAT>(Volume,0.01f,1.0f);

	// Set default values.
	Sources(Index).ZoneRadius	= Radius;
	Sources(Index).UsedRadius	= Radius;
	Sources(Index).WantedRadius = Radius;

	// Calculate initial radius.
	if( !(Flags & SF_No3D) && Actor && Actor->GetLevel() )
	{
		guard(SoundOcclusion);
		clock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));
		if( !Actor->GetLevel()->IsAudibleAt( Location, LastPosition, Actor, (ESoundOcclusion) Actor->SoundOcclusion ) )
		{
			FPointRegion RegionSource	= Actor->GetLevel()->Model->PointRegion( Actor->Level, Location );
			FLOAT TempRadius = Radius * Actor->GetLevel()->CalculateRadiusMultiplier( RegionSource.ZoneNumber, RegionListener.ZoneNumber );
			Sources(Index).ZoneRadius	= TempRadius;
			TempRadius *= OCCLUSION_FACTOR;
			Sources(Index).UsedRadius	= TempRadius;
			Sources(Index).WantedRadius	= TempRadius;
		}
		unclock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));
		unguard;
	}

	// Setup the voice.
	Sources(Index).Sound	= Sound;
	Sources(Index).Id		= Id;
	if (Flags & SF_Streaming)
		Sources(Index).Flags	= Flags;//!!TODO | Streams( Sound->Handle-1 ).Flags;
	else
		Sources(Index).Flags	= Flags | Buffers( Sound->GetHandle()-1 ).Flags; // gam
	Sources(Index).Actor	= Actor;
	Sources(Index).Priority	= Priority;
	Sources(Index).Location	= Location;
	Sources(Index).Radius		= Radius ? Radius : 10;
	Sources(Index).LastChange	= appSeconds();
	Sources(Index).FadeDuration	= FadeDuration;
	Sources(Index).FadeTime		= 0.f;
	Sources(Index).FadeMode		= FadeDuration > 0.f ? FADE_In : FADE_None;
	Sources(Index).Volume	= (Flags & SF_Music) ? MusicVolume : Volume;
	Sources(Index).Started	= 1;
	Sources(Index).Paused	= 0;
	
	DSBUFFERDESC bd;
	appMemzero( &bd, sizeof(DSBUFFERDESC) );
	bd.dwSize		 = 0;
	bd.dwFlags		 = (Flags & SF_No3D) ? 0 : DSBCAPS_CTRL3D;
	bd.dwBufferBytes = 0;//DSBSIZE_MIN;
	bd.lpwfxFormat	 = &Buffers( Sound->GetHandle()-1 ).wfx; // gam

	HRESULT Result;
	if( FAILED(Result=DirectSound8->CreateSoundBuffer( &bd, &Sources(Index).DSBuffer, NULL ) ) )
	{
		// If we run out of voices release stopped ones till there is a free one of the right kind.
		while( StoppedBuffers.Num() )
		{
			StoppedBuffers(0)->Release();
			StoppedBuffers.Remove(1);
			if( !FAILED(Result=DirectSound8->CreateSoundBuffer( &bd, &Sources(Index).DSBuffer, NULL ) ) )
				break;
		}
		if( FAILED(Result) )
			XBoxSoundError(Result);
	}
	LPDIRECTSOUNDBUFFER DSBuffer = Sources(Index).DSBuffer;

	check(Sources(Index).Sound->GetData().Num()>0); // gam
	if (FAILED(Result=DSBuffer->SetBufferData( Buffers( Sound->GetHandle()-1 ).Data, Buffers( Sound->GetHandle()-1 ).Size ) )) // gam
		XBoxSoundError(Result);

	if( !(Flags & SF_No3D) )
	{
		DSBuffer->SetMinDistance( Max( 0.001f, Sources(Index).UsedRadius ), COMMIT_MODE ); // sjs - cannot pass 0.0!
		DSBuffer->SetMaxDistance( DS3D_DEFAULTMAXDISTANCE, COMMIT_MODE );
		DSBuffer->SetPosition( Location.X, Location.Y, Location.Z, COMMIT_MODE );
		DirectSound8->SetVelocity( 0, 0, 0, COMMIT_MODE );
	}

	DSBuffer->SetVolume( log10(Sources(Index).FadeMode == FADE_In ? 0.f : Sources(Index).Volume) * 1000 );
	DSBuffer->SetFrequency( Pitch * Buffers(Sound->GetHandle()-1).Rate ); // sjs
	DSBuffer->Play( 0, 0, (Sources(Index).Flags & SF_Looping) ? DSBPLAY_LOOPING : 0 );

#if USE_DEFERRED
	DirectSound8->CommitDeferredSettings();
#endif
	//debugf(TEXT("playing sound in source %i with priority %f"), Index, Priority);

	unclock(GStats.DWORDStats(XBoxAudioStats.STATS_PlaySoundCycles));
	return 1;

	unguard;
}


UBOOL UXBoxAudioSubsystem::StopSound( AActor* Actor, USound* Sound )
{
	UBOOL Stopped = false;

	// Stop all sounds.
	if ( !Actor && !Sound )
	{
		for (INT i=0; i<Sources.Num(); i++)	
		{
		    // gam ---
		    if( Sources(i).Flags & SF_Music )
		        continue;
		    // --- gam
			StopSound(i);
		}
		return true;
	}

	// Stop selected sound.
	for (INT i=0; i<Sources.Num(); i++)	
	{
		if ( Sound )
		{
			if ( Actor )
			{
				if ( (Actor == Sources(i).Actor) && (Sound == Sources(i).Sound) )
				{
					StopSound(i);
					Stopped = true;
				}
			}
			else
			{
				if ( Sound == Sources(i).Sound )
				{
					StopSound(i);
					Stopped = true;
				}
			}
		}
		else if ( Actor )
		{
			if ( Actor == Sources(i).Actor )
			{
				StopSound(i);
				Stopped = true;
			}
		}
	}
	return Stopped;
}


void UXBoxAudioSubsystem::NoteDestroy(AActor* Actor)
{
	guard(UXBoxAudioSubsystem::NoteDestroy);
	check(Actor);
	check(Actor->IsValid());

	// Stop the actor's ambient sound, and dereference owned sounds.
	for(INT i=0; i<Sources.Num(); i++)
	{
		if(Sources(i).Actor == Actor)
		{
			if((Sources(i).Id & 14) == SLOT_Ambient * 2)
				StopSound(i);
			else 
				Sources(i).Actor = NULL; // Non interruptable sound.
		}
	}

	unguard;
}


static DWORD WINAPI StreamThreadProc( LPVOID lpParameter )
{
	XBStream* Stream = (XBStream*) lpParameter;
    
	do
	{
		// Find a free packet.
		INT PacketIndex = 0;
		for( PacketIndex = 0; PacketIndex<BUFFERS_PER_STREAM; PacketIndex++ )
			if( Stream->PacketStatus[PacketIndex] != XMEDIAPACKET_STATUS_PENDING )
				break;
			
		if( PacketIndex != BUFFERS_PER_STREAM )
		{ 
			// Setup packet.
			XMEDIAPACKET Packet;
			appMemzero( &Packet, sizeof(Packet) );
		    DWORD BytesDecoded;

			Packet.pvBuffer			= Stream->SampleData + PacketIndex * STREAM_CHUNKSIZE;
			Packet.dwMaxSize		= STREAM_CHUNKSIZE;
			Packet.pdwCompletedSize	= &BytesDecoded;

			// Get packets from decoder.
			Stream->Decoder->Process( NULL, &Packet );

			// Check for EOF.
		    if( 0 != BytesDecoded )
			{
				// Feed DirectSound stream with more packets.
				Packet.pdwStatus		= (DWORD*) &Stream->PacketStatus[ PacketIndex ];
				Packet.pdwCompletedSize = NULL;
				Packet.pContext			= (LPVOID) Stream->DSStream;
			
				Stream->DSStream->Process( &Packet, NULL );
			}
			else
			{
				//!! Seek asserts so loop manually.
				Stream->Decoder->Discontinuity();
				Stream->Decoder->Flush();
				Stream->Status |= STREAM_ShouldLoop;
				Stream->Status &= ~STREAM_KeepPlaying;
			}
		}

		// Don't eat too many CPU cycles.
		Sleep( 5 );
	} 
	while( Stream->Status & STREAM_KeepPlaying );

	// Mark stream killed.
	Stream->Status |= STREAM_Terminated;

	return 0;
}


INT UXBoxAudioSubsystem::PlayMusic( FString Song, FLOAT FadeInTime )
{
	guard(UXBoxAudioSubsystem::PlayMusic);

	FString Filename = FString(TEXT("d:\\music\\")) + Song;
	if( Song != TEXT("") && (GFileManager->FileSize(*Filename) > 0) )
	{
		// Initialize Stream.
		INT StreamIndex		= GetNewStream();
		XBStream *Stream	= &Streams(StreamIndex);

		Stream->Id			= StreamIndex+1;
		Stream->SampleData	= new BYTE[ BUFFERS_PER_STREAM * STREAM_CHUNKSIZE ];
		Stream->Status		= STREAM_KeepPlaying;
		Stream->Name		= Song;

		// Create WMA decoder.
		WAVEFORMATEX Format;			
		if( FAILED(WmaCreateDecoder( (LPCSTR) TCHAR_TO_ANSI(*Filename), NULL, TRUE, STREAM_CHUNKSIZE, BUFFERS_PER_STREAM, 3, &Format, &Stream->Decoder ) ))
			appErrorf( TEXT("Couldn't create decoder.") );

		// Create DirectSound stream.
		DSSTREAMDESC SD = {0};
		SD.dwMaxAttachedPackets	= BUFFERS_PER_STREAM;
		SD.lpwfxFormat			= &Format;

		if( FAILED( DirectSoundCreateStream( &SD, &Stream->DSStream ) ))
			appErrorf( TEXT("Couldn't create stream.") );

		// Initialize packet status.
		for( INT PacketIndex=0; PacketIndex<BUFFERS_PER_STREAM; PacketIndex++ )
			Stream->PacketStatus[PacketIndex] = XMEDIAPACKET_STATUS_SUCCESS;

		// Create worker thread.
		Stream->Thread = CreateThread( NULL, 12 * 1024, StreamThreadProc, Stream, 0, NULL );
		check(Stream->Thread);

		return StreamIndex+1;
	}
	else
		return 0;

	unguard;
}


UBOOL UXBoxAudioSubsystem::StopMusic( INT SongHandle, FLOAT FadeOutTime )
{
	guard(UXBoxAudioSubsystem::StopMusic);

	// Stop music.
	SongHandle--;
	if( (SongHandle < Streams.Num()) && (SongHandle >= 0) )
	{
		Streams(SongHandle).Status &= ~STREAM_KeepPlaying;
		return 1;
	}
	else
		return 0;
	unguard;
}


INT UXBoxAudioSubsystem::StopAllMusic( FLOAT FadeOutTime )
{
	guard(UXBoxAudioSubsystem::StopAllMusic);

	// Stop all music.
	for (INT i=0; i<Streams.Num(); i++)
		Streams(i).Status &= ~STREAM_KeepPlaying;
	return 0;

	unguard;
}

static INT Compare(XBAmbient& A,XBAmbient& B)
{
	return (B.Priority - A.Priority >= 0) ? 1 : -1;
}
void UXBoxAudioSubsystem::Update( FSceneNode* SceneNode )
{
	guard(UXBoxAudioSubsystem::Update);

	if(!Viewport)
		return;

	clock(GStats.DWORDStats(XBoxAudioStats.STATS_UpdateCycles));

	// Required since June XDK
	DirectSoundDoWork();

	// Projection/ Orientation.
	FVector ViewLocation = SceneNode->ViewOrigin;
	FVector ProjUp		 = SceneNode->CameraToWorld.TransformNormal(FVector(0,1000,0));
	FVector ProjRight	 = SceneNode->CameraToWorld.TransformNormal(FVector(ReverseStereo ? -1000 : 1000,0,0));
	FVector ProjFront	 = ProjRight ^ ProjUp;

	ProjUp.Normalize();
	ProjRight.Normalize();
	ProjFront.Normalize();

	// Find out which zone the listener is in.
	ULevel* Level = Viewport->Actor->GetViewTarget()->GetLevel();
	RegionListener = Level->Model->PointRegion( Viewport->Actor->ViewTarget->Level, ViewLocation );

	// Time passes...
	DOUBLE CurrentTime	= appSeconds();
	FLOAT  DeltaTime	= CurrentTime - LastTime;
	LastTime  = CurrentTime;
	DeltaTime = Clamp(DeltaTime,0.0001f,1.0f);
	
	UBOOL Realtime = Viewport->IsRealtime() && !Level->IsPaused();

	// Stop all sounds if transitioning out of realtime.
	if( !Realtime && LastRealtime )
		StopSound( NULL, NULL );

	LastRealtime = Realtime;

	// Check for finished sounds
	for (INT i=0; i<Sources.Num(); i++)
	{
		if(	Sources(i).Id && Sources(i).Sound && !(Sources(i).Flags & SF_Looping) )
		{
			DWORD Status;
			Sources(i).DSBuffer->GetStatus( &Status );
		
			if ( !(Status & DSBSTATUS_PLAYING) )
			{
				if ( Sources(i).Flags & SF_Streaming )
					appErrorf(TEXT("SF_Streaming"));
				StopSound(i);
			}
		}
	}

	// See if any new ambient sounds need to be started.
	guard(HandleAmbience);
	if( Realtime )
	{
		TArray<XBAmbient> AmbientSounds;

		for(INT i=0; i<Level->Actors.Num(); i++)
		{
			AActor* Actor = Level->Actors(i);
			if ( Actor && Actor->AmbientSound )
			{
				INT Id = Actor->GetIndex()*16 + SLOT_Ambient*2;
				INT j;
				for( j=0; j<Sources.Num(); j++ )
					if( Sources(j).Id==Id )
						break;
				if( j==Sources.Num() )
				{
					if( Actor->IsOwnedBy( Viewport->Actor->ViewTarget ) )
					{
						INT iAmbient = AmbientSounds.Add( 1 );
						AmbientSounds(iAmbient).Actor		= Actor;
						AmbientSounds(iAmbient).Priority	= SoundPriority( Viewport, Actor->Location, Actor->SoundVolume/255.f, Actor->SoundRadius, SF_Looping | SF_No3D | SF_UpdatePitch );
						AmbientSounds(iAmbient).Flags		= SF_Looping | SF_No3D | SF_UpdatePitch;
						AmbientSounds(iAmbient).Id			= Id;
					}
					else
					{
						// Sound occclusion.
						clock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));
						FLOAT RM = 1.f;
						if( !Level->IsAudibleAt( Actor->Location, ViewLocation, Actor, (ESoundOcclusion) Actor->SoundOcclusion ) )
						{
							FPointRegion RegionSource	= Level->Model->PointRegion( Actor->Level, Actor->Location );
							RM *= Level->CalculateRadiusMultiplier( RegionSource.ZoneNumber, RegionListener.ZoneNumber );
						}
						unclock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));

						if (FDistSquared(ViewLocation,Actor->Location)<=Square(RM*Actor->SoundRadius*GAudioMaxRadiusMultiplier) )
						{
							INT iAmbient = AmbientSounds.Add( 1 );
							AmbientSounds(iAmbient).Actor		= Actor;
							AmbientSounds(iAmbient).Priority	= SoundPriority( Viewport, Actor->Location, Actor->SoundVolume/255.f, RM*Actor->SoundRadius, SF_Looping | SF_UpdatePitch );
							AmbientSounds(iAmbient).Flags		= SF_Looping | SF_UpdatePitch;
							AmbientSounds(iAmbient).Id			= Id;
						}
					}
				}
			}
		}

		// Sort ambient sounds by priority to avoid thrashing of channels.
		if( AmbientSounds.Num() )
		{
			Sort( &AmbientSounds(0), AmbientSounds.Num() );
			for( INT iAmbient = 0; iAmbient < AmbientSounds.Num(); iAmbient++ )
			{
				AActor* &Actor = AmbientSounds(iAmbient).Actor;
				// Early out if sound couldn't be played because of priority.
				if( !PlaySound( Actor, AmbientSounds(iAmbient).Id, Actor->AmbientSound, Actor->Location, Actor->SoundVolume/255.f, Actor->SoundRadius, Actor->SoundPitch/64.f, AmbientSounds(iAmbient).Flags, 0.f, AmbientSounds(iAmbient).Priority ) )
					break;
			}
		}
	}
	unguard;

	// Update all playing ambient sounds.
	guard(UpdateAmbience);
	for(INT i=0; i<Sources.Num(); i++)
	{
		if((Sources(i).Id&14) == SLOT_Ambient*2)
		{
			if( Sources(i).Actor )
			{
				FLOAT NewDist = FDistSquared(ViewLocation,Sources(i).Actor->Location);				
				if( 
					(NewDist > Square(Sources(i).ZoneRadius*GAudioMaxRadiusMultiplier)) || 
					(Sources(i).Actor->AmbientSound != Sources(i).Sound)
				)
				{
					StopSound(i);
				}
#if 0 //!!vogel
				else
				{
					// Update radius.
					Sources(i).Radius = Sources(i).Actor->SoundRadius;
					Sources(i).DSBuffer->SetMinDistance( Sources(i).Radius, COMMIT_MODE );
					Sources(i).DSBuffer->SetMaxDistance( Sources(i).Radius * GAudioMaxRadiusMultiplier , COMMIT_MODE );
				}
#endif
			}
		}
	}
	unguard;

	// Update all streams.
	for(INT i=0; i<Streams.Num(); i++)
	{
		// Clean up terminated streams.
		if( Streams(i).Status & STREAM_Terminated )
		{
			// Release resources and clean up.
			XBStream* Stream = &Streams(i);
			Stream->Decoder->Release();
			Stream->Decoder = NULL;

			Stream->DSStream->Pause( DSSTREAMPAUSE_RESUME );
			Stream->DSStream->Release();
			Stream->DSStream = NULL;

			delete [] Stream->SampleData;

			// Free thread handle.
			WaitForSingleObject( Stream->Thread, INFINITE );
			CloseHandle( Stream->Thread );

			if( Stream->Status & STREAM_ShouldLoop )
				PlayMusic( Stream->Name, 0.f );

			appMemzero( Stream, sizeof(XBStream) );
		}
	}

	// Update all active sounds.
	guard(UpdateSounds);
	for(INT i=0; i<Sources.Num(); i++)
	{
		if(Sources(i).Actor)
			check(Sources(i).Actor->IsValid());

		if(Sources(i).Id != 0 && !Sources(i).Paused)
		{
			// Manage streaming sounds.
			if (Sources(i).Flags & SF_Streaming)
			{
				appErrorf(TEXT("Streaming"));
				GStats.DWORDStats(XBoxAudioStats.STATS_ActiveStreamingSounds)++;
			}
			else
				GStats.DWORDStats(XBoxAudioStats.STATS_ActiveRegularSounds)++;


			// Update position, velocity, pitch and volume from actor (if wanted)
			FVector Velocity = FVector(0,0,0);
			FLOAT Volume = Sources(i).Volume;
			if( Sources(i).Actor && !(Sources(i).Flags & SF_NoUpdates) )
			{
				// Set location.
				Sources(i).Location = Sources(i).Actor->Location;
				if (!(Sources(i).Flags & SF_No3D))
					Velocity = Sources(i).Actor->Velocity;

				// Set pitch.
				if ( Sources(i).Flags & SF_UpdatePitch )
				{
				FLOAT Frequency = Clamp<FLOAT>(Sources(i).Actor->SoundPitch/64.f,0.5f,2.0f) * Buffers(Sources(i).Sound->GetHandle()-1).Rate; // gam
				Sources(i).DSBuffer->SetFrequency( Frequency );
				}

				// Set Volume.
				if( (Sources(i).Id&14) == SLOT_Ambient*2 )
				{
					Volume = Sources(i).Actor->SoundVolume / 255.f;
					Sources(i).Volume = Volume;
				}
				if( Sources(i).Actor->LightType!=LT_None )
					Volume *= Sources(i).Actor->LightBrightness / 255.0;
			}

			if( Sources(i).FadeMode == FADE_In )
			{
				// Disregard initial loading time.
				if( DeltaTime < 1.f )
					Sources(i).FadeTime += DeltaTime;
				if( Sources(i).FadeTime >= Sources(i).FadeDuration )
					Sources(i).FadeMode = FADE_None;
				else
					Volume *= Sources(i).FadeTime / Sources(i).FadeDuration;
				}
				
			if( Sources(i).FadeMode == FADE_Out )
			{
				// Disregard initial loading time.
				if( DeltaTime < 1.f )
					Sources(i).FadeTime += DeltaTime;
				if( Sources(i).FadeTime >= Sources(i).FadeDuration )
				{
					// Stop sound.
					if( Sources(i).Flags & SF_Streaming )
						UnregisterSound(Sources(i).Sound);
					else
						StopSound(i);
					continue;
				}
				else
					Volume *= (1.f - Sources(i).FadeTime / Sources(i).FadeDuration);
                }
				
				Sources(i).DSBuffer->SetVolume( log10(Volume) * 1000 );

			if ( !(Sources(i).Flags & SF_No3D) )
			{
				// Sound occclusion.
				clock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));
				FLOAT Radius = Sources(i).Radius;
				if( !Level->IsAudibleAt(	
						Sources(i).Location, 
						ViewLocation, 
						Sources(i).Actor, 
						Sources(i).Actor ? (ESoundOcclusion) Sources(i).Actor->SoundOcclusion : OCCLUSION_Default ) 
				)
				{					
					FPointRegion RegionSource = Level->Model->PointRegion( Viewport->Actor->Level, Sources(i).Location );
					Radius *= Level->CalculateRadiusMultiplier( RegionSource.ZoneNumber, RegionListener.ZoneNumber );
					Sources(i).ZoneRadius = Radius;
					Radius *= OCCLUSION_FACTOR;
					GStats.DWORDStats(XBoxAudioStats.STATS_OccludedSounds)++;
				}
				else
					Sources(i).ZoneRadius = Radius;				
				unclock(GStats.DWORDStats(XBoxAudioStats.STATS_OcclusionCycles));


				// Smooth transition between radii.
				if( Sources(i).WantedRadius != Radius )
				{
					Sources(i).LastChange	= CurrentTime;
					Sources(i).WantedRadius = Radius;
				}

				if( (CurrentTime - Sources(i).LastChange) < 1.f )
					Radius = Lerp( Sources(i).UsedRadius, Radius, CurrentTime - Sources(i).LastChange );
				else
					Sources(i).UsedRadius = Radius;
				
				// Set Radius.
				Sources(i).DSBuffer->SetMinDistance( Clamp( Radius, 0.001f, 100000.0f ), COMMIT_MODE );
			}

			// Play at listener location if no 3D effects are wanted.
			if (Sources(i).Flags & SF_No3D)
				Sources(i).Location = ViewLocation;

			// Update the priority.
			Sources(i).Priority = SoundPriority(
				Viewport,
				Sources(i).Location,
				Sources(i).Volume,
				Sources(i).ZoneRadius,
				Sources(i).Flags
			);
			
			// Set position & velocity.
			if( !(Sources(i).Flags & SF_No3D) )
			{
			Sources(i).DSBuffer->SetPosition( Sources(i).Location.X, Sources(i).Location.Y, Sources(i).Location.Z, COMMIT_MODE );
			if( Velocity.Size() < MAX_SOURCE_VELOCITY )
				Sources(i).DSBuffer->SetVelocity( Velocity.X, Velocity.Y, Velocity.X, COMMIT_MODE );
			}
			//!! TODO: Level->TimeDilation
		}
	}

	// Set Player position, orientation and velocity.
	FVector Up			= ProjUp;		//!! might be -ProjUp
	FVector Front		= ProjFront;
	FVector Velocity	= (ViewLocation - LastPosition) / DeltaTime;
	LastPosition		= ViewLocation;

	DirectSound8->SetPosition( ViewLocation.X, ViewLocation.Y, ViewLocation.Z, COMMIT_MODE );
	DirectSound8->SetOrientation( Front.X, Front.Y, Front.Z, Up.X, Up.Y, Up.Z, COMMIT_MODE );
	if( Velocity.Size() < MAX_LISTENER_VELOCITY )	
		DirectSound8->SetVelocity( Velocity.X, Velocity.Y, Velocity.Z, COMMIT_MODE );

	// Set I3DL2 listener zone effect.
	SetI3DL2Listener( RegionListener.Zone->ZoneEffect );

#if USE_DEFERRED
	DirectSound8->CommitDeferredSettings();
#endif

	// Deferred buffer deletion (workaround for HW stalls)
	for( INT iBuffer=0; iBuffer<StoppedBuffers.Num(); iBuffer++ )
	{
		DWORD Status;
		StoppedBuffers(iBuffer)->GetStatus( &Status );

		// Release if no longer playing.
		if( (Status == 0) )
		{
			check(StoppedBuffers(iBuffer)->Release() == 0);
			StoppedBuffers.Remove(iBuffer);
		}
	}

#if USE_VOICE_CHAT
	// Update Voice Communicators.
	UpdateVoiceChat( DeltaTime );
#endif

	unclock(GStats.DWORDStats(XBoxAudioStats.STATS_UpdateCycles));

	unguard;
	unguard;
}


UBOOL UXBoxAudioSubsystem::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(UXBoxAudioSubsystem::Exec);
	const TCHAR*	Str = Cmd;
	if(ParseCommand(&Str,TEXT("ASTAT")))
	{
		if(ParseCommand(&Str,TEXT("AUDIO")))
		{
			AudioStats ^= 1;
			return 1;
		}
	}
	else if(ParseCommand(&Str,TEXT("PAUSESOUNDS")))
	{
		for(INT i=0; i<Sources.Num(); i++)
		{
			if(Sources(i).Id)
			{
				Sources(i).Paused = 1;
				Sources(i).DSBuffer->Stop();
			}
		}
		return 1;
	}
	else if(ParseCommand(&Str,TEXT("UNPAUSESOUNDS")))
	{
		for(INT i=0; i<Sources.Num(); i++)
		{
			if(Sources(i).Id)
			{
				Sources(i).Paused = 0;
				Sources(i).DSBuffer->Play(0, 0, (Sources(i).Flags & SF_Looping) ? DSBPLAY_LOOPING : 0 );
			}
		}
		return 1;
	}
    else if(ParseCommand(&Str,TEXT("E3-VOICEMASK"))) // sjs
    {
        // Update the voice masks.
        debugf(TEXT("E3-VOICEMASK called."));
        if( VoiceManager )
            VoiceManager->UpdateVoiceMasks();
        return 1;
    }
	return 0;	
	unguard;
}


/*------------------------------------------------------------------------------------
	Global Pause/Resume music functions, needed for sync with StaticLoadObject.
------------------------------------------------------------------------------------*/

void appPauseMusic()
{
	guard(UXBoxAudioSubsystem::PauseMusic);
	unguard;
}

void appResumeMusic()
{
	guard(UXBoxAudioSubsystem::ResumeMusic);
	unguard;
}

/*------------------------------------------------------------------------------------
	Internals.
------------------------------------------------------------------------------------*/


void UXBoxAudioSubsystem::SetVolumes()
{
	guard(UXBoxAudioSubsystem::SetVolumes);
	// Update the music volume.
	unguard;
}

void UXBoxAudioSubsystem::StopSound( INT i )
{
	guard(UXBoxAudioSubsystem::StopSound);
 	if(Sources(i).Id != 0)
	{
		GStats.DWORDStats(XBoxAudioStats.STATS_StoppedSounds)++;

		if ( Sources(i).Flags & SF_Streaming )
			appErrorf(TEXT("SF_Streaming"));
		
		Sources(i).DSBuffer->Stop();
#if 0
		// Defer release.
		StoppedBuffers.AddItem(Sources(i).DSBuffer);
#else
		Sources(i).DSBuffer->Release();
#endif
		Sources(i).DSBuffer = NULL;

		appMemzero( &Sources(i), sizeof( XBSource ) );
	}
	unguard;
}


FLOAT UXBoxAudioSubsystem::SoundPriority( UViewport* Viewport, FVector Location, FLOAT Volume, FLOAT Radius, INT Flags )
{
	guard(UXBoxAudioSubsystem::SoundPriority);
	FLOAT RadiusFactor;
	if ( Radius )
	{
		RadiusFactor = 1 - FDistSquared(Location, Viewport->Actor->GetViewTarget()->Location) / Square(GAudioMaxRadiusMultiplier*Radius);
	}
	else
		RadiusFactor = 1;
	RadiusFactor = Clamp(RadiusFactor, 0.01f, 1.f);

	return Volume * RadiusFactor + ((Flags & SF_Streaming)? 1 : 0) + ((Flags & SF_Music)? 2 : 0) + ((Flags & SF_No3D)? 1 : 0);
	unguard;
}


INT UXBoxAudioSubsystem::GetNewStream()
{
	guard(UXBoxAudioSubsystem::GetNewStream);
	for( INT i=0; i< Streams.Num(); i++ )
	{
		if( Streams(i).Id == 0 )
		{
			appMemzero( &Streams(i), sizeof(XBStream) );
			return i;
		}
	}
	appErrorf(TEXT("More than %i streams in use"), MAX_AUDIOSTREAMS );
	return -1;
	unguard;
}


void UXBoxAudioSubsystem::SetI3DL2Listener( UI3DL2Listener* Listener )
{
	guard(SetI3DL2Listener);

	// Check whether update is necessary.
	if( (OldListener == Listener) && (!Listener || !Listener->Updated) )
		return;

	OldListener	= Listener;

	// I3DL2 Properties.
	_DSI3DL2LISTENER DSProperties;
	
	if( !Listener )
	{
		// Set default properties.
		DSProperties.lRoom						= -10000;
		DSProperties.lRoomHF					= -10000;
		DSProperties.flRoomRolloffFactor		= 0.f;
		DSProperties.flDecayTime				= 0.1f;
		DSProperties.flDecayHFRatio				= 0.5f;
		DSProperties.lReflections				= -10000;
		DSProperties.flReflectionsDelay			= 0.0f;
		DSProperties.lReverb					= -10000;
		DSProperties.flReverbDelay				= 0.0f;
		DSProperties.flDiffusion				= 0.f;
		DSProperties.flDensity					= 0.f;
		DSProperties.flHFReference				= 5000.0f;
	}
	else
	{
		// Set zone properties.
		DSProperties.lRoom						= Listener->Room;
		DSProperties.lRoomHF					= Listener->RoomHF;
		DSProperties.flRoomRolloffFactor		= Listener->RoomRolloffFactor;
		DSProperties.flDecayTime				= Listener->DecayTime;
		DSProperties.flDecayHFRatio				= Listener->DecayHFRatio;
		DSProperties.lReflections				= Listener->Reflections;
		DSProperties.flReflectionsDelay			= Listener->ReflectionsDelay;
		DSProperties.lReverb					= Listener->Reverb;
		DSProperties.flReverbDelay				= Listener->ReverbDelay;
		DSProperties.flDiffusion				= Listener->EnvironmentDiffusion;
		DSProperties.flDensity					= 100.f;
		DSProperties.flHFReference				= 5000.0f;

		// Change has been commited.
		Listener->Updated = false;
	}

	DirectSound8->SetI3DL2Listener( &DSProperties, COMMIT_MODE );

	unguard;
}


#if USE_VOICE_CHAT
void UXBoxAudioSubsystem::UpdateVoiceChat( FLOAT DeltaTime )
{
	guard(UXBoxAudioSubsystem::UpdateCommunicators);

	// Flush queues to avoid lag.
	if( DeltaTime > 0.1f )
		VoiceManager->FlushQueues();

	// Check for incoming packets.
	WORD		MessageData[ COMPRESSED_PACKET_SIZE / 2 + 1 ];
	u_long		NumBytes;
	SOCKADDR_IN SockAddrFromIn;
	INT			Size = sizeof( SOCKADDR_IN );

	ioctlsocket( VoiceSocket, FIONREAD, &NumBytes );
	while( NumBytes >= (COMPRESSED_PACKET_SIZE+2) )
	{
		INT Bytes = recvfrom(
							VoiceSocket,
							(char*) MessageData,
							sizeof(MessageData),
							0,
							(sockaddr*)(&SockAddrFromIn),
							&Size
					);

		if( Bytes == (COMPRESSED_PACKET_SIZE+2) )
		{
			VoiceManager->ReceivePacket( 
							SockAddrFromIn.sin_addr.s_addr, 
							MessageData[0], 
							&MessageData[1], 
							COMPRESSED_PACKET_SIZE 
						);

			ioctlsocket( VoiceSocket, FIONREAD, &NumBytes );	
		}
		else 
			NumBytes = -1;
	}

	// Tick voice code.
	if( VoiceManager )
		VoiceManager->ProcessVoice();

	unguard;
}


void UXBoxAudioSubsystem::ChangeVoiceChatter( DWORD IpAddr, DWORD ControllerPort, UBOOL Add )
{
	guard(UXBoxAudioSubsystem::AddChatter);

	debugf(TEXT("ChangeVoiceChatter IpAddr=%i Port=%i Add=%i LocalIp=%i"),IpAddr,ControllerPort,Add,LocalIpAddress);
	if( VoiceManager && (LocalIpAddress != IpAddr) )
	{
		if( Add )
		{
			// Add Chatter.
			VoiceManager->AddChatter( IpAddr, ControllerPort );
			
			// Add IpAddr to recipents list.
			INT i;
			for( i=0; i<VoiceRecipents.Num(); i++ )
			{
				if( VoiceRecipents(i).IpAddr == IpAddr )
				{
					VoiceRecipents(i).RefCount++;
					break;
				}
			}
			if( i == VoiceRecipents.Num() )
			{
				VoiceRecipents.AddZeroed(1);
				VoiceRecipents(i).IpAddr	= IpAddr;
				VoiceRecipents(i).RefCount	= 1;
			}
		}
		else
		{
			// Remove Chatter.
			VoiceManager->RemoveChatter( IpAddr, ControllerPort );

			// Remove IpAddr from recipents list.
			INT i;
			for( i=0; i<VoiceRecipents.Num(); i++ )
			{
				if( VoiceRecipents(i).IpAddr == IpAddr )
				{
					VoiceRecipents(i).RefCount--;
					break;
				}
			}
			if( i < VoiceRecipents.Num() && !VoiceRecipents(i).RefCount )
			{
				VoiceRecipents.Remove(i,1);
			}
		}
		VoiceManager->FlushQueues();
	}
	unguard;
}

void UXBoxAudioSubsystem::EnterVoiceChat()
{
	guard(UXBoxAudioSubsystem::EnterVoiceChat);

	debugf(TEXT("EnterVoiceChat"));
	check(Viewport);

	// Initialize voice manager.
	if( VoiceManager )
		VoiceManager->EnterChatSession();

	// Get remote chatters.
	Viewport->Actor->eventServerGetVoiceChatters( Viewport->Actor );

	// Send initial state.
	for( INT i=0; i<4; i++ )
	{
		if( VoiceManager && VoiceManager->IsCommunicatorInserted(i) )
		{
			Viewport->Actor->eventServerChangeVoiceChatter( Viewport->Actor, LocalIpAddress, i, 1 );
		}
	}

	unguard;
}

void UXBoxAudioSubsystem::LeaveVoiceChat()
{
	guard(UXBoxAudioSubsystem::LeaveVoiceChat);

	debugf(TEXT("LeaveVoiceChat"));
	check(Viewport);

	// Remove chatters from server.
	for( INT i=0; i<4; i++ )
	{
		if( VoiceManager && VoiceManager->IsCommunicatorInserted(i) )
			Viewport->Actor->eventServerChangeVoiceChatter( Viewport->Actor, LocalIpAddress, i, 0 );
	}

	// Tear down voice chat.
	VoiceRecipents.Empty();
	if( VoiceManager )
		VoiceManager->LeaveChatSession();

	unguard;
}


void UXBoxAudioSubsystem::SendVoiceData( void* Data, WORD ControllerPort )
{
	guard(UXBoxAudioSubsystem::SendVoiceData);

	// Broadcast voice data to all recipents.
	WORD MessageData[ COMPRESSED_PACKET_SIZE / 2 + 1 ];
	MessageData[0] = ControllerPort;
	appMemcpy( &MessageData[1], Data, COMPRESSED_PACKET_SIZE );

	sockaddr_in SockAddrIn;
	SockAddrIn.sin_family	= AF_INET;
	SockAddrIn.sin_port	= htons(VOICE_PORT);

	for( INT i=0; i<VoiceRecipents.Num(); i++ )
	{
		SockAddrIn.sin_addr	= *((in_addr*) &VoiceRecipents(i).IpAddr);

		INT Bytes = sendto( 
							VoiceSocket, 
							(char*) MessageData, 
							sizeof(MessageData), 
							0, 
							(const sockaddr*)(&SockAddrIn), 
							sizeof( SOCKADDR_IN ) 
					);
		check( Bytes == sizeof(MessageData) );
	}

	unguard;
}
#endif

UXBoxAudioSubsystem::FXBoxAudioStats::FXBoxAudioStats()
{
	guard(FXBoxAudioStats::FXBoxAudioStats)
	appMemset( &STATS_FirstEntry, 0xFF, (DWORD) &STATS_LastEntry - (DWORD) &STATS_FirstEntry );
	unguard;
}


void UXBoxAudioSubsystem::FXBoxAudioStats::Init()
{
	guard(FXBoxAudioStats::Init);

	// If already initialized retrieve indices from GStats.
	if( GStats.Registered[STATSTYPE_Audio] )
	{
		INT* Dummy = &STATS_PlaySoundCycles;
		for( INT i=0; i<GStats.Stats[STATSTYPE_Audio].Num(); i++ )
			*(Dummy++) = GStats.Stats[STATSTYPE_Audio](i).Index;
		return;
	}

	// Register stats with GStat.
	STATS_PlaySoundCalls				= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("PlaySound"		), TEXT("Audio"		), STATSUNIT_Combined_Default_MSec	);
	STATS_PlaySoundCycles				= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("PlaySound"		), TEXT("Audio"		), STATSUNIT_MSec					);
	STATS_OccludedSounds				= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("Occlusion"		), TEXT("Audio"		), STATSUNIT_Combined_Default_MSec	);
	STATS_OcclusionCycles				= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("Occlusion"		), TEXT("Audio"		), STATSUNIT_MSec					);
	STATS_UpdateCycles					= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("Update"			), TEXT("Audio"		), STATSUNIT_MSec					);
	STATS_ActiveStreamingSounds			= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("Streaming"		), TEXT("Audio"		), STATSUNIT_Default				);
	STATS_ActiveRegularSounds			= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("Regular"		), TEXT("Audio"		), STATSUNIT_Default				);
	STATS_StoppedSounds					= GStats.RegisterStats( STATSTYPE_Audio, STATSDATATYPE_DWORD, TEXT("StoppedSounds"	), TEXT("Audio"		), STATSUNIT_Default				);
	
	// Initialized.
	GStats.Registered[STATSTYPE_Audio] = 1;

	unguard;
}


void XBoxSoundError( HRESULT hr )
{
	switch (hr)
	{
	case DS_OK:
		break;
	case DSERR_CONTROLUNAVAIL:
		debugf(TEXT("AUDIO: DSERR_CONTROLUNAVAIL"));
		break;
	case DSERR_INVALIDCALL:
		debugf(TEXT("AUDIO: DSERR_INVALIDCALL"));
		break;
	case DSERR_GENERIC:
		debugf(TEXT("AUDIO: DSERR_GENERIC"));
		break;
	case DSERR_OUTOFMEMORY:
		debugf(TEXT("AUDIO: DSERR_OUTOFMEMORY"));
		break;
	case DSERR_UNSUPPORTED:
		debugf(TEXT("AUDIO: DSERR_UNSUPPORTED"));
		break;
	case DSERR_NODRIVER:
		debugf(TEXT("AUDIO: DSERR_NODRIVER"));
		break;
	case DSERR_NOAGGREGATION:
		debugf(TEXT("AUDIO: DSERR_NOAGGREGATION"));
		break;
	default:
		debugf(TEXT("AUDIO: UNKNOWN ERROR"));
	}
}


/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


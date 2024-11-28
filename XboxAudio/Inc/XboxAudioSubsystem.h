/*=============================================================================
	XBoxAudioSubsystem.h: Unreal XBox Audio interface object.
	Copyright 1999-2001 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Daniel Vogel.
=============================================================================*/

#ifndef _INC_XBOXAUDIOSUBSYSTEM
#define _INC_XBOXAUDIOSUBSYSTEM

#include "XBoxAudioVoice.h"

/*------------------------------------------------------------------------------------
	Helpers
------------------------------------------------------------------------------------*/

extern void XBoxSoundError( HRESULT hr );

// Constants.

// Maximum number of audio channels.
#define MAX_AUDIOCHANNELS 32

// Maximum number of audio streams.
#define MAX_AUDIOSTREAMS 4

// Maximum listener velocity in meters/ second (the part before the *).
#define MAX_LISTENER_VELOCITY (50 * (2.f / 0.0254f))

// Maximum source velocity in meters/ seconds (the part before the *).
#define MAX_SOURCE_VELOCITY (300 * (2.f / 0.0254f))

// Amount of OpenAL buffers per stream.
#define BUFFERS_PER_STREAM 4

// Size in bytes per buffer in stream.
#define STREAM_CHUNKSIZE 65536

// Occlusion factor.
#define OCCLUSION_FACTOR 0.35f

// Priority modifier for playing sounds.
#define PLAYING_PRIORITY_MULTIPLIER 1.0f

// Whether to use VoIP or not
#define USE_VOICE_CHAT 1

// Port used for voice chat.
#define VOICE_PORT 12035

/*------------------------------------------------------------------------------------
	UGenericAudioSubsystem.
------------------------------------------------------------------------------------*/

enum EStreamFlags
{
	STREAM_KeepPlaying		= 1,
	STREAM_Terminated		= 2,
	STREAM_ShouldLoop		= 4
};

struct XBSource
{
	USound*			Sound;
	LPDIRECTSOUNDBUFFER DSBuffer;
	AActor*			Actor;
	FVector			Location;
	FLOAT			Priority;
	FLOAT			Radius;
	FLOAT			ZoneRadius;
	FLOAT			UsedRadius;
	FLOAT			WantedRadius;
	DOUBLE			LastChange;
	FLOAT			Volume;
	FLOAT			FadeDuration;
	FLOAT			FadeTime;
	EFadeMode		FadeMode;
	INT				Flags;
	INT				Id;
	UBOOL			Started;
	UBOOL			Paused;
};


struct XBBuffer
{
	USound*			Sound;
	WAVEFORMATEX	wfx;
	INT				Rate;
	INT				Flags;
	INT				Size;
	void*			Data;
};


struct XBStream
{
	INT					Id;
	INT					Status;
	INT					PacketStatus[BUFFERS_PER_STREAM];
	BYTE*				SampleData;
	FString				Name;
	XFileMediaObject*	Decoder;
	LPDIRECTSOUNDSTREAM DSStream;
	HANDLE				Thread;
};

struct XBAmbient
{
	AActor*				Actor;
	FLOAT				Priority;
	DWORD				Flags;
	INT					Id;
};

struct XBChatter
{
	DWORD				IpAddr;
	INT					RefCount;
};


//
// The Generic implementation of UAudioSubsystem.
//
class XBOXAUDIO_API UXBoxAudioSubsystem : public UAudioSubsystem
{
	DECLARE_CLASS(UXBoxAudioSubsystem,UAudioSubsystem,CLASS_Config,XBoxAudio)

	// Variables.
	UViewport*					Viewport;
	UViewport*					DummyViewport;
	DOUBLE						LastTime;
	FVector						LastPosition;
	UBOOL						Initialized;
	UBOOL						LastRealtime;

	UI3DL2Listener*				OldListener;
	FPointRegion				RegionListener;

	// Channels.
	TArray<XBSource>			Sources;
	TArray<XBBuffer>			Buffers;
	TArray<XBStream>			Streams;

	// Deferred release.
	TArray<LPDIRECTSOUNDBUFFER>	StoppedBuffers;

	// Configuration.
	FLOAT						MusicVolume, 
								SoundVolume,
								VoiceVolume; // gam
	INT							MaxChannels;
	UBOOL						AudioStats,
								ReverseStereo,
								UsePrecache;
	
	// Voice Communicator.
#if USE_VOICE_CHAT
    CVoiceManager*				VoiceManager;
	SOCKET						VoiceSocket;
	TArray<XBChatter>			VoiceRecipents;
	DWORD						LocalIpAddress;
#endif
    
	// DSOUND specific variables.
	LPDSEFFECTIMAGEDESC			DSPImage;

	
	// Stats.
	class FXBoxAudioStats
	{
	public:

		INT		STATS_FirstEntry,
				STATS_PlaySoundCycles,
				STATS_UpdateCycles,
				STATS_OcclusionCycles,
				STATS_PlaySoundCalls,
				STATS_OccludedSounds,
				STATS_ActiveStreamingSounds,
				STATS_ActiveRegularSounds,
			STATS_StoppedSounds,
				STATS_LastEntry;
		FXBoxAudioStats();
		void Init();
	} XBoxAudioStats;

	// DirectSound specific
	LPDIRECTSOUND8 DirectSound8;

	// Constructor.
	UXBoxAudioSubsystem();
	void StaticConstructor();

	// UObject interface.
	void Destroy();
	void PostEditChange();
	void ShutdownAfterError();
	void Serialize( FArchive& Ar );

	// UAudioSubsystem interface.
	UBOOL Init();
	void SetViewport( UViewport* Viewport );
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	void NoteDestroy( AActor* Actor );
	UViewport* GetViewport();
	void Update( FSceneNode* SceneNode );
	void RegisterSound( USound* Sound );
	void UnregisterSound( USound* Sound );
	UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch, INT Flags, FLOAT FadeInTime, FLOAT InPriority = 0.f );
	UBOOL StopSound( AActor* Actor, USound* Sound );
  
	INT PlayMusic( FString Song, FLOAT FadeInTime );
	UBOOL StopMusic( INT SongHandle, FLOAT FadeOutTime );
	INT StopAllMusic( FLOAT FadeOutTime );

	void ChangeVoiceChatter( DWORD IpAddr, DWORD ControllerPort, UBOOL Add );
	void EnterVoiceChat();
	void LeaveVoiceChat();

	// Internal functions.
	void SetVolumes();
	void StopSound( INT Index );
	FLOAT SoundPriority( UViewport* Viewport, FVector Location, FLOAT Volume, FLOAT Radius, INT Flags );
	INT GetNewStream();
	void SetI3DL2Listener( UI3DL2Listener* Listener );

	// Voice chat.
#if USE_VOICE_CHAT
	void UpdateVoiceChat( FLOAT DeltaTime );
	void SendVoiceData( void* Data, WORD ControllerPort );
#endif

	virtual void Render( int flags ) {} // amb
};

#define AUTO_INITIALIZE_REGISTRANTS_XBOXAUDIO	\
	UXBoxAudioSubsystem::StaticClass();

#endif

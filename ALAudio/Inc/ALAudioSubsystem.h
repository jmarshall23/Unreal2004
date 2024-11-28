/*=============================================================================
	ALAudioSubsystem.h: Unreal OpenAL Audio interface object.
	Copyright 1999-2001 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Daniel Vogel.
	* Ported to Linux by Ryan C. Gordon.
=============================================================================*/

#ifndef _INC_ALAUDIOSUBSYSTEM
#define _INC_ALAUDIOSUBSYSTEM

/*------------------------------------------------------------------------------------
	Dependencies.
------------------------------------------------------------------------------------*/

#if SUPPORTS_PRAGMA_PACK
#pragma pack (push,8)
#endif

#include "altypes.h"
#include "alctypes.h"
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

#if _WIN64  // !!! FIXME: Hack because AL implementation is BROKEN!
#define ALuint QWORD
#endif

#ifdef WIN32
#ifndef _WIN64  // !!! FIXME ... remove this when Win64 _DOES_ have EAX. --ryan.
#define SUPPORTS_EAX 1
#endif
#endif

#define DYNAMIC_BIND 1

#if SUPPORTS_EAX
typedef ALenum (*EAXSet)(const GUID*, ALuint, ALuint, ALvoid*, ALuint);
typedef ALenum (*EAXGet)(const GUID*, ALuint, ALuint, ALvoid*, ALuint);
#endif

#if SUPPORTS_PRAGMA_PACK
#pragma pack (pop)
#endif

/*------------------------------------------------------------------------------------
	Helpers
------------------------------------------------------------------------------------*/

// Constants.
#define MAX_AUDIOCHANNELS 64
#define NUM_VOICE_BUFFERS 10

#if __WIN32__
#define AL_DLL TEXT("OpenAL32.dll")
#ifdef DEBUG
//#define AL_DEFAULT_DLL TEXT("DefOpenAL32D.dll")
//#define AL_DEFAULT_DLL TEXT("MiniAL32.dll")
#define AL_DEFAULT_DLL TEXT("DefOpenAL32.dll")
#else
#define AL_DEFAULT_DLL TEXT("DefOpenAL32.dll")
#endif
#elif MACOSX
#define AL_DLL TEXT("./openal.dylib")
#define AL_DEFAULT_DLL TEXT("./openal.dylib")
#else
#define AL_DLL TEXT("libopenal.so")
#define AL_DEFAULT_DLL TEXT("openal.so")
#endif

class UALAudioSubsystem;
class FALAudioStreamManager;

//
//	FALSoundBuffer.
//
class FALSoundBuffer
{
public:
	FALSoundBuffer( UALAudioSubsystem* AudioSubsystem );
	~FALSoundBuffer();

	void Reset();

	static FALSoundBuffer* Init( USound* Sound, UALAudioSubsystem* AudioSubsystem );

	UALAudioSubsystem*	AudioSubsystem;
	FALAudioStream*		AudioStream;
	BYTE*				AudioStreamData;
	USound*				Sound;
	TArray<ALuint>		BufferIds;
	INT					BufferQueueIndex,
						BufferUnqueueIndex;
};


//
//	OpenAL implementation of FSoundSource.
//
class ALAUDIO_API FALSoundSource : public FSoundSource
{
public:
	FALSoundSource( UALAudioSubsystem* AudioSubsystem );
	~FALSoundSource();

	UBOOL Update( FLOAT DeltaTime );

	void Play();
	void Stop();
	void Pause();

	UBOOL IsPaused() { return Paused; }
	UBOOL IsStopped();
	UBOOL IsFinished( FLOAT DeltaTime );

	UBOOL						Paused;
	ALuint						SourceId;
	FALSoundBuffer*				Buffer;
};


//
// OpenAL implementation of UAudioSubsystem.
//
class ALAUDIO_API UALAudioSubsystem : public UAudioSubsystem
{
	DECLARE_CLASS(UALAudioSubsystem,UAudioSubsystem,CLASS_Config,ALAudio)

	// Constructor.
	UALAudioSubsystem();
	void StaticConstructor();

	// UObject interface.
	void Destroy();
	void PostEditChange();
	void Shutdown();
	void ShutdownAfterError();
	void Serialize(FArchive& Ar);

	// UAudioSubsystem interface.

	// Initialization.
	virtual UBOOL Init();
	virtual void SetViewport( UViewport* Viewport );
	virtual void Flush( UBOOL Force );

	// Exec.
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	
	// Update.
	virtual UBOOL Update( FSceneNode* SceneNode );

	// Sound playback.
	virtual void PlaySound( FSoundSource* SoundSource );
	INT SeekStream( INT Handle, FLOAT Time );

	// VoIP handling.
	virtual UBOOL PlayVoicePacket( FVoiceInfo* VoiceInfo );

	// Notifications.
	virtual void NoteDestroy( AActor* Actor );
	virtual void NoteDestroy( USound* Sound );

	// Helper functions.
	virtual void SetTransientSource( FSoundSource* Source );
	virtual void SetEAXEffect();

	// Query functions.
	virtual FSoundSource* GetSource( INT SourceIndex ) { return Sources(SourceIndex); }
	virtual FSoundSource* GetTransientSource() { return TransientSource; }
	virtual INT GetNumSources() { return Sources.Num(); }
	
	// Dynamic binding.
	void FindProc( void*& ProcAddress, char* Name, char* SupportName, UBOOL& Supports, UBOOL AllowExt, UBOOL IsALC );
	void FindProcs( UBOOL AllowExt );
	UBOOL FindExt( const TCHAR* Name, UBOOL IsALC );

	// OpenAL
#if DYNAMIC_BIND
	#define AL_EXT(name) static UBOOL SUPPORTS##name;
	#define ALC_EXT(name) static UBOOL SUPPORTS##name;
	#define AL_PROC(ext,ret,func,parms) static ret (CDECL *func)parms;
	#define ALC_PROC(ext,ret,func,parms) static ret (CDECL *func)parms;
	#include "ALFuncs.h"
	#undef AL_EXT
	#undef ALC_EXT
	#undef AL_PROC
	#undef ALC_PROC
#endif

#if SUPPORTS_EAX
	static EAXGet alEAXGet;
	static EAXSet alEAXSet;
#endif

	// Error handling.
	static UBOOL alError( TCHAR* Text, UBOOL Log = 1 );

protected:
	// Variables.
	TArray<FALSoundSource*>		Sources;
	TArray<FALSoundBuffer*>		Buffers;
	FALSoundSource*				TransientSource;
	DOUBLE						LastHWUpdate,
								LastVoiceTime;
	UBOOL						DampenVolume;

	// AL specific.
	ALCdevice*					SoundDevice;
	ALCcontext*					SoundContext;
	void*						DLLHandle;
	INT							EAXVersion;

#if SAVE_SOUNDS
	TArray<BYTE>				RawSoundBeforeDenoiser,
								RawSoundAfterDenoiser;
#endif

#if WITH_SPEEX
	// Voice encoding/ decoding.
	FALVoiceModule*				VoiceModule;
#endif

	// Streaming sound.
	FALAudioStreamManager*		StreamManager;

	// Configuration.
	FLOAT						DopplerFactor,
								TimeBetweenHWUpdates,
								RollOff,
								SpatializedVoiceRadius,
								LocalZOffset;
	INT							MaxChannels,
								MaxEAXVersion;
	UBOOL						ReverseStereo,
								UsePrecache,
								UseEAX,
								Use3DSound,
								UseMMSYSTEM,
								UseDefaultDriver,
								DisablePitch,
								UseLowQualitySound,
								UseVAD,
								EnhancedDenoiser,
								UseSpatializedVoice;

	friend class FALSoundSource;
	friend class FALSoundBuffer;
	friend class FALVoiceModule;
};

#define AUTO_INITIALIZE_REGISTRANTS_ALAUDIO	\
	UALAudioSubsystem::StaticClass();

#endif


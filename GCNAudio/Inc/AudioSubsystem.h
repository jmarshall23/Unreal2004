/*=============================================================================
	AudioSubsystem.h: Unreal audio interface object.
	Copyright 1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Brandon Reinhart.
=============================================================================*/

/*------------------------------------------------------------------------------------
	Helpers
------------------------------------------------------------------------------------*/

// Constants.

#define AUDIO_MINVOLUME		0
#define AUDIO_MAXVOLUME		0x1fff

#define AUDIO_MINPAN		0
#define AUDIO_MIDPAN		64
#define AUDIO_MAXPAN		127
#define AUDIO_SURPAN		127

// Utility Macros.

#define SafeCall(Function) \
{ \
	guard(Function); \
	INT	Result = Function; \
	if(!Result) \
		debugf(NAME_Warning,TEXT("%s failed: %i"),TEXT(#Function),Result); \
	unguard; \
}

/*------------------------------------------------------------------------------------
	UGenericAudioSubsystem.
------------------------------------------------------------------------------------*/

struct FGCNSound
{
	USound*			SourceSound;
	AXVPB*			VoicePointer;
	AXPBADDR		VoiceAddr;
	AXPBSRC			VoiceSrc;

	AActor*			Actor;
	FVector			Location;
	FLOAT			Radius,
					Volume,
					Pitch;
	INT				Panning,
					ViewportNum,
					Flags,
					Priority,
					Id,
					Paused;
};

struct FGCNVoice
{
	FGCNSound*		Sound;
	INT				Id;
	AActor*			Actor;
	FLOAT			Priority;
	FVector			Location;
	FLOAT			Radius,
					Volume,
					Pitch;
	INT				Panning;
	UBOOL			Started,
					Paused;
	INT				Flags;
	INT				PausedSSA;
	INT				ViewportNum;
};

//
// The Generic implementation of UAudioSubsystem.
//
class UGCNAudioSubsystem : public UAudioSubsystem
{
	DECLARE_CLASS(UGCNAudioSubsystem,UAudioSubsystem,0,GCNDrv)

	// Variables.

	UViewport*	Viewport;

	BITFIELD	Initialized,
				AudioStats;
	FTime		LastTime;

	DWORD		ARAMUsed;
	TArray<FGCNSound*> GCNSounds;


	BITFIELD	ReverseStereo,
				Monaural;
	FLOAT		SoundVolume,
				MusicVolume;

	FString		CurrentSong;

	// Constructor.
	UGCNAudioSubsystem();
	void StaticConstructor();

	// UObject interface.
	void Destroy();
	void PostEditChange();
	void ShutdownAfterError();

	// UAudioSubsystem interface.
	UBOOL Init();
	void SetViewport( UViewport* Viewport );
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	void Update( FPointRegion Region, FCoords& Coords );
	void UnregisterSound( USound* Sound );
//	void UnregisterMusic( UMusic* Music );

	UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch, INT Flags );
	UBOOL StopSound( AActor* Actor, USound* Sound );
	void NoteDestroy( AActor* Actor );
	void RegisterSound( USound* Sound );
//	void RegisterMusic( UMusic* Music ) {};
	UBOOL GetLowQualitySetting() {return 0;}
	UViewport* GetViewport();
	void RenderAudioGeometry( FSceneNode* Frame );
	void PostRender( FSceneNode* Frame );
	void CleanUp();

	// Internal functions.
	void ShutupMusic();
	void SetVolumes();
	void StopSound( INT Index );
	FLOAT SoundPriority( UViewport* Viewport, FVector Location, FLOAT Volume, FLOAT Radius )
	{
		return Volume * (1.0 - (Location - (Viewport->Actor->ViewTarget?Viewport->Actor->ViewTarget:Viewport->Actor)->Location).Size()/Radius);
	}
};

#define AUTO_INITIALIZE_REGISTRANTS_GCNAUDIO \
	UGCNAudioSubsystem::StaticClass();

//extern class UGCNAudioPackage : public UPackageBase
//{
//public:
//	const TCHAR* GetName() {return TEXT("GCNAudio");}
//	void InitPackage()
//	{
//		UGCNAudioSubsystem::StaticClass();
//	}
//} GCNAudio;


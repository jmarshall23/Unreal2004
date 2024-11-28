/*=============================================================================
	ALAudioVoice.h: Voice encoding/ decoding module.
	Copyright 2003 Epic Games, Inc. All Rights Reserved.

	Revision history:
	* Created by Daniel Vogel.
=============================================================================*/

#ifndef _INC_ALAUDIOVOICE
#define _INC_ALAUDIOVOICE

/*------------------------------------------------------------------------------------
	Dependencies.
------------------------------------------------------------------------------------*/

#if SUPPORTS_PRAGMA_PACK
#pragma pack(push,8)
#endif

#define EPIC_48K
#include "speex.h"
#include "speex_preprocess.h"

#if SUPPORTS_PRAGMA_PACK
#pragma pack(pop)
#endif

/*------------------------------------------------------------------------------------
	FALVoiceModule.
------------------------------------------------------------------------------------*/

class FALVoiceModule
{
public:
	FALVoiceModule( class UALAudioSubsystem* InAudioSubsystem, FAudioCaptureDevice* InCaptureDevice );
	~FALVoiceModule();

	void SetEncoder( EVoiceCodec InVoiceCodec );

	INT Encode();
	UBOOL Decode( FVoiceInfo* VoiceInfo );

	void NoteDestroy( AActor* Actor );

protected:
	UALAudioSubsystem*			AudioSubsystem;
	FAudioCaptureDevice*		CaptureDevice;

	DWORD						CaptureOffset;
	UBOOL						CapturingVoice,
								VADActive;

	TMap<INT, APawn*>			PlayerIDToPawn;

	void*						SpeexEncoder; 
	SpeexPreprocessState*		SpeexPreprocessor;
	TMap<USound*,void*>			SpeexDecoders;
	SpeexBits*					SpeexData;
	EVoiceCodec					VoiceCodec;
};

#endif


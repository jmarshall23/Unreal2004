/*=============================================================================
	ALAudio.cpp: Unreal OpenAL Audio package.
	Copyright 1999-2001 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Daniel Vogel.
	* Ported to Linux by Ryan C. Gordon.
=============================================================================*/

#include "ALAudioPrivate.h"

/*-----------------------------------------------------------------------------
	Package implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_PACKAGE(ALAudio);

extern "C" {
#ifdef _MSC_VER
FILE _iob[] = { *stdin, *stdout, *stderr };
#define __iob_func() _iob
#endif
};

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/


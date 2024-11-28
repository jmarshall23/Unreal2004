/*=============================================================================
	WinDrv.cpp: Unreal Windows viewport and platform driver.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/

#include "WinDrv.h"

/*-----------------------------------------------------------------------------
	Package implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_PACKAGE(WinDrv);

extern "C" {
#ifdef _MSC_VER
FILE _iob[] = { *stdin, *stdout, *stderr };
#define __iob_func() _iob
#endif
};

/*-----------------------------------------------------------------------------
	Global functions.
-----------------------------------------------------------------------------*/

#include "dxerr8.h"

void DirectInputError( const FString Error, HRESULT hr, UBOOL Fatal )
{
#ifndef _WIN64  // !!! FIXME: No DirectInput support yet!  --ryan.
#ifdef _UNICODE
	debugf( TEXT("%s"), DXGetErrorString8W(hr) );
	debugf( TEXT("%s"), Error );
#else
	debugf( TEXT("%s"), DXGetErrorString8A(hr) );
	debugf( TEXT("%s"), Error );
#endif
#endif
	if ( Fatal )
		appErrorf(TEXT("unrecoverable error - bombing out"));
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


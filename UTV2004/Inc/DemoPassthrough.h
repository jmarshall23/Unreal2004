/*=============================================================================
	UnDemoRec.h: Unreal demo recording.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/
#ifndef _UNNETPASSTHROUGHDRV_H_
#define _UNNETPASSTHROUGHDRV_H_


//#include "engine.h"
#include "UnNet.h"

#include "UnForcePacking_begin.h"

//class UDemoPassthroughDriver;
//class UDemoPassthroughConnection;

/*-----------------------------------------------------------------------------
	UDemoRecConnection.
-----------------------------------------------------------------------------*/

//
// Simulated network connection for recording and playing back game sessions.
//

class UDemoPassthroughConnection : public UDemoRecConnection
{
	DECLARE_CLASS(UDemoPassthroughConnection,UDemoRecConnection,CLASS_Config|CLASS_Transient,utv2003)
	NO_DEFAULT_CONSTRUCTOR(UDemoPassthroughConnection)

	void StaticConstructor();
	UDemoPassthroughConnection( UNetDriver* InDriver, const FURL& InURL );

	// UNetConnection interface.
	void HandleClientPlayer( APlayerController* PC );

	// UDemoRecConnection functions.
	//UDemoPassthroughDriver* GetDriver();
};

/*-----------------------------------------------------------------------------
	UDemoRecDriver.
-----------------------------------------------------------------------------*/

//
// Simulated network driver for recording and playing back game sessions.
//

class UDemoPassthroughDriver : public UDemoRecDriver
{
	DECLARE_CLASS(UDemoPassthroughDriver,UDemoRecDriver,CLASS_Config|CLASS_Transient,utv2003)

	void StaticConstructor();
	// UNetDriver interface.
	UBOOL InitConnect( FNetworkNotify* InNotify, FURL& ConnectURL, FString& Error );
	void TickDispatch( FLOAT DeltaTime );

	INT mu;
};

#include "UnForcePacking_end.h"

#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


/*=============================================================================
	UnPenLev.h: Unreal pending level definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "UnForcePacking_begin.h"

/*-----------------------------------------------------------------------------
	UPendingLevel.
-----------------------------------------------------------------------------*/

//
// Class controlling a pending game level.
//
class UPendingLevel : public ULevelBase
{
	DECLARE_ABSTRACT_CLASS(UPendingLevel,UObject,0,Engine)
	NO_DEFAULT_CONSTRUCTOR(UPendingLevel)

	// Variables.
	UBOOL		Success;
	UBOOL		SentJoin;
	UBOOL		LonePlayer;
	INT			FilesNeeded;
	FString		Error;

	// Constructors.
	UPendingLevel( UEngine* InEngine, const FURL& InURL );

	// UPendingLevel interface.
	virtual void Tick( FLOAT DeltaTime )=0;
	virtual UNetDriver* GetDriver()=0;
	virtual void SendJoin()=0;
	virtual UBOOL TrySkipFile();
	void ReceiveNextFile( UNetConnection* Connection, INT Attempt );
	void NotifyReceivedFile( UNetConnection* Connection, INT PackageIndex, const TCHAR* Error, UBOOL Skipped, INT Attempt );
};

/*-----------------------------------------------------------------------------
	UNetPendingLevel.
-----------------------------------------------------------------------------*/

class UNetPendingLevel : public UPendingLevel
{
	DECLARE_CLASS(UNetPendingLevel,UPendingLevel,CLASS_Transient,Engine)
	NO_DEFAULT_CONSTRUCTOR(UNetPendingLevel)

	// Constructors.
	UNetPendingLevel( UEngine* InEngine, const FURL& InURL );

	// FNetworkNotify interface.
	EAcceptConnection NotifyAcceptingConnection();
	void NotifyAcceptedConnection( class UNetConnection* Connection );
	UBOOL NotifyAcceptingChannel( class UChannel* Channel );
	ULevel* NotifyGetLevel();
	void NotifyReceivedText( UNetConnection* Connection, const TCHAR* Text );
	UBOOL NotifySendingFile( UNetConnection* Connection, FGuid Guid );

	// UPendingLevel interface.
	void Tick( FLOAT DeltaTime );
	UNetDriver* GetDriver() { return NetDriver; }
	void SendJoin();
};

#include "UnForcePacking_end.h"

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


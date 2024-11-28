#ifndef UTVCONNECTION
#define UTVCONNECTION

/*=============================================================================
	UnTcpNetDriver.h: Unreal TCP/IP driver.
	Copyright 1997-2000 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Brandon Reinhart.
=============================================================================*/

#include "UnIpDrv.h"

/*-----------------------------------------------------------------------------
	UTcpipConnection.
-----------------------------------------------------------------------------*/

//
// Windows socket class.
//
class DLL_EXPORT_CLASS UTVConnection : public UNetConnection
{
	DECLARE_CLASS(UTVConnection,UNetConnection,CLASS_Config|CLASS_Transient,utv2003)
	NO_DEFAULT_CONSTRUCTOR(UTVConnection)
/*	void* operator new(size_t size)
	{return malloc(size);}
	void operator delete(void* p)
	{free(p);}
*/
	// Variables.
	sockaddr_in		RemoteAddr;
	SOCKET			Socket;
	UBOOL			OpenedLocally;
	FResolveInfo*	ResolveInfo;

	// Constructors and destructors.
	UTVConnection( SOCKET InSocket, UNetDriver* InDriver, sockaddr_in InRemoteAddr, EConnectionState InState, UBOOL InOpenedLocally, const FURL& InURL );

	void LowLevelSend( void* Data, INT Count );
	FString LowLevelGetRemoteAddress();
	FString LowLevelDescribe();
};

/*-----------------------------------------------------------------------------
	UTcpNetDriver.
-----------------------------------------------------------------------------*/

//
// BSD sockets network driver.
//
class DLL_EXPORT_CLASS UTVNetDriver : public UNetDriver
{
	DECLARE_CLASS(UTVNetDriver,UNetDriver,CLASS_Transient|CLASS_Config,utv2003)
public:
/*	void* operator new(size_t size)
	{return malloc(size);}
	void operator delete(void* p)
	{free(p);}
*/
	UBOOL AllowPlayerPortUnreach;
	UBOOL LogPortUnreach;

	// Variables.
	sockaddr_in	LocalAddr;
	SOCKET		Socket;

	// Constructor.
	void StaticConstructor();
	UTVNetDriver()
	{}

	// UNetDriver interface.
	UBOOL InitConnect( FNetworkNotify* InNotify, FURL& ConnectURL, FString& Error );
	UBOOL InitListen( FNetworkNotify* InNotify, FURL& LocalURL, FString& Error );
	void TickDispatch( FLOAT DeltaTime );
	FString LowLevelGetNetworkNumber();
	void LowLevelDestroy();

	// UTcpNetDriver interface.
	UBOOL InitBase( UBOOL Connect, FNetworkNotify* InNotify, FURL& URL, FString& Error );
	UTVConnection* GetServerConnection();
	FSocketData GetSocketData();
};

#endif
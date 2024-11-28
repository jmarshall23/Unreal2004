/*=============================================================================
	MasterServer.h: Unreal Master, CD Key and Stats Server
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter

=============================================================================*/

/*-----------------------------------------------------------------------------
	Includes.
-----------------------------------------------------------------------------*/

// Socket API.
#if _MSC_VER
	#define __WINSOCK__ 1
	#define SOCKET_API TEXT("WinSock")
#else
	#define __BSD_SOCKETS__ 1
	#define SOCKET_API TEXT("Sockets")
#endif

// WinSock includes.
#if __WINSOCK__
#ifdef _XBOX
	#include <xtl.h>
#else
	#include <windows.h>
	#include <winsock.h>
	#include <conio.h>
#endif
#endif

// BSD socket includes.
#if __BSD_SOCKETS__
	#include <stdio.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/uio.h>
	#include <sys/ioctl.h>
	#include <sys/time.h>
	#include <errno.h>
	#include <pthread.h>
	#include <fcntl.h>

	// Handle glibc < 2.1.3
	#ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 0x4000
	#endif
#endif

// libMySQL
#include "mysql.h"
#include "Engine.h"
#include "../../IpDrv/Inc/UnIpDrv.h"
#include "Database.h"


/*-----------------------------------------------------------------------------
	Defines.
-----------------------------------------------------------------------------*/

#if !_MSC_VER
#define MASTERSERVER_API 
#endif

/*-----------------------------------------------------------------------------
	Database Key.
-----------------------------------------------------------------------------*/

#define MASTER_SERVER_DATABASE	TEXT("ut2004")

/*-----------------------------------------------------------------------------
	UMasterServerCommandlet
-----------------------------------------------------------------------------*/

class UMasterServerCommandlet : public UCommandlet
{
	DECLARE_CLASS(UMasterServerCommandlet,UCommandlet,CLASS_Transient,MasterServer);

	class FMasterServerListenLink* ListeningSocket;

	// UCommandlet interface
	void StaticConstructor();
	INT Main( const TCHAR* Parms );

	// UMasterServerCommandlet
	void MainLoop( INT ListenPort, INT RestartTime );
	void UpdateStats();
};

/*-----------------------------------------------------------------------------
	Misc helper functions
-----------------------------------------------------------------------------*/

INT SnipAppart( TArray<FString>& LineParts, FString Line, const TCHAR* Separator );
FString JoinTogether( TArray<FString>& LineParts, const TCHAR* Separator );

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


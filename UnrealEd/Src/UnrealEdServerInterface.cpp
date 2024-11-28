//----------------------------------------------------------------------
// UnrealEdServerInterface.cpp
//
// Joshua Lee, Travis Johnson at Secret Level
//
// Implementes the COM server interface for UnrealEd.
//----------------------------------------------------------------------

#include "UnrealEdServerInterface.h"
#include "UnrealEd.h"
#include "UnrealEdImport.h"
#include "SerializedDataLoader.h"

//
//Forward declarations from the DLL so that the client knows if the DLL should unload
//
extern BOOL CanEXEServerUnload(void);
extern BOOL UnloadEXEServer(void);
extern BOOL AddEXEServerRef(void);
extern BOOL ReleaseEXEServerRef(void);
extern BOOL LockEXEServer(void);
extern BOOL UnLockEXEServer(void);


// This struct holds data members for UnrealEdServerInterface that
// we want to keep insulated, thus avoiding a long chain of dependencies
// for clients of this class.
struct UnrealEdServerInterfaceImp
{
	// Stores result of ExecCommand(); reported by FetchCommandResult().
	FStringOutputDevice  commandResult;
};


CUnrealEdServerInterface  gUnrealEdServerInterface;

STDMETHODIMP CUnrealEdServerInterface::QueryInterface(REFIID iid, LPVOID *ppv)
{
    *ppv = NULL;
    if (IID_IUnknown == iid)
        *ppv = (LPVOID)(IUnknown *)this;     
	else if (IID_IUnrealEdServerInterface == iid)
        *ppv = (LPVOID)(IUnrealEdServerInterface *)this;     
    else 
		return E_NOINTERFACE;	//Interface not supported
	//Perform reference count through the returned interface
    ((IUnknown *)*ppv)->AddRef();
    return NOERROR;    
}

STDMETHODIMP_(ULONG) CUnrealEdServerInterface::AddRef(void)
{
	++mIRef;
	return mIRef;
}

STDMETHODIMP_(ULONG) CUnrealEdServerInterface::Release(void)
{
	--mIRef;
	if (0 == mIRef)
	{
		//Decrement the global object count 
		::ReleaseEXEServerRef();
		//See if it's alright to unload the server
		if (::CanEXEServerUnload())
			::UnloadEXEServer();
		return 0;
	}
	return mIRef;
}


//----------------------------------------------------------------------
// Ping the server.  This allows a client to probe an existintg
// instance of the interface to see if the server is alive,
// useful for detecting stale handles.
//----------------------------------------------------------------------

STDMETHODIMP  CUnrealEdServerInterface::Ping()
{
	return( S_OK );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

STDMETHODIMP CUnrealEdServerInterface::LoadMesh( 
										const char * packageName, 
										const char * groupName, 
										const char * objName, 
										int   byteCount, const char * buffer )
{
	HRESULT  hResult = S_OK;

	SerializedMeshLoader  loader( buffer, byteCount );

	loader.setPackageName( packageName );
	loader.setGroupName  ( groupName );
	loader.setObjName    ( objName );

	if( ! loader.load() )
	{
		hResult = E_FAIL;
	}

	return( hResult );
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------

STDMETHODIMP CUnrealEdServerInterface::LoadSkin( 
										const char * packageName, 
										const char * objName, 
										float scale,
										int   byteCount, const char * buffer )
{
	HRESULT  hResult = S_OK;

	SerializedSkinLoader  loader( buffer, byteCount );

	loader.setPackageName( packageName );
	loader.setObjName    ( objName );
	loader.setScale( scale );

	if( ! loader.load() )
	{
		hResult = E_FAIL;
	}

	return( hResult );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

STDMETHODIMP CUnrealEdServerInterface::LoadAnimation( 
										const char * packageName, 
										const char * objName, 
										int byteCount, const char * buffer )
{
	HRESULT  hResult = S_OK;

	SerializedAnimationLoader  loader( buffer, byteCount );

	loader.setPackageName( packageName );
	loader.setObjName    ( objName );

	if( ! loader.load() )
	{
		hResult = E_FAIL;
	}

	return( hResult );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

STDMETHODIMP CUnrealEdServerInterface::ExecCommand( 
										const char * command, 
										int * resultBufferSize )
{
	extern class UUnrealEdEngine * GUnrealEd;

	HRESULT  status = S_OK;

	if( 0 == mpImp )
	{
		// We cannot guarentee that the mpImp object can be constructed when
		// this class is constructed because the Unreal memory services may
		// not have been started yet.  However, by the time this method
		// executes, the memory services are up and running, so we can create
		// the object the first time we get here.
		mpImp = new UnrealEdServerInterfaceImp;
	}


	if( GUnrealEd->Exec( *FString( command ), mpImp->commandResult ) )
	{
		if( 0 < mpImp->commandResult.Len() )
		{
			*resultBufferSize = 1 + strlen( TCHAR_TO_ANSI( &mpImp->commandResult[0] ) );
		}
		else
		{
			*resultBufferSize = 0;
		}
	}
	else
	{
		status = E_FAIL;
	}

	return( status );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

STDMETHODIMP  CUnrealEdServerInterface::FetchCommandResult(
							int resultBufferSize,
							char * resultBuffer )
{
	strncpy( resultBuffer, TCHAR_TO_ANSI( &mpImp->commandResult[0]  ) , resultBufferSize );

	resultBuffer[ resultBufferSize - 1 ] = '\0';
	
	mpImp->commandResult.Empty();

	return( S_OK );
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------


CUnrealEdServerInterface::CUnrealEdServerInterface()
: mpImp( ::new UnrealEdServerInterfaceImp() )
{
	mIRef = 0;
}

CUnrealEdServerInterface::~CUnrealEdServerInterface()
{
	mIRef = -1;
	delete mpImp;
}




//----------------------------------------------------------------------
//----------------[ class CUnrealEdServerInterfaceFactory ]-------------
//----------------------------------------------------------------------

//
//CreateInstance
//
STDMETHODIMP CUnrealEdServerInterfaceFactory::CreateInstance(IUnknown* pUnknownOuter, 
											  REFIID iid, LPVOID *ppv)
{
	HRESULT hr;
	CUnrealEdServerInterface * pCUnrealEdServerInterface = NULL;

	*ppv = NULL;
	//This object doesn't support aggregation
	if (NULL != pUnknownOuter)
		return CLASS_E_NOAGGREGATION;
	//Create the CUserInfo object
//	pCUnrealEdServerInterface = new CUnrealEdServerInterface();
	pCUnrealEdServerInterface = &gUnrealEdServerInterface;

	if (NULL == pCUnrealEdServerInterface)
		return E_OUTOFMEMORY;
	//Retrieve the requested interface
	hr = pCUnrealEdServerInterface->QueryInterface(iid, ppv);
	if (FAILED(hr))
	{
//		delete pCUnrealEdServerInterface;
		pCUnrealEdServerInterface = NULL;
		return hr;
	}
	//Increment the global object counter
	AddEXEServerRef();	

	return NOERROR;
}//CreateInstance

//
//LockServer
//
STDMETHODIMP CUnrealEdServerInterfaceFactory::LockServer(BOOL bLock)
{
	if (bLock)
	{
		LockEXEServer();
	}
	else
	{
		UnLockEXEServer();
		//See if it's alright to unload the server
		if (::CanEXEServerUnload())
			::UnloadEXEServer();
	}
	return NOERROR;
}//LockServer

//
//QueryInterface
//
STDMETHODIMP CUnrealEdServerInterfaceFactory::QueryInterface(REFIID iid, LPVOID *ppv)
{
    *ppv = NULL;
    if (IID_IUnknown == iid)
        *ppv = (LPVOID)(IUnknown *)this;     
	else if (IID_IClassFactory == iid) 
        *ppv = (LPVOID)(IClassFactory *)this;     
    else 
		return E_NOINTERFACE;	//Interface not supported
	//Perform reference count through the returned interface
    ((IUnknown *)*ppv)->AddRef();

	return NOERROR;    
}//QueryInterface

//
//AddRef
//
STDMETHODIMP_(ULONG) CUnrealEdServerInterfaceFactory::AddRef(void)
{
	return ++mFRef;
}//AddRef

//
//Release
//
STDMETHODIMP_(ULONG) CUnrealEdServerInterfaceFactory::Release(void)
{
	mFRef--;
	if (0 == mFRef)
	{
//		delete this;
		return 0;
	}
	return mFRef;
}//Release


//----------------------------------------------------------------------
//--------------------------[ Free functions ]--------------------------
//----------------------------------------------------------------------



#include <objbase.h>
#include <tchar.h>
#include "UnrealEdServerInterface.h"

#define MAX_STRING_LENGTH 255
#define GUID_SIZE 128

BOOL StartCOMServices( void );
BOOL RegisterServer(CLSID clsid, LPTSTR lpszDescription);
BOOL UnregisterServer(CLSID clsid);
BOOL SetRegKeyValue(HKEY hKeyRoot, LPTSTR lpszKey, LPTSTR lpszSubKey, 
					LPTSTR lpszNamedValue, LPTSTR lpszValue);

HMODULE g_hModule = NULL;
ULONG gRefs = 0;
ULONG gLocks = 0;
DWORD g_dwRegisterUnrealInterface;
DWORD g_dwRegisterUserInfoHandler;

CUnrealEdServerInterfaceFactory  gUnrealEdServerInterfaceFactory;


BOOL StartCOMServices( const TCHAR * rawCmdLine  )
{
	HRESULT hr;
	MSG     msg;
	_TCHAR  szTokens[] = _TEXT("-/");
	LPTSTR  szNextToken;
	LPTSTR  szCmdLine;
	CUnrealEdServerInterface        *pCUnrealEdServerInterface        = NULL;
	CUnrealEdServerInterfaceFactory *pCUnrealEdServerInterfaceFactory = NULL;

	g_hModule = GetModuleHandle( NULL );

	if( ! ParseParam( rawCmdLine, TEXT( "Embedding" ) ) )
	{
		if( ParseParam( rawCmdLine, TEXT( "UnregServer" ) ) )
		{
			::UnregisterServer( CLSID_UnrealEdServerInterface );
			
			return FALSE;
		}

		if( ParseParam( rawCmdLine, TEXT( "RegServer" ) ) )
		{
			::RegisterServer(CLSID_UnrealEdServerInterface, 
				_TEXT( "Secret Level UnrealEdServerInterface" ) );

			MessageBox( NULL, TEXT( "RegisteredComServer" ), TEXT( "COM" ), MB_OK );
			
			return FALSE;
		}
	}

	// Initialize the COM Library.
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		MessageBox( NULL, TEXT( "Can Not Get COM going" ), TEXT( "com" ), MB_OK );
		return FALSE;
	}

	//Create the UnrealEdServerInterface class factory
	pCUnrealEdServerInterfaceFactory = &gUnrealEdServerInterfaceFactory;

	//Check for out of memory error
	if (NULL != pCUnrealEdServerInterfaceFactory)
	{
		// Register the UnrealEdServerInterface class factory
		hr = CoRegisterClassObject(CLSID_UnrealEdServerInterface, 
			(IUnknown *)pCUnrealEdServerInterfaceFactory, CLSCTX_LOCAL_SERVER, 
			REGCLS_MULTIPLEUSE, &g_dwRegisterUnrealInterface);
		if (FAILED(hr))
		{
			delete pCUnrealEdServerInterfaceFactory;
			pCUnrealEdServerInterfaceFactory = NULL;
		}
	}
	else
	{
		MessageBox(NULL,TEXT("new failed for CUnrealEdServerInterfaceFactory"),TEXT("com"), MB_OK);
	}

	return( TRUE );
}



BOOL RegisterServer(CLSID clsid, LPTSTR lpszDescription)
{
	BOOL bOK;
	_TCHAR szModulePath[MAX_PATH + 1];
	_TCHAR szCLSID[GUID_SIZE + 1];
	_TCHAR szCLSIDKey[MAX_STRING_LENGTH + 1];
	wchar_t wszGUID[GUID_SIZE + 1];

	// Obtain the path to server's executable file for later use
	GetModuleFileName(g_hModule, szModulePath, 
		sizeof(szModulePath) / sizeof(_TCHAR));
	//Convert the CLSID to the format
	//{00000000-0000-0000-0000-000000000000}
	StringFromGUID2(clsid, wszGUID, sizeof(wszGUID) / sizeof(wchar_t));
#ifdef  _UNICODE
	//UNICODE
	_tcscpy(szCLSID, wszGUID);
#else	
	//SBCS and MBCS
	//Convert from the wide character set to the multibyte character set
	WideCharToMultiByte(CP_ACP, 0, wszGUID, -1, szCLSID, 
		sizeof(szCLSID) / sizeof(_TCHAR), NULL, NULL);
#endif
	//HKEY_CLASSES_ROOT\CLSID\{00000000-0000-0000-0000-000000000000}
	_tcscpy(szCLSIDKey, _TEXT("CLSID\\"));
	_tcscat(szCLSIDKey, szCLSID);
	bOK = SetRegKeyValue(HKEY_CLASSES_ROOT, szCLSIDKey, NULL, NULL, lpszDescription);
	if (bOK)
		bOK = SetRegKeyValue(HKEY_CLASSES_ROOT, szCLSIDKey, _TEXT("LocalServer32"), NULL, szModulePath);

	return bOK;
}

BOOL UnregisterServer(CLSID clsid)
{
	long lErrorCode;
	_TCHAR szCLSID[GUID_SIZE + 1];
	_TCHAR szCLSIDKey[MAX_STRING_LENGTH + 1];
	_TCHAR szLocalServer32Key[MAX_STRING_LENGTH + 1];
	wchar_t wszGUID[GUID_SIZE + 1];

	//Convert the CLSID to the format
	//{00000000-0000-0000-0000-000000000000}
	StringFromGUID2(clsid, wszGUID, GUID_SIZE);
#ifdef  _UNICODE
	//UNICODE
	_tcscpy(szCLSID, wszGUID);
#else	
	//SBCS and MBCS
	//Convert from the wide character set to the multibyte character set
	WideCharToMultiByte(CP_ACP, 0, wszGUID, -1, szCLSID, 
		sizeof(szCLSID) / sizeof(_TCHAR), NULL, NULL);
#endif
	//HKEY_CLASSES_ROOT\CLSID\{00000000-0000-0000-0000-000000000000}
	_tcscpy(szCLSIDKey, _TEXT("CLSID\\"));
	_tcscat(szCLSIDKey, szCLSID);
	_tcscpy(szLocalServer32Key, szCLSIDKey);
	_tcscat(szLocalServer32Key, _TEXT("\\LocalServer32"));
	//Delete sub-keys first
	lErrorCode = RegDeleteKey(HKEY_CLASSES_ROOT, szLocalServer32Key);
	//Delete the entry under CLSID.
	if (ERROR_SUCCESS == lErrorCode)
		lErrorCode = RegDeleteKey(HKEY_CLASSES_ROOT, szCLSIDKey);
	if (ERROR_SUCCESS == lErrorCode)
		return TRUE;
	else
		return FALSE;
}

BOOL SetRegKeyValue(HKEY hKeyRoot, LPTSTR lpszKey, LPTSTR lpszSubKey, 
					LPTSTR lpszNamedValue, LPTSTR lpszValue)
{
	BOOL bOk = FALSE;
	long lErrorCode;
	HKEY hKey;
	_TCHAR szKey[MAX_STRING_LENGTH + 1];

	_tcscpy(szKey, lpszKey);
	if (NULL != lpszSubKey)
	{
		_tcscat(szKey, _TEXT("\\"));
		_tcscat(szKey, lpszSubKey);
	}
	lErrorCode = RegCreateKeyEx(hKeyRoot, szKey, 0, 
		 NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 
		 NULL, &hKey, NULL);
	if (ERROR_SUCCESS == lErrorCode)
	{
		lErrorCode = RegSetValueEx(hKey, lpszNamedValue, 0, REG_SZ, 
		   (BYTE *)lpszValue, (_tcslen(lpszValue) + 1) * sizeof(_TCHAR));
		if (ERROR_SUCCESS == lErrorCode)
			bOk = TRUE;
		RegCloseKey(hKey);
	}

	return bOk;
}


BOOL CanEXEServerUnload(void)
{	//The server can unload if there are no outstanding objects or class factory locks
	if(0 == gRefs && 0 == gLocks)
		return TRUE;
	else
		return FALSE;
}

BOOL UnloadEXEServer(void)
{
	//Unload the server by posting the WM_QUIT to the message que
	PostQuitMessage(0);
	return TRUE;
}

BOOL AddEXEServerRef(void)
{
	++gRefs;

	return TRUE;
}

BOOL ReleaseEXEServerRef(void)
{
	--gRefs;
	return TRUE;
}

BOOL LockEXEServer()
{
	++gLocks;
	return TRUE;
}
BOOL UnLockEXEServer()
{
	--gLocks;
	return FALSE;
}



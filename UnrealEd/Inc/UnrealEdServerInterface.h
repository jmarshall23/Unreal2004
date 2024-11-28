//----------------------------------------------------------------------
// UnrealEdServerInterface.cpp
//
// Joshua Lee, Travis Johnson at Secret Level
//
// These classes are the concrete COM server interface for UnrealEd.  
// They are derived from Abstract Base Classes declared in the file
// "UnrealEdServerInterface_h.h" that is automatically generated from
// the description in "UnrealEdInterface.idl".
//----------------------------------------------------------------------

#if !defined __UNREALEDSERVERINTERFACE_H__
#define __UNREALEDSERVERINTERFACE_H__

#include "UnrealEdServerInterface_h.h"


BOOL StartCOMServices( const TCHAR * rawCmdLine  );

struct UnrealEdServerInterfaceImp;



class CUnrealEdServerInterface : IUnrealEdServerInterface
{
private:
	ULONG			mIRef;
	UnrealEdServerInterfaceImp * mpImp;

public:

	//IUnknown
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID *ppv );
	STDMETHODIMP_(ULONG)AddRef();
	STDMETHODIMP_(ULONG)Release();

	// Ping the server.  This allows a client to probe an existintg
	// instance of the interface to see if the server is alive,
	// useful for detecting stale handles.
	STDMETHODIMP	 Ping();

	// Loads a static mesh; the mesh data is serialized in "buffer" 
	// according to the schema described in SerializedDataTypes.h
	STDMETHODIMP	LoadMesh( 
							const char * packageName, 
							const char * groupName, 
							const char * objName, 
							int   byteCount, const char * buffer );

	// Loads a skin, i.e. a mesh bound to a skeleton; the skin data is serialized 
	//in "buffer" according to the schema described in SerializedDataTypes.h
	STDMETHODIMP	LoadSkin( 
							const char * packageName, 
							const char * objName, 
							float scale,
							int   byteCount, const char * buffer );

	// Loads animations for a skin; the animation data is serialized in "buffer" 
	// according to the schema described in SerializedDataTypes.h
	STDMETHODIMP	LoadAnimation( 
							const char * packageName, 
							const char * objName, 
							int   byteCount, const char * buffer ); 

	// Execute an unreal command.  This sets resultBufferSize to the number
	// of bytes required to return the result.  If resultBufferSize is non-zero,
	// you should fetch the result using FetchCommandResult. Note that the
	// result will only be valid until the next time ExecCommand() executes, or
	// the result is fetched.
	STDMETHODIMP	ExecCommand( 
							const char * command, 
							int * resultBufferSize );

	// Fetch the result of the last ExecCommand().  [See ExecCommand() description
	// regarding the buffer size.]  After returning this result, the server will
	// release its internal copy; further invocations of this method will be invalid
	// until the next ExecCommand() concludes.
	STDMETHODIMP FetchCommandResult(
							int resultBufferSize,
							char * resultBuffer );


	CUnrealEdServerInterface();
	~CUnrealEdServerInterface();
};




class CUnrealEdServerInterfaceFactory : public IClassFactory
{
private:
	ULONG			mFRef;
public:
	//IUnknown
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID *ppv );
    STDMETHODIMP_(ULONG)AddRef( void );
    STDMETHODIMP_(ULONG)Release( void );
	//IClassFactory
	STDMETHODIMP CreateInstance( IUnknown* pUnknownOuter, REFIID iid, LPVOID *ppv );
	STDMETHODIMP LockServer( BOOL bLock );
	//Constructor
	CUnrealEdServerInterfaceFactory()
	{
		mFRef = 0;
	}
};

#endif //__UNREALEDSERVERINTERFACE_H__

#ifndef _UTVPACKAGEMAP_H_
#define _UTVPACKAGEMAP_H_

#include "Engine.h"
#include "UnNet.h"

#include "UnForcePacking_begin.h"

#define UTV2004C TEXT("UTV2004C")
#define UTVVERSION TEXT("0.99b8")

class UUTVPackageMap : public UPackageMap
{
	DECLARE_CLASS(UUTVPackageMap,UPackageMap,CLASS_Transient,utv2004);
	void ShowStatus ();
	UBOOL CreateLinkers ();
	INT GetMaxObjectIndex ();
	void IndexToObjectName( INT Index );
	INT GetReplicationId ();
	void Compute ();
	UBOOL ComputeError ();
	void FixLinkers (UPackageMap* Other);
	void PostFixLinkers (UPackageMap* Other);
	void ClearDepend ();
	INT GetDepend ();
	UBOOL SerializeObject( FArchive& Ar, UClass* Class, UObject*& Object );

	INT GetReplicationSend ();
	INT GetReplicationGet ();
	INT GetReplicationMax ();
	INT GetReplicationSendTarget ();
	INT GetReplicationGetTarget ();

	INT NeededPackage ();
protected:
	INT UTVReplication;
	INT RSend, RGet, RMax;
	INT RGetT, RSendT;
	INT MissingPackage;
	UBOOL WasComputeError;

	TArray<FString> RealNames;
	INT LastDepend;
};

#include "UnForcePacking_end.h"

#endif
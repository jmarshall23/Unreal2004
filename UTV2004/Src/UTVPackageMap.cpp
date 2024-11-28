#include "UTVPackageMap.h"
#include "UnLinker.h"
#include "..\inc\utvpackagemap.h"

void UUTVPackageMap::ShowStatus ()
{
	debugf (TEXT ("We have %i names and %i objects loaded"), MaxNameIndex, MaxObjectIndex);
/*	for(int a=0;a<100;++a){
		printf("%i ",NameIndices(a));
		IndexToObjectName(NameIndices(a));
	}*/
}

void UUTVPackageMap::Compute ()
{
	UPackageMap::Compute ();

	guard (UUTVPackageMap::Compute);

	WasComputeError = false;

	//check so we are not outdated
	for( INT j=0; j<List.Num(); j++ ) {
		if( List(j).LocalGeneration<List(j).RemoteGeneration ){
			debugf(TEXT("Error: Both server and primary has newer version of package than us %i %i %s"),List(j).RemoteGeneration,List(j).LocalGeneration,*List(j).URL);
			WasComputeError = true;
			return;
		}
		//debugf (TEXT ("Package %i (%s) - local %i - remote %i"), j, *List(j).URL, List(j).LocalGeneration, List(j).RemoteGeneration);
	}

	//Find the object id of our new replication object
	UTVReplication = 0;

	INT i = List.Num () - 1;
	FPackageInfo& Info = List (i);
	for (INT j = 0; j < Info.ObjectCount; ++j) {
		if (Info.Linker->ExportMap(j).ObjectName == FName (TEXT ("utvReplication"))) {
			UTVReplication = Info.ObjectBase + j;
			debugf (TEXT ("Found utvReplication with object id %i"), UTVReplication);
		}

		//debugf (TEXT ("Loaded %i: %s"), Info.ObjectBase + j, *(Info.Linker->ExportMap(j).ObjectName));
	}

	if (UTVReplication == 0) {
		debugf (TEXT ("Could not find the utvReplication!"));
	}

	//Create the needed things for reading out the utvReplication stuff
	RSend = 0;
	RGet = 0;
	RSendT = 0;
	RGetT = 0;

	UClass* tmp = StaticLoadClass( AActor::StaticClass(), NULL, UTV2004C TEXT(".utvReplication"), NULL, LOAD_NoFail, NULL );
	FClassNetCache* cc = GetClassNetCache(tmp);
	if (!cc) {
		debugf (TEXT ("Could not create the ClassNetCache for utvReplication!"));
	}

	printf("-");
	RMax = cc->GetMaxIndex ();
	for (int i=0; i < cc->GetMaxIndex(); ++i) {
		printf("\010-");
		FFieldNetCache* FieldCache = cc->GetFromIndex( i );
		printf("\010/");
		FName f = FieldCache->Field->GetFName();		
		printf("\010|");

		if (f == FName (TEXT ("SendToServer"))) {
			RSend = i;
			printf("\010");
			debugf (TEXT ("\010Found utvReplication.SendToServer with id %i"), RSend);
			printf("-");	
		}
		printf("\010\\");
		if (f == FName (TEXT ("GetFromServer"))) {
			RGet = i;
			printf("\010");
			debugf (TEXT ("Found utvReplication.GetFromServer with id %i"), RGet);
			printf("-");
		}
		if (f == FName (TEXT ("SendTarget"))) {
			RSendT = i;
			printf("\010");
			debugf (TEXT ("\010Found utvReplication.SendTarget with id %i"), RSendT);
			printf("-");	
		}
		printf("\010\\");
		if (f == FName (TEXT ("GetTarget"))) {
			RGetT = i;
			printf("\010");
			debugf (TEXT ("Found utvReplication.GetTarget with id %i"), RGetT);
			printf("-");
		}
	}
	printf("\010");
	if (RSend == 0) {
		debugf (TEXT ("Could not find utvReplication.SendToServer!"));
	}
	if (RGet == 0) {
		debugf (TEXT ("Could not find utvReplication.GetFromServer!"));
	}

	//precache
	debugf (TEXT ("Simple precache start.."));
	UObject* Object;
	Object = IndexToObject( UTVReplication - 1000, 1 );
	debugf (TEXT ("Simple precache end.."));

	unguard;
}

void UUTVPackageMap::FixLinkers (UPackageMap* Other) 
{
	guard (UUTVPackageMap::FixLinkers);

	//Start with clearing the other's list (should be empty)
	while (Other->List.Num () > 0) {
		Other->List.Remove (0);
	}

	//now copy our list there
	for (INT i = 0; i < List.Num(); ++i) {
		FPackageInfo& Info = *new(Other->List)FPackageInfo(NULL);
		Info.Guid = List(i).Guid;
		Info.RemoteGeneration = List(i).RemoteGeneration;
		Info.LocalGeneration = List(i).RemoteGeneration;
		Info.FileSize = List(i).FileSize;
		Info.PackageFlags = List(i).PackageFlags;
		Info.URL = List(i).URL;
		Info.Parent = CreatePackage (NULL, List(i).Parent->GetName ());
		Info.Linker = List(i).Linker;
	}

	unguard;
}

//This function fixes all url entries that reside in the cache folder
void UUTVPackageMap::PostFixLinkers (UPackageMap* Other) 
{
	guard (UUTVPackageMap::PostFixLinkers);

	for (INT i = 0; i < Other->List.Num(); ++i) {
		if (RealNames(i).Len () > 0) {
			//debugf (TEXT ("Changing %s to %s"), *Other->List(i).URL, *RealNames (i));
			Other->List(i).URL = RealNames (i);
		}
	}

	unguard;
}

void UUTVPackageMap::ClearDepend ()
{
	LastDepend = 0;
}

INT UUTVPackageMap::GetDepend ()
{
	return LastDepend;
}

UBOOL UUTVPackageMap::SerializeObject( FArchive& Ar, UClass* Class, UObject*& Object )
{
	guard(UUTVPackageMap::SerializeObject);
	DWORD Index=0;
	if( Ar.IsLoading() )
	{
		Object = NULL;
		BYTE B=0; Ar.SerializeBits( &B, 1 );
		if (B)
		{
			// Dynamic actor or None.
			Ar.SerializeInt( Index, UNetConnection::MAX_CHANNELS );
			Object = (UObject*)Index;
			if (Class != NULL) {
				if (Class->IsChildOf (AActor::StaticClass ()))
					LastDepend = Index;
			}
//			LastDepend = Index;
		}
		else
		{
			// Static object.
			Ar.SerializeInt( Index, MaxObjectIndex );
			//debugf (TEXT ("Static object with index %i"), Index);
			Index = Index + 2000;
			Object = (UObject*)Index;
		}

		return 1;
	}
	else
	{
		Index = (DWORD)Object;
		if (Index < 2000) {
			BYTE B=1;
			Ar.SerializeBits (&B, 1);
			Ar.SerializeInt (Index, UNetConnection::MAX_CHANNELS);
		}
		else {
			Index = Index - 2000;
			BYTE B=0;
			Ar.SerializeBits (&B, 1);
			Ar.SerializeInt (Index, MaxObjectIndex);
		}
		return 1;
	}
	unguard;
}

INT UUTVPackageMap::GetReplicationId ()
{
	return UTVReplication;
}

INT UUTVPackageMap::GetReplicationGet ()
{
	return RGet;
}

INT UUTVPackageMap::GetReplicationSend ()
{
	return RSend;
}

INT UUTVPackageMap::GetReplicationSendTarget ()
{
	return RSendT;
}

INT UUTVPackageMap::GetReplicationGetTarget ()
{
	return RGetT;
}

INT UUTVPackageMap::GetReplicationMax ()
{
	return RMax;
}

INT UUTVPackageMap::NeededPackage ()
{
	return MissingPackage;
}

UBOOL UUTVPackageMap::ComputeError ()
{
	return WasComputeError;
}

UBOOL UUTVPackageMap::CreateLinkers ()
{
	TCHAR t[256] = TEXT("");

	guard (UTVPackageMap::CreateLinkers);
	
	//Indicate nothing missing yet
	MissingPackage = -1;
	RealNames.Empty ();
	
	BeginLoad ();

	for( INT i=0; i<List.Num(); i++ ) {

		//Start with checking if the load will fail
		if (!appFindPackageFile( List(i).Parent->GetName(), &List(i).Guid, t, List(i).RemoteGeneration )) {
			debugf (TEXT ("Packagemap: Could not find the package: %s"), List(i).Parent->GetName ());
			MissingPackage = i;

			EndLoad ();
			return false;
		}
		else {
			//Update the list with actual path+name of this file
			INT j = RealNames.AddZeroed ();	//must return == i :)
			check (i == j);
			RealNames (i) = FString::Printf (TEXT ("%s"), t);
		}

		//Now get a linker
		List(i).Linker = GetPackageLinker
		(
			List(i).Parent,
			NULL,
			LOAD_Verify | LOAD_Throw | LOAD_NoWarn | LOAD_NoVerify,
			NULL,
			&List(i).Guid,
			List(i).RemoteGeneration
		);

		//debugf (TEXT ("Linker %i (%s): %s"), i, *List(i).URL, *List(i).Linker->GetFName ());
	}

	EndLoad ();

	return true;

	unguard;
}

INT UUTVPackageMap::GetMaxObjectIndex ()
{
	return MaxObjectIndex;
}

void UUTVPackageMap::IndexToObjectName( INT Index )
{
	guard(UUTVPackageMap::IndexToObjectName);
	if( Index>=0 )
	{
		for( INT i=0; i<List.Num(); i++ )
		{
			FPackageInfo& Info = List(i);
			if( Index < Info.ObjectCount )
			{
				const TCHAR *t = *(Info.Linker->ExportMap(Index).ObjectName);
				debugf (TEXT("Actorname: %s"), t);
				//Info.Linker->ExportMap(Index).
				return;
				
				//debugf(TEXT("jao %s"),Info.Linker->ExportMap(Index).ObjectName);
				//return Info.Linker->ExportMap(Index).ObjectName;
			}
			Index -= Info.ObjectCount;
		}
	}
	//return NULL;
	unguard;
}

IMPLEMENT_CLASS(UUTVPackageMap);

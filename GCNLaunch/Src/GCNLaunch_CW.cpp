// This has the template specializations, only needed for CodeWarrior.

#if __MWERKS__

#include "UnCon.h"
#include "UnRender.h"
#include "UnNet.h"
#include "Engine.h"
#include "UnFractal.h"
#include "Audio.h"
#include "FFileManagerArc.h"

FArchive& operator<<( FArchive& Ar, TMapBase<UObject*,INT>::TPair& F )
{
	guardSlow(TMapBase::TPair<<);
	return Ar << F.Key << F.Value;
	unguardSlow;
}
FArchive& operator<<( FArchive& Ar, TMapBase<UObject*,FClassNetCache*>::TPair& F )
{
	guardSlow(TMapBase::TPair<<);
	return Ar << F.Key << F.Value;
	unguardSlow;
}
FArchive& operator<<( FArchive& Ar, TMapBase<FString,FString>::TPair& F )
{
	guardSlow(TMapBase::TPair<<);
	return Ar << F.Key << F.Value;
	unguardSlow;
}
FArchive& operator<<( FArchive& Ar, TMapBase<char,char>::TPair& F )
{
	guardSlow(TMapBase::TPair<<);
	return Ar << F.Key << F.Value;
	unguardSlow;
}

FArchive& operator<<( FArchive& Ar, TArray<TMapBase<FString,FString>::TPair>& A ) 
{ 
	A.CountBytes( Ar ); 
	if( sizeof(TMapBase<FString,FString>::TPair)==1 ) 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		if( Ar.IsLoading() ) 
		{ 
			A.ArrayMax = A.ArrayNum; 
			A.Realloc( sizeof(TMapBase<FString,FString>::TPair) ); 
		} 
		Ar.Serialize( &A(0), A.Num() ); 
	} 
	else if( Ar.IsLoading() ) 
	{ 
		INT NewNum; 
		Ar << AR_INDEX(NewNum); 
		A.Empty( NewNum ); 
		for( INT i=0; i<NewNum; i++ ) 
			Ar << *new(A)TMapBase<FString,FString>::TPair; 
	} 
	else 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		for( INT i=0; i<A.ArrayNum; i++ ) 
			Ar << A(i); 
	} 
	return Ar; 
}

FArchive& operator<<( FArchive& Ar, TArray<TMapBase<UObject*,INT>::TPair>& A ) 
{ 
	A.CountBytes( Ar ); 
	if( sizeof(TMapBase<UObject*,INT>::TPair)==1 ) 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		if( Ar.IsLoading() ) 
		{ 
			A.ArrayMax = A.ArrayNum; 
			A.Realloc( sizeof(TMapBase<UObject*,INT>::TPair) ); 
		} 
		Ar.Serialize( &A(0), A.Num() ); 
	} 
	else if( Ar.IsLoading() ) 
	{ 
		INT NewNum; 
		Ar << AR_INDEX(NewNum); 
		A.Empty( NewNum ); 
		for( INT i=0; i<NewNum; i++ ) 
			Ar << *new(A)TMapBase<UObject*,INT>::TPair; 
	} 
	else 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		for( INT i=0; i<A.ArrayNum; i++ ) 
			Ar << A(i); 
	} 
	return Ar; 
}

FArchive& operator<<( FArchive& Ar, TArray<TMapBase<UObject*,FClassNetCache*>::TPair>& A ) 
{ 
	A.CountBytes( Ar ); 
	if( sizeof(TMapBase<UObject*,FClassNetCache*>::TPair)==1 ) 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		if( Ar.IsLoading() ) 
		{ 
			A.ArrayMax = A.ArrayNum; 
			A.Realloc( sizeof(TMapBase<UObject*,FClassNetCache*>::TPair) ); 
		} 
		Ar.Serialize( &A(0), A.Num() ); 
	} 
	else if( Ar.IsLoading() ) 
	{ 
		INT NewNum; 
		Ar << AR_INDEX(NewNum); 
		A.Empty( NewNum ); 
		for( INT i=0; i<NewNum; i++ ) 
			Ar << *new(A)TMapBase<UObject*,FClassNetCache*>::TPair; 
	} 
	else 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		for( INT i=0; i<A.ArrayNum; i++ ) 
			Ar << A(i); 
	} 
	return Ar; 
}

FArchive& operator<<( FArchive& Ar, TArray<TMapBase<char,char>::TPair>& A ) 
{ 
	A.CountBytes( Ar ); 
	if( sizeof(TMapBase<char,char>::TPair)==1 ) 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		if( Ar.IsLoading() ) 
		{ 
			A.ArrayMax = A.ArrayNum; 
			A.Realloc( sizeof(TMapBase<char,char>::TPair) ); 
		} 
		Ar.Serialize( &A(0), A.Num() ); 
	} 
	else if( Ar.IsLoading() ) 
	{ 
		INT NewNum; 
		Ar << AR_INDEX(NewNum); 
		A.Empty( NewNum ); 
		for( INT i=0; i<NewNum; i++ ) 
			Ar << *new(A)TMapBase<char,char>::TPair; 
	} 
	else 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		for( INT i=0; i<A.ArrayNum; i++ ) 
			Ar << A(i); 
	} 
	return Ar; 
}

INSTANTIATE_TEMPLATE(FName);
INSTANTIATE_TEMPLATE(FDependency);
INSTANTIATE_TEMPLATE(FColor);
INSTANTIATE_TEMPLATE(BYTE);
INSTANTIATE_TEMPLATE(UNetConnection*);
INSTANTIATE_TEMPLATE(FPackageInfo);
INSTANTIATE_TEMPLATE(FMeshVert);
INSTANTIATE_TEMPLATE(FMeshTri);
INSTANTIATE_TEMPLATE(UTexture*);
INSTANTIATE_TEMPLATE(_WORD);
INSTANTIATE_TEMPLATE(FMeshFace);
INSTANTIATE_TEMPLATE(FMeshWedge);
INSTANTIATE_TEMPLATE(FMeshMaterial);
INSTANTIATE_TEMPLATE(MeshAnimChannel);
INSTANTIATE_TEMPLATE(MeshBoneScaler);
INSTANTIATE_TEMPLATE(MeshBoneDirector);
INSTANTIATE_TEMPLATE(FAnimMeshVertex);
INSTANTIATE_TEMPLATE(FAnimMeshSection);
INSTANTIATE_TEMPLATE(FQuat);
INSTANTIATE_TEMPLATE(FVector);
INSTANTIATE_TEMPLATE(FMeshBone);
INSTANTIATE_TEMPLATE(VBoneInfIndex);
INSTANTIATE_TEMPLATE(VBoneInfluence);
INSTANTIATE_TEMPLATE(FCoords);
INSTANTIATE_TEMPLATE(VertInfIndex);
INSTANTIATE_TEMPLATE(INT);
INSTANTIATE_TEMPLATE(FMeshSection);
//INSTANTIATE_TEMPLATE(UObject*);
INSTANTIATE_TEMPLATE(AActor*);
INSTANTIATE_TEMPLATE(FString);
INSTANTIATE_TEMPLATE(UTerrainSector*);
INSTANTIATE_TEMPLATE(FTerrainNormalPair);
INSTANTIATE_TEMPLATE(FPlane);
INSTANTIATE_TEMPLATE(FSelectedTerrainVertex);
INSTANTIATE_TEMPLATE(AProjector*);
INSTANTIATE_TEMPLATE(DWORD);
INSTANTIATE_TEMPLATE(FTerrainSectorLayerInfo);
INSTANTIATE_TEMPLATE(FTerrainSectorLightInfo);
INSTANTIATE_TEMPLATE(FReachSpec);
INSTANTIATE_TEMPLATE(FLOAT);
INSTANTIATE_TEMPLATE(FMeshAnimSeq);
INSTANTIATE_TEMPLATE(FMeshNorm);
INSTANTIATE_TEMPLATE(FBox);
INSTANTIATE_TEMPLATE(FSphere);
INSTANTIATE_TEMPLATE(FArchiveItem);
INSTANTIATE_TEMPLATE(FPoly);
INSTANTIATE_TEMPLATE(FFontPage);
INSTANTIATE_TEMPLATE(FBspNode);
INSTANTIATE_TEMPLATE(FBspSurf);
INSTANTIATE_TEMPLATE(FVert);
INSTANTIATE_TEMPLATE(FLightMapIndex);
INSTANTIATE_TEMPLATE(FFontCharacter);
INSTANTIATE_TEMPLATE(FDecal);
INSTANTIATE_TEMPLATE(FLightBitmap);
INSTANTIATE_TEMPLATE(FLeaf);
INSTANTIATE_TEMPLATE(FBspVertex);
INSTANTIATE_TEMPLATE(FBspVertexStream);
INSTANTIATE_TEMPLATE(FColorHack);
INSTANTIATE_TEMPLATE(FUV2Data);
INSTANTIATE_TEMPLATE(FPosNormTexData);
INSTANTIATE_TEMPLATE(FUntransformedVertex);
INSTANTIATE_TEMPLATE(FSkinVertex);
INSTANTIATE_TEMPLATE(FNamedBone);
INSTANTIATE_TEMPLATE(FStaticMeshSection);
INSTANTIATE_TEMPLATE(FStaticMeshLightInfo);
INSTANTIATE_TEMPLATE(FStaticMeshCollisionPrimitive);
INSTANTIATE_TEMPLATE(FStaticMeshTriangle);
INSTANTIATE_TEMPLATE(FRawColorStream);
INSTANTIATE_TEMPLATE(UViewport*);
INSTANTIATE_TEMPLATE(FSpark);
INSTANTIATE_TEMPLATE(FMeshAnimNotify);
INSTANTIATE_TEMPLATE(FStaticMeshVertex);
INSTANTIATE_TEMPLATE(AnalogTrack);
INSTANTIATE_TEMPLATE(MotionChunk);

FArchive& operator<<( FArchive& Ar, TArray<UObject*>& A ) 
{ 
	A.CountBytes( Ar ); 
	if( sizeof(UObject*)==1 ) 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		if( Ar.IsLoading() ) 
		{ 
			A.ArrayMax = A.ArrayNum; 
			A.Realloc( sizeof(UObject*) ); 
		} 
		Ar.Serialize( &A(0), A.Num() ); 
	} 
	else if( Ar.IsLoading() ) 
	{ 
		INT NewNum; 
		Ar << AR_INDEX(NewNum); 
		A.Empty( NewNum ); 
		for( INT i=0; i<NewNum; i++ ) 
			Ar << *new(A)UObject*; 
	} 
	else 
	{ 
		Ar << AR_INDEX(A.ArrayNum); 
		for( INT i=0; i<A.ArrayNum; i++ ) 
			Ar << A(i); 
	} 
	return Ar; 
}
#endif


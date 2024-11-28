//----------------------------------------------------------------------
// Implementation of abstract base class SerializedDataLoader.
// Also contains implementations of concrete derived classes; these
// might be split off into their own files later.
//----------------------------------------------------------------------

#include "SerializedDataLoader.h"
#include "SerializedDataTypes.h"


// N.B. Normally this is a precompiled header, but using the precompiled
// version is causing conflicts with the iostreams headers.
// TODO: Sort out and fix the conflicts!
#include "UnrealEd.h"

#include "UnrealEdImport.h"

#include <assert.h>


// A convienent local shorthand...
namespace SDT = SerializedDataTypes;



//----------------------------------------------------------------------
//----------------------------------------------------------------------
SerializedDataLoader::SerializedDataLoader( const char * buffer, int bufferSize )
		: d_serializedData( buffer ),
		  d_pGet( buffer ),
		  d_pEnd( buffer + bufferSize ),
		  d_bufferSize( bufferSize ),
		  d_chunkHeader( new SDT::ChunkHeader )
{
	if( 0 == d_chunkHeader )
	{
		throw "SerializedDataLoader: new failed on chunkHeader";
	}
}


SerializedDataLoader::~SerializedDataLoader()
{
	delete  d_chunkHeader;
}



void  SerializedDataLoader::setPackageName( const char * packageName )
{
	d_packageName = packageName;
}



void  SerializedDataLoader::setObjName( const char * objName )
{
	d_objName = objName;
}



bool  SerializedDataLoader::f_readChunkHeader( const int id )
{
	bool  isOkay = f_readRaw( d_chunkHeader, sizeof(*d_chunkHeader) );

	if( id >= 0 )
	{
		assert( id == d_chunkHeader->id );
		isOkay = (id == d_chunkHeader->id);
	}

	return( isOkay );
}



bool  SerializedDataLoader::f_skipChunk()
{
	int  delta = d_chunkHeader->dataSize * d_chunkHeader->dataCount;

	bool  isOkay = (d_pGet + delta) <= d_pEnd;
	
	if( isOkay )
	{
		d_pGet += delta;
	}

	return( isOkay );
}


bool  SerializedDataLoader::f_readRaw( void * dst, int byteCount )
{
	bool  isOkay = (d_pGet + byteCount) <= d_pEnd;

	if( isOkay )
	{
		appMemcpy( dst, d_pGet, byteCount );
		d_pGet += byteCount;
	}

	return( isOkay );
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//----------------------------------------------------------------------

SerializedMeshLoader::SerializedMeshLoader( const char * buffer, int bufferSize )
		: SerializedDataLoader( buffer, bufferSize ),
		  d_groupName( 0 )
{
}

SerializedMeshLoader::~SerializedMeshLoader()
{
}


void  SerializedMeshLoader::setGroupName( const char * name )
{
	d_groupName = name;
}


bool  SerializedMeshLoader::load( )
{
	UnEditor::FMesh_1  mesh;

	bool  isOkay = this->f_getMaterials( &mesh );

	if( isOkay )
	{
		isOkay = this->f_getPoints( &mesh );
	}

	if( isOkay )
	{
		isOkay = this->f_getNormals( &mesh );
	}

	if( isOkay )
	{
		isOkay = this->f_getUVs( &mesh );
	}

	if( isOkay )
	{
		isOkay = this->f_getFaces( &mesh );
	}

	if( isOkay )
	{
		isOkay = this->f_getCollision( &mesh );
	}


	if( isOkay )
	{
		::LoadMeshWithNormals( d_packageName, d_groupName, d_objName, &mesh );

		::FinishMeshImport();
	}

	return( isOkay );
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedMeshLoader::f_getMaterials( UnEditor::FMesh_1 * mesh )
{
	bool  isOkay = f_readChunkHeader( SDT::kStaticMeshMaterial );

	if( isOkay )
	{
		mesh->materials.SetSize( d_chunkHeader->dataCount );

		for( int i = 0; i < mesh->materials.Num(); ++i )
		{
			isOkay = f_readRaw( &mesh->materials( i ), d_chunkHeader->dataSize );

			if( ! isOkay )
			{
				break;
			}
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedMeshLoader::f_getPoints( UnEditor::FMesh_1 * mesh )
{
	bool  isOkay = f_readChunkHeader( SDT::kPoint );

	if( isOkay )
	{
		mesh->points.SetSize( d_chunkHeader->dataCount );

		for( int i = 0; i < mesh->points.Num(); ++i )
		{
			isOkay = f_readRaw( &mesh->points( i ), d_chunkHeader->dataSize );

			if( ! isOkay )
			{
				break;
			}
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedMeshLoader::f_getNormals( UnEditor::FMesh_1 * mesh )
{
	bool  isOkay = f_readChunkHeader( SDT::kNormal );

	if( isOkay )
	{
		mesh->normals.SetSize( d_chunkHeader->dataCount );

		for( int i = 0; i < mesh->normals.Num(); ++i )
		{
			isOkay = f_readRaw( &mesh->normals( i ), d_chunkHeader->dataSize );

			if( ! isOkay )
			{
				break;
			}
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedMeshLoader::f_getUVs( UnEditor::FMesh_1 * mesh )
{
	bool  isOkay = f_readChunkHeader( SDT::kUV );

	if( isOkay )
	{
		mesh->uvs.SetSize( d_chunkHeader->dataCount );

		for( int i = 0; i < mesh->uvs.Num(); ++i )
		{
			isOkay = f_readRaw( &mesh->uvs( i ), d_chunkHeader->dataSize );

			if( ! isOkay )
			{
				break;
			}
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedMeshLoader::f_getFaces( UnEditor::FMesh_1 * mesh )
{
	bool  isOkay = f_readChunkHeader( SDT::kStaticMeshTri );

	if( isOkay )
	{
		mesh->faces.SetSize( d_chunkHeader->dataCount );

		for( int i = 0; i < mesh->faces.Num(); ++i )
		{
			isOkay = f_readRaw( &mesh->faces( i ), d_chunkHeader->dataSize );

			if( ! isOkay )
			{
				break;
			}
		}
	}

	return( isOkay );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedMeshLoader::f_getCollision( UnEditor::FMesh_1 * mesh )
{
	bool  isOkay = f_readChunkHeader( SDT::kCollisionTri );

	if( isOkay )
	{
		mesh->collision.SetSize( d_chunkHeader->dataCount );

		for( int i = 0; i < mesh->collision.Num(); ++i )
		{
			isOkay = f_readRaw( &mesh->collision( i ), d_chunkHeader->dataSize );

			if( ! isOkay )
			{
				break;
			}
		}
	}

	return( isOkay );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

SerializedSkinLoader::SerializedSkinLoader( const char * buffer, int bufferSize )
		: SerializedDataLoader( buffer, bufferSize ),
		  d_scale( 1.0f )
{
}

SerializedSkinLoader::~SerializedSkinLoader()
{
}


void  SerializedSkinLoader::setScale( float scale )
{
	d_scale = scale;
}


bool  SerializedSkinLoader::load( )
{
	UnEditor::VSkin  skin;

	bool  isOkay = this->f_getMaterials( &skin );

	if( isOkay )
	{
		isOkay = this->f_getPoints( &skin );
	}

	if( isOkay )
	{
		isOkay = this->f_getWedges( &skin );
	}

	if( isOkay )
	{
		isOkay = this->f_getFaces( &skin );
	}

	if( isOkay )
	{
		isOkay = this->f_getRefBones( &skin );
	}

	if( isOkay )
	{
		isOkay = this->f_getRawWeights( &skin );
	}

	if( isOkay )
	{
		::LoadSkin( d_packageName, d_objName, &skin, d_scale );

		::FinishAnimImport();	
	}

	return( isOkay );
}




//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedSkinLoader::f_getMaterials( UnEditor::VSkin * skin )
{
	bool  isOkay = f_readChunkHeader( SDT::kWeightedMeshMaterial );

	skin->Materials.SetSize( d_chunkHeader->dataCount );

	for( int i = 0; i < skin->Materials.Num(); ++i )
	{
		isOkay = f_readRaw( &skin->Materials( i ), d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedSkinLoader::f_getPoints( UnEditor::VSkin * skin )
{
	bool  isOkay = f_readChunkHeader( SDT::kPoint );

	skin->Points.SetSize( d_chunkHeader->dataCount );

	for( int i = 0; i < skin->Points.Num(); ++i )
	{
		isOkay = f_readRaw( &skin->Points( i ), d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedSkinLoader::f_getWedges( UnEditor::VSkin * skin )
{
	bool  isOkay = f_readChunkHeader( SDT::kWedge );

	skin->Wedges.SetSize( d_chunkHeader->dataCount );

	for( int i = 0; i < skin->Wedges.Num(); ++i )
	{
		isOkay = f_readRaw( &skin->Wedges( i ), d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedSkinLoader::f_getFaces( UnEditor::VSkin * skin )
{
	bool  isOkay = f_readChunkHeader( SDT::kWeightedMeshTri );

	skin->Faces.SetSize( d_chunkHeader->dataCount );

	for( int i = 0; i < skin->Faces.Num(); ++i )
	{
		isOkay = f_readRaw( &skin->Faces( i ), d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedSkinLoader::f_getRefBones( UnEditor::VSkin * skin )
{
	bool  isOkay = f_readChunkHeader( SDT::kBone );

	skin->RefBones.SetSize( d_chunkHeader->dataCount );

	for( int i = 0; i < skin->RefBones.Num(); ++i )
	{
		isOkay = f_readRaw( &skin->RefBones( i ), d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedSkinLoader::f_getRawWeights( UnEditor::VSkin * skin )
{
	bool  isOkay = f_readChunkHeader( SDT::kWeight );

	skin->RawWeights.SetSize( d_chunkHeader->dataCount );

	for( int i = 0; i < skin->RawWeights.Num(); ++i )
	{
		isOkay = f_readRaw( &skin->RawWeights( i ), d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//----------------------------------------------------------------------

SerializedAnimationLoader::SerializedAnimationLoader( const char * buffer, int bufferSize )
		: SerializedDataLoader( buffer, bufferSize )
{
}

SerializedAnimationLoader::~SerializedAnimationLoader()
{
}




//----------------------------------------------------------------------
//----------------------------------------------------------------------

bool  SerializedAnimationLoader::load( )
{
	UnEditor::VSkin           skin;
	UnEditor::VAnimationList  animList;
	animList.AnimList = new TArray<UnEditor::VAnimation>();

	bool  mergeAnims = false;
	bool  isOkay = this->f_getLoadBehavior( mergeAnims );

	if( isOkay )
	{
		isOkay = this->f_getSkeleton( &skin );
	}

	if( isOkay )
	{
		isOkay = this->f_getAnimation( &animList );
	}


	if( isOkay )
	{
		// N.B. We're taking advantage of knowing the internals of LoadAnimations()
		// in how we set up the skin.  Specifically, LoadAnimations() only uses the
		// RefBones field of the VSkin; the rest is ignored.  Presumably it was set
		// up this way to correlate with the LoadSkin() method.
		// 
		::LoadAnimations( d_packageName, d_objName, &skin, &animList, mergeAnims );

		::FinishAnimImport();
	}

	return( isOkay );
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------

bool  SerializedAnimationLoader::f_getLoadBehavior( bool & mergeFlag )
{
	bool  isOkay = f_readChunkHeader( SDT::kAnimFlags );

	if( isOkay )
	{
		SDT::AnimLoadBehavior  behavior;

		isOkay = f_readRaw( &behavior, d_chunkHeader->dataSize );

		mergeFlag = (SDT::AnimLoadBehavior::kMerge == behavior.mode);
	}

	return( isOkay );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

bool  SerializedAnimationLoader::f_getSkeleton( UnEditor::VSkin * skin )
{
	bool  isOkay = f_readChunkHeader( SDT::kBone );

	skin->RefBones.SetSize( d_chunkHeader->dataCount );

	for( int i = 0; i < skin->RefBones.Num(); ++i )
	{
		isOkay = f_readRaw( &skin->RefBones( i ), d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool  SerializedAnimationLoader::f_getAnimation( UnEditor::VAnimationList * animList )
{
	bool  isOkay = f_getAnimInfo( animList );

	if( isOkay )
	{
		isOkay = f_getAnimKeys( animList );
	}

	return( isOkay );
}

bool  SerializedAnimationLoader::f_getAnimInfo( UnEditor::VAnimationList * animList )
{
	if (NULL == animList || NULL == animList->AnimList)
		return false;

	bool  isOkay = f_readChunkHeader( SDT::kAnimInfo );

	// THIS IS A HACK - FArray is not calling VAnimation constructor
	animList->AnimList->AddZeroed( d_chunkHeader->dataCount );

	for( int i = 0; i < animList->AnimList->Num(); ++i )
	{
		isOkay = f_readRaw( &animList->AnimList->operator()( i ).AnimInfo, d_chunkHeader->dataSize );

		if( ! isOkay )
		{
			break;
		}
	}

	return( isOkay );
}


bool  SerializedAnimationLoader::f_getAnimKeys( UnEditor::VAnimationList * animList )
{
	if (NULL == animList || NULL == animList->AnimList)
		return false;

	bool  isOkay = f_readChunkHeader( SDT::kQuatAnimKey );

	for( int i = 0; isOkay && i < animList->AnimList->Num(); ++i )
	{
		UnEditor::VAnimation& animI = animList->AnimList->operator()( i );
		int iNumKeys = animI.AnimInfo.NumRawFrames * animI.AnimInfo.TotalBones;
		animI.KeyTrack.SetSize( iNumKeys );
		for (int k = 0; isOkay && k < iNumKeys; ++k)
		{
			isOkay = f_readRaw( &animList->AnimList->operator()( i ).KeyTrack( k ), d_chunkHeader->dataSize );
		}
	}

	return( isOkay );
}


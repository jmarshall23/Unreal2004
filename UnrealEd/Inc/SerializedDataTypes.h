//----------------------------------------------------------------------
// SerializedDataTypes.h
//
// Declarations for the layout of serialized data transmitted via the
// UnrealEdServerInterface object.
//
// The serialized data is in buffers transmitted using a COM interface,
// so we need an encoding/decoding scheme that is seen by both the
// client and the server.   One objective of doing it this way is to
// decouple the client from the implementation details of the UnrealEd
// server, so we define the serialzed structures here rather than using
// those defined in the common Unreal headers; if we were to use 
// Unreal headers directly, they would create a much heavier coupling.
//
// What we have here is a set of struct declarations that have the same
// memory layout as elements internal to the UnrealEd implementation.
// They can be composited, with the aid of a header, into chunks and
// sequences of chunks that are readily extracted and copied to Unreal
// structures. 
//
// Now one thing to keep in mind is that these streams of serialized
// data are in memory allocated by COM, but must be copied to memory
// allocated by Unreal.  We endevor to layout the stream in such a way
// as to make the copy cheap, but having isomorphic layouts is not
// strictly necessary; should the internal Unreal structures change,
// the the SerializedDataReader classes can adapt to them without 
// affecting these streams--provided, of course, that no additional 
// content is required.
//
// ---[ Regarding chunks ]---
//
// A chunk is an array of like elements.  In the simple case, 
// elements are of uniform size.  However, you can also have elements 
// of like types, but with different sizes; e.g. an animation can have 
// an array of key-lists (one per bone), but with a different number
// of keys in each list.  We handle this by creating chunks with other
// chunks for their elements.
//
// A chunk is introduced by a ChunkHeader.  The header has two 
// essential fields: dataSize, and dataCount.  dataSize is the size,
// in bytes, per element; dataCount is the number of elements in 
// the chunk.  In the case where the elements are not of uniform
// size, dataSize should be set to zero.  There are also two other
// fields that are present for convenience: name, and id.  Name is
// text, id is an int; they are intended as debugging aids in case
// you need to examine buffers.
//
// ---[ Regarding encoding ]---
//
// There is nothing in this scheme to interpret encoding at runtime;
// it is simply handled by contract.  The client and server programs
// (and therfore programmers) must agree on the ordering.
//
//----------------------------------------------------------------------

#pragma once

namespace SerializedDataTypes {


//----------------------------------------------------------------------
// This is a cheap-ass string that has a fixed buffer of 64 elements.
// It is suitable for replacing struct fields that are char[64].
//----------------------------------------------------------------------

struct NameBuffer
{
	// The data...
	enum { kBufferSize = 64 };
	char		buffer[ kBufferSize ];

	// C'tors
	NameBuffer()
	{
		buffer[ 0 ] = '\0';
	}

	NameBuffer( const NameBuffer & other )
	{
		assign( &other.buffer[0] );
	}

	NameBuffer( const char * n )
	{
		assign( n );
	}

	// Assignment
	NameBuffer & operator=( const NameBuffer & rhs )
	{
		assign( &rhs.buffer[0] );
		return( *this );
	}

	NameBuffer & operator=( const char * n )
	{
		assign( n );
		return( *this );
	}

	// Subscripting
	char &		operator[]( int i )       { return( buffer[ i ] ); }
	const char &operator[]( int i ) const { return( buffer[ i ] ); }

	// The underlying assignment mechanism
	void  assign( const char * n )
	{
		if( 0 == n )
		{
			buffer[ 0 ] = '\0';
		}
		else
		{
			for( unsigned i = 0; i < kBufferSize; ++i )
			{
				buffer[ i ] = n[ i ];
				if( '\0' == n[ i ] )
					break;
			}
			buffer[ kBufferSize - 1 ] = '\0';	// probably redundant, but safe
		}
	}
};


//----------------------------------------------------------------------
// ChunkHeader introduces a "chunk", an array of similar data elements.  
// If elements are of fixed size, then dataSize is the number of bytes 
// in an element; if dataSize is zero, the elements vary in length,
// even though logically they are the same type.  This allows structs
// with dynamic buffers to be serialized.
//
// ChunkType is an id tag for chunks.
//----------------------------------------------------------------------

enum  ChunkType
{
	kUndefinedChunkType,
	kPoint,
	kNormal,
	kUV,
	kWedge,
	kBone,
	kWeight,
	kAnimFlags,
	kAnimInfo,
	kQuatAnimKey,

	kStaticMeshMaterial,
	kStaticMeshTri,
	kCollisionTri,

	kWeightedMeshMaterial,
	kWeightedMeshTri,

	kSkeleton,
	kAnimationSequence
};


struct ChunkHeader
{
	NameBuffer	name;
	ChunkType	id;
	int			dataSize;
	int			dataCount;
};



//----------------------------------------------------------------------
// Layout for SerializedMeshLoader:
//
//	chunk Materials:	array of Material;
//	chunk Points:		array of Vector3;
//	chunk Normals:		array of Vector3;
//	chunk UVs:			array of UvCoord;
//	chunk Faces:		array of Triangle;
//	chunk Collision:	array of CollisionTriangle;
//----------------------------------------------------------------------

// Cloned from UnEditor::FMaterial
struct StaticMeshMaterial
{
	NameBuffer	materialName;
	int         textureIndex;     // multi/sub texture index 

	StaticMeshMaterial( const char * name, int index ) 
		: materialName( name ), textureIndex( index )
		{}
};


// Cloned from UnEditor::FUV
struct UvCoord
{
	float	u, v;

	UvCoord() : u( 0.0f ), v( 0.0f ) {}
	UvCoord( float uu, float vv ) : u( uu ), v( vv ) {}
};


// Cloned from FVector3
struct Vector3
{
	float	x, y, z;

	Vector3() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {}
	Vector3( float xx, float yy, float zz ) : x( xx ), y( yy ), z( zz ) {}
};


// Cloned from FVertex
struct Vertex
{
	int		positionIndex;
	int		normalIndex;
	int		uvIndex;

	Vertex( int p = 0, int n = 0, int uv = 0 )
		: positionIndex( p ), normalIndex( n ), uvIndex( uv ) {}
};


// Cloned from FQuaternion
struct Quaternion
{
	float	x, y, z, w;

	Quaternion() : x( 0.0f ), y( 0.0f), z( 0.0f ), w( 0.0f ) {}
	Quaternion( float xx, float yy, float zz, float ww ) 
		: x( xx ), y( yy), z( zz ), w( ww ) {}
};


// Cloned from UnEditor::FTriangle
struct StaticMeshTri
{
	int		points[3];
	int		texCoords[3];
	int		materialIndex;

	StaticMeshTri( int p0, int p1, int p2, int t0, int t1, int t2, int m ) 
		: materialIndex( m )
		{
			points[0] = p0;
			points[1] = p1;
			points[2] = p2;
			texCoords[0] = t0;
			texCoords[1] = t1;
			texCoords[2] = t2;
		}
};


// Cloned from UnEditor::FTriangle_1
struct StaticMeshTri_1
{
	Vertex	vertex[ 3 ];
	int		materialIndex;

	StaticMeshTri_1() : materialIndex( 0 ) {}
	StaticMeshTri_1( Vertex v0, Vertex v1, Vertex v2, int mIdx )
		: materialIndex( mIdx )
		{
			vertex[ 0 ] = v0;
			vertex[ 1 ] = v1;
			vertex[ 2 ] = v2;
		}
};


struct StaticMeshCollisionTri
{
	int		points[3];

	StaticMeshCollisionTri()
	{
		points[ 0 ] = 0;
		points[ 1 ] = 0;
		points[ 2 ] = 0;
	}
	StaticMeshCollisionTri( int p0, int p1, int p2 )
	{
		points[ 0 ] = p0;
		points[ 1 ] = p1;
		points[ 2 ] = p2;
	}
};



//----------------------------------------------------------------------
// Layout for SerializedSkinLoader:
//
//	chunk Materials:	array of VMaterial;
//	chunk Points:		array of Vector3;
//	chunk Wedges:		array of Wedge;
//	chunk Faces:		array of SkinTri;
//	chunk RefBones:		array of Bone;
//	chunk RawWeights:	array of Weights;
//----------------------------------------------------------------------

// Cloned from struct VMaterial
struct WeightedMeshMaterial
{
	NameBuffer	materialName;
	int			textureIndex;  // texture index ('multiskin index')
	long		polyFlags;     // ALL poly's with THIS material will have this flag.
	int			auxMaterial;   // reserved: index into another material, eg. detailtexture/shininess/whatever.
	long		auxFlags;      // reserved: auxiliary flags 
	int			lodBias;       // material-specific lod bias
	int			lodStyle;      // material-specific lod style

	WeightedMeshMaterial()
		: materialName( 0 ), textureIndex( 0 ), polyFlags( 0 ), auxMaterial( 0 ), 
		  auxFlags( 0 ), lodBias( 0 ), lodStyle( 0 )
	{}

	WeightedMeshMaterial( const char * name, int index )
		: materialName( name ), textureIndex( index ), polyFlags( 0 ), auxMaterial( 0 ), 
		  auxFlags( 0 ), lodBias( 0 ), lodStyle( 0 )
	{}

	
};


// Cloned from VTriangle
struct WeightedMeshTri
{
	unsigned short	wedgeIndex[3];	 // Point to three vertices in the vertex list.
	unsigned char	matIndex;	     // Materials can be anything.
	unsigned char	auxMatIndex;     // Second material from exporter (unused)
	unsigned long	smoothingGroups; // 32-bit flag for smoothing groups.

	WeightedMeshTri()
		: matIndex( 0 ), auxMatIndex( 0 ), smoothingGroups( 0 )
		{
			wedgeIndex[0] = 0;
			wedgeIndex[1] = 0;
			wedgeIndex[2] = 0;
		}
};


// Cloned from VJointPos
struct JointPos
{
	Quaternion	orientation;  //
	Vector3		position;     //  needed or not ?

	float		length;       //  For collision testing / debugging drawing...
	float		xSize;
	float		ySize;
	float		zSize;

	JointPos() 
		: orientation(), position(), length( 0.0f ), xSize( 0.0f ), ySize( 0.0f ), zSize( 0.0f )
		{}
};


// Cloned from VBone
struct Bone
{
	NameBuffer		name;
	unsigned long	flags;        // reserved / 0x02 = bone where skin is to be attached...	
	int 			numChildren;  // children  // only needed in animation ?
	int			    parentIndex;  // 0/NULL if this is the root bone.  
	JointPos		bonePos;      // reference position

	Bone() : name(), flags( 0 ), numChildren( 0 ), parentIndex( 0 ), bonePos() {}
};


// Cloned from VRawBoneInfluence
struct Weight
{
	float	weight;
	int		pointIndex;
	int		boneIndex;

	Weight() : weight( 0.0f ), pointIndex( 0 ), boneIndex( 0 ) {}
	Weight( float w, int pIdx, int bIdx ) : weight( w ), pointIndex( pIdx ), boneIndex( bIdx ) {}
};


// Cloned from VVertex
struct Wedge
{
	int		pointIndex;
	float	u, v;
	int		materialIndex;

	Wedge() : pointIndex( 0 ), u( 0.0f ), v( 0.0f ), materialIndex( 0 ) {}
	Wedge( int pIx, float uu, float vv, int mIx ) : pointIndex( pIx ), u( uu ), v( vv ), materialIndex( mIx ) {}
};


//----------------------------------------------------------------------
// Layout for SerializedAniamtionLoader:
//
//	chunk Skeleton:			array of Bone;
//	chunk AnimationList:	array of AnimationSequence;
//
// AnimatinSequence:
//
//	chunk AnimLoadBehavior:	AnimLoadBehavior;
//	chunk Animation:		AnimInfo;
//	chunk AnimKeys:			array of AnimKey;
//----------------------------------------------------------------------


// This is not part of the animation data per se; rather, it is used
// to control how the data is loaded into the package.
struct AnimLoadBehavior
{
	enum Mode { kOverwrite, kMerge };
	Mode	mode;
};


// Cloned from AnimInfoBinary
struct AnimInfo
{
	NameBuffer	name;				// Animation's name
	NameBuffer	group;				// Animation's group name	

	int		totalBones;				// TotalBones * NumRawFrames is number of animation keys to digest.

	int		rootInclude;			// 0 none 1 included 		
	int		keyCompressionStyle;	// Reserved: variants in tradeoffs for compression.
	int		keyQuotum;				// Max key quotum for compression	
	float	keyReduction;			// desired 
	float	trackTime;				// explicit - can be overridden by the animation rate
	float	animRate;				// frames per second.
	int		startBone;				// - Reserved: for partial animations.
	int		firstRawFrame;			//
	int		numRawFrames;			// NumRawFrames and AnimRate dictate tracktime...

	AnimInfo() 
		: name(), group(), totalBones( 0 ), rootInclude( 0 ), keyCompressionStyle( 0 ),
		  keyQuotum( 0 ), keyReduction( 0 ), trackTime( 0 ), animRate( 0 ), startBone( 0 ),
		  firstRawFrame( 0 ), numRawFrames( 0 )
		{}
};


// Cloned from AnimInfoBinary
struct AnimKey
{
	Vector3		position;           // relative to parent.
	Quaternion	orientation;        // relative to parent.
	float		time;				// The duration until the next key (end key wraps to first...)

	AnimKey() : position(), orientation(), time( 0.0f ) {}
};


} // end namespace SerializedDataTypes



//#pragma once
//----------------------------------------------------------------------
// SerializedDataLoader is an Abstract Base Class for loading 
// serialized data streams.
//
// SerializedDataLoader objects load serialized data from a memory
// buffer into memory allocated by Unreal.  All objects have a name,
// and are loaded into a package, which is specified by a name.
//
// The buffer is a stream of "chunks," each one introduced by a
// UesChunkHeader that mainly serves to indicate the chunk size.
// Chunks are seen as raw binary data that will be interpreted by
// classes derived from this one.
//
// CAVEAT: This is intended to support UnrealEd as a COM server.
// The client must format the chunk data in the layout anticipated
// by the server, and there is insufficient metadata in the stream
// for automated interpretation.  This implies that it is the 
// programmer's responsibility to coordinate the data layout on
// both sides.  Not pretty, but them's the breaks.
//----------------------------------------------------------------------


// Forward declarations
namespace SerializedDataTypes {
	class  ChunkHeader;
}



class SerializedDataLoader
{
public:

	// buffer - the buffer containing serialized data;
	// bufferSize - size of buffer, in bytes.
	SerializedDataLoader( const char * buffer, int bufferSize );
	virtual ~SerializedDataLoader();

	// Loads the buffer into Unreal memory.
	// Returns true on success.
	virtual bool		load() = 0;

	// Set the name of the package that data is loaded into.
	virtual void		setPackageName( const char * packageName );

	// Set the name of the object being loaded.
	virtual void		setObjName( const char * objName );


protected:

	// Read a chunk header from the stream; makes no assumptions
	// that the data is valid.  If you set id >=0, this method will
	// assert() that the value matches the id field of the header.
	// Return true if the raw read succeeds.
	bool				f_readChunkHeader( const int id = -1 );

	// Skip the current chunk; returns true if the stream is good.
	bool				f_skipChunk();

	// Read raw binary from the stream; returns true if the stream is good.
	// dst - buffer into which the data is read;
	// byteCount - number of bytes to read.
	bool				f_readRaw( void * dst, int byteCount );

	//---[ data members ]---
	const char * 		d_packageName;
	const char * 		d_objName;
	const char *	 	d_serializedData;
	const char *		d_pGet;
	const char *		d_pEnd;
	int					d_bufferSize;
	SerializedDataTypes::ChunkHeader *	
						d_chunkHeader;

private:
	// Verboten!
	SerializedDataLoader();
	SerializedDataLoader( const SerializedDataLoader & );
	SerializedDataLoader & operator=( const SerializedDataLoader & );

};



//----------------------------------------------------------------------
//-------------------[ class SerializedMeshLoader ]---------------------
//----------------------------------------------------------------------
// SerializedDataLoader that knows how to load UnEditor::FMesh streams.
//----------------------------------------------------------------------

// Forward declarations
namespace UnEditor {
	class  FMesh_1;
}



class SerializedMeshLoader : public SerializedDataLoader
{
public:

	SerializedMeshLoader( const char * buffer, int bufferSize );
	virtual ~SerializedMeshLoader();

	virtual bool		load();

	// Set the scale applied to this skin.
	void				setGroupName( const char * name );


private:
	// Methods that parse chunks
	bool			f_getMaterials( UnEditor::FMesh_1 * mesh );
	bool			f_getPoints   ( UnEditor::FMesh_1 * mesh );
	bool			f_getNormals  ( UnEditor::FMesh_1 * mesh );
	bool			f_getUVs      ( UnEditor::FMesh_1 * mesh );
	bool			f_getFaces    ( UnEditor::FMesh_1 * mesh );
	bool			f_getCollision( UnEditor::FMesh_1 * mesh );


	//---[ data members ]---
	const char *	d_groupName;
};


//----------------------------------------------------------------------
//-------------------[ class SerializedSkinLoader ]---------------------
//----------------------------------------------------------------------
// SerializedDataLoader that knows how to load UnEditor::VSkin streams.
//----------------------------------------------------------------------

// Forward declarations
namespace UnEditor {
	class  VSkin;
}



class SerializedSkinLoader : public SerializedDataLoader
{
public:

	SerializedSkinLoader( const char * buffer, int bufferSize );
	virtual ~SerializedSkinLoader();

	virtual bool		load();

	// Set the scale applied to this skin.
	void				setScale( float scale );


private:

	// Methods that parse chunks
	bool			f_getMaterials ( UnEditor::VSkin * skin );
	bool			f_getPoints    ( UnEditor::VSkin * skin );
	bool			f_getWedges    ( UnEditor::VSkin * skin );
	bool			f_getFaces     ( UnEditor::VSkin * skin );
	bool			f_getRefBones  ( UnEditor::VSkin * skin );
	bool			f_getRawWeights( UnEditor::VSkin * skin );


	//---[ data members ]---
	float			d_scale;
};



//----------------------------------------------------------------------
//----------------[ class SerializedAnimationLoader ]-------------------
//----------------------------------------------------------------------
// SerializedDataLoader that knows how to load UnEditor::VSkin streams.
//----------------------------------------------------------------------

// Forward declarations
namespace UnEditor {
	class  VSkin;
	class  VAnimationList;
	class  VAnimation;
}


class SerializedAnimationLoader : public SerializedDataLoader
{
public:

	SerializedAnimationLoader( const char * buffer, int bufferSize );
	virtual ~SerializedAnimationLoader();

	virtual bool		load();


private:
	
	// Methods that parse chunks
	bool			f_getLoadBehavior( bool & mergeFlag );
	bool			f_getSkeleton     ( UnEditor::VSkin * skin );
	bool			f_getAnimation    ( UnEditor::VAnimationList * animList );
	bool			f_getAnimInfo     ( UnEditor::VAnimationList * animList );
	bool			f_getAnimKeys     ( UnEditor::VAnimationList * animList );


	//---[ data members ]---
};







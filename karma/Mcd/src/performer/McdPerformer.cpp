/*
  McdPerformer.cpp
*/

#include <stdio.h>
#include <McdFrame.h>
#include <McdModel.h> 
#include <McdTriangleMesh.h> 
#include <McdPerformer.h> 
#include "CxTriangleMesh.h"
#include "McdCheck.h"
#include <MeMessage.h>
#include <MeAssert.h>
#include "lsVec3.h"
#include <Performer/pf/pfDCS.h>
#include <Performer/pf/pfSCS.h>
#include <Performer/pf/pfLOD.h>
#include <Performer/pf/pfGeode.h>
#include <Performer/pr/pfGeoSet.h>

#ifdef MCD_TRACE
/*
 *
 */
static void 
printType( pfGeoSet* gs )
{
  int primType = gs->getPrimType();

  fprintf(stderr, "Type: ");

  switch( primType )
    {
    case PFGS_LINESTRIPS:
      fprintf(stderr, "LINESTRIPS" "\n");
      break;
    case PFGS_FLAT_LINESTRIPS:
      fprintf(stderr, "FLAT_LINESTRIPS" "\n");
      break;
    case PFGS_TRISTRIPS:
      fprintf(stderr, "TriSTRIPS" "\n");
      break;
    case PFGS_FLAT_TRISTRIPS:
      fprintf(stderr, "FALT_TRISTRIPS" "\n");
      break;
    case PFGS_FLAT_TRIFANS:
      fprintf(stderr, "FLAT_TRIFANS" "\n");
      break;
    case PFGS_TRIFANS:
      fprintf(stderr, "TRIFANS" "\n");
      break;
    case PFGS_POLYS:
      fprintf(stderr, "POLYS" "\n");
      break;    
    case PFGS_POINTS: 
      fprintf(stderr, "POINTS" "\n");
      break;          
    case PFGS_LINES: 
      fprintf(stderr, "LINES" "\n");
      break;
    case PFGS_TRIS: 
      fprintf(stderr, "TRIS" "\n");
      break;
    case PFGS_QUADS:
      fprintf(stderr, "QUADS" "\n");
      break;
    }
}
#endif /* MCD_TRACE */

 
//--------------------------------------------------------------------------
/*
 * A simple dynamic array hodling data of type lsVec3
 */
typedef struct McdVec3Array {  
  void    init( int size );     		    // initiliazer
  void    resize( int size );   		    // resize
  int     getSize() const;         		  // return the size
  int     getNumOfElements() const;  	  // return the occupied size
  void    squeeze();                    // shrink the size to minimum
  void    insert( const lsVec3 &v);     // append a value
  lsVec3  getValue( int index ) const;  // return ith value from array
  void    destroy();            		    // delete the content
  McdVec3Array();
  ~McdVec3Array();
 
  lsVec3  *array;               		    // the container
  int     size;                 		    // allocated size
  int     index;                		    // filled size

} McdVec3Array;


//--------------------------------------------------------------------------
typedef struct McdVec3Int {
	int v[3];
} McdVec3Int;

/*
 * A simple dynamic array hodling data of type McdVec3Int
 */
typedef struct McdVec3IntArray {  
  void    init( int size );     		      // initiliazer
  void    resize( int size );   		      // resize 
  int     getNumOfElements() const;  	    // return the occupied size
  void    squeeze();                      // shrink the size to minimum
  void    insert( int i0, int i1, int i2);// insert 3 ints 
  void    destroy();            		      // delete the content
  McdVec3IntArray();
  ~McdVec3IntArray();
 
  McdVec3Int  *array;               		   // the container
  int          size;                 		   // allocated size
  int          index;                		   // filled size

} McdVec3IntArray;


//--------------------------------------------------------------------------
 
static void traverse( pfNode* node, pfMatrix* tm, pfNode* root,
		      McdVec3Array *vertexList, 
		      McdVec3IntArray *triList,
		      McdPerformerNodeCallback acceptNodeCB, void* userCBData);

static void traverseChildren( pfNode* node, pfMatrix* m, pfNode *root,  
                              McdVec3Array *vertexList,
                              McdVec3IntArray *triList,
			      McdPerformerNodeCallback acceptNodeCB, 
			      void* userCBData)
{
  // decide which children to do down:
  if ( node->getType()->isDerivedFrom( pfGroup::getClassType() ) )
  {
    int i;
    for (i=0; i < ((pfGroup*) node)->getNumChildren(); i++ )
      {
	if (acceptNodeCB(root,node,((pfGroup*)node)->getChild(i),userCBData)) {
        traverse( ((pfGroup*)node)->getChild(i), m, root, vertexList, 
		  triList, acceptNodeCB, userCBData );
	}
      }
  }
}

/* forward decl. */
static int addToTriangleMeshFromGeoSet(pfGeoSet* gset, pfMatrix* tm, 
			McdVec3Array *v, McdVec3IntArray* triList);

/*
 *
 */
static void traverse( pfNode* node, pfMatrix* tm, pfNode* root, 
                     McdVec3Array *vertexList, McdVec3IntArray* triList,
		     McdPerformerNodeCallback acceptNodeCB, void* userCBData)
{
  if (node == NULL) return;
  if ( node->getType() == pfGeode::getClassType() ) 
  {
    int i;
    for (i=0; i< ((pfGeode*)node)->getNumGSets(); i++) {
      addToTriangleMeshFromGeoSet( ((pfGeode*)node)->getGSet(i), 
                                    tm, vertexList, triList ); 
    }
    return;
  }

  if (node->getType()->isDerivedFrom(pfSCS::getClassType()))
  {  // careful: DCS is derived from SCS.

    pfMatrix m;  // allocated locally on the stack
    m.copy(*tm); // copy over the accumulated transform
    m.postMult( *(((pfSCS*)node)->getMatPtr()) ); // compound with this node

    // pass down pointer to m down!
    traverseChildren( node, &m, root, vertexList, triList, 
		      acceptNodeCB,userCBData );
  }
  else  // no relative transforms, so reuse same matrix pointer
  {
    traverseChildren( node, tm, root, vertexList, triList, 
		      acceptNodeCB,userCBData );
  }
  
}

/**
 * Default traversal callback. Returns 1 if node accepted, 0 otherwise.
 * Accepts LOD nodes of level 0 and stops traversal at any DCS node
 * that is not the root.
 */ 
int MEAPI
McdPerformerDefaultAcceptNodeCallback( pfNode* root, pfNode* parent, 
                                     pfNode* node, void* )
{
  // ingore anything below a DCS
  if (node->getType()->isDerivedFrom(pfDCS::getClassType()) && node != root)
    return 0;

  // accept only highest LOD
  if ( node->getType()->isDerivedFrom( pfLOD::getClassType() ) )
	{
	  if (((pfLOD*)parent)->getChild(0) != node) return 0; // reject this node
	}
  return 1; // accept everything else
}


/**
 Create an McdTriangleMesh from a Iris Performer scene graph.
 The scenegraph is descended, taking into account any pfSCS nodes,
 and the geometry under the \a root  node is collected into a mesh.
 By default, traversal stops at any pfDCS nodes, 
 which contain a variable transform,
 except if the \a root  node is a pfDCS, whose transform is ignored
 (it can be used to specify the position of the model using the
 McdModelUpdateTransformFromPerformerMatrix callback.)

 If non NULL, the \a acceptNodeCB callback is called at every node to let 
 the user identify which children to traverse, \a userCBData is passed to it. 
 It can be set to 0, in which case the default callback
 McdModelDefaultPerformerNodeCallback is called and the default behaviour
 described above occurs.

 The \a rootmat matrix, is applied to the entire geometry, if non NULL.

 The \a options parameter indicates various options when creating 
 the triangle mesh. Currently, if \a options = 0, a default mesh will be 
 created. If \a options = McdTriangleMeshWithDistance, 
 the created mesh can also be used for computing distance purpose.
 See McdTriangleMeshCreate.
 */
McdGeometryID MEAPI
McdTrangleMeshCreateFromPerformer( pfNode* root, pfMatrix *rootmat, 
	McdPerformerNodeCallback acceptNodeCB, void* userCBData, int options )
{
  pfMatrix m;
  if (rootmat) 
    m.copy(*rootmat); 
  else 
    m.makeIdent();

  if (acceptNodeCB == 0) {
    acceptNodeCB = McdPerformerDefaultAcceptNodeCallback;
    userCBData = 0;
  }
  
  McdVec3Array *vertexList = 0;
  CX_NEW(vertexList, McdVec3Array); 
  MCD_CHECK_NULLPTR(vertexList, "McdVec3Array");
  vertexList->init(10);

  McdVec3IntArray *triList = 0;
  CX_NEW(triList, McdVec3IntArray); 
  MCD_CHECK_NULLPTR(triList, "McdVec3IntArray");
  triList->init(10);

  traverse(root, &m, root, vertexList, triList, acceptNodeCB, userCBData); 
  vertexList->squeeze(); 
  // triList->squeeze(); // no need to shrink here, will be deleted anyway

  McdTriangleMeshID mesh = 0;
  mesh = McdTriangleMeshCreate(triList->size);
  
  McdVec3Int vi;
  for (int i=0; i<triList->index; i++) {
    vi = triList->array[i];
	  McdTriangleMeshAddTriangle( mesh, vertexList->array[vi.v[0]].v, 
     								                  vertexList->array[vi.v[1]].v, 
                                      vertexList->array[vi.v[2]].v);
  }
  
  ((CxTriangleMesh_*)mesh)->m_vertexPtr = vertexList->array;
  ((CxTriangleMesh_*)mesh)->m_flags = options;
  
  McdTriangleMeshBuild( mesh );  
  
  CX_DELETE(triList, McdVec3IntArray);
  CX_DELETE(vertexList, McdVec3Array);  // vertexList->array will not be deleted,
                                        // passed to mesh for later deletion
  // printf("%d \n", vertexList->size);  
  // printf("tris: %d \n", McdTriangleMeshGetNumberOfTriangles(mesh));
  
  return mesh;
}


/*
 * return v*m: Performer convention
 */
lsVec3 lsFullXformPt_( const lsVec3& v, const pfMatrix& m )
{
  lsVec3 vtmp;
  MeReal ctmp;

  for ( int i = 0; i < 3; i++ )  {
    ctmp = 0;
    for ( int j = 0; j < 3; j++ ) {
	    ctmp += v[j]*m[j][i];	 
	  }
      
    ctmp += m[3][i];
    vtmp[i] = ctmp;
  }
      
  return vtmp;
}

/*
 *
 */
void lsTransform_( McdVec3Array *vertexList, int jBegin, int jEnd, 
                   const pfMatrix& mat )
{   
  lsVec3 v;
  for (int j=jBegin; j<jEnd; j++)
  { 
    v = lsFullXformPt_( vertexList->array[j], mat );
    vertexList->array[j] = v;
  }
}


/*
 *
 */ 
lsVec3 
copy_lsVec3(const pfVec3& pfv) 
{
  lsVec3 v3;
  v3.v[0] = pfv[0];
  v3.v[1] = pfv[1];
  v3.v[2] = pfv[2];
  return v3;
}


/*
 *
 */
void 
McdExtractVertices( const void* alist, const unsigned short *ilist, 
			              const int& iBegin, const int& iEnd,
			              McdVec3Array *verList )
{
  int iRef;
  pfVec3 *attrList = (pfVec3 *) alist;

  for( int i = iBegin; i < iEnd; i++ ) 
  {
    if ( ilist != NULL ) {  
	    iRef = ilist[i];   // indexed
	  } else {
	    iRef = i;          // noindexed 
	  }
     
    verList->insert( copy_lsVec3( attrList[iRef] ) ); 
  }
}
 
/*
 *
 */
static int 
addToTriangleMeshFromGeoSet(pfGeoSet* gSet, pfMatrix* scs,  
                            McdVec3Array *vertexList,
                            McdVec3IntArray* triList )
{  
  int primType = gSet->getPrimType(); 
  
  if ( primType == PFGS_LINESTRIPS || primType == PFGS_FLAT_LINESTRIPS || 
       primType == PFGS_POINTS     || primType == PFGS_LINES )
  {     
      return 0;
  }

  int *lengths = NULL;

  if ( primType == PFGS_TRISTRIPS || primType == PFGS_FLAT_TRISTRIPS ||
       primType == PFGS_FLAT_TRIFANS || primType == PFGS_TRIFANS ||
       primType == PFGS_POLYS )
  {
    lengths = gSet->getPrimLengths();
  }

  int j, iBegin = 0, iEnd = 0;
  int prevVers = vertexList->index;

  void *alist = NULL;
  unsigned short *ilist = NULL;
  gSet->getAttrLists( PFGS_COORD3, &alist, &ilist );

  for ( int ip = 0; ip < gSet->getNumPrims(); ip++ )
  {
    switch( primType )
	  {
	    case PFGS_TRIFANS:
	    case PFGS_FLAT_TRIFANS:
	    case PFGS_TRISTRIPS:  
	    case PFGS_POLYS:
	    case PFGS_FLAT_TRISTRIPS:
	      iEnd = iBegin + lengths[ip];
	      break;
	  
	    case PFGS_TRIS: 
      {
	      iEnd = iBegin + 3;
	      break;
	    }
	    case PFGS_QUADS:
	    {
	      iEnd = iBegin + 4;
	      break;
	    }

	    default:
	    {
	      MEASSERT( false );
	      break;      
	    }
	  }
     
    McdExtractVertices( alist, ilist, iBegin, iEnd, vertexList );     
    
    // iBegin - index in the pf attrList
    // iEnd   - index in the pf attrList
    // jBegin - index in the vertexList
    // jEnd   - index in the vertexList
    int jBegin = prevVers+iBegin;    
    int jEnd   = prevVers+iEnd;        
    
    if ( scs != NULL ) // apply compound pfSCS	    
      lsTransform_( vertexList, jBegin, jEnd, *scs );     
    
    switch( primType )
	  {
	    case PFGS_TRISTRIPS:
	    case PFGS_FLAT_TRISTRIPS:	  
	    
	      int c = 0;
	      for( j = jBegin; j < jEnd - 2; j++ ) {	
          if( c % 2 )          
            triList->insert(j, j+2, j+1);
 	        else
	          triList->insert(j, j+1, j+2);	 
          c++;                                              
        } 	      
	    
	      break;
	  
	    case PFGS_TRIFANS:
	    case PFGS_FLAT_TRIFANS:        
        for( j = jBegin; j < jEnd - 2; j++ ) {	    
	        triList->insert(jBegin, j+2, j+1);      
	      }
 	    
	      break;

	    case PFGS_POLYS:  // It's convex by definition in Performer         
        for (j=jBegin; j<jEnd-2; j++) {        
          triList->insert(jBegin, j+1, j+2); 
        }
 
	      break;

	    case PFGS_TRIS: 
	    {         
        triList->insert(jBegin, jBegin+1, jBegin+2); 
 	      break;
	    }

	    case PFGS_QUADS:
	    {  
			  triList->insert(jBegin, jBegin+1, jBegin+3);
			  triList->insert(jBegin+1, jBegin+2, jBegin+3);            

        break;
	    }

	    default:
	    {
	      // MEASSERT( false );
	      break;      
	    }
	  }

    iBegin = iEnd;
  }

  return 1;
}
  
/** Copy a pfMatrix to an MeMatrix. */
void MEAPI
McdCopyTransformFromPerformerMatrix(MeMatrix4Ptr m, const pfMatrix * const pfm)
{

  // set each axis of lsTransform from a row of pfm 

  m[0][0] = pfm->mat[0][0] ; 
  m[0][1] = pfm->mat[0][1] ; 
  m[0][2] = pfm->mat[0][2] ; 

  m[1][0] = pfm->mat[1][0] ; 
  m[1][1] = pfm->mat[1][1] ; 
  m[1][2] = pfm->mat[1][2] ; 
  
  m[2][0] = pfm->mat[2][0] ; 
  m[2][1] = pfm->mat[2][1] ; 
  m[2][2] = pfm->mat[2][2] ; 
  
  // set translation component
  
  m[3][0] = pfm->mat[3][0]; 
  m[3][1] = pfm->mat[3][1]; 
  m[3][2] = pfm->mat[3][2];
}

/** Copy a pfMatrix in a pfDCS to an MeMatrix. */
void MEAPI
McdCopyTransformFromPerformerDCS(MeMatrix4Ptr m, pfDCS *dcs)
{

  // set each axis of lsTransform from a row of pfm 
  
  const pfMatrix *pfm = dcs->getMatPtr();

  m[0][0] = pfm->mat[0][0] ; 
  m[0][1] = pfm->mat[0][1] ; 
  m[0][2] = pfm->mat[0][2] ; 

  m[1][0] = pfm->mat[1][0] ; 
  m[1][1] = pfm->mat[1][1] ; 
  m[1][2] = pfm->mat[1][2] ; 
  
  m[2][0] = pfm->mat[2][0] ; 
  m[2][1] = pfm->mat[2][1] ; 
  m[2][2] = pfm->mat[2][2] ; 
  
  // set translation component
  
  m[3][0] = pfm->mat[3][0]; 
  m[3][1] = pfm->mat[3][1]; 
  m[3][2] = pfm->mat[3][2];
}

/** Copy an MeMatrix to a pfMatrix. */
void MEAPI
McdCopyTransformToPerformerMatrix(pfMatrix *pfm, const MeMatrix4Ptr m)
{
  // each axis of lsTransform becomes a row of pfMatrix

  pfm->mat[0][0] = m[0][0];
  pfm->mat[0][1] = m[0][1];
  pfm->mat[0][2] = m[0][2];
  pfm->mat[0][3] = 0;

  pfm->mat[1][0] = m[1][0];
  pfm->mat[1][1] = m[1][1];
  pfm->mat[1][2] = m[1][2];
  pfm->mat[1][3] = 0;

  pfm->mat[2][0] = m[2][0];
  pfm->mat[2][1] = m[2][1];
  pfm->mat[2][2] = m[2][2];
  pfm->mat[2][3] = 0;

  // set translation component

  pfm->mat[3][0] = m[3][0];
  pfm->mat[3][1] = m[3][1];
  pfm->mat[3][2] = m[3][2];
  pfm->mat[3][3] = 1;
}

/** Copy an MeMatrix to a pfDCS. */
void MEAPI
McdCopyTransformToPerformerDCS(pfDCS *dcs, const MeMatrix4Ptr m)
{

  // each axis of lsTransform becomes a row of pfMatrix
  
  pfMatrix pfm;

  pfm.mat[0][0] = m[0][0];
  pfm.mat[0][1] = m[0][1];
  pfm.mat[0][2] = m[0][2];
  pfm.mat[0][3] = 0;

  pfm.mat[1][0] = m[1][0];
  pfm.mat[1][1] = m[1][1];
  pfm.mat[1][2] = m[1][2];
  pfm.mat[1][3] = 0;

  pfm.mat[2][0] = m[2][0];
  pfm.mat[2][1] = m[2][1];
  pfm.mat[2][2] = m[2][2];
  pfm.mat[2][3] = 0;

  // set translation component

  pfm.mat[3][0] = m[3][0];
  pfm.mat[3][1] = m[3][1];
  pfm.mat[3][2] = m[3][2];
  pfm.mat[3][3] = 1;
  
  dcs->setMat( pfm );
}

/**
  Set a pointer to a pfDCS (dynamic coordinate system node) 
  to be synchronized with the models internal
  transform, which must be allocated explicitely be the user.
  The matrix must be orthonormal, in a right handed coordinate system,
  with no scaling (use pfFlatten to apply any scaling).
  Use the McdModelUpdateTransformFromPerformerDCS function or the
  McdModelUpdateTransformToPerformerDCS callback to synchronized the
  model's transform with a pfDCS node.
*/
void MEAPI
McdModelSetPerformerDCSPtr( McdModelID cm, pfDCS* dcs )
{
  cm->mAuxTM = (void*) dcs;
}


/** Return pfDCS pointer set in McdModelSetPerformerMatrixPtr */
pfDCS* MEAPI
McdModelGetPerformerDCSPtr( McdModelID cm)
{
  return (pfDCS*) cm->mAuxTM;
}


/**
  Callback to be used with McdModelSetUpdateCallback to automatically
  synchronize the McdModel transform with its associated pfDCS.
  Can be used if the collision library is used to independently and
  the user wants an McdModel to automatically follow a changing 
  pfDCS which is set in McdModelSetPerformerDCSPtr, when inserted
  into an McdSpace.
  To use automatic updates, pass this function to McdModelSetUpdateCallback.
  McdModelSetPerformerDCSPtr must be called to set the pfDCS pointer.
*/
void MEAPI
McdModelUpdateTransformFromPerformerDCS( McdModelID cm )
{
  MEASSERT(cm->mGlobalTM);
  McdCopyTransformFromPerformerDCS(cm->mGlobalTM, (pfDCS*)(cm->mAuxTM));
}


/**
  Callback to be used with McdModelSetUpdateCallback to automatically
  synchronize a pfDCS with the models transform. 
  Can be used when the collision library is used with the Mdt dynamics
  library which changes the model's transform automatically.
  When set using McdModelSetUpdateCallback, 
  the pfDCS (set in McdModelSetPerformerMatrixPtr) is automatically
  updated.
  McdModelSetPerformerDCSPtr must be called to set the pfMatrix pointer.
*/
void MEAPI
McdModelUpdateTransformToPerformerDCS( McdModelID cm )
{
  MEASSERT(cm->mGlobalTM);
  McdCopyTransformToPerformerDCS((pfDCS*)(cm->mAuxTM), cm->mGlobalTM);
} 


//------------------------------------------------------------------------

/*
 * 
 */
McdVec3Array::McdVec3Array()
{
  array = 0;
  size  = 0;
  index = 0;
}

/*
 * 
 */
McdVec3Array::~McdVec3Array()
{
  // destroy();
}

/*
 * 
 */
void
McdVec3Array::destroy()
{
  if (array)
    MeMemoryAPI.destroy(array);
}

/*
 * 
 */
void 
McdVec3Array::init( int sizeIn )
{  
  array = (lsVec3 *) MeMemoryAPI.create(sizeIn * sizeof(lsVec3));
  MCD_CHECK_NULLPTR(array, "McdVec3Array");  
  size = sizeIn;
}
  
/*
 * 
 */
int
McdVec3Array::getNumOfElements() const
{  
  return index;   
} 

/*
 * 
 */
void    
McdVec3Array::insert( const lsVec3 &v)
{   
  if (index >= size) resize(index+1);
  array[index] = v;
  index++;
}

/*
 * 
 */
void 
McdVec3Array::resize( int sizeIn )
{  
  if (sizeIn > 2*size) 
    size = sizeIn;
  else
    size = 2*size;
  array = (lsVec3*) MeMemoryAPI.resize(array, size*sizeof(lsVec3));
  MCD_CHECK_NULLPTR(array, "McdVec3Array");
}

/*
 * 
 */
void    
McdVec3Array::squeeze()
{ 
  if (index<size) {
    array = (lsVec3*) MeMemoryAPI.resize(array, index*sizeof(lsVec3));
    size = index;
  }    
}


//------------------------------------------------------------------------

/*
 * 
 */
McdVec3IntArray::McdVec3IntArray()
{
  array = 0;
  size  = 0;
  index = 0;
}

/*
 * 
 */
McdVec3IntArray::~McdVec3IntArray()
{
  destroy();
}

/*
 * 
 */
void
McdVec3IntArray::destroy()
{
  if (array)
    MeMemoryAPI.destroy(array);
}

/*
 * 
 */
void 
McdVec3IntArray::init( int sizeIn )
{  
  array = (McdVec3Int *) MeMemoryAPI.create(sizeIn * sizeof(McdVec3Int));
  MCD_CHECK_NULLPTR(array, "McdVec3IntArray");  
  size = sizeIn;
}
  
/*
 * 
 */
int
McdVec3IntArray::getNumOfElements() const
{  
  return index;   
} 

/*
 * 
 */
void    
McdVec3IntArray::insert( int i0, int i1, int i2)
{   
  if (index >= size) resize(index+1);
  McdVec3Int vi;
  vi.v[0] = i0, vi.v[1] = i1; vi.v[2] = i2;
  array[index] = vi;
  index++;
}

/*
 * 
 */
void 
McdVec3IntArray::resize( int sizeIn )
{  
  if (sizeIn > 2*size) 
    size = sizeIn;
  else
    size = 2*size;
  array = (McdVec3Int*) MeMemoryAPI.resize(array, size*sizeof(McdVec3Int));
  MCD_CHECK_NULLPTR(array, "McdVec3IntArray");
}

/*
 * 
 */
void    
McdVec3IntArray::squeeze()
{ 
  if (index<size) {
    array = (McdVec3Int*) MeMemoryAPI.resize(array, index*sizeof(McdVec3Int));
    size = index;
  }    
}


#ifndef CXTRIANGLENE
#define CXTRIANGLENE

#include <lsTransform.h>
#include <McdGeometryTypes.h>

/*---------------------------------------------------------------------------*/
class CxTriangleNE {  // CxTriangle with Normal (N) and Edges (E)
public:

  CxTriangleNE() {}
  
  lsVec3 *mVertex[3];
  lsVec3 mNormal;	
  lsVec3 mEdge[3];
  McdTriangleFlags flags;

  void SetVertices( lsVec3* v0, lsVec3* v1, lsVec3* v2) {
    mVertex[0] = v0; mVertex[1] = v1; mVertex[2] = v2;
  }

  void ComputeEdges() {
    mEdge[0] = *mVertex[1]-*mVertex[0];	
    mEdge[1] = *mVertex[2]-*mVertex[1];	
    mEdge[2] = *mVertex[0]-*mVertex[2];
  }

  void ComputeAll() {
    ComputeEdges();
    mNormal = mEdge[0].cross(mEdge[1]);
    mNormal.normalize();
  }
};


#endif

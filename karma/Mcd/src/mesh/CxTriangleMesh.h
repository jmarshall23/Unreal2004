/* -*-c++-*-
 *===============================================================
 * File:        CxTriangleMesh.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.23.2.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef CxTriangleMESH_H
#define CxTriangleMESH_H

#include <McdGeometry.h>
#include <lsTransform.h>
#include <McdTriangleMesh.h> // for properties

/*----------------------------------------------------------------
 *  CxTriangleMesh
 *----------------------------------------------------------------
 */

MCD_DECLARE_GEOMETRY_TYPE(CxTriangleMesh);

typedef McdGeometry CxTriangleMesh;

/**
 * Half Egde data structure -- element of triangle
 */
typedef struct CxHalfEdge {
    MeVector3 *vertex;      //
    int       heMateId;     // opposite half edge (from half edge list)

} CxHalfEdge;

/**
 * Triangle Mesh data structure
*/
typedef struct CxTriangleMesh_ {
    McdGeometry m_g;

    void     *m_bvt;        // BVTree associated with the mesh

    int      *m_auxData;    // data used for rendering

    int       m_triangleCount;  // number of triangles

    CxHalfEdge *m_triangleArray;    // list of triangles

    MeVector3 *m_normalList;    // triangle normals

    MeReal   *m_areaList;   // triangle area

    int       m_triangleMaxCount;   // current allocated memory size for tris.

    void      (*m_destroyCallbackFn) (void *);

    void      *m_destroyCallbackData;

    void      *m_vertexPtr;

    MeVector3 m_AABB_ext;   // local AABB dimension
    MeVector3 m_AABB_center;    // local AABB center
    MeReal    m_radius;

    int m_flags; // level of optimization for distance

} CxTriangleMesh_;

/*--------------------------------------------------------------------------*/

CxTriangleMesh *MEAPI
CxTriangleMeshCreate(McdFrameworkID frame, int triCount, int properties);
void MEAPI
CxTriangleMeshGetAuxData(CxTriangleMesh *g, int **data);
void MEAPI
CxTriangleMeshComputeLocalAABB(CxTriangleMesh *g);
int MEAPI
CxTriangleMeshAddTriangle(CxTriangleMesh *g, MeVector3 v0, MeVector3 v1, MeVector3 v2);

void MEAPI
CxTriangleMeshGetTriangleVertexPtrs(CxTriangleMesh *g,
                    int index, lsVec3 **v1, lsVec3 **v2, lsVec3 **v3);
void MEAPI
CxTriangleMeshGetTriangleVertexPtr(CxTriangleMesh *g, int index, lsVec3 **vertices);
void MEAPI
CxTriangleMeshGetTriangleVertices(CxTriangleMesh *g, int index, lsVec3 *v1, lsVec3 *v2, lsVec3 *v3);

void MEAPI
CxTriangleMeshGetTriangleNormalPtr(CxTriangleMesh *g, int index, lsVec3 **n);

MeReal MEAPI
CxTriangleMeshGetTriangleArea(CxTriangleMesh *g, int index);
int MEAPI
CxTriangleMeshGetTriangleCount(CxTriangleMesh *g);

unsigned int MEAPI
CxTriangleMeshBuild(CxTriangleMesh *g);
unsigned int MEAPI
CxTriangleMeshIsBuilt(CxTriangleMesh *g);

/* not a public fn */
void MEAPI
CxTriangleMeshComputeNormal(CxTriangleMesh *g);

void MEAPI
CxTriangleMeshSetDestroyCallbackFunction(CxTriangleMesh *g, void (*f) (void *));
void MEAPI
CxTriangleMeshSetDestroyCallbackData(CxTriangleMesh *g, void *d);

void MEAPI
ComputeExtremalVertices(CxTriangleMesh *mesh, int *id,
            int numTris, const lsVec3 &axis, lsVec3 **vMax, lsVec3 **vMin,
            MeReal *projMax, MeReal *projMin);

void MEAPI
ComputeExtremalVerticesEntireMesh(CxTriangleMesh *mesh,
            const lsVec3 &axis, lsVec3 **vMax, lsVec3 **vMin,
            MeReal *projMax, MeReal *projMin);

void MEAPI
ComputeSampledExtremalVerticesEntireMesh(CxTriangleMesh *mesh,
            const lsVec3 &axis, lsVec3 **vMax, lsVec3 **vMin,
            MeReal *projMax, MeReal *projMin);


#endif              /* CxTriangleMESH_H */

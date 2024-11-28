#ifndef _MCDTRIANGLEMESH_H
#define _MCDTRIANGLEMESH_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:53 $ - Revision: $Revision: 1.42.2.3 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

/**
   @file

    The triangle mesh geometry type.
    McdTriangleMesh defines a mesh of triangles with shared vertices.
    The geometrical surface of an arbitrarily complex model can be specified
    as a collection of such triangles.
    Vertices are allocated and managed by the user and the mesh triangle point
    to those vertices. This allows the vertex data to be shared and
    synchronized between the collision package and the user's 3D graphics
    package.
    If two triangles share a vertex, it is preferable if the corresponding
    vertex pointers are also the same in each triangle.
    Otherwise, if each triangle has its own copy of its vertices,
    the system will function but will not be able to exploit connectivity
    information, and as a result contact generation might be less precise or
    less efficient in the future.

    Meshes can be loaded from Wavefront obj files using the
    McduTriangleMeshCreateFromObj function found in
    Mcd/include/McduTriangleMeshIO.h.
    In addition, meshes can be created from IRIS Performer scene graph
    nodes, interfacing to any file format that can be loaded by Performer,
    including pfb and flt files.

 */

#include <McdCTypes.h>
#include <McdGeometry.h>
#include <McdModelPair.h>
#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------
 *  McdTriangleMesh
 *----------------------------------------------------------------
 */

typedef McdGeometryID McdTriangleMeshID;

typedef
enum { McdTriangleMeshOptionNoDistance = 0,
       McdTriangleMeshOptionDistanceAllowed = 1
}
McdTrianglesMeshOptions;


void              MEAPI McdTriangleMeshRegisterType(McdFramework *frame);
int               MEAPI McdTriangleMeshGetTypeId();

MeBool            MEAPI McdTriangleMeshRegisterInteractions(McdFrameworkID frame);
MeBool            MEAPI McdTriangleMeshTriangleMeshRegisterInteraction(McdFrameworkID frame);


McdTriangleMeshID MEAPI McdTriangleMeshCreate(McdFrameworkID frame, int triCount );

McdTriangleMeshID MEAPI McdTriangleMeshCreateWithOptions(McdFrameworkID frame, int triMaxCount,
                    int options);

int               MEAPI McdTriangleMeshAddTriangle( McdTriangleMeshID mesh,
                            MeVector3 v0, MeVector3 v1, MeVector3 v2 );
#define McdTriangleMeshGetNumberOfTriangles( m ) McdTriangleMeshGetTriangleCount( m )
int               MEAPI McdTriangleMeshGetTriangleCount( McdTriangleMeshID );

void              MEAPI McdTriangleMeshGetAuxData( McdTriangleMeshID, int** );

void              MEAPI McdTriangleMeshGetTriangleVertexPtrs( McdTriangleMeshID,
                            int index,
                            MeVector3Ptr *v1, MeVector3Ptr *v2, MeVector3Ptr *v3 );

void              MEAPI McdTriangleMeshGetTriangleNormalPtr( McdTriangleMeshID,
                            int index, MeVector3Ptr *n );

unsigned int      MEAPI McdTriangleMeshBuild( McdTriangleMeshID );
unsigned int      MEAPI McdTriangleMeshIsBuilt( McdTriangleMeshID mesh);

MeI16 MEAPI McdTriangleMeshGetMassProperties( McdTriangleMeshID mesh, MeVector3 relTM, MeMatrix3 m, MeReal* volume);

void              MEAPI McdTriangleMeshSetDestroyCallback(
                            McdTriangleMeshID, void (*)(void*), void* );

struct McdDistanceResult;
int           MEAPI McdTriangleMeshTriangleMeshIntersect(McdModelPair *p,
                            McdIntersectResult* r );


void          MEAPI McdTriangleMeshTriangleMeshDistance(McdModelPair *p,
                            struct McdDistanceResult* r );

void MEAPI McdTriangleMeshTriangleMeshGetClosestTriangles( struct McdDistanceResult* r, int *triangle1, int *triangle2);


#ifdef __cplusplus
}
#endif

#endif /* _MCDTRIANGLEMESH_H */

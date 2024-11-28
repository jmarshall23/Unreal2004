/* -*-c++-*-
 *===============================================================
 * File:        McdTriangleMesh.c
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.24.2.2 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#include <MePrecision.h>
#include <MeMessage.h>
#include <McdCTypes.h>
#include <McdTriangleMesh.h>

#include "CxTriangleMesh.h"

/*----------------------------------------------------------------
 * McdTriangleMesh (wrapper) implementation
 *----------------------------------------------------------------
 */

/**
   Registers the TriangleMesh geometry type.
 */
extern    "C" void MEAPI
McdTriangleMeshRegisterType(McdFramework *frame)
{
    CxTriangleMeshRegisterType(frame);
}

/**
   Create a triangle mesh with given initial maximum number of triangles.
   Can be resized later.
 */
extern    "C" McdTriangleMeshID MEAPI
McdTriangleMeshCreate(McdFrameworkID frame, int triMaxCount)
{
    return (McdTriangleMeshID) CxTriangleMeshCreate(frame, triMaxCount,McdTriangleMeshOptionNoDistance);
}

/**
   Create a triangle mesh with given initial maximum number of triangles,
   with certain options.
   If options are set to McdTriangleMeshWithDistance, the mesh will be
   suitable for finding mesh-mesh distance
   (McdTriangleMeshTriangleMeshDistance).
   Collision tests and contact generation
   (McdIntersect, McdTriangleMeshTriangleMeshIntersect) are not affected.
 */

extern    "C" McdTriangleMeshID MEAPI
McdTriangleMeshCreateWithOptions(McdFrameworkID frame, int triMaxCount, int options)
{
    return (McdTriangleMeshID) CxTriangleMeshCreate(frame,triMaxCount,options);
}



/**
   Add a triangle to the triangle mesh. It can be referred to later
   by an index which starts at 0 and increments by one with each triangle added
   with this call.
   Triangles are assumed to be given so that their vertices are listed
   counter-clockwise as seen from the outside of the model. Normals
   are computed using this assumption, and point outside.
   @a v0, @a v1, @a, v2 are pointers to an array of three reals of type
   MeReal.
 */
extern    "C" int MEAPI
McdTriangleMeshAddTriangle(McdTriangleMeshID mesh, MeVector3 v0, MeVector3 v1, MeVector3 v2)
{
    return CxTriangleMeshAddTriangle((McdGeometry *) mesh, v0, v1, v2);
}

/**
   Returns the number of triangles currently added to the mesh.
*/
extern    "C" int MEAPI
McdTriangleMeshGetTriangleCount(McdTriangleMeshID mesh)
{
    return CxTriangleMeshGetTriangleCount((McdGeometry *) mesh);
}

/** @internal */
extern    "C" void MEAPI
McdTriangleMeshGetAuxData(McdTriangleMeshID mesh, int **data)
{
    CxTriangleMeshGetAuxData((McdGeometry *) mesh, data);
}

/**
   Get the pointer to the vertices of the triangle specified by the index.
 */
extern    "C" void MEAPI
McdTriangleMeshGetTriangleVertexPtrs(McdTriangleMeshID mesh, int index,
                     MeVector3Ptr * v1, MeVector3Ptr * v2, MeVector3Ptr * v3)
{
    CxTriangleMeshGetTriangleVertexPtrs((McdGeometry *) mesh, index, (lsVec3 **) v1,
                    (lsVec3 **) v2, (lsVec3 **) v3);
}

/**
   Get a pointer to the normal of the triangle specified by the index.
 */
extern    "C" void MEAPI
McdTriangleMeshGetTriangleNormalPtr(McdTriangleMeshID mesh, int index, MeVector3Ptr * n)
{
    CxTriangleMeshGetTriangleNormalPtr((McdGeometry *) mesh, index, (lsVec3 **) n);
}

/**
   Do precomputation necessary for fast collision detection. Allocates memory.
   Needs to be called once, after adding all the triangles.
   No subsequent modifications of the triangles mesh are allowed, this includes
   adding triangles and modifying the vertices of the triangles.
   Call to McdIntersect on a model with this geometry will fail if this function is not
   called.
 */
extern    "C" unsigned int MEAPI
McdTriangleMeshBuild(McdTriangleMeshID mesh)
{
    return CxTriangleMeshBuild((McdGeometry *) mesh);
}

/**
   Returns 1 if McdTriangleMeshBuild has been called.
 */
extern    "C" unsigned int MEAPI
McdTriangleMeshIsBuilt(McdTriangleMeshID mesh)
{
    return CxTriangleMeshIsBuilt((McdGeometry *) mesh);
}

/**
   Sets a convenience callback function that will be called upon
   McdTriangleMeshDestroy. Can be used to deallocate memory for
   the vertices. The @a data parameter is passed to the function.
 */
extern    "C" void MEAPI
McdTriangleMeshSetDestroyCallback(McdTriangleMeshID mesh, void (*f) (void *), void *data)
{
    CxTriangleMeshSetDestroyCallbackFunction((McdGeometry *) mesh, f);
    CxTriangleMeshSetDestroyCallbackData((McdGeometry *) mesh, data);
}

/**
   Returns the integer id corresponding to the TriangleMesh geometry type.
*/
int       MEAPI
McdTriangleMeshGetTypeId()
{
    return CxTriangleMeshGetTypeId();
}

/**
   Compute the mass properties of the triangle mesh using an exact
   volumetric method that is robust in the presence of small holes in
   the model.
*/
MeI16     MEAPI
McdTriangleMeshGetMassProperties(McdTriangleMeshID mesh, MeMatrix4 relTM, MeMatrix3 m,
                 MeReal *volume)
{
    return CxTriangleMeshGetMassProperties(mesh, relTM, m, volume);
}

void MEAPI McdTriangleMeshTriangleMeshGetClosestTriangles( McdDistanceResult* r, int *triangle1, int *triangle2)
{
  *triangle1 = r->element1.tag;
  *triangle2 = r->element2.tag;
}

void MEAPI McdTriangleMeshDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    static int warn = 1;
    if(warn)
        MeWarning(0, "McdTriangleMeshDebugDraw: Don't know how to draw a triangle mesh!");
    warn = 0;
}

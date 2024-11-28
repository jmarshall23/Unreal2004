/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:05 $ - Revision: $Revision: 1.18.2.8 $

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

#include <string.h>
#include <MeAssetDB.h>
#include "MeAssetDBInternal.h"
#include <MeMath.h>
#include <MeMemory.h>
#include <MeMessage.h>

/*----------------------- creation and destruction ---------------------- */

/** @internal */
static void MEAPI _MeFPrimitiveInit(MeFPrimitive *const p)
{
    p->id = 0;
    p->geometry = 0;
    p->nVertices = 0;
    p->maxVertices = 1;
    p->vertices = (MeVector3*)MeMemoryAPI.create(sizeof(MeVector3) * p->maxVertices);
    p->type = kMeFPrimitiveTypeUnknown;
    p->dims[0] = p->dims[1] = p->dims[2] = 1;
    MeMatrix4TMMakeIdentity(p->tm);
}

/**
 * Creates a primitive of the specified type.
 */
MeFPrimitive *MEAPI MeFPrimitiveCreate(const char *const name, MeFPrimitiveType type)
{
    MeFPrimitive *p = (MeFPrimitive*)MeMemoryAPI.createZeroed(sizeof(MeFPrimitive));
    
    _MeFPrimitiveInit(p);

    _FSetStringProperty(&p->id, name);
    p->type = type;

    return p;
}

/**
 * Returns a copy of the primitive geometry. The copy must be inserted
 * into a parent geometry.
 */
MeFPrimitive *MEAPI MeFPrimitiveCreateCopy(const MeFPrimitive *const p, MeBool recurse)
{
    MeFPrimitive *copy = (MeFPrimitive*)MeMemoryAPI.createZeroed(sizeof(MeFPrimitive));

    _FSetStringProperty(&copy->id, p->id);
    copy->type = p->type;
    MeVector3Copy(copy->dims, p->dims);
    MeMatrix4Copy(copy->tm, p->tm);
    copy->nVertices = p->nVertices;
    copy->maxVertices = p->maxVertices;
    copy->vertices = (MeVector3*)MeMemoryAPI.create(sizeof(MeVector3) * p->maxVertices);
    memcpy(copy->vertices, p->vertices, sizeof(MeVector3) * p->nVertices);

    return copy;
}

/**
 * Destroys a primitive geometry. The primitive must be removed from a geometry
 * before it can be destroyed.
 */
void MEAPI MeFPrimitiveDestroy(MeFPrimitive *const p)
{
    if (!p)
    {
        ME_REPORT(MeWarning(3, "You attempted to destroy a non-existent geometry."));
        return;
    }

    MeMemoryAPI.destroy(p->vertices);
    MeMemoryAPI.destroy(p->id);
    MeMemoryAPI.destroy(p);
}

/*--------------------------------- accessors ---------------------------------- */

/**
 * Returns type of primitive geometry.
 */
MeFPrimitiveType MEAPI MeFPrimitiveGetType(const MeFPrimitive *const p)
{
    return p->type;
}

/**
 * Returns the name of an primitive.
 */
char * MEAPI MeFPrimitiveGetName(const MeFPrimitive *const p)
{
    return p->id;
}

/**
 * Returns primitive's radius. Use for sphere, cylinder and sphyl.
 */
MeReal MEAPI MeFPrimitiveGetRadius(const MeFPrimitive *const p)
{
    return p->dims[0];
}

/**
 * Yields primitive's dimensions. Use for box only.
 */
void MEAPI MeFPrimitiveGetDimensions(const MeFPrimitive *const p, MeVector3 dims)
{
    MeVector3Copy(dims, p->dims);
}

/**
 * Returns height of primitive. Use for cylinder and sphyl (but not box).
 */
MeReal MEAPI MeFPrimitiveGetHeight(const MeFPrimitive *const p)
{
    return p->dims[1];
}

/**
 * Initialize an iterator to iterate over vertices. Use for convex. Do not add
 * any vertices during iteration.
 */
void MEAPI MeFPrimitiveInitVertexIterator(MeFPrimitive *const p, MeFVertexIt *const it)
{
    it->currentVertex = 0;
    it->prim = p;
}

/**
 * Returns a pointer to the vertex associated with the current iterator and advances
 * the iterator.
*/
MeReal *MEAPI MeFPrimitiveGetVertex(MeFVertexIt *const it)
{
    MeReal *v;
    
    if (it->currentVertex == it->prim->nVertices)
        return 0;

    v = it->prim->vertices[it->currentVertex++];

    return v;
}

/**
 * Returns the number of vertices in the convex geometry.
 */
int MEAPI MeFPrimitiveGetVertexCount(const MeFPrimitive *const p)
{
    return p->nVertices;
}

/**
 * Returns a pointer to the beginning of the vertex array. Use
 * with convex as a faster alternative to the vertex iterator.
 */
MeVector3 *MEAPI MeFPrimitiveGetVertexArray(MeFPrimitive *const p)
{
    return p->vertices;
}

/**
 * Returns the relative transform of the primitive geometry to its parent geometry.
 */
MeMatrix4Ptr MEAPI MeFPrimitiveGetTransformPtr(MeFPrimitive *const p)
{
    return p->tm;
}

/*--------------------------------- Mutators ---------------------------------- */

/**
 * Renames an MeFPrimitive.
 */
void MEAPI MeFPrimitiveRename(MeFPrimitive *const p, const char *const newName)
{
    if (strcmp(p->id, newName) != 0)
        _FSetStringProperty(&p->id, newName);
}

/**
 * Sets primitive's radius. Use for sphers, cylinder, sphyl.
 */
void MEAPI MeFPrimitiveSetRadius(MeFPrimitive *const p, MeReal r)
{
    p->dims[0] = r;
}

/**
 * Sets primitive's dimensions. Use for box only.
 */
void MEAPI MeFPrimitiveSetDimensions(MeFPrimitive *const p, MeReal dx, MeReal dy, MeReal dz)
{
    MeVector3Set(p->dims, dx, dy, dz);
}

/**
 * Sets primitive's height. Use for cylinder and sphyl (but not for box).
 */
void MEAPI MeFPrimitiveSetHeight(MeFPrimitive *const p, MeReal height)
{
    p->dims[1] = height;
}

/**
 * Adds an vertex to a primitive. Use for convex.
 */
void MEAPI MeFPrimitiveAddVertex(MeFPrimitive *const p, MeVector3 pos)
{
    MeVector3Copy(p->vertices[p->nVertices++], pos);
    
    if (p->nVertices >= p->maxVertices)
    {
        p->maxVertices *= 2;
        p->vertices = (MeVector3*)MeMemoryAPI.resize(p->vertices, sizeof(MeVector3) * p->maxVertices);
    }
}

/**
 * Set the primitive's vertices in one go, for a more efficient alternative to
 * MeFPrimitiveAddVertex.
 */
void MEAPI MeFPrimitiveSetVertexArray(MeFPrimitive *const p, MeVector3 *vertices, int nVertices)
{
    p->vertices = (MeVector3*)MeMemoryAPI.resize(p->vertices, sizeof(MeVector3) * nVertices);
    memcpy(p->vertices, vertices, nVertices * sizeof(MeVector3));
    p->nVertices = nVertices;
    p->maxVertices = nVertices;
}

/**
 * Sets the primitive geometry's transform relative to its parent geometry.
 */
void MEAPI MeFPrimitiveSetTransform(MeFPrimitive *const p, const MeMatrix4Ptr tm)
{
    MeMatrix4Copy(p->tm, tm);
}

/**
 * Scale the geometry primitive by the specified scale factor. Scales all
 * parameters, regardless of primitive type.
 */
void MEAPI MeFPrimitiveScale(MeFPrimitive *const p, MeReal scale)
{
    MeFVertexIt it;
    MeReal *v;

    MeFPrimitiveInitVertexIterator(p, &it);

    while (v = MeFPrimitiveGetVertex(&it))
        MeVector3Scale(v, scale);

    MeVector3Scale(p->dims, scale);
    MeVector3Scale(p->tm[3], scale);
   
}


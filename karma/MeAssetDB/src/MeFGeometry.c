/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:05 $ - Revision: $Revision: 1.31.2.8 $

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
#include <MeXMLParser.h>

/*----------------------- creation and destruction ---------------------- */

/** @internal */
static void MEAPI _MeFGeometryInit(MeFGeometry *const fg)
{
    fg->id = 0;
    fg->asset = 0;
    LIST_INIT_SENTINEL(fg->nilPrimitive);
    fg->graphicHint = 0;
    fg->graphicScale = 1;
    MeVector3Set(fg->graphicOffset, 0, 0, 0);
    fg->primCount = 0;
}

/**
 * Create a geometry which will be inserted into an asset.
 */
MeFGeometry *MEAPI MeFGeometryCreate(const char *const name)
{
    MeFGeometry *fg = (MeFGeometry*)MeMemoryAPI.create(sizeof(MeFGeometry));
    fg->nilPrimitive = (MeFPrimitiveNode*)MeMemoryAPI.create(sizeof(MeFPrimitiveNode));
    
    _MeFGeometryInit(fg);
    _FSetStringProperty(&fg->id, name);
    return fg;
}

/**
 * Returns a copy of the geometry. The copy will not be associated with an
 * asset and must be inserted into one. Copies its primitive children if
 * @param recurse is true.
 */
MeFGeometry *MEAPI MeFGeometryCreateCopy(const MeFGeometry *const geometry, MeBool recurse)
{
    MeFGeometry *copy = (MeFGeometry*)MeMemoryAPI.createZeroed(sizeof(MeFGeometry));
    copy->nilPrimitive = (MeFPrimitiveNode*)MeMemoryAPI.create(sizeof(MeFPrimitiveNode));
    LIST_INIT_SENTINEL(copy->nilPrimitive);

    _FSetStringProperty(&copy->id, geometry->id);
    _FSetStringProperty(&copy->graphicHint, geometry->graphicHint);
    copy->graphicScale = geometry->graphicScale;
    MeVector3Set(copy->graphicOffset, geometry->graphicOffset[0], geometry->graphicOffset[1], geometry->graphicOffset[2]);

    if (recurse)
    {
        MeFPrimitive *prim;
        MeFPrimitiveIt it;
        MeFGeometryInitPrimitiveIterator(geometry, &it);

        while (prim = MeFGeometryGetPrimitive(&it))
        {
            MeFPrimitive *copyPrim;
            copyPrim = MeFPrimitiveCreateCopy(prim, recurse);
            MeFGeometryInsertPrimitive(copy, copyPrim);
        }
    }

    return copy;
}

/**
 * Destroys a geometry. The geometry must be removed from an asset
 * database before it can be destroyed.
 */
void MEAPI MeFGeometryDestroy(MeFGeometry *const fg)
{
    if (!fg)
    {
        ME_REPORT(MeWarning(3, "You attempted to destroy a non-existent geometry."));
        return;
    }

    {
        MeFPrimitiveIt it;
        MeFPrimitive *prim;
        MeFGeometryInitPrimitiveIterator(fg, &it);
    
        while (prim = MeFGeometryGetPrimitive(&it))
        {
            MeFGeometryRemovePrimitive(prim);
            MeFPrimitiveDestroy(prim);
            MeFGeometryInitPrimitiveIterator(fg, &it);
        }
    }

    MeMemoryAPI.destroy(fg->id);
    MeMemoryAPI.destroy(fg->nilPrimitive);
    
    if (fg->graphicHint)
        MeMemoryAPI.destroy(fg->graphicHint);
    
    MeMemoryAPI.destroy(fg);
}

/*--------------------------------- Accessors ---------------------------------- */

/**
 * Returns the name of the geometry.
 */
char * MEAPI MeFGeometryGetName(const MeFGeometry *const fg)
{
    return fg->id;
}

/**
 * Returns geometry's graphic hint.
 */
char *MEAPI MeFGeometryGetGraphicHint(const MeFGeometry *const fg)
{
    return fg->graphicHint;
}

/**
 * Returns the scale factor associated with a graphic. Defaults to 1.
 */
MeReal MEAPI MeFGeometryGetGraphicScale(const MeFGeometry *const fg)
{
    return fg->graphicScale;
}

/**
 * Yields the offset of the graphic associated with this geometry.
 */
void MEAPI MeFGeometryGetGraphicOffset(const MeFGeometry *const geometry, MeVector3 v)
{
    MeVector3Copy(v, geometry->graphicOffset);
}

/**
 * Initialize an iterator to iterate over all the primitives contained in this geometry.
 */
void MEAPI MeFGeometryInitPrimitiveIterator(const MeFGeometry *const geometry, MeFPrimitiveIt *const it)
{
    it->node = geometry->nilPrimitive->prev;
}

/** 
 * Return the next primitive associated with the iterator.
 */
MeFPrimitive *MEAPI MeFGeometryGetPrimitive(MeFPrimitiveIt *const it)
{
    MeFPrimitive *prim;
    prim = it->node->current;
    it->node = it->node->prev;
    return prim;
}

/**
 * Look up a primitive from the geometry, by name.
 */
MeFPrimitive *MEAPI MeFGeometryLookupPrimitive(const MeFGeometry *const fg, char *const name)
{
    MeFPrimitiveIt it;
    MeFPrimitive *p;
    MeFGeometryInitPrimitiveIterator(fg, &it);
    
    while (p = MeFGeometryGetPrimitive(&it))
    {
        if (strcmp(p->id, name) == 0)
            return p;
    }

    return 0;
}

/**
 * Return the number of primitives in the geometry.
 */
int MEAPI MeFGeometryGetPrimitiveCount(const MeFGeometry *const geometry)
{
    return geometry->primCount;
}

/*--------------------------------- Mutators ---------------------------------- */

/**
 * Renames an MeFGeometry. If the geometry is inserted into an asset, all
 * references will be resolved - ie any models referencing this geometry
 * will have their references updated.
 */
void MEAPI MeFGeometryRename(MeFGeometry *const fg, const char *const newName)
{
    if (strcmp(fg->id, newName) != 0)
    {
	    if (fg->asset)
        {
		    MeFAssetResolveGeometryReferences(fg->asset, fg->id, newName);
            MeHashDelete(fg->id, fg->asset->nameToGeometry);
        }

        _FSetStringProperty(&fg->id, newName);
    
        if (fg->asset)
            MeHashInsert(fg->id, fg, fg->asset->nameToGeometry);
    }
}

/**
 * Sets the geometry's graphic hint. This may be overridden by an FModel's
 * graphic hint which may be overridden by an FAssetPart graphic hint.
 */
void MEAPI MeFGeometrySetGraphicHint(MeFGeometry *const fg, const char *const hint)
{
    _FSetStringProperty(&fg->graphicHint, hint);
}

/**
 * Sets the geometry's graphic scale.
 */
void MEAPI MeFGeometrySetGraphicScale(MeFGeometry *const fg, MeReal scale)
{
    fg->graphicScale = scale;
}

/**
 * Sets the offset of the graphic associated with the geometry.
 */
void MEAPI MeFGeometrySetGraphicOffset(MeFGeometry *const geometry, MeReal dx, MeReal dy, MeReal dz)
{
    MeVector3Set(geometry->graphicOffset, dx, dy, dz);
}

/**
 * Insert a primitive geometry into the geometry. If the primitive is already in a different
 * geometry it will be removed and inserted into the new one.
 */
void MEAPI MeFGeometryInsertPrimitive(MeFGeometry *const geometry, MeFPrimitive *const p)
{
    MeFPrimitiveNode *node;
    
    /* don't insert twice */
    if (p->geometry && p->geometry == geometry)
        return;

    /* if in a different asset, remove it */
    if (p->geometry)
        MeFGeometryRemovePrimitive(p);
    
    node = MeMemoryAPI.create(sizeof(MeFPrimitiveNode));
    node->current = p;
    
    {
        MeFPrimitiveNode *temp;
        MeFPrimitive *tempPrim;
        temp = geometry->nilPrimitive;
        while ((tempPrim = temp->next->current) && (strcmp(tempPrim->id, p->id) > 0))
            temp = temp->next;

        LIST_INSERT(node, temp);
    }
    p->geometry = geometry;
    geometry->primCount++;
}

/**
 * Remove a primitive geometry from the geometry.
 */
void MEAPI MeFGeometryRemovePrimitive(MeFPrimitive *const p)
{
    MeFPrimitiveNode *node;

    if (!p->geometry)
    {
        ME_REPORT(MeWarning(3, "You attempted to remove a primitive geometry "
            "that wasn't inserted."));
        return;
    }

    LIST_REMOVE(node, p->geometry->nilPrimitive, p);

    MeMemoryAPI.destroy(node);
    p->geometry->primCount--;
    p->geometry = 0;
}

/**
 * Scales a geometry by the specified scale factor. Also scales the graphic scale.
 */
void MEAPI MeFGeometryScale(MeFGeometry *const geometry, MeReal scale)
{
    MeFPrimitiveIt it;
    MeFPrimitive *p;
    MeFGeometryInitPrimitiveIterator(geometry, &it);
    
    while (p = MeFGeometryGetPrimitive(&it))
        MeFPrimitiveScale(p, scale);

    geometry->graphicScale *= scale;
    MeVector3Scale(geometry->graphicOffset, scale);
}

/**
 * Scales a geometry by the specified scale factor. Does NOT scale the graphic.
 */
void MEAPI MeFGeometryScaleNoGraphic(MeFGeometry *const geometry, MeReal scale)
{
    MeFPrimitiveIt it;
    MeFPrimitive *p;
    MeFGeometryInitPrimitiveIterator(geometry, &it);
    
    while (p = MeFGeometryGetPrimitive(&it))
        MeFPrimitiveScale(p, scale);
}



/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:05 $ - Revision: $Revision: 1.36.2.8 $

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
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeMath.h>
#include "MeAssetDBInternal.h"

/*---------------------------- creation and destruction ------------- */

/** @internal */
static void MEAPI _MeFAssetPartInit(MeFAssetPart *const p)
{
    p->id = 0;
    p->asset = 0;
    p->graphicHint = 0;
    p->model = 0;
    p->parent = 0;
    MeMatrix4TMMakeIdentity(p->tm);
    p->graphicScale = 1;
    MeVector3Set(p->graphicOffset, 0, 0, 0);
    p->index = -1;
}

/**
 * Create an asset part.
 */
MeFAssetPart *MEAPI MeFAssetPartCreate(const char *const name, MeFModel *const model, const MeMatrix4Ptr relTM)
{
    MeFAssetPart *p = (MeFAssetPart*)MeMemoryAPI.createZeroed(sizeof(MeFAssetPart));

    _MeFAssetPartInit(p);

    _FSetStringProperty(&p->id, name);
    _FSetStringProperty(&p->model, model->id);
    
    if (relTM)
    {
        if (!MeMatrix4IsTM(relTM, ME_MEDIUM_EPSILON))
        {
            ME_REPORT(MeWarning(3, "MeFAssetPartCreate: Invalid transform."));
        }
        MeMatrix4Copy(p->tm, relTM);
    }

    return p;
}

/**
 * Return a copy of the asset part. The returned copy must be inserted into an
 * asset.
 */
MeFAssetPart *MEAPI MeFAssetPartCreateCopy(const MeFAssetPart *const part, MeBool recurse)
{
    MeFAssetPart *copy = (MeFAssetPart*)MeMemoryAPI.createZeroed(sizeof(MeFAssetPart));

    _FSetStringProperty(&copy->id, part->id);
    _FSetStringProperty(&copy->model, part->model);
    MeMatrix4Copy(copy->tm, part->tm);
    _FSetStringProperty(&copy->graphicHint, part->graphicHint);
    _FSetStringProperty(&copy->parent, part->parent);
    copy->graphicScale = part->graphicScale;
    MeVector3Set(copy->graphicOffset, part->graphicOffset[0], part->graphicOffset[1], part->graphicOffset[2]);
    copy->index = -1;

    return copy;
}

/** 
 * Destroys an asset part.
 */
void MEAPI MeFAssetPartDestroy(MeFAssetPart *const p)
{
    MeMemoryAPI.destroy(p->id);
    MeMemoryAPI.destroy(p->model);

    if (p->graphicHint)
        MeMemoryAPI.destroy(p->graphicHint);

    if (p->parent)
        MeMemoryAPI.destroy(p->parent);

    MeMemoryAPI.destroy(p);
}

/*---------------------------- MeFAssetPart accessors --------------------------- */

/**
 * Returns the name of the asset part.
 */
char *MEAPI MeFAssetPartGetName(const MeFAssetPart *const p)
{
    return p->id;
}

/**
 * Returns the name of the model associated with the asset part.
 */
char *MEAPI MeFAssetPartGetModelName(const MeFAssetPart *const p)
{
    return p->model;
}

/**
 * Returns the model associated with the asset part.
 */
MeFModel *MEAPI MeFAssetPartGetModel(const MeFAssetPart *const p)
{
    MeFModel *m = MeFAssetLookupModel(p->asset, p->model);
    return m;
}

/**
 * Returns the geometry associated with the asset part's model.
 * The asset part must have been inserted into the asset for
 * this to work.
 */
MeFGeometry *MEAPI MeFAssetPartGetGeometry(const MeFAssetPart *const p)
{
    MeFModel *m;
    MeFGeometry *g = 0;
    
    if (!p->asset)
        return 0;

    m = MeFAssetLookupModel(p->asset, p->model);

    if (m->geometry)
        g = MeFAssetLookupGeometry(p->asset, m->geometry);

    return g;
}

/**
 * Returns the transform of the asset part.
 */
MeMatrix4Ptr MEAPI MeFAssetPartGetTransformPtr(MeFAssetPart *const p)
{
    MEASSERT(MeMatrix4IsTM(p->tm, (MeReal)0.001));
    return p->tm;
}

/**
 * Returns the position of the asset part.
 */
void MEAPI MeFAssetPartGetPosition(const MeFAssetPart *const part, MeVector3 pos)
{
    MeVector3Copy(pos, part->tm[3]);
}

/**
 * Returns the asset part's graphic (hint).
 */
char *MEAPI MeFAssetPartGetGraphicHint(const MeFAssetPart *const p)
{
    return p->graphicHint;
}

/**
 * Returns the scale factor associated with the asset part's graphic. Defaults to 1.
 */
MeReal MEAPI MeFAssetPartGetGraphicScale(const MeFAssetPart *const p)
{
    return p->graphicScale;
}

/**
 * Yields the offset of the graphic associated with this part.
 */
void MEAPI MeFAssetPartGetGraphicOffset(const MeFAssetPart *const p, MeVector3 v)
{
    MeVector3Copy(v, p->graphicOffset);
}

/**
 * Returns the part's parent part. The asset part must be inserted into an asset
 * for this to work. Returns 0 if the asset part is not in an asset.
 */
MeFAssetPart *MEAPI MeFAssetPartGetParentPart(const MeFAssetPart *const part)
{
    MeFAssetPart *p = 0;

    if (!part->asset)
        return 0;

    if (part && part->parent)
        p = MeFAssetLookupPart(part->asset, part->parent);
    
    return p;
}

/**
 * Returns the name of the part's parent part.
 */
char *MEAPI MeFAssetPartGetParentPartName(const MeFAssetPart *const part)
{
    return part->parent;
}

/** @internal **/
static void _GetGridLocation(const MeFAssetPart *const p1, const MeFAssetPart *const p2, MeU32 **p, MeU32 *mask)
{
    int index, block, stride, row, col, size;

    row = p1->index;
    col = p2->index;
    size = p1->asset->maxParts;

    if (row > col)
    {
        col = p1->index;
        row = p2->index;
    }

    MEASSERT(row < col);

    index = row * size + col; 
    block = index / 32;
    
    MEASSERT(block <= (size * size)/32);
    
    stride = index % 32;
    *mask = 0x80000000;
    *mask >>= stride;

    *p = p1->asset->disabledColArray + block;
}

/**
 * Returns true if collision detection will occur between the specified parts, false if not.
 * Returns true if @param p1 and @param p2 point top the same part.
 */
MeBool MEAPI MeFAssetPartIsCollisionEnabled(const MeFAssetPart *const p1, MeFAssetPart *const p2)
{
    MeU32 *p, mask;
    
    if (p1 == p2)
        return 1;

    _GetGridLocation(p1, p2, &p, &mask);

    MEASSERT(mask);

    return !(*p & mask);
}

/**
 * Returns the part's index into the disabled collision grid. Useful when
 * iterating over disabled parts.
 */
int MEAPI MeFAssetPartGetDisabledCollisionIndex(const MeFAssetPart *const part)
{
    return part->index;
}

/*---------------------------- MeFAssetPart mutators --------------------------- */

/**
 * Renames an MeFAssetPart. If the part is inserted into an asset, all
 * references will be resolved - ie any joints referencing this part
 * will have their references updated, as will any parent part
 * references.
 */
void MEAPI MeFAssetPartRename(MeFAssetPart *const part, const char *const newName)
{
    if (strcmp(part->id, newName) != 0)
    {
        if (part->asset)
        {
            MeFAssetResolvePartReferences(part->asset, part->id, newName);
            MeHashDelete(part->id, part->asset->nameToPart);
        }

        _FSetStringProperty(&part->id, newName);

        if (part->asset)
            MeHashInsert(part->id, part, part->asset->nameToPart);
    }
}

/**
 * Sets the asset's graphic (hint).
 */
void MEAPI MeFAssetPartSetGraphicHint(MeFAssetPart *const part, const char *const hint)
{
    _FSetStringProperty(&part->graphicHint, hint);
}

/**
 * Sets the asset's graphic scale.
 */
void MEAPI MeFAssetPartSetGraphicScale(MeFAssetPart *const part, MeReal scale)
{
    part->graphicScale = scale;
}

/**
 * Scales the asset part by the specified scale factor. Also scales the part's associated
 * graphic scale.
 */
void MEAPI MeFAssetPartScale(MeFAssetPart *const part, MeReal scale)
{
    MeVector3Scale(part->tm[3], scale);
    part->graphicScale *= scale;
    MeVector3Scale(part->graphicOffset, scale);
}

/**
 * Sets the offset of the graphic associated with the part.
 */
void MEAPI MeFAssetPartSetGraphicOffset(MeFAssetPart *const part, MeReal dx, MeReal dy, MeReal dz)
{
    MeVector3Set(part->graphicOffset, dx, dy, dz);
}

/**
 * Sets the part's transform.
 */
void MEAPI MeFAssetPartSetTransform(MeFAssetPart *const part, const MeMatrix4Ptr tm)
{
    MeMatrix4Copy(part->tm, tm);
    MeMatrix4TMOrthoNormalize(part->tm);

    if (!MeMatrix4IsTM(tm, ME_MEDIUM_EPSILON))
    {
        ME_REPORT(MeWarning(3, "MeFAssetPartSetTransform: Invalid transform."));
    }
}

/** 
 * Sets the part's position.
 */
void MEAPI MeFAssetPartSetPosition(MeFAssetPart *const part, MeReal x, MeReal y, MeReal z)
{
    MeVector3Set(part->tm[3], x, y, z);
}

/**
 * Sets the parent part of the part. A part can only have one parent.
 */
void MEAPI MeFAssetPartSetParentPart(MeFAssetPart *const part, const MeFAssetPart *const parent)
{
    if(parent)
        MeFAssetPartSetParentPartByName(part, parent->id);
    else
        MeFAssetPartSetParentPartByName(part, 0);
}

/**
 * Sets the part's parent by name.
 */
void MEAPI MeFAssetPartSetParentPartByName(MeFAssetPart *const part, const char *const name)
{
    _FSetStringProperty(&part->parent, name);
}

/**
 * Sets the part's model.
 */
void MEAPI MeFAssetPartSetModel(MeFAssetPart *const part, const MeFModel *const model)
{
    if (model)
        MeFAssetPartSetModelByName(part, model->id);
}

/**
    Sets the part's model by name.
*/
void MEAPI MeFAssetPartSetModelByName(MeFAssetPart *const part, const char *const name)
{
    _FSetStringProperty(&part->model, name);
}

/**
 * Enables or disables collision between the specified parts. Doesn't do
 * anything if @param p1 and @param p2 point to the same part.
 */
void MEAPI MeFAssetPartEnableCollision(const MeFAssetPart *const p1, const MeFAssetPart *const p2, MeBool enable)
{
    MeU32 *p, mask;
    
    if (p1 == p2)
        return;

    if (p1->asset && p1->asset->partCount > p1->asset->maxParts)
    {
        ME_REPORT(MeWarning(0, "Max parts exceeded. Cannot enable/disable collision"));
        return;
    }

    _GetGridLocation(p1, p2, &p, &mask);

    if (enable)
        *p &=~mask;
    else
        *p |= mask;
}

/**
 * Optionally called in MeFAssetRemovePart() to remove all disabled collisions for
 * this part from the disabled collisions grid.
 */
void MEAPI MeFAssetPartEnableAllCollisions(const MeFAssetPart *const part)
{
    MeFAssetPartIt it;
    MeFAssetPart *p;
    
    if (!part->asset)   /* not inserted */
        return;

    MeFAssetInitPartIterator(part->asset, &it);
    
    while (p = MeFAssetGetPart(&it))
        MeFAssetPartEnableCollision(p, part, 1);
    
}



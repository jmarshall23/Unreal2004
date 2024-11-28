/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/11 17:08:27 $ - Revision: $Revision: 1.64.2.13 $

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
#include <ctype.h>
#include <MeAssetDB.h>
#include "MeAssetDBInternal.h"
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeMath.h>
#include <MeHash.h>

/*----------------------- Creation and destruction ---------------------- */

/** @internal */
static void MEAPI _MeFAssetInit(MeFAsset *asset)
{
    asset->id = 0;
    asset->name = 0;
    asset->db = 0;
    asset->graphicHint = 0;
    asset->refPart = 0;
    asset->graphicScale = 1;
    asset->geomCount = 0;
    asset->modelCount = 0;
    asset->partCount = 0;
    asset->jointCount = 0;
    asset->massScale = 1;
    asset->lengthScale = 1;
    LIST_INIT_SENTINEL(asset->nilGeometry);
    LIST_INIT_SENTINEL(asset->nilModel);
    LIST_INIT_SENTINEL(asset->nilPart);
    LIST_INIT_SENTINEL(asset->nilJoint);
    MeIDPoolReset(asset->disabledColIndexPool);
    memset(asset->disabledColArray, '\0', (asset->maxParts * asset->maxParts)/8);
}

/**
 * Creates an MeFAsset with the specified name and ID. The caller is
 * responsible for providing a unique ID. This can be generated using
 * the MeIDPool utility. The ID pool owns the ID memory and should be
 * used to free this memory.
 */
MeFAsset *MEAPI MeFAssetCreate(const char *const name, int id)
{
    MeFAsset *asset = (MeFAsset*)MeMemoryAPI.createZeroed(sizeof(MeFAsset));
    asset->nilGeometry = (MeFGeometryNode*)MeMemoryAPI.create(sizeof(MeFGeometryNode));
    asset->nilModel = (MeFModelNode*)MeMemoryAPI.create(sizeof(MeFModelNode));
    asset->nilPart = (MeFAssetPartNode*)MeMemoryAPI.create(sizeof(MeFAssetPartNode));
    asset->nilJoint = (MeFJointNode*)MeMemoryAPI.create(sizeof(MeFJointNode));
    asset->nameToGeometry = MeHashCreate(17);
    asset->nameToModel = MeHashCreate(17);
    asset->nameToPart = MeHashCreate(17);
    asset->nameToJoint = MeHashCreate(17);
    asset->maxParts = 256;
    asset->disabledColArray = MeMemoryAPI.createZeroed((asset->maxParts * asset->maxParts)/8);
    asset->disabledColIndexPool = MeIDPoolCreate();

    _MeFAssetInit(asset);

    _FSetStringProperty(&asset->name, name);
    asset->id = id;

    return asset;
}

/**
 * Create and return a copy of the specified asset. The copy will not be associated with
 * an asset database and must be inserted into one. The copy will have the same
 * name and ID as the original. If the copy is inserted into the same database
 * as the original, the ID must be made unique. This can be done using the
 * MeIDPool utility. @param recurse indicates whether the copy operation
 * should extend to children (parts and joints etc).
 */
MeFAsset *MEAPI MeFAssetCreateCopy(const MeFAsset *const asset, MeBool recurse)
{
    MeFAsset *copy = MeFAssetCreate(asset->name, asset->id);

    _FSetStringProperty(&copy->refPart, asset->refPart);
    _FSetStringProperty(&copy->graphicHint, asset->graphicHint);
    copy->graphicScale = asset->graphicScale;
    copy->massScale = asset->massScale;
    copy->lengthScale = asset->lengthScale;

    if (recurse)
    {
        {
            MeFJoint *joint;
            MeFJointIt it;
            MeFAssetInitJointIterator(asset, &it);

            while (joint = MeFAssetGetJoint(&it))
            {
                MeFJoint *copyJoint;
                copyJoint = MeFJointCreateCopy(joint, recurse);
                MeFAssetInsertJoint(copy, copyJoint);
            }
        }
    
        /* copy parts and disabled collisions */
        {
            int i;
            int count = MeFAssetGetPartCount(asset);
            MeFAssetPart **origArray = (MeFAssetPart**)MeMemoryALLOCA(sizeof(MeFAssetPart*) * count);
            MeFAssetPart **copyArray = (MeFAssetPart**)MeMemoryALLOCA(sizeof(MeFAssetPart*) * count);
            MeFAssetGetPartsSortedByName(asset, origArray);

            for (i = 0; i < count; i++)
            {
                MeFAssetPart *copyPart = MeFAssetPartCreateCopy(origArray[i], recurse);
                MeFAssetInsertPart(copy, copyPart);
                copyArray[i] = copyPart;
            }

            for (i = 0; i < count; i++)
            {
                int j;
                for (j = i + 1; j < count; j++)
                {
                    if (!MeFAssetPartIsCollisionEnabled(origArray[i], origArray[j]))
                    {
                        MeFAssetPartEnableCollision(copyArray[i], copyArray[j], 0);
                    }
                }
            }
        }

        {
            MeFModel *model;
            MeFModelIt it;
            MeFAssetInitModelIterator(asset, &it);

            while (model = MeFAssetGetModel(&it))
            {
                MeFModel *copyModel;
                copyModel = MeFModelCreateCopy(model, recurse);
                MeFAssetInsertModel(copy, copyModel);
            }
        }

        {
            MeFGeometry *geometry;
            MeFGeometryIt it;
            MeFAssetInitGeometryIterator(asset, &it);

            while (geometry = MeFAssetGetGeometry(&it))
            {
                MeFGeometry *copyGeom;
                copyGeom = MeFGeometryCreateCopy(geometry, recurse);
                MeFAssetInsertGeometry(copy, copyGeom);
            }
        }
    }

    return copy;
}

/**
 * Destroys an MeFAsset. If the asset is inserted in an asset database
 * it must be removed before it can be destroyed.
 */
void MEAPI MeFAssetDestroy(MeFAsset *const asset)
{
    {
        MeFJointIt it;
        MeFJoint *joint;
        MeFAssetInitJointIterator(asset, &it);
    
        while (joint = MeFAssetGetJoint(&it))
        {
            MeFAssetRemoveJoint(joint);
            MeFJointDestroy(joint);
            MeFAssetInitJointIterator(asset, &it);
        }
    }

    {
        MeFAssetPartIt it;
        MeFAssetPart *part;
        MeFAssetInitPartIterator(asset, &it);
    
        while (part = MeFAssetGetPart(&it))
        {
            MeFAssetRemovePart(part);
            MeFAssetPartDestroy(part);
            MeFAssetInitPartIterator(asset, &it);
        }
        MEASSERT(MeIDPoolIsEmpty(asset->disabledColIndexPool));
    }

    {
        MeFModelIt it;
        MeFModel *model;
        MeFAssetInitModelIterator(asset, &it);
    
        while (model = MeFAssetGetModel(&it))
        {
            MeFAssetRemoveModel(model);
            MeFModelDestroy(model);
            MeFAssetInitModelIterator(asset, &it);
        }
    }

    {
        MeFGeometryIt it;
        MeFGeometry *geometry;
        MeFAssetInitGeometryIterator(asset, &it);
    
        while (geometry = MeFAssetGetGeometry(&it))
        {
            MeFAssetRemoveGeometry(geometry);
            MeFGeometryDestroy(geometry);
            MeFAssetInitGeometryIterator(asset, &it);
        }
    }

    if (asset->name)
        MeMemoryAPI.destroy(asset->name);

    if (asset->graphicHint)
        MeMemoryAPI.destroy(asset->graphicHint);

    if (asset->refPart)
        MeMemoryAPI.destroy(asset->refPart);

    MeMemoryAPI.destroy(asset->nilGeometry);
    MeMemoryAPI.destroy(asset->nilModel);
    MeMemoryAPI.destroy(asset->nilPart);
    MeMemoryAPI.destroy(asset->nilJoint);
    MeHashDestroy(asset->nameToGeometry);
    MeHashDestroy(asset->nameToModel);
    MeHashDestroy(asset->nameToPart);
    MeHashDestroy(asset->nameToJoint);
    MeIDPoolDestroy(asset->disabledColIndexPool);
    MeMemoryAPI.destroy(asset->disabledColArray);
    MeMemoryAPI.destroy(asset);
}

/**
 * Combines the two assets. It does not matter if there are any name clashes because
 * they will be resolved. The source asset IS changed.
 */
void MEAPI MeFAssetCombine(MeFAsset *const dest, MeFAsset *const source)
{
    /*
     * Each element of the source asset will be checked to see if
     * its name clashes with an element in the destination asset. If there is
     * a clash the element will be renamed to something unique to both assets, and all
     * the references resolved. Once this process is complete we will be left with
     * two assets with two valid assets with guaranteed unqiue names. The source
     * asset's elements will then be copied and inserted into the destination asset.
     * Finally the disabled collisions will be copied across. 
     */

    char buffer[256], *name;
    MeHash *hash = MeHashCreate(97);
    MeHashSetKeyCompareFunc(hash, MeHashIntCompare);

    {
        MeFGeometry *geom;
        MeFGeometryIt it;

        MeFAssetInitGeometryIterator(source, &it);

        while (geom = MeFAssetGetGeometry(&it))
        {
            name = MeFAssetMakeGeometryNameUnique(dest, source, geom->id, buffer, 256);
            MeFGeometryRename(geom, name);
        }
    }

    {
        MeFModel *model;
        MeFModelIt it;

        MeFAssetInitModelIterator(source, &it);

        while (model = MeFAssetGetModel(&it))
        {
            name = MeFAssetMakeModelNameUnique(dest, source, model->id, buffer, 256);
            MeFModelRename(model, name);
        }
    }
    {
        MeFAssetPart *part;
        MeFAssetPartIt it;

        MeFAssetInitPartIterator(source, &it);

        while (part = MeFAssetGetPart(&it))
        {
            name = MeFAssetMakePartNameUnique(dest, source, part->id, buffer, 256);
            MeFAssetPartRename(part, name);            


        }
    }
    {
        MeFJoint *joint;
        MeFJointIt it;

        MeFAssetInitJointIterator(source, &it);

        while (joint = MeFAssetGetJoint(&it))
        {
            name = MeFAssetMakeJointNameUnique(dest, source, joint->id, buffer, 256);
            MeFJointRename(joint, name);
        }
    }

    /*
     * The temporary asset is now valid, all renaming and reference resolution
     * issues having been taken care of.
     *
     * Now it is just a question of copying all the contents of the temporary
     * asset to the destination asset, and then resolving disabled collisions.
     */

    {
        MeFGeometry *geom;
        MeFGeometryIt it;

        MeFAssetInitGeometryIterator(source, &it);

        while (geom = MeFAssetGetGeometry(&it))
        {
            MeFGeometry *copy = MeFGeometryCreateCopy(geom, 1);
            MeFAssetInsertGeometry(dest, copy);
        }
    }

    {
        MeFModel *model;
        MeFModelIt it;

        MeFAssetInitModelIterator(source, &it);

        while (model = MeFAssetGetModel(&it))
        {
            MeFModel *copy = MeFModelCreateCopy(model, 1);
            MeFAssetInsertModel(dest, copy);
        }
    }
    {
        MeFAssetPart *part;
        MeFAssetPartIt it;

        MeFAssetInitPartIterator(source, &it);

        while (part = MeFAssetGetPart(&it))
        {
            MeFAssetPart *copy = MeFAssetPartCreateCopy(part, 1);
            MeFAssetInsertPart(dest, copy);
        }
    }
    {
        MeFJoint *joint;
        MeFJointIt it;

        MeFAssetInitJointIterator(source, &it);

        while (joint = MeFAssetGetJoint(&it))
        {
            MeFJoint *copy = MeFJointCreateCopy(joint, 1);
            MeFAssetInsertJoint(dest, joint);
        }
    }

    /* finally, copy across the disabled collisions */
    {
        MeFAssetPartIt it1, it2;
        MeFAssetPart *part1, *part2;
    
        MeFAssetInitPartIterator(source, &it1);
        while (part1 = MeFAssetGetPart(&it1))
        {
            MeFAssetInitPartIterator(source, &it2);
            while (part2 = MeFAssetGetPart(&it2))
            {
                MeFAssetPart *p1, *p2;
                if (MeFAssetPartGetDisabledCollisionIndex(part1) >= MeFAssetPartGetDisabledCollisionIndex(part2) ||
                    MeFAssetPartIsCollisionEnabled(part1, part2))
                    continue;
             
                p1 = MeFAssetLookupPart(dest, part1->id);
                p2 = MeFAssetLookupPart(dest, part2->id);
                
                MeFAssetPartEnableCollision(p1, p2, 0);
            }
        }
    }    

}

/*--------------------------------- Accessors ---------------------------------- */

/**
 * Returns an MeFAsset's name.
 */
char *MEAPI MeFAssetGetName(const MeFAsset *const asset)
{
    return asset->name;
}

/**
 * Returns an MeFAsset's unique ID.
 */
int MEAPI MeFAssetGetID(const MeFAsset *const asset)
{
    return asset->id;
}

/**
 * Returns the reference part of the asset.
 */
char *MEAPI MeFAssetGetReferencePart(const MeFAsset *const asset)
{   
    return asset->refPart;
}

/**
 * Returns the graphic (hint) associated with this asset.
 */
char *MEAPI MeFAssetGetGraphicHint(const MeFAsset *const asset)
{ 
    return asset->graphicHint;
}

/**
 * Returns the scale factor associated with the asset's graphic. Defaults to 1.
 */
MeReal MEAPI MeFAssetGetGraphicScale(const MeFAsset *const asset)
{
    return asset->graphicScale;
}

/**
 * Returns the number of geometries in the asset.
 */
int MEAPI MeFAssetGetGeometryCount(const MeFAsset *const asset)
{
    return asset->geomCount;
}

/**
 * Returns the number of models in the asset.
 */
int MEAPI MeFAssetGetModelCount(const MeFAsset *const asset)
{
    return asset->modelCount;
}

/**
 * Returns the number of joints in the asset.
 */
int MEAPI MeFAssetGetJointCount(const MeFAsset *const asset)
{
    return asset->jointCount;
}

/**
 * Returns the number of parts in the asset (joints are separate).
 */
int MEAPI MeFAssetGetPartCount(const MeFAsset *const asset)
{
    return asset->partCount;
}

/**
 * Initialize an iterator to traverse geometries within an asset.
 */
void MEAPI MeFAssetInitGeometryIterator(const MeFAsset *const asset, MeFGeometryIt *const it)
{
    it->node = asset->nilGeometry->prev;
}

/**
 * Return the geometry associated with the iterator and advance the iterator.
 */
MeFGeometry *MEAPI MeFAssetGetGeometry(MeFGeometryIt *const it)
{
    MeFGeometry *g;
    g = it->node->current;
    it->node = it->node->prev;
    return g;
}

/**
 * Initialize an iterator to traverse models within an asset.
 */
void MEAPI MeFAssetInitModelIterator(const MeFAsset *const asset, MeFModelIt *const it)
{
    it->node = asset->nilModel->prev;
}

/**
 * Return the model associated with the iterator and advance the iterator.
 */
MeFModel *MEAPI MeFAssetGetModel(MeFModelIt *const it)
{
    MeFModel *model;
    model = it->node->current;
    it->node = it->node->prev;
    return model;
}

/**
 * Initialize an iterator to traverse joints within an asset.
 */
void MEAPI MeFAssetInitJointIterator(const MeFAsset *const asset, MeFJointIt *const it)
{
    it->node = asset->nilJoint->prev;
}

/**
 * Return the joint associated with the iterator and advance the iterator.
 */
MeFJoint *MEAPI MeFAssetGetJoint(MeFJointIt *const it)
{
    MeFJoint *fj;
    fj = it->node->current;
    it->node = it->node->prev;
    return fj;
}

/**
 * Initialize an iterator to traverse parts within an asset.
 */
void MEAPI MeFAssetInitPartIterator(const MeFAsset *const asset, MeFAssetPartIt *const it)
{
    it->node = asset->nilPart->prev;
}

/**
 * Return the part associated with the iterator and advance the iterator.
 */
MeFAssetPart *MEAPI MeFAssetGetPart(MeFAssetPartIt *const it)
{
    MeFAssetPart *p;
    p = it->node->current; 
    it->node = it->node->prev;

    return p;
}

/**
 * Look up a geometry from an asset, by name.
 */
MeFGeometry *MEAPI MeFAssetLookupGeometry(const MeFAsset *const asset, char *const name)
{
    return MeHashLookup(name, asset->nameToGeometry);
}

/**
 * Look up a model from an asset, by name.
 */
MeFModel *MEAPI MeFAssetLookupModel(const MeFAsset *const asset, char *const name)
{
    return MeHashLookup(name, asset->nameToModel);
}

/**
 * Look up a joint from an asset, by name.
 */
MeFJoint *MEAPI MeFAssetLookupJoint(const MeFAsset *const asset, char *const name)
{
    return MeHashLookup(name, asset->nameToJoint);
}

/**
 * Look up a part from an asset, by name.
 */
MeFAssetPart *MEAPI MeFAssetLookupPart(const MeFAsset *const asset, char *const name)
{
    return MeHashLookup(name, asset->nameToPart);
}

static int _MeFGeometrySortFunc(const void *elem1, const void *elem2)
{
    MeFGeometry *geom1 = *(MeFGeometry**)elem1;
    MeFGeometry *geom2 = *(MeFGeometry**)elem2;
    return strcmp(geom1->id, geom2->id);
}

static int _MeFModelSortFunc(const void *elem1, const void *elem2)
{
    MeFModel *model1 = *(MeFModel**)elem1;
    MeFModel *model2 = *(MeFModel**)elem2;
    return strcmp(model1->id, model2->id);
}

static int _MeFAssetPartSortFunc(const void *elem1, const void *elem2)
{
    MeFAssetPart *part1 = *(MeFAssetPart**)elem1;
    MeFAssetPart *part2 = *(MeFAssetPart**)elem2;
    return strcmp(part1->id, part2->id);
}

static int _MeFJointSortFunc(const void *elem1, const void *elem2)
{
    MeFJoint *joint1 = *(MeFJoint**)elem1;
    MeFJoint *joint2 = *(MeFJoint**)elem2;
    return strcmp(joint1->id, joint2->id);
}

/**
 * Fills a user allocated array with MeFGeometry pointers, sorted on part name.
 */
void MEAPI MeFAssetGetGeometrySortedByName(const MeFAsset *const asset, MeFGeometry **geomArray)
{
    MeFGeometryIt it;
    MeFGeometry *geom;
    int i;

    MeFAssetInitGeometryIterator(asset, &it);

    for (i = 0; geom = MeFAssetGetGeometry(&it); i++)
    {
        geomArray[i] = geom;
    }

    qsort(geomArray, asset->geomCount, sizeof(MeFGeometry*), _MeFGeometrySortFunc);
}

/**
 * Fills a user allocated array with MeFModel pointers, sorted on part name.
 */
void MEAPI MeFAssetGetModelsSortedByName(const MeFAsset *const asset, MeFModel **modelArray)
{
    MeFModelIt it;
    MeFModel *model;
    int i;

    MeFAssetInitModelIterator(asset, &it);

    for (i = 0; model = MeFAssetGetModel(&it); i++)
    {
        modelArray[i] = model;
    }

    qsort(modelArray, asset->modelCount, sizeof(MeFModel*), _MeFModelSortFunc);
}

/**
 * Fills a user allocated array with MeFAssetPart pointers, sorted on part name.
 */
void MEAPI MeFAssetGetPartsSortedByName(const MeFAsset *const asset, MeFAssetPart **partArray)
{
    MeFAssetPartIt it;
    MeFAssetPart *part;
    int i;

    MeFAssetInitPartIterator(asset, &it);

    for (i = 0; part = MeFAssetGetPart(&it); i++)
    {
        partArray[i] = part;
    }

    qsort(partArray, asset->partCount, sizeof(MeFAssetPart*), _MeFAssetPartSortFunc);
}

/**
 * Fills a user allocated array with MeFJoint pointers, sorted on part name.
 */
void MEAPI MeFAssetGetJointsSortedByName(const MeFAsset *const asset, MeFJoint **jointArray)
{
    MeFJointIt it;
    MeFJoint *joint;
    int i;

    MeFAssetInitJointIterator(asset, &it);
    
    for (i = 0; joint = MeFAssetGetJoint(&it); i++)
    {
        jointArray[i] = joint;
    }

    qsort(jointArray, asset->jointCount, sizeof(MeFJoint*), _MeFJointSortFunc);
}

/*
 * If there is a numeric suffix, increment it, else start
 * a new one, beginning at 1.
 */
static void _IncrementNumericSuffix(char **name)
{
    MeBool done = 0;
    int count = 0, length = strlen(*name), last = length - 1;

    while (!done)
    {
        if (isdigit((*name)[last--]))
            count++;
        else
            done = 1;
    }

    if (count > 0)
    {
        char number[32];
        int n;
        strcpy(number, *name + length - count);
        n = atoi(number) + 1;
        sprintf(number, "%d", n);
        strcpy(*name + length - count, number);
    }
    else
    {
        strcat(*name, "1");
    }
}

/**
 * Takes a given name and makes it into a unique geometry name for a given asset. A second asset can
 * optionally be specified, in which case the geometry name will be unique to both the assets.
 */
char *MEAPI MeFAssetMakeGeometryNameUnique(const MeFAsset *const asset, const MeFAsset *const asset2, char *name, char *buffer, int bufLength)
{
    /*
     * This function loops until it can find a unique name. To generate a unique
     * name it strips any numeric characters from the end of the name and increments
     * the number. This is repeated until it is unique.
     */

    strncpy(buffer, name, bufLength);

    while (1)
    {
        MeFGeometry *geom = MeFAssetLookupGeometry(asset, buffer);
    
        if (!geom)
        {
            if (asset2)
                geom = MeFAssetLookupGeometry(asset2, buffer);
            
            if (!geom)
                return buffer;
        }

        _IncrementNumericSuffix(&buffer);
    }
}

/**
 * As above but for models.
 */
char *MEAPI MeFAssetMakeModelNameUnique(const MeFAsset *const asset, const MeFAsset *const asset2, char *name, char *buffer, int bufLength)
{
    strncpy(buffer, name, bufLength);

    while (1)
    {
        MeFModel *model = MeFAssetLookupModel(asset, buffer);
    
        if (!model)
        {
            if (asset2)
                model = MeFAssetLookupModel(asset2, buffer);
            
            if (!model)
                return buffer;
        }

        _IncrementNumericSuffix(&buffer);
    }
}

/**
 * As above but for parts.
 */
char *MEAPI MeFAssetMakePartNameUnique(const MeFAsset *const asset, const MeFAsset *const asset2, char *name, char *buffer, int bufLength)
{
    strncpy(buffer, name, bufLength);

    while (1)
    {
        MeFAssetPart *part = MeFAssetLookupPart(asset, buffer);
    
        if (!part)
        {
            if (asset2)
                part = MeFAssetLookupPart(asset2, buffer);
            
            if (!part)
                return buffer;
        }

        _IncrementNumericSuffix(&buffer);
    }
}

/**
 * As above but for joints.
 */
char *MEAPI MeFAssetMakeJointNameUnique(const MeFAsset *const asset, const MeFAsset *const asset2, char *name, char *buffer, int bufLength)
{
    strncpy(buffer, name, bufLength);

    while (1)
    {
        MeFJoint *joint = MeFAssetLookupJoint(asset, buffer);
    
        if (!joint)
        {
            if (asset2)
                joint = MeFAssetLookupJoint(asset2, buffer);
            
            if (!joint)
                return buffer;
        }

        _IncrementNumericSuffix(&buffer);
    }
}

/**
 * Returns true if there is no geometry, models, parts or joints in the asset.
 */
MeBool MEAPI MeFAssetIsEmpty(const MeFAsset *const asset)
{
    return asset->geomCount + asset->modelCount + asset->partCount + asset->jointCount == 0 ? 1 : 0;
}

MeReal MEAPI MeFAssetGetMassScale(const MeFAsset *const asset)
{
    return asset->massScale;
}

MeReal MEAPI MeFAssetGetLengthScale(const MeFAsset *const asset)
{
    return asset->lengthScale;
}

/*--------------------------------- Mutators ---------------------------------- */

/**
 * Renames the asset.
 */
void MEAPI MeFAssetRename(MeFAsset *const asset, const char *const name)
{
    _FSetStringProperty(&asset->name, name);
}

/**
 * Sets the asset's unique identifier. It is the caller's responsibility to
 * supply a unique ID using the MeIDPool utility.
 */
void MEAPI MeFAssetSetID(MeFAsset *const asset, int id)
{
    asset->id = id;
}

/**
 * Sets the reference part for the asset.
 */
void MEAPI MeFAssetSetReferencePart(MeFAsset *const asset, const char *const refPart)
{
    _FSetStringProperty(&asset->refPart, refPart);
}

/**
 * Sets a graphic hint for this asset.
 */
void MEAPI MeFAssetSetGraphicHint(MeFAsset *const asset, const char *const hint)
{
    _FSetStringProperty(&asset->graphicHint, hint);
}

/**
 * Sets a the scale of the asset's graphic.
 */
void MEAPI MeFAssetSetGraphicScale(MeFAsset *const asset, MeReal scale)
{
    asset->graphicScale = scale;
}

/**
 * Insert a geometry into an asset. If the geometry is already in a different asset
 * it will be removed and inserted into the new one.
 */
void MEAPI MeFAssetInsertGeometry(MeFAsset *const asset, MeFGeometry *const geometry)
{
    MeFGeometryNode *node;
    
    /* don't insert twice */
    if (geometry->asset && geometry->asset == asset)
        return;

    /* if in a different asset, remove it */
    if (geometry->asset)
        MeFAssetRemoveGeometry(geometry);
    
    node = MeMemoryAPI.create(sizeof(MeFGeometryNode));
    node->current = geometry;
    
    LIST_INSERT(node, asset->nilGeometry);
    MeHashInsert(geometry->id, geometry, asset->nameToGeometry);
    
    geometry->asset = asset;
    asset->geomCount++;
}

/**
 * Removes a geometry from an asset.
 */
void MEAPI MeFAssetRemoveGeometry(MeFGeometry *const geometry)
{
    MeFGeometryNode *node;

    if (!geometry->asset)
    {
        ME_REPORT(MeWarning(3, "You attempted to remove geometry '%s' "
            "that is not in an asset.", geometry->id));
        return;
    }

    LIST_REMOVE(node, geometry->asset->nilGeometry, geometry);
    MeHashDelete(geometry->id, geometry->asset->nameToGeometry);

    MeMemoryAPI.destroy(node);
    geometry->asset->geomCount--;
    geometry->asset = 0;
}

/**
 * Insert a model into an asset. If the model is already in a different asset
 * it will be removed and inserted into the new one.
 */
void MEAPI MeFAssetInsertModel(MeFAsset *const asset, MeFModel *const model)
{
    MeFModelNode *node;

    /* don't insert twice */
    if (model->asset && model->asset == asset)
        return;

    /* if in a different asset, remove it */
    if (model->asset)
        MeFAssetRemoveModel(model);
    
    
    node = MeMemoryAPI.create(sizeof(MeFModelNode));
    node->current = model;
    
    LIST_INSERT(node, asset->nilModel);
    MeHashInsert(model->id, model, asset->nameToModel);
    
    model->asset = asset;
    asset->modelCount++;
}

/**
 * Removes a model from an asset.
 */
void MEAPI MeFAssetRemoveModel(MeFModel *const model)
{
    MeFModelNode *node;

    if (!model->asset)
    {
        ME_REPORT(MeWarning(3, "You attempted to remove a model that wasn't inserted."));
        return;
    }

    LIST_REMOVE(node, model->asset->nilModel, model);
    MeHashDelete(model->id, model->asset->nameToModel);

    MeMemoryAPI.destroy(node);
    model->asset->modelCount--;
    model->asset = 0;
}

/**
 * Insert a joint into an asset. If the joint is already in a different asset
 * it will be removed and inserted into the new one.
 */
void MEAPI MeFAssetInsertJoint(MeFAsset *const asset, MeFJoint *const joint)
{
    MeFJointNode *node;

    /* don't insert twice */
    if (joint->asset && joint->asset == asset)
        return;

    /* if in a different asset, remove it */
    if (joint->asset)
        MeFAssetRemoveJoint(joint);
    
    node = MeMemoryAPI.create(sizeof(MeFJointNode));
    node->current = joint;
    
    LIST_INSERT(node, asset->nilJoint);
    MeHashInsert(joint->id, joint, asset->nameToJoint);
    
    joint->asset = asset;
    asset->jointCount++;
}

/**
 * Removes a joint from an asset.
 */
void MEAPI MeFAssetRemoveJoint(MeFJoint *const joint)
{
    MeFJointNode *node;

    if (!joint->asset)
    {
        ME_REPORT(MeWarning(3, "You attempted to remove a joint that wasn't inserted."));
        return;
    }

    LIST_REMOVE(node, joint->asset->nilJoint, joint);
    MeHashDelete(joint->id, joint->asset->nameToJoint);

    MeMemoryAPI.destroy(node);
    joint->asset->jointCount--;
    joint->asset = 0;
}

/**
 * Insert a part into an asset. If the part is already in a different asset
 * it will be removed and inserted into the new one.
 */
void MEAPI MeFAssetInsertPart(MeFAsset *const asset, MeFAssetPart *const part)
{
    MeFAssetPartNode *node;

    /* don't insert twice */
    if (part->asset && part->asset == asset)
        return;

    if (part->asset && part->asset->partCount >= part->asset->maxParts)
    {
        ME_REPORT(MeWarning(0, "Maximum parts exceeded"));
        return;
    }

    /* if in a different asset, remove it */
    if (part->asset)
        MeFAssetRemovePart(part);

    node = (MeFAssetPartNode*)MeMemoryAPI.create(sizeof(MeFAssetPartNode));
    node->current = part;

    LIST_INSERT(node, asset->nilPart);
    MeHashInsert(part->id, part, asset->nameToPart);

    part->asset = asset;

    part->index = MeIDPoolRequestID(asset->disabledColIndexPool);
    
    asset->partCount++;
}

/**
 * Removes a part from an asset and removes
 * the part from the disabled collisions table.
 */
void MEAPI MeFAssetRemovePart(MeFAssetPart *const part)
{
    MeFAssetPartNode *node;

    if (!part->asset)
    {
        ME_REPORT(MeWarning(3, "You attempted to remove a part that wasn't inserted."));
        return;
    }
    
    LIST_REMOVE(node, part->asset->nilPart, part);
    MeHashDelete(part->id, part->asset->nameToPart);

    MeMemoryAPI.destroy(node);

    MeFAssetPartEnableAllCollisions(part);
    MeIDPoolReturnID(part->asset->disabledColIndexPool, part->index);
    part->index = -1;

    part->asset->partCount--;
    MEASSERT(part->asset->partCount >= 0);
    part->asset = 0;
}

/**
 * Removes all geometries from an asset.
 */
void MEAPI MeFAssetRemoveAllGeometry(MeFAsset *const asset)
{
    MeFGeometry *geom;
    MeFGeometryIt it;
    
    MeFAssetInitGeometryIterator(asset, &it);
    while (geom = MeFAssetGetGeometry(&it))
    {
        MeFAssetRemoveGeometry(geom);
        MeFAssetInitGeometryIterator(asset, &it);
    }
}

/**
 * Removes all models from an asset.
 */
void MEAPI MeFAssetRemoveAllModels(MeFAsset *const asset)
{
    MeFModel *model;
    MeFModelIt it;
    
    MeFAssetInitModelIterator(asset, &it);
    while (model = MeFAssetGetModel(&it))
    {
        MeFAssetRemoveModel(model);
        MeFAssetInitModelIterator(asset, &it);
    }
}

/**
 * Removes all parts from an asset and updates the disabled
 * collisions grid.
 */
void MEAPI MeFAssetRemoveAllParts(MeFAsset *const asset)
{
    MeFAssetPart *part;
    MeFAssetPartIt it;
    
    MeFAssetInitPartIterator(asset, &it);
    while (part = MeFAssetGetPart(&it))
    {
        MeFAssetRemovePart(part);
        MeFAssetInitPartIterator(asset, &it);
    }
}

/**
 * Removes all joints from an asset.
 */
void MEAPI MeFAssetRemoveAllJoints(MeFAsset *const asset)
{
    MeFJoint *joint;
    MeFJointIt it;
    
    MeFAssetInitJointIterator(asset, &it);
    while (joint = MeFAssetGetJoint(&it))
    {
        MeFAssetRemoveJoint(joint);
        MeFAssetInitJointIterator(asset, &it);
    }
}

/**
 * Scales an asset by the specified scale factor. Affects the graphic scale.
 */
void MEAPI MeFAssetScale(MeFAsset *const asset, MeReal scale)
{
    MeFGeometryIt geomIt;
    MeFGeometry *geom;
    MeFModelIt modelIt;
    MeFModel *model;
    MeFAssetPartIt partIt;
    MeFAssetPart *part;
    MeFJointIt jointIt;
    MeFJoint *joint;

    /* scale all the geometry */

    MeFAssetInitGeometryIterator(asset, &geomIt);

    while (geom = MeFAssetGetGeometry(&geomIt))
        MeFGeometryScale(geom, scale);

    /* scale all the models */

    MeFAssetInitModelIterator(asset, &modelIt);

    while (model = MeFAssetGetModel(&modelIt))
        MeFModelScale(model, scale);

    /* scale all the parts */

    MeFAssetInitPartIterator(asset, &partIt);

    while (part = MeFAssetGetPart(&partIt))
        MeFAssetPartScale(part, scale);

    /* scale all the joints */

    MeFAssetInitJointIterator(asset, &jointIt);

    while (joint = MeFAssetGetJoint(&jointIt))
        MeFJointScale(joint, scale);

    asset->graphicScale *= scale;

}

/**
 * Resolves model->geometry references following geometry renaming.
 * Iterates over all models in the asset. Any model pointing to
 * a geometry with @param oldName will be changed to point to
 * @newName.
 */
void MEAPI MeFAssetResolveGeometryReferences(MeFAsset *const asset, const char *oldName, const char *newName)
{
    MeFModelIt it;
    MeFModel *model;

    MeFAssetInitModelIterator(asset, &it);

    while (model = MeFAssetGetModel(&it))
    {
        if (model->geometry && strcmp(model->geometry, oldName) == 0)
        {
            _FSetStringProperty(&model->geometry, newName);
        }
    }
}

/**
 * Same as MeFAssetResolveGeometryReferences except it iterates over
 * parts resolving the model references if necessary.
 */
void MEAPI MeFAssetResolveModelReferences(MeFAsset *const asset, const char *oldName, const char *newName)
{
    MeFAssetPartIt it;
    MeFAssetPart *part;

    MeFAssetInitPartIterator(asset, &it);

    while (part = MeFAssetGetPart(&it))
    {
        if (part->model && strcmp(part->model, oldName) == 0)
        {
            _FSetStringProperty(&part->model, newName);
        }
    }
}

/**
 * Same as MeFAssetResolveGeometryReferences except it iterates over
 * joints resolving the part references. It also resolves parent
 * part references.
 */
void MEAPI MeFAssetResolvePartReferences(MeFAsset *const asset, const char *oldName, const char *newName)
{
    MeFJointIt it;
    MeFJoint *joint;

    MeFAssetInitJointIterator(asset, &it);

    while (joint = MeFAssetGetJoint(&it))
    {
        if (joint->part[0] && strcmp(joint->part[0], oldName) == 0)
        {
            _FSetStringProperty(&joint->part[0], newName);
        }
        if (joint->part[1] && strcmp(joint->part[1], oldName) == 0)
        {
            _FSetStringProperty(&joint->part[1], newName);
        }
    }
    {
        MeFAssetPartIt it;
        MeFAssetPart *part;

        MeFAssetInitPartIterator(asset, &it);

        while (part = MeFAssetGetPart(&it))
        {
            if (part->parent && strcmp(part->parent, oldName) == 0)
            {
                _FSetStringProperty(&part->parent, newName);
            }
        }
    }
}

void MEAPI MeFAssetSetMassScale(MeFAsset *const asset, MeReal scale)
{
    asset->massScale = scale;
}

void MEAPI MeFAssetSetLengthScale(MeFAsset *const asset, MeReal scale)
{
    asset->lengthScale = scale;
}

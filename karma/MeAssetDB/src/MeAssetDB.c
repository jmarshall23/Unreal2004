/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:05 $ - Revision: $Revision: 1.59.2.7 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not`
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#include <string.h>
#include <stdio.h>
#include <MeAssetDB.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include "MeAssetDBInternal.h"

/*----------------------- Creation and destruction ---------------------- */

/**
 * Create a MeAssetDB object.
 */
MeAssetDB *MEAPI MeAssetDBCreate()
{
    MeAssetDB *db;
    db = (MeAssetDB*)MeMemoryAPI.createZeroed(sizeof(MeAssetDB));
    db->nilAsset = (MeFAssetNode*)MeMemoryAPI.create(sizeof(MeFAssetNode));
    LIST_INIT_SENTINEL(db->nilAsset);
    db->assetCount = 0;
    return db;
}

/**
 * Destroys the MeAssetDB object and all the MeFGeometrys, MeFModels and MeFModels contained within.
 */
void MEAPI MeAssetDBDestroy(MeAssetDB *const db)
{
    MeAssetDBDeleteContents(db);
    MeMemoryAPI.destroy(db->nilAsset);
    MeMemoryAPI.destroy(db);
}

/**
 * Cleans out the contents of the MeAssetDB object without destroying it.
 */
void MEAPI MeAssetDBDeleteContents(MeAssetDB *const db)
{
    MeFAsset *asset;
    MeFAssetIt assetIt;

    MeAssetDBInitAssetIterator(db, &assetIt);
    
    while (asset = MeAssetDBGetAsset(&assetIt))
    {
        MeAssetDBRemoveAsset(asset);
        MeFAssetDestroy(asset);
        MeAssetDBInitAssetIterator(db, &assetIt);
    }
}

/**
 * Creates a copy of an asset database.
 */
MeAssetDB *MEAPI MeAssetDBCreateCopy(const MeAssetDB *const db)
{
    MeFAssetIt assetIt;
    MeFAsset *asset;
    MeAssetDB *copyDB = MeAssetDBCreate();

    MeAssetDBInitAssetIterator(db, &assetIt);

    while (asset = MeAssetDBGetAsset(&assetIt))
    {
        MeFAsset *copy = MeFAssetCreateCopy(asset, 1);
        MeAssetDBInsertAsset(copyDB, copy);
    }
    return copyDB;
}

/**
 * Adds contents of one MeAssetDB object into another.
 */
void MEAPI MeAssetDBInsertCopy(MeAssetDB *const to, const MeAssetDB *const from)
{
    MeFAssetIt assetIt;
    MeFAsset *asset;

    MeAssetDBInitAssetIterator(from, &assetIt);

    while (asset = MeAssetDBGetAsset(&assetIt))
    {
        MeFAsset *copy = MeFAssetCreateCopy(asset, 1);
        MeAssetDBInsertAsset(to, copy);
    }
}

/*--------------------------------- Accessors ---------------------------------- */

/**
 * Returns true if there is nothing in the MeAssetDB object.
 */
MeBool MEAPI MeAssetDBIsEmpty(const MeAssetDB *const db)
{
    return db->assetCount == 0 ? 1 : 0;
}

/**
 * Returns the number of assets that have been inserted into the
 * database.
 */
int MEAPI MeAssetDBGetAssetCount(const MeAssetDB *const db)
{
    return db->assetCount;
}

/**
 * Initialize an iterator to traverse assets in the database.
 */
void MEAPI MeAssetDBInitAssetIterator(const MeAssetDB *const db, MeFAssetIt *const it)
{
    it->node = db->nilAsset->prev;
}

/**
 * Retrieves the next asset associated with the iterator.
 */
MeFAsset *MEAPI MeAssetDBGetAsset(MeFAssetIt *const it)
{
    MeFAsset *asset;
    asset = it->node->current;
    it->node = it->node->prev;
    return asset;
}

/**
 * Returns an asset, given it's unique ID.
 */
MeFAsset *MEAPI MeAssetDBLookupAsset(const MeAssetDB *const db, int id)
{
    MeFAssetIt it;
    MeFAsset *asset;
    MeAssetDBInitAssetIterator(db, &it);
    
    while (asset = MeAssetDBGetAsset(&it))
    {
        if (asset->id == id)
            return asset;
    }

    return 0;
}

/**
 * Returns an asset, given it's name. This will not work reliably if assets
 * are not uniquely named.
 */
MeFAsset *MEAPI MeAssetDBLookupAssetByName(const MeAssetDB *const db, const char *const name)
{
    MeFAssetIt it;
    MeFAsset *asset;
    MeAssetDBInitAssetIterator(db, &it);
    
    while (asset = MeAssetDBGetAsset(&it))
    {
        if (strcmp(asset->name, name) == 0)
            return asset;
    }

    return 0;
}
/*--------------------------------- Mutators ---------------------------------- */

/**
 * Insert an asset into the asset database. If the asset is already in a different
 * database it will be removed and inserted into the new one.
 */
void MEAPI MeAssetDBInsertAsset(MeAssetDB *const db, MeFAsset *const asset)
{
    MeFAssetNode *node;

    /* don't insert twice */
    if (asset->db && asset->db == db)
        return;

    /* if in a different asset, remove it */
    if (asset->db)
        MeAssetDBRemoveAsset(asset);

    asset->db = db;
    
    node = MeMemoryAPI.create(sizeof(MeFAssetNode));
    node->current = asset;
    
    {
        MeFAssetNode *temp;
        MeFAsset *tempAsset;
        temp = db->nilAsset;
        while ((tempAsset = temp->next->current) && (strcmp(tempAsset->name, asset->name) > 0))
            temp = temp->next;

        LIST_INSERT(node, temp);
    }
    
    db->assetCount++;
}

/**
 * Remove an asset from the asset database.
 */
void MEAPI MeAssetDBRemoveAsset(MeFAsset *const asset)
{
    MeFAssetNode *node;

    if (!asset->db)
    {
        ME_REPORT(MeWarning(3, "You attempted to remove an asset that wasn't inserted."));
        return;
    }

    LIST_REMOVE(node, asset->db->nilAsset, asset);

    MeMemoryAPI.destroy(node);

    asset->db->assetCount--;
    asset->db = 0;
}

/** @internal String property setting utility */
void MEAPI _FSetStringProperty(char **const data, const char *const newVal)
{
    if (*data)
    {
        MeMemoryAPI.destroy(*data);
        *data = 0;
    }

    if (newVal)
    {
        *data = MeMemoryAPI.create(strlen(newVal) + 1);
        strcpy(*data, newVal);
    }
}


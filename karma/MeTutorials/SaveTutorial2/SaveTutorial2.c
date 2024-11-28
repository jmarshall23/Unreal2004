/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:37 $ - Revision: $Revision: 1.11.8.3 $

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

/*
   Demonstrates loading an MstUniverse from a .me file hierarchy and then
   saving it again. The saved .me file will contain the whole hierarchy in
   one file.
 */

#include <Mst.h>
#include "MstLoadXML.h"
#include "MstSaveXML.h"
#include <MeMath.h>

MstUniverseID universe;

int MEAPI_CDECL main (int argc,const char *argv[])
{
    MstUniverseSizes sizes;
    MeHash *load_hash;
    MeHash *save_hash;
    MeStream stream;
    MeBool success;
    MeMatrix4 offset;
    int flags = 0;

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 100;
    sizes.dynamicConstraintsMaxCount = 100;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 100;
    sizes.collisionPairsMaxCount = 50;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);

    /* sets up a hash table with correct hashing function, key compare function,
       key free function and datum free function */
    load_hash = MeLoadHashCreate(97);

    /* first get something to save */
    stream = MeStreamOpen("cradle.me",kMeOpenModeRDONLY);

    MeMatrix4TMMakeIdentity(offset);

    success = MstUniverseLoad(universe,stream,load_hash,offset,0,0,flags);

    if (!success) {
        MeInfo(0,"Error loading file.");
    }

    MeStreamClose(stream);

    /* hash1 now contains all the items in the cradle.me hierarchy. These
       items can be looked up by name */

    save_hash = MeSaveHashCreate(97);

    /* converts a hash of string->pointer to a hash of pointer->string */
    MeHashConvert(load_hash,save_hash);

    /* As an alternative to the above you could call
       MstUniverseHashInsert(universe,hash2); */

    /* hash2 now contains all the items in the cradle.me hierarchy. These
       items can be looked up by ID */

    stream = MeStreamOpen("cradle_copy.me",kMeOpenModeWRONLY);

    /* MdtSave() will pick out all Mdt items from the hash table and
       save them */
    success = MstSave(save_hash,stream,0,0,0);

    /* could have specified McdSave() to just save geometry or
     * MstSave() to save both dynamics and geometry */

    MeStreamClose(stream);

    if (!success) {
        MeInfo(0,"Error saving file.");
    }

    MeHashDestroy(load_hash);
    MeHashDestroy(save_hash);


    return 0;
}

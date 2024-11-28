/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:37 $ - Revision: $Revision: 1.20.4.3 $

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
   Demonstrates basic saving functionality.
 */
#include <Mst.h>
#include <MstSave.h>
#include <MeXMLWrite.h>

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

/* Radius of each box. */
MeReal boxRadii[30][3] =
{
    {1.5, 0.3, 0.5}, {0.3, 1.0, 0.5}, {0.3, 1.0, 0.5},
    {0.4, 0.3, 0.4}, {0.5, 0.15, 0.5}, {0.4, 0.3, 0.4},

    {1.5, 0.3, 0.5}, {0.3, 1.0, 0.5}, {0.3, 1.0, 0.5},
    {1.0, 0.25, 0.5}, {0.2, 0.6, 0.3}, {0.2, 0.6, 0.3},

    {1.5, 0.3, 0.5}, {0.3, 1.0, 0.5}, {0.3, 1.0, 0.5},
    {0.5, 0.5, 0.5}, {0.3, 0.3, 0.3},

    {0.25, 1.5, 0.25}, {1.0, 0.25, 0.25}, {0.25, 0.7, 0.25},

    {0.25, 0.8, 0.25}, {0.25, 0.8, 0.25}, {0.25, 0.8, 0.25},
    {0.25, 0.8, 0.25}, {1.0, 0.2, 1.0}, {0.4, 2.0, 0.4},

    {0.4, 2.0, 0.4}, {0.4, 2.0, 0.4}, {0.4, 2.0, 0.4},
    {0.4, 2.0, 0.4}
};

/* Position of each box */
MeReal boxPos[30][3] =
{
    /* middle pile */
    {0.0, 1.35, -2.0}, {-1.0, 0.01, -2.0}, {1.0, 0.01, -2.0},
    {0.0, 1.98, -2.0}, {0.0, 2.45, -2.0}, {0.0, 3.0, -2.0},

    /* front pile */
    {0.0, 1.35, -8.0}, {-1.0, 0.01, -8.0}, {1.0, 0.01, -8.0},
    {0.0, 3.1, -8.0}, {-0.6, 2.20, -8.0}, {0.6, 2.20, -8.0},

    /* back pile */
    {0.0, 1.3, 6.0}, {-1.0, 0.01, 6.0}, {1.0, 0.01, 6.0},
    {0.0, 2.1, 6.0}, {0.0, 2.95, 6.0},

    /* cross */
    {6.0, 0.5, 0.0}, {6.0, 2.25, 0.0}, {6.0, 3.3, 0.0},

    /* table */
    {-7.6, -0.15, -0.7}, {-7.6, -0.15, 0.7}, {-6.4, -0.15, -0.7},
    {-6.4, -0.15, 0.7}, {-7.0, 0.8, 0.0},

    /* single pillars */
    {-6.0, 1.0, -6.0}, {-6.0, 1.0, 6.0}, {6.0, 1.0, -6.0},
    {6.0, 1.0, 6.0}, {0.0, 1.0, 12.0}
};

MeMatrix4 groundTM =
{
    {1,  0,  0, 0},
    {0,  0, -1, 0},
    {0,  1,  0, 0},
    {0, -1,  0, 1}
};

#define NBoxes      30
McdModelID box[NBoxes];

MeBool MEAPI SaveMyData(MeHash *hash,void *userdata,MeStream stream)
{
    int some_int = 3;
    McdModelID m = (McdModelID)userdata;
    char *model = McdModelLookupStringID(m,hash);

    MeXMLIndent();
    MeXMLWriteStartTag("MY_XML_ELEMENT",stream);
    MeXMLWrite("SOME_INT",stream,"%d",some_int);
    MeXMLWrite("MCDMODEL",stream,"%s",model);
    MeXMLWriteEndTag("MY_XML_ELEMENT",stream);
    MeXMLDeIndent();

    return 1;
}



int MEAPI_CDECL main (int argc,const char *argv[])
{
    MeSaveContext *sc;
    MeHash *hash;
    MeStream stream;
    MeBool success;
    MstUniverseID universe;
    MstUniverseSizes sizes;
    int i;
    McdModelID plane;
    McdGeometryID planeGeom;
    MstBridgeID bridge;
    MdtWorldID world;
    McdSpaceID space;
    McdGeometryID boxGeom[NBoxes];
    McdFrameworkID framework;

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 100;
    sizes.dynamicConstraintsMaxCount = 200;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 50;
    sizes.collisionPairsMaxCount = 1000;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);
    framework = MstUniverseGetFramework(universe);

    MdtWorldSetGravity(world, 0,-10,0);

    /* PLANE */
    planeGeom = McdPlaneCreate(framework);
    plane = MstFixedModelCreate(universe,planeGeom,groundTM);


    /* BOXES */
    for (i = 0; i < NBoxes; i++)
    {
        boxGeom[i] = McdBoxCreate(framework,2 * boxRadii[i][0], 2 * boxRadii[i][1],
            2 * boxRadii[i][2]);
        box[i] = MstModelAndBodyCreate(universe, boxGeom[i], 1);
        McdModelDynamicsSetDamping(box[i], 0.2, 0.1);
        McdModelDynamicsSetPosition(box[i], boxPos[i][0], boxPos[i][1], boxPos[i][2]);
    }


    /* sets up a hash table with correct hashing function, key compare function,
       key free function and datum free function */

    sc = MeSaveContextCreate();
    MeSaveContextSetUserdataHandler(sc,SaveMyData,(void*)box[0]);
    hash = MeSaveContextGetHash(sc);

    /* we are particularly interested in this model and body so insert them
       explicily */

    MstModelAndBodyHashInsert(box[0],"my_model","my_body",hash);

    /* add all the rest anonymously - we don't need handles to any of them
       when we load in */

    MstUniverseHashInsertAll(universe,hash);

    stream = MeStreamOpen("SaveTutorial1.me",kMeOpenModeWRONLY);

    success = MstSave(sc,stream);

    MeStreamClose(stream);

    if (!success) {
        MeInfo(0,"Error saving file.");
    }

    MeSaveContextDestroy(sc);
    MstUniverseDestroy(universe);

    return 0;
}

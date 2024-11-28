/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.3.2.1 $

   This software and its accompanying manuals have been developed
   by Mathengine PLC ("MathEngine") and the copyright and all other
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

#ifndef ASSETVIEWER2_H
#define ASSETVIEWER2_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <MeApp.h>
#include <MeAssetDB.h>
#include <MeAssetFactory.h>
#include <RMenu.h>

#include <MeAssetDBXMLIO.h>
#include <MeStream.h>
#include <MeIDPool.h>

MeCommandLineOptions    *options;
MstUniverseID           universe;
MstBridgeID             bridge;
McdSpaceID              space;
McdFrameworkID          frame;
MdtWorldID              world;
MeApp                   *app;
MeAssetDB               *db;
MeIDPool                *idPool;
MeAssetFactory          *af;
RRender                 *rc;
RMenu                   *mainMenu;
RMenu                   *visualsMenu;
RMenu                   *simMenu;
RMenu                   *constraintsMenu;
char                    *file;

McdGeometryID           planeGeom;
McdModelID              plane;
RGraphic                *groundG;

extern MeMatrix4 groundTM;

extern MeReal scale;
extern MeBool bNetInitDone;

/* simulation options */
extern MdtContactParamsID contactparams;
extern MeReal timestep;

/* visualization options */
extern int yIsUp;
extern int displayHelp;
extern int isPaused;

extern int drawmeshes;
extern int drawcollision;
extern int drawcontacts;
extern int showDynamicsOrigins;
extern int showMassOrigins;
extern int drawBSJoints;
extern int drawHinges;
extern int drawCarwheels;
extern int drawPrismatics;


void MEAPI_CDECL cleanup(void);

void ResetApp();
void FlushAssetDB();
void LoadAssetDB(MeStream stream);
void SetAssetDB(MeStream stream);

void FlushAll();

void MEAPI tick(RRender *rc,void *userdata);


#endif
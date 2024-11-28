/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:14 $ - Revision: $Revision: 1.5.6.1 $

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

#include <MeApp.h>
#include "visualization.h"

extern MstUniverse *universe;
extern MdtWorld *world;
extern MeReal timestep;
extern MdtContactParamsID contactparams;
extern MeApp *app;

void MEAPI SetGravityX(MeReal gx)
{
    MeVector3 g;
    MdtWorldGetGravity(world,g);
    MdtWorldSetGravity(world,-gx,g[1],g[2]);
}

void MEAPI SetGravityY(MeReal gy)
{
    MeVector3 g;
    MdtWorldGetGravity(world,g);
    MdtWorldSetGravity(world,g[0],-gy,g[2]);
}

void MEAPI SetGravityZ(MeReal gz)
{
    MeVector3 g;
    MdtWorldGetGravity(world,g);
    MdtWorldSetGravity(world,g[0],g[1],-gz);
}

void MEAPI SetTimestep(MeReal t)
{
    timestep = t;
}

void MEAPI SetFriction(MeReal f)
{
    MdtContactParamsSetFriction(contactparams, f);
    if (f == 0)
        MdtContactParamsSetType(contactparams, MdtContactTypeFrictionZero);
    else
        MdtContactParamsSetType(contactparams, MdtContactTypeFriction2D);
}


void MEAPI tick(RRender *rc,void *userdata)
{
    MstUniverseStep(universe,timestep);
    UpdateConstraintGraphics();
    MeAppStep(app);
}

void MEAPI single_step(RRender *rc,void *userdata)
{
    tick(rc,0);
}


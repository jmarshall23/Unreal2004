#ifndef _CARTERRAIN_H
#define _CARTERRAIN_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:33 $ - Revision: $Revision: 1.12.6.1 $

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

#include <Mdt.h>
#include <Mcd.h>
#include <Mst.h>
#include <MeViewer.h>

#include "CarData.h"

/*
  Structure containing car dynamics objects - bodies, joints and contacts
  and graphics objects.
*/

typedef struct Car
{
    /* wheels (bl,br,fl,fr) and chassis */
    MdtBodyID body[5];

    /* wheel collision models */
    McdModelID wheelCM[4];

    /* chasis collision models */
    McdModelID chasisCM;

    /* parameters specifying car dynamics and dimensions */
    CarData *params;

    /*
      Wheel constraints; rolling and steering contacts.
    */
    MdtCarWheelID wheel_joint[4];

    MeReal steering_angle;
    MeReal drive_torque;

    /* Graphics: depends on renderer */
    RGraphic *part[5];

    MeMatrix4 chassisTM;
} Car;

#endif

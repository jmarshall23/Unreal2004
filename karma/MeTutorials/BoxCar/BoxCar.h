#ifndef _BOXCAR_H
#define _BOXCAR_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:32 $ - Revision: $Revision: 1.10.6.1 $

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
#include <MeViewer.h>

#include "BoxCarData.h"

/* render context */
extern RRender *rc;

/* once-off initialization of renderer */
int  GraphicsInit(int argc, const char **argv);
/* final clean up of renderer */
void GraphicsDelete();
/* draw graphics for ground plane */
void GroundGraphics();

/*
  Structure containing car dynamics objects - bodies, joints and contacts
  and graphics objects.
*/
typedef struct Car
{

    /* the world the car is in */
    MdtWorldID world;
    /* wheels (bl,br,fl,fr) and chassis */
    MdtBodyID body[5];

    /* parameters specifying car dynamics and dimensions */
    CarData *params;

    /*
      Wheel constraints; rolling and steering contacts.
    */
    MdtCarWheelID wheel_joint[4];
    MdtContactID wheel_ground_contact[4];

    MeReal steering_angle;
    MeReal drive_torque;

    /* Graphics: depends on renderer */
    RGraphic *part[5];
    MeReal GraphicsTMatrix[5][16];
} Car;

void CarInit(Car * c, MdtWorld * w, const MeReal *grav, CarData * params);
void CarUpdate(Car * c);
void CarReset(Car * c, MeMatrix4 *TM);
const MeReal *CarGetTMatrix(Car * c, int i);
void CarInputSteeringAndDrive(Car * c);

void CarCreateGraphics(Car * c);
void CarCameraTrack(Car * c);
void CarGraphicsTransform(Car * c, const MeReal *m, MeReal *g);
void CarChassisGraphicsTransform(Car * c, const MeReal *m, MeReal *g);
void CarWheelGraphicsTransform(Car * c, const MeReal *m, MeReal *g);

#endif

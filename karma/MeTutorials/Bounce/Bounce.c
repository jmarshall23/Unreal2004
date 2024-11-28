/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:32 $ - Revision: $Revision: 1.37.6.3 $

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
Overview:

  Shows a ball dropping on a plane and bouncing. This example
  demonstrates the use of contacts with the Dynamics Toolkit,
  but without using the Collision Toolkit. Additional balls
  can be thrown by pressing the spacebar. Move the camera position
  to aim by line of sight.
*/

/* #include <malloc.h> IDW */
#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>
#include <MeViewer.h>

#define MASS_SCALE 1000
/* Render context */
RRender *rc;

/* ball graphic */
RGraphic *sphereG;

/* plane graphic */
RGraphic *planeG;

char *help[] =
{
  "$ACTION2 resets the simulation",
  "$ACTION3 shoots the ball"
};

float color[4] = { 1, 1, 1, 1 };  /* RGB+A color */

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;
/* dynamics body */
MdtBodyID body;
/* dynamics contact */
MdtContactID contact;

int numContacts = 0;

MeReal step = (MeReal)(0.03);

/*
Note that if you fire the ball from below the plane, the dynamics will
compensate for the penetration and the ball will shoot up out of the
plane.
*/

void MEAPI shoot(RRender *rc, void *userData)
{
  int i;
  MeReal u[3], v[3];
  MeReal norm;

  /*
  Place the ball at the 'eye' of the camera.
  */
  RCameraGetPosition(rc,v);
  MdtBodySetPosition(body,v[0], v[1], v[2]);

  RCameraGetLookAt(rc,u);
  for (i = 0; i < 3; i++)
    v[i] = u[i]-v[i];

  norm = MeRecipSqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

  v[0] *= norm;
  v[1] *= norm;
  v[2] *= norm;

  /*
  scale up velocity to make the shot more powerful
  */
  for (i = 0; i < 3; i++)
    v[i] *= 20;

  /* fire the ball */
  MdtBodySetLinearVelocity(body, v[0], v[1], v[2]);

  /* but don't give it any spin */
  MdtBodySetAngularVelocity(body, 0, 0, 0);

  MdtBodyEnable(body);
}


void MEAPI tick(RRender *rc, void *userData)
{
  MeVector3 pos;
  MeReal penetration;

  rc=rc; /* To ingore warnings */

         /*
         If there was a contact in the last timestep, remove it. We don't
         know whether we need another one during this timestep because the
         ball might have bounced.
  */
  if (contact)
  {
    MdtContactDisable(contact);
    MdtContactDestroy(contact);
    contact = 0;
  }

  /*
  Get the ball's position, which is the center of mass.
  */
  MdtBodyGetPosition(body, pos);

  /*
  Take into account it's radius to see if it has penetrated the
  plane.
  */

  /* plane height is zero */
  penetration = 0 - (pos[1] - (MeReal)(2.0));

  if (penetration > 0)
  {
    MdtContactParamsID params;
    /*
    It has so we need to create a contact.
    The second body is zero, which means that the ball is in
    contact with the static environment.
    */
    contact = MdtContactCreate(world);
    MdtContactSetBodies(contact,body,0);

    /*
    Set the contact point, which is in world coordinates.
    */
    MdtContactSetPosition(contact, pos[0], pos[1] - 2, pos[2]);

    MdtContactSetNormal(contact, 0, 1, 0);
    MdtContactSetPenetration(contact, penetration);

    /*
    Get material parameters for contact.
    */
    params = MdtContactGetParams(contact);

    /*
    2D friction means that the ball will roll on the plane as one
    would expect. If it was a zero friction contact then the ball
    would slide. 1D friction would mean that the ball rolls in one
    direction and slides in the other (perpendicular) direction.
    */
    MdtContactParamsSetType(params, MdtContactTypeFriction2D);

    MdtContactParamsSetFriction(params, (MeReal)(5.0));

    /*
    A value of 0.6 means that the contact is quite bouncy.
    */
    MdtContactParamsSetRestitution(params, (MeReal)(0.6));

    /*
    Tell Kea to process this contact.
    */
    MdtContactEnable(contact);
  }
  /*
  Process all bodies and constraints that are currently enabled.
  */

  MdtWorldStep(world, step);

}


void MEAPI reset(RRender *rc, void *userData)
{
  MdtBodySetPosition(body, 0, 30, 0);
  MdtBodySetLinearVelocity(body, 0, 0, 0);
  MdtBodySetAngularVelocity(body, 0, 0, 0);
  MdtBodySetQuaternion(body, 1, 0, 0, 0);
}

/*
Cleanup and exit
*/
void MEAPI_CDECL cleanup(void)
{
  MdtWorldDestroy(world);
  RRenderContextDestroy(rc);
}

/*
Main Routine
*/
int MEAPI_CDECL main(int argc, const char *(argv[]))
{

  MeMatrix3 I;
  MeVector3 v;
  float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  MeCommandLineOptions* options;

  /*
  Initialise renderer
  */

  options = MeCommandLineOptionsCreate(argc, argv);
  rc = RRenderContextCreate(options, 0, !MEFALSE);
  MeCommandLineOptionsDestroy(options);
  if (!rc)
    return 1;

  /*
  Initialise dynamics
  */

  /*
  Create a dynamics world with a maximum of 1 body, 1 constraint and
  1 material.
  */
  world = MdtWorldCreate(1, 1, 1, MASS_SCALE);

  MdtWorldSetGravity(world, 0, -(MeReal)(9.8), 0);

  body = MdtBodyCreate(world);

  /* The default is 1 anyway */
  MdtBodySetMass(body, MASS_SCALE);

  /*
  Add some damping so that the ball is eventually brought to rest and
  does not travel forever. When the ball is brought to rest it will be
  disabled and will not be processed by Kea until something moves
  it, which will enable again.
  */
  MdtBodySetLinearVelocityDamping(body, (MeReal)(0.2));

  MdtMakeInertiaTensorSphere(1, 1, I);
  MdtBodySetInertiaTensor(body, I);

  /*
  Set position, velocity, angular velocity and quaternion.
  */
  reset(rc,0);

  /*
  This body will now be continually processed by Kea until it is
  disabled.
  */
  MdtBodyEnable(body);

  /*
  Initialise rendering attributes.
  */

  RCameraSetView(rc, 40, 0.1f, 1.4f);
  v[0] = 4; v[1] = 0; v[2] = 0;
  RCameraSetLookAt(rc, v);


  /*
  Keyboard callbacks.
  */

  RRenderSetActionNCallBack(rc, 2, reset, 0);
  RRenderSetActionNCallBack(rc, 3, shoot, 0);

  /*
  On screen help text.
  */
  RRenderSetWindowTitle(rc,"Bounce tutorial");
  RPerformanceBarCreate(rc);

  RRenderCreateUserHelp(rc, help, 2);
  RRenderToggleUserHelp(rc);

  /*
  Create graphics.
  */

  sphereG = RGraphicSphereCreate(rc, 2, color, 0);
  RGraphicSetTexture(rc, sphereG, "ME_ball3");
  planeG = RGraphicGroundPlaneCreate(rc, 30.0f,30, white, 0);
  RGraphicSetTexture(rc, planeG, "checkerboard");

  /*
  Point the ball graphic's transformation at the updated dynamics
  transformation.
  */
  RGraphicSetTransformPtr(sphereG, MdtBodyGetTransformPtr(body));

  /*
  Run the Simulation.
  */

  atexit(cleanup);

  /*
  Here the tick() function is registered as being the function that the
  rendering loop will call continuously until the simulation is exited.

    Pseudocode: while no exit-request { Handle user input call Tick() to
    evolve the simulation and update graphic transforms Draw graphics }
  */

  RRun(rc, tick,0);

  return 0;
}

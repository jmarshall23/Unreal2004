/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:09 $ - Revision: $Revision: 1.41.6.2 $

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

    This example demonstrates Newton's Cradle. Four pendulums hang
    freely under gravity.


  Discussion:

    Ball and socket joints anchor the pendulums to the world.
    Pressing 1 applies a force to the first ball which sets the
    cradle in motion.
*/

#include <math.h>

#include <Mdt.h>
#include <MeViewer.h>
#include <MeMath.h>

#define N_PENDULUMS 5
#define N_CONTACTS 10

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;
/* Array of dynamics bodies  IDs */
MdtBodyID ball[N_PENDULUMS];
/* Array of ball and socket  joint  IDs */
MdtBSJointID bs[N_PENDULUMS];
/* Array of contact IDs */
MdtContactID contact[N_CONTACTS];

int contactsUsed = 0;

/* Render context */
RRender *rc;
/* body geometry */
RGraphic *sphereG[N_PENDULUMS];
RGraphic *lineG[N_PENDULUMS];
RGraphic *planeG;

char *help[] = { "$ACTION3 lifts the first ball" };

MeReal height = 5;

/* distance between spheres when they are at rest */
MeReal separation = (MeReal)(0.1);

/*
The three fields correspond to red/green/blue
*/
float blue[]   = { 0,      0,  1.1f, 1 };
float orange[] = { 1.0f, 0.4f, 0,    1 };
float black[]  = { 0.0f, 0.0f, 0,    1 };

/*
Initial line geometry. The lines will be given the same
transformation matrix as the balls so that they can rotate with them
*/
MeReal lineOrigin[] = { 0, 1, 0 };
MeReal lineEnd[] = { 0, 5, 0 };

/*
Timestep in seconds
*/
MeReal step = (MeReal)(0.04);

/*
This function determines if two spheres of specified radii have
collided. If they have a contact point is created and the necessary
contact parameters set.

  @param b1 first body to be tested
  @param b2 second body to be tested
  @param contactIndex index into contact array
  @param r1 radius of first sphere
  @param r2 radius of second sphere
*/

void sphereSphereCheck(MdtBodyID b1, MdtBodyID b2, int *contactIndex,
                       MeReal r1, MeReal r2)
{
  /*
  MeReal is typedef-ed to a float by default on Windows, Linux and
  PlayStation and to a double by default on Irix.
  */
  MeReal normal[3];
  MeReal separation;
  MeReal position[3];
  MeReal wA;
  MeReal wB;
  MeVector3 pos1, pos2;
  int i;

  MdtContactParamsID params;

  MdtBodyGetPosition(b1, pos1);
  MdtBodyGetPosition(b2, pos2);

  for (i = 0; i < 3; i++)
    normal[i] = pos1[i] - pos2[i];

  /*
  separation of sphere-centres
  */
  separation = MeSqrt(normal[0] * normal[0]
    + normal[1] * normal[1] + normal[2] * normal[2]);

  /*
  Normalize  the normal.
  */
  for (i = 0; i < 3; i++)
    normal[i] /= separation;

  /*
  Separation of spheres.
  */
  separation -= r1 + r2;

  /*
  Calculate position of point of contact.
  */
  wB = r1 / (r1 + r2);
  wA = r2 / (r1 + r2);
  for (i = 0; i < 3; i++)
    position[i] = pos1[i] * wA + pos2[i] * wB;

  if (separation < 0)
  {
    /*
    Set contact defaults. This must be the first function called on a
    contact. If it is not an error message will be generated.
    */

    MdtContactReset(contact[*contactIndex]);

    /*
    We tested b1 and b2 against each other to find out if their
    surfaces had penetrated (ie whether a collision had
    occurred. There has been penetration and we have created a
    contact.

    The contact needs to be told which two bodies are involved in the
    collision - this is done with the call to MdtContactSetBodies().

    One thing to note is that the second body in a contact can be set
    to zero. This means that the collision is happening between a
    body and the static environment.
    */
    MdtContactSetBodies(contact[*contactIndex], b1, b2);

    /*
    The point of contact in world coordinates
    */
    MdtContactSetPosition(contact[*contactIndex], position[0],
      position[1], position[2]);

    /*
    The contact normal. This must be of unit length
    */
    MdtContactSetNormal(contact[*contactIndex], normal[0],
      normal[1], normal[2]);

    /*
    Penetration is a measure of how much the two bodies have
    penetrated. If the value is negative then the surfaces of the two
    bodies will be some distance apart.
    */
    MdtContactSetPenetration(contact[*contactIndex], -separation);

    /*
    Get the constant, material properties for the contact.
    */
    params = MdtContactGetParams(contact[*contactIndex]);

    /*
    Here we are specifying a contact with zero friction between the
    two bodies. The friction type can also be 2D or 1D.
    */

    MdtContactParamsSetType(params, MdtContactTypeFrictionZero);

    /*
    Setting restitution on a contact determines how 'bouncy' a
    contact will be. The higher the value the more bouncy the contact
    will be.

    Values of greater than 1 will actually cause a gain in energy and
    would act like a spring in a pinball machine. By default
    restitution is zero.
    */
    MdtContactParamsSetRestitution(params, (MeReal)(0.9));

    /*
    Add the contact to the linked list of constraints to be processed
    by Kea the next time MdtWorldStep() is called.
    */
    MdtContactEnable(contact[*contactIndex]);

    /*
    Increment the contact counter so we know how many to disable
    before performing collision detection.
    */
    (*contactIndex)++;
  }
}


/*
Applies a force to the leftmost ball. Because forces are specified
in world coordinates we need to use the utility function
MdtConvertPositionVector() to convert the relative force we want to
apply into a vector in world coordinates.
*/
void MEAPI liftFirstBall(RRender *rc, void *userData)
{
  MeVector3 absForce;
  MeVector3 relForce = { -50, 0, 0 };

  MdtBodyEnable(ball[0]);

  MdtConvertPositionVector(ball[0], relForce, 0, absForce);

  /*
  MdtBodyAddForce() applies the force at the center of mass.

    You can add a force at a specific point by calling
    MdtBodyAddForceAtPosition().
  */
  MdtBodyAddForce(ball[0], absForce[0], absForce[1], absForce[2]);

}

/*
First tick() performs collision detection between all balls. Then
the world is stepped by 'step' seconds. Finally the new transformations
are passed to the renderer.
*/
void MEAPI tick(RRender * rc, void *userData)
{
  int i, j;

  /*
  The RStart/StopTimer function calls cause a performance bar to be
  displayed which gives a percentage breakdown of dynamics, collision
  and rendering activity. The performance bar only works in OpenGL
  */

  MeProfileStartSection("Collision", 1);

  /*
  Disable any contacts that were created in the last timestep
  */
  for (i = 0; i < contactsUsed; i++)
    MdtContactDisable(contact[i]);

  /*
  Reset contact count.
  */
  contactsUsed = 0;

  /*
  Test each sphere against every other sphere.
  */
  for (i = 0; i < N_PENDULUMS; i++)
    for (j = i + 1; j < N_PENDULUMS; j++)
      sphereSphereCheck(ball[i], ball[j], &contactsUsed, 1, 1);
    MeProfileStartSection ("Dynamics",0);

    MdtWorldStep(world, step);

    MeProfileEndSection ("Dynamics");
}

void MEAPI_CDECL cleanup(void)
{
  RRenderContextDestroy(rc);
  MdtWorldDestroy(world);
}

/*
Main routine.
*/

int MEAPI_CDECL main(int argc, const char *(argv[]))
{
  MeReal oneOverRootTwo = MeRecipSqrt(2);
  MeVector3 cameraLookAt;
  float color[4];
  MeReal CenteringOffset;
  MeCommandLineOptions *options;
  MeReal lengthScale=1.0;
  MeReal massScale=1.0;
  int i;
  

  /*
  Kea requires a memory area which is used to store contiguously packed
  arrays of body and constraint data. Here we are simply mallocing a
  fixed amount that we know is large enough. If for some reason Kea
  overflows this area an error message will be generated.
  */

  world = MdtWorldCreate(N_PENDULUMS, N_PENDULUMS + N_CONTACTS,
                         lengthScale, massScale);

  /*
  Setting gravity means that the specified force will be applied to
  every body in the world, every timestep.
  */
  MdtWorldSetGravity(world, 0, -(MeReal)(9.8), 0);

  /*
  Shift the pendula to be centered WRT the plane
  */
  CenteringOffset = (MeReal)((N_PENDULUMS-1) * (2 + separation) / 2.0);

  for (i = 0; i < N_PENDULUMS; i++)
  {
    ball[i] = MdtBodyCreate(world);

    /*
    A body's position is set in world coordinates.
    */

    MdtBodySetPosition(ball[i],
                       i * (2 + separation) - CenteringOffset,
                       height, 0);

    /*
    Adding velocity damping makes the simulation more physically
    realistic. Velocity damping acts very much like air resistance.
    */
    MdtBodySetLinearVelocityDamping(ball[i], (MeReal)(0.1));
  }

  /*
  Start the first pendulum raised.
  */
  MdtBodySetPosition(ball[0], -height-CenteringOffset, 2*height, 0);
  MdtBodySetQuaternion(ball[0],
    oneOverRootTwo, 0, 0, -oneOverRootTwo);

  for (i = 0; i < N_PENDULUMS; i++)
  {
    /*
    The second body parameter of zero means that the ball and
    socket joint is anchored to the static environment.
    */
    bs[i] = MdtBSJointCreate(world);
    MdtBSJointSetBodies(bs[i],ball[i],0);

    /*
    Set the ball and socket's position. This is specified in
    world coordinates.
    */
    MdtBSJointSetPosition(bs[i],
                          i * (2 + separation) - CenteringOffset,
                          2*height, 0);
  }

  /*
  Create a pool of contacts. The total number of contacts created
  is the maximum number of contacts that can be passed to Kea in
  any given timestep.
  */
  for (i = 0; i < N_CONTACTS; i++)
    contact[i] = MdtContactCreate(world);

  for (i = 0; i < N_PENDULUMS; i++)
  {
    MdtBodyEnable(ball[i]);
    MdtBSJointEnable(bs[i]);
  }

  /*
  Initialise rendering attributes.
  */

  options = MeCommandLineOptionsCreate(argc, argv);
  rc = RRenderContextCreate(options, 0, !MEFALSE);
  MeCommandLineOptionsDestroy(options);
  if (!rc)
    return 1;

  /*
  Shapes that can be created are spheres, lines, cubes, cones,
  cylinders and user-defined meshes. See "MeViewer.h" for details.
  */
  for (i = 0; i < N_PENDULUMS; i++)
  {
    sphereG[i] = RGraphicSphereCreate(rc, 1, orange,
      MdtBodyGetTransformPtr(ball[i]));
    lineG[i] = RGraphicLineCreate(rc, lineOrigin, lineEnd, blue,
      MdtBodyGetTransformPtr(ball[i]));
  }

  /*
  Ground
  */
  color[0] = color[1] = color[2] = color[3] = 1;
  planeG = RGraphicGroundPlaneCreate(rc, 24, 2, color, 0);
  RGraphicSetTexture(rc, planeG, "checkerboard");

  /*
  Camera attributes can be displayed from the renderer by pressing F7.
  */
  RCameraSetView(rc,(float)15,(float)-0.1,(float)0.5);

  /* RCameraSetView(rc, 16, 0.8f, 0.5f); */
  cameraLookAt[0] = 2.0f;
  cameraLookAt[1] = height;
  cameraLookAt[2] = 2.0f;
  RCameraSetLookAt(rc, cameraLookAt);

  RRenderSetActionNCallBack(rc, 3, liftFirstBall, 0);

  RRenderSetWindowTitle(rc,"Cradle example");
  RPerformanceBarCreate(rc);
  RRenderCreateUserHelp(rc, help, 1);
  RRenderToggleUserHelp(rc);

  /*
  Cleanup after simulation.
  */
#ifndef PS2
  atexit(cleanup);
#endif

  /*
  Run the Simulation
  */

  /*
  RRun() executes the main loop.

  Pseudocode:
     while no exit-request {
        Handle user input
        Call Tick() to evolve the simulation
        Update graphic transforms
        Draw graphics
     }
  */
  RRun(rc, tick, 0);

  return 0;
}

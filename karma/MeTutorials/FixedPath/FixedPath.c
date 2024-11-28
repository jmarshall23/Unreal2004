/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:33 $ - Revision: $Revision: 1.31.8.1 $

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
  This example demonstrates both a fixed path and a fixed path/fixed
  orientation joint. Pressing 't' toggles between the two joint types.

  The red sphere graphically depicts the position of the joint and is not a
  physics body.

  By default the joint acts on one body and the static environment. By
  #defining TWO_BODIES the joint can be made to involve two bodies.
*/

#include <string.h>

#include <Mdt.h>
#include <MeMath.h>
#include <MeViewer.h>

#define PATH_RADIUS     3

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;
/* Physics body */
MdtBodyID body;
/* Fixed path joint */
MdtFixedPathID FPjoint;
/* Fixed path, fixed orientation joint */
MdtFPFOJointID FPFOjoint;
int jointType = MdtBclFIXEDPATH;

/*
  If TWO_BODIES is defined then the fixed path joint constrains two
  bodies. Otherwise the fixed path joint constrains one body and the static
  environment.
*/
#ifdef TWO_BODIES
  MdtBodyID body2;
#endif


#define  TEXTPOS_X   (  3.f)
#define  TEXTPOS_Y   (350.f)

char string1[] = "Fixed Path joint";
char string2[] = "Fixed Path, Fixed Orientation joint";

char *help[] = {
  "$UP - toggle between Fixed Path and Fixed Path, Fixed Orientation joints"
};


MeReal TimeStep = (MeReal)(0.01);
/* render context */
RRender *rc;

RGraphic *r_cuboid = 0;
RGraphic *r_pivot = 0;
RGraphic *r_cylinder = 0;
RGraphic *text = NULL;
MeMatrix4 m;
/*
  Graphic-only objects' transformation matrices.
*/
MeMatrix4 cylTransform =
{
    {1, 0,  0, 0},
    {0, 0, -1, 0},
    {0, 1,  0, 0},
    {0, 0,  0, 1}
};

MeMatrix4 pivotTransform =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

/*
  N.B. Renderer colour is never double precision.
*/
const float Col1[4]   = { 1.0, 0.0, 1.0, 1.0 };
const float Col2[4]   = { 0.0, 1.0, 1.0, 1.0 };
const float ColRed[4] = { 1.0, 0.0, 0.0, 1.0 };

/*
  Positions and velocities for fixed path joint.
*/
#define N 500

MeVector3 pos0[N];
MeVector3 vel0[N];
MeVector3 pos1[N];
MeVector3 vel1[N];


void MEAPI Tick(RRender * rc, void *user_data)
{
    static int n = 0;

    ++n;

    n %= N;

    /*
      The RStart/StopTimer function calls cause a performance bar to be
      displayed which gives a percentage breakdown of dynamics, collision
      and rendering activity. The performance bar only works in OpenGL.
    */
    MeProfileStartSection("Dynamics",0);
    {
        /*
          0 or 1 correspond to first body and second body.
        */
        if (jointType == MdtBclFIXEDPATH)
        {
            MdtFixedPathSetVelocity(FPjoint, 0, vel0[n][0],vel0[n][1],vel0[n][2]);
            MdtFixedPathSetVelocity(FPjoint, 1, vel1[n][0],vel1[n][1],vel1[n][2]);
            MdtFixedPathSetPosition(FPjoint, 0, pos0[n][0],pos0[n][1],pos0[n][2]);
            MdtFixedPathSetPosition(FPjoint, 1, pos1[n][0],pos1[n][1],pos1[n][2]);
        }
        else
        {
            MdtFPFOJointSetPosition(FPFOjoint, 0, pos0[n][0],pos0[n][1],pos0[n][2]);
            MdtFPFOJointSetPosition(FPFOjoint, 1, pos1[n][0],pos1[n][1],pos1[n][2]);
        }

        MdtWorldStep(world, TimeStep);
    }
    MeProfileEndSection("Dynamics");
    /*
      Update renderer object transformations.
    */
    RGraphicSetTransformPtr(r_cuboid,MdtBodyGetTransformPtr(body));
#ifdef TWO_BODIES
/*    MeMatrix4Multiply( m, MdtBodyGetTransformPtr(body2), cylTransform); */
    MeMatrix4Multiply( m, cylTransform, MdtBodyGetTransformPtr(body2));
    RGraphicSetTransformPtr(r_cylinder,m);
#endif

    /*
      The "pivot" is a graphics-only object which marks the joint
      position:
    */
    pivotTransform[3][0] =
        r_cuboid->m_pObject->m_Matrix[2][0] * pos0[n][2] + r_cuboid->m_pObject->m_Matrix[3][0];
    pivotTransform[3][1] =
        r_cuboid->m_pObject->m_Matrix[2][1] * pos0[n][2] + r_cuboid->m_pObject->m_Matrix[3][1];
    pivotTransform[3][2] =
        r_cuboid->m_pObject->m_Matrix[2][2] * pos0[n][2] + r_cuboid->m_pObject->m_Matrix[3][2];
}

/*
  Toggle joint type
*/
void MEAPI toggleJoint(RRender *rc, void *user_data)
{
    if (jointType == MdtBclFIXEDPATH)
    {
        MdtFixedPathDisable(FPjoint);

        /*
          Set new orientation for body.
        */
#ifdef TWO_BODIES
        MdtFPFOJointSetBodies(FPFOjoint, body, body2);
#else
        MdtFPFOJointSetBodies(FPFOjoint, body, 0);
#endif
        MdtFPFOJointEnable(FPFOjoint);
        jointType = MdtBclFPFOJOINT;
        if (text) RGraphicDelete(rc, text, 1);
        text = RGraphicTextCreate(rc, string2,
                                  TEXTPOS_X, TEXTPOS_Y, ColRed);
    }
    else
    {
        MdtFPFOJointDisable(FPFOjoint);
        MdtFixedPathEnable(FPjoint);
        jointType = MdtBclFIXEDPATH;
        if (text) RGraphicDelete(rc, text, 1);
        text = RGraphicTextCreate(rc, string1,
                                  TEXTPOS_X, TEXTPOS_Y, ColRed);
    }
}

void MEAPI_CDECL cleanup(void)
{
    RRenderContextDestroy(rc);
    MdtWorldDestroy(world);
}

/*
  Main Routine
*/

int MEAPI_CDECL main(int argc, const char *(argv[]))
{
    AcmeVector3 cam_lookat = {0,0,0};
    const MeReal Ratio = 10;

    /*
      OpenGL or Direct3D can be specified on the command line with -gl or
      -d3d respectively.
    */
    MeCommandLineOptions* options;

    {
        /* Period = N */
        const MeReal omega = 2 * ME_PI / N;

        const MeReal k1 = omega / TimeStep;
        unsigned int n;

        for (n = 0; n < N; n++)
        {
            const MeReal cos_ = MeCos(n * omega) * PATH_RADIUS;
            const MeReal sin_ = MeSin(n * omega) * PATH_RADIUS;

            /*
              Set the joint position to move up and down the long axis
              of the cuboid.
            */
            pos0[n][0] = 0;
            pos0[n][1] = 0;
            pos0[n][2] = cos_;

            /*
              The path velocity is not strictly required (i.e. could
              be left zero) but may be used to improve accuracy:
            */
            vel0[n][0] = 0;
            vel0[n][1] = 0;
            vel0[n][2] = -k1 * sin_;

            /*
              Set the joint position to move around the circumference
              of the cylinder.
            */
            pos1[n][0] = cos_;
            pos1[n][1] = 0;
            pos1[n][2] = sin_;

            /*
              See path velocity comment above.
            */
            vel1[n][0] = -k1 * sin_;
            vel1[n][1] = 0;
            vel1[n][2] = k1 * cos_;
        }
    }

    world = MdtWorldCreate(2, 2);

#ifndef TWO_BODIES
    MdtWorldSetGravity(world, 0, 0, -10);
#endif

    /*
      Set up body/bodies.
    */

    body = MdtBodyCreate(world);
    MdtBodySetMass(body, 1);

#ifdef TWO_BODIES
    body2 = MdtBodyCreate(world);
    MdtBodySetMass(body2, 1);
    MdtBodySetPosition(body2, 0, 0, 0);
#endif

    /*
      Set MoI for  a solid cuboid, mass = 1kg, w,h = r * x, d = x,
      where w,h,d are half-width, half-height etc.
    */
    {
        MeMatrix3 I;
        const MeReal IRSq = 1 / (Ratio * Ratio);
        memset(I, 0, sizeof (MeMatrix3));

        I[0][0] = I[1][1] = (1 + IRSq) * PATH_RADIUS * PATH_RADIUS / 3;
        I[2][2] = 2 * IRSq * PATH_RADIUS * PATH_RADIUS / 3;
        MdtBodySetInertiaTensor(body, I);
    }

    /*
      Set the position of the first body in the inertial ref. frame,
      assuming that the other "body" is either the world or has been
      positioned at the origin of the world.  Zero initial rotation is also
      assumed.
    */

    MdtBodySetPosition(body, pos1[0][0] - pos0[0][0],
                             pos1[0][1] - pos0[0][1],
                             pos1[0][2] - pos0[0][2]);

    /*
      Set up fixed path joints.
    */

#ifdef TWO_BODIES
    FPjoint = MdtFixedPathCreate(world);
    MdtFixedPathSetBodies(FPjoint,body,body2);
    FPFOjoint = MdtFPFOJointCreate(world);
    MdtFPFOJointSetBodies(FPFOjoint,body,body2);
#else
    FPjoint = MdtFixedPathCreate(world);
    MdtFixedPathSetBodies(FPjoint,body,0);
    FPFOjoint = MdtFPFOJointCreate(world);
    MdtFPFOJointSetBodies(FPFOjoint,body,0);
#endif

    MdtBodyEnable(body);

#ifdef TWO_BODIES
    MdtBodyEnable(body2);
#endif

    if (jointType == MdtBclFIXEDPATH)
        MdtFixedPathEnable(FPjoint);
    else
        MdtFPFOJointEnable(FPFOjoint);

    /*
      Initialise rendering attributes.
    */

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);

    RCameraSetLookAt(rc,cam_lookat);
    RCameraSetView(rc,10,-0.3f,1);
    RCameraUpdate(rc);


    r_cylinder = RGraphicCylinderCreate(rc,
        PATH_RADIUS, PATH_RADIUS / Ratio, Col2, cylTransform);
#ifndef TWO_BODIES
    RGraphicSetTransformPtr(r_cylinder,cylTransform);
#endif

    r_cuboid = RGraphicBoxCreate(rc,
                           2 * PATH_RADIUS / Ratio,
                           2 * PATH_RADIUS / Ratio,
                           2 * PATH_RADIUS, Col1, 0);

    r_pivot = RGraphicSphereCreate(rc, PATH_RADIUS / Ratio, ColRed, 0);
    RGraphicSetTransformPtr(r_pivot,pivotTransform);

    RRenderSetWindowTitle(rc, "FixedPath example");
    RRenderCreateUserHelp(rc,help,1);  /* press F1 for onscreen user help */
    RRenderSetUpCallBack(rc,toggleJoint,0);
    RPerformanceBarCreate(rc);
    RRenderToggleUserHelp(rc);
    text = RGraphicTextCreate(rc, string1, TEXTPOS_X, TEXTPOS_Y, ColRed);


    /*
      Cleanup after simulation.
    */
#ifndef PS2
    atexit(cleanup);
#endif
    /*
      Run the simulation.
    */

    /*
      RRun() executes the main loop.

      Pseudocode: while no exit-request { Handle user input call Tick() to
      evolve the simulation and update graphic transforms Draw graphics }
    */

    RRun(rc, Tick, 0);

    return 0;
}

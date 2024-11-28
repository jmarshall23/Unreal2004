/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:37 $ - Revision: $Revision: 1.19.8.1 $

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

#include <string.h>
#include <Mdt.h>
#include <MeMath.h>
#include <MeViewer.h>

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;
/* Physics body */
MdtBodyID body1;
MdtBodyID body2;
MdtBodyID body3;


MdtBSJointID joint0;
MdtRPROJointID joint1;
MdtRPROJointID joint2;
MdtRPROJointID joint3;

char *help[] =
{
    "$UP     - rotate joint1 ccw about its z",
    "$DOWN   - rotate joint1 cw  about its z",
    "$LEFT   - rotate joint1 ccw about its y",
    "$RIGHT  - rotate joint1 cw  about its y",
    "$UP2    - rotate joint2 ccw about its z",
    "$DOWN2  - rotate joint2 cw  about its z",
    "$LEFT2  - rotate joint2 ccw about its y",
    "$RIGHT2 - rotate joint2 cw  about its y",
};

MeReal TimeStep = (MeReal)(0.01);

MeReal angle1 = (MeReal)(0.0);
MeReal angle2 = (MeReal)(0.0);
MeReal angle3 = (MeReal)(0.0);
MeReal offsetAngle1 = 0;
MeReal offsetAngle2 = 0;
MeReal offsetAngle3 = 0;

MeReal delta = 2*ME_PI/(MeReal)100;  /* odd division makes joint
                                       orientation skip singularity */
MeReal deltaOffset = ME_PI/(MeReal)50;

const MeVector3 cubeSize1 = {2,2,2};
const MeVector3 cubeSize2 = {2,2,2};
const MeVector3 cubeSize3 = {2,2,2};

const MeVector3 cubeOffset = {0,4,0};

/* render context */
RRender *rc;

RGraphic *r_cuboid1 = 0;
RGraphic *r_cuboid2 = 0;
RGraphic *r_cuboid3 = 0;
RGraphic *text1;
RGraphic *text2;
RGraphic *text3;
MeReal *m;

/*
  N.B. Renderer colour is never double precision.
*/
const float Col1[3] = { 1.0, 1.0, 0.0 };
const float Col2[3] = { 1.0, 0.0, 1.0 };
const float Col3[3] = { 0.0, 1.0, 1.0 };

void SetJointOrientation(MdtRPROJointID joint, MeReal a, MeReal b)
{
  MeReal ca, sa, cb, sb;
  MeVector4 q0;
  MeVector4 offset;
  MeVector4 q;

  ca = MeCos(a/2); sa = MeSin(a/2); cb = MeCos(b/2); sb = MeSin(b/2);

  q0[0] = ca; q0[1] = (MeReal)0; q0[2] = (MeReal)0; q0[3] = sa;

  offset[0] = cb; offset[1] = (MeReal)0; offset[2] = sb; offset[3] = (MeReal)0;

  MeQuaternionProduct(q, q0,offset);

  MdtRPROJointSetRelativeQuaternion(joint,q);
}


void MEAPI_CDECL cleanup(void)
{
    MeMemoryAPI.destroy(m);
    MdtWorldDestroy(world);
    RRenderContextDestroy(rc);
    /*RMenuDestroy(menu);*/
}

void MEAPI turnJoint1CCW(RRender *rc, void *user_data)
{
    SetJointOrientation(joint1,angle1 += delta,offsetAngle1);
}

void MEAPI turnJoint1CW(RRender *rc, void *user_data)
{
    SetJointOrientation(joint1,angle1 -= delta,offsetAngle1);
}

void MEAPI turnJoint2CCW(RRender *rc, void *user_data)
{
    SetJointOrientation(joint2,angle2 += delta,offsetAngle2);
}

void MEAPI turnJoint2CW(RRender *rc, void *user_data)
{
    SetJointOrientation(joint2,angle2 -= delta,offsetAngle2);
}

void MEAPI turnJoint3CCW(RRender *rc, void *user_data)
{
    SetJointOrientation(joint3,angle3 += delta,offsetAngle3);
}

void MEAPI turnJoint3CW(RRender *rc, void *user_data)
{
    SetJointOrientation(joint3,angle3 -= delta,offsetAngle3);
}

void MEAPI increaseOffsetAngle1(RRender *rc, void *user_data)
{
    SetJointOrientation(joint1,angle1,offsetAngle1 += deltaOffset);
}

void MEAPI decreaseOffsetAngle1(RRender *rc, void *user_data)
{
    SetJointOrientation(joint1,angle1,offsetAngle1 -= deltaOffset);
}

void MEAPI increaseOffsetAngle2(RRender *rc, void *user_data)
{
    SetJointOrientation(joint2,angle2,offsetAngle2 += deltaOffset);
}

void MEAPI decreaseOffsetAngle2(RRender *rc, void *user_data)
{
    SetJointOrientation(joint2,angle2,offsetAngle2 -= deltaOffset);
}

void MEAPI increaseOffsetAngle3(RRender *rc, void *user_data)
{
    SetJointOrientation(joint3,angle3,offsetAngle3 += deltaOffset);
}

void MEAPI decreaseOffsetAngle3(RRender *rc, void *user_data)
{
    SetJointOrientation(joint3,angle3,offsetAngle3 -= deltaOffset);
}

void MEAPI Tick(RRender *rc, void *user_data)
{
  static MeVector4 q0;
  MeVector3 w;
  MeVector3 zero3 = {0, 0, 0};
  q0[0] = MeCos(ME_PI/3);
  q0[1] = 0;
  q0[2] = 0;
  q0[3] = MeSin(ME_PI/3);

  MeProfileStartSection("Dynamics", 0);
  MdtWorldStep(world, TimeStep);

  MdtRPROJointSetRelativeAngularVelocity((MdtRPROJointID)joint1,zero3);
  MdtRPROJointSetRelativeAngularVelocity((MdtRPROJointID)joint2,zero3);
  MdtRPROJointSetRelativeAngularVelocity((MdtRPROJointID)joint3,zero3);

  MdtBodyGetAngularVelocity(body1, w);

/*  turnJoint1CCW(rc,user_data); */
  turnJoint2CCW(rc,user_data);
  turnJoint3CCW(rc,user_data);

/*  increaseOffsetAngle1(rc,user_data); */
  increaseOffsetAngle2(rc,user_data);
  increaseOffsetAngle3(rc,user_data);

  MeProfileEndSection("Dynamics");
}

/*
  Main Routine
*/

int MEAPI_CDECL main(int argc, const char *(argv[]))
{
    MeCommandLineOptions* options;
    /*
      OpenGL or Direct3D can be specified on the command line with -gl or
      -d3d respectively.
    */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    m = (MeReal*)MeMemoryAPI.create(sizeof(MeReal)*16);


    world = MdtWorldCreate(10, 10);
    MdtWorldSetGamma(world, 0.4f);
    MdtWorldSetEpsilon(world, 0.001f);


    /*
      Set up bodies.
    */

    body1 = MdtBodyCreate(world);
    MdtBodyEnable(body1);
    MdtBodySetPosition(body1,0, 0, 0);
    body2 = MdtBodyCreate(world);
    MdtBodyEnable(body2);
    MdtBodySetPosition(body2,cubeOffset[0],cubeOffset[1],cubeOffset[2]);
    body3 = MdtBodyCreate(world);
    MdtBodyEnable(body3);
    MdtBodySetPosition(body3,2*cubeOffset[0],2*cubeOffset[1],2*cubeOffset[2]);

    MdtBodySetAngularVelocity(body1, 1.1f, 0.3f, 0.62f);

    /*
      Set up joint.
    */
    /*
      joint0 = MdtBSJointCreate(world);
      MdtBSJointSetBodies(joint0, body1, 0);
      MdtBSJointSetPosition(joint0, 0, 0, 0);
      MdtBSJointEnable(joint0);
    */

    {
      MeVector4 q1;
      q1[0] = MeSin(23424); q1[1] = q1[2] = (MeReal)0; q1[3] = MeCos(23424);

      joint1 = MdtRPROJointCreate(world);
      MdtRPROJointSetBodies(joint1, body1, 0);
      MdtRPROJointEnable(joint1);

      joint2 = MdtRPROJointCreate(world);
      MdtRPROJointSetBodies(joint2, body2, body1);
      MdtRPROJointEnable(joint2);
      MdtRPROJointSetAttachmentPosition(joint2,0,0,0,0);


      MdtRPROJointSetAttachmentQuaternion(joint2,q1[0], q1[1], q1[2], q1[3], 0);
      MdtRPROJointSetAttachmentQuaternion(joint2,1,0,0,0, 1);
      MdtRPROJointSetAttachmentPosition(joint2,cubeOffset[0],cubeOffset[1],cubeOffset[2],1);
      joint3 = MdtRPROJointCreate(world);
      MdtRPROJointSetBodies(joint3, body3, body2);
      MdtRPROJointSetAttachmentPosition(joint3,0,0,0,0);
      MdtRPROJointEnable(joint3);
      MdtRPROJointSetAttachmentQuaternion(joint3,q1[0], q1[1], q1[2], q1[3], 0);
      MdtRPROJointSetAttachmentPosition(joint3,cubeOffset[0],cubeOffset[1],cubeOffset[2],1);
    }



    SetJointOrientation(joint1,angle1,offsetAngle1);
    SetJointOrientation(joint2,angle2,offsetAngle2);
    SetJointOrientation(joint3,angle3,offsetAngle3);

    /*
      Initialise rendering attributes.
    */

    /* Set up camera */
    RCameraRotateAngle(rc, (MeReal)0.8);
    RCameraRotateElevation(rc, (MeReal)0.25);
    RCameraSetLookAt(rc, cubeOffset);

    RLightSwitchOff(rc, kRAmbient);

    r_cuboid1 = RGraphicBoxCreate(rc, cubeSize1[0], cubeSize1[1], cubeSize1[2], Col1, MdtBodyGetTransformPtr(body1) );
    r_cuboid2 = RGraphicBoxCreate(rc, cubeSize1[0], cubeSize1[1], cubeSize1[2], Col1, MdtBodyGetTransformPtr(body2) );
    r_cuboid3 = RGraphicBoxCreate(rc, cubeSize1[0], cubeSize1[1], cubeSize1[2], Col1, MdtBodyGetTransformPtr(body3) );

    RRenderSetUpCallBack(rc, turnJoint1CCW, 0);
    RRenderSetDownCallBack(rc, turnJoint1CW, 0);
    RRenderSetLeftCallBack(rc, increaseOffsetAngle1, 0);
    RRenderSetRightCallBack(rc, decreaseOffsetAngle1, 0);
    RRenderSetUp2CallBack(rc, turnJoint2CCW, 0);
    RRenderSetDown2CallBack(rc, turnJoint2CW, 0);
    RRenderSetLeft2CallBack(rc, increaseOffsetAngle2, 0);
    RRenderSetRight2CallBack(rc, decreaseOffsetAngle2, 0);
/*
    RRenderSetUpCallBack(rc, turnJoint3CCW, 0);
    RRenderSetDownCallBack(rc, turnJoint3CW, 0);
    RRenderSetLeftCallBack(rc, increaseOffsetAngle3, 0);
    RRenderSetRightCallBack(rc, decreaseOffsetAngle3, 0);
*/

    /* Set windows title */
    RRenderSetWindowTitle(rc, "RPRO Joint example");

    /* Show help text */
    RRenderCreateUserHelp(rc, help, sizeof(help)/sizeof(help[0]));
    RRenderToggleUserHelp(rc);

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

      Pseudocode:
        while no exit-request {
            Handle user input
            Call Tick() to evolve the simulation and update graphic transforms
            Draw graphics
        }
    */
    RRun(rc, Tick, 0);

    return 0;
}

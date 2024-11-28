/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/30 18:27:17 $ - Revision: $Revision: 1.48.2.11 $

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

#ifdef _MSC_VER
#pragma warning( disable : 4305 4244 )
#endif


#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <Mst.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>

#define NBalls      11
#define NRocks      2
#define NBoxes      6
#define NSteps      10

#ifdef PS2
#  define DEFAULT_FRICTION    (0)
#  define MAX_MATRIX_SIZE     (110)
#else
#  define DEFAULT_FRICTION    (1)
#  define MAX_MATRIX_SIZE     (140)
#endif

int nextRock = 0;

MeReal ballDensity = (MeReal)(0.7);
MeReal rockRadius = (MeReal)(1.0);

MeVector3 boxDim = {2.5, 2.5, 2.5};
MeReal boxDensity = (MeReal)(0.10);
MeReal boxSpacing = 2.5;

MeVector3 bodyStart = {0, 7, 10};

#define TOHEAD    1.4
#define SHOULDER  1.0
#define TOELBOW   2
#define TOHAND    3.5
#define TOWAIST   2
#define LEGSPREAD 0.8
#define TOKNEE    4
#define TOFOOT    6

#define COLOR1 {0.0 , 0.73, 0.73, 1.0}
#define COLOR2 {0.0 , 0.4 , 1.0,  1.0}
#define COLOR3 {1.0 , 0.0 , 0.5,  1.0}
#define COLOR4 {1.0 , 0.6 , 0.0,  1.0}
#define COLOR5 {1.0,  0.4 , 0.0,  1.0}
#define COLOR6 {0.6 , 0.4 , 1.0,  1.0}

char *help[5] =
{
    "$ACTION1 - toggle options menu",
    "$ACTION2 - reset",
    "$ACTION3 - shoot",
    "$ACTION4 - chuck",
    "$MOUSE - mouse force",
};

/*
Position of each box
*/
MeReal ballPos[][3] =
{
    {0, 0, 0}, /* chest */
    {0, TOHEAD, 0}, /* head */
    {-TOELBOW, 0, 0}, /* right elbow */
    {-TOHAND, 0, 0}, /* right hand */
    {TOELBOW, 0, 0}, /* left elbow */
    {TOHAND, 0, 0}, /* left hand */
    {0, -TOWAIST, 0}, /* waist */
    {-LEGSPREAD, -TOKNEE, 0}, /* right knee */
    {-LEGSPREAD, -TOFOOT, 0}, /* right foot */
    {LEGSPREAD, -TOKNEE, 0}, /* left knee */
    {LEGSPREAD, -TOFOOT, 0} /* left foot */
};

/*
Position of each box
*/
float ballColor[11][4] =
{
    COLOR1, /* chest */
        COLOR2, /* head */
        COLOR3, /* right elbow */
        COLOR3, /* right hand */
        COLOR3, /* left elbow */
        COLOR3, /* left hand */
        COLOR4, /* waist */
        COLOR5, /* right knee */
        COLOR5, /* right foot */
        COLOR5, /* left knee */
        COLOR5  /* left foot */
};

MeReal ballRadius[11] =
{
    0.75, /* chest */
        0.5,  /* head */
        0.42, /* right elbow */
        0.5,  /* right hand */
        0.42, /* left elbow */
        0.5,  /* left hand */
        0.65, /* waist */
        0.42, /* right knee */
        0.5,  /* right foot */
        0.42, /* left knee */
        0.5   /* left foot */
};

/* startRadius, endRadius, length */
MeReal coneDim[10][3] =
{
    {0.25, 0.15, 1.5}, /* head */
    {0.25, 0.1,  1},   /* right elbow */
    {0.25, 0.25, 1.5}, /* right hand */
    {0.25, 0.1,  1},   /* left elbow */
    {0.25, 0.25, 1.5}, /* left hand */
    {0.35, 0.45, 2},   /* waist */
    {0.25, 0.1,  2},   /* right knee */
    {0.25, 0.25, 2},   /* right foot */
    {0.25, 0.1,  2},   /* left knee */
    {0.25, 0.25, 2}    /* left foot */
};

MeReal hipConeAngle = ME_PI/8;
MeReal hipLimitAngle = ME_PI/4;

/* World for the Dynamics * Toolkit simulation */
McdFrameworkID framework;
MdtWorldID world;
McdSpaceID space;
MstBridgeID bridge;

/* Physics representations */
MdtBodyID       ball[NBalls];
MdtBodyID       rock[NRocks];
MdtBodyID       box[NBoxes];
MdtBSJointID    bs[4];
MdtHingeID      hinge[6];
MdtConeLimitID  cone[2];

/* Graphical representations */
RGraphic *groundG;
RGraphic *lineG[NBalls];
RGraphic *ballG[NBalls];
RGraphic *rockG[NRocks];
RGraphic *boxG[NBoxes];

/* Collision representations */
McdModelID groundCM;
McdModelID ballCM[NBalls];
McdModelID rockCM[NRocks];
McdModelID boxCM[NBoxes];

McdGeometryID step_prim;
McdModelID stepCM[NSteps];
RGraphic* stepG[NSteps];
MeMatrix4 *stepTM;

MeVector3 stepSize = {19, 1, 4.5};
MeVector3 stepStart = {0, -0.5, 14};
MeVector3 stepIncrement = {0, 1, 1.8};

McdGeometryID plane_prim,
    ball_prim[NBalls], rock_prim[NRocks], box_prim[NBoxes];

MeReal gravity[3] = { 0, -8, 0 };

/* Render context */
RRender *rc;
RMenu* menu;

/* MeApp utility library */
MeApp* meapp;

MeReal step = (MeReal)(0.03);

MeALIGNDATA(MeMatrix4,groundTransform,16) =
{
    {1,  0,  0, 0},
    {0,  0, -1, 0},
    {0,  1,  0, 0},
    {0, -1,  0, 1}
};

MeMatrix4 groundRenderTransform =
{
    { 1,  0,  0, 0 },
    { 0,  0, -1, 0 },
    { 0,  1,  0, 0 },
    { 0, -1,  0, 1 }
};

void MEAPI chuck(RRender* rc, void* userData)
{
    int i;
    
    static const struct
    {
        int ball;
        MeReal x, y, z;
    }
    Balls[] = { { 1,  0, 0, -30}, { 3,  0, 0, -30} , {-1} };
        
    for(i=0; Balls[i].ball >= 0; ++i)
    {
        const MdtBodyID Ball = ball [Balls[i].ball];
        MdtBodyEnable(Ball);
        MdtBodySetLinearVelocity(Ball, Balls[i].x, Balls[i].y, Balls[i].z);
    }
}


void MEAPI shoot(RRender* rc, void* userData)
{
    MeReal v[3];
    AcmeReal r[4];

    /* Re-Enables a body in case it has been AutoDisabled */
    MdtBodyEnable(rock[nextRock]);
    RCameraGetPosition (rc, r);
    MdtBodySetPosition(rock[nextRock], r[0], r[1], r[2]);
    MdtBodySetQuaternion(rock[nextRock], 1, 0, 0, 0);
    
    MeVector3Subtract(v, rc->m_CameraLookAt, r);
    
    MeVector3Normalize(v);
    
    MeVector3Scale(v, 20);
    
    MdtBodySetLinearVelocity(rock[nextRock], v[0], v[1], v[2]);
    MdtBodySetAngularVelocity(rock[nextRock], 0, 0, 0);
    
    nextRock = (nextRock + 1) % NRocks;
}

void MEAPI singleStep(RRender * rc, void *userdata)
{
    MeVector3 pos;
    
    MdtBodyGetPosition(ball[0], pos);
    RCameraSetLookAt(rc, pos);
    
    MeProfileStartSection("Collision", 0);
    
    MeAppStep(meapp);
    
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge, space, world);
    MeProfileEndSection("Collision");
    
    /* Evolve the world. */
    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, step);
    MeProfileEndSection("Dynamics");
}

/*
Tick() is a callback function called from the renderer's main loop
to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc, void *userdata)
{
    singleStep(rc, userdata);
}

/* Reset boxes and balls to initial positions */
void MEAPI resetBodies()
{
    int i;
    MeMatrix3 R;
    MeVector3 pos;
    
    bodyStart[0] =     stepStart[0] + stepIncrement[0]*NSteps;
    bodyStart[1] = 6 + stepStart[1] + stepIncrement[1]*NSteps;
    bodyStart[2] =     stepStart[2] + stepIncrement[2]*NSteps;
    
    for (i = 0; i < NBalls; i++)
    {
        MdtBodySetPosition(ball[i],
            bodyStart[0] + ballPos[i][0],
            bodyStart[1] + ballPos[i][1],
            bodyStart[2] + ballPos[i][2]);
        
        MdtBodySetQuaternion(ball[i], 1, 0, 0, 0);
        MdtBodySetLinearVelocity(ball[i], 0, 0, 0);
        MdtBodySetAngularVelocity(ball[i], 0, 0, 0);
        MdtBodyEnable(ball[i]);
    }
    
    MeMatrix3MakeRotationX(R, -ME_PI/2);
    MdtBodySetOrientation(ball[1], (void *)R);
    
    MeMatrix3MakeRotationX(R, ME_PI/2);
    MdtBodySetOrientation(ball[6], (void *)R);
    MdtBodySetOrientation(ball[7], (void *)R);
    MdtBodySetOrientation(ball[8], (void *)R);
    MdtBodySetOrientation(ball[9], (void *)R);
    MdtBodySetOrientation(ball[10], (void *)R);
    
    MeMatrix3MakeRotationY(R, ME_PI/2);
    MdtBodySetOrientation(ball[2], (void *)R);
    MdtBodySetOrientation(ball[3], (void *)R);
    
    MeMatrix3MakeRotationY(R, -ME_PI/2);
    MdtBodySetOrientation(ball[4], (void *)R);
    MdtBodySetOrientation(ball[5], (void *)R);
    
    for (i = 0; i < NRocks; i++)
    {
        MdtBodySetPosition(rock[i], 10, -0.8 + rockRadius,
            2 + (2.5 * rockRadius * i));
        MdtBodySetQuaternion(rock[i], 1, 0, 0, 0);
        MdtBodySetLinearVelocity(rock[i], 0, 0, 0);
        MdtBodySetAngularVelocity(rock[i], 0, 0, 0);
        MdtBodyEnable(rock[i]);
    }
    
    for (i = 0; i < NBoxes; i++)
    {
        MdtBodySetPosition(box[i], 0, (i * boxSpacing)+0.25, 4);
        MdtBodySetQuaternion(box[i], 1, 0, 0, 0);
        MdtBodySetLinearVelocity(box[i], 0, 0, 0);
        MdtBodySetAngularVelocity(box[i], 0, 0, 0);
        MdtBodyEnable(box[i]);
    }

    MdtWorldResetForces(world);
    
    MdtBodyGetPosition(ball[0], pos);
    RCameraSetLookAt(rc, pos);
}

void MEAPI reset(RRender *rc, void* userData)
{
    int i;
    resetBodies();
    for(i=0; i<6; i++) 
        MdtLimitResetState(MdtHingeGetLimit(hinge[i]));
}

void MEAPI_CDECL cleanup(void)
{
    MeAppDestroy(meapp);
    
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);    
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    McdSpaceDestroy(space);
    MdtWorldDestroy(world);
    MstBridgeDestroy(bridge);
    McdTerm(framework);
    MeMemoryAPI.destroyAligned(stepTM);
}

void MEAPI ToggleHighQualityFriction(MeBool on)
{
    short material1;
    MdtContactParamsID props;

    material1 = MstBridgeGetDefaultMaterial();
    props = MstBridgeGetContactParams(bridge, material1, material1);

    MdtContactParamsReset(props);

    MdtContactParamsSetType(props, MdtContactTypeFriction2D);
    MdtContactParamsSetRestitution(props, 0.3);

    if (on)
    {    
        MdtContactParamsSetFriction(props, 25.0f);
        MdtContactParamsSetPrimarySlip(props, 0);
        MdtContactParamsSetSecondarySlip(props, 0);
        MdtContactParamsSetMaxAdhesiveForce(props, 0);
    }
    else
    {
        MdtContactParamsSetFriction(props, MEINFINITY);
        MdtContactParamsSetPrimarySlip(props, 0.1);
        MdtContactParamsSetSecondarySlip(props, 0.1);
        MdtContactParamsSetMaxAdhesiveForce(props, MEINFINITY);
    }

    MeInfo(0,"Using %s quality friction",(on) ? "high" : "low");
}

int MEAPI_CDECL main(int argc, const char **argv)
{
    MdtLimitID limit;
    
    MeI32 bodyKey = 0;
    MeI32 jointKey = 0;
    MeI16 modelKey = 0;
    
    int i;
    float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    MeReal mass;
    MeMatrix3 I;
    MeVector3 pos;
    MeCommandLineOptions* options;
    
    /* Initialise renderer */
    /* 0 means don't override any of the command line options, !MEFALSE means
    the renderer swallows options it recognizes */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;
    
    RRenderSetAppName(rc,"BallMan");
    
    /* Initialise dynamics */
    world = MdtWorldCreate(NBalls + NRocks + NBoxes, 200, 1, 1);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    
    MdtWorldSetAutoDisable(world, 1);
    MdtWorldSetMaxMatrixSize(world,MAX_MATRIX_SIZE);
    
    /* Collision detection */
    framework = McdInit(0, 50, 0,1);

    McdFrameworkSetDefaultContactTolerance(framework, 0.);
    
    McdPrimitivesRegisterTypes(framework);
    McdPrimitivesRegisterInteractions(framework);
    
    /* max objects and pairs */
    space = McdSpaceAxisSortCreate(framework,McdAllAxes, 50, 150);
    bridge = MstBridgeCreate(framework,10);
    MstSetWorldHandlers(world);
    
    /* BALLS: */
    for (i = 0; i < NBalls; i++)
    {
        mass = ME_PI * ballRadius[i] * ballRadius[i]
            * ballRadius[i] * ballDensity;
        MdtMakeInertiaTensorSphere(mass, ballRadius[i], I);
        
        ball[i] = MdtBodyCreate(world);
        
        MdtBodySetMass(ball[i], mass);
        MdtBodySetInertiaTensor(ball[i], (void *)I);
        MdtBodySetSortKey(ball[i],bodyKey++);
    }
    
    /* ROCKS: */
    for (i = 0; i < NRocks; i++)
    {
        rock[i] = MdtBodyCreate(world);
        MdtBodySetSortKey(rock[i],bodyKey++);
    }
    
    /* BOXES: */
    for (i = 0; i < NBoxes; i++)
    {
        box[i] = MdtBodyCreate(world);
        
        mass = boxDensity
            * boxDim[0] * boxDim[1] * boxDim[2];
        
        MdtMakeInertiaTensorBox(mass,
            boxDim[0], boxDim[1], boxDim[2], I);
        
        MdtBodySetMass(box[i], mass);
        MdtBodySetInertiaTensor(box[i], (void *)I);
        
        MdtBodySetSortKey(box[i],bodyKey++);
    }
    
    
    resetBodies();
    
    step_prim = McdBoxCreate(framework,stepSize[0], stepSize[1], stepSize[2]);
    
    stepTM = (MeMatrix4 *)MeMemoryAPI.createAligned(
        NSteps*sizeof(MeMatrix4),16);
    for(i=0; i<NSteps; i++)
    {
        stepCM[i] = McdModelCreate(step_prim);
        MeMatrix4TMMakeIdentity(stepTM[i]);
        MeMatrix4TMSetPosition(stepTM[i],
            stepStart[0] + i*stepIncrement[0],
            stepStart[1] + i*stepIncrement[1],
            stepStart[2] + i*stepIncrement[2]);
        McdModelSetTransformPtr(stepCM[i], stepTM[i]);
        McdModelSetSortKey(stepCM[i],modelKey++);
        McdSpaceInsertModel(space, stepCM[i]);
        McdSpaceUpdateModel(stepCM[i]);
        McdSpaceFreezeModel(stepCM[i]);        
    }
    
    /* Make all those joints.. */
    
    /* BALL-AND-SOCKET */
    /* r-elbow-chest */
    bs[0] = MdtBSJointCreate(world);
    MdtBSJointSetBodies(bs[0], ball[0], ball[2]);
    MdtBodyGetPosition(ball[0], pos);
    MdtBSJointSetPosition(bs[0], pos[0]-SHOULDER, pos[1], pos[2]);
    
    /* l-elbow-chest */
    bs[1] = MdtBSJointCreate(world);
    MdtBSJointSetBodies(bs[1], ball[0], ball[4]);
    MdtBodyGetPosition(ball[0], pos);
    MdtBSJointSetPosition(bs[1], pos[0]+SHOULDER, pos[1], pos[2]);
    
    
    /* r-knee-waist */
    bs[2] = MdtBSJointCreate(world);
    MdtBSJointSetBodies(bs[2], ball[6], ball[7]);
    MdtBodyGetPosition(ball[6], pos);
    MdtBSJointSetPosition(bs[2], pos[0]-LEGSPREAD, pos[1], pos[2]);
    
    cone[0]= MdtConeLimitCreate(world);
    MdtConeLimitSetBodies(cone[0], ball[6], ball[7]);
    MdtConeLimitBodySetAxes(cone[0], 0,
        -MeSin(hipConeAngle), -MeCos(hipConeAngle), 0,
        0, 0, 1);
    MdtConeLimitBodySetAxes(cone[0], 1,
        0, -1, 0,
        0, 0, 1);
    MdtConeLimitSetConeHalfAngle(cone[0], hipLimitAngle);
    
    /* l-knee-waist */
    bs[3] = MdtBSJointCreate(world);
    MdtBSJointSetBodies(bs[3], ball[6], ball[9]);
    MdtBodyGetPosition(ball[6], pos);
    MdtBSJointSetPosition(bs[3], pos[0]+LEGSPREAD, pos[1], pos[2]);
    
    cone[1]= MdtConeLimitCreate(world);
    MdtConeLimitSetBodies(cone[1], ball[6], ball[9]);
    MdtConeLimitBodySetAxes(cone[1], 0,
        MeSin(hipConeAngle), -MeCos(hipConeAngle), 0,
        0, 0, 1);
    MdtConeLimitBodySetAxes(cone[1], 1,
        0, -1, 0,
        0, 0, 1);
    MdtConeLimitSetConeHalfAngle(cone[1], hipLimitAngle);
    
    /* HINGES */
    
    /* waist-chest */
    hinge[4] = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge[4], ball[0], ball[6]);
    MdtBodyGetPosition(ball[0], pos);
    MdtHingeSetPosition(hinge[4], pos[0], pos[1], pos[2]);
    MdtHingeSetAxis(hinge[4], 0, 1, 0);
    limit = MdtHingeGetLimit(hinge[4]);
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), -ME_PI/3);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), ME_PI/3);
    MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), 100);
    MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), 100);
    MdtLimitActivateLimits(limit, 1);
    
    /* head-chest */
    hinge[5] = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge[5], ball[0], ball[1]);
    MdtBodyGetPosition(ball[0], pos);
    MdtHingeSetPosition(hinge[5], pos[0], pos[1], pos[2]);
    MdtHingeSetAxis(hinge[5], 1, 0, 0);
    limit = MdtHingeGetLimit(hinge[5]);
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), -ME_PI/4);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), ME_PI/4);
    MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), 1000);
    MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), 1000);
    MdtLimitActivateLimits(limit, 1);
    
    /* r-hand-elbow */
    hinge[0] = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge[0], ball[2], ball[3]);
    MdtBodyGetPosition(ball[2], pos);
    MdtHingeSetPosition(hinge[0], pos[0], pos[1], pos[2]);
    MdtHingeSetAxis(hinge[0], 0, 1, 0);
    limit = MdtHingeGetLimit(hinge[0]);
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), -ME_PI/4);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), ME_PI/4);
    MdtLimitActivateLimits(limit, 1);
    
    /* l-hand-elbow */
    hinge[1] = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge[1], ball[4], ball[5]);
    MdtBodyGetPosition(ball[4], pos);
    MdtHingeSetPosition(hinge[1], pos[0], pos[1], pos[2]);
    MdtHingeSetAxis(hinge[1], 0, 1, 0);
    limit = MdtHingeGetLimit(hinge[1]);
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), -ME_PI/4);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), ME_PI/4);
    MdtLimitActivateLimits(limit, 1);

    /* r-foot-knee */
    hinge[2] = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge[2], ball[7], ball[8]);
    MdtBodyGetPosition(ball[7], pos);
    MdtHingeSetPosition(hinge[2], pos[0], pos[1], pos[2]);
    MdtHingeSetAxis(hinge[2], 1, 0, 0);
    limit = MdtHingeGetLimit(hinge[2]);
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), 0);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), 3*ME_PI/4);
    MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), 100);
    MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), 100);
    MdtLimitActivateLimits(limit, 1);

    /* l-foot-knee */
    hinge[3] = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge[3], ball[9], ball[10]);
    MdtBodyGetPosition(ball[9], pos);
    MdtHingeSetPosition(hinge[3], pos[0], pos[1], pos[2]);
    MdtHingeSetAxis(hinge[3], 1, 0, 0);
    limit = MdtHingeGetLimit(hinge[3]);
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), 0);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), 3*ME_PI/4);
    MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), 100);
    MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), 100);
    MdtLimitActivateLimits(limit, 1);
    
    for(i=0; i<4; i++) 
    {
        MdtBSJointSetSortKey(bs[i],jointKey++);
        MdtBSJointEnable(bs[i]);
    }
    
    for(i=0; i<2; i++) 
    {
        MdtConeLimitSetSortKey(cone[i],jointKey++);
        MdtConeLimitEnable(cone[i]);
    }
    
    for(i=0; i<6; i++) 
    {
        MdtHingeSetSortKey(hinge[i],jointKey++);
        MdtHingeEnable(hinge[i]);
    }
    
    /* Set parameters for contacts. */
    ToggleHighQualityFriction(DEFAULT_FRICTION);
    
    plane_prim = McdPlaneCreate(framework);
    groundCM = McdModelCreate(plane_prim);
    McdModelSetTransformPtr(groundCM, groundTransform);
    McdModelSetSortKey(groundCM,modelKey++);
    McdSpaceInsertModel(space, groundCM);
    
    McdSpaceUpdateModel(groundCM);
    McdSpaceFreezeModel(groundCM);
    
    for (i = 0; i < NBalls; i++)
    {
        ball_prim[i] = McdSphereCreate(framework,ballRadius[i]);
        ballCM[i] = McdModelCreate(ball_prim[i]);
        McdModelSetBody(ballCM[i], ball[i]);
        McdModelSetSortKey(ballCM[i],modelKey++);
        McdSpaceInsertModel(space, ballCM[i]);
    }
    
    for (i = 0; i < NRocks; i++)
    {
        rock_prim[i] = McdSphereCreate(framework,rockRadius);
        rockCM[i] = McdModelCreate(rock_prim[i]);
        McdModelSetBody(rockCM[i], rock[i]);
        McdModelSetSortKey(rockCM[i],modelKey++);
        McdSpaceInsertModel(space, rockCM[i]);
    }
    
    for (i = 0; i < NBoxes; i++)
    {
        box_prim[i] = McdBoxCreate(framework,boxDim[0], boxDim[1], boxDim[2]);
        boxCM[i] = McdModelCreate(box_prim[i]);
        McdModelSetBody(boxCM[i], box[i]);
        McdModelSetSortKey(boxCM[i],modelKey++);
        McdSpaceInsertModel(space, boxCM[i]);
    }
    
    McdSpaceBuild(space);
    
    RCameraSetOffset(rc, 15);
    RCameraRotateAngle(rc,2.8f);
    RCameraRotateElevation(rc,0.7f);
    RCameraUpdate(rc);
    
    /* GROUND: */
    
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 0.0f;
    
    groundG = RGraphicGroundPlaneCreate(rc, 50.0f,30, color, -1);
    RGraphicSetTexture(rc, groundG, "checkerboard");
    
    for (i = 0; i < NBalls; i++)
    {
        color[0] = ballColor[i][0];
        color[1] = ballColor[i][1];
        color[2] = ballColor[i][2];
        color[3] = ballColor[i][3];
        ballG[i] = RGraphicSphereCreate(rc, ballRadius[i],
            color, MdtBodyGetTransformPtr(ball[i]));
        RGraphicSetTexture(rc, ballG[i], "wood1");
        
        color[0] = ballColor[i][0] + ((1.0 - ballColor[i][0])*0.5);
        color[1] = ballColor[i][1] + ((1.0 - ballColor[i][1])*0.5);
        color[2] = ballColor[i][2] + ((1.0 - ballColor[i][2])*0.5);
        color[3] = ballColor[i][3] + ((1.0 - ballColor[i][3])*0.5);
        
        if(i != 0)
            lineG[i] = RGraphicFrustumCreate(rc,
                coneDim[i-1][0], coneDim[i-1][1], 0, coneDim[i-1][2],
                5, color, MdtBodyGetTransformPtr(ball[i]));
    }
    
    for (i = 0; i < NRocks; i++)
    {
        color[0] = 0.9;
        color[1] = 0.0;
        color[2] = 0.0;
        color[2] = 1.0;
        ballG[i] = RGraphicSphereCreate(rc, rockRadius, color,
            MdtBodyGetTransformPtr(rock[i]));
    }
    
    for (i = 0; i < NBoxes; i++)
    {
        color[0] = 1.0;
        color[1] = 1.0;
        color[2] = 1.0;
        color[2] = 1.0;
        boxG[i] = RGraphicBoxCreate(rc, boxDim[0], boxDim[1], boxDim[2],
            color, MdtBodyGetTransformPtr(box[i]));
        RGraphicSetTexture(rc, boxG[i], "wood1");
    }
    
    for(i=0; i<NSteps; i++)
    {
        color[0] = 0.5;
        color[1] = 0.6;
        color[2] = 0.5;
        color[2] = 1.0;
        stepG[i] = RGraphicBoxCreate(rc,
            stepSize[0], stepSize[1],stepSize[2], color, stepTM[i]);
    }
    
    /* CONTROLS: */
    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);
    RRenderSetActionNCallBack(rc, 4, chuck, 0);
    
    RRenderSetWindowTitle(rc,"BallMan example");
    
    RRenderCreateUserHelp(rc, help, 5);
    RRenderToggleUserHelp(rc);
    
    RPerformanceBarCreate(rc);
    
    meapp = MeAppCreate(world, space, rc);
    
    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    menu = RMenuCreate(rc, "Options Menu");
    RMenuAddToggleEntry(menu, "High quality friction",
        ToggleHighQualityFriction, DEFAULT_FRICTION);    
    RRenderSetDefaultMenu(rc, menu);
    
    
    /* Cleanup after simulation. */
    
#ifndef PS2
    atexit(cleanup);
#endif
    /* Run the Simulation. */
    
    /*
    RRun() executes the main loop.
    
      Pseudocode: while no exit-request { Handle user input call Tick() to
      evolve the simulation and update graphic transforms Draw graphics }
    */
    RRun(rc, tick, 0);
    
    return 0;
}

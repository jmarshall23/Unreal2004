/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 20:53:14 $ - Revision: $Revision: 1.5.4.12 $

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


#include <stdio.h>

#include <MeAssert.h>
#include <Mst.h>
#include <MeApp.h>

#include <MeViewer.h>
#include <McdTriangleList.h>

#include "TankUtils.h"

/* Global variables. */

/* time stepsize */
const MeReal timeStep = (MeReal)(0.025);

/* Gravity */
#define GRAV_MAG (MeReal)(9.8)
const MeReal grav_mag = GRAV_MAG;
const MeReal gravity[3] = { 0, -GRAV_MAG, 0 };

/* Simulation object */
MstUniverseID   universe;
MdtWorldID      world;
McdFrameworkID  framework;
McdSpaceID      space;
MstBridgeID     bridge;


MeApp           *app;
MeMatrix4       lcTransform;

/* Landscape */
McdModelID terrainCM;
McdGeometryID terrainPrim;
RGraphic *terrainG;
RGraphic *waterG;

#define WATER_LEVEL     (3.5f)

MstMaterialID groundMaterial, trackMaterial, objectMaterial;

/* Object-ground properties */
#define OBJECT_FRICTION   (2.0f)
#define OBJECT_SOFTNESS   (0.01f)

/* Tank */
/* +X is forward, +Z is up and +Y is right (track 0) */
MdtBodyID       tank;
RGraphic*       tankG;
McdGeometryID   tankPrim;
McdModelID      tankCM;


/*
   Useful track-segment info
   Each track models userdata points to one of these.
 */
typedef struct
{
    int     side;
    int     segment;
    MeReal  rideHeight;
    int     numContacts;
} TrackInfo;

/* #define FOUR_WHEEL */

#ifdef FOUR_WHEEL
#  define NUM_SEGMENTS    (2)
#  define TRACK_SPACING   (1.2f)
#  define TRACK_OFFSET    (0.6f) /* x-offset to start track */
#else
#  define NUM_SEGMENTS    (5)
#  define TRACK_SPACING   (0.35f)
#  define TRACK_OFFSET    (2*TRACK_SPACING) /* x-offset to start track */
#endif

McdGeometryID   trackPrim;
McdModelID      trackCM[2][NUM_SEGMENTS];
/* Fixed relative transform between physics and collision models */
MeMatrix4      *trackRelTM;
/* Collision transform updated automatically each frame */
MeMatrix4      *trackColTM;
/* Transform used to draw spheres */
MeMatrix4       trackGraTM[2][NUM_SEGMENTS];
RGraphic*       trackG[2][NUM_SEGMENTS];
TrackInfo       trackInfo[2][NUM_SEGMENTS];


#define TANK_MASS       (10)

#define TRACK_DROP      (0.3f)
#define TRACK_RADIUS    (0.3f)
#define TRACK_SEP       (1.0f)

#define START_POS_X     (0.0f)
#define START_POS_Z     (-40.0f)
#define START_HEIGHT    (1.5f)

MeBool   onIce = 0;
MeBool   followCam = 1;
MeBool   debugInfo = 0;

#define SPEED_SLIP_FACTOR   (0.01f)
#define TRACK_FRICTION      (2.0f * TANK_MASS)
#define ICE_FRICTION        (0.4f * TANK_MASS)
#define TRACK_SOFTNESS      (0.03f)

#define NORMAL_TOLERANCE    (0.6f)
#define MAX_ANGVEL          (1.0f)
#define MAX_TILT            (0.4f * ME_PI)
#define MAX_TRACK_SPEED     (10.0f)
#define MAX_TURN            (1.0f)

#define TANK_RADIUS     (1.5f) /* Used for heightfield query */
#define TANK_COM_DROP   (-0.35f)
MeVector3 chassisSize = {1.5f, 0.5f, 1.6f}; /* Length, Width, Height */

MeReal speedIncrement = 0.2f;

MeReal speed = 0;
MeReal turn = 0;

MeReal leftSpeed = 0;
MeReal rightSpeed = 0;

/* Used to simulate rotation of wheels */
MeReal segmentRotation[2] = {0, 0};

MeMatrix4Ptr tankTM;
MdtConeLimitID  coneLimit;

/* Graphics render context */
RRender *rc;

char *help[] =
{
    "$ACTION2 - reset",
    "Arrows - drive tank",
    "$ACTION3 - toggle ice/grass",
    "$ACTION4 - toggle camera follow",
#ifndef PS2
    "$ACTION5 - toggle debug info"
#endif
};

/* Height field grid data */
MyHeightField       heightField;

#define TRILIST_SIZE    (600)

/* Rotate PI/2 about x-axis */
MeALIGNDATA(MeMatrix4,terrainTransform,16);

/* Assuming only one model is with physics, find it */
McdModelID GetDynamicsModel(McdIntersectResult* result)
{
    McdModelID m1, m2;
    McdModelPairGetModels(result->pair, &m1, &m2);

    return (McdModelGetBody(m1)) ? m1 : m2;
}


MeBool MEAPI ObjectGroundCB(McdIntersectResult* result,
    McdContact* colC, MdtContactID dynC)
{
    MeReal pen = MdtContactGetPenetration(dynC);
    MeVector3 rvel, vel1, vel2, pos, normal, dir;
    int contactColor[3] = {0, 0, 0};

    MdtBodyID b1 = MdtContactGetBody(dynC, 0);
    MdtBodyID b2 = MdtContactGetBody(dynC, 1);

    MdtContactGetNormal(dynC, normal);
    MdtContactGetPosition(dynC, pos);

    MdtBodyGetVelocityAtPoint(b1, pos, vel1);

    if(b2 != 0)
        MdtBodyGetVelocityAtPoint(b2, pos, vel2);
    else
        vel2[0] = vel2[1] = vel2[2] = 0;

    MeVector3Subtract(rvel, vel1, vel2);

    if (MeVector3MagnitudeSqr(rvel) > 0)
    {
        MeReal adotb = MeVector3Dot(normal, rvel);
        MeVector3Scale(normal, adotb);
        MeVector3Subtract(dir, rvel, normal);
        MeVector3Normalize(dir);
        MdtContactSetDirection(dynC, dir[0], dir[1], dir[2]);
    }

    if (pen < 0)
        return 0;

    if (MeSqrt(MeVector3MagnitudeSqr(normal)) == 0.0f )
        return 0;

    /* contact drawing */
    if (debugInfo)
    {
        MeVector3 end;
        /* McdContact* firstContact = &(result->contacts[0]); */

        MdtContactGetNormal(dynC, normal);

        end[0] = pos[0] + normal[0] * 15 * pen;
        end[1] = pos[1] + normal[1] * 15 * pen;
        end[2] = pos[2] + normal[2] * 15 * pen;

        switch(colC->dims>>8)
        {
            case 0: /* vertex - red */
            {
                contactColor[0] = 255;
                break;
            }
            case 1: /* edge - green */
            {
                contactColor[1] = 255;
                break;
            }
            case 2: /* face  - blue */
            {
                contactColor[2] = 255;
                break;
            }
            default:
            {
                contactColor[0] = 255;
                contactColor[1] = 255;
                contactColor[2] = 255;
                break;
            }
        }

        DebugLine(pos, end, contactColor[0], contactColor[1], contactColor[2]);
    }



    return 1;
}

/* Callback executed for every contact between a track segment & the ground */
MeBool MEAPI TrackGroundCB(McdIntersectResult* result,
                           McdContact* colC, MdtContactID dynC)
{
    MeVector3 normal, dir;
    MdtContactParamsID params = MdtContactGetParams(dynC);
    TrackInfo* info;
    McdModelID segModel = GetDynamicsModel(result);
    MeReal pen = MdtContactGetPenetration(dynC);
    MeReal ride, speedDiff;

    /* The first contact is important because we have turned on
       'Face Normals First'. This means the first contact will be
       with a face if possible, and is probably good. */
    McdContact* firstContact = &(result->contacts[0]);

    if(colC->dims>>8 == 2 && firstContact->dims>>8 != 2)
        printf("ERROR!\n");

    /* If penetration too low, throw away */
    if(pen < 0)
        return 0;

    /* Set the contact 'direction' using cross product of tank 'right' vector
       and contact normal. We have to set the 'direction' for each contact and
       set the 'relative surface velocity' (aka 'slide') in that direction to
       simulate the rolling of the caterpiller. */
    MdtContactGetNormal(dynC, normal);
    MeVector3Cross(dir, normal, tankTM[2]);
    MeVector3Normalize(dir);
    MdtContactSetDirection(dynC, dir[0], dir[1], dir[2]);

    /* Find if we are left or right track and set 'slide' (relative surface
       velocity) accordingly. */
    info = (TrackInfo*)McdModelGetUserData(segModel);
    MEASSERT(info);

    if(info->side == 0)
        MdtContactParamsSetPrimarySlide(params, rightSpeed);
    else
        MdtContactParamsSetPrimarySlide(params, leftSpeed);

    /* Allow track segments to 'slip' sideways more when turning. */
    speedDiff = (MeReal)fabs(rightSpeed - leftSpeed);
    MdtContactParamsSetSecondarySlip(params, speedDiff * SPEED_SLIP_FACTOR);

    /* Set 'ride' height for drawing segments going up and down. */
    ride = pen * MeVector3Dot(normal, tankTM[1]);

    if(ride > info->rideHeight)
        info->rideHeight = ride;

    /* Avoid contacts with very similar normals (prefer face contacts) */
    if (colC != firstContact)
    {
        /* Straightened normal is too similar to cp1's.  We can kill it. */
        if(MeVector3Dot(normal, firstContact->normal) > NORMAL_TOLERANCE)
            return 0;
    }

    if(debugInfo)
    {
        MeVector3 start, end;

        MdtContactGetPosition(dynC, start);

        end[0] = start[0] + normal[0] * 15 * pen;
        end[1] = start[1] + normal[1] * 15 * pen;
        end[2] = start[2] + normal[2] * 15 * pen;

        /* RED for first, YELLOW for others */
        if(colC == firstContact)
            DebugLine(start, end, 255, 0, 0);
        else
            DebugLine(start, end, 0, 255, 0);
    }

    info->numContacts++;

    return 1;
}

/*
  Height-field query
*/

/*
    Find the potentially-intersecting set of triangles.
    'Pos' is given in tri-list ref-frame.
*/
static int MEAPI TriListGeneratorCB(McdModelPair *mp,
    McdUserTriangle *list,MeVector3 pos,MeReal rad,int max)
{
    McdUserTriangle *tri;
    int triCount = 0;
    int ix, iz, k;
    int minX, maxX, minZ, maxZ;

    minX = (int)((pos[0] - heightField.xOrigin - rad)/DELTA_X);
    maxX = (int)(1 + (pos[0] - heightField.xOrigin + rad)/DELTA_X);

    minZ = (int)((pos[2] - heightField.zOrigin - rad)/DELTA_Z);
    maxZ = (int)(1 + (pos[2] - heightField.zOrigin + rad)/DELTA_Z);

    minX = MeCLAMP(minX, 0, GRIDSIZE_X-1);
    maxX = MeCLAMP(maxX, 0, GRIDSIZE_X-1);
    minZ = MeCLAMP(minZ, 0, GRIDSIZE_Z-1);
    maxZ = MeCLAMP(maxZ, 0, GRIDSIZE_Z-1);

    for(ix=minX; ix<maxX; ix++)
        for(iz=minZ; iz<maxZ; iz++)
            /* [i,j] now points at bottom left-hand corner of possible square
               (2 triangles). */
            for(k=0; k<2; k++)
            {
                tri = list+triCount;

                /* Vertices */
                tri->vertices[0] = &heightField
                    .vertex_store[ix + vOrder[k][0][0]][iz + vOrder[k][0][1]];
                tri->vertices[1] = &heightField
                    .vertex_store[ix + vOrder[k][1][0]][iz + vOrder[k][1][1]];
                tri->vertices[2] = &heightField
                    .vertex_store[ix + vOrder[k][2][0]][iz + vOrder[k][2][1]];

                /*  Normal -- Must be related to
                    vertices/edges using RH rule */
                tri->normal = &heightField.normal_store[ix][iz][k];

                /*  Could use this to keep your own data for use in
                    contact callbacks etc. */
                tri->triangleData.ptr = 0;

                /*  We wont use any edge contacts (assume landscape is
                    'smooth') and triangles are 1-sided, so default
                    flags are fine. */
                tri->flags = 0;

#if 0
                if(debugInfo)
                {
                  DebugLine(*tri->vertices[0], *tri->vertices[1], 255, 255, 0);
                  DebugLine(*tri->vertices[1], *tri->vertices[2], 255, 255, 0);
                  DebugLine(*tri->vertices[2], *tri->vertices[0], 255, 255, 0);
                }
#endif
                triCount++;
            }

    return triCount;
}

void MEAPI Reset(RRender* rc, void* userData)
{
    MdtBodySetQuaternion(tank, 1, 0, 0, 0);
    MdtBodySetLinearVelocity(tank, 0, 0, 0);
    MdtBodySetAngularVelocity(tank, 0, 0, 0);
    MdtBodySetPosition(tank,
        START_POS_X,
        FindHeight(&heightField, START_POS_X, START_POS_Z) + START_HEIGHT,
        START_POS_Z);
    leftSpeed = rightSpeed = speed = turn = 0;
}

/*
    Evolve
 */
void MEAPI Tick(RRender * rc, void* userData)
{
    int i,j;
    MeReal angularVelocity;
    MeVector3 pos;

    MeProfileStartSection("MathEngine", 0);

    /* Wind on next history slot to use (circular buffer) */
    for(i=0; i<2; i++)
    {
        for(j=0; j<NUM_SEGMENTS; j++)
        {
            trackInfo[i][j].rideHeight = 0;
            trackInfo[i][j].numContacts = 0;
        }
    }
    ClearDebugLines();

    /* Reset tank if we go in the water. */

    MdtBodyGetPosition(tank, pos);
    if(pos[1] < WATER_LEVEL)
        Reset(rc, 0);

    MeProfileStartSection("Collision", 0);

    MeProfileStartSection("McdSpaceUpdateAll", 0);
    McdSpaceUpdateAll(space);
    MeProfileEndSection("McdSpaceUpdateAll");

    MeProfileStartSection("MstBridgeUpdateContacts", 0);

    MstBridgeUpdateContacts(bridge, space, world);
    MeProfileEndSection("MstBridgeUpdateContacts");

    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, timeStep);

    MdtBodyEnable(tank);

    /* We just make _really_ sure the tank doesn't do anything odd! */
    {
        MeVector3 angvel;
        MeReal mag;

        MdtBodyGetAngularVelocity(tank, angvel);
        mag = MeVector3Magnitude(angvel);

        if(mag > MAX_ANGVEL)
        {
            MeVector3Normalize(angvel);
            MeVector3Scale(angvel, MAX_ANGVEL);
            MdtBodySetAngularVelocity(tank, angvel[0], angvel[1], angvel[2]);
        }
    }

    MeProfileEndSection("Dynamics");

    /* 
    move the wheels to keep up with the tank  - they get implictly moved by
    collision, which is not the most efficient way to do this.
    */

    for(i=0;i<NUM_SEGMENTS;i++)
    {
        McdModelUpdate(trackCM[0][i]);
        McdModelUpdate(trackCM[1][i]);
    }

    /* Simulate segment rotations. */
    angularVelocity = 2 * rightSpeed / (TRACK_RADIUS * ME_PI);
    segmentRotation[0] -= angularVelocity * timeStep;

    angularVelocity = 2 * leftSpeed / (TRACK_RADIUS * ME_PI);
    segmentRotation[1] -= angularVelocity * timeStep;

    for(i=0; i<2; i++)
    {
        MeMatrix4 rotTM;
        MeVector4 quat = {1, 0, 0, 0};
        MeVector3 axle = {0, 0, 1};

        MeQuaternionFiniteRotation(quat, axle, segmentRotation[i]);
        MeMatrix4TMMakeIdentity(rotTM);
        MeQuaternionToTM(rotTM, quat);

        for(j=0; j<NUM_SEGMENTS; j++)
        {
            MeReal currentRide;

            MeMatrix4MultiplyMatrix(trackGraTM[i][j],
                                    (void *)rotTM,
                                    (void *)trackColTM[i+j*2]);

            /* Move track segments according to contact penetration. */
            currentRide = trackInfo[i][j].rideHeight;

            trackGraTM[i][j][3][0] += tankTM[1][0] * currentRide;
            trackGraTM[i][j][3][1] += tankTM[1][1] * currentRide;
            trackGraTM[i][j][3][2] += tankTM[1][2] * currentRide;
        }
    }

    MeProfileEndSection("MathEngine");

    if (followCam)
        RCameraSetLookAt(rc, pos);
}


/* Control callbacks */
void RecalcTreadSpeeds()
{
    leftSpeed  = speed - turn/2;
    rightSpeed = speed + turn/2;
}

void MEAPI IncreaseSpeed(RRender * rc, void *userData)
{
    /* Ensure tank is 'enabled' */
    MdtBodyEnable(tank);
    speed -= speedIncrement;
    if(speed < -MAX_TRACK_SPEED)
        speed = -MAX_TRACK_SPEED;

    RecalcTreadSpeeds();
}

void MEAPI DecreaseSpeed(RRender * rc, void *userData)
{
    MdtBodyEnable(tank);
    speed += speedIncrement;
    if(speed > MAX_TRACK_SPEED)
        speed = MAX_TRACK_SPEED;

    RecalcTreadSpeeds();
}

void MEAPI TurnRight(RRender * rc, void *userData)
{
    MdtBodyEnable(tank);
    turn += speedIncrement;
    if(turn > MAX_TURN)
        turn = MAX_TURN;

    RecalcTreadSpeeds();
}

void MEAPI TurnLeft(RRender * rc, void *userData)
{
    MdtBodyEnable(tank);
    turn -= speedIncrement;
    if(turn < -MAX_TURN)
        turn = -MAX_TURN;

    RecalcTreadSpeeds();
}

void MEAPI ToggleOnIce(RRender * rc, void *userData)
{
    MdtContactParamsID params = MstBridgeGetContactParams(bridge,
        groundMaterial, trackMaterial);

    onIce = !onIce;

    if(onIce)
        MdtContactParamsSetFriction(params, ICE_FRICTION);
    else
        MdtContactParamsSetFriction(params, TRACK_FRICTION);

}

void MEAPI ToggleFollowCam(RRender * rc, void *userData)
{
    followCam = !followCam;
}

void MEAPI ToggleDebugInfo(RRender * rc, void *userData)
{
    debugInfo = !debugInfo;
}



void MEAPI_CDECL cleanup(void)
{
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    McdTerm(framework);

    MdtWorldDestroy(world);

    RRenderContextDestroy(rc);
    MeMemoryAPI.destroyAligned(trackColTM);
    MeMemoryAPI.destroyAligned(trackRelTM);
}

int MEAPI_CDECL main(int argc, const char **argv)
{
    int i, j, k;
    MeCommandLineOptions* options;
    MstUniverseSizes sizes;
    MdtBclContactParams *params;
    MeMatrix3 I;
    MeVector3 min = {-20000,-20000,-20000}, max = {20000,20000,20000};

    /* Initialize mini-renderer */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    sizes = MstUniverseDefaultSizes;

    sizes.dynamicBodiesMaxCount = 200;
    sizes.dynamicConstraintsMaxCount = 1000;
    sizes.materialsMaxCount = 10;
    sizes.collisionModelsMaxCount = 300;
    sizes.collisionPairsMaxCount = 1000;
    sizes.collisionUserGeometryTypesMaxCount = 0;
    sizes.collisionGeometryInstancesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    app      = MeAppCreateFromUniverse(universe, rc);

    world     = MstUniverseGetWorld(universe);
    space     = MstUniverseGetSpace(universe);
    bridge    = MstUniverseGetBridge(universe);
    framework = MstUniverseGetFramework(universe);

    MdtWorldSetEpsilon(world, 0.0001f);

    /* Initialize physics world */
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /* Set collision/dynamics callbacks */
    MstSetWorldHandlers(world);

    /* Set heightfield so +y is up */
    MeMatrix4TMMakeIdentity(terrainTransform);

    /* Materials */
    objectMaterial = MstBridgeGetDefaultMaterial();
    groundMaterial = MstBridgeGetNewMaterial(bridge);
    trackMaterial = MstBridgeGetNewMaterial(bridge);

    /* Track -> Ground */
    params = MstBridgeGetContactParams(bridge, groundMaterial, trackMaterial);

    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, TRACK_FRICTION);
    MdtContactParamsSetSoftness(params, TRACK_SOFTNESS);

    MstBridgeSetPerContactCB(bridge, groundMaterial, trackMaterial,
                             TrackGroundCB);

    /* Object -> Ground */
    params = MstBridgeGetContactParams(bridge, groundMaterial, objectMaterial);

    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, OBJECT_FRICTION);
    MdtContactParamsSetSoftness(params, OBJECT_SOFTNESS);

    MstBridgeSetPerContactCB(bridge, groundMaterial, objectMaterial,
                             ObjectGroundCB);

    /* Terrain height field */
    HeightFieldFromBMP(&heightField, "terrain2", 15);

    /* Just use a real big bounding-box... crude */
    terrainPrim = McdTriangleListCreate(framework,
        min,max,TRILIST_SIZE,TriListGeneratorCB);

    ((McdTriangleList*)terrainPrim)->triangleMaxCount = TRILIST_SIZE;

    terrainCM = McdModelCreate(terrainPrim);
    McdModelSetTransformPtr(terrainCM, terrainTransform);
    McdSpaceInsertModel(space, terrainCM);
    McdModelSetMaterial(terrainCM, groundMaterial);

    McdSpaceUpdateModel(terrainCM);
    McdSpaceFreezeModel(terrainCM);

    terrainG = HeightfieldCreateGraphic(rc, &heightField, grassColor,
                                        terrainTransform);

    RGraphicSetTexture(rc, terrainG, "stone");

    McdFrameworkGetDefaultRequestPtr(framework)->faceNormalsFirst = 1;
    McdFrameworkGetDefaultRequestPtr(framework)->contactMaxCount = 20;


    /* Make The Tank */
    tank = MdtBodyCreate(world);
    MdtMakeInertiaTensorBox(TANK_MASS,
        chassisSize[0], chassisSize[1], chassisSize[2], I);
    MdtBodySetMass(tank, TANK_MASS);
    MdtBodySetInertiaTensor(tank, I);
    {
        MeMatrix4 comTM;
        MeMatrix4TMMakeIdentity(comTM);
        comTM[3][1] = TANK_COM_DROP;
        MdtBodySetCenterOfMassRelativeTransform(tank, comTM);
    }
    MdtBodySetLinearVelocityDamping(tank, 0.5f);
    MdtBodySetAngularVelocityDamping(tank, 0.5f);

    MdtBodySetPosition(tank,
        START_POS_X,
        START_HEIGHT + FindHeight(&heightField,START_POS_X,START_POS_Z),
        START_POS_Z);

    MdtBodyEnable(tank);

    tankPrim = McdBoxCreate(framework,
        chassisSize[0], chassisSize[1], chassisSize[2]);
    tankCM = McdModelCreate(tankPrim);
    McdModelSetBody(tankCM, tank);
    McdModelSetMaterial(tankCM, objectMaterial);
    McdSpaceInsertModel(space, tankCM);

    tankTM = MdtBodyGetTransformPtr(tank);
    tankG = RGraphicBoxCreate(rc,
        chassisSize[0], chassisSize[1], chassisSize[2],
        tankColor, tankTM);

    /* Tracks */
    trackPrim = McdSphereCreate(framework,TRACK_RADIUS);

    trackColTM = (MeMatrix4 *)MeMemoryAPI.createAligned(
        NUM_SEGMENTS*2*sizeof(MeMatrix4),16);
    trackRelTM = (MeMatrix4 *)MeMemoryAPI.createAligned(
        NUM_SEGMENTS*2*sizeof(MeMatrix4),16);

    for (i=0; i<2; i++) /* For each side */
    {
        for (j=0; j<NUM_SEGMENTS; j++) /* For each track sphere */
        {

            trackCM[i][j] = McdModelCreate(trackPrim);
            McdModelSetBody(trackCM[i][j], tank);

            McdModelSetMaterial(trackCM[i][j], trackMaterial);

            /* Set relative transform between tank origin and each sphere */
            MeMatrix4TMMakeIdentity(trackRelTM[i+j*2]);

            trackRelTM[i+j*2][3][0] = TRACK_OFFSET - j * TRACK_SPACING;
            trackRelTM[i+j*2][3][1] = -TRACK_DROP;
            trackRelTM[i+j*2][3][2] = (i==0)
                ? (MeReal)TRACK_SEP/2 : -(MeReal)TRACK_SEP/2;

            McdModelSetRelativeTransformPtrs(trackCM[i][j],
                trackRelTM[i+j*2], tankTM, trackColTM[i+j*2],0);

            McdSpaceInsertModel(space, trackCM[i][j]);

            trackInfo[i][j].side = i;
            trackInfo[i][j].segment = j;
            trackInfo[i][j].rideHeight = 0;

            McdModelSetUserData(trackCM[i][j], (void*)&(trackInfo[i][j]));

            trackG[i][j] = RGraphicSphereCreate(rc,
                TRACK_RADIUS, tankColor, trackGraTM[i][j]);

            RGraphicSetTexture(rc, trackG[i][j], "wood");
        }

        /* Disable all collision between spheres on the
           same side of the track */
        for(j=0; j<NUM_SEGMENTS-1; j++)
        {
            for(k=j+1; k<NUM_SEGMENTS; k++)
            {
                McdSpaceDisablePair(trackCM[i][j], trackCM[i][k]);
            }
        }

        /* Disable collisions between tracks and tank itself */
        for(j=0; j<NUM_SEGMENTS; j++)
        {
            McdSpaceDisablePair(trackCM[i][j], tankCM);
        }
    }

    /* disable pairs of spheres from the opposite track */
    for(j=0; j<NUM_SEGMENTS; j++)
    {
        for (k=0; k<NUM_SEGMENTS; k++)
            McdSpaceDisablePair(trackCM[0][j], trackCM[1][k]);
    }

    coneLimit = MdtConeLimitCreate(world);
    MdtConeLimitSetBodies(coneLimit, tank, 0);
    MdtConeLimitSetAxes(coneLimit, 0, 1, 0, 0, 0, 1);
    MdtConeLimitSetConeHalfAngle(coneLimit, MAX_TILT);
    MdtConeLimitEnable(coneLimit);

    /* Build collision space. */
    McdSpaceBuild(space);

    RPerformanceBarCreate(rc);

    RRenderSetActionNCallBack(rc, 2, Reset, 0);
    RRenderSetActionNCallBack(rc, 3, ToggleOnIce, 0);
    RRenderSetActionNCallBack(rc, 4, ToggleFollowCam, 0);
    RRenderSetActionNCallBack(rc, 5, ToggleDebugInfo, 0);

    RRenderSetUpCallBack(rc, IncreaseSpeed, 0);
    RRenderSetDownCallBack(rc, DecreaseSpeed, 0);
    RRenderSetLeftCallBack(rc, TurnLeft, 0);
    RRenderSetRightCallBack(rc, TurnRight, 0);

    RCameraSetView(rc, 30.0f, ME_PI/4, ME_PI/6);

    RRenderSetWindowTitle(rc, "Tank example");
    RRenderCreateUserHelp(rc,help,sizeof help/sizeof help[0]);
    RRenderToggleUserHelp(rc);

    RRenderSkydomeCreate(rc, "skydome", 2, 1);

/* reduce the water plane on the PS2 to stop the frame rate dropping */
#ifdef PS2
    waterG = RGraphicGroundPlaneCreate(rc,200.0f,30,waterColor,WATER_LEVEL);
#else
    waterG = RGraphicGroundPlaneCreate(rc,500.0f,30,waterColor,WATER_LEVEL);
#endif
    RGraphicSetTexture(rc, waterG, "stone");

    CreateDebugLines(rc, terrainTransform);

#ifndef PS2
    atexit(cleanup);
#endif

    /* Pass control to the renderer loop */
    RRun(rc, Tick, 0);

    return 0;
}

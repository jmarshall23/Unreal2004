/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:33 $ - Revision: $Revision: 1.79.2.7 $

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

  This program demonstrates how to create a geometrical composite
  object (chair) using a set of primitives available in the collision
  toolkit. You can break the chair by shooting a wrecking ball
*/

#include <stdlib.h>
#include <stdio.h>

#include <MeMath.h>
#include <Mcd.h>
#include <MeViewer.h>
#include <MeApp.h>
#include <McdPrimitives.h>

#ifdef PS2
#define SQUARE_SIZE 2
#define MAX_MATRIX_SIZE 36
#else
#define SQUARE_SIZE 4
#define MAX_MATRIX_SIZE 100
#endif

#define CHAIR_COUNT SQUARE_SIZE*SQUARE_SIZE
#define MODEL_COUNT (CHAIR_COUNT*10+3)
#define BODY_COUNT (CHAIR_COUNT*10+3)
#define MODELPAIR_COUNT CHAIR_COUNT*100
#define PLANK_COUNT CHAIR_COUNT*8
#define CONTACT_COUNT MODELPAIR_COUNT*10

#define SQUARE_CENTRE ((MeReal)((SQUARE_SIZE-1)*0.5))


/* number of box parts in the chair */
#define NParts        8
/* number of  shooting balls */
#define NBalls        2

MdtWorldID world;
McdSpaceID space;
MstBridgeID bridge;
McdFrameworkID framework;
MdtContactParamsID params;

/*
Graphics representations.
*/

RGraphic *ballG[NBalls];
RGraphic *planeG;
RRender  *rc = 0;
float color[4] = { 1, 1, 1, 0 };

MeApp    *meapp;

McdGeometryID chairGeom, back1Geom, back2Geom, legGeom, seatGeom;
McdGeometryID ballGeom = 0;
McdGeometryID planeGeom = 0;

McdModelID ballCM[NBalls];
McdModelID planeCM;

MdtBodyID ball[NBalls];

int smashMode = 0; // nothing smashes until wrecking ball is fired!

typedef enum
{
    kChairType = 0,
    kPlankType = 1
} ThingType;

/* Struct MUST be 16-byte multiple length. */
typedef struct
{
    int type;
    McdModelID cm;
    MdtBodyID dm;
    int active; 

    MeMatrix4 tm[8]; /* This MUST be 16-byte multiple offset from start. */

    RGraphic *g[8];
} Chair;

typedef struct
{
    int type;
    McdModelID cm;
    MdtBodyID dm;
} Plank;

Plank* plank;
Chair* chair;

struct {
    Chair *chair;
    MeVector3 velocity, position;
} toSmash[CHAIR_COUNT];

Plank *toDelete[1000];
int toDeleteCount = 0, toSmashCount = 0;

int plankCount;


MeReal    step = (MeReal) 0.02;
MeReal    gravity[3] = { 0, -6, 0 };

MeMatrix4* tmPlane;

MeMatrix4 baseTMPlane = 
{{1, 0, 0, 0},
{0, 0,-1, 0},
{0, 1, 0, 0},
{0, 0, 0, 1}};

MeReal seatSize[3]  = {2.4f, 0.3f, 2.4f};
MeReal back1Size[3] = {0.4f, 1.475f, 0.2f};
MeReal back2Size[3] = {1.575f, 0.2f, 0.2f};
MeReal legSize[3]   = {0.4f, 1.675f, 0.4f};

char     *help[] =
{
        "Press $ACTION2 to reset",
        "Press $ACTION3 to shoot",
        "Press $ACTION4 for wrecking ball",
        "$MOUSE - mouse force"
};


void
buildChairGeometry()
{
    int       i;
    static MeReal transl[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
    MeMatrix4 tmRel[NParts];

    /*
    Initialze useful transformation matrices
    */
    for (i = 0; i < NParts; i++) {
        MeMatrix4TMMakeIdentity(tmRel[i]);
    }

    chairGeom = McdAggregateCreate(framework,8);

    /*  The Seat */
    seatGeom = McdBoxCreate(framework,seatSize[0], seatSize[1], seatSize[2]);
    MeMatrix4TMSetPosition(tmRel[0], 0.f, 0.5f, 0.f);
    McdAggregateAddElement(chairGeom, seatGeom, tmRel[0]);

    /*  The Back */
    back1Geom = McdBoxCreate(framework, back1Size[0], back1Size[1], back1Size[2]);
    MeMatrix4TMSetPosition(tmRel[1], -1.0f, 1.4f, 1.1f);
    McdAggregateAddElement(chairGeom, back1Geom, tmRel[1]);
    MeMatrix4TMSetPosition(tmRel[2], 1.0f, 1.4f, 1.1f);
    McdAggregateAddElement(chairGeom, back1Geom, tmRel[2]);

    back2Geom = McdBoxCreate(framework, back2Size[0], back2Size[1], back2Size[2]);
    MeMatrix4TMSetPosition(tmRel[3], 0.f, 2.0f, 1.1f);
    McdAggregateAddElement(chairGeom, back2Geom, tmRel[3]);

    /* The Four legs */
    legGeom = McdBoxCreate(framework, legSize[0], legSize[1], legSize[2]);

    for (i = 4; i < NParts; i++)
    {
        MeMatrix4TMSetPosition(tmRel[i], transl[i-4][0], -0.5f, transl[i-4][1]);
        McdAggregateAddElement(chairGeom, legGeom, tmRel[i]);
    }
}


void createChair(Chair *c, MeReal x, MeReal y, MeReal z)
{
    int i;
    c->type = kChairType;
    /* back */
    c->g[0] = RGraphicBoxCreate(rc, seatSize[0], seatSize[1], seatSize[2], color, c->tm[0]);
    c->g[1] = RGraphicBoxCreate(rc, back1Size[0], back1Size[1], back1Size[2], color, c->tm[1]);
    c->g[2] = RGraphicBoxCreate(rc, back1Size[0], back1Size[1], back1Size[2], color, c->tm[2]);
    c->g[3] = RGraphicBoxCreate(rc, back2Size[0], back2Size[1], back2Size[2], color, c->tm[3]);
    
    /* legs */
    for (i = 4; i < NParts; i++) {
        c->g[i] = RGraphicBoxCreate(rc, legSize[0], legSize[1], legSize[2], color, c->tm[i]);
    }
    for(i=0;i<8;i++)
        RGraphicSetTexture(rc,c->g[i],"wood1");

    c->active = 0;
    c->cm = 0;
    c->dm = 0;
}

void resetChair(Chair *c, MeReal x, MeReal y, MeReal z)
{
    McdGeometryInstanceID chairIns, childIns;
    int i;

    if(!c->active)
    {
        c->cm = McdModelCreate(chairGeom);
        chairIns = McdModelGetGeometryInstance(c->cm);
        for(i=0;i<8;i++)
        {
            childIns = McdGeometryInstanceGetChild(chairIns,i);
            McdGeometryInstanceSetTransformPtr(childIns, c->tm[i]);
        }

        c->dm = MdtBodyCreate(world);
        MdtBodySetMass(c->dm,1.0f);
        MdtBodySetSphericalInertiaTensor(c->dm,0.5f);

        McdModelSetBody(c->cm,c->dm);
        McdSpaceInsertModel(space,c->cm);

        for(i=0;i<8;i++)
            RGraphicSetTransformPtr(c->g[i],c->tm[i]);

        McdModelSetUserData(c->cm,c);
    }


    MdtBodySetPosition(c->dm, x, y, z);
    MdtBodySetQuaternion(c->dm, 1, 0, 0,0);
    MdtBodySetLinearVelocity(c->dm, 0,0,0);
    MdtBodySetAngularVelocity(c->dm, 0,0,0);
    MdtBodyEnable(c->dm);
    c->active = 1;
}


void MEAPI
smashChair(Chair *c, MeVector3 position, MeVector3 velocity)
{
    int       i;
    MeVector3 linearVel, angularVel;
    McdGeometryInstanceID chairIns, childIns;
    MeVector3 v;

    MdtBodyGetLinearVelocity(c->dm, linearVel);
    MdtBodyGetAngularVelocity(c->dm, angularVel);

    chairIns = McdModelGetGeometryInstance(c->cm);

    for (i = 0; i < 8; i++)
    {
        Plank *p = plank + plankCount++;
        p->type = kPlankType;

        childIns = McdGeometryInstanceGetChild(chairIns,i);
        p->cm = McdModelCreate(McdGeometryInstanceGetGeometry(childIns));
        p->dm = MdtBodyCreate(world);
        MdtBodySetTransform(p->dm,McdGeometryInstanceGetTransformPtr(childIns));
        McdModelSetBody(p->cm, p->dm);
        RGraphicSetTransformPtr(c->g[i], MdtBodyGetTransformPtr(p->dm));

        MdtBodyGetPosition(p->dm,v);
        MeVector3Set(linearVel,(v[0]+velocity[0])/2,(v[1]+2)/2,(v[2]+velocity[2])/2);

        MdtBodySetLinearVelocity(p->dm, linearVel[0], linearVel[1], linearVel[2]);
        MdtBodySetAngularVelocity(p->dm, angularVel[0], angularVel[1], angularVel[2]);
        McdSpaceInsertModel(space, p->cm);
        MdtBodyEnable(p->dm);
        McdModelSetUserData(p->cm,p);
    }

    MdtBodyDisable(c->dm);
    McdSpaceRemoveModel(c->cm);

    toDelete[toDeleteCount++] = (Plank *)c;
    c->active = 0;
}


/**
*
*/
void MEAPI
shoot(RRender * rc, void *userData)
{
    int       i;
    MeReal    v[3];
    MeVector3 cam_pos, cam_lookat;
    int       id = (int) userData;

    RCameraGetPosition(rc, cam_pos);
    RCameraGetLookAt(rc, cam_lookat);

    MdtBodyEnable(ball[id]);

    MdtBodySetPosition(ball[id], cam_pos[0], cam_pos[1] + 1.0f, cam_pos[2]);

    for (i = 0; i < 3; i++)
        v[i] = cam_lookat[i] - cam_pos[i];

    MeVector3Normalize(v);

    for (i = 0; i < 3; i++)
        v[i] *= 12.0;

    MdtBodySetLinearVelocity(ball[id], v[0], v[1], v[2]);
    MdtBodySetAngularVelocity(ball[id], 0, 0, 0);

    if(id==1)
    {
        MdtContactParamsSetMaxAdhesiveForce(params,MEINFINITY);
        smashMode = 1;
    }

}


/**
*
*/
void MEAPI
smashCallback(McdIntersectResult * r)
{
    McdModelID m1 = r->pair->model1;
    McdModelID m2 = r->pair->model2;
    MdtBodyID body;

    Plank *p1 = (Plank *)McdModelGetUserData(m1);
    Plank *p2 = (Plank *)McdModelGetUserData(m2);

    if(!smashMode)
        return;

    if (p1 && p1->type == kChairType &&
          (m2 == ballCM[1] || (p2 && p2->type==kPlankType)))
    {
        toSmash[toSmashCount].chair = (Chair *)p1;

        body = p2?p2->dm:ball[1];
        MdtBodyGetPosition(body,toSmash[toSmashCount].position);
        MdtBodyGetLinearVelocity(body,toSmash[toSmashCount].velocity);
        toSmashCount++;
    }

    if (p2 && p2->type == kChairType &&
          (m1 == ballCM[1] || (p1 && p1->type==kPlankType)))
    {
        toSmash[toSmashCount].chair = (Chair *)p2;
        body = p1?p1->dm:ball[1];
        MdtBodyGetPosition(body,toSmash[toSmashCount].position);
        MdtBodyGetLinearVelocity(body,toSmash[toSmashCount].velocity);
        toSmashCount++;
    }
}

/**
*
*/


/*
Tick() is a callback function called from the renderer's main loop
to evolve the world by 'step' seconds
*/
void MEAPI
tick(RRender * rc, void *userdata)
{
    static int ct = 0;
    MeAppStep(meapp);

    while(toSmashCount>0)
    {
        Chair *c = toSmash[--toSmashCount].chair;
        if(c->active)
            smashChair(c,
                       toSmash[toSmashCount].position,
                       toSmash[toSmashCount].velocity);
    }

    /* This gives modelpairs a chance to be goodbye'd
       before things they refer to get deleted */

    MeProfileStartSection("Collision", 0);
    McdSpaceUpdateAll(space);

    MstBridgeUpdateContacts(bridge, space, world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, step);
    MeProfileEndSection("Dynamics");

    while(toDeleteCount>0)
    {
        toDeleteCount--;
        McdModelDestroy(toDelete[toDeleteCount]->cm);

        if(toDelete[toDeleteCount]->dm == meapp->mouseInfo.dragBody)
            meapp->mouseInfo.dragBody = 0;

        MdtBodyDestroy(toDelete[toDeleteCount]->dm);

    }
}

void destroyPlanks()
{
    int i;
    
    for(i=0;i<plankCount;i++)
    {
        MdtBodyDisable(plank[i].dm);
        McdSpaceRemoveModel(plank[i].cm);
    }
    McdSpaceUpdateAll(space);
    MstBridgeUpdateTransitions(bridge,space,world);
    
    for(i=0;i<plankCount;i++)
    {
        McdModelDestroy(plank[i].cm);
        MdtBodyDestroy(plank[i].dm);
    }

}

void destroyChairs()
{
    int i;


    for(i=0;i<CHAIR_COUNT;i++)
    {
        Chair *c = chair+i;
        if(c->active)        
            McdModelDestroy(c->cm);
    }
}

/**
*
*/
void MEAPI_CDECL
cleanup(void)
{
    MeAppDestroy(meapp);
    /* graphics */
    RRenderContextDestroy(rc);

    MdtWorldDestroy(world);
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    McdTerm(framework);

    MeMemoryAPI.destroyAligned(tmPlane);
    MeMemoryAPI.destroyAligned(plank);
    MeMemoryAPI.destroyAligned(chair);
}



/**
*
*/
void MEAPI
reset(RRender * rc, void *userData)
{
    int i,j;

    destroyPlanks();

    for(i=0;i<SQUARE_SIZE;i++)
        for(j=0;j<SQUARE_SIZE;j++)
            resetChair(chair+(i*SQUARE_SIZE+j),
                        ((MeReal)i - SQUARE_CENTRE)*3,
                        4,
                        ((MeReal)j - SQUARE_CENTRE)*3);

    for (i = 0; i < NBalls; i++)
    {
        MdtBodyEnable(ball[i]);
        MdtBodySetPosition(ball[i], -10 - i * 3.0f, 3, 0);
        MdtBodySetQuaternion(ball[i], 1, 0, 0,0);
        MdtBodySetLinearVelocity(ball[i], 0,0,0);
        MdtBodySetAngularVelocity(ball[i], 0,0,0);
    }

    MdtContactParamsSetMaxAdhesiveForce(params,0);
    smashMode = 0;
    toSmashCount = 0;
    toDeleteCount = 0;
    plankCount = 0;

}

/**
*
*/
int MEAPI_CDECL
main(int argc, const char **argv)
{
    int       i,j;
    MeMatrix4Ptr tm;

    MeCommandLineOptions *options;

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    /*
        Create and initialize a dynamics world.
    */
    world = MdtWorldCreate(BODY_COUNT, CONTACT_COUNT, 1, 1);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    MdtWorldSetAutoDisable(world,1);
    MdtWorldSetMaxMatrixSize(world,MAX_MATRIX_SIZE);

    /*
        Collision initialization.
    */
    framework = McdInit(0, MODEL_COUNT, MODEL_COUNT*10, 1);
    McdPrimitivesRegisterTypes(framework);
    McdPrimitivesRegisterInteractions(framework);
    McdAggregateRegisterType(framework);
    McdAggregateRegisterInteractions(framework);

    /*
        Create a collision space.
    */
    space = McdSpaceAxisSortCreate(framework, McdAllAxes,
                                   MODEL_COUNT, MODELPAIR_COUNT);

    /*
        Link Collision and Dynamics
    */

    bridge = MstBridgeCreate(framework,10);
    MstSetWorldHandlers(world);
    MstBridgeSetIntersectCB(bridge, 0, 0, smashCallback);
    params = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(),
        MstBridgeGetDefaultMaterial());

#ifdef PS2
    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, MEINFINITY);
    MdtContactParamsSetPrimarySlip(params, 0.3f);
    MdtContactParamsSetSecondarySlip(params, 0.3f);
    MdtContactParamsSetRestitution(params, 0.3f);
#else

    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, 3.0);
    MdtContactParamsSetRestitution(params, (MeReal) 0.3);
#endif

    /*
        Create balls
    */
    ballGeom = McdSphereCreate(framework,0.5f);

    for (i = 0; i < NBalls; i++) {
        ball[i] = MdtBodyCreate(world);
        MdtBodyEnable(ball[i]);
        MdtBodySetMass(ball[i], 1.0f);
        ballCM[i] = McdModelCreate(ballGeom);
        McdSpaceInsertModel(space, ballCM[i]);
        McdModelSetBody(ballCM[i], ball[i]);
    }

    /*
        create ground
    */
    planeGeom = McdPlaneCreate(framework);

    planeCM = McdModelCreate(planeGeom);

    tmPlane = (MeMatrix4*)MeMemoryAPI.createAligned(sizeof(MeMatrix4),16);
    MeMatrix4Copy(*tmPlane, baseTMPlane);
    McdModelSetTransformPtr(planeCM, *tmPlane);

    McdSpaceInsertModel(space, planeCM);
    McdSpaceUpdateModel(planeCM);
    McdSpaceFreezeModel(planeCM);

    /* create the collision geometry for the chair */

    plank = (Plank*)MeMemoryAPI.createAligned(PLANK_COUNT * sizeof(Plank), 16);
    chair = (Chair*)MeMemoryAPI.createAligned(CHAIR_COUNT * sizeof(Chair), 16);

    buildChairGeometry();

    
    /* sphere graphics */
    for (i = 0; i < NBalls; i++) {
        tm = MdtBodyGetTransformPtr(ball[i]);
        ballG[i] = RGraphicSphereCreate(rc, 0.5, color, tm);
        if (i == 1)
            RGraphicSetTexture(rc, ballG[i], "rock");
        else
            RGraphicSetTexture(rc, ballG[i], "ME_ball3");
    }

    /*
        Set up camera.
    */
    RCameraRotateAngle(rc, -0.3f);
    RCameraRotateElevation(rc, 0.6f);
#if (SQUARE_SIZE < 3)
    RCameraZoom(rc,0.4f);
    planeG = RGraphicGroundPlaneCreate(rc, 20.0f, 20, color, 0);
#else
    RCameraZoom(rc,4.0f);
    planeG = RGraphicGroundPlaneCreate(rc, 30.0f, 30, color, 0);
#endif

    /* ground graphics */

    RGraphicSetTexture(rc, planeG, "checkerboard");

    RRenderSetWindowTitle(rc, "Chair example");
    RRenderCreateUserHelp(rc, help, 4);
    RRenderToggleUserHelp(rc);

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, (void *) 0);
    RRenderSetActionNCallBack(rc, 4, shoot, (void *) 1);

    meapp = MeAppCreate(world, space, rc);
    RPerformanceBarCreate(rc);

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void *) meapp);

    plankCount = 0;


    /*
        Cleanup after simulation.
    */

    atexit(cleanup);
    
    /*
        nRun the Simulation.
    */

    for(i=0;i<SQUARE_SIZE;i++)
        for(j=0;j<SQUARE_SIZE;j++)
            createChair(chair+(i*SQUARE_SIZE+j),
                        ((MeReal)i - SQUARE_CENTRE)*3,
                        4,
                        ((MeReal)j - SQUARE_CENTRE)*3);

    reset(rc,0);

    RRun(rc, tick, 0);

    return 0;
}

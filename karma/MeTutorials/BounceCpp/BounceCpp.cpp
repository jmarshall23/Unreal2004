/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:32 $ - Revision: $Revision: 1.7.6.2 $

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



#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>
#include <MeViewer.h>

RRender *rc;
RGraphic *sphereG;
RGraphic *planeG;

char *help[] =
{
    "$ACTION2 resets the simulation",
        "$ACTION3 shoots the ball"
};

float color[4] = { 1, 1, 1, 1 };

MdtWorldID world;
MdtBodyID body;
MdtContactID contact;

int numContacts = 0;

MeReal step = (MeReal)(0.03);

class MyMdtConstraintIteratorCB : public MdtConstraintIteratorCB
{
public:
    void MEAPI operator()(const MdtConstraintID c, void* ccbdata) 
    {
        MeInfo(0,"yay!");
    }
};

MyMdtConstraintIteratorCB *forall = new MyMdtConstraintIteratorCB;

void MEAPI shoot(RRender *rc, void *userData)
{
    int i;
    MeReal u[3], v[3];
    MeReal norm;
    
    RCameraGetPosition(rc,v);
    body->setPosition(v[0],v[1],v[2]);
    
    RCameraGetLookAt(rc,u);
    for (i = 0; i < 3; i++)
        v[i] = u[i]-v[i];
    
    norm = MeRecipSqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    
    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;
    
    for (i = 0; i < 3; i++)
        v[i] *= 20;
    
    body->setLinearVelocity(v[0],v[1],v[2]);
    
    body->setAngularVelocity(0,0,0);
    
    body->enable();
}


void MEAPI tick(RRender *rc, void *userData)
{
    MeVector3 pos;
    MeReal penetration;
    
    rc=rc; /* To ingore warnings */
    
    if (contact)
    {
        contact->disable();
        contact->destroy();
        contact = 0;
    }
    
    body->getPosition(pos);

    penetration = 0 - (pos[1] - (MeReal)(2.0));
    
    if (penetration > 0)
    {
        MdtContactParamsID params;

        contact = MdtContact::create(world);
        contact->setBodies(body,0);
        contact->setPosition(pos[0],pos[1]-2,pos[2]);
        contact->setNormal(0,1,0);
        contact->setPenetration(penetration);

        params = contact->getParams();
        params->setType(MdtContactTypeFriction2D);
        params->setFriction(5);
        params->setRestitution(0.6f);
        
        contact->enable();
        body->forAllConstraints(forall,0);

    }
    
    world->step(step);
    
}


void MEAPI reset(RRender *rc, void *userData)
{
    body->setPosition(0,30,0);
    body->setLinearVelocity(0,0,0);
    body->setAngularVelocity(0,0,0);
    body->setQuaternion(1,0,0,0);
}

void MEAPI_CDECL cleanup(void)
{
    world->destroy();
    RRenderContextDestroy(rc);
}

int MEAPI_CDECL main(int argc, const char *(argv[]))
{
    
    MeMatrix3 I;
    MeVector3 v;
    float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    MeCommandLineOptions* options;
    
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    world = MdtWorld::create(1,1);
    world->setGravity(0,-10,0);

    body = MdtBody::create(world);

    world->setAutoDisable(1);
    body->setMass(1);

    body->setLinearVelocityDamping(0.2f);

    MdtMakeInertiaTensorSphere(1, 1, I);
    body->setInertiaTensor(I);

    reset(rc,0);

    body->enable();

    RCameraSetView(rc, 40, 0.1f, 1.4f);
    v[0] = 4; v[1] = 0; v[2] = 0;
    RCameraSetLookAt(rc, v);

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);

    RRenderSetWindowTitle(rc,"Bounce tutorial");
    RRenderCreateUserHelp(rc, help, 2);
    RRenderToggleUserHelp(rc);

    sphereG = RGraphicSphereCreate(rc, 2, color, 0);
    RGraphicSetTexture(rc, sphereG, "ME_ball3");
    planeG = RGraphicGroundPlaneCreate(rc, 50, 2, white, 0);
    RGraphicSetTexture(rc, planeG, "checkerboard");

    RGraphicSetTransformPtr(sphereG, MdtBodyGetTransformPtr(body));

    atexit(cleanup);

    RRun(rc, tick,0);

    return 0;
}

/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.1.1.1.10.1 $

   This software and its accompanying manuals have been developed
   by Mathengine PLC ("MathEngine") and the copyright and all other
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
#include <MeApp.h>
#include <RGeometry.h>

extern MeApp *app;

float geomcolor[] = {0,0,1,1};
float bodyOriginColor[] = {0,1,1,1};
float massOriginColor[] = {1,1,0,1};
float BSJointColor[] = {1,1,0,1};
float HingeColor[] = {1,1,0,1};
float CarwheelColor[] = {1,1,0,1};
float PrismaticColor[] = {1,1,0,1};

RGraphic *RModelGraphic[200];
RGraphic *RBodyOrigin[200];
RGraphic *RMassOrigin[200];
RGraphic *RBSJoint[50];
RGraphic *RHinge[50];
RGraphic *RHingeAxis[50];
RGraphic *RCarwheel[50];
RGraphic *RPrismatic[50];

MdtConstraintID bsjointID[50];
MdtConstraintID hingeID[50];
MdtConstraintID carwheelID[50];
MdtConstraintID prismaticID[50];

MeMatrix4 BSJointTM[50];
MeMatrix4 HingeTM[50];
MeMatrix4 CarwheelTM[50];
MeMatrix4 PrismaticTM[50];


int nModelGraphics = 0;
int nBodyOrigins = 0;
int nMassOrigins = 0;
int nBSJoints = 0;
int nHinges = 0;
int nCarwheels = 0;
int nPrismatics = 0;


void MEAPI ToggleDrawCollisionGeom(MeBool b)
{
    McdSpaceModelIterator it;
    McdModelID model;
    int i=0;
    
    if (b) {
        McdSpaceModelIteratorBegin(app->space,&it);

        while(McdSpaceGetModel(app->space,&it,&model))
        {
            if (McdGeometryGetTypeId(McdModelGetGeometry(model)) == kMcdGeometryTypeNull)
                continue;
            RModelGraphic[i] = RGraphicCreateFromModel(app->rc,model,geomcolor);
            RGraphicSetWireframe(RModelGraphic[i],1);
            i++;
        }
        nModelGraphics = i;
    }
    else {
        for (i = 0; i < nModelGraphics; i++) {
            RGraphicDelete(app->rc,RModelGraphic[i],0);
            RModelGraphic[i] = NULL;
        }
        nModelGraphics = 0;
    }
}


void MEAPI ToggleDynamicsOrigins(MeBool b)
{
    int i = 0;
    if (b) {
        MdtBodyID body;
        body = MdtBodyGetFirst(app->world);
        while (body) {
            RBodyOrigin[i] = RGraphicSphereCreate(app->rc,0.05f,bodyOriginColor,MdtBodyGetTransformPtr(body));
            body = MdtBodyGetNext(body);
            i++;
        }
        nBodyOrigins = i;
    }
    else {
        for (i = 0; i < nBodyOrigins; i++) {
            RGraphicDelete(app->rc,RBodyOrigin[i],0);
            RBodyOrigin[i] = NULL;
        }
        nBodyOrigins = 0;
    }
}

/* center of mass */

void MEAPI ToggleMassOrigins(MeBool b)
{
    int i = 0;
    if (b) {
        MdtBodyID body;
        body = MdtBodyGetFirst(app->world);
        while (body) {
            RMassOrigin[i] = RGraphicSphereCreate(app->rc,0.05f,massOriginColor,MdtBodyGetCenterOfMassTransformPtr(body));
            body = MdtBodyGetNext(body);
            i++;
        }
        nMassOrigins = i;
    }
    else {
        for (i = 0; i < nMassOrigins; i++) {
            RGraphicDelete(app->rc,RMassOrigin[i],0);
            RMassOrigin[i] = NULL;
        }
        nMassOrigins = 0;
    }
}


/* constraints */

void MEAPI UpdateConstraintGraphics(void)
{
    int i;

    for (i = 0; i < nBSJoints; i++) {
        MeMatrix4MultiplyMatrix(BSJointTM[i],bsjointID[i]->head.ref1,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(bsjointID[i],0)));
    }
    for (i = 0; i < nHinges; i++) {
        MeMatrix4MultiplyMatrix(HingeTM[i],hingeID[i]->head.ref1,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(hingeID[i],0)));
    }
    for (i = 0; i < nCarwheels; i++) {
        MeMatrix4MultiplyMatrix(CarwheelTM[i],carwheelID[i]->head.ref2,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(carwheelID[i],1)));
    }
    for (i = 0; i < nPrismatics; i++) {
        MeMatrix4MultiplyMatrix(PrismaticTM[i],prismaticID[i]->head.ref1,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(prismaticID[i],0)));
    }

}


void MEAPI ToggleDrawBSJoints(MeBool b)
{
    MdtConstraintID c;
    if (b) {
        c = MdtConstraintGetFirst(app->world);
        while (c) {
            if (MdtConstraintDCastBSJoint(c)) {
                bsjointID[nBSJoints] = c;
                MeMatrix4MultiplyMatrix(BSJointTM[nBSJoints],c->head.ref1,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(c,0)));
                RBSJoint[nBSJoints] = RGraphicSphereCreate(app->rc,0.05f,BSJointColor,BSJointTM[nBSJoints]);
                nBSJoints++;
            }
        c = MdtConstraintGetNext(c);
        }
    }
    else {
        int i;
        for (i = 0; i < nBSJoints; i++) {
            RGraphicDelete(app->rc,RBSJoint[i],0);
            RBSJoint[i] = NULL;
            bsjointID[i] = NULL;
        }
        nBSJoints = 0;
    }
}

void MEAPI ToggleDrawHinges(MeBool b)
{
    MdtConstraintID c;
    if (b) {
        c = MdtConstraintGetFirst(app->world);
        while (c) {
            if (MdtConstraintDCastHinge(c)) {
                MeVector3 origin = {-0.5,0,0},end={0.5,0,0};
                hingeID[nHinges] = c;
                MeMatrix4MultiplyMatrix(HingeTM[nHinges],c->head.ref1,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(c,0)));
                RHinge[nHinges] = RGraphicSphereCreate(app->rc,0.05f,HingeColor,HingeTM[nHinges]);
                RHingeAxis[nHinges] = RGraphicLineCreate(app->rc,origin,end,HingeColor,HingeTM[nHinges]);

                nHinges++;
            }
        c = MdtConstraintGetNext(c);
        }
    }
    else {
        int i;
        for (i = 0; i < nHinges; i++) {
            RGraphicDelete(app->rc,RHinge[i],0);
            RHinge[i] = NULL;
            RGraphicDelete(app->rc,RHingeAxis[i],0);
            RHingeAxis[i] = NULL;
            hingeID[i] = NULL;
        }
        nHinges = 0;
    }
}

void MEAPI ToggleDrawPrismatics(MeBool b)
{
    MdtConstraintID c;
    if (b) {
        c = MdtConstraintGetFirst(app->world);
        while (c) {
            if (MdtConstraintDCastPrismatic(c)) {
                MeVector3 origin = {0,0,0},end={1,0,0};
                prismaticID[nPrismatics] = c;
                MeMatrix4MultiplyMatrix(PrismaticTM[nPrismatics],c->head.ref1,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(c,0)));
                RPrismatic[nPrismatics] = RGraphicLineCreate(app->rc,origin,end,PrismaticColor,PrismaticTM[nPrismatics]);

                nPrismatics++;
            }
        c = MdtConstraintGetNext(c);
        }
    }
    else {
        int i;
        for (i = 0; i < nPrismatics; i++) {
            RGraphicDelete(app->rc,RPrismatic[i],0);
            RPrismatic[i] = NULL;
            prismaticID[i] = NULL;
        }
        nPrismatics = 0;
    }
}

void MEAPI ToggleDrawCarWheelJoints(MeBool b)
{
    MdtConstraintID c;
    if (b) {
        c = MdtConstraintGetFirst(app->world);
        while (c) {
            if (MdtConstraintDCastCarWheel(c)) {
                carwheelID[nCarwheels] = c;
                MeMatrix4MultiplyMatrix(CarwheelTM[nCarwheels],c->head.ref2,MdtBodyGetCenterOfMassTransformPtr(MdtConstraintGetBody(c,1)));
                RCarwheel[nCarwheels] = RGraphicSphereCreate(app->rc,0.05f,CarwheelColor,CarwheelTM[nCarwheels]);
                nCarwheels++;
            }
        c = MdtConstraintGetNext(c);
        }
    }
    else {
        int i;
        for (i = 0; i < nCarwheels; i++) {
            RGraphicDelete(app->rc,RCarwheel[i],0);
            RCarwheel[i] = NULL;
            carwheelID[i] = NULL;
        }
        nCarwheels = 0;
    }
}

/* contacts */

void MEAPI SetContactDrawLength(MeReal l)
{
    MeAppSetContactDrawLength(app,l);
}

void MEAPI ToggleDrawContacts(MeBool d)
{
    MeAppToggleDrawContacts(app,d);
}




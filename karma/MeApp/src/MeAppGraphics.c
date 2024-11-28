/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.64.6.3 $

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
#include <MeApp.h>
#include <MeMath.h>

/**
 * Supply this function to RRenderSetMouseCallBack() in order to
 * enable mouse picking functionality. Mouse picking is done using collision
 * geometry only, so if something only has a graphical representation and
 * no collision it will not get picked. To enable mouse picking you must
 * also call MeAppStep() in your tick function.
 */
void MEAPI
MeAppMousePickCB(RRender *rc, int x, int y, int modifiers,
                 RMouseButtonWhich which, RMouseButtonEvent event,
                 void *userdata)
{
    MeVector3 normClickDir, pos;
    MeApp *app = (MeApp*)userdata;
    MeVector3 zeroVec = {0, 0, 0};

    /* just clicked - see if we got anything! */
    if ((event == kRNewlyPressed) && (which == kRLeftButton)
        && (x >= 0) && (y >= 0) && (x <= 640) && (y <= 448))
    {
        McdModelID model = MeAppPickMcdModel(app, x, y, normClickDir, pos, 0);
        if (model)
        {
            MdtBodyID body = McdModelGetBody(model);
            if (body)
            {
                MeVector3 camPos, camToGrab;
                app->mouseInfo.dragBody = body;
                MdtConvertPositionVector(0, pos, app->mouseInfo.dragBody, app->mouseInfo.grabPosition);
                /*app->mouseInfo.grabPosition[0] =
                  app->mouseInfo.grabPosition[1] =
                    app->mouseInfo.grabPosition[2] = 0;*/

                MeVector3Copy(app->mouseInfo.desiredPosition, pos);

                /* now store distance from camera to object initially. */
                RCameraGetPosition(rc, camPos);
                MeVector3Subtract(camToGrab, pos, camPos);
                app->mouseInfo.range = MeSqrt(MeVector3MagnitudeSqr(camToGrab));
            }
        }
    }
    /* just update 'desiredPosition' */
    else if (event == kRStillPressed)
    {
        if (app->mouseInfo.dragBody)
        {
            MeVector3 clickDir, camPos;

            MeAppFindClickDir(app, x, y, clickDir);
            RCameraGetPosition(rc, camPos);

            MeVector3Scale(clickDir, app->mouseInfo.range);
            MeVector3Add(app->mouseInfo.desiredPosition, camPos, clickDir);
        }
    }
    /* let go of anything. */
    else
    {
        app->mouseInfo.dragBody = 0;
        RGraphicLineMoveEnds(app->mouseInfo.line, zeroVec, zeroVec);
    }
}

/**
 * Recalculates the mouse-spring force each timestep. Called by MeAppStep().
 */
void MEAPI MeAppUpdateMouseSpring(MeApp *app)
{
    MeVector3 grabPosWorld, force, bodyVel, tmp, gravity;

    MdtWorldGetGravity(app->world,gravity);

    if (app->mouseInfo.dragBody)
    {
        /* Make sure the body is always enabled. */
        MdtBodyEnable(app->mouseInfo.dragBody);

        /* Work out vector from grabbed position on object to current mouse position. */
        MdtConvertPositionVector(app->mouseInfo.dragBody,
                                 app->mouseInfo.grabPosition, 0, grabPosWorld);
        MeVector3Subtract(force, app->mouseInfo.desiredPosition, grabPosWorld);

        /* Scale for 'stiffness' part */
        MeVector3Scale(force, app->mouseInfo.mouseSpringStiffness);

        /* Now add velocity damping. */
        MdtBodyGetVelocityAtPoint(app->mouseInfo.dragBody,
                                  grabPosWorld, bodyVel);
        MeVector3Scale(bodyVel, -app->mouseInfo.mouseSpringDamping);

        MeVector3Add(force, force, bodyVel);
        MeVector3Scale(force, MdtBodyGetMass(app->mouseInfo.dragBody));

        /* Apply force and update 'rubber band' */
        MdtBodyAddForceAtPosition(app->mouseInfo.dragBody,
                                  force[0], force[1], force[2],
                                  grabPosWorld[0],
                                  grabPosWorld[1],
                                  grabPosWorld[2]);

        /*  If the magnitude of gravity is more than 15, then compensate 
               for force on body due to gravity. */

        MeVector3Copy(tmp, gravity);
        MeVector3Clamp(tmp, -15, 15);
        MeVector3Subtract(tmp, tmp, gravity);
        MeVector3Scale(tmp, MdtBodyGetMass(app->mouseInfo.dragBody));

        MdtBodyAddForce(app->mouseInfo.dragBody, tmp[0], tmp[1], tmp[2]);

        RGraphicLineMoveEnds(app->mouseInfo.line,
                             grabPosWorld, app->mouseInfo.desiredPosition);
    }

}


/**
 * Initialize drawing of contacts.
 */
void MEAPI MeAppDrawContactsInit(MeApp *const app, float color[4],int max)
{
    int i;
    MeAppContactDrawInfo *d = (MeAppContactDrawInfo*)
                              MeMemoryAPI.create(sizeof(MeAppContactDrawInfo));

    MeVector3 zeroVec = {0};
    d->length = 1;
    d->color[0]=color[0];
    d->color[1]=color[1];
    d->color[2]=color[2];
    d->color[3]=color[3];
    d->maxContacts = max;
    d->contactG = (RGraphic**)MeMemoryAPI.create(sizeof(RGraphic*) * max);
    d->contactsDrawn = 0;
    app->contactDrawInfo = d;
    for (i = 0; i < max; i++)
        d->contactG[i] = RGraphicLineCreate(app->rc,zeroVec,zeroVec,d->color,0);

}

/**
 * Set the length of contact graphics.
 */
void MEAPI MeAppSetContactDrawLength(MeApp *app, MeReal length)
{
    app->contactDrawInfo->length = length;
}

/**
 * Toggles contact drawing.
 */
void MEAPI MeAppToggleDrawContacts(MeApp *app, MeBool d)
{
    int i;
    MeAppContactDrawInfo *c = app->contactDrawInfo;
    MeVector3 zeroVec = {0};
    app->drawContacts = d;
    if (d==1) return;

    for (i = 0; i < c->maxContacts; i++)
        RGraphicLineMoveEnds(c->contactG[i],zeroVec,zeroVec);
}

/**
 * Draw contacts.
 */
void MEAPI MeAppDrawContacts(const MeApp *app)
{
    MeAppContactDrawInfo *d = app->contactDrawInfo;
    MdtConstraintID c;
    MeVector3 line[2] = {{0},{0}};
    MeVector3 normal;
    int i = 0,j;
    MdtContactID contact;

    c = MdtConstraintGetFirst(app->world);

    while (c && d->maxContacts)
    {
        if (MdtConstraintDCastContact(c)) 
        {
            contact = MdtConstraintDCastContact(c);
            if (i < d->maxContacts) 
            {
                MdtContactGetPosition(contact,line[0]);
                MdtContactGetNormal(contact,normal);
                MeVector3Scale(normal,d->length *(1+50*MdtContactGetPenetration(contact)));
                MeVector3Add(line[1],normal,line[0]);
                RGraphicLineMoveEnds(d->contactG[i],line[0],line[1]);                
                i++;
            }
        }
        else if (MdtConstraintDCastContactGroup(c)) 
        {
            contact = MdtContactGroupGetFirstContact(MdtConstraintDCastContactGroup(c)); 
            while (contact && i<d->maxContacts) 
            {
                MdtContactGetPosition(contact,line[0]);
                MdtContactGetNormal(contact,normal);
                MeVector3Scale(normal,d->length*(1+50*MdtContactGetPenetration(contact)));
                MeVector3Add(line[1],normal,line[0]);
                RGraphicLineMoveEnds(d->contactG[i],line[0],line[1]);                
                contact = MdtContactGroupGetNextContact(MdtConstraintDCastContactGroup(c),contact);
                i++;
            }

        }
        c = MdtConstraintGetNext(c);
    }

    d->contactsDrawn = i;

    for (j = i; j < d->maxContacts; j++)
    {
        RGraphicLineMoveEnds(d->contactG[j],line[0],line[1]);
    }

}

/**
 * Utility to calculate the direction vector from the camera lookat position
 * to where the mouse is clicked. The resulting vector is in world space.
 */
void MEAPI
MeAppFindClickDir(const MeApp *const app, int x, int y,
                  MeVector3 normClickDir)
{
    MeVector3 camPos;

    MeVector4 point = {0,0,0,1};
    MeVector4 point_camera, point_world;
    MeMatrix4 invCam;

    point[0] = (MeReal)x;
    point[1] = (MeReal)y;

    MeMatrix4Copy(invCam, app->rc->m_CamMatrix);
    MeMatrix4TMInvert(invCam);

    MeMatrix4MultiplyVector(point_camera, app->rc->m_CamMatrix2D, point);
    MeMatrix4MultiplyVector(point_world, invCam, point_camera);

    RCameraGetPosition(app->rc,camPos);

    MeVector3Subtract(normClickDir, point_world, camPos);

    MeVector3Normalize(normClickDir);
}

/**
 * Find the point in world space (on camera plane) where the user clicked.
 */
void MEAPI
MeAppFindClickPos(const MeApp *const app, int x, int y,
                  MeVector3 clickPos)
{
    MeVector4 point = {0,0,0,1};
    MeVector4 point_camera, point_world;
    MeMatrix4 invCam;

    point[0] = (MeReal)x;
    point[1] = (MeReal)y;

    MeMatrix4Copy(invCam, app->rc->m_CamMatrix);
    MeMatrix4TMInvert(invCam);

    MeMatrix4MultiplyVector(point_camera, app->rc->m_CamMatrix2D, point);
    MeMatrix4MultiplyVector(point_world, invCam, point_camera);

    MeVector3Copy(clickPos, point_world);
}

/**
 * Utility to select a collision model given x and y screen coordinates.
 * Uses McdSpaceGetLineSegFirstIntersection().
 */
McdModelID MEAPI
MeAppPickMcdModel(const MeApp *const app,
                  int x,
                  int y,
                  MeVector3 normClickDir,
                  MeVector3 pos, MeBool orth)
{
    int picked;
    MeVector3 line[2];
    McdLineSegIntersectResult iResult;

    MeVector3 clickDir;

    if (orth)
    {
        MeVector3 lookDir;
        MeAppFindClickPos(app, x, y, line[0]);
        lookDir[0] = app->rc->m_CamMatrix[0][2];
        lookDir[1] = app->rc->m_CamMatrix[1][2];
        lookDir[2] = app->rc->m_CamMatrix[2][2];
        MeVector3Scale(lookDir, 500);
        MeVector3Add(line[1], lookDir, line[0]);

    }
    else
    {
        MeAppFindClickDir(app, x, y, clickDir);
        MeVector3Copy(normClickDir, clickDir);
        
        RCameraGetPosition(app->rc, line[0]);
        MeVector3Normalize(clickDir);
        MeVector3Scale(clickDir, 500);
        MeVector3Add(line[1], clickDir, line[0]);
    }


    picked = McdSpaceGetLineSegFirstIntersection(app->space,
                                                 line[0], line[1], &iResult);

    if (picked)
    {
        MeVector3Copy(pos,iResult.position);
        return iResult.model;
    }

    return NULL;
}





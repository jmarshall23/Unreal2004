/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:33 $ - Revision: $Revision: 1.92.2.1 $

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
   This example shows a simple car interacting with
   terrain (Regular Grid Height Field).
*/

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include <stdio.h>
#include "CarTerrain.h"
#include <McdRGHeightField.h>
#include <RRGHeightfield.h>

/* Global variables. */

/* time stepsize */
const MeReal timeStep = (MeReal)(0.02);

/* gravity */
#define GRAV_MAG (MeReal)(9.81)

/* gravity  magnitude */
const MeReal grav_mag = GRAV_MAG;

/* gravity  vector */
const MeReal gravity[3] = { 0, -GRAV_MAG, 0 };

/* simulation object */
MdtWorldID world;
McdFrameworkID framework;
McdSpaceID space;
MstBridgeID bridge;

/* car physics object */
Car car1;

/* data structure for car parameters */
CarData carData1;

/* Height offset for car at reset */
const MeReal resetHeight = (MeReal)(0.9);

/* Wheel positions */
MeReal pWhV[4][3];

/* contact properties */
short groundMat, frontMat, rearMat, chasisMat;
MdtBclContactParams *params;

McdModelID terrainCM;
McdGeometryID terrainPrim;
RGraphic *terrainG;

McdGeometryID spherePrim[4];
McdGeometryID boxPrim;

/* graphics render context */
RRender *rc;

float red[4]       = { 0.9f, 0.0f,   0.05f,   1.0f};
float orange[4]    = { 1.0f, 0.4f,   0.0f,   1.0f};
float blue[4]      = { 0.0f, 0.598f, 0.797f, 1.0f};
float sandColor[4] = { 1.0f, 0.8f,   0.4f,   1.0f};
float tyreColor[4] = { 0.1f, 0.1f,   0.1f,   1.0f};

/* height field grid data */
MeReal *heights;
int ixGrid = 50;
int iyGrid = 50;
MeReal deltaX = 1.5f;
MeReal deltaY = 1.5f;
MeReal xOrigin;
MeReal yOrigin;
MeReal zval  = 0.5f;
MeReal angHz = 0.7f;

int autoEvolve = 1;

int clickStart[2] = {0, 0};

/* rotate PI/2 about x-axis */
MeMatrix4 terrainTransform =
{
  {  1,  0,  0,  0},
  {  0,  0, -1,  0},
  {  0,  1,  0,  0},
  {  0, -2,  0,  1}
};

MeMatrix4 chassisRelTM;

char *help[2] =
{
    "$ACTION2 - reset",
    "$MOUSE - drive"
};

unsigned int MEAPI SetContactDirection(McdIntersectResult* result, MdtContactGroupID cg)
{
  int i;
  MeVector3 a;
  MeVector3 haxis, normal;
  MdtContactID contact;

  for(contact = MdtContactGroupGetFirstContact(cg); contact!=NULL;
      contact = MdtContactGroupGetNextContact(cg,contact))
  {

    for (i=0; i<4; i++)
    {
      if(car1.body[i] == MdtContactGetBody(contact, 0))
      {

      /* Create principal friction axis (normal x hinge_axis). */

        MdtCarWheelGetHingeAxis(car1.wheel_joint[i], haxis);
        MdtContactGetNormal(contact, normal);

        MeVector3Cross(a, normal, haxis);
        MeVector3Normalize(a);

        MdtContactSetDirection(contact, a[0], a[1], a[2]);
      }
    }
  }

  return 1;
}


void MEAPI MouseCB(RRender *rc, int x, int y,
               int modifiers, RMouseButtonWhich which,
               RMouseButtonEvent event, void *userdata)
{
    if(event == kRNewlyPressed)
    {
        clickStart[0] = x;
        clickStart[1] = y;
    }
    else if(event == kRNewlyReleased)
    {
        car1.drive_torque = 0;
        car1.steering_angle = 0;
    }
    else
    {
        MeReal dx = (MeReal)(x - clickStart[0])/640;
        MeReal dy = (MeReal)(y - clickStart[1])/448;

        if(dx > 1) dx = 1;
        if(dx < -1) dx = -1;
        if(dy > 1) dy = 1;
        if(dy < -1) dy = -1;


        car1.drive_torque = dy * 10;
        car1.steering_angle = dx * car1.params->max_steering_angle;
    }
}


/* Initialize physics and graphics objects for a car */
void CarInit(Car * c, const MeReal *grav, CarData * d)
{
    int k;

    /*
        Initialize physics objects.
        First, initialise chassis body mass, inertia tensor, positions, etc.
    */

    /* chassis inertia tensor */
    MeMatrix3 cI = {{0,0,0},{0,0,0},{0,0,0}};

    MdtBodyID chassisBody;

    /*
        Now, initialise the wheels and wheel joints ;
        tabulate positions of Wheels in the Vehicle body (chassis) reference
        frame
    */
    const MeReal rear_height =
        d->rear_wheel_radius - d->chassis_height_off_ground;

    const MeReal front_height =
        d->front_wheel_radius - d->chassis_height_off_ground;

    cI[0][0] = d->chassis_MoI_roll;
    cI[1][1] = d->chassis_MoI_pitch;
    cI[2][2] = d->chassis_MoI_yaw;

    /* Set wheel positions */
    pWhV[0][0] = pWhV[1][0] = -d->wheelbase / 2;
    pWhV[2][0] = pWhV[3][0] = d->wheelbase / 2;
    pWhV[0][1] = pWhV[1][1] = rear_height;
    pWhV[2][1] = pWhV[3][1] = front_height;
    pWhV[0][2] = pWhV[2][2] = d->rear_track / 2;
    pWhV[1][2] = pWhV[3][2] = -d->rear_track / 2;

    /* Set car parameter */
    c->params = d;

    /* Initialize steering angle; */
    c->steering_angle = 0;
    c->body[4] = MdtBodyCreate(world);
    chassisBody = c->body[4];

    c->drive_torque = 0;

    /*
        Angular and linear velocity damping ensures that the car will come to
        a halt if there is no user input.
    */

    MdtBodySetAngularVelocityDamping(chassisBody, (MeReal)(0.2));
    MdtBodySetLinearVelocityDamping(chassisBody, (MeReal)(0.2));

    /* Initialise inertia */
    MdtBodySetInertiaTensor(chassisBody, cI);
    MdtBodySetMass(chassisBody, d->chassis_mass);
    MdtBodySetPosition(chassisBody, d->chassis_CoM_forward_offset,
        d->chassis_height_off_ground + d->chassis_CoM_upward_offset +
        resetHeight,0);

    /* initialize wheels: back-left back-right front-left front-right */
    for (k = 0; k < 4; k++)
    {
        /* wheel  inertia tensor */
        MeMatrix3 wI = {{0,0,0},{0,0,0},{0,0,0}};

        MdtBodyID wheelBody;
        MeVector3 p;
        MeMatrix3 R;

        /* chassis right->left axis */
        MeReal *haxis;

        /* chassis down->up axis */
        MeReal *saxis;

        /*
            Suspension setup (also see comments in header file)

            load   Load acting on each wheel at equilibrium
            height Height of wheel suspension strut at equilibrium load
            travel Travel of suspension between bottom stop and top stop
            soft   Softness of suspension stops
            damp   Damping ratio of suspension
            equi   Equilibrium position of wheel as a ratio between stops
            ztos   Zero to Semi-load travel of suspension

            zero_load_height    Unloaded height of wheel suspension
            top_stop_height     top stop suspension limit

            bottom_stop_height  bottom stop suspension limit
            Kp                  Spring constant  (proportional)
            Kd                  Damping constant (derivative)
        */

        MeReal load, height, travel, damp, equi, ztos, soft, tmp;

        wI[0][0] = d->wheel_MoI_radial;
        wI[1][1] = d->wheel_MoI_axial;
        wI[2][2] = d->wheel_MoI_radial;

        c->body[k] = MdtBodyCreate(world);
        wheelBody = c->body[k];

        /*
            This must be the first function called on a physics body
            initialise inertia.
        */
        MdtBodySetInertiaTensor(wheelBody, wI);
        MdtBodySetMass(wheelBody, d->wheel_mass);

        /*
            Angular and linear velocity damping ensures that the car will
            come to a halt if there is no user input.
        */

        MdtBodySetAngularVelocityDamping(wheelBody, (MeReal)(0.2));
        MdtBodySetLinearVelocityDamping(wheelBody, (MeReal)(0.2));

        MdtBodySetPosition(wheelBody, pWhV[k][0], pWhV[k][1] + resetHeight,
            pWhV[k][2]);

        /* Initialise car wheel constraint. */
        c->wheel_joint[k] = MdtCarWheelCreate(world);
        MdtCarWheelSetBodies(c->wheel_joint[k], c->body[4], c->body[k]);

        MdtBodyGetPosition(wheelBody, p);
        MdtCarWheelSetPosition(c->wheel_joint[k], p[0], p[1], p[2]);
        MdtBodyGetOrientation(chassisBody, R);

        /* chassis right->left axis */
        haxis = R[2];

        /* chassis down->up axis */
        saxis = R[1];

        MdtCarWheelSetHingeAxis(c->wheel_joint[k],
            haxis[0], haxis[1], haxis[2]);

        MdtCarWheelSetSteeringAxis(c->wheel_joint[k],
            saxis[0], saxis[1], saxis[2]);

        if (k < 2)
        {
            /* We don't want steering on the back wheels so lock them. */
            MdtCarWheelSetSteeringLock(c->wheel_joint[k], 1);
        }

        tmp = d->chassis_CoM_forward_offset / d->wheelbase / 2;

        if (k < 2)
        {
            /* fixed rear wheel */
            load = ((MeReal)(0.25) - tmp) * d->chassis_mass;
            height = -d->chassis_height_off_ground + d->rear_wheel_radius;
            travel = d->rear_suspension_travel;
            soft = d->rear_suspension_soft;
            damp = d->rear_suspension_damp;
            equi = d->rear_suspension_equi;
            ztos = d->rear_suspension_ztos;
        }
        else
        {
            /* steered front wheel */
            load = ((MeReal)(0.25) + tmp) * d->chassis_mass;
            height = -d->chassis_height_off_ground + d->front_wheel_radius;
            travel = d->front_suspension_travel;
            soft = d->front_suspension_soft;
            damp = d->front_suspension_damp;
            equi = d->front_suspension_equi;
            ztos = d->front_suspension_ztos;
        }

        {
            const MeReal grav_mag = MeSqrt(grav[0] * grav[0]
                + grav[1] * grav[1] + grav[2] * grav[2]);

            const MeReal zero_load_height = height - 0.5f * ztos * travel;
            const MeReal top_stop_height = height + (1 - equi) * travel;
            const MeReal bottom_stop_height = height - equi * travel;

            const MeReal Kp = 2 * load * grav_mag / (ztos * travel);
            const MeReal Kd = damp * MeSqrt(load * Kp);

            MdtCarWheelSetSuspension(c->wheel_joint[k], Kp, Kd, soft,
                bottom_stop_height, top_stop_height, zero_load_height);
        }

        /* wheel collision*/
        if(k<2)
        { /* rear wheels */

            spherePrim[k] = McdSphereCreate(framework,d->rear_wheel_radius);
            c->wheelCM[k] = McdModelCreate(spherePrim[k]);

            McdModelSetMaterial(c->wheelCM[k], rearMat);

        }
        else
        { /* front wheels */

            spherePrim[k] = McdSphereCreate(framework,d->front_wheel_radius);
            c->wheelCM[k] = McdModelCreate(spherePrim[k]);
            McdModelSetMaterial(c->wheelCM[k], frontMat);
        }

        McdModelSetBody(c->wheelCM[k], c->body[k]);
        McdSpaceInsertModel(space, c->wheelCM[k]);
    }

    /* chasis collision */
    boxPrim = McdBoxCreate(framework,d->chassis_length, d->chassis_depth,d->chassis_width);
    c->chasisCM = McdModelCreate(boxPrim) ;
    McdModelSetBody(c->chasisCM, c->body[4]);
    McdModelSetMaterial(c->chasisCM, chasisMat);
    McdSpaceInsertModel(space, c->chasisCM);

    /* setup callbacks to set contact direction */
    MstBridgeSetPerPairCB(bridge, rearMat, groundMat, SetContactDirection);
    MstBridgeSetPerPairCB(bridge, frontMat, groundMat, SetContactDirection);

    for (k = 0; k < 5; k++)
        MdtBodyEnable(c->body[k]);

    for (k = 0; k < 4; k++)
        MdtCarWheelEnable(c->wheel_joint[k]);
}

/* Update function to call at each timestep. */
void CarUpdate(Car * c)
{
    /* Get new steering angle and drive. */
    {
        /* Calculate Ackerman steering angles. */
#if 0
        const MeReal tana = MeTan(c->steering_angle);
        const MeReal left_angle = MeAtan2(tana, 1 -
            0.5f * tana * c->params->front_track / c->params->wheelbase);
        const MeReal right_angle = MeAtan2(tana, 1 +
            0.5f * tana * c->params->front_track / c->params->wheelbase);
#endif
        int k;

        for (k = 0; k < 4; k++)
        {
            MeVector3 haxis;
            MdtCarWheelGetHingeAxis(c->wheel_joint[k], haxis);

            /* Wheel torque calculations. */
            if (k < 2)
            {
                /* Rear wheel. */
                const MeReal p = (c->drive_torque > 0)
                    ? 1.0f * c->drive_torque : 0.75f * c->drive_torque;

                MdtBodyAddTorque(c->body[k],
                    p * haxis[0], p * haxis[1], p * haxis[2]);

                MdtBodyAddTorque(c->body[4],
                    -p * haxis[0], -p * haxis[1], -p * haxis[2]);
            }
            else
            {

                /* Front wheel do the steering proportional gap (radians). */
                const MeReal pgap = 0.4f;
                const MeReal width = 0.5f;
                const MeReal speed = 5.0f;
                const MeReal forcemax = 100.0f;
                const MeReal theta =
                    MdtCarWheelGetSteeringAngle(c->wheel_joint[k]);

                MeReal desired_vel = c->steering_angle * width + theta;

                if (desired_vel > pgap)
                    desired_vel = pgap;

                if (desired_vel < -pgap)
                    desired_vel = -pgap;

                MdtCarWheelSetSteeringLimitedForceMotor(c->wheel_joint[k],
                    speed * desired_vel, forcemax);

                {
                    const MeReal p = (c->drive_torque > 0)
                        ? 0.75f * c->drive_torque : c->drive_torque;

                    MdtBodyAddTorque(c->body[k],
                        p * haxis[0], p * haxis[1], p * haxis[2]);

                    MdtBodyAddTorque(c->body[4],
                        -p * haxis[0], -p * haxis[1], -p * haxis[2]);
                }
            }
        }
    }

    /* Set contact parameters for rear wheels */
    params = MstBridgeGetContactParams(bridge,groundMat, rearMat);
    MdtContactParamsSetType(params,MdtContactTypeFriction2D);
    MdtContactParamsSetSecondarySlip(params,c->params->rear_contact_lateral_slip);
    MdtContactParamsSetSoftness(params,c->params->rear_contact_softness);

    /* Set contact parameters for front wheels */
    params = MstBridgeGetContactParams(bridge,groundMat, frontMat);
    MdtContactParamsSetType(params,MdtContactTypeFriction2D);
    MdtContactParamsSetSecondarySlip(params,c->params->front_contact_lateral_slip);
    MdtContactParamsSetSoftness(params,c->params->front_contact_softness);

    /* Set contact parameters for chassis */
    params = MstBridgeGetContactParams(bridge,groundMat, chasisMat);
    MdtContactParamsSetType(params,MdtContactTypeFriction2D);
    MdtContactParamsSetSecondarySlip(params,c->params->front_contact_lateral_slip);
    MdtContactParamsSetSoftness(params,c->params->front_contact_softness);

    {
        MeMatrix4 tm;
        MdtBodyGetTransform(c->body[4], tm);
/*        MeMatrix4Multiply(c->chassisTM, tm, chassisRelTM); */
        MeMatrix4MultiplyMatrix(c->chassisTM, chassisRelTM, tm);
    }

}

/*
    c->steering_angle = x * c->params->max_steering_angle;
    c->drive_torque = -y * 10;
*/

/* Create and connect up the graphics. */
void CarCreateGraphics(Car *c)
{
    CarData *d = c->params;

    MeReal rOuterRadius = d->rear_wheel_radius;
    MeReal rInnerRadius = d->rear_wheel_radius - d->rear_wheel_width;
    MeReal fOuterRadius = d->front_wheel_radius;
    MeReal fInnerRadius = d->front_wheel_radius - d->front_wheel_width;

    MeMatrix4TMMakeIdentity(c->chassisTM);
    c->part[4] = RGraphicLoadASE(rc, "wild.ase", 0.12f, 0.12f, 0.12f, red, c->chassisTM);

    {
        MeMatrix3 R;
        MeMatrix3FromEulerAngles(R, ME_PI/2, 0, -0.1);
        MeMatrix4TMMakeFromRotationAndPosition(chassisRelTM, R, 0, -0.85, 0);
    }

    c->part[0] = RGraphicTorusCreate(rc,
        rInnerRadius, rOuterRadius, tyreColor, MdtBodyGetTransformPtr(c->body[0]));
    c->part[1] = RGraphicTorusCreate(rc,
        rInnerRadius, rOuterRadius, tyreColor, MdtBodyGetTransformPtr(c->body[1]));

    c->part[2] = RGraphicTorusCreate(rc,
        fInnerRadius, fOuterRadius, tyreColor, MdtBodyGetTransformPtr(c->body[2]));
    c->part[3] = RGraphicTorusCreate(rc,
        fInnerRadius, fOuterRadius, tyreColor, MdtBodyGetTransformPtr(c->body[3]));
}

void MEAPI Reset(RRender* rc, void* userData)
{
    int i;
    MdtBodySetPosition(car1.body[4],
        carData1.chassis_CoM_forward_offset,
        carData1.chassis_height_off_ground +
        carData1.chassis_CoM_upward_offset + resetHeight, 0);

    MdtBodySetQuaternion(car1.body[4], 1, 0, 0, 0);
    MdtBodySetLinearVelocity(car1.body[4], 0, 0, 0);
    MdtBodySetAngularVelocity(car1.body[4], 0, 0, 0);

    for(i=0; i<4; i++)
    {
        MdtBodySetPosition(car1.body[i], pWhV[i][0], pWhV[i][1] + resetHeight,
            pWhV[i][2]);
        MdtBodySetQuaternion(car1.body[i], 1, 0, 0, 0);
        MdtBodySetLinearVelocity(car1.body[i], 0, 0, 0);
        MdtBodySetAngularVelocity(car1.body[i], 0, 0, 0);
    }

}

void stepEvolve(void)
{
    MeVector3 pos;

    MeProfileStartSection("Mathengine", 0);

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
    MeProfileEndSection("Dynamics");

    MeProfileEndSection("Mathengine");

    CarUpdate(&car1);

    MdtBodyGetPosition(car1.body[4], pos);
    RCameraSetLookAt(rc, pos);

    if(pos[1] < -15)
    {
        Reset(rc,0);
    }
}

void MEAPI Tick(RRender * rc, void* userData)
{
    if (autoEvolve)
        stepEvolve();
}

void SetCarData1(CarData * c1)
{
    MeReal mrr;

    c1->wheelbase = 1.9f;
    c1->front_track = 1.35f;
    c1->rear_track = 1.35f;
    c1->front_wheel_radius = 0.35f;
    c1->rear_wheel_radius = 0.35f;
    c1->chassis_height_off_ground = 0.55f;

    c1->chassis_mass = 10;
    c1->chassis_CoM_upward_offset = -0.55f;
    c1->chassis_CoM_forward_offset = 0;

    mrr = c1->chassis_mass * c1->wheelbase * c1->front_track;

    c1->chassis_MoI_roll = 0.1f * mrr;
    c1->chassis_MoI_pitch = 0.25f * mrr;
    c1->chassis_MoI_yaw = 0.15f * mrr;

    c1->wheel_mass = 1;
    mrr = c1->wheel_mass * c1->front_wheel_radius * c1->front_wheel_radius;
    c1->wheel_MoI_axial = 1 * mrr;
    c1->wheel_MoI_radial = 1 * mrr;

    c1->front_suspension_travel = 0.25f;
    c1->rear_suspension_travel = 0.25f;
    c1->front_suspension_damp = 1.5f;
    c1->rear_suspension_damp = 1.5f;
    c1->front_suspension_equi = 0.5f;
    c1->rear_suspension_equi = 0.5f;
    c1->front_suspension_ztos = 1.2f;
    c1->rear_suspension_ztos = 1.2f;
    c1->front_suspension_soft = 0.005f;
    c1->rear_suspension_soft = 0.005f;

    c1->max_steering_angle = 45 * ME_PI / 180;

    c1->front_contact_softness = 0.01f;
    c1->rear_contact_softness = 0.01f;
    c1->front_contact_lateral_slip = 0.02f;
    c1->rear_contact_lateral_slip = 0.05f;

    c1->chassis_length = 2.3f;
    c1->chassis_width = 0.8f;
    c1->chassis_depth = 0.5f;
    c1->chassis_forward_offset = -0.05f;
    c1->front_wheel_width = 0.3f;
    c1->rear_wheel_width = 0.3f;
}

void toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}


void MEAPI_CDECL cleanup(void)
{
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    McdTerm(framework);
    MdtWorldDestroy(world);

    RRenderContextDestroy(rc);

    free(heights);
}

int MEAPI_CDECL main(int argc, const char **argv)
{
    int ix, iy, id;
    MeCommandLineOptions* options;

    /* initialize mini-renderer */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    xOrigin = -ixGrid*deltaX*0.5f;
    yOrigin = -iyGrid*deltaY*0.5f;

    /* allocate a pool of memory for Kea to use when solving */
    world = MdtWorldCreate(100, 100);

    /* initialize physics world */
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /* initialise collision */
    framework = McdInit(0, 100, 0, 1);
    McdPrimitivesRegisterTypes(framework);
    McdRGHeightFieldRegisterType(framework);
    McdPrimitivesRegisterInteractions(framework);
    McdRGHeightFieldPrimitivesRegisterInteractions(framework);

    space = McdSpaceAxisSortCreate(framework,McdAllAxes, 100, 200,1);
    bridge = MstBridgeCreate(framework,10);

    MstSetWorldHandlers(world);

    groundMat = MstBridgeGetDefaultMaterial();
    frontMat  = MstBridgeGetNewMaterial(bridge);
    rearMat   = MstBridgeGetNewMaterial(bridge);
    chasisMat = MstBridgeGetNewMaterial(bridge);

    /* initialize car parameters */
    SetCarData1(&carData1);
    CarInit(&car1, gravity, &carData1);
    CarCreateGraphics(&car1);

    /* terrain height field */
    heights = (MeReal*) MeMemoryAPI.create(sizeof(MeReal)*ixGrid*iyGrid);

    id = 0;
    for (iy=0; iy<iyGrid; iy++)
    {
        for (ix=0; ix<ixGrid; ix++)
        {
            heights[id] = zval*MeSin(angHz*ix)*MeSin(angHz*iy);
            id++;
        }
    }

    terrainPrim = McdRGHeightFieldCreate(framework,heights, ixGrid, iyGrid, deltaX, deltaY, xOrigin, yOrigin);
    terrainCM = McdModelCreate(terrainPrim);
    McdModelSetTransformPtr(terrainCM, terrainTransform);
    McdSpaceInsertModel(space, terrainCM);
    McdModelSetMaterial(terrainCM, groundMat);
    McdSpaceUpdateModel(terrainCM);
    McdSpaceFreezeModel(terrainCM);

    terrainG = RGraphicRGHeightfieldCreate(rc,
        ixGrid, iyGrid, deltaX, deltaY, xOrigin, yOrigin,
        heights, sandColor, McdModelGetTransformPtr(terrainCM));

    /* Only use x contacts per pair. */
    /* McdGetDefaultRequestPtr()->contactMaxCount = 3; */

    /* Build collision space. */
    McdSpaceBuild(space);

    RPerformanceBarCreate(rc);

    RRenderSkydomeCreate(rc, "skydome", 2, 1);

    RCameraZoom(rc, -5);

    RRenderSetActionNCallBack(rc, 2, Reset, 0);

    RRenderSetMouseCallBack(rc, MouseCB, 0);

    RRenderSetWindowTitle(rc, "CarTerrain example");
    RRenderCreateUserHelp(rc,help,2);
    RRenderToggleUserHelp(rc);

#ifndef PS2
    atexit(cleanup);
#endif

    /* pass control to the renderer loop */
    RRun(rc, Tick, 0);

    return 0;
}

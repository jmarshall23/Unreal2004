/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:32 $ - Revision: $Revision: 1.23.6.2 $

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

#include "BoxCar.h"

/* graphics render context */
RRender *rc;

/* Height offset for car at reset */
const MeReal resetHeight = (MeReal)(0.5);

/* graphics render context */
RRender *rc;

MeMatrix3 wheelOrientation = {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}};

extern MeReal pad_drive, pad_steer;
/*
  Initialize physics and graphics objects for a car.
*/

/* Initialize physics and graphics objects for a car */

void CarInit(Car * c, MdtWorld * w, const MeReal *grav, CarData * d)
{
    int k;
    /*
      Initialize physics objects.

      First, initialise chassis body mass, moment of inertia, positions,
      etc.
    */
    MeMatrix3 cI = {{0,0,0},{0,0,0},{0,0,0}};
    MdtBodyID chassisBody;

    /*
      Now, initialise the wheels, wheel joint and contact constraints;
      tabulate positions of Wheels in the Vehicle body (chassis) reference
      frame
    */
    const MeReal rear_height =
        d->rear_wheel_radius - d->chassis_height_off_ground;

    const MeReal front_height =
        d->front_wheel_radius - d->chassis_height_off_ground;

    MeReal pWhV[4][3];

    /* chassis inertia tensor */
    cI[0][0] = d->chassis_MoI_roll;
    cI[1][1] = d->chassis_MoI_pitch;
    cI[2][2] = d->chassis_MoI_yaw;

    /*
      back-left back-right nfront-left front-right
    */
    pWhV[0][0] = -d->wheelbase / 2;
    pWhV[0][1] = d->rear_track / 2;
    pWhV[0][2] = rear_height;

    pWhV[1][0] = -d->wheelbase / 2;
    pWhV[1][1] = -d->rear_track / 2;
    pWhV[1][2] = rear_height;

    pWhV[2][0] = d->wheelbase / 2;
    pWhV[2][1] = d->rear_track / 2;
    pWhV[2][2] = front_height;

    pWhV[3][0] = d->wheelbase / 2;
    pWhV[3][1] = -d->rear_track / 2;
    pWhV[3][2] = front_height;

    /* Set car parameter */
    c->params = d;

    /* Set world */
    c->world = w;

    /* Initialize steering angle; */
    c->steering_angle = 0;

    c->body[4] = MdtBodyCreate(c->world);
    chassisBody = c->body[4];

    /*
      Variables
    */

    /* Initialise inertia */
    MdtBodySetInertiaTensor(chassisBody, (void *)cI);
    MdtBodySetMass(chassisBody, d->chassis_mass);

    MdtBodySetPosition(chassisBody, d->chassis_CoM_forward_offset, 0,
        d->chassis_height_off_ground + d->chassis_CoM_upward_offset +
        resetHeight);

    for (k = 0; k < 4; k++)
    {
        MdtBodyID wheelBody;
        MeVector3 p;
        MeMatrix3 R;
        MeMatrix3 wI = {{0,0,0},{0,0,0},{0,0,0}};

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
          equi   Equilibrium position of wheel as a ratio between
          stops
          ztos   Zero to Semi-load travel of suspension

          zero_load_height    Unloaded height of wheel suspension
          top_stop_height     top stop suspension limit
          bottom_stop_height  bottom stop suspension limit
          Kp                  Spring constant  (proportional)
          Kd                  Damping constant (derivative)
        */

        MeReal load, height, travel, damp, equi, ztos, soft, tmp;

        /* back-left back-right front-left front-right */
        wI[0][0] = d->wheel_MoI_radial;
        wI[1][1] = d->wheel_MoI_axial;
        wI[2][2] = d->wheel_MoI_radial;

        c->body[k] = MdtBodyCreate(c->world);
        wheelBody = c->body[k];

        /*
          This must be the first function called on a physics body
          initialise inertia.
        */
        MdtBodySetInertiaTensor(wheelBody, (void *)wI);
        MdtBodySetMass(wheelBody, d->wheel_mass);

        /*
          Angular velocity damping ensures that the car will come to a halt
          if there is no user input.
        */
        MdtBodySetAngularVelocityDamping(wheelBody, (MeReal)(0.1));

        MdtBodySetPosition(wheelBody, pWhV[k][0], pWhV[k][1],
            pWhV[k][2] + resetHeight);
        MdtBodySetOrientation(wheelBody, (void *)wheelOrientation);

        /*
          Initialise wheel joint constraint.
        */

        c->wheel_joint[k] = MdtCarWheelCreate(c->world);
        MdtCarWheelSetBodies(c->wheel_joint[k],c->body[4], c->body[k]);
        MdtBodyGetPosition(wheelBody, p);
        MdtCarWheelSetPosition(c->wheel_joint[k], p[0], p[1], p[2]);

        MdtBodyGetOrientation(chassisBody, R);

        /* chassis right->left axis */
        haxis = R[1];
        /* chassis down->up axis */
        saxis = R[2];
#if 0
        MdtCarWheelSetHingeAxis(c->wheel_joint[k],
            haxis[0], haxis[1], haxis[2]);
        MdtCarWheelSetSteeringAxis(c->wheel_joint[k],
            saxis[0], saxis[1], saxis[2]);
#endif
        MdtCarWheelSetSteeringAndHingeAxes(c->wheel_joint[k],
            saxis[0], saxis[1], saxis[2],
            haxis[0], haxis[1], haxis[2]);

        if (k < 2)
        {
            /*
              We don't want steering on the back wheels so lock them.
            */
            MdtCarWheelSetSteeringLock(c->wheel_joint[k], 1);
        }

        c->wheel_ground_contact[k] = MdtContactCreate(c->world);

        /*
          Suspension setup (also see comments in header file)

          load   Load acting on each wheel at equilibrium
          height Height of wheel suspension strut at equilibrium load
          travel Travel of suspension between bottom stop and top stop
          soft   Softness of suspension stops
          damp   Damping ratio of suspension
          equi   Equilibrium position of wheel as a ratio between
          stops
          ztos   Zero to Semi-load travel of suspension

          zero_load_height    Unloaded height of wheel suspension
          top_stop_height     top stop suspension limit
          bottom_stop_height  bottom stop suspension limit
          Kp                  Spring constant  (proportional)
          Kd                  Damping constant (derivative)
        */

        tmp = d->chassis_CoM_forward_offset / d->wheelbase / 2;

        if (k < 2)
        {
            /* fixed rear wheel */

            load = ((MeReal)(0.25) - tmp) * d->chassis_mass;
            height = -d->chassis_height_off_ground
                + d->rear_wheel_radius;
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
            height = -d->chassis_height_off_ground
                + d->front_wheel_radius;
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
    }

    for (k = 0; k < 5; k++)
        MdtBodyEnable(c->body[k]);

    for (k = 0; k < 4; k++)
        MdtCarWheelEnable(c->wheel_joint[k]);
}

/*
  Create and connect up the graphics.
*/
void CarCreateGraphics(Car *c)
{
    CarData *d = c->params;
    const float blue[4] = { 0.5f, 0.3f, 0.9f, 1.0f };

    c->part[4] = RGraphicBoxCreate(rc, d->chassis_length, d->chassis_depth,
        d->chassis_width, blue, (MeMatrix4Ptr)c->GraphicsTMatrix[4]);
    c->part[0] = RGraphicCylinderCreate(rc,
        d->rear_wheel_radius, d->rear_wheel_width, blue, (MeMatrix4Ptr)c->GraphicsTMatrix[0]);
    c->part[1] = RGraphicCylinderCreate(rc,
        d->rear_wheel_radius, d->rear_wheel_width, blue, (MeMatrix4Ptr)c->GraphicsTMatrix[1]);
    c->part[2] = RGraphicCylinderCreate(rc,
        d->front_wheel_radius, d->front_wheel_width, blue, (MeMatrix4Ptr)c->GraphicsTMatrix[2]);
    c->part[3] = RGraphicCylinderCreate(rc,
        d->front_wheel_radius, d->front_wheel_width, blue, (MeMatrix4Ptr)c->GraphicsTMatrix[3]);
}

/*
  Update function to call at each timestep.
*/
void CarUpdate(Car * c)
{
    /*
      Get new steering angle and drive.
    */
    CarInputSteeringAndDrive(c);

    {
        /*
          Calculate Ackerman steering angles.
        */
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
            MeVector3 p;
            MdtContactID contact;
            MdtContactParamsID params;

            MdtCarWheelGetHingeAxis(c->wheel_joint[k], haxis);
            contact = c->wheel_ground_contact[k];

            /*
              Wheel/ground collision detection; a sphere of wheel_radius
              against a horizontal plane at zero height.
            */

            if (MdtContactIsEnabled(contact))
                MdtContactDisable(contact);

            MdtBodyGetPosition(c->body[k], p);

            {
                const MeReal vertical_height = p[2];
                const MeReal wheel_radius = (k < 2)
                    ? c->params->rear_wheel_radius
                    : c-> params->front_wheel_radius;

                if (vertical_height <= wheel_radius)
                {
                    /*
                      wheel colliding with ground
                    */
                    MeReal soft, slip;
                    MeVector3 normal;
                    MeVector3 a;
                    MeReal ma;
                    MeReal norma;

                    MdtContactSetBodies(contact, c->body[k], 0);
                    MdtContactEnable(contact);

                    MdtContactSetPosition(contact, p[0], p[1],
                        p[2] - wheel_radius);
                    MdtContactSetNormal(contact, 0, 0, 1);
                    MdtContactSetPenetration(contact,
                        -(p[2] - wheel_radius));

                    params = MdtContactGetParams(contact);

                    MdtContactParamsSetType(params, MdtContactTypeFriction2D);



                    /*
                      Create principal friction axis (normal x hinge_axis).
                    */
                    MdtContactGetNormal(contact, normal);

                    a[0] = normal[1] * haxis[2] - normal[2] * haxis[1];
                    a[1] = normal[2] * haxis[0] - normal[0] * haxis[2];
                    a[2] = normal[0] * haxis[1] - normal[1] * haxis[0];
                    ma = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
                    norma = MeRecipSqrt(ma);

                    a[0] *= norma;
                    a[1] *= norma;
                    a[2] *= norma;

                    MdtContactSetDirection(contact, a[0], a[1], a[2]);

                    /*
                      Set slip factor ... should be based on wheel
                      velocity!
                    */
                    if (k < 2)
                    {
                        /* rear wheel */

                        soft = c->params->rear_contact_softness;
                        slip = c->params->rear_contact_lateral_slip;
                    }
                    else
                    {
                        /* front wheel */

                        soft = c->params->front_contact_softness;
                        slip = c->params->front_contact_lateral_slip;
                    }


                    MdtContactParamsSetSecondarySlip(params, slip);
                    MdtContactParamsSetSoftness(params, soft);
                }
            }

            /*
              Wheel torque calculations.
            */
            if (k < 2)
            {
                /*
                  Rear wheel.
                */

                const MeReal p = (c->drive_torque > 0)
                    ? 1.0f * c->drive_torque
                    : 0.75f * c->drive_torque;

                MdtBodyAddTorque(c->body[k],
                    p * haxis[0], p * haxis[1], p * haxis[2]);
                MdtBodyAddTorque(c->body[4],
                    -p * haxis[0], -p * haxis[1], -p * haxis[2]);
            }
            else
            {
                /*
                  Front wheel do the steering proportional gap (radians).
                */
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

                MdtBodyEnable(c->body[k]);
                MdtCarWheelSetSteeringLimitedForceMotor(c->wheel_joint[k],
                    speed * desired_vel, forcemax);

                {
                    const MeReal p = (c->drive_torque > 0)
                        ? 0.75f * c->drive_torque
                        : c->drive_torque;
                    MdtBodyAddTorque(c->body[k],
                        p * haxis[0], p * haxis[1], p * haxis[2]);
                    MdtBodyAddTorque(c->body[4],
                        -p * haxis[0], -p * haxis[1], -p * haxis[2]);
                }
            }
        }
    }

    /*
      Ensure that the graphics have the correct orientation
    */

    /* ensure that the graphics have the correct orientation */
    CarWheelGraphicsTransform(c,CarGetTMatrix(c, 0),
        c->GraphicsTMatrix[0]);
    CarWheelGraphicsTransform(c,CarGetTMatrix(c, 1),
        c->GraphicsTMatrix[1]);
    CarWheelGraphicsTransform(c,CarGetTMatrix(c, 2),
        c->GraphicsTMatrix[2]);
    CarWheelGraphicsTransform(c,CarGetTMatrix(c, 3),
        c->GraphicsTMatrix[3]);
    CarChassisGraphicsTransform(c,CarGetTMatrix(c, 4),
        c->GraphicsTMatrix[4]);
}

void ReadMouseInput(MeReal *drive, MeReal *steer);

void CarInputSteeringAndDrive(Car * c)
{
    MeReal drive = 0;
    MeReal steer = 0;

    drive = pad_drive;
    steer = pad_steer;
/*    ReadMouseInput(&drive, &steer);*/

#if 0
    /* enable to drive around in a circle */
    drive=1;
    steer=1;
#endif

    c->steering_angle = steer * c->params->max_steering_angle;
    c->drive_torque = drive * 5;
    c->steering_angle = steer;
}

const MeReal *CarGetTMatrix(Car * c, int i)
{
    return (MeReal *)MdtBodyGetTransformPtr(c->body[i]);
}

void CarReset(Car *c, MeMatrix4 *TM)
{
}

void MEAPI Reset(RRender *rc, void *user_data)
{
}

/*
  Connect input and graphic output to the car dynamics.
*/

#define MOUSE_BUTTON_UP    0
#define MOUSE_BUTTON_DOWN  1

/*
  Variables for mouse input and keyboard commands.
*/

char leftButtonIsDown = MOUSE_BUTTON_UP;
int mouseDownX = 0, mouseDownY = 0;
int mouseX = 0, mouseY = 0;

/*
  This function was registered by RRenderSetMouseCallBack().
*/
void MEAPI MouseCallback(RRender *rc, int x, int y,
    int modifiers, RMouseButtonWhich which, RMouseButtonEvent event,
    void *userdata)
{
    if ((event == kRNewlyPressed || event == kRStillPressed) &&
        which == kRLeftButton)
    {
        mouseDownX = x;
        mouseDownY = y;
        leftButtonIsDown = MOUSE_BUTTON_DOWN;
    }
    else
        leftButtonIsDown = MOUSE_BUTTON_UP;

    mouseX = x;
    mouseY = y;
}

void ReadMouseInput(MeReal *drive, MeReal *steer)
{
    *drive = 0;
    *steer = 0;

    if (leftButtonIsDown)
    {
        MeReal drive_input = (MeReal) (mouseDownY - mouseY) / 200;
        MeReal steering_input = (MeReal) (mouseDownX - mouseX) / 100;

        if (drive_input > 1)
            drive_input = 1;

        if (drive_input < -1)
            drive_input = -1;

        *drive = drive_input;

        if (steering_input > 1)
            steering_input = 1;

        if (steering_input < -1)
            steering_input = -1;

        *steer = steering_input;
    }
}

int GraphicsInit(int argc, const char **argv)
{
    AcmeVector3 cam_pos = {0,0,5};

    MeCommandLineOptions* options;
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 0;

    /* register mouse callback */
    RRenderSetMouseCallBack(rc, MouseCallback, 0);

    RRenderSetUp2CallBack(rc, Reset, 0);

    RCameraSetLookAt(rc,cam_pos);
    RCameraUpdate(rc);
    return 1;
}


#define gridLines 20

void GroundGraphics()
{
    /*
      A 20*20 grid of lines helps show motion over the ground plane.
    */

    const float gridSpacing = 2;
    float color[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
    RGraphic *gridLineG[2 * gridLines];
    int i;

    for (i = 0; i < gridLines; ++i)
    {
        const AcmeReal offset = gridSpacing * (gridLines - 1) / 2.0f;

        AcmeReal originX[3];
        AcmeReal endX[3];
        originX[0] = -offset + i * gridSpacing;
        originX[1] = 0;
        originX[2] = -offset;
        endX[0] = -offset + i * gridSpacing;
        endX[1] = 0;
        endX[2] = offset;

        gridLineG[i * 2] = RGraphicLineCreate(rc, originX, endX, color, 0);

        {
            AcmeReal originZ[3];
            AcmeReal endZ[3];

            originZ[0] = -offset;
            originZ[1] = 0;
            originZ[2] = -offset + i * gridSpacing;
            endZ[0] = offset;
            endZ[1] = 0;
            endZ[2] = -offset + i * gridSpacing;

            gridLineG[i * 2 + 1] = RGraphicLineCreate(rc,
                originZ, endZ, color, 0);
        }
    }
}

void GraphicsDelete()
{
    RRenderContextDestroy(rc);
}

void CarChassisGraphicsTransform(Car * c, const MeReal *Me, MeReal *Gr)
{
    Gr[0] = Me[0];
    Gr[1] = Me[2];
    Gr[2] = Me[1];
    Gr[3] = 0;
    Gr[4] = Me[8];
    Gr[5] = Me[10];
    Gr[6] = Me[9];
    Gr[7] = 0;
    Gr[8] = Me[4];
    Gr[9] = Me[6];
    Gr[10] = Me[5];
    Gr[11] = 0;
    Gr[12] = Me[12] + Gr[0] * c->params->chassis_forward_offset;
    Gr[13] = Me[14] + Gr[1] * c->params->chassis_forward_offset;
    Gr[14] = Me[13] + Gr[2] * c->params->chassis_forward_offset;
    Gr[15] = 1;
}

void CarWheelGraphicsTransform(Car * c, const MeReal *Me, MeReal *Gr)
{
    Gr[0] = Me[0];
    Gr[1] = Me[2];
    Gr[2] = Me[1];
    Gr[3] = 0;
    Gr[4] = Me[4];
    Gr[5] = Me[6];
    Gr[6] = Me[5];
    Gr[7] = 0;
    Gr[8] = -Me[8];
    Gr[9] = -Me[10];
    Gr[10] = -Me[9];
    Gr[11] = 0;
    Gr[12] = Me[12];
    Gr[13] = Me[14];
    Gr[14] = Me[13];
    Gr[15] = 1;
}

/*
  This transform should work for renderware/surrender.
*/
void CarGraphicsTransform(Car * c, const MeReal *Me, MeReal *Gr)
{
    Gr[0] = Me[0];
    Gr[1] = Me[2];
    Gr[2] = -Me[1];
    Gr[3] = 0;
    Gr[4] = Me[8];
    Gr[5] = Me[10];
    Gr[6] = -Me[9];
    Gr[7] = 0;
    Gr[8] = -Me[4];
    Gr[9] = -Me[6];
    Gr[10] = Me[5];
    Gr[11] = 0;
    Gr[12] = Me[12];
    Gr[13] = Me[14];
    Gr[14] = -Me[13];
    Gr[15] = 1;
}

void CarCameraTrack(Car * c)
{
    AcmeVector3 cam_lookat;
    cam_lookat[0] = c->GraphicsTMatrix[4][12];
    cam_lookat[1] = c->GraphicsTMatrix[4][13];
    cam_lookat[2] = c->GraphicsTMatrix[4][14];
    RCameraSetLookAt(rc,cam_lookat);
    RCameraUpdate(rc);
}

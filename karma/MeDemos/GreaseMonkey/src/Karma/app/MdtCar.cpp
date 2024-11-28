/***********************************************************/
// This file implements all the mathengine dynamics
// - initializing and deleting the framework and world
// - car chassis, suspension, wheels, steering
/***********************************************************/

#include <math.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <MeAssert.h>
//#if USE_CX_COLLISION
extern "C" {

#include "Mst.h"
//#include "McdDtBridge.h"
//#include "McdBox.h"
//#include "McdSphere.h"
//#include "McdRwBSP.h"
}

extern MstBridgeID cdHandler;
extern MstMaterialID mat_lod1wheel, mat_lod2wheel, mat_chassis, mat_track;
extern McdSpaceID mcd_space;
extern McdModelID mcd_world;
extern McdFrameworkID mcdframe;

//static    McdModel *mcd_chassis, *mcd_wheel[4];

//#endif
#include "MdtCar.hpp"
#include "Mdt.h"
#include "MeMath.h"
#include "rwcore.h"
#include "rpworld.h"
#include "utils.hpp"

#if _DEBUG
#   define  _DEBUG_CDICT    0
#   define  _DEBUG_CONTACTS 0
#endif

extern unsigned int WheelGroundCB(MdtContact *contact, McdContact *cp, McdIntersectResult *c);
extern unsigned int SimpleWheelGroundCB(MdtContact *contact, McdContact *cp, McdIntersectResult *c);

const MeReal resetHeight = (MeReal)(0.7);//2.0f;//
extern MeReal frame_delta_t;

static MeALIGNDATA(MeMatrix4,identity_mtx,16) = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

/***********************************************************************************************
*
*   Utility function. Slightly different to Kea MakeUnitVector
*
************************************************************************************************/
MeReal _MakeUnitVector (MeReal v[3])
{
  MeReal k = 0;
  for (int j=0; j<3; j++) k += _Sqr(v[j]);
  if (k > 0) {
    k = MeRecipSqrt(k);
    for (int j=0; j<3; j++) v[j] *= k;
    return (1/k); // Original Length of vector
  }
  else {
    v[0] = 1;
    v[1] = 0;
    v[2] = 0;
    return(0.0f);
  }
}

/***********************************************************************************************
*
*   Interpolated exponantial function. Only because exp() didn't seem to work on PS2
*
************************************************************************************************/
#define NUM_E_VALS (9)
static MeReal e_vals[NUM_E_VALS] = {0.000045f,  0.0076f,    0.0183f,    0.0498f,    0.1353f,    0.3679f,    0.6065f,    0.8187f,    1.0f};
static MeReal expo[NUM_E_VALS] =    {-10.0f,    -5.0f,      -4.0f,      -3.0f,      -2.0f,      -1.0f,      -0.5f,      -0.2f,      0.0f};

static MeReal MeExp(MeReal v)
{
    int hi=NUM_E_VALS-1 , lo=0, p;
    MeReal ret;

    if(v > expo[NUM_E_VALS]) return e_vals[NUM_E_VALS];

    if(v < expo[0]) return e_vals[0];

    /* Find two values on either side of v  */
    do {
        p = lo + (hi - lo) / 2;
        if(v < expo[p])
            hi = p;
        else
            lo = p;
    } while((hi-lo) > 1);

    ret = FRACTION(expo[lo], v, expo[hi]);
    ret = INTERPOLATE(e_vals[lo], ret, e_vals[hi]);
    return(ret);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MdtSuspendedWheel::MdtSuspendedWheel( MdtWorld* sys, MdtBody *wheelBody, MdtBody *carBody,
                                      MeReal radius, bool setFixed) :
                                chassis_body(carBody), wheel_body(wheelBody),
                                wheel_radius(radius), isFixed(setFixed)
{
    req_steer_angle = act_steer_angle = 0;
    drive_torque = 0;
    rot_angle = 0;

    /* Construct a new CarWheelJoint between the chassis and wheel  */
    wheel_joint = MdtCarWheelCreate(sys);
    MdtCarWheelSetBodies(wheel_joint,carBody,wheelBody); //chassis has to be body 1
    MeVector3 p;
    MdtBodyGetPosition(wheelBody, p);
    MdtCarWheelSetPosition (wheel_joint,p[0],p[1],p[2]);
    MeMatrix3 R;
    MdtBodyGetOrientation(carBody, R);
    MeReal *haxis = R[1];               // chassis y axis
    MeReal *saxis = R[2];               // chassis z axis


	MdtCarWheelSetSteeringAndHingeAxes(wheel_joint, saxis[0], saxis[1], saxis[2],
		                               haxis[0], haxis[1], haxis[2]);


    if (isFixed) {

        /* Lock the rear wheels */
        MdtCarWheelSetSteeringLimitedForceMotor (wheel_joint, 0, 1e9);
        MdtCarWheelSetSteeringLock (wheel_joint, 1);

    } else { // !isFixed

    }


//    MdtCarWheelEnable(wheel_joint);

}

/***********************************************************************************************
*
*   Update steering and drive on a wheel
*
************************************************************************************************/
void MdtSuspendedWheel::Update(MdtWorld* sys, int fwd_gear) {

    MeVector3 haxis;
    MeVector3 p, p_chass, p_err, w_vel, c_vel, c_ang_vel;
    MeReal high_lim, low_lim, v_disp = 0.0f;

    /* First have a look at the wheel velocity and position rel to chassis and correct any gross errors!!   */
    /* This is a bug with the implemtation of the end stops and shouldn't be necessary in the future    */
    MdtBodyGetPosition(wheel_body, p);
    MdtBodyGetPosition(chassis_body, p_chass);
    p_err[0] = p[0] - p_chass[0];   p_err[1] = p[1] - p_chass[1];   p_err[2] = p[2] - p_chass[2];
    MdtConvertVector((MdtBody *)NULL, p_err, chassis_body, p_err); // Wheel position rel to chassis in chassis frame
    p_err[0] -= wheel_pos[0];p_err[1] -= wheel_pos[1];p_err[2] -= wheel_pos[2];
    high_lim = MdtCarWheelGetSuspensionHighLimit(wheel_joint);
    low_lim = MdtCarWheelGetSuspensionLowLimit(wheel_joint);

    /* For the moment, manually enforce suspension limits to stop kea from going wild   */
    if(p_err[2] > high_lim) v_disp = high_lim;
    if(p_err[2] < low_lim) v_disp = low_lim;
    if(MeFabs(p_err[0]) > 0.5 || MeFabs(p_err[1]) > 0.5) v_disp = high_lim; //trap any other problems

#if 1
    if(v_disp != 0.0f)
    {
        /* Wheel has exceeded vertical limits so reset pos and vel  */
        p_err[0] = wheel_pos[0]; p_err[1] = wheel_pos[1]; p_err[2] = v_disp + wheel_pos[2];

        MdtBodyGetLinearVelocity(wheel_body, w_vel); //test

        MdtBodyGetLinearVelocity(chassis_body, c_vel);
        MdtBodyGetAngularVelocity(chassis_body, c_ang_vel);
        MdtConvertVector((MdtBody *)NULL, c_ang_vel, chassis_body, c_ang_vel); // chassis ang vel in chassis frame
        _Cross(c_ang_vel, p_err, w_vel);
        MdtConvertVector(chassis_body, w_vel, (MdtBody *)NULL, w_vel); // Wheel velocity in world frame local to chassis
        w_vel[0] += c_vel[0];   w_vel[1] += c_vel[1];   w_vel[2] += c_vel[2];
        MdtBodySetLinearVelocity(wheel_body, w_vel[0], w_vel[1], w_vel[2]);

        MdtConvertVector(chassis_body, p_err, (MdtBody *)NULL, p_err); // Wheel position rel to chassis in chassis frame
        p_err[0] += p_chass[0]; p_err[1] += p_chass[1]; p_err[2] += p_chass[2];
        MdtBodySetPosition(wheel_body, p_err[0], p_err[1], p_err[2]);
    }
#endif

    /* Set Fast spin axis because potentially wheels at high rpm    */
    MdtCarWheelGetHingeAxis(wheel_joint, haxis);
    MdtBodySetFastSpinAxis(wheel_body, haxis[0], haxis[1], haxis[2]);

    // Control wheel motors depending on drive or braking
    if(isFixed) { // isFixed
        const MeReal p = drive_torque;
        const MeReal spd = LIMITS(0, 2.5f*p, 250);
        if(p >= 0.01)
        {
            if(fwd_gear)
                MdtCarWheelSetHingeLimitedForceMotor (wheel_joint, spd,p + 300);
            else
                MdtCarWheelSetHingeLimitedForceMotor (wheel_joint, -spd,p + 300);
        }
        else if(p < -0.5)
            MdtCarWheelSetHingeLimitedForceMotor(wheel_joint, 0, -1000.0f * p);
        else
            MdtCarWheelSetHingeLimitedForceMotor(wheel_joint, 0, 0);
    } else {  // isSteered
        /* A simple controller to steer the front wheels    */
        const MeReal pgap = 0.5f;  // proportional gap (radians)
        const MeReal speed = 22.0f;
        const MeReal forcemax = 1e12f;
        MeReal theta = -MdtCarWheelGetSteeringAngle (wheel_joint);
        act_steer_angle = theta;
        const MeReal p = drive_torque;
        if(p < -0.05)
            MdtCarWheelSetHingeLimitedForceMotor(wheel_joint, 0, -1000.0f * p);
        else
            MdtCarWheelSetHingeLimitedForceMotor(wheel_joint, 0, 0);
        MeReal desired_vel = req_steer_angle - theta;
        desired_vel = LIMITS(-pgap, desired_vel, pgap); //limit

        MdtCarWheelSetSteeringLimitedForceMotor(wheel_joint,speed*desired_vel,forcemax);

    }
}

/***********************************************************************************************
*   set the suspension load acting on the wheel at equilibrium,
*   the corresponding height (vertical height from the chassis frame to the wheel centre)
*   and the total suspension travel between stops.
************************************************************************************************/
void MdtSuspendedWheel::SetSuspensionEquilibriumLoadHeightAndTravel(
                const MeReal* grav,
                MeReal load, MeReal height, MeReal travel,
                MeReal damp, MeReal equi, MeReal ztos, MeReal soft)  {

    const MeReal grav_mag = MeSqrt(grav[0]*grav[0] + grav[1]*grav[1] + grav[2]*grav[2]);
    const MeReal zero_load_height = height - 0.5f*ztos*travel;
    const MeReal top_stop_height = height + (1-equi)*travel;
    const MeReal bottom_stop_height = height - equi*travel;

    const MeReal Kp = 2*load*grav_mag/(ztos*travel);
    const MeReal Kd = damp*MeSqrt(load*Kp);
    MdtCarWheelSetSuspension(wheel_joint,Kp,Kd,soft,
        bottom_stop_height,top_stop_height,zero_load_height);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtSuspendedWheel::SetSteeringAngle( MeReal angle) {
    req_steer_angle = angle;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtSuspendedWheel::SetDriveTorque( MeReal torque) {
    drive_torque = torque;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MdtCar::MdtCar():
mGear(1),
mFinalDriveRatio(35.00),
mRedline(7200),
tranny_type(2),
#if PS2
level_of_detail(LOD2)
#else
level_of_detail(LOD1)
#endif
{
    int i;

    shape_offsets = (MeMatrix4 *)MeMemoryAPI.createAligned(5*sizeof(MeMatrix4),16);
    mcd_mtx_mem = (MeMatrix4 *)MeMemoryAPI.createAligned(5*sizeof(MeMatrix4),16);

    memset((void*)shape_offsets, 0, 5*sizeof(MeMatrix4));

    for(i=0;i<5;i++)
    {
        shape_offsets[i][0][0] = shape_offsets[i][1][1] = shape_offsets[i][2][2] = shape_offsets[i][3][3] = 1.0f;
    }

    mGearRatio[0] = 4.00f; //reverse => !mGear
    mGearRatio[1] = 3.4f;
    mGearRatio[2] = 2.1f;
    mGearRatio[3] = 1.4f;
    mGearRatio[4]= 1.00f;

    initialised_already = 0;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MdtCar::~MdtCar()
{
    int i;
    for(i=0;i<4;i++) delete wheel_joint[i];
    MeMemoryAPI.destroyAligned(shape_offsets);
    MeMemoryAPI.destroyAligned(mcd_mtx_mem);
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::CreateMdtCar( MdtWorldID world )
{
    int i;

    for(i=0; i<5; i++)
    {
        body[i] = MdtBodyCreate(world); // shorthand
    }

}

/***********************************************************************************************
*
*   Initialise the car dynamics and collision
*
************************************************************************************************/
void MdtCar::InitMdtCar( MdtWorld* mdt_world, const MeReal* grav, CarData* car) //: sys(sys), car_data(car)
{
    int i,j;

    sys = mdt_world;
    memcpy(&car_data, car, sizeof(CarData));

    req_steer_angle = 0;
//  wheelbase = car->wheelbase;
//  front_track = car->front_track;
    drive_torque = 0;

#if PS2
    if(level_of_detail == NO_LOD) level_of_detail = LOD2;
#else
    if(level_of_detail == NO_LOD) level_of_detail = LOD1;
#endif

    // positions of the Wheels in the Vehicle body (chassis) frame
    const MeReal pWhV[4][3] = {
        car->wheelbase/2-car->chassis_X_pos-car->chassis_CoM_forward_offset, car->front_track/2, car->front_wheel_radius - car->chassis_height_off_ground-car->chassis_CoM_upward_offset,
        car->wheelbase/2-car->chassis_X_pos-car->chassis_CoM_forward_offset, -car->front_track/2, car->front_wheel_radius - car->chassis_height_off_ground-car->chassis_CoM_upward_offset,
        -car->wheelbase/2-car->chassis_X_pos-car->chassis_CoM_forward_offset, car->rear_track/2, car->rear_wheel_radius - car->chassis_height_off_ground-car->chassis_CoM_upward_offset,
        -car->wheelbase/2-car->chassis_X_pos-car->chassis_CoM_forward_offset, -car->rear_track/2, car->rear_wheel_radius - car->chassis_height_off_ground-car->chassis_CoM_upward_offset
    };

    /* Set up the chassis pyhsical body */
    MdtBodyID chassisBody = body[CHASSIS_CMPT];
    MdtBodyReset(chassisBody);

//  if(!initialised_already)
//  {

	MdtBodyEnable (chassisBody);

//  }
    MeMatrix3 cI = {{car->chassis_MoI_xx,0,0},{0,car->chassis_MoI_yy,0},{0,0,car->chassis_MoI_zz}};
    MdtBodySetMass(chassisBody,car->chassis_mass);
    MdtBodySetInertiaTensor(chassisBody,cI);

    MeVector3 chassis_pos = {car->chassis_X_pos + car->chassis_CoM_forward_offset,
                            0,
                            car->chassis_height_off_ground + car->chassis_CoM_upward_offset};
    MdtBodySetPosition(chassisBody,chassis_pos[0],chassis_pos[1],chassis_pos[2] + resetHeight);

    chassis_height = chassis_pos[2]; // store for in game resetting

    /* Set up shape offset relative to c.g. */

    for(i=0; i<4; i++)
    {
        for(j=0; j<3; j++)  shape_offsets[i][3][j] = pWhV[i][j]; //store for simple model
    }
    shape_offsets[CHASSIS_CMPT][3][0] = -car->chassis_CoM_forward_offset;
    shape_offsets[CHASSIS_CMPT][3][2] = -car->chassis_CoM_upward_offset;


    if(!initialised_already)
    {
        /* Create the chassis collision model & insert in to previously created collison space  */
        mcd_chassis = McdModelCreate( McdBoxCreate(mcdframe, car->chassis_coll_box[0],car->chassis_coll_box[1],car->chassis_coll_box[2]) );
//        mcd_chassis = McdModelCreate( McdBoxCreate(5,5,5));
        McdSpaceInsertModel(mcd_space, mcd_chassis);
        /* Associate the chassis physical boby with this collision model    */
        McdModelSetBody(mcd_chassis, chassisBody );
        /* Set the offset between the collision model an dphysical body */
        McdModelSetRelativeTransformPtrs(mcd_chassis, 
				                         shape_offsets[CHASSIS_CMPT],
				                         McdModelGetTransformPtr(mcd_chassis), 
				                         mcd_mtx_mem[CHASSIS_CMPT],
                                         0);
        /* Set material for this collision model    */
        McdModelSetMaterial( mcd_chassis, mat_chassis );

        //McdModelSetInteractionDisabled( mcd_world, mcd_chassis); //testing
    }

    /* Set up the wheel bodies and collision models and suspension joints*/
    for (i=0; i<4; i++) { // front-left front-right back-left back-right

        MdtBodyID wheelBody = body[i];
        MdtBodyReset(wheelBody);

//      if(!initialised_already)
//      {
            if(level_of_detail == LOD1)
                MdtBodyEnable (wheelBody);
//      }

        MeMatrix3 wI = {{car->wheel_MoI_xx,0,0},{0,car->wheel_MoI_yy,0},{0,0,car->wheel_MoI_zz}};
        MdtBodySetMass(wheelBody,car->wheel_mass);
        MdtBodySetInertiaTensor(wheelBody,wI);
        MdtBodySetPosition(wheelBody, pWhV[i][0]+chassis_pos[0], pWhV[i][1]+chassis_pos[1], pWhV[i][2]+chassis_pos[2] + resetHeight);

        if(!initialised_already)
        {
            /* Create a new car wheel joint */
            wheel_joint[i] = new MdtSuspendedWheel( sys, wheelBody, chassisBody,
                (i<2) ? car->front_wheel_radius : car->rear_wheel_radius,  // radius
                (i<2) ? false : true);                                     // isFixed

            if(level_of_detail == LOD1)
                MdtCarWheelEnable(wheel_joint[i]->GetJoint());
        }

        MeReal load, height, travel, damp, equi, ztos, soft;
        MeReal tmp = (car->chassis_X_pos + car->chassis_CoM_forward_offset)/car->wheelbase/2 + car->suspension_level_tweak;
        if (i<2) {
            load = ((MeReal)(0.25) + tmp)*car->chassis_mass;
            height = 0; //-car->chassis_height_off_ground + car->front_wheel_radius;
            travel = car->front_suspension_travel;
            damp = car->front_suspension_damp;
            equi = car->front_suspension_equi;
            ztos = car->front_suspension_ztos;
            soft = car->front_suspension_soft;
        } else {
            load = ((MeReal)(0.25) - tmp)*car->chassis_mass;
            height = 0; //-car->chassis_height_off_ground + car->rear_wheel_radius;
            travel = car->rear_suspension_travel;
            damp = car->rear_suspension_damp;
            equi = car->rear_suspension_equi;
            ztos = car->rear_suspension_ztos;
            soft = car->rear_suspension_soft;
        }
        /* Initialise the suspension joint stiffness, damping, travel etc.  */
        wheel_joint[i]->SetSuspensionEquilibriumLoadHeightAndTravel( grav, load, height, travel,
                                                                     damp, equi, ztos, soft);
        shape_offsets[i][3][2] -= travel*0.5f*ztos;

        if(!initialised_already)  //won't work if wheel radii are changed
        {
            /* Create the wheel collision model & insert into collision space   */
            mcd_wheel[i] = McdModelCreate( McdSphereCreate(mcdframe, i < 2 ? car->front_wheel_radius : car->rear_wheel_radius) );
		
            McdSpaceInsertModel(mcd_space, mcd_wheel[i]);
            /* Associate the physical body with the collision model */
            if(level_of_detail == LOD1)
            {
                McdModelSetBody(mcd_wheel[i], body[i] );
                /* Set the offset between physical body and collision model. Zero in this case  */
                McdModelSetRelativeTransformPtrs( mcd_wheel[i],
						identity_mtx,
						McdModelGetTransformPtr(mcd_wheel[i]), 
						mcd_mtx_mem[i],
                        0);
                /* Set the wheel collision material */
                McdModelSetMaterial( mcd_wheel[i], mat_lod1wheel );
            }
            else if(level_of_detail == LOD2)
            {
                McdModelSetBody(mcd_wheel[i], body[CHASSIS_CMPT] );
                /* Set the offset between physical body and collision model. Zero in this case  */
                McdModelSetRelativeTransformPtrs( mcd_wheel[i],
												shape_offsets[i],
												McdModelGetTransformPtr(mcd_wheel[i]), 
												mcd_mtx_mem[i], 
                                                0
                                                );
                /* Set the wheel collision material */
                McdModelSetMaterial( mcd_wheel[i], mat_lod2wheel );
            }

            /* We don't want collision between the wheels and the chassis   */
            McdSpaceDisablePair( mcd_wheel[i], mcd_chassis);

//            McdSpaceDisablePair( mcd_wheel[i], mcd_world);

            if(i>0) //turn off collisions between wheels on the same car
            {
                for(j = i-1; j>=0; j--)
                {
                    McdSpaceDisablePair( mcd_wheel[i], mcd_wheel[j] );
                }
            }
        }

        WHEEL_USERDATA *ud = wheel_joint[i]->GetUserData();
        MdtBodySetUserData(body[i], (void*)(wheel_joint[i])); //Needed in collision later
        McdModelSetUserData( mcd_wheel[i], (void *)(wheel_joint[i]) );
        ud->wheel_joint = wheel_joint[i]->GetJoint();
        ud->car_data = &car_data;
        ud->contact = 0;
        ud->on_track = 0;

        /* Store Equilibrium position of wheels relatice to chassis */
        wheel_joint[i]->SetRelWheelPos(&pWhV[i][0]);
    }

    initialised_already = 1;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::Update() {
int i;

    // do ackerman steering
    const MeReal tana = MeTan(req_steer_angle);
    MeReal left_angle = MeAtan2(tana, 1 - 0.5f*tana*car_data.front_track/car_data.wheelbase);
    wheel_joint[0]->SetSteeringAngle( left_angle);
    MeReal right_angle = MeAtan2(tana, 1 + 0.5f*tana*car_data.front_track/car_data.wheelbase);
    wheel_joint[1]->SetSteeringAngle( right_angle);

if(level_of_detail == LOD1)
{
    /* Check and reset angular velocity of chassis to reduce excess energy. Shouldn't be
        required in the release version */
#if 1
    MeVector3 ang_vel;
    MeReal a_v_mag, max_a_v;
    MdtBodyGetAngularVelocity(body[CHASSIS_CMPT], ang_vel);
    a_v_mag = _MakeUnitVector(ang_vel);
    max_a_v = 0.15f*ME_PI/frame_delta_t;
    if(a_v_mag > max_a_v)
    {
        ang_vel[0] *= max_a_v;
        ang_vel[1] *= max_a_v;
        ang_vel[2] *= max_a_v;
        MdtBodySetAngularVelocity(body[CHASSIS_CMPT], ang_vel[0],ang_vel[1],ang_vel[2]);
    }
#endif

    /* Update each wheel setting any new steering / drive inputs    */
    for (i=0; i<4; i++) wheel_joint[i]->Update(sys, mGear);

    /* Get the average rpm of the back wheels to calculate a rough engine rpm   */
    MdtBody *wheel_body1 = body[REAR_L_CMPT];
    MdtBody *wheel_body2 = body[REAR_R_CMPT];
    MeVector3 avW1, avW2;
    MdtBodyGetAngularVelocity(wheel_body1,avW1);
    MdtBodyGetAngularVelocity(wheel_body2,avW2);
    mRPM = 0.5f * (MeSqrt(_Dot(avW1, avW1)) + MeSqrt(_Dot(avW2, avW2)))* mGearRatio[mGear] * mFinalDriveRatio;

    /* Some sound cheats here   */
    mRPM = MAX(mRPM, 1200);
}
else if(level_of_detail == LOD2)
{

    WHEEL_USERDATA *wheel_ud;
    MeReal force, rad, v_mag;
    MeVector3 pos, dir, vel, x_axis;
    MeMatrix4 *me_mtx;

    for(i=0;i<=REAR_R_CMPT;i++)
    {
        if(i < REAR_L_CMPT) //front wheels steer
        {
            MeReal diff = wheel_joint[i]->req_steer_angle - wheel_joint[i]->act_steer_angle;
            MeReal max_ang = 22.0f*frame_delta_t;
            diff = LIMITS(-max_ang, diff, max_ang);
            wheel_joint[i]->act_steer_angle += diff;
            rad = car_data.front_wheel_radius;
        } else {
            rad = car_data.rear_wheel_radius;
        }


        wheel_ud = (WHEEL_USERDATA *)(wheel_joint[i]->GetUserData());
        if(wheel_ud->contact)
        {
            MdtContactGetPosition(wheel_ud->contact, pos);
            MdtBodyGetVelocityAtPoint(body[CHASSIS_CMPT], pos, vel);
            me_mtx =(MeMatrix4 *)GetTMatrix(CHASSIS_CMPT);
            x_axis[0] = (*me_mtx)[0][0];x_axis[1] = (*me_mtx)[0][1];x_axis[2] = (*me_mtx)[0][2];
            v_mag = MeVector3Dot(vel, x_axis);
            wheel_joint[i]->ang_vel = v_mag/rad;

            if(drive_torque < -0.5)
            {
                TYRE_PARAMS *tp;
                MdtContactParamsID cprms = MdtContactGetParams(wheel_ud->contact);

                if(wheel_ud->on_track)
                    tp = &car_data.tyre_track_props;
                else
                    tp = &car_data.tyre_grass_props;

                /* Primary direction value is left as maximum   */
                MdtContactParamsSetPrimaryFriction(cprms, -1000.0f*drive_torque/rad);
                //MdtContactParamsSetPrimarySlip(cprms, 0.0f);
            } else if(drive_torque >= -0.01f)
            {
                if(i >= REAR_L_CMPT)
                {
                    force = drive_torque*1.0f/rad;
                    MdtContactGetDirection(wheel_ud->contact, dir);
                    MdtBodyAddForceAtPosition(body[CHASSIS_CMPT], force*dir[0], force*dir[1], force*dir[2], pos[0], pos[1],pos[2]);
                }
            }
        }

        wheel_joint[i]->rot_angle += frame_delta_t*wheel_joint[i]->ang_vel;
//        if(wheel_joint[i]->rot_angle > ME_PI) wheel_joint[i]->rot_angle -= 2*ME_PI;
//        if(wheel_joint[i]->rot_angle < -ME_PI) wheel_joint[i]->rot_angle += 2*ME_PI;
        wheel_joint[i]->ang_vel *= 0.99f; // decay angular vel when off ground
    }

    mRPM = 0.5f * (wheel_joint[REAR_L_CMPT]->ang_vel + wheel_joint[REAR_R_CMPT]->ang_vel)* mGearRatio[mGear] * mFinalDriveRatio;

    /* Some sound cheats here   */
    mRPM = MAX(mRPM, 1200);

}

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
const MeReal MdtCar::GetSpeed() {
    MeMatrix3 cR;
    MdtBody *chassis_body = body[4];
    MdtBodyGetOrientation(chassis_body, cR);
    const MeReal *xChW = cR[0];         // x axis of Chassis in World
    MeVector3 vChW;
    MdtBodyGetLinearVelocity(chassis_body,vChW);
    return vChW[0]*xChW[0] + vChW[1]*xChW[1] + vChW[2]*xChW[2];
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
const MeReal MdtCar::GetWheelSpeed() {
    if(level_of_detail == LOD1) {
        MdtBody *wheel_body = body[0];
        MeVector3 avW;
        MdtBodyGetAngularVelocity(wheel_body,avW);
        return MeSqrt(_Dot(avW, avW));
    } else if(level_of_detail == LOD2) {
        return(wheel_joint[0]->ang_vel);
    }
    return 0;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
const MeReal MdtCar::GetMotorRPM() {
    return mRPM;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::SetSteeringAngle(MeReal steer) {
    MeVector3 v;
    MeReal v_mag_s, s_fac=1.0f;
    MdtBodyGetLinearVelocity(body[4],v);
    v_mag_s = v[0]*v[0]+v[1]*v[1];
    if(v_mag_s > 1000.0f && mGear) s_fac = 0.4f + 0.6f*MeExp(-(v_mag_s-1000.0f)/100.0f);
    req_steer_angle = steer*car_data.max_steering_angle*s_fac;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::SetDrive(MeReal drive) {
    int i;
    MeReal t;
    MeReal up_t1[3] = {2500, 2500, 2000};
    MeReal dn_t1[3] = {1500, 1500, 1300};
    MeReal up_t2[3] = {7300, 7200, 6500};
    MeReal dn_t2[3] = {3000, 3000, 4000};

    if(drive < 0)
        t = drive * car_data.brake_multiplier; //braking
    else
    {
     switch(tranny_type)
     {
     case 0:    // manual, do nothing.
         break;
     case 1:     // a.i. auto tranny, select gear based on RPM and drive input
     {
         switch(mGear)
         {
         case 1:
             if(drive > 0.4)
             {
                if(mRPM > up_t1[0])
                    Upshift();
             }
             break;
         case 2:
              if(drive > 0.4)
             {
                if(mRPM > up_t1[1])
                    Upshift();
                else if (mRPM < dn_t1[0])
                    Downshift();
             }
            else if(drive < 0.2 )
            {
                if(mRPM < dn_t1[0])
                    Downshift();
            }
             break;
         case 3:
              if(drive > 0.4 )
             {
                if(mRPM > up_t1[2])
                    Upshift();
                if(mRPM < dn_t1[1])
                    Downshift();
             }
            else if(drive < 0.15)
            {
                if(mRPM < dn_t1[1])
                    Downshift();
            }
             break;
          case 4:
            if((drive < 0.15 || drive > 0.4) && mRPM < dn_t1[2])
            {
                Downshift();
            }
             break;

         }
     }
         break;
         case 2:    //non-ai automatic
            {

             switch(mGear)
             {
             case 1:
                 if(drive > 0.5)
                 {
                    if(mRPM > up_t2[0])
                        Upshift();
                }
                 break;
            case 2:
                if(drive > 0.5)
                {
                    if(mRPM > up_t2[1])
                        Upshift();
                    else if (mRPM < dn_t2[0])
                        Downshift();
                }
                else if(drive < 0.2 )
                {
                    if(mRPM < dn_t2[0])
                        Downshift();
                }
                 break;
            case 3:
                if(drive > 0.5 )
                {
                if(mRPM > up_t2[2])
                        Upshift();
                    if(mRPM < dn_t2[1])
                        Downshift();
                }
                else if(drive < 0.2)
                {
                    if(mRPM < dn_t2[1])
                        Downshift();
                }
                break;
            case 4:
                if((drive < 0.2 || drive > 0.5) && mRPM < dn_t2[2])
                {
                    Downshift();
                }
                break;
             }
             break;
         }
     }
        t = car_data.torque_multiplier * drive
                    * mGearRatio[mGear] * mFinalDriveRatio
//      * MeExp(-(mRPM - 0.45f * mRedline) * (mRPM - 0.45f * mRedline)/(mRedline * mRedline/6.0f));
        * MeExp(-(mRPM - 0.65f * mRedline) * (mRPM - 0.65f * mRedline)/(mRedline * mRedline/6.0f));  // was 6.0f
    }

    drive_torque = t;
    for (i=0;i<4;i++) wheel_joint[i]->SetDriveTorque(t);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::Reset() {
    int i;
    MeVector3 chp;

    req_steer_angle = 0;
    drive_torque = 0;

    /* Wheel positions in chassis frame of reference    */
    const MeReal pWhV[4][3] = {
        car_data.wheelbase/2-car_data.chassis_X_pos-car_data.chassis_CoM_forward_offset, car_data.front_track/2, car_data.front_wheel_radius - car_data.chassis_height_off_ground-car_data.chassis_CoM_upward_offset,
        car_data.wheelbase/2-car_data.chassis_X_pos-car_data.chassis_CoM_forward_offset, -car_data.front_track/2, car_data.front_wheel_radius - car_data.chassis_height_off_ground-car_data.chassis_CoM_upward_offset,
        -car_data.wheelbase/2-car_data.chassis_X_pos-car_data.chassis_CoM_forward_offset, car_data.rear_track/2, car_data.rear_wheel_radius - car_data.chassis_height_off_ground-car_data.chassis_CoM_upward_offset,
        -car_data.wheelbase/2-car_data.chassis_X_pos-car_data.chassis_CoM_forward_offset, -car_data.rear_track/2, car_data.rear_wheel_radius - car_data.chassis_height_off_ground-car_data.chassis_CoM_upward_offset
    };

    MdtBodyGetPosition(body[CHASSIS_CMPT], chp);

    for(i=0;i<5;i++)
    {
        MdtBodySetLinearVelocity(body[i], 0,0,0);
        MdtBodySetAngularVelocity(body[i],0,0,0);
        MdtBodySetQuaternion(body[i],1,0,0,0);
        if(i < CHASSIS_CMPT)
        {
            MdtBodySetPosition(body[i], chp[0] + pWhV[i][0], chp[1]+pWhV[i][1], chp[2]+pWhV[i][2]);
        }
    }

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
const MeMatrix4* MdtCar::GetTMatrix(int i) {
    return (const MeMatrix4*)MdtBodyGetTransformPtr(body[i]);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetBodyVelCmpt(int b, int i)
{
    MeVector3 p,v,offset_w;

    if(level_of_detail == LOD1 || b == CHASSIS_CMPT)
    {
        MdtBodyGetLinearVelocity(body[b], v);
    }
    else if(level_of_detail == LOD2)
    {
        MdtBodyGetPosition(body[CHASSIS_CMPT], p);
        MdtConvertVector(body[CHASSIS_CMPT], &shape_offsets[b][3][0], (MdtBody *)NULL, offset_w);
        MeVector3Add(p, offset_w, p);
        MdtBodyGetVelocityAtPoint(body[CHASSIS_CMPT],p,v);
    }
    return(v[i]);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetBodyAngVelCmpt(int b, int i)
{
    MeVector3 v;
    MdtBodyGetAngularVelocity(body[b], v);
    return(v[i]);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetBodyPosCmpt(int b, int i)
{
    MeVector3 v, offset_w;

    if(level_of_detail == LOD1 || b == CHASSIS_CMPT)
    {
        MdtBodyGetPosition(body[b], v);
        offset_w[0] = offset_w[1] = offset_w[2] = 0;
    }
    else if(level_of_detail == LOD2)
    {
        MdtBodyGetPosition(body[CHASSIS_CMPT], v);
        MdtConvertVector(body[CHASSIS_CMPT], &shape_offsets[b][3][0], (MdtBody *)NULL, offset_w);
    }

    return(v[i]+offset_w[i]);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetBodySlipAng()
{
    MeVector3 v;

    MdtBodyGetLinearVelocity(body[4], v);
    MdtConvertVector((MdtBody *)NULL, v, body[4], v);

    return(MeAtan2(v[1],v[0]));
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetBodySlipVel()
{
    MeVector3 v;

    MdtBodyGetLinearVelocity(body[4], v);
    MdtConvertVector((MdtBody *)NULL, v, body[4], v);

    return(v[1]);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetBodyForwardVel()
{
    MeVector3 v;

    MdtBodyGetLinearVelocity(body[4], v);
    MdtConvertVector((MdtBody *)NULL, v, body[4], v);

    return(v[0]);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetWheelSlipAng(int i)
{
    MeVector3 normal, vel, contact_pos;
    MdtContact *contact = (MdtContact *)NULL;

    MeReal n_cmpt;

    if(i>REAR_R_CMPT) return 0.0f;

    contact = wheel_joint[i]->GetUserData()->contact;

    if(!contact) return 0.0f; //wheel not in contact the ground

    /* Get in Plane cmpt of velocity    */
    MdtContactGetNormal(contact,normal);
    if(level_of_detail == LOD1)
        MdtBodyGetLinearVelocity(body[i], vel);
    else if(level_of_detail == LOD2)
    {
        MdtContactGetPosition(contact, contact_pos);
        MdtBodyGetVelocityAtPoint(body[CHASSIS_CMPT], contact_pos, vel);
    }
    n_cmpt = _Dot(normal, vel);
    vel[0] = vel[0] - normal[0]*n_cmpt; // Subtract out of plane cmpt to get in plane cmpt
    vel[1] = vel[1] - normal[1]*n_cmpt;
    vel[2] = vel[2] - normal[2]*n_cmpt;

    /* Convert this velocity into chassis space */

    MdtConvertVector((MdtBody *)NULL, vel, body[CHASSIS_CMPT], vel);


    return (MeAtan2(vel[1],vel[0]));
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetWheelSlipVel(int i)
{
    MeVector3 normal, vel;
    MdtContact *contact = (MdtContact *)NULL;
    MeReal n_cmpt;

    if(i>3) return 0.0f;

    contact = wheel_joint[i]->GetUserData()->contact;

    if(!contact) return 0.0f; //wheel not in contact the ground

    /* Get in Plane cmpt of velocity    */
    MdtContactGetNormal(contact,normal);
    MdtBodyGetLinearVelocity(body[i], vel);
    n_cmpt = _Dot(normal, vel);
    vel[0] = vel[0] - normal[0]*n_cmpt; // Subtract out of plane cmpt to get in plane cmpt
    vel[1] = vel[1] - normal[1]*n_cmpt;
    vel[2] = vel[2] - normal[2]*n_cmpt;

    /* Convert this velocity into chassis space */
    MdtConvertVector((MdtBody *)NULL, vel, body[4], vel);

    return (vel[1]);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::CalcShapeOffset(int i, MeVector3 v)
{
//  MeReal *mtx = MdtBodyGetTransformPtr(body[i]);

    MdtConvertVector(body[i], *shape_offsets[4*i+3], (MdtBody *)NULL, v);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::SetPosition(MeReal x, MeReal y)
{
    MeReal dx,dy;
    int i;
    MeVector3 v;

    MdtBodyGetPosition(body[CHASSIS_CMPT], v);

    dx = x - v[0];
    dy = -y - v[1];

    for(i=0;i<5;i++)
    {
        MdtBodyGetPosition(body[i], v);
        MdtBodySetPosition(body[i], v[0]+dx, v[1]+dy, v[2]);
    }

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::SetPosition(MeReal x, MeReal y, MeReal z)
{
    MeReal dx,dy,dz;
    MeVector3 v;

    int i;

    MdtBodyGetPosition(body[CHASSIS_CMPT], v);

    dx = x - v[0];
    dy = y - v[2];
    dz = -z - v[1];

    for(i=0;i<5;i++)
    {

        MdtBodyGetPosition(body[i], v);
        MdtBodySetPosition(body[i], v[0]+dx, v[1]+dz, v[2]+dy);

    }

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::SetOrientation(MeVector4 q)
{
    int i;
    MeVector3 c_pos, n_pos;

    /*  First set chassis orientation   */
    MdtBodySetQuaternion(body[CHASSIS_CMPT], q[0],q[1],q[2],q[3]);
    MdtBodyGetPosition(body[CHASSIS_CMPT], c_pos);

    for(i=0;i<4;i++)
    {
        MdtBodySetQuaternion(body[i], q[0],q[1],q[2],q[3]);
        MdtConvertVector(body[4], wheel_joint[i]->GetWheelPos(), (MdtBody *)NULL, n_pos);
        n_pos[0] += c_pos[0];   n_pos[1] += c_pos[1];   n_pos[2] += c_pos[2];
        MdtBodySetPosition(body[i], n_pos[0], n_pos[1], n_pos[2]);

    }

}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::GetOrientation(MeVector4 q)
{
    MdtBodyGetQuaternion(body[CHASSIS_CMPT], q);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::PreEvolve()
{
    int i;
    WHEEL_USERDATA *ud;

    for (i=0;i<4;i++)
    {
        ud = wheel_joint[i]->GetUserData();
        ud->contact=(MdtContact *)NULL;
        ud->on_track = 0;
    }

}
/***********************************************************************************************
*
*
*
************************************************************************************************/
int MdtCar::OnGround()
{
    for(int i=0;i<4;i++)
    {
        if(wheel_joint[i]->GetUserData()->contact) return 1;
    }

    return 0;

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::Upshift()
{
    if(mGear == 4)
        return;
    else
        mGear++;
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::Downshift()
{
    if(mGear == 0)
        return;
    else
        mGear--;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::Shift(int gear)
{
    if((gear < -1 ) || (gear > 3))
        return;
    else
        mGear = gear;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::SetTrannyType(int type)
{
    if(type < 0 || type > 2)
        return;
    else
        tranny_type = type;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal MdtCar::GetWheelSteerAngle(int wheel)
{
    MeReal theta = -MdtCarWheelGetSteeringAngle (wheel_joint[wheel]->GetJoint());
    wheel_joint[wheel]->act_steer_angle = theta;
    theta /= car_data.max_steering_angle;

    return theta;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MdtCar::SetLevelOfDetail(LEVEL_OF_DETAIL new_lod)
{
    int i;
    MeVector3 *steer_axis, *x_axis, ang_vel, chassis_pos, wheel_pos, wheel_vel, offset_w;
    MeMatrix4 *chassis_mtx, *wheel_mtx;
    LEVEL_OF_DETAIL old_lod = level_of_detail;
    MeReal av_mag;
    MeVector4 ch_quat;

    const MdtBodyID chassisBody = body[CHASSIS_CMPT];

    level_of_detail = new_lod;

    if(old_lod == new_lod) return;

    chassis_mtx = (MeMatrix4 *)GetTMatrix(CHASSIS_CMPT);
    steer_axis = (MeVector3 *)&(*chassis_mtx)[2];               // chassis z axis
    x_axis = (MeVector3 *)&(*chassis_mtx)[0];               // chassis x axis

    /* Switch to full model */
    if(new_lod == LOD1)
    {
        MeVector3 *hinge_axis;
        MdtBodyGetPosition(chassisBody, chassis_pos);

        for(i=0;i<CHASSIS_CMPT;i++)
        {
            const MdtBodyID wheelBody = body[i];
            MdtSuspendedWheel *const wheelJoint = wheel_joint[i];
            const McdModelID wheelModel = mcd_wheel[i];

            /* Reposition The wheels and joints */
            MdtConvertVector(chassisBody, &shape_offsets[i][3][0], NULL, offset_w);
            MeVector3Add(wheel_pos, chassis_pos, offset_w);
            MdtBodySetPosition(wheelBody, wheel_pos[0],wheel_pos[1],wheel_pos[2]);

            /* Set the wheel and joint orientations     */
            hinge_axis = (MeVector3 *)&(*chassis_mtx)[1];               // chassis y axis
            MdtBodyGetQuaternion(chassisBody, ch_quat);
            MeQuaternionFiniteRotation(ch_quat, *hinge_axis, wheelJoint->rot_angle);
            MeQuaternionFiniteRotation(ch_quat, *steer_axis, wheelJoint->act_steer_angle);
            MdtBodySetQuaternion(wheelBody,ch_quat[0],ch_quat[1],ch_quat[2],ch_quat[3]);

            wheel_mtx = (MeMatrix4 *)GetTMatrix(i);
            hinge_axis = (MeVector3 *)&(*wheel_mtx)[1];               // wheel y axis
            MdtBodySetFastSpinAxis(wheelBody, (*hinge_axis)[0], (*hinge_axis)[1], (*hinge_axis)[2]);

            /* Set wheel velocities */
            MdtBodyGetVelocityAtPoint(chassisBody, wheel_pos, wheel_vel);
            MdtBodySetLinearVelocity(wheelBody, wheel_vel[0],wheel_vel[1],wheel_vel[2]);

            /* Set wheel angular velocities */
            MdtBodyGetAngularVelocity(chassisBody, ang_vel);
            av_mag = wheelJoint->ang_vel;
            MdtBodySetAngularVelocity(wheelBody, av_mag*(*hinge_axis)[0] + ang_vel[0], av_mag*(*hinge_axis)[1] + ang_vel[1], av_mag*(*hinge_axis)[2] + ang_vel[2]);

            /* Enable wheel body and suspension joint   */
            MdtBodyEnable(wheelBody);
            MdtCarWheelEnable(wheelJoint->GetJoint());

#if _DEBUG_CDICT
            {
                MeDict *const d = &chassisBody->constraintDict;
                MeDictNode *n;

                MeDebug(3,"LOD2: constraintDict 0x%08x\n",(long) d);

                for (n = MeDictFirst(d); n != 0; n = MeDictNext(d,n))
                    MeDebug(3,"LOD2:   node 0x%08x, constraint 0x%08x\n",
                        (long) n,(long) MeDictNodeGet(n));
            }
#endif

            /* Attach all wheel contacts to the wheel from the chassis  */
            MeDictNode *next, *c_list = MeDictFirst(&chassisBody->constraintDict);
            int iterations = 0;
            while(c_list)
            {
                /* Store next link  */
                MdtConstraintID constraint = (MdtConstraintID)MeDictNodeGet(c_list);
                next = MeDictNext(&chassisBody->constraintDict,c_list);

#if _DEBUG_CDICT
                MeDebug(1,"LOD1: %02d: c_list 0x%08x, constraint 0x%08x next 0x%08x\n",
                    iterations, (long) c_list,(long) constraint,(long) next);
#endif

                /* if this constraint is a contact... */
                if(constraint->head.tag == MdtBclCONTACT || constraint->head.tag == MdtBclCONTACTGROUP)
                {
                    MEASSERT(constraint->head.tag==MdtBclCONTACTGROUP);

                    MdtContactGroupID group = MdtConstraintDCastContactGroup(constraint);
                    McdModelID mcd_model = (McdModelID)MdtConstraintGetUserData(constraint);

                    /* If group is on this wheels mcd model   */
                    if(mcd_model == wheelModel)
                    {
                        MdtBodyID body1 = MdtContactGroupGetBody(group, 0);
                        MdtBodyID body2 = MdtContactGroupGetBody(group, 1);

#if _DEBUG_CONTACTS
                    MeDebug(3,"LOD2: %02d: body[i=%d] 0x%08x"
                        ", chassisBody 0x%08x, body1 0x%08x, body2 0x%08x\n",
                        iterations,i,(long) wheelBody,
                        (long) chassisBody,(long) body1,(long) body2);

                    if (body1 != chassisBody && body2 != chassisBody)
                        MeFatalError(0,"LOD2: %02d: contact moved to"
                            "wheel is not on chassis\n", i);
#endif

                        /* Swop body1 & body2 if body2 is the chassis   */
                        if(body2 == chassisBody)
                            body2 = body1;

                        /* Reset group, chassis with wheel */
                        if(body2 != wheelBody)
                            MdtContactGroupDestroy(group);
                        else
                        {
#if _DEBUG_CONTACTS
                            MeDebug(1,"LOD1: %02d: group 0x%08x"
                                ", body2 0x%08x, body[i=%d] 0x%08x\n",
                                iterations,(long) group,(long) body2,
                                i,(long) wheelBody);
#endif

                            MdtContactGroupDisable(group);
                            MdtContactGroupSetBodies(group, wheelBody, body2);
                            MdtContactGroupEnable(group);
                        }
                    }
                }

                /* Set c_list to previously stored next link    */
                c_list = next;
                iterations++;

                if (iterations > 20)
                    MeFatalError(0,"%s\n","LOD1 looping > 20 times");
            }

            /* Attach wheel collision model back to wheel   */
            McdModelSetBody(wheelModel, wheelBody );
            /* Set the offset between physical body and collision model. Zero in this case  */
            McdModelSetRelativeTransformPtrs( wheelModel, 
				identity_mtx, 
				McdModelGetTransformPtr(wheelModel),
				mcd_mtx_mem[i],
                0);
            McdModelSetMaterial(wheelModel, mat_lod1wheel);
        }
    }
    /* Switch to simplemodel    */
    else if(new_lod == LOD2)
    {
        MeVector3 hinge_axis,temp, *body_y_axis, *body_z_axis;
		MeMatrix4 *body_mtx;

        for(i=0;i<CHASSIS_CMPT;i++)
        {
            const MdtBodyID wheelBody = body[i];
            MdtSuspendedWheel *const wheelJoint = wheel_joint[i];
            const McdModelID wheelModel = mcd_wheel[i];

            /* Disable wheel body and suspension joint  */
            MdtCarWheelDisable(wheelJoint->GetJoint());
            MdtBodyDisable(wheelBody);

#if _DEBUG_CDICT
            {
                MeDict *const d = &wheelBody->constraintDict;
                MeDictNode *n;

                MeDebug(3,"LOD2: constraintDict 0x%08x\n",(long) d);

                for (n = MeDictFirst(d); n != 0; n = MeDictNext(d,n))
                    MeDebug(3,"LOD2:   node 0x%08x, constraint 0x%08x\n",
                        (long) n,(long) MeDictNodeGet(n));
            }
#endif

            /* Attach all the contacts to the chassis instead of the wheel  */
            MeDictNode *next, *c_list = MeDictFirst(&wheelBody->constraintDict);
            int iterations = 0;
            while(c_list)
            {
                /* Store next link  */
                next = MeDictNext(&wheelBody->constraintDict,c_list);
                MdtConstraintID constraint = (MdtConstraintID)MeDictNodeGet(c_list);

#if _DEBUG_CDICT
                MeDebug(1,"LOD2: %02d: c_list 0x%08x, constraint 0x%08x, next 0x%08x\n",
                    iterations,(long) c_list,(long) constraint, (long) next);
#endif

                /* if this constraint is a group... */
                if(constraint->head.tag == MdtBclCONTACT || constraint->head.tag == MdtBclCONTACTGROUP)
                {
                    MEASSERT(constraint->head.tag==MdtBclCONTACTGROUP);

                    MdtContactGroupID group = MdtConstraintDCastContactGroup(constraint);
                    MdtBodyID body1 = MdtContactGroupGetBody(group, 0);
                    MdtBodyID body2 = MdtContactGroupGetBody(group, 1);

#if _DEBUG_CONTACTS
                    MeDebug(3,"LOD2: %02d: body[i=%d] 0x%08x"
                        ", chassisBody 0x%08x, body1 0x%08x, body2 0x%08x\n",
                        iterations,i,(long) wheelBody,
                        (long) chassisBody,(long) body1,(long) body2);

                    if (body1 != wheelBody && body2 != wheelBody)
                        MeFatalError(0,"LOD2: %02d: contact moved to"
                            "chassis is not on wheel\n", i);
#endif

                    /* Swop body1 & body2 if body2 is the wheel */
                    if(body2 == wheelBody)
                        body2 = body1;

#if _DEBUG_CONTACTS
                    MeDebug(3,"LOD2: %02d: body[i=%d] 0x%08x"
                        ", body1 0x%08x, body2 0x%08x\n",
                        iterations,i,(long) wheelBody,
                        (long) body1,(long) body2);
#endif

                    /* Reset group, replacing wheel with chassis  */
                    if(body2 == chassisBody)
                        MdtContactGroupDestroy(group);
                    else
                    {
#if _DEBUG_CONTACTS
                        MeDebug(1,"LOD2: %02d: group 0x%08x"
                            ", body2 0x%08x, chassisBody 0x%08x\n",
                            iterations,(long) group,(long) body2,
                            (long) chassisBody);
#endif

                        {
                            MdtContactID contact;

                            for (contact = MdtContactGroupGetFirstContact(group);
                                 contact != 0;
                                 contact = MdtContactGroupGetNextContact(group,contact)
                            )
                            {
                                const MdtContactParamsID cprms = MdtContactGetParams(contact);
                                MdtContactParamsSetPrimaryFriction(cprms, 0.0f);
                            }
                        }

                        MdtContactGroupDisable(group);
                        MdtContactGroupSetBodies(group, chassisBody, body2);
                        MdtContactGroupEnable(group);
                    }
                }

                /* Set c_list to previously stored next link    */
                c_list = next;
                iterations++;

                if (iterations > 20)
                    MeFatalError(0,"%s\n","LOD2 looping > 20 times");
            }

            McdModelSetBody(wheelModel, chassisBody);
            /* Set the offset between physical body and collision model. Zero in this case  */
            McdModelSetRelativeTransformPtrs( wheelModel,
				shape_offsets[i],
				McdModelGetTransformPtr(wheelModel),
				mcd_mtx_mem[i],
                0);
            McdModelSetMaterial(wheelModel, mat_lod2wheel);

            /* Set simple ang vel of wheel  */
            MdtBodyGetAngularVelocity(wheelBody, ang_vel);
            MdtCarWheelGetHingeAxis(wheelJoint->GetJoint(), hinge_axis);
            wheelJoint->ang_vel = MeVector3Dot(ang_vel, hinge_axis);

            /* Set simple rot ang of wheel  */
			body_mtx = (MeMatrix4 *)GetTMatrix(i);
			body_y_axis = (MeVector3 *)&(*body_mtx)[1];
			body_z_axis = (MeVector3 *)&(*body_mtx)[2];
			MeVector3Cross( temp, *body_y_axis, *steer_axis);
			wheelJoint->rot_angle = MeAtan2( MeVector3Dot(*body_z_axis, temp), MeVector3Dot(*steer_axis, *body_z_axis));
		}
    }
}

#if 0 //USE_CX_COLLISION

    //draws collision box around chassis
    McdGeometry *g;
    MeReal x,y,z;
    MeVector3 corner[8], pos;
    /* Draw around collision on chassis */
    g = McdModelGetGeometry( mcd_chassis );
    McdBoxGetRadii( (McdBoxID)g, &x, &y, &z );
    //MdtBodyGetPosition(&body[i], pos);
    pos[0] = mcd_mtx_mem[4][3][0];
    pos[1] = mcd_mtx_mem[4][3][1];
    pos[2] = mcd_mtx_mem[4][3][2];
    corner[0][0] =pos[0] + x*mcd_mtx_mem[4][0][0]+y*mcd_mtx_mem[4][1][0]+z*mcd_mtx_mem[4][2][0];
    corner[0][1] =pos[1] + x*mcd_mtx_mem[4][0][1]+y*mcd_mtx_mem[4][1][1]+z*mcd_mtx_mem[4][2][1];
    corner[0][2] =pos[2] + x*mcd_mtx_mem[4][0][2]+y*mcd_mtx_mem[4][1][2]+z*mcd_mtx_mem[4][2][2];

    corner[1][0] =pos[0] + x*mcd_mtx_mem[4][0][0]+y*mcd_mtx_mem[4][1][0]-z*mcd_mtx_mem[4][2][0];
    corner[1][1] =pos[1] + x*mcd_mtx_mem[4][0][1]+y*mcd_mtx_mem[4][1][1]-z*mcd_mtx_mem[4][2][1];
    corner[1][2] =pos[2] + x*mcd_mtx_mem[4][0][2]+y*mcd_mtx_mem[4][1][2]-z*mcd_mtx_mem[4][2][2];

    corner[2][0] =pos[0] + x*mcd_mtx_mem[4][0][0]-y*mcd_mtx_mem[4][1][0]+z*mcd_mtx_mem[4][2][0];
    corner[2][1] =pos[1] + x*mcd_mtx_mem[4][0][1]-y*mcd_mtx_mem[4][1][1]+z*mcd_mtx_mem[4][2][1];
    corner[2][2] =pos[2] + x*mcd_mtx_mem[4][0][2]-y*mcd_mtx_mem[4][1][2]+z*mcd_mtx_mem[4][2][2];

    corner[3][0] =pos[0] + x*mcd_mtx_mem[4][0][0]-y*mcd_mtx_mem[4][1][0]-z*mcd_mtx_mem[4][2][0];
    corner[3][1] =pos[1] + x*mcd_mtx_mem[4][0][1]-y*mcd_mtx_mem[4][1][1]-z*mcd_mtx_mem[4][2][1];
    corner[3][2] =pos[2] + x*mcd_mtx_mem[4][0][2]-y*mcd_mtx_mem[4][1][2]-z*mcd_mtx_mem[4][2][2];

    corner[4][0] =pos[0] - x*mcd_mtx_mem[4][0][0]+y*mcd_mtx_mem[4][1][0]+z*mcd_mtx_mem[4][2][0];
    corner[4][1] =pos[1] - x*mcd_mtx_mem[4][0][1]+y*mcd_mtx_mem[4][1][1]+z*mcd_mtx_mem[4][2][1];
    corner[4][2] =pos[2] - x*mcd_mtx_mem[4][0][2]+y*mcd_mtx_mem[4][1][2]+z*mcd_mtx_mem[4][2][2];

    corner[5][0] =pos[0] - x*mcd_mtx_mem[4][0][0]+y*mcd_mtx_mem[4][1][0]-z*mcd_mtx_mem[4][2][0];
    corner[5][1] =pos[1] - x*mcd_mtx_mem[4][0][1]+y*mcd_mtx_mem[4][1][1]-z*mcd_mtx_mem[4][2][1];
    corner[5][2] =pos[2] - x*mcd_mtx_mem[4][0][2]+y*mcd_mtx_mem[4][1][2]-z*mcd_mtx_mem[4][2][2];

    corner[6][0] =pos[0] - x*mcd_mtx_mem[4][0][0]-y*mcd_mtx_mem[4][1][0]+z*mcd_mtx_mem[4][2][0];
    corner[6][1] =pos[1] - x*mcd_mtx_mem[4][0][1]-y*mcd_mtx_mem[4][1][1]+z*mcd_mtx_mem[4][2][1];
    corner[6][2] =pos[2] - x*mcd_mtx_mem[4][0][2]-y*mcd_mtx_mem[4][1][2]+z*mcd_mtx_mem[4][2][2];

    corner[7][0] =pos[0] - x*mcd_mtx_mem[4][0][0]-y*mcd_mtx_mem[4][1][0]-z*mcd_mtx_mem[4][2][0];
    corner[7][1] =pos[1] - x*mcd_mtx_mem[4][0][1]-y*mcd_mtx_mem[4][1][1]-z*mcd_mtx_mem[4][2][1];
    corner[7][2] =pos[2] - x*mcd_mtx_mem[4][0][2]-y*mcd_mtx_mem[4][1][2]-z*mcd_mtx_mem[4][2][2];


    for(i=0;i<7;i++)
    {
        DebugLine(corner[i],corner[i+1],255,0,0);
    }

#endif

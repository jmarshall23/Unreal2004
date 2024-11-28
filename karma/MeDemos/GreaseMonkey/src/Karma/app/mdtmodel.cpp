/***********************************************************/
// This file implements all the mathengine dynamics
// - initializing and deleting the framework and world
// - car chassis, suspension, wheels, steering
/***********************************************************/

#include <math.h>
#include <malloc.h>

#include"rwcore.h"
#include"rpworld.h"



#include "mdtcar.hpp"

#include "MdtMath.h"


// *********** ME framework and world *************

extern bool debug;

void  TransformVec( keaImmTransformation* tm, MeVector4 vec, MeReal* tvec) {
	int i;
	for (i=0; i<3; i++) tvec[i] = tm->R0[i]*vec[0] + tm->R1[i]*vec[1] + tm->R2[i]*vec[2];
}

const bool fixChassis = false;//true;//

const MeReal resetHeight = (MeReal)(2.0);//2.0f;//


MdtSuspendedWheel::MdtSuspendedWheel( MdtWorld* sys, MdtBody *wheelBody, MdtBody *carBody,
					                  MeReal radius, bool setLeft, bool setFixed) :
								chassis_body(carBody), wheel_body(wheelBody),
								wheel_radius(radius), wheel_ground_attached(false),
								isLeft(setLeft), isFixed(setFixed)
{
	steering_angle = 0;
	drive_torque = 0;

	MdtCarWheelInit(&wheel_joint);
	MdtCarWheelSetBodies(&wheel_joint,carBody,wheelBody);
	MeVector3 p;
	MdtBodyGetPosition(wheelBody, p);
    MdtCarWheelSetPosition (&wheel_joint,p[0],p[1],p[2]);
	MeMatrix3 R;
	MdtBodyGetOrientation(carBody, R);
	MeReal *haxis = R[1];               // chassis y axis
	MdtCarWheelSetHingeAxis (&wheel_joint, haxis[0], haxis[1], haxis[2]);
	MeReal *saxis = R[2];               // chassis z axis
	MdtCarWheelSetSteeringAxis (&wheel_joint, saxis[0], saxis[1], saxis[2]);
//	MdtCarWheelSetHingeLimitedForceMotor (&wheel_joint, 0, 1e6);

    if (isFixed) {

        MdtCarWheelSetSteeringLimitedForceMotor (&wheel_joint, 0, 1e9);
        MdtCarWheelSetSteeringLock (&wheel_joint, 1);

	} else { // !isFixed

	}

    MdtCarWheelAttach(&wheel_joint,sys);
	MdtContact2Init(&wheel_ground_contact);
}

// Update steering and drive on a wheel

void MdtSuspendedWheel::Update(MdtWorld* sys) {

	MeVector3 haxis;
	MdtCarWheelGetHingeAxis(&wheel_joint,haxis);

    // Wheel/ground collision detection;
	// a sphere of wheel_radius against a horizontal plane at zero height
	int j;
    if (wheel_ground_attached) {
        MdtContact2Detach(&wheel_ground_contact,sys);
        wheel_ground_attached = false;
    }

	MeVector3 p;
	MdtBodyGetPosition(wheel_body, p);
    MeReal z = p[2];
    if (z <= wheel_radius) {
		MdtContact2Attach(&wheel_ground_contact,sys);
        wheel_ground_attached = true;
		MdtContact2SetType(&wheel_ground_contact,keaContactTypeFriction2D);
        MdtContact2SetSoftness (&wheel_ground_contact,0.01f);

		MdtContact2SetBodies(&wheel_ground_contact,wheel_body,0);
		for (j=0; j<3; j++)
			MdtContact2SetPosition(&wheel_ground_contact,p[0],p[1],p[2]-wheel_radius);
		for (j=0; j<3; j++)
			MdtContact2SetNormal(&wheel_ground_contact,0,0,1);
		MdtContact2SetPenetration(&wheel_ground_contact,-(p[2]-wheel_radius));

		// create principle friction axis (normal x hinge_axis)
		MeVector3 normal;
		MdtContact2GetNormal(&wheel_ground_contact,normal);

		MeVector3 a;
		Cross (normal,haxis, a);
        MakeUnitVector( a);
		MdtContact2SetDirection(&wheel_ground_contact,a[0],a[1],a[2]);

		// set slip factor ... should be based on wheel velocity!
		if(isFixed)
            MdtContact2SetSecondarySlip(&wheel_ground_contact,0.1f);
			//MdtContactSetParamK1(&wheel_ground_contact,0.1f);
		else // isSteered
            MdtContact2SetSecondarySlip(&wheel_ground_contact,0.05f);
			//MdtContactSetParamK1(&wheel_ground_contact,0.05f);
	}

	// wheel torque calculations
	if(isFixed) { // isFixed

		const MeReal p = (drive_torque>0) ? 1.0f*drive_torque : 0.75f*drive_torque;
		MdtBodyAddTorque (wheel_body, p*haxis[0],p*haxis[1],p*haxis[2]);
		MdtBodyAddTorque (chassis_body, -p*haxis[0],-p*haxis[1],-p*haxis[2]);

	} else {  // isSteered
              // do the steering
        const MeReal pgap = 0.4f;  // proportional gap (radians)
		const MeReal width = 0.5f;
		const MeReal speed = 5.0f;
		const MeReal forcemax = 1000.0f;
		MeReal theta = MdtCarWheelGetSteeringAngle (&wheel_joint);

        MeReal desired_vel = steering_angle*width + theta;
        if (desired_vel > pgap) desired_vel = pgap;
        if (desired_vel < -pgap) desired_vel = -pgap;
        MdtCarWheelSetSteeringLimitedForceMotor(&wheel_joint,speed*desired_vel,forcemax);

		const MeReal p = (drive_torque>0) ? 0.75f*drive_torque : drive_torque;
		MdtBodyAddTorque (wheel_body,p*haxis[0],p*haxis[1],p*haxis[2]);
		MdtBodyAddTorque (chassis_body,-p*haxis[0],-p*haxis[1],-p*haxis[2]);

	}
}

// set the suspension load acting on the wheel at equilibrium,
// the corresponding height (vertical height from the chassis frame to the wheel centre)
// and the total suspension travel between stops.
void MdtSuspendedWheel::SetSuspensionEquilibriumLoadHeightAndTravel(
				const MeReal* grav, MeReal timeStep,
                MeReal load, MeReal height, MeReal travel,
				MeReal damp, MeReal equi, MeReal ztos)  {

    const MeReal grav_mag = MeSqrt(grav[0]*grav[0] + grav[1]*grav[1] + grav[2]*grav[2]);
	const MeReal zero_load_height = height - 0.5f*ztos*travel;
	const MeReal top_stop_height = height + (1-equi)*travel;
	const MeReal bottom_stop_height = height - equi*travel;

	const MeReal Kp = 2*load*grav_mag/(ztos*travel);
	const MeReal Kd = damp*MeSqrt(load*Kp);
    MdtCarWheelSetSuspension(&wheel_joint,Kp,Kd,0.1f,
        bottom_stop_height,top_stop_height,zero_load_height,timeStep);
}

void MdtSuspendedWheel::SetSteeringAngle( MeReal angle) {
	steering_angle = angle;
}

void MdtSuspendedWheel::SetDriveTorque( MeReal torque) {
	drive_torque = torque * mGearRatio[mGear]/mFinalDriveRatio;
}

// *********************** MdtCar *******************************

MdtCar::MdtCar( MdtWorld* sys, const MeReal* grav, MeReal timeStep, CarData* car) : sys(sys), car_data(car)
{

	steering_angle = 0;

	// positions of the Wheels in the Vehicle body (chassis) frame
	const MeReal pWhV[4][3] = {
		car->wheelbase/2, car->front_track/2, car->front_wheel_radius - car->chassis_height_off_ground,
		car->wheelbase/2, -car->front_track/2, car->front_wheel_radius - car->chassis_height_off_ground,
		-car->wheelbase/2, car->rear_track/2, car->rear_wheel_radius - car->chassis_height_off_ground,
		-car->wheelbase/2, -car->rear_track/2, car->rear_wheel_radius - car->chassis_height_off_ground
	};

	MdtBody *chassisBody = &body[4];
	MdtBodyInit (chassisBody);
	MdtBodyAttach (chassisBody,sys);

	MeMatrix3 cI = {{car->chassis_MoI_xx,0,0},{0,car->chassis_MoI_yy,0},{0,0,car->chassis_MoI_zz}};
	MdtBodySetMass(chassisBody,car->chassis_mass);
	MdtBodySetInertiaTensor(chassisBody,cI);
    if (fixChassis) { // to do
	}
	MdtBodySetPosition(chassisBody,car->chassis_CoM_forward_offset,0,
		car->chassis_height_off_ground + car->chassis_CoM_upward_offset + resetHeight);

	int i;
	for (i=0; i<4; i++) { // front-left front-right back-left back-right

		MdtBody *wheelBody = &body[i];
		MdtBodyInit(wheelBody);

		MeMatrix3 wI = {{car->wheel_MoI_xx,0,0},{0,car->wheel_MoI_yy,0},{0,0,car->wheel_MoI_zz}};
		MdtBodySetMass(wheelBody,car->wheel_mass);
		MdtBodySetInertiaTensor(wheelBody,wI);

		MdtBodySetPosition(wheelBody, pWhV[i][0], pWhV[i][1], pWhV[i][2] + resetHeight);
		MdtBodyAttach (wheelBody,sys);

		wheel_joint[i] = new MdtSuspendedWheel( sys, wheelBody, chassisBody,
			(i<2) ? car->front_wheel_radius : car->rear_wheel_radius,  // radius
			(i==0 || i==2) ? true : false,                             // isLeft
			(i<2) ? false : true);                                     // isFixed

        MeReal load, height, travel;
		MeReal tmp = car->chassis_CoM_forward_offset/car->wheelbase/2;
		if (i<2) {
			load = ((MeReal)(0.25) + tmp)*car->chassis_mass;
			height = -car->chassis_height_off_ground + car->front_wheel_radius;
			travel = car->front_suspension_travel;
		} else {
			load = ((MeReal)(0.25) - tmp)*car->chassis_mass;
			height = -car->chassis_height_off_ground + car->rear_wheel_radius;
			travel = car->rear_suspension_travel;
		}
		wheel_joint[i]->SetSuspensionEquilibriumLoadHeightAndTravel( grav, timeStep, load, height, travel);
	}
}


void MdtCar::Update() {
	// do ackerman steering
	const MeReal tana = MeTan(steering_angle);
	MeReal left_angle = MeAtan2(tana, 1 - 0.5f*tana*front_track/wheelbase);
	wheel_joint[0]->SetSteeringAngle( left_angle);
	MeReal right_angle = MeAtan2(tana, 1 + 0.5f*tana*front_track/wheelbase);
	wheel_joint[1]->SetSteeringAngle( right_angle);
	int i;
	for (i=0; i<4; i++) wheel_joint[i]->Update(sys);
}

const MeReal MdtCar::GetSpeed() {
	MeMatrix3 cR;
	MdtBody *chassis_body = &body[4];
	MdtBodyGetOrientation(chassis_body, cR);
	const MeReal *xChW = cR[0];			// x axis of Chassis in World
	MeVector3 vChW;
	MdtBodyGetLinearVelocity(chassis_body,vChW);
	return vChW[0]*xChW[0] + vChW[1]*xChW[1] + vChW[2]*xChW[2];
}

void MdtCar::SetDrive(MeReal drive) {
	int i;
	for (i=0;i<4;i++) wheel_joint[i]->SetDriveTorque(drive*100);
}

void MdtCar::Reset() {
	steering_angle = 0;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
const MeReal MdtCar::GetBodyPosCmpt(int b, int i) {
	MeVector3 v;
	MdtBodyGetPosition(&body[b], v);
	return v[i];
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
const MeReal MdtCar::GetBodyVelCmpt(int b, int i) {
	MeVector3 v;
	MdtBodyGetLinearVelocity(&body[b], v);
	return v[i];
}


#ifndef _MDTCAR_H
#define _MDTCAR_H

#include "Mdt.h"

#include "CarData.hpp"

//#define USE_CX_COLLISION 1

typedef enum {
	FRONT_L_CMPT=0,
	FRONT_R_CMPT,
	REAR_L_CMPT,
	REAR_R_CMPT,
	CHASSIS_CMPT,
	NUM_CAR_CMPTS
} CAR_CMPTS;

typedef enum {
	NO_LOD = -1,
	LOD1 = 0,
	LOD2,
	NUM_LODS,
} LEVEL_OF_DETAIL;

void MdtInit(MdtWorld* sys);    // create mathengine framework and world
void MdtDestroy();  // delete mathengine framework and world

// Suspension setup notes
// ======================
//
// When the car is at rest on level ground the equilibrium suspension height of a
// particular wheel depends on the load it carries and its suspension spring rate.
// Now, the load that a wheel carries as a fraction of the vehicle chassis load
// depends on the positions of all the wheels and on the car's centre of mass,
// so the wheel loads are calculated when constructing the car and suspension
// setup is done when constructing the car.
//
// Rather than setting spring and damping parameters, the suspension setup
// uses suspension travel and load specifications to calculate the spring rate
// and a damping ratio (which defaults to 1 for critical damping).
// The equilibrium height of a wheel is taken as the reference.
//
// This sketch shows the suspension kinematics with the default proportions.
//
//     chassis _________               wheel
//      level      |                  heights
//                 |          100%   __________ top stop height
//                 |           90% ^ __________ twice-equilibrium-load height
//     equilibrium | height        |            (roughly semi-car-load)
//       ( usually | -ve )         |
//                 |               |
//                 |               |
//             ____v____      50%  | __________ equilibrium-load height
//                                 |            (roughly quarter-car-load)
//                                 |
//                                 |
//                                 |
//                             10% | __________ zero load height
//                              0% v __________ bottom stop height


// suspended wheel with steering optional
// This class contains the constraints to implement rolling contact, suspension
// and, optionally, steering.

typedef struct _wheel_userdata_s {
	MdtContact *contact; //filled in by collision
	MdtCarWheel *wheel_joint;
	CarData	*car_data;
	int	on_track	:1;
} WHEEL_USERDATA;

class MdtSuspendedWheel {  // wheel with or without steering, no suspension
public:
	bool isFixed; // wheel can be fixed or steered (fixed by default; isFixed=true
//	bool isLeft;  // wheel can be left or right
	MdtBody *chassis_body;    // pointer to the body wheel is attached to
	MdtBody *wheel_body;      // pointer to wheel body

	// Constraints joints, forces and contacts used to implement
	// wheel rolling and steering;
public:
	MdtCarWheelID wheel_joint;  // steering hinge
//	MdtContact wheel_ground_contact;
	WHEEL_USERDATA user_data;
//	bool	wheel_ground_attached;
	MeReal  wheel_radius;
	MeVector3 wheel_pos;	//Equilibrium position of wheels relative to car chassis
	// steering
	MeReal req_steer_angle, act_steer_angle;
	MeReal rot_angle, ang_vel;
    //drive
	MeReal drive_torque;

public:

	MdtSuspendedWheel( MdtWorld* sys, MdtBody *chassis, MdtBody *wheel, MeReal radius,
					   bool setFixed=true);
	void Update(MdtWorld* sys, int fwd_gear);
	void Reset();
	void SetSteeringAngle( MeReal angle);
	void SetDriveTorque( MeReal torque);
	MeReal GetDriveTorque() {return drive_torque; }
	WHEEL_USERDATA *GetUserData() { return &user_data; }
	MdtCarWheelID GetJoint() { return wheel_joint; }
	// Set suspension parameters for the wheel;
	// (1) Equilibrium load;
	//    (mass, actually; that part of the chassis mass that this wheel carries)
	// (2) Equilibrium height of suspension;
	//    (height of wheel centre frame above chassis frame, usually negative)
	// (3) Total length of suspension travel from top stop to bottom stop.
	// default parameters are;
	// (4) Damping ratio (set by default to critical damping = 1),
	// (5) Equilibrium height as a ratio between bottom stop (0) and top stop (1)
	//    (set by default to the half way mark = 0.5)
	// (6) The ratio a/b of;
	//     (a) the suspension travel between zero load and twice equilibrium load,
	//   & (b) the total suspension travel defined above.
	//    (set to 0.8 by default)

	void SetSuspensionEquilibriumLoadHeightAndTravel( const MeReal* grav,
					        MeReal load, MeReal height, MeReal travel,
					        MeReal damp=1, MeReal equi=0.5, MeReal ztos = 0.8,
                            MeReal soft = 0.02);
	void SetRelWheelPos(const MeReal *v) { wheel_pos[0] = v[0]; wheel_pos[1] = v[1]; wheel_pos[2] = v[2]; }
	MeReal *GetWheelPos(void) { return wheel_pos; }
	MeReal GetRadius(void) {return wheel_radius; }
};

class  MdtCar {
public:
    MdtWorldID sys;
	MdtBodyID body[5];		// wheels (fl,fr,bl,br) and chassis
	MdtSuspendedWheel *wheel_joint[4];		// constraints for the wheels; hinge, contact
	CarData car_data;
//	MeReal wheelbase;
//	MeReal front_track;
//	MeReal max_steering_angle;
//	MeReal max_steering_speed;
	MeReal req_steer_angle;
	MeReal drive_torque;

	MeReal mGearRatio[5];
	int    mGear;
	MeReal mFinalDriveRatio;
	MeReal mRedline;
	MeReal mRPM;
	int	   tranny_type;  // 0 = manual, 1 = auto
	int initialised_already; //flag to determine if car is being reinitialised

	MeMatrix4 *shape_offsets; //Used for offsetting collision models as well
	MeMatrix4 *mcd_mtx_mem;
	McdModel *mcd_chassis, *mcd_wheel[4];
	LEVEL_OF_DETAIL level_of_detail;

//protected:
	MdtCar();
	~MdtCar();
	//MdtCar(MdtWorld* sys, const MeReal* grav, MeReal timeStep, CarData* c);  // create car and initialize from car data structure
    void CreateMdtCar(MdtWorldID world);
	void InitMdtCar(MdtWorld* sys, const MeReal* grav, CarData* c);
public:
	MeReal chassis_height;
	void Reset();
	void Update();
	void PreEvolve();
	const MeReal GetSpeed();
	const MeReal GetWheelSpeed();
	const MeReal GetMotorRPM();
	void SetSteeringAngle(MeReal steer);
	void SetDrive(MeReal drive);
	const MeMatrix4* GetTMatrix(int i);
	MdtBodyID GetBody(int i) {return body[i]; }
	void SetPosition(MeReal x, MeReal y);
	void SetPosition(MeReal x, MeReal y, MeReal z);
	void SetOrientation(MeVector4 q);
	void GetOrientation(MeVector4 q);
	MeReal GetBodyPosCmpt(int b, int i);
	MeReal GetBodyVelCmpt(int b, int i);
	MeReal GetBodyAngVelCmpt(int b, int i);
	MeReal GetBodySlipAng();
	MeReal GetBodySlipVel();
	MeReal GetBodyForwardVel();
	MeReal GetWheelSlipAng(int i);
	MeReal GetWheelSlipVel(int i);
	MeReal GetWheelSteerAngle(int i);
	MeReal GetReqSteerAngle(int i) { return wheel_joint[i]->req_steer_angle; }
	MeReal GetActualSteerAngle(int i) { return wheel_joint[i]->act_steer_angle; }
	MeReal GetWheelRotAngle(int i) { return wheel_joint[i]->rot_angle; }
	int	WheelOnTrack(int i) { return wheel_joint[i]->GetUserData()->on_track; }
	int OnTrack() { return WheelOnTrack(0) | WheelOnTrack(1) | WheelOnTrack(2) | WheelOnTrack(3) ; }
	int OnGround();
	void CalcShapeOffset(int i, MeVector3 v); //Pass vector to be filled in
	void Upshift();
	void Downshift();
	void Shift(int gear);
	int Gear() {return mGear; }
	int TrannyType() {return tranny_type;}
	void SetTrannyType(int type);
	void SetLevelOfDetail(LEVEL_OF_DETAIL lod);
};

/* These are just temporarily inluded here until we standardise the maths utilities	*/
inline MeReal _Sqr (MeReal x) ///
/// Return x*x.
{
  return x*x;
}

//***************************************************************************
//+ Vector and quanternion stuff.

inline MeReal _Dot (MeReal b[3], MeReal c[3]) ///
/// 3-way dot product.
{
  return b[0]*c[0] + b[1]*c[1] + b[2]*c[2];
}

inline void _Cross (MeReal b[3], MeReal c[3], MeReal a[3]) ///
/// Set a = b x c.
{
  a[0] = b[1]*c[2] - b[2]*c[1];
  a[1] = b[2]*c[0] - b[0]*c[2];
  a[2] = b[0]*c[1] - b[1]*c[0];
}

extern MeReal _Normalise(MeVector3 out, const MeVector3 in);

extern MeReal _MakeUnitVector (MeReal v[3]);

#endif

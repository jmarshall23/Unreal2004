#ifndef _MDTCAR_H
#define _MDTCAR_H

#include "Mdt.h"

#include "CarData.hpp"

//extern const MeReal gravity[3];
//extern const MeReal timeStep;

//extern MdtWorld sys;


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

class MdtSuspendedWheel {  // wheel with or without steering, no suspension
public:
	bool isFixed; // wheel can be fixed or steered (fixed by default; isFixed=true)
	bool isLeft;  // wheel can be left or right
	MdtBody *chassis_body;    // pointer to the body wheel is attached to
	MdtBody *wheel_body;      // pointer to wheel body

	// Constraints joints, forces and contacts used to implement
	// wheel rolling and steering;
private:
	MdtCarWheel wheel_joint;  // steering hinge
	MdtContact2 wheel_ground_contact;
	bool	wheel_ground_attached;
	MeReal  wheel_radius;

	// steering
	MeReal steering_angle;

    //drive
	MeReal drive_torque;

public:

	MdtSuspendedWheel( MdtWorld* sys, MdtBody *chassis, MdtBody *wheel, MeReal radius,
					   bool setLeft, bool setFixed=true);
	void Update(MdtWorld* sys);
	void Reset();
	void SetSteeringAngle( MeReal angle);
	void SetDriveTorque( MeReal torque);

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

	void SetSuspensionEquilibriumLoadHeightAndTravel(
								const MeReal* grav, MeReal timeStep,
                                MeReal load, MeReal height, MeReal travel,
								MeReal damp=1, MeReal equi=0.5, MeReal ztos = 0.8);

};
class  MdtCar {
	MdtWorld* sys;
	CarData* car_data;
	MdtBody body[5];		// wheels (bl,br,fl,fr) and chassis
	MdtSuspendedWheel *wheel_joint[4];		// constraints for the wheels; hinge, contact

	MeReal wheelbase;
	MeReal front_track;

	MeReal	steering_angle;

public:
	MdtCar(MdtWorld* sys, const MeReal* grav, MeReal timeStep, CarData* c);  // create car and initialize from car data structure
	void Reset();
	void Update();
	const MeReal GetSpeed();
	const MeReal GetBodyVelCmpt(int b, int i);
	const MeReal GetBodyPosCmpt(int b, int i);
	void SetSteeringAngle(MeReal steer) {steering_angle = steer*car_data->max_steering_angle; }
	void SetDrive(MeReal drive);
	const MeReal* GetTMatrix(int i) { return MdtBodyGetTM(&body[i]); }

};


#endif

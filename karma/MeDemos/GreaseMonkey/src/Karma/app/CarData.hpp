#ifndef _CARDATA_H
#define _CARDATA_H

typedef struct _tyre_params_s {
	MeReal max_friction;		//max limiting friction value
	MeReal max_friction_zone;	//range of contact camber angle
	MeReal min_friction;		//min ,limiting friction value
	MeReal min_friction_cutoff;	//min_friction applies after this cutoff
	MeReal max_lat_slip_vel;	//max lateral slip velocity
	MeReal max_roll_slip_vel;	//max rolling slip vel
	MeReal min_slip_vel;		//min slip tolerance
	MeReal slip_rate;			//increase in slip with tyre vel
} TYRE_PARAMS;

struct CarData { // Graphical and dynamical data for a simple car

//  Parameters playing a part in both the dynamics and the graphics
	MeReal wheelbase;
	MeReal front_track;
	MeReal rear_track;
	MeReal front_wheel_radius;
	MeReal rear_wheel_radius;
	MeReal chassis_height_off_ground;

//  Parameters mostly affecting the dynamics
	MeReal chassis_mass;
	MeReal chassis_CoM_upward_offset;
	MeReal chassis_CoM_forward_offset;
	MeReal chassis_X_pos; //Equivalent to height off ground but measured forward of midpoint between wheels
	MeReal chassis_MoI_xx;
	MeReal chassis_MoI_yy;
	MeReal chassis_MoI_zz;
	MeVector3 chassis_coll_box;
	MeReal wheel_mass;
	MeReal wheel_MoI_xx;
	MeReal wheel_MoI_yy;
	MeReal wheel_MoI_zz;

	MeReal front_suspension_travel; // full travel distance between top stop and bottom stop
	MeReal rear_suspension_travel;

	MeReal suspension_level_tweak;  // Suspension height is calculated assuming linear springs.
                                    // The suspension springs implemented in Kea are not linear.
                                    // This tweak is a virtual forward offset, in proportion
	                                // to the wheelbase, to set the car level.

	MeReal front_suspension_damp;   // damping ratios. set to 1 for critical damping
	MeReal rear_suspension_damp;
	MeReal front_suspension_equi;   // proportional equilibrium (loaded) height
	MeReal rear_suspension_equi;    //   between bottom stop and top stop
	MeReal front_suspension_ztos;
	MeReal rear_suspension_ztos;
	MeReal front_suspension_soft;
	MeReal rear_suspension_soft;

	MeReal max_steering_angle;
	MeReal torque_multiplier;
	MeReal brake_multiplier;

//	Extra graphics parameters for simple cuboid chassis and cylindrical wheels
	MeReal chassis_length;
	MeReal chassis_width;
	MeReal chassis_depth;
	MeReal chassis_forward_offset;
	MeReal front_wheel_width;
	MeReal rear_wheel_width;

	TYRE_PARAMS tyre_track_props;
	TYRE_PARAMS tyre_grass_props;
};


#endif
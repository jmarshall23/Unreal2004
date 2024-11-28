#ifndef _CARDATA_H
#define _CARDATA_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:33 $ - Revision: $Revision: 1.5.6.1 $

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
  Dynamical data for a simple car.
*/

typedef struct CarData
{
    /*
      Parameters playing a part in both the dynamics and the graphics.
    */

    /* distance between front and rear axles */
    MeReal wheelbase;

    /* distance between front wheel centres */
    MeReal front_track;

    /* distance between rear wheel centres */
    MeReal rear_track;

    /* rolling radius of front wheel */
    MeReal front_wheel_radius;

    /* rolling radius of rear wheel */
    MeReal rear_wheel_radius;

    /*
      Height of chassis ref.  frame above ground (chassis ref. frame is
      located centrally half way between front and rear axles).
    */

    MeReal chassis_height_off_ground;

    /*
      Parameters mostly affecting the dynamics.
    */

    /* mass of chassis */
    MeReal chassis_mass;

    /* height of pCoM above chassis ref.  frame */
    MeReal chassis_CoM_upward_offset;

    /* forward offset of Com from chassis ref. frame */
    MeReal chassis_CoM_forward_offset;

    /* Chassis MoI about front-rear axis through CoM */
    MeReal chassis_MoI_roll;

    /* Chassis MoI about left-right axis through CoM */
    MeReal chassis_MoI_pitch;

    /* Chassis MoI about vertical axis through CoM */
    MeReal chassis_MoI_yaw;

    /* mass of wheels */
    MeReal wheel_mass;

    /* Wheel MoI about rolling axis */
    MeReal wheel_MoI_axial;

    /* Wheel MoIs about diameter lines */
    MeReal wheel_MoI_radial;

    /* Total wheel travel between suspension stops */
    MeReal front_suspension_travel;
    MeReal rear_suspension_travel;

    /*
      Suspension damping factor.

      <1 underdamped, =1 critical damping, >1 overdamped.
    */

    MeReal front_suspension_damp;
    MeReal rear_suspension_damp;

    /*
      Equilibrium height of wheel above suspension bottom stop as a ratio of
      full travel (0.5).
    */
    MeReal front_suspension_equi;
    MeReal rear_suspension_equi;

    /*
      Travel of wheel from Zero load TO Semi-car load as a ratio of full
      suspension travel.
    */
    MeReal front_suspension_ztos;
    MeReal rear_suspension_ztos;

    /*
      ``Softness'' of suspension stops (0.05).
    */
    MeReal front_suspension_soft;
    MeReal rear_suspension_soft;

    /* Parameters for controlling handling */
    MeReal max_steering_angle;

    /*
      Parameters for wheel ground contact 'softness' of wheel-ground
      contacts.
    */
    MeReal front_contact_softness;
    MeReal rear_contact_softness;

    /*
      side slip parameters
    */
    MeReal front_contact_lateral_slip;
    MeReal rear_contact_lateral_slip;

    /*
      Extra graphics parameters for simple cuboid chassis and cylindrical
      wheels.
    */

    /* full length of car chassis */
    MeReal chassis_length;

    /* full width of car chassis */
    MeReal chassis_width;

    /* full height of car chassis */
    MeReal chassis_depth;

    /* offset of chassis graphic (cuboid) from chassis frame */
    MeReal chassis_forward_offset;

    /* width of front wheel graphic */
    MeReal front_wheel_width;   /* (cylinder) */

    /* width of rear wheel graphic */
    MeReal rear_wheel_width;    /* (cylinder) */

} CarData ;


#endif



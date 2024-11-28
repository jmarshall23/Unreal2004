/***********************************************************************************************
*
*   $Id: carAPI.cpp,v 1.1.2.5 2002/03/13 13:17:33 richardm Exp $
*
************************************************************************************************/

/* INCLUDES */
#include<malloc.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "rwcore.h"
#include "rpworld.h"

#include "RwFuncs.hpp"
#include "skeleton.h"

#define COLLISION_BEFORE_EVOLVE 0
#define USE_TRI_LIST_COLL       1
//#define DO_TIME_OF_IMPACT     0

#define DEBUG_LOGGING           0

#ifndef _XBOX
    #ifdef WIN32
        #include <windows.h>
    #endif
#else
    #include <xtl.h>
#endif

//#if USE_CX_COLLISION
extern "C" {
//extern struct MeMemoryAPI MeMemoryAPIMalloc;

#include "Mst.h"

//#include "McdFrame.h"
//#include "McdPrimitives.h"

#if USE_TRI_LIST_COLL
#include "rpcollis.h"
#include "McdTriangleList.h"
#define TRI_LIST_SIZE 20
McdUserTriangle tri_list_memory[TRI_LIST_SIZE];
MeVector3 normal_memory[TRI_LIST_SIZE]; //RenderWare doesn't store normals
MeVector3 vertex_memory[TRI_LIST_SIZE][3]; //test

#else
#include "McdRwBSP.h"
#endif

//#include "McdDtBridge.h"
}
int geoTypeMaxCount;
MstBridgeID cdHandler = 0;
McdSpaceID mcd_space = 0;
McdModelID mcd_world = 0;
McdFramework *mcdframe = 0;
MstMaterialID mat_track, mat_lod1wheel,mat_lod2wheel, mat_chassis;
//#endif

MeReal Px, By;

#include "../plat/platxtra.h"
#include "MdtCar.hpp"
#include "carAPI.hpp"
#include "MeMath.h"
#include "utils.hpp"
#include "parser.hpp"
#include "car.hpp"

#include "driver.hpp"

#include "MeMemory.h"

//void *mdt_mem;
//static int m_size = 1024*1024*2;
int num_contacts;
static MdtWorldID mdt_world = 0;
static MeReal gravity[] = {0,0,-32.2f};

static CarData cardat;

int MEAPI DefaultGroundCB(McdIntersectResult* c, McdContact* cp,
                         MdtContactID contact);

static RpWorld *RW_World = 0;
int safe_time_on = 0;

/* Car data file parser functions   */
static void xWheelBase(Parser *p);
static void xWheelTrack(Parser *p);
static void xWheelRad(Parser *p);
static void xWheelMass(Parser *p);
static void xChassisHOG(Parser *p);
static void xChassisXPos(Parser *p);
static void xChassisMass(Parser *p);
static void xChassisUpOff(Parser *p);
static void xChassisFwdOff(Parser *p);
static void xSuspTravel(Parser *p);
static void xMaxSteer(Parser *p);
static void xTorqMult(Parser *p);
static void xBrakeMult(Parser *p);
static void xSuspTravel(Parser *p);
static void xSuspLevelTweak(Parser *p);
static void xSuspDamp(Parser *p);
static void xSuspEquilibrium(Parser *p);
static void xSuspZToS(Parser *p);
static void xSuspSoft(Parser *p);
static void xTyreTrackProps(Parser *p);
static void xTyreGrassProps(Parser *p);
static void xChassisCollision(Parser *p);

static TEXT_2_PFUNC comtable[] = {
    {"WHEEL_BASE", xWheelBase},
    {"WHEEL_TRACK", xWheelTrack},
    {"WHEEL_RADIUS", xWheelRad},
    {"WHEEL_MASS", xWheelMass},
    {"CHASSIS_HEIGHT_OFF_GROUND", xChassisHOG},
    {"CHASSIS_X_POS", xChassisXPos},
    {"CHASSIS_MASS", xChassisMass},
    {"CHASSIS_COM_UP_OFFSET", xChassisUpOff},
    {"CHASSIS_COM_FWD_OFFSET", xChassisFwdOff},
    {"CHASSIS_COLL_BOX", xChassisCollision},
    {"SUSP_TRAVEL", xSuspTravel},
    {"SUSP_LEVEL_TWEAK", xSuspLevelTweak},
    {"SUSP_DAMPING", xSuspDamp},
    {"SUSP_EQUILIBRIUM", xSuspEquilibrium},
    {"SUSP_Z_TO_S", xSuspZToS},
    {"SUSP_SOFT", xSuspSoft},
    {"MAX_STEERING_ANGLE", xMaxSteer},
    {"TORQUE_MULT", xTorqMult},
    {"BRAKE_MULT", xBrakeMult},
    {"TYRE_TRACK_PROPS", xTyreTrackProps},
    {"TYRE_GRASS_PROPS", xTyreGrassProps},
    {0,0}
};

//static int num_contacts = 0;
void ToggleSafeTime(void)
{
	safe_time_on = 1 - safe_time_on;
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xWheelBase(Parser *p)
{
    cardat.wheelbase = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xWheelTrack(Parser *p)
{
    cardat.front_track = p->GetFloat();
    cardat.rear_track = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xWheelRad(Parser *p)
{
    cardat.front_wheel_radius = p->GetFloat();
    cardat.rear_wheel_radius = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xChassisHOG(Parser *p)
{
    cardat.chassis_height_off_ground = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xChassisXPos(Parser *p)
{
    cardat.chassis_X_pos = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xChassisMass(Parser *p)
{
    cardat.chassis_mass = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xChassisUpOff(Parser *p)
{
    cardat.chassis_CoM_upward_offset = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xChassisFwdOff(Parser *p)
{
    cardat.chassis_CoM_forward_offset = p->GetFloat();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xChassisCollision(Parser *p)
{
    cardat.chassis_coll_box[0] = p->GetFloat();
    cardat.chassis_coll_box[1] = p->GetFloat();
    cardat.chassis_coll_box[2] = p->GetFloat();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xWheelMass(Parser *p)
{
    cardat.wheel_mass = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xSuspTravel(Parser *p)
{
    cardat.front_suspension_travel = p->GetFloat();
    cardat.rear_suspension_travel = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xMaxSteer(Parser *p)
{
    cardat.max_steering_angle = DegsToRads(p->GetFloat());
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xTorqMult(Parser *p)
{
    cardat.torque_multiplier = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xBrakeMult(Parser *p)
{
    cardat.brake_multiplier = p->GetFloat();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xSuspLevelTweak(Parser *p)
{
    cardat.suspension_level_tweak = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xSuspDamp(Parser *p)
{
    cardat.front_suspension_damp = p->GetFloat();
    cardat.rear_suspension_damp = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xSuspEquilibrium(Parser *p)
{
    cardat.front_suspension_equi = p->GetFloat();
    cardat.rear_suspension_equi = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xSuspZToS(Parser *p)
{
    cardat.front_suspension_ztos = p->GetFloat();
    cardat.rear_suspension_ztos = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xSuspSoft(Parser *p)
{
    cardat.front_suspension_soft = p->GetFloat();
    cardat.rear_suspension_soft = p->GetFloat();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xTyreTrackProps(Parser *p)
{
    cardat.tyre_track_props.max_friction            = p->GetFloat();
    cardat.tyre_track_props.max_friction_zone       = p->GetFloat();
    cardat.tyre_track_props.min_friction            = p->GetFloat();
    cardat.tyre_track_props.min_friction_cutoff     = p->GetFloat();
    cardat.tyre_track_props.max_lat_slip_vel        = p->GetFloat();
    cardat.tyre_track_props.max_roll_slip_vel       = p->GetFloat();
    cardat.tyre_track_props.min_slip_vel            = p->GetFloat();
    cardat.tyre_track_props.slip_rate               = p->GetFloat();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xTyreGrassProps(Parser *p)
{
    cardat.tyre_grass_props.max_friction            = p->GetFloat();
    cardat.tyre_grass_props.max_friction_zone       = p->GetFloat();
    cardat.tyre_grass_props.min_friction            = p->GetFloat();
    cardat.tyre_grass_props.min_friction_cutoff     = p->GetFloat();
    cardat.tyre_grass_props.max_lat_slip_vel        = p->GetFloat();
    cardat.tyre_grass_props.max_roll_slip_vel       = p->GetFloat();
    cardat.tyre_grass_props.min_slip_vel            = p->GetFloat();
    cardat.tyre_grass_props.slip_rate               = p->GetFloat();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int LoadCarDataFromFile(char *fname){
    Parser p;
    MeReal mrr;

    if(p.Open(fname))
    {
        p.SetComTable(comtable);
        p.Parse();

        /* Initialise Moments of Inertia for chassis & wheels   */
        mrr = cardat.chassis_mass*cardat.wheelbase*cardat.front_track;
    //  cardat.chassis_MoI_xx =  0.1f*mrr;
    //  cardat.chassis_MoI_yy = 0.25f*mrr;
    //  cardat.chassis_MoI_zz = 0.15f*mrr;
        cardat.chassis_MoI_xx = 0.2f*mrr;
        cardat.chassis_MoI_yy = 0.2f*mrr;
        cardat.chassis_MoI_zz = 0.2f*mrr;


        mrr = cardat.wheel_mass*cardat.front_wheel_radius*cardat.front_wheel_radius;
        cardat.wheel_MoI_xx = 1*mrr;
        cardat.wheel_MoI_yy = 1*mrr;
        cardat.wheel_MoI_zz = 1*mrr;
        //cardat.wheel_MoI_xx = 0.2f;
        //cardat.wheel_MoI_yy = 0.2f;
        //cardat.wheel_MoI_zz = 0.2f;


        return 1;
    }

    return(0);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int UseCompiledCarData(){
    MeReal mrr;

    /* Use this data if no external data file supplied  */
    cardat.wheelbase = 4.735f;
    cardat.front_track = 3.0f;
    cardat.rear_track = 3.0f;
    cardat.front_wheel_radius = 0.717f;
    cardat.rear_wheel_radius = 0.717f;
    cardat.chassis_height_off_ground = 1.6f;
    cardat.chassis_X_pos = 0.15f;
    cardat.chassis_mass = 1000.0f;
    cardat.chassis_CoM_upward_offset = -1.25f;
    cardat.chassis_CoM_forward_offset = 0.05f;
    cardat.wheel_mass = 50.0f;
    cardat.front_suspension_travel = 1.0f;
    cardat.rear_suspension_travel = 0.7f;
    cardat.max_steering_angle = DegsToRads(40);
    cardat.torque_multiplier = 7.75f;
    cardat.brake_multiplier = 1.0f;
    cardat.suspension_level_tweak = 0.0;
    cardat.front_suspension_damp = 0.2f;
    cardat.rear_suspension_damp = 0.2f;
    cardat.front_suspension_equi = 0.3f;
    cardat.rear_suspension_equi = 0.3f;
    cardat.front_suspension_ztos = 0.3f;
    cardat.rear_suspension_ztos = 0.3f;
    cardat.front_suspension_soft = 0.01f;
    cardat.rear_suspension_soft = 0.01f;

    cardat.chassis_coll_box[0] = 3.5f;
    cardat.chassis_coll_box[1] = 1.4f;
    cardat.chassis_coll_box[2] = 0.9f;

    mrr = cardat.chassis_mass*cardat.wheelbase*cardat.front_track;
    //  cardat.chassis_MoI_xx =  0.1f*mrr;
    //  cardat.chassis_MoI_yy = 0.25f*mrr;
    //  cardat.chassis_MoI_zz = 0.15f*mrr;
        cardat.chassis_MoI_xx = 0.2f*mrr;
        cardat.chassis_MoI_yy = 0.2f*mrr;
        cardat.chassis_MoI_zz = 0.2f*mrr;


        mrr = cardat.wheel_mass*cardat.front_wheel_radius*cardat.front_wheel_radius;
        cardat.wheel_MoI_xx = 1*mrr;
        cardat.wheel_MoI_yy = 1*mrr;
        cardat.wheel_MoI_zz = 1*mrr;
        //cardat.wheel_MoI_xx = 0.2f;
        //cardat.wheel_MoI_yy = 0.2f;
        //cardat.wheel_MoI_zz = 0.2f;

    cardat.tyre_track_props.max_friction            = 15000.0f;
    cardat.tyre_track_props.max_friction_zone       = 0.05f;
    cardat.tyre_track_props.min_friction            = 0.0f;
    cardat.tyre_track_props.min_friction_cutoff     = 0.3f;
    cardat.tyre_track_props.max_lat_slip_vel        = 0.06f;
    cardat.tyre_track_props.max_roll_slip_vel       = 0.02f;
    cardat.tyre_track_props.min_slip_vel            = 0.0001f;
    cardat.tyre_track_props.slip_rate               = 0.00008f;

    cardat.tyre_grass_props.max_friction            = 2000.0f;
    cardat.tyre_grass_props.max_friction_zone       = 0.05f;
    cardat.tyre_grass_props.min_friction            = 0.0f;
    cardat.tyre_grass_props.min_friction_cutoff     = 0.3f;
    cardat.tyre_grass_props.max_lat_slip_vel        = 0.06f;
    cardat.tyre_grass_props.max_roll_slip_vel       = 0.02f;
    cardat.tyre_grass_props.min_slip_vel            = 0.00015f;
    cardat.tyre_grass_props.slip_rate               = 0.0006f;


        return 1;

}


/***********************************************************************************************
*
*
*
************************************************************************************************/
//#if USE_CX_COLLISION
inline void RenderToPhysics(MeVector3 physPoint, MeVector3 rendPoint, MeMatrix4 matrix) {
    MeVector3 rel;
    rel[0] = rendPoint[0]-matrix[3][0];
    rel[1] = rendPoint[1]-matrix[3][1];
    rel[2] = rendPoint[2]-matrix[3][2];
    physPoint[0] = _Dot(rel,matrix[0]);
    physPoint[1] = _Dot(rel,matrix[1]);
    physPoint[2] = _Dot(rel,matrix[2]);
}

/**
 Used by WheelGroundCB to calculate slip parameter
*/

/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal getChassisSpeed(MdtBody*chassis_body)
{
    MeMatrix3 cR;
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
MeReal getWheelTangentialSpeed(MdtSuspendedWheel* wheel)
{
    MeVector3 ang_vel;
    MdtBodyGetAngularVelocity(wheel->wheel_body, ang_vel);
    MdtConvertVector((MdtBody *)NULL, ang_vel, wheel->wheel_body, ang_vel);

    return ang_vel[1]*wheel->GetRadius();
}

/***********************************************************************************************
*
*   This is essentially the 'Tyre model'. It varies the contact parameters in relation to
*   the contact conditions. It also determines whether the contact is necessary in the
*   first place.
*
************************************************************************************************/
MeBool MEAPI WheelGroundCB(McdIntersectResult* c, McdContact* cp,
                         MdtContactID contact) {

    MeVector3 hinge_axis, a, normal, pos;
    MdtSuspendedWheel *wheel;
    MdtBody *b1, *b2;
 //   RpWorldSector *sector; // = (RpWorldSector *)cp->element1.ptr;
    MeReal epsilon;
    MeMatrix4 *groundTM;
    MeReal radius;
    RwBool track_contact = FALSE;
    int i=0;
    MdtBodyID wheel_body;
    int triIndex = cp->element2.tag;

//	sector = (RpWorldSector *)cp->element1.ptr;
	RpPolygon *poly = (RpPolygon *)cp->element2.ptr; //sector->polygons+triIndex;

    if(cp->separation > 0.01f)
    {
        return 0;
    }


    /* Determine if contact is with track material or not   */
    while(i<NUM_TRACK_TEXTURES && !track_contact)
    {
        if(poly->matIndex == track_mats[i]) 
			track_contact = TRUE;
        i++;
    }

    /* Need to get the wheel joint but dont know which body is the wheel    */
    b1 = McdModelGetBody(c->pair->model1);
    b2 = McdModelGetBody(c->pair->model2);

    if(b1) //Since ground doesn't have a body, only b1 or b2 should be non zero
    {
        wheel = (MdtSuspendedWheel *)MdtBodyGetUserData(b1);
        radius = McdSphereGetRadius(McdModelGetGeometry(c->pair->model1));
        //groundTM = McdModelGetGeometryTransformPtr(c->cm2);
        groundTM = (MeMatrix4 *)McdModelGetTransformPtr(c->pair->model2);
        epsilon = MdtWorldGetEpsilon(MdtBodyGetWorld(b1));
        wheel_body = b1;
    }
    else if(b2)
    {
        wheel = (MdtSuspendedWheel *)MdtBodyGetUserData(b2);
        radius = McdSphereGetRadius(McdModelGetGeometry(c->pair->model2));
        groundTM = (MeMatrix4 *)McdModelGetTransformPtr(c->pair->model1);
        epsilon = MdtWorldGetEpsilon(MdtBodyGetWorld(b2));
        wheel_body = b2;
    }
    else
    {
        return 1; //Hmmm we shouldn't really get this case
    }

    if(!wheel) return 1;

    McdContact *cp1 = &(c->contacts[0]);

	if(cp == cp1) {
		//first is not a face
		if(cp->dims>>8 != 2) {
			int f = 0;
		}
	} else {
		// this is a face but first is not
		if(cp->dims>>8 == 2 && cp1->dims>>8 != 2) {
			int f = 0;
		}
	}

    /* Determine whether the contact is necessary by comparing it with previous contacts    */
#if !USE_TRI_LIST_COLL
    MeVector3 vp[3];
    if (groundTM) {
        MeVector3 vr;
        for (int i = 0; i < 3; i++) {
            RwV3d *v = sector->vertices+poly->vertIndex[i];
            vr[0] = v->x;
            vr[1] = v->y;
            vr[2] = v->z;
            RenderToPhysics(vp[i],vr,*groundTM);
        }
    } else {
        for (int i = 0; i < 3; i++) {
            RwV3d *v = sector->vertices+poly->vertIndex[i];
            vp[i][0] = v->x;
            vp[i][1] = v->y;
            vp[i][2] = v->z;
        }
    }
    MeVector3 e1 = {vp[1][0]-vp[0][0],vp[1][1]-vp[0][1],vp[1][2]-vp[0][2]};
    MeVector3 e2 = {vp[2][0]-vp[0][0],vp[2][1]-vp[0][1],vp[2][2]-vp[0][2]};
    _Cross(e1,e2,normal);
    MeReal d2 = _Dot(normal,normal);
    if (d2 == 0.0) {
        return 0;
    }
    d2 = MeRecipSqrt(d2);
    normal[0] *= d2;
    normal[1] *= d2;
    normal[2] *= d2;
    MdtBodyGetPosition(wheel->wheel_body,pos);
    pos[0] -= radius*normal[0];
    pos[1] -= radius*normal[1];
    pos[2] -= radius*normal[2];
    MeReal penetration = (vp[0][0]-pos[0])*normal[0]+
                         (vp[0][1]-pos[1])*normal[1]+
                         (vp[0][2]-pos[2])*normal[2];
    if (cp != cp1 && _Dot(normal,cp1->normal) > 0.999f) {
        // Straightened normal is too similar to cp1's.  We can kill it.
        return 0;
    }

#else
	MeReal penetration;
	MdtContactGetNormal(contact,normal);
	MdtContactGetPosition(contact,pos);
	Px = penetration = MdtContactGetPenetration(contact);

    if (cp != cp1) {
		if( _Dot(normal,cp1->normal) > 0.5f) {
        // Straightened normal is too similar to cp1's.  We can kill it.
			return 0;
		}
    }

#endif

#if 1
    if (cp == cp1 || ((cp->dims>>8) == 2)) {

        cp->normal[0] = normal[0];
        cp->normal[1] = normal[1];
        cp->normal[2] = normal[2];

        MdtContactSetNormal(contact,normal[0],normal[1],normal[2]);
        MdtContactSetPosition(contact,pos[0],pos[1],pos[2]);
        MdtContactSetPenetration(contact,penetration);
    }
#endif

    // We're keeping this one.  Set wheel parameters.
    MdtCarWheelGetHingeAxis(wheel->GetUserData()->wheel_joint, hinge_axis);
    MdtContactGetNormal(contact,normal);
    _Cross (hinge_axis,normal, a);
    _MakeUnitVector( a);

    /* Set the direction vector for a 2D friction contact. This is the primary direction and
        is the direction the tyre is rolling. The secondary direction is at right angles to
        this.   */
    MdtContactSetDirection(contact,a[0],a[1],a[2]);


    /* Set up contact depending on whether it is on the track on not    */
    TYRE_PARAMS *tp;
    if(track_contact)
        tp = &wheel->GetUserData()->car_data->tyre_track_props;
    else
        tp = &wheel->GetUserData()->car_data->tyre_grass_props;

    /* In this example the slip parameter is proportional to the tangential velocity of
        the wheels circumferance.   */
    const MeReal slip = tp->min_slip_vel + tp->slip_rate*MeFabs(getWheelTangentialSpeed(wheel));
    MdtContactParamsID cprms = MdtContactGetParams(contact);

    /* Set the slip parameter but limit to a maximum value  */

    MdtContactParamsSetPrimarySlip(cprms, slip>tp->max_roll_slip_vel?tp->max_roll_slip_vel:slip);
    MdtContactParamsSetSecondarySlip(cprms, slip>tp->max_lat_slip_vel?tp->max_lat_slip_vel:slip);


    /* Calculate a maximum limiting friction force. In this example it is not in fact a function
        of normal force but just dependent on the wheel camber relative to the ground. Normal
        force can be included here by extracting the value from the last frame  */
//#ifdef PS2
#if 1
    const MeReal dotmag = MeFabs(_Dot(normal, hinge_axis)); // cos(angle) between wheel axis and ground normal
    MeReal friction =   tp->max_friction;
    if( dotmag> tp->max_friction_zone) friction *= (tp->min_friction_cutoff-dotmag)/(tp->min_friction_cutoff-tp->max_friction_zone);
    if(friction<tp->min_friction) friction =tp->min_friction;

    /* Set the secondary (Lateral) limiting friction    */
    MdtContactParamsSetSecondaryFriction(cprms, friction);

    /* Primary direction value is left as maximum   */
    MdtContactParamsSetPrimaryFriction(cprms, tp->max_friction);
#else
    MeVector3 force;
    MeReal friction;
    MeReal dotmag = MeFabs(_Dot(normal, hinge_axis));


  MdtConstraintGetForce( (MdtBaseConstraint*)contact, 0, force);

    friction = 4.0f*_Dot(force,normal);
    friction = LIMITS(tp->min_friction,friction,tp->max_friction);
    /* Set the Primary (rolling) limiting   friction    */
    MdtContactParamsSetPrimaryFriction(cprms, friction);
    if( dotmag> tp->max_friction_zone) friction *= (tp->min_friction_cutoff-dotmag)/(tp->min_friction_cutoff-tp->max_friction_zone);
    if(friction<tp->min_friction) friction =tp->min_friction;
    /* Set the secondary (Lateral) limiting friction    */
    MdtContactParamsSetSecondaryFriction(cprms, friction);

#endif
    /* Seet some user data for determining later what the tyre is in contact with   */
    wheel->GetUserData()->contact = contact;
    wheel->GetUserData()->on_track = track_contact;

  DefaultGroundCB(c, cp, contact);

num_contacts++;
/*
#if 0
    {
       /* Draw in the contact  /
        MeVector3 end1, end2, vel;
        MeReal v1, v2, roll_vel, slip;

        end1[0] = cp->position[0];
        end1[1] = cp->position[1];
        end1[2] = cp->position[2];

        end2[0] = end1[0] + cp->normal[0]*5;
        end2[1] = end1[1] + cp->normal[1]*5;
        end2[2] = end1[2] + cp->normal[2]*5;

        DebugLine(end1,end2,0,255,0);

        MdtBodyGetLinearVelocity(wheel_body, vel);
        v1 = _Dot(hinge_axis, vel);
        roll_vel = _Dot(a, vel);
        slip = v1/MAX(1.0f, MeFabs(roll_vel));
        end2[0] = end1[0] + hinge_axis[0]*5*slip;
        end2[1] = end1[1] + hinge_axis[1]*5*slip;


        DebugLine(end1,end2,0,0,255);

        MdtBodyGetAngularVelocity(wheel_body, vel);
        v1 = _Dot(vel, hinge_axis)*radius;
        v2 = MAX(1.0f, MIN(MeFabs(v1), MeFabs(roll_vel)));
        slip = (roll_vel - v1) / v2;

        end2[0] = end1[0] - a[0]*5*slip;
        end2[1] = end1[1] - a[1]*5*slip;
        DebugLine(end1,end2,255,0,0);

    }
#endif
	*/
    /* Draw tyre slip angles    */
    return 1;
}
//#endif

/***********************************************************************************************
*
*   This is essentially the 'Tyre model'. It varies the contact parameters in relation to
*   the contact conditions. It also determines whether the contact is necessary in the
*   first place.
*
************************************************************************************************/
MeBool MEAPI SimpleWheelGroundCB(McdIntersectResult* c, McdContact* cp,
                         MdtContactID contact) {

//	McdContact *cp =
    MeVector3 z_axis,hinge_axis, a, normal;
    MdtSuspendedWheel *wheel;
    MdtBodyID b1, b2, chassis_body;
    RpWorldSector *sector = (RpWorldSector *)cp->element1.ptr;
    MeReal epsilon;
    MeMatrix4 *groundTM, *chassisTM;
    int triIndex = cp->element2.tag;
    MeReal radius;
    RwBool track_contact = FALSE;
    int i=0;
    MeMatrix4 *chassis_mcd_mtx;
    MeVector4 quat = {1,0,0,0};
    MeMatrix3 rot;

    if(cp->separation > 0.01f)
    {
        return 0;
    }

	RpPolygon *poly = (RpPolygon *)cp->element2.ptr;//sector->polygons+triIndex;

    /* Determine if contact is with track material or not   */
    while(i<NUM_TRACK_TEXTURES && !track_contact)
    {
        if(poly->matIndex == track_mats[i]) 
			track_contact = TRUE;
        i++;
    }

    /* Need to get the wheel joint but dont know which body is the wheel    */
    b1 = McdModelGetBody(c->pair->model1);
    b2 = McdModelGetBody(c->pair->model2);


    if(b1) //Since ground doesn't have a body, only b1 or b2 should be non zero
    {
        wheel = (MdtSuspendedWheel *)McdModelGetUserData(c->pair->model1);
        radius = McdSphereGetRadius(McdModelGetGeometry(c->pair->model1));
        groundTM = (MeMatrix4 *)McdModelGetTransformPtr(c->pair->model2);
        epsilon = MdtWorldGetEpsilon(MdtBodyGetWorld(b1));
        chassis_body = b1;
        chassis_mcd_mtx = (MeMatrix4 *)McdModelGetTransformPtr( c->pair->model1 );

        /* Store mcd model in Mdt Contact user data to ease switching between lods  */
        MdtContactSetUserData(contact, (void*)c->pair->model1); }
    else if(b2)
    {
        wheel = (MdtSuspendedWheel *)McdModelGetUserData(c->pair->model2);
        radius = McdSphereGetRadius(McdModelGetGeometry(c->pair->model2));
        groundTM = (MeMatrix4 *)McdModelGetTransformPtr(c->pair->model1);
        epsilon = MdtWorldGetEpsilon(MdtBodyGetWorld(b2));
        chassis_body = b2;
        chassis_mcd_mtx = (MeMatrix4 *)McdModelGetTransformPtr( c->pair->model2 );

        /* Store mcd model in Mdt Contact user data to ease switching between lods  */
        MdtContactSetUserData(contact, (void*)c->pair->model2);
    }
    else
    {
        return 1; //Hmmm we shouldn't really get this case
    }

    if(!wheel) return 1;

    McdContact *cp1 = &(c->contacts[0]);

    /* Determine whether the contact is necessary by comparing it with previous contacts    */
#if !USE_TRI_LIST_COLL
    MeVector3 vp[3];
    if (groundTM) {
        MeVector3 vr;
        for (int i = 0; i < 3; i++) {
            RwV3d *v = sector->vertices+poly->vertIndex[i];
            vr[0] = v->x;
            vr[1] = v->y;
            vr[2] = v->z;
            RenderToPhysics(vp[i],vr,*groundTM);
        }
    } else {
        for (int i = 0; i < 3; i++) {
            RwV3d *v = sector->vertices+poly->vertIndex[i];
            vp[i][0] = v->x;
            vp[i][1] = v->y;
            vp[i][2] = v->z;
        }
    }
    MeVector3 e1 = {vp[1][0]-vp[0][0],vp[1][1]-vp[0][1],vp[1][2]-vp[0][2]};
    MeVector3 e2 = {vp[2][0]-vp[0][0],vp[2][1]-vp[0][1],vp[2][2]-vp[0][2]};
    _Cross(e1,e2,normal);
    MeReal d2 = _Dot(normal,normal);
    if (d2 == 0.0) {
        return 0;
    }
    d2 = MeRecipSqrt(d2);
    normal[0] *= d2;
    normal[1] *= d2;
    normal[2] *= d2;
    MeVector3 pos;
    //MdtBodyGetPosition(wheel->wheel_body,pos);
    pos[0] = (*chassis_mcd_mtx)[3][0] - radius*normal[0];
    pos[1] = (*chassis_mcd_mtx)[3][1] - radius*normal[1];
    pos[2] = (*chassis_mcd_mtx)[3][2] - radius*normal[2];
    MeReal penetration = (vp[0][0]-pos[0])*normal[0]+
                         (vp[0][1]-pos[1])*normal[1]+
                         (vp[0][2]-pos[2])*normal[2];
    if (cp != cp1 && _Dot(normal,cp1->normal) > 0.8f) {
        // Straightened normal is too similar to cp1's.  We can kill it.
        return 0;
    }
#else
    MeVector3 pos;
	MeReal penetration;
	MdtContactGetNormal(contact,normal);
	MdtContactGetPosition(contact,pos);
	penetration = MdtContactGetPenetration(contact);
    if (cp != cp1 && _Dot(normal,cp1->normal) > 0.8f) {
        // Straightened normal is too similar to cp1's.  We can kill it.
        return 0;
    }

#endif

#if 1
    if (cp == cp1 || ((cp->dims>>8) == 2)) {

        cp->normal[0] = normal[0];
        cp->normal[1] = normal[1];
        cp->normal[2] = normal[2];

        MdtContactSetNormal(contact,normal[0],normal[1],normal[2]);
        MdtContactSetPosition(contact,pos[0],pos[1],pos[2]);
        MdtContactSetPenetration(contact,penetration);
    }
#endif


    MdtContactParamsID cprms = MdtContactGetParams(contact);
    // We're keeping this one.  Set wheel parameters.
    chassisTM = (MeMatrix4 *)MdtBodyGetTransformPtr(chassis_body);
    MdtContactGetNormal(contact,normal);
//  x_axis[0] = (*chassisTM)[0][0]; x_axis[1] = (*chassisTM)[0][1]; x_axis[2] = (*chassisTM)[0][2];
//  z_axis[0] = (*chassisTM)[2][0]; z_axis[1] = (*chassisTM)[2][1]; z_axis[2] = (*chassisTM)[2][2];
    z_axis[0] = 0;  z_axis[1] = 0;  z_axis[2] = 1;
    hinge_axis[0] = (*chassisTM)[1][0]; hinge_axis[1] = (*chassisTM)[1][1]; hinge_axis[2] = (*chassisTM)[1][2];
//  _Cross (normal,x_axis, hinge_axis);

    /* Rotate the hinge axis around the z_axis  */
    MeQuaternionFiniteRotation(quat, z_axis, wheel->act_steer_angle);
    MeQuaternionToR(rot, quat);
    MeMatrix3MultiplyVector(hinge_axis, rot, hinge_axis);

    _Cross (hinge_axis,normal, a);
    _MakeUnitVector( a);
    MdtContactSetDirection(contact,a[0],a[1],a[2]);

    MeVector3 vel;
    MeReal v_mag;
    MdtBodyGetVelocityAtPoint(chassis_body, pos, vel);
    v_mag = MeFabs(MeVector3Dot(vel, a));

        /* Set up contact depending on whether it is on the track on not    */
    TYRE_PARAMS *tp;
    if(track_contact)
        tp = &wheel->GetUserData()->car_data->tyre_track_props;
    else
        tp = &wheel->GetUserData()->car_data->tyre_grass_props;

    /* In this example the slip parameter is proportional to the tangential velocity of
        the wheels circumferance.   */
    const MeReal slip = tp->min_slip_vel + tp->slip_rate*v_mag;

    MdtContactParamsSetPrimarySlip(cprms, slip>tp->max_roll_slip_vel?tp->max_roll_slip_vel:slip);

    MdtContactParamsSetSecondarySlip(cprms, slip>tp->max_lat_slip_vel?tp->max_lat_slip_vel:slip);

    const MeReal dotmag = MeFabs(_Dot(normal, hinge_axis)); // cos(angle) between wheel axis and ground normal
    MeReal friction =   tp->max_friction;
    if( dotmag> tp->max_friction_zone) friction *= (tp->min_friction_cutoff-dotmag)/(tp->min_friction_cutoff-tp->max_friction_zone);
    if(friction<tp->min_friction) friction =tp->min_friction;

    /* Set the secondary (Lateral) limiting friction    */
    MdtContactParamsSetSecondaryFriction(cprms, friction);

    /* Primary direction value is left as maximum   */
    MdtContactParamsSetPrimaryFriction(cprms, 0.0f);

    MdtContactParamsSetSoftness(cprms, 0.0005f);

    wheel->GetUserData()->contact = contact;
    wheel->GetUserData()->on_track = track_contact;

#if 0
    {
        /* Draw in the contact  */
        MeVector3 end1, end2, vel;
        MeReal v1, v2, roll_vel, slip;

        end1[0] = cp->position[0];
        end1[1] = cp->position[1];
        end1[2] = cp->position[2];

        end2[0] = end1[0] + cp->normal[0]*5;
        end2[1] = end1[1] + cp->normal[1]*5;
        end2[2] = end1[2] + cp->normal[2]*5;

        DebugLine(end1,end2,0,255,0);

        MdtBodyGetVelocityAtPoint(chassis_body, cp->position, vel);
        v1 = _Dot(hinge_axis, vel);
        roll_vel = _Dot(a, vel);
        slip = v1/MAX(1.0f, MeFabs(roll_vel));
        end2[0] = end1[0] + hinge_axis[0]*5*slip;
        end2[1] = end1[1] + hinge_axis[1]*5*slip;

        DebugLine(end1,end2,0,0,255);

        v1 = wheel->ang_vel*radius;
        v2 = MAX(1.0f, MIN(MeFabs(v1), MeFabs(roll_vel)));
        slip = (roll_vel - v1) / v2;

        end2[0] = end1[0] + a[0]*5*slip;
        end2[1] = end1[1] + a[1]*5*slip;
        DebugLine(end1,end2,255,0,0);

    }
#endif

  DefaultGroundCB(c, cp, contact);

    return 1;
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
Car::Car() {
//  Init(game_vars.car_data_file);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
Car::~Car() {
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Car::CreatePhysics() {
    CreateMdtCar(mdt_world);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Car::Init(char *fname) {


    if(*fname)
    {
        RwChar* temp;
        temp = RsPathnameCreate(RWSTRING(fname));
        LoadCarDataFromFile(temp);
    }
    else
    {
        UseCompiledCarData();
    }

    InitMdtCar(mdt_world, gravity, &cardat);

    UpdateShapeMatrices();

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Car::Update() {

    MdtCar::Update();

    UpdateShapeMatrices();

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Car::SetPosition(MeReal x, MeReal y) {

    MdtCar::SetPosition(x,y);

    UpdateShapeMatrices();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void Car::SetPosition(MeReal x, MeReal y, MeReal z) {

    MdtCar::SetPosition(x,y,z);

    UpdateShapeMatrices();
}

//MdtBody *temp_body;
//McdModel *temp_body_coll;
//MeMatrix4 mmm;
//MeMatrix4 temp_body_shape_offset = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Car::UpdateShapeMatrices()
{
    int i;
    const MeMatrix4 *me_mtx;
    MeMatrix4 rot = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, temp_mtx;
    MeVector4 offset_w = {0,0,0,1},quat;
    WHEEL_USERDATA *wheel_ud;
    MeReal penetration;//, rad;
    MeVector3 y_axis, z_axis;

    for(i=0;i<5;i++)
    {
        if(level_of_detail == LOD1)
        {
            me_mtx = GetTMatrix(i);
            if(i == CHASSIS_CMPT)
                MdtConvertVector(GetBody(i), &shape_offsets[i][3][0], NULL, offset_w);

        }

        else if(level_of_detail == LOD2)
        {
            me_mtx = GetTMatrix(CHASSIS_CMPT);

            /* Chassis lateral axis */
//          y_axis[0] = (*me_mtx)[0][1]; y_axis[1] = (*me_mtx)[1][1]; y_axis[2] = (*me_mtx)[2][1];
            /* Reset graphic rotation quaternion    */
            quat[0] = 1.0f; quat[1] = quat[2] = quat[3] = 0.0f;

            MdtConvertVector(GetBody(CHASSIS_CMPT), &shape_offsets[i][3][0], NULL, offset_w);

            if(i < CHASSIS_CMPT)
            {
                wheel_ud = (WHEEL_USERDATA *)(wheel_joint[i]->GetUserData());


                if(wheel_ud->contact)
                {
                    /* Artifically move wheel graphic up and down   */
                    penetration = MdtContactGetPenetration(wheel_ud->contact);

                    /* Chassis Vertical Axis    */
                    z_axis[0] = (*me_mtx)[0][2]; z_axis[1] = (*me_mtx)[1][2]; z_axis[2] = (*me_mtx)[2][2];

                    offset_w[0] += (penetration)*z_axis[0];
                    offset_w[1] += (penetration)*z_axis[1];
                    offset_w[2] += (penetration)*z_axis[2];

                }

                /* Artificially roll wheel graphic  */
                y_axis[0] = 0; y_axis[1] = 1; y_axis[2] = 0;
                z_axis[0] = 0; z_axis[1] = 0; z_axis[2] = 1;
                MeQuaternionFiniteRotation(quat, y_axis, GetWheelRotAngle(i));

                /* Artificially steer front wheel graphic   */
                //if(i < REAR_L_CMPT)
                //{
                    MeQuaternionFiniteRotation(quat, z_axis, GetActualSteerAngle(i));
                //}

                /* Convert quaternion to matrix and set pointer to temporarily point to it  */
                MeQuaternionToTM( rot, quat);
                MeMatrix4MultiplyMatrix(temp_mtx, rot, *me_mtx);
                me_mtx = (const MeMatrix4 *)&temp_mtx;

            }
        }

        GraphicsTMatrix[i][0] = (*me_mtx)[0][0];
        GraphicsTMatrix[i][1] = (*me_mtx)[0][2];
        GraphicsTMatrix[i][2] =-(*me_mtx)[0][1];
        GraphicsTMatrix[i][3] = 0;
        GraphicsTMatrix[i][4] = (*me_mtx)[2][0];
        GraphicsTMatrix[i][5] = (*me_mtx)[2][2];
        GraphicsTMatrix[i][6] =-(*me_mtx)[2][1];
        GraphicsTMatrix[i][7] = 0;
        GraphicsTMatrix[i][8] =-(*me_mtx)[1][0];
        GraphicsTMatrix[i][9] =-(*me_mtx)[1][2];
        GraphicsTMatrix[i][10]= (*me_mtx)[1][1];
        GraphicsTMatrix[i][11]= 0;
        GraphicsTMatrix[i][12]=  (*me_mtx)[3][0]+offset_w[0];
        GraphicsTMatrix[i][13]=  (*me_mtx)[3][2]+offset_w[2];
        GraphicsTMatrix[i][14]= -(*me_mtx)[3][1]-offset_w[1];
        GraphicsTMatrix[i][15]= 1;

    }
}

int MEAPI DefaultGroundCB(McdIntersectResult* c, McdContact* cp,
                         MdtContactID contact) {
#if 1 

    MeVector3 end1, end2;
	int red = 0, green = 0, blue = 0;

 //   num_contacts++;

	switch(cp->dims>>8) {
		case 0:	red = 255;		break; //vertex
		case 1:	blue = 255;		break; //edge
		case 2:	green = 255;	break; //face
		default:				break;
	}
    /* Draw in the contact  */

    end1[0] = cp->position[0];
    end1[1] = cp->position[1];
    end1[2] = cp->position[2];

    end2[0] = end1[0] - cp->normal[0]*cp->separation*500;
    end2[1] = end1[1] - cp->normal[1]*cp->separation*500;
    end2[2] = end1[2] - cp->normal[2]*cp->separation*500;

    DebugLine(end1,end2,red,green,blue);

//    end2[0] = end1[0] + cp->normal[0]*cp->separation*500;
//    end2[1] = end1[1] + cp->normal[1]*cp->separation*500;
//    end2[2] = end1[2] + cp->normal[2]*cp->separation*500;

//    DebugLine(end1,end2,0,0,255);

//    end1[0] -= cp->normal[0]*cp->separation*2;
//    end1[1] -= cp->normal[1]*cp->separation*2;
//    end1[2] -= cp->normal[2]*cp->separation*2;
//    end2[0] = end1[0] + contact->bclContact.direction[0]*50;
//    end2[1] = end1[1] + contact->bclContact.direction[1]*50;
//    end2[2] = end1[2] + contact->bclContact.direction[2]*50;

//    DebugLine(end1,end2,255,0,0);

#endif
    return 1;
}

static MeMatrix4 mcd_m = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

#if USE_TRI_LIST_COLL

typedef struct 
{
    McdUserTriangle *list;
    int count;
    int maxCount;
} TriangleAccumulator;
/***********************************************************************************************
*
*
*
************************************************************************************************/
RpCollisionTriangle *rpSphereWorldCb(RpIntersection *intersect,
									 RpWorldSector *sector,
									 RpCollisionTriangle *collTriangle,
									 RwReal distance,
									 void *data) {
    TriangleAccumulator *acc = (TriangleAccumulator *)data;
    int count = acc->count;
	McdUserTriangle *tri = acc->list + count;

	if(count < acc->maxCount)
	{
		/* Vertices	*/

		//test for double precision build
    	MeVector3 edge1, edge2;
		vertex_memory[count][0][0] = collTriangle->vertices[0]->x; 
		vertex_memory[count][0][1] = collTriangle->vertices[0]->y; 
		vertex_memory[count][0][2] = collTriangle->vertices[0]->z; 

		vertex_memory[count][1][0] = collTriangle->vertices[1]->x; 
		vertex_memory[count][1][1] = collTriangle->vertices[1]->y; 
		vertex_memory[count][1][2] = collTriangle->vertices[1]->z; 

		vertex_memory[count][2][0] = collTriangle->vertices[2]->x; 
		vertex_memory[count][2][1] = collTriangle->vertices[2]->y; 
		vertex_memory[count][2][2] = collTriangle->vertices[2]->z; 

		tri->vertices[0] = &vertex_memory[count][0];
		tri->vertices[1] = &vertex_memory[count][1];
		tri->vertices[2] = &vertex_memory[count][2];

		/* Edges - Not needed if you are storing the poly Normals	*/
		MeVector3Subtract(edge1, *tri->vertices[1], *tri->vertices[0]);
		MeVector3Subtract(edge2, *tri->vertices[2], *tri->vertices[1]);

		/* Normal -- Must be related to vertices / edges using RH rule	*/
		tri->normal = &normal_memory[count];
		MeVector3Cross(*tri->normal, edge1, edge2);
		MeVector3Normalize(*tri->normal);

		/* Store Userdata relating to info about RW polygon	*/
		//tri->element1.ptr = (void*) sector;
        //tri->element2.tag = collTriangle->index;
		tri->triangleData.ptr = (void *)(sector->polygons + collTriangle->index);
        tri->flags = (McdTriangleFlags)(kMcdTriangleUseSmallestPenetration | kMcdTriangleUseEdges);

		/* Test	*/
	    /* {
		    const RwV3d *rwOrigin = RpWorldGetOrigin(RW_World);
			MeVector3 v0,v1,v2;
			v0[0] = collTriangle->vertices[0]->x - rwOrigin->x;
			v0[1] = -(collTriangle->vertices[0]->z - rwOrigin->z);
			v0[2] = collTriangle->vertices[0]->y - rwOrigin->y + 0.1f;
			v1[0] = collTriangle->vertices[1]->x - rwOrigin->x;
			v1[1] = -(collTriangle->vertices[1]->z - rwOrigin->z);
			v1[2] = collTriangle->vertices[1]->y - rwOrigin->y + 0.1f;
			v2[0] = collTriangle->vertices[2]->x - rwOrigin->x;
			v2[1] = -(collTriangle->vertices[2]->z - rwOrigin->z);
			v2[2] = collTriangle->vertices[2]->y - rwOrigin->y + 0.1f;
	        DebugLine(v0,v1,255,255,255);
		    DebugLine(v1,v2,255,255,255);
			DebugLine(v2,v0,255,255,255);
		}*/
		acc->count++;
	}
    return collTriangle;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static int MEAPI RwBSPTriListGenerator(McdModelPair* pair, 
                                       McdUserTriangle *list,
                                       MeVector3 pos, 
                                       MeReal rad, 
                                       int max)
{
    TriangleAccumulator acc;
	RpIntersection intersect;
 	McdTriangleList *tl = (McdTriangleList *)pair->model2->mInstance.mGeometry;

    acc.count = 0;
    acc.maxCount = max;
    acc.list = list;

	intersect.type = rpINTERSECTSPHERE;
	intersect.t.sphere.center.x = (RwReal)pos[0];
	intersect.t.sphere.center.y = (RwReal)pos[1];
	intersect.t.sphere.center.z = (RwReal)pos[2];
    intersect.t.sphere.radius = (RwReal)rad;

	RpCollisionWorldForAllIntersections(RW_World,&intersect,rpSphereWorldCb,(void *)&acc);

	return acc.count;
}
#endif
/***********************************************************************************************
*
*
*
************************************************************************************************/
MeALIGNDATA(MeMatrix4, worldPositionMatrix, 16) =
{ 1,0,0,0, 
  0,0,1,0, 
  0,-1,0,0, 
  0,0,0,1};


void InitialiseCarSim(RpWorld *world_bsp) {
//	const RwBBox *bbox = RpWorldGetBBox(world_bsp);
//	MeReal x_dim, y_dim, z_dim;

//	x_dim = bbox->sup.x - bbox->inf.x;
//	y_dim = bbox->sup.y - bbox->inf.y;
//	z_dim = bbox->sup.z - bbox->inf.z;

//  mdt_mem = malloc(m_size);

//  mdt_mem = MeMemoryAPI.create(m_size);

    /* Set up my error logging functions    */
#if !DEBUG_LOGGING
    MeSetInfoHandler(OutputMessage);
    MeSetWarningHandler(OutputMessage);
    MeSetDebugHandler(OutputMessage);
    MeSetFatalErrorHandler(OutputMessage);
#endif

    /* Initialise dynamics environment/world    */

//  MdtWorldInit(&mdt_world, mdt_mem, m_size);
    mdt_world = MdtWorldCreate(MAX_CARS*5, MAX_CARS*20, 1, 1);

    {
        const MdtKeaDebugDataRequest ddr =
            {
                0, "keaInputW.txt",
                0, "keaInputR.txt",
                0, "keaInterW.txt",
                1, "keaOutput.txt",
                1,
                0,
                0
            };

        MdtWorldSetKeaDebugRequest(mdt_world,ddr);
    }

    MdtWorldSetEpsilon (mdt_world,0.00001f);
    MdtWorldSetGamma(mdt_world,0.3f);
    MdtWorldSetGravity(mdt_world, gravity[0], gravity[1], gravity[2]);

//  MdtWorldSetDEMMode(mdt_world, MdtDEMModePartitionNoAutoDisable);
    MdtWorldSetAutoDisable(mdt_world, 0);

    MdtBclContactParams *params;

    /* Initialise Mcd system */
    /* #types = primitives + rwbsp */

    mcdframe = McdInit( 0, MAX_CARS*5+1 , 0, 1); //maxmodels

    McdPrimitivesRegisterTypes(mcdframe);
    McdPrimitivesRegisterInteractions(mcdframe);

    /* Create collision space, setting max models & max overlapping pairs   */
    mcd_space = McdSpaceAxisSortCreate(mcdframe, McdAllAxes,MAX_CARS*5+1,MAX_CARS*20);

	cdHandler = MstBridgeCreate(mcdframe,4); //maxMaterials
    MstSetWorldHandlers(mdt_world);

    // RW BSP collision model

	RW_World = world_bsp;

    MeVector3 min = {-20000,-20000,-20000},max = {20000,20000,20000};
    
    mcd_world = McdModelCreate( 
       McdTriangleListCreate(mcdframe, min,max,TRI_LIST_SIZE, RwBSPTriListGenerator));
	
    McdSpaceInsertModel(mcd_space, mcd_world);
	McdModelSetBody(mcd_world, (MdtBody *)NULL );

    /* Set up the transformation between graphics axes and dynamics if they are not coincident  */
    /* Matrix must be static or declared outside function since it needs to be preserved    */
    const RwV3d *rwOrigin = RpWorldGetOrigin(world_bsp);

    MeMatrix4TMSetPosition(worldPositionMatrix,-rwOrigin->x, rwOrigin->z, -rwOrigin->y);
    McdModelSetTransformPtr(mcd_world, worldPositionMatrix);

    McdSpaceUpdateAll(mcd_space); // need to call before freeze
    McdSpaceFreezeModel(mcd_world); // world doesn't move

    /* Contact material properties  */
    mat_track = MstBridgeGetDefaultMaterial();
    mat_lod1wheel = MstBridgeGetNewMaterial(cdHandler);
    mat_lod2wheel = MstBridgeGetNewMaterial(cdHandler);
    mat_chassis = MstBridgeGetNewMaterial(cdHandler);

    /* Wheel / Track    Params that don't get set in callback function */
    params = MstBridgeGetContactParams(cdHandler,mat_lod1wheel,mat_track);
    params->type = MdtContactTypeFriction2D;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    MstBridgeSetPerContactCB(cdHandler,mat_lod1wheel, mat_track, WheelGroundCB); // 'Tyre model'

    params = MstBridgeGetContactParams(cdHandler,mat_lod2wheel,mat_track);
    params->type = MdtContactTypeFriction2D;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    MstBridgeSetPerContactCB(cdHandler,mat_lod2wheel, mat_track, SimpleWheelGroundCB); // 'Tyre model'

    /* Wheel / Wheel    */
    params = MstBridgeGetContactParams(cdHandler,mat_lod1wheel,mat_lod1wheel);
    params->type = MdtContactTypeFriction2D;
    params->friction1 = 1000.0f;    //box friction max force
    params->friction2 = 1000.0f;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    params->slip1 = 0.1f;           params->options |= MdtBclContactOptionSlip1;
    params->slip2 = 0.1f;           params->options |= MdtBclContactOptionSlip2;

    params = MstBridgeGetContactParams(cdHandler,mat_lod1wheel,mat_lod2wheel);
    params->type = MdtContactTypeFriction2D;
    params->friction1 = 1000.0f;    //box friction max force
    params->friction2 = 1000.0f;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    params->slip1 = 0.1f;           params->options |= MdtBclContactOptionSlip1;
    params->slip2 = 0.1f;           params->options |= MdtBclContactOptionSlip2;

    params = MstBridgeGetContactParams(cdHandler,mat_lod2wheel,mat_lod2wheel);
    params->type = MdtContactTypeFriction2D;
    params->friction1 = 1000.0f;    //box friction max force
    params->friction2 = 1000.0f;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    params->slip1 = 0.1f;           params->options |= MdtBclContactOptionSlip1;
    params->slip2 = 0.1f;           params->options |= MdtBclContactOptionSlip2;

    /* Chassis / Track  */
    params = MstBridgeGetContactParams(cdHandler,mat_chassis,mat_track);
    params->type = MdtContactTypeFriction2D;
    params->friction1 = 1000.0f;    //box friction max force
    params->friction2 = 1000.0f;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    params->slip1 = 0.1f;           params->options |= MdtBclContactOptionSlip1;
    params->slip2 = 0.1f;           params->options |= MdtBclContactOptionSlip2;
    MstBridgeSetPerContactCB(cdHandler,mat_chassis, mat_track, DefaultGroundCB);

    /* Chassis / Wheel  */
    params = MstBridgeGetContactParams(cdHandler,mat_chassis,mat_lod1wheel);
    params->type = MdtContactTypeFrictionZero; //2D;
    params->friction1 = 1000.0f;    //box friction max force
    params->friction2 = 1000.0f;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    params->slip1 = 0.1f;           params->options |= MdtBclContactOptionSlip1;
    params->slip2 = 0.1f;           params->options |= MdtBclContactOptionSlip2;

    params = MstBridgeGetContactParams(cdHandler,mat_chassis,mat_lod2wheel);
    params->type = MdtContactTypeFrictionZero; //2D;
    params->friction1 = 1000.0f;    //box friction max force
    params->friction2 = 1000.0f;
    params->softness = 0.0001f;     params->options |= MdtBclContactOptionSoft;
    params->slip1 = 0.1f;           params->options |= MdtBclContactOptionSlip1;
    params->slip2 = 0.1f;           params->options |= MdtBclContactOptionSlip2;

    /* Set the RW BSP to have this type of material. This means that the callback function
    will be called for each contact between the wheels and the world. Most of the actual
    friction parameters are set in the callback depending on the texture of the contact polygon */
    //McdDtBridgeSetMaterialID( mcd_world, mat_track );
	McdModelSetMaterial(mcd_world, mat_track );

//    int defaultRequest = McdIntersectGetDefaultRequestID();
//    McdIntersectRequestSetContactMaxCount( defaultRequest, defaultRequest, 8 );
//    McdIntersectSetFaceNormalsFirst( defaultRequest, defaultRequest, 1 );
	McdFrameworkGetDefaultRequestPtr(mcdframe)->contactMaxCount = 8;
	McdFrameworkGetDefaultRequestPtr(mcdframe)->faceNormalsFirst = 1;

    McdSpaceBuild(mcd_space);

    /********************************************/
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void TerminateCarSim() {
//  free(mdt_mem);

//#if USE_CX_COLLISION
	MstBridgeDestroy(cdHandler);
//    McdDtBridgeTerm();
//    McdTerm();
//#endif

//  MdtDestroy();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/

#if DEBUG_LOGGING
/*
  With RW3, the PS2 prints all floats as '0.0000'. We convert
  them to 'int' 100ths, 
*/

static int asFixed(MeReal f)
{
    return (int) (f*100.0f);
}
#endif

void SysEvolve(MeReal delta_t)
{
//	if(!safe_time_on)
//	{
	//  static int temp = 0;
	#if COLLISION_BEFORE_EVOLVE
	  ResetPerformanceTimer();

	  num_contacts = 0;
		/* Update the collision space and generate appropriate contact constraints  */
		//McdPairHandlerUpdate();
		McdSpaceUpdate(mcd_space);
		handleContacts();

		collision_time = ReadPerformanceTimer();
	#endif

		ResetPerformanceTimer();

		/* Update the dynamics taking into account any contacts that were generated by the
			collision detection */
		MdtWorldSetGamma(mdt_world, delta_t*10.0f); //0.3f);

		if(!safe_time_on)
			MdtWorldStep(mdt_world, delta_t);
		else
			MdtWorldStepSafeTime(mdt_world, delta_t);

		dynamics_time = ReadPerformanceTimer();

#if DEBUG_LOGGING
        {
            struct pgCar
            {
                MeReal x,y,z;
                MeReal fvel,wspeed;
            };
            static struct pgCar *prevs = 0;

            const unsigned n = game_vars.num_cars;
            register unsigned i;

            if (prevs == 0)
            {
                prevs = (struct pgCar *) (*MeMemoryAPI.create)
                    (n*sizeof (struct pgCar));

                for (i = 0; i < n; i++)
                {
                    struct pgCar *const p = prevs+i;

                    p->x = p->y = p->z = 0.0f;
                    p->fvel = p->wspeed = 0.0f;
                }
            }

            for (i = 0; i < n; i++)
            {
                struct pgCar *const p = prevs+i;
                struct pgCar s;

                MdtCar *const c = drivers[i]->GetCar();

                s.x = c->GetBodyPosCmpt(0,0);
                s.y = c->GetBodyPosCmpt(0,1);
                s.z = c->GetBodyPosCmpt(0,2);

                s.fvel = c->GetBodyForwardVel();
                s.wspeed = c->GetWheelSpeed();

                /*
                  Typical range of value: z 0.20 ->19.95,
                  fvel 2.20->128.10, wspeed 4.5->335.90
                */

                if (s.z < -1.0f || s.z > 25.0f
                    || s.fvel > 200.0f || s.wspeed > 360.0f
                    || (prevs != 0
                        && (MeFabs(s.z-p->z) > 5.0f
                            || MeFabs(s.fvel-p->fvel > 50.0f)))
                )
                {
                    if (prevs != 0)
                        MeDebug(0,"%02d: previous pos (%d,%d,%d), fvel %d, wspeed %d\n",
                            i,asFixed(p->x),asFixed(p->y),asFixed(p->z),
                            asFixed(p->fvel),asFixed(p->wspeed)
                        );

                    MeDebug(0,"%02d: current pos (%d,%d,%d), fvel %d, wspeed %d\n",
                        i,asFixed(s.x),asFixed(s.y),asFixed(s.z),
                        asFixed(s.fvel),asFixed(s.wspeed)
                    );
                }

                if (prevs != 0)
                {
                    struct pgCar *const p = prevs+i;

                    (*p) = s;
                }
            }
        }
#endif
        

	#if (!COLLISION_BEFORE_EVOLVE)

		ResetPerformanceTimer();
		num_contacts = 0;
		/* Update the collision space and generate appropriate contact constraints  */
		McdSpaceUpdateAll(mcd_space);
		MstBridgeUpdateContacts(cdHandler, mcd_space, mdt_world);
		collision_time = ReadPerformanceTimer();

	#endif
//	}
//	else //safe_time_on
//	{
#if 0
		int i;
		MeBool pairOverflow;
		McdSpacePairIterator spaceIter;
		McdSafeTimeResult safeTimeResult;
		MdtKeaParameters params;
		MdtBodyID body;

		MdtWorldID w = mdt_world;
		MstBridgeID b = cdHandler;
        MeDictNode *node;

		//Dynamics
		ResetPerformanceTimer();
 
		/* call the partitioner */
		if(w->partitionParams.autoDisable)
		{
			MdtUpdatePartitions(&w->enabledBodyDict, &w->partitionOutput,
				MdtAutoDisableLastPartition, &w->partitionParams);
		}
		else
		{
			MdtUpdatePartitions(&w->enabledBodyDict, &w->partitionOutput, 0, 0);
		}

		/* Step Kea with all partition of bodies and constraints. */

        for(i = 0; i < w->partitionOutput.nPartitions; i++)
        {
            
            int maxRows,start,num_constraints;
        	MdtConstraintPartition singlePartitionInfo;

            MeReal partitionStepSize = MeMIN(delta_t,
                MdtPartitionGetSafeTime(&w->partitionOutput, i));
            
            maxRows = MdtPackPartition(&w->partitionOutput, i, partitionStepSize,
                &w->params, w->keabodyArray, w->keatmArray, &w->constraints);
            
            MdtWorldSetGamma(w, partitionStepSize*10.0f); //0.3f);
            
            MdtMakeKeaParameters(w, maxRows, partitionStepSize, &params);
            
            start = 0;
            num_constraints = w->constraints.num_constraints;
            singlePartitionInfo.constraintsSize = &num_constraints;
            singlePartitionInfo.constraintsStart = &start;
            singlePartitionInfo.totalConstraints = num_constraints;
            
            OldMdtKeaAddConstraintForces(w->constraints, 
                singlePartitionInfo,
                w->keabodyArray,
                w->keatmArray, 
                w->partitionOutput.bodyListInfo.bodiesSize[i], 
                params,
                1);
            
            
            MdtKeaIntegrateSystem(w->keabodyArray, w->keatmArray,
                w->partitionOutput.bodyListInfo.bodiesSize[i], params);
            
            MdtUnpackPartition(w->keabodyArray, w->keatmArray, &w->constraints,
                i, &w->partitionOutput);
        }

		/* Finally, reset forces on enabled bodies. */
		for (node = MeDictFirst(&w->enabledBodyDict); node != 0; node = MeDictNext(&w->enabledBodyDict,node))
		{
            body = (MdtBody *)MeDictNodeGet(node);
			MEASSERT(body->enabledTime > -1);
			MdtBodyResetForces(body);
			body->safeTime = MEINFINITY;
		}

		dynamics_time = ReadPerformanceTimer();

		//Collision
		ResetPerformanceTimer();
		num_contacts = 0;

		McdSpacePathUpdateAll(mcd_space, delta_t);

		McdSpaceEndChanges(mcd_space);
    
		/* Initialise iterator for this space. */

		McdSpacePairIteratorBegin(mcd_space, &spaceIter);    

		/* Keep getting pairs from farfield until we're done (ie. no overflow). */
		do
		{
			McdModelPairContainerReset(b->pairs);
			pairOverflow = McdSpaceGetPairs(mcd_space, &spaceIter, b->pairs);

			MstHandleTransitions(b->pairs,mcd_space,w,b);

			MstHandleCollisions(b->pairs, mcd_space, w, b);

	//        if(doSafeTimeCheck)
	//        {
				for( i = b->pairs->helloFirst ; i < b->pairs->stayingEnd ; ++i )
				{
					McdModelPair* pair = b->pairs->array[i];

					if (pair->model1->mInstance.mGeometry->mID != McdBoxGetTypeId() ) continue;
					if (pair->model2->mInstance.mGeometry->mID != McdBoxGetTypeId() ) continue;	

					MdtBodyID body1 = (MdtBodyID)pair->model1->mBody;
					MdtBodyID body2 = (MdtBodyID)pair->model2->mBody;

					/* ZZZZ */

					MeReal v;
					MeReal* v1 = pair->model1->linearVelocity;
					MeReal* v2 = pair->model2->linearVelocity;    
                
					if( !v1 && v2 ) 
					{
						v = v2[0]*v2[0] + v2[1]*v2[1] + v2[2]*v2[2];
					}
					else if( !v2 && v1)
					{
						v = v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2];
					}
					else if( v1 && v2 ) 
					{
						v = (v1[0] -v2[0]) * (v1[0] -v2[0]) +   
							(v1[1] -v2[1]) * (v1[1] -v2[1]) + 
							(v1[2] -v2[2]) * (v1[2] -v2[2]);
					}
					else
					{
						continue;
					}

					v = (MeReal)sqrt(v);
					if( v < 150.0) continue;

					/* ZZZZ */   
 
					McdSafeTime(pair, delta_t, &safeTimeResult);
                
					/* If we are in danger of passing through, 
					   and are not currently touching, reduce time step. */
					if( delta_t/5.0 < safeTimeResult.time && safeTimeResult.time < delta_t && !pair->responseData)
					{
						if(body1)
							body1->safeTime = safeTimeResult.time;
                    
						if(body2)
							body2->safeTime = safeTimeResult.time;
					}

				}
	 //       }

		}
		while(pairOverflow);    

		McdSpaceBeginChanges(mcd_space);

		collision_time = ReadPerformanceTimer();
#endif
//	}


#if 0
    {
//      MeVector3 pos;
        char temp[250];
        //draws collision box
        McdGeometry *g;
        MeReal x,y,z;
        MeVector3 corner[8], pos;
        MeMatrix4 *mmm = &mcd_m;
//      int i;
        /* Draw around collision on chassis */
        g = McdModelGetGeometry( mcd_world );
        McdBoxGetDimensions( (McdBoxID)g, &x, &y, &z );
        //MdtBodyGetPosition(&body[i], pos);
        x/=2;y/=2;z/=2;
        pos[0] = (*mmm)[3][0];
        pos[1] = (*mmm)[3][1];
        pos[2] = (*mmm)[3][2];
        corner[0][0] =pos[0] + x*(*mmm)[0][0]+y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[0][1] =pos[1] + x*(*mmm)[0][1]+y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[0][2] =pos[2] + x*(*mmm)[0][2]+y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[1][0] =pos[0] + x*(*mmm)[0][0]+y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[1][1] =pos[1] + x*(*mmm)[0][1]+y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[1][2] =pos[2] + x*(*mmm)[0][2]+y*(*mmm)[1][2]-z*(*mmm)[2][2];

        corner[2][0] =pos[0] + x*(*mmm)[0][0]-y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[2][1] =pos[1] + x*(*mmm)[0][1]-y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[2][2] =pos[2] + x*(*mmm)[0][2]-y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[3][0] =pos[0] + x*(*mmm)[0][0]-y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[3][1] =pos[1] + x*(*mmm)[0][1]-y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[3][2] =pos[2] + x*(*mmm)[0][2]-y*(*mmm)[1][2]-z*(*mmm)[2][2];

        corner[4][0] =pos[0] - x*(*mmm)[0][0]+y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[4][1] =pos[1] - x*(*mmm)[0][1]+y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[4][2] =pos[2] - x*(*mmm)[0][2]+y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[5][0] =pos[0] - x*(*mmm)[0][0]+y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[5][1] =pos[1] - x*(*mmm)[0][1]+y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[5][2] =pos[2] - x*(*mmm)[0][2]+y*(*mmm)[1][2]-z*(*mmm)[2][2];

        corner[6][0] =pos[0] - x*(*mmm)[0][0]-y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[6][1] =pos[1] - x*(*mmm)[0][1]-y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[6][2] =pos[2] - x*(*mmm)[0][2]-y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[7][0] =pos[0] - x*(*mmm)[0][0]-y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[7][1] =pos[1] - x*(*mmm)[0][1]-y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[7][2] =pos[2] - x*(*mmm)[0][2]-y*(*mmm)[1][2]-z*(*mmm)[2][2];


        DebugLine(corner[0],corner[1],0,0,255);
        DebugLine(corner[0],corner[2],0,0,255);
        DebugLine(corner[0],corner[4],0,0,255);
        DebugLine(corner[6],corner[2],0,0,255);
        DebugLine(corner[6],corner[4],0,0,255);
        DebugLine(corner[6],corner[7],0,0,255);
        DebugLine(corner[5],corner[1],0,0,255);
        DebugLine(corner[5],corner[4],0,0,255);
        DebugLine(corner[5],corner[7],0,0,255);
        DebugLine(corner[3],corner[1],0,0,255);
        DebugLine(corner[3],corner[7],0,0,255);
        DebugLine(corner[3],corner[2],0,0,255);

        DebugLine(corner[0],corner[5],0,0,255);
        DebugLine(corner[0],corner[6],0,0,255);
        DebugLine(corner[0],corner[3],0,0,255);
        DebugLine(corner[7],corner[1],0,0,255);
        DebugLine(corner[7],corner[2],0,0,255);
        DebugLine(corner[7],corner[4],0,0,255);
        DebugLine(corner[6],corner[5],0,0,255);
        DebugLine(corner[6],corner[3],0,0,255);
        DebugLine(corner[4],corner[1],0,0,255);
        DebugLine(corner[4],corner[2],0,0,255);
        DebugLine(corner[2],corner[1],0,0,255);
        DebugLine(corner[3],corner[5],0,0,255);

        sprintf(temp,"%d contacts\n",num_contacts);
        OutputDebugString(temp);

        mmm = &(drivers[0]->GetCar()->mcd_mtx_mem[CHASSIS_CMPT]);
        g = McdModelGetGeometry( drivers[0]->GetCar()->mcd_chassis );
        McdBoxGetDimensions( (McdBoxID)g, &x, &y, &z );
        x/=2;y/=2;z/=2;
        //MdtBodyGetPosition(&body[i], pos);
        pos[0] = (*mmm)[3][0];
        pos[1] = (*mmm)[3][1];
        pos[2] = (*mmm)[3][2];
        corner[0][0] =pos[0] + x*(*mmm)[0][0]+y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[0][1] =pos[1] + x*(*mmm)[0][1]+y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[0][2] =pos[2] + x*(*mmm)[0][2]+y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[1][0] =pos[0] + x*(*mmm)[0][0]+y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[1][1] =pos[1] + x*(*mmm)[0][1]+y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[1][2] =pos[2] + x*(*mmm)[0][2]+y*(*mmm)[1][2]-z*(*mmm)[2][2];

        corner[2][0] =pos[0] + x*(*mmm)[0][0]-y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[2][1] =pos[1] + x*(*mmm)[0][1]-y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[2][2] =pos[2] + x*(*mmm)[0][2]-y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[3][0] =pos[0] + x*(*mmm)[0][0]-y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[3][1] =pos[1] + x*(*mmm)[0][1]-y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[3][2] =pos[2] + x*(*mmm)[0][2]-y*(*mmm)[1][2]-z*(*mmm)[2][2];

        corner[4][0] =pos[0] - x*(*mmm)[0][0]+y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[4][1] =pos[1] - x*(*mmm)[0][1]+y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[4][2] =pos[2] - x*(*mmm)[0][2]+y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[5][0] =pos[0] - x*(*mmm)[0][0]+y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[5][1] =pos[1] - x*(*mmm)[0][1]+y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[5][2] =pos[2] - x*(*mmm)[0][2]+y*(*mmm)[1][2]-z*(*mmm)[2][2];

        corner[6][0] =pos[0] - x*(*mmm)[0][0]-y*(*mmm)[1][0]+z*(*mmm)[2][0];
        corner[6][1] =pos[1] - x*(*mmm)[0][1]-y*(*mmm)[1][1]+z*(*mmm)[2][1];
        corner[6][2] =pos[2] - x*(*mmm)[0][2]-y*(*mmm)[1][2]+z*(*mmm)[2][2];

        corner[7][0] =pos[0] - x*(*mmm)[0][0]-y*(*mmm)[1][0]-z*(*mmm)[2][0];
        corner[7][1] =pos[1] - x*(*mmm)[0][1]-y*(*mmm)[1][1]-z*(*mmm)[2][1];
        corner[7][2] =pos[2] - x*(*mmm)[0][2]-y*(*mmm)[1][2]-z*(*mmm)[2][2];


        DebugLine(corner[0],corner[1],0,255,0);
        DebugLine(corner[0],corner[2],0,255,0);
        DebugLine(corner[0],corner[4],0,255,0);
        DebugLine(corner[6],corner[2],0,255,0);
        DebugLine(corner[6],corner[4],0,255,0);
        DebugLine(corner[6],corner[7],0,255,0);
        DebugLine(corner[5],corner[1],0,255,0);
        DebugLine(corner[5],corner[4],0,255,0);
        DebugLine(corner[5],corner[7],0,255,0);
        DebugLine(corner[3],corner[1],0,255,0);
        DebugLine(corner[3],corner[7],0,255,0);
        DebugLine(corner[3],corner[2],0,255,0);

        DebugLine(corner[0],corner[5],0,255,0);
        DebugLine(corner[0],corner[6],0,255,0);
        DebugLine(corner[0],corner[3],0,255,0);
        DebugLine(corner[7],corner[1],0,255,0);
        DebugLine(corner[7],corner[2],0,255,0);
        DebugLine(corner[7],corner[4],0,255,0);
        DebugLine(corner[6],corner[5],0,255,0);
        DebugLine(corner[6],corner[3],0,255,0);
        DebugLine(corner[4],corner[1],0,255,0);
        DebugLine(corner[4],corner[2],0,255,0);
        DebugLine(corner[2],corner[1],0,255,0);
        DebugLine(corner[3],corner[5],0,255,0);

}

#endif



}


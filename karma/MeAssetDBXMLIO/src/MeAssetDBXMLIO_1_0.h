#ifndef _MEASSETDBXMLIO_1_0__H
#define _MEASSETDBXMLIO_1_0__H

/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 12:29:56 $ - Revision: $Revision: 1.7.2.6 $

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

#include <MeAssetDBTypes.h>
#include <MeAssetDBXMLIOTypes.h>
#include <MeXMLOutput.h>
#include <MeXMLTree.h>
#include <MeXMLParser.h>

/*************************************************************\
  HEADER FILE FOR VERSION 1.0 OF THE FILE FORMAT 
\*************************************************************/

MeXMLError      MEAPI Handle_KaFile_0_1(MeXMLElement *elem, PElement *parent);
MeFAsset       *MEAPI KaFileCreate_1_0(MeAssetDB *db, MeIDPool *IDPool, PElement *e);
MeXMLElementID  MEAPI KaFileWriteXML_1_0(MeXMLOutput *op, MeFAsset *asset, MeXMLElementID parent);

MeXMLElementID  MEAPI MeFAssetPartWriteXML_1_0(MeXMLOutput *op, MeFAssetPart *part, MeXMLElementID parent);
MeXMLElementID  MEAPI MeFPrimitiveWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *prim, MeXMLElementID parent);
MeXMLElementID  MEAPI MeFGeometryWriteXML_1_0(MeXMLOutput *op, MeFGeometry *geometry, MeXMLElementID parent);
MeXMLElementID  MEAPI MeFModelWriteXML_1_0(MeXMLOutput *op, MeFModel *model, MeXMLElementID parent);
MeXMLElementID  MEAPI MeFJointWriteXML_1_0(MeXMLOutput *op, MeFJoint *joint, MeXMLElementID parent);

MeFGeometry    *MEAPI MeFGeometryCreateFromFile_1_0(MeFAsset *asset, PElement *e);
MeFPrimitive   *MEAPI MeFPrimitiveCreateFromFile_1_0(MeFGeometry *parent, PElement *e);
MeFModel       *MEAPI MeFModelCreateFromFile_1_0(MeFAsset *asset, PElement *e);
MeFJoint       *MEAPI MeFJointCreateFromFile_1_0(MeFAsset *asset, PElement *e);
MeFAssetPart   *MEAPI MeFAssetPartCreateFromFile_1_0(MeFAsset *asset, PElement *e);

MeXMLError      MEAPI Handle_Asset_1_0(MeXMLElement *elem, PElement *parent);

#define ME_ELEM_ASSET                   "ASSET"
#define ME_ELEM_GEOMETRY                    "GEOMETRY"
#define ME_ELEM_GEOMETRY_PART                   "PRIMITIVE"
#define ME_ELEM_GEOMETRY_RADIUS                     "RADIUS"
#define ME_ELEM_GEOMETRY_HEIGHT                     "HEIGHT"
#define ME_ELEM_GEOMETRY_DIMS                       "DIMS"
#define ME_ELEM_GEOMETRY_VERTEX                     "VERTEX"
#define ME_ELEM_GEOMETRY_TM                         "TM"
#define ME_ELEM_MODEL                       "MODEL"
#define ME_ELEM_DYNAMICS                        "DYNAMICS"
#define ME_ELEM_DYNAMICS_MASS                       "MASS"
#define ME_ELEM_DYNAMICS_DENSITY                    "DENSITY"
#define ME_ELEM_DYNAMICS_MASS_OFFSET                "MASS_OFFSET"
#define ME_ELEM_DYNAMICS_INERTIA                    "INERTIA"
#define ME_ELEM_DYNAMICS_LIN_DAMP                   "LIN_DAMP"
#define ME_ELEM_DYNAMICS_ANG_DAMP                   "ANG_DAMP"
#define ME_ELEM_DYNAMICS_FAST_SPIN                  "FAST_SPIN"
#define ME_ELEM_DYNAMICS_USE_FAST_SPIN				"USE_FAST_SPIN"
#define ME_ELEM_JOINT                       "JOINT"
#define ME_ELEM_JOINT_POS1                      "POS1"
#define ME_ELEM_JOINT_POS2                      "POS2"
#define ME_ELEM_JOINT_PRIMARY_AXIS1             "PRIMARY_AXIS1"
#define ME_ELEM_JOINT_PRIMARY_AXIS2             "PRIMARY_AXIS2"
#define ME_ELEM_JOINT_ORTHOGONAL_AXIS1          "ORTHOGONAL_AXIS1"
#define ME_ELEM_JOINT_ORTHOGONAL_AXIS2          "ORTHOGONAL_AXIS2"
/* used by car wheel */
#define ME_ELEM_JOINT_S_LOCK                    "STEERING_LOCK"
#define ME_ELEM_JOINT_S_DESVEL                  "STEERING_DES_VEL"
#define ME_ELEM_JOINT_S_MAXFORCE                "STEERING_MAX_FORCE"
#define ME_ELEM_JOINT_H_DESVEL                  "HINGE_DES_VEL"
#define ME_ELEM_JOINT_H_MAXFORCE                "HINGE_MAX_FORCE"
#define ME_ELEM_JOINT_SUSP_LOW                  "SUSP_LOW_LIMIT"
#define ME_ELEM_JOINT_SUSP_HIGH                 "SUSP_HIGH_LIMIT"
#define ME_ELEM_JOINT_SUSP_SOFT                 "SUSP_SOFTNESS"
#define ME_ELEM_JOINT_SUSP_REF                  "SUSP_REF"
#define ME_ELEM_JOINT_SUSP_STIFF                "SUSP_STIFFNESS"
#define ME_ELEM_JOINT_SUSP_DAMP                 "SUSP_DAMPING"
/* used by RPRO */
#define ME_ELEM_JOINT_STRENGTH_L0               "L0"
#define ME_ELEM_JOINT_STRENGTH_L1               "L1"
#define ME_ELEM_JOINT_STRENGTH_L2               "L2"
#define ME_ELEM_JOINT_STRENGTH_A0               "A0"
#define ME_ELEM_JOINT_STRENGTH_A1               "A1"
#define ME_ELEM_JOINT_STRENGTH_A2               "A2"
/* used by hinge */
#define ME_ELEM_JOINT_LOW_LIMIT                 "LOW_LIMIT"
#define ME_ELEM_JOINT_HIGH_LIMIT                "HIGH_LIMIT"
#define ME_ELEM_JOINT_LOW_STIFF                 "LOW_STIFFNESS"
#define ME_ELEM_JOINT_HIGH_STIFF                "HIGH_STIFFNESS"
#define ME_ELEM_JOINT_LIMITED                   "LIMITED"
#define ME_ELEM_JOINT_MOTORIZED                 "MOTORIZED"
#define ME_ELEM_JOINT_DES_VEL                   "DES_VEL"
#define ME_ELEM_JOINT_MAX_FORCE                 "MAX_FORCE"
/* used by cone limit */
#define ME_ELEM_JOINT_HALF_ANGLE                "HALF_ANGLE"
#define ME_ELEM_JOINT_STIFFNESS                 "STIFFNESS"
/* used by skeletal joint */
#define ME_ELEM_JOINT_TWISTTYPE                 "TWIST_TYPE"
#define ME_ELEM_JOINT_CONETYPE                  "CONE_TYPE"
#define ME_ELEM_JOINT_TWIST_HALF_ANGLE          "TWIST_HALF_ANGLE"
#define ME_ELEM_JOINT_CONE_HALF_ANGLE_X         "CONE_HALF_ANGLE_X"
#define ME_ELEM_JOINT_CONE_HALF_ANGLE_Y         "CONE_HALF_ANGLE_Y"
#define ME_ELEM_JOINT_TWIST_STIFF               "TWIST_STIFFNESS"
#define ME_ELEM_JOINT_CONE_STIFF                "CONE_STIFFNESS"
#define ME_ELEM_JOINT_TWIST_DAMP                "TWIST_DAMPING"
#define ME_ELEM_JOINT_CONE_DAMP                 "CONE_DAMPING"
/* used by angular3 */
#define ME_ELEM_JOINT_DAMPING                   "DAMPING"
#define ME_ELEM_JOINT_ROTATION_ENABLED          "ROTATION_ENABLED"
/* used by spring6 */
#define ME_ELEM_JOINT_LINEAR_STIFF_X            "LINEAR_STIFF_X"
#define ME_ELEM_JOINT_LINEAR_STIFF_Y            "LINEAR_STIFF_Y"
#define ME_ELEM_JOINT_LINEAR_STIFF_Z            "LINEAR_STIFF_Z"
#define ME_ELEM_JOINT_LINEAR_DAMP_X             "LINEAR_DAMP_X"
#define ME_ELEM_JOINT_LINEAR_DAMP_Y             "LINEAR_DAMP_Y"
#define ME_ELEM_JOINT_LINEAR_DAMP_Z             "LINEAR_DAMP_Z"
#define ME_ELEM_JOINT_ANGULAR_STIFF_X           "ANGULAR_STIFF_X"
#define ME_ELEM_JOINT_ANGULAR_STIFF_Y           "ANGULAR_STIFF_Y"
#define ME_ELEM_JOINT_ANGULAR_STIFF_Z           "ANGULAR_STIFF_Z"
#define ME_ELEM_JOINT_ANGULAR_DAMP_X            "ANGULAR_DAMP_X"
#define ME_ELEM_JOINT_ANGULAR_DAMP_Y            "ANGULAR_DAMP_Y"
#define ME_ELEM_JOINT_ANGULAR_DAMP_Z            "ANGULAR_DAMP_Z"

#define ME_ELEM_ASSET_PART                  "PART"
#define ME_ELEM_ASSET_PART_TM                   "TM"
#define ME_ELEM_NO_COLLISION                "NO_COLLISION"
/* geometry types */

#define ME_GEOM_TYPE_SPHERE         "sphere"
#define ME_GEOM_TYPE_BOX            "box"
#define ME_GEOM_TYPE_CYLINDER       "cylinder"
#define ME_GEOM_TYPE_SPHYL          "sphyl"
#define ME_GEOM_TYPE_PLANE          "plane"
#define ME_GEOM_TYPE_CONVEX         "convex"

/* joint types */

#define ME_JOINT_TYPE_BALLANDSOCKET "ballandsocket"
#define ME_JOINT_TYPE_UNIVERSAL     "universal"
#define ME_JOINT_TYPE_RPRO          "rpro"
#define ME_JOINT_TYPE_HINGE         "hinge"
#define ME_JOINT_TYPE_CARWHEEL      "carwheel"
#define ME_JOINT_TYPE_CONELIMIT     "conelimit"
#define ME_JOINT_TYPE_PRISMATIC     "prismatic"
#define ME_JOINT_TYPE_SKELETAL      "skeletal"
#define ME_JOINT_TYPE_ANGULAR3      "angular3"
#define ME_JOINT_TYPE_SPRING6       "spring6"

/* attributes used in Karma file format */

#define ME_ATTR_ID                 "id"
#define ME_ATTR_GEOMETRY           "geometry"
#define ME_ATTR_MODEL              "model"
#define ME_ATTR_TYPE               "type"
#define ME_ATTR_PART1              "part1"
#define ME_ATTR_PART2              "part2"
#define ME_ATTR_GRAPHIC            "graphic"
#define ME_ATTR_SCALE              "scale"
#define ME_ATTR_REF_PART           "ref_part"
#define ME_ATTR_GRAPHIC_OFFSET     "graphic_offset"
#define ME_ATTR_PARENT             "parent"
#define ME_ATTR_MASS_SCALE         "mass_scale"
#define ME_ATTR_LENGTH_SCALE       "length_scale"

typedef struct PPrimitive            PPrimitive;
typedef struct PDynamics             PDynamics;
typedef struct PJoint                PJoint;
typedef struct PAssetPart            PAssetPart;


/* 
    There is one type for each PElement that we are interested in. 
*/
typedef enum
{
    kPElementTypeUnknown = 0,
    kPElementTypeGeometry,
    kPElementTypeGeometryPrimitive,            
    kPElementTypeModel,          
    kPElementTypeDynamics,
    kPElementTypeJoint,
    kPElementTypeAsset,
    kPElementTypeAssetPart,
    kPElementTypeNoCollision
} PElementType;


/* union of all geometry types */
struct PPrimitive
{ 
    MeVector3       dims;
    int             nVertices;
    MeVector3       *vertices;
    int             maxVertices;
    MeVector3       tempVertex; /* temporary storage for convex */
    MeMatrix4       tm;
};

struct PDynamics
{
    MeVector3       mass_offset;
    MeReal          mass;
    MeReal          density;
    MeReal          inertia[6];
    MeReal          linearDamp;
    MeReal          angularDamp;
    MeVector3       fastSpin;
	MeBool			useFastSpin;
};

/* covers all joint types */
struct PJoint
{   
    MeVector3       pos[2];
    MeVector3       pax[2];
    MeVector3       oax[2];

    /* joint-specific data */
    MeReal          stop[3];
    MeReal          stiff[6];
    MeReal          damp[6]; 
    MeBool          bLimited[1];
    MeBool          bMotorized[1];
    MeReal          strength[6];
    MeReal          desVel[2];
    MeReal          special_f[1];
    int             special_i[2];
    MeBool          special_b[1];
};

struct PAssetPart
{
    MeMatrix4       tm;
};


#endif

/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/25 09:24:01 $ - Revision: $Revision: 1.3.2.2 $

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

/* Mathengine headers   */
#include <Mst.h>
#include <MeApp.h>
#include <MeViewer.h>

/* Application headers  */
#include "main.h"
#include "utils.h"
#include "vehicle.h"
#include "rider.h"

/* disable unused variable warnings */
#ifdef _MSC_VER
#pragma warning( disable : 4101 )
#endif

RiderData riderData =
{
    /* Initial Limb Positions   */
    {{-0.21, 0.08, 0.0}, /* lower legs */
    {-0.26, 0.5, 0.0},  /* upper legs */
    {-0.27, 0.95, 0.0}, /* torso */
    {-0.14, 0.94, 0.25},    /* l upper arm */
    {-0.14, 0.94, -0.25},   /* r upper arm */
    {0.04, 0.68, 0.3},  /* l fore arm */
    {0.04, 0.68, -0.3}}, /* r fore arm */

    /* Initial Limb Orientations    */
    {{0.0, 0.0, -1.116},        /* lower legs */
    {0.0, 0.0, 0.927},      /* upper legs */
    {0.0, 0.0, -1.03},      /* torso */
    {0.0, -.438, 1.326},    /* l upper arm */
    {0.0, 0.438, 1.326},    /* r upper arm */
    {0.0, 0.0, 0.627},      /* l fore arm */
    {0.0, 0.0, 0.627}},     /* r fore arm */

    /* Limb Dimensions  */
    {{0.5, 0.12, 0.7},  /* lower legs */
    {0.5, 0.15, 0.55},  /* upper legs */
    {0.6, 0.25, 0.4},   /* torso */
    {0.3, 0.12, 0.12},  /* l upper arm */
    {0.3, 0.12, 0.12},  /* r upper arm */
    {0.35, 0.1, 0.1},   /* l fore arm */
    {0.35, 0.1, 0.1}},  /* r fore arm */

    /* Initial Joint Positions  */
    {{-0.32, -0.15, 0.0},   /* ankles */
    {-0.1, 0.3, 0.0},       /* knees */
    {-0.4, 0.7, 0.0},       /* hips */
    {-0.18, 1.1, 0.225},        /* l shoulder */
    {-0.18, 1.1, -0.225},       /* r shoulder */
    {-0.1, 0.78, 0.3},      /* l elbow */
    {-0.1, 0.78, -0.3}, /* r elbow */
    {0.19, 0.57, 0.3},      /* l wrist */
    {0.19, 0.57, -0.3}},        /* r wrist */

    /* Joint Axes   */
    {{0.0, 0.0, 1.0},       /* ankles */
    {0.0, 0.0, 1.0},        /* knees */
    {1.0, 0.0, .0},     /* hips */
    {0.5, 0.0, 0.866},      /* l shoulder */
    {0.5, 0.0, -0.866},     /* r shoulder */
    {0.0, 1.0, 0.0},        /* l elbow */
    {0.0, 1.0, 0.0},        /* r elbow */
    {0.0, 0.0, 1.0},        /* l wrist */
    {0.0, 0.0, 1.0}},       /* r wrist */

    /* Joint Bodies */
    {{LOWER_LEGS, -1},          /* ankles */
    {UPPER_LEGS, LOWER_LEGS},   /* knees */
    {TORSO, UPPER_LEGS},        /* hips */
    {TORSO, UPPER_ARM_L},       /* l shoulder */
    {TORSO, UPPER_ARM_R},       /* r shoulder */
    {UPPER_ARM_L, LOWER_ARM_L}, /* l elbow */
    {UPPER_ARM_R, LOWER_ARM_R}, /* r elbow */
    {LOWER_ARM_L, -1},          /* l wrist */
    {LOWER_ARM_R, -1}},         /* r wrist */

    {1<<ANKLES | 1<<KNEES | 1<<HIPS | 1<<SHOULDER_L | 1<<SHOULDER_R},   /* isHinge  */

    {{100.0, 20.0}, /* ankles */
    {100.0, 10.0}, /* knees */
    {400.0, 50.0}, /* hips */
    {5.0, 50.0}, /* l shoulder */
    {5.0, 50.0}, /* r shoulder */
    {0.0, 0.0}, /* l elbow */
    {0.0, 0.0}, /* r elbow */
    {0.0, 0.0}, /* l wrist */
    {0.0, 0.0}} /* r wrist */
};

Rider rider;

/***********************************************************************************************
*
***********************************************************************************************/
void UpdateRiderControls(Rider *rider)
{
    MeReal speed, maxAng = 0.35f,angle;
    MdtLimitID limit;
    MeVector3 vel;
    MeVector3Ptr xAxis;

    /* Add Rider Control code here  */
    /* PASTE_19 */
}

/***********************************************************************************************
*
***********************************************************************************************/
void CreateRiderGraphics(Rider *rider)
{
    int i;
    for(i = 0; i < NUM_LIMBS; i++)
    {
        if(!rider->limbBody[i])
            return;
    }

    rider->limbGraphic[LOWER_LEGS]  = RGraphicLoadASE(rc,"lowerLegs.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(rider->limbBody[LOWER_LEGS]));
    RGraphicSetTexture(rc, rider->limbGraphic[LOWER_LEGS], "torso");
    rider->limbGraphic[UPPER_LEGS]  = RGraphicLoadASE(rc,"upperLegs.ase",1.0f,-1.0f,1.0f,white,MdtBodyGetTransformPtr(rider->limbBody[UPPER_LEGS]));
    RGraphicSetTexture(rc, rider->limbGraphic[UPPER_LEGS], "torso");
    rider->limbGraphic[TORSO]       = RGraphicLoadASE(rc,"torso.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(rider->limbBody[TORSO]));
    RGraphicSetTexture(rc, rider->limbGraphic[TORSO], "torso");
    rider->limbGraphic[UPPER_ARM_L] = RGraphicLoadASE(rc,"upperArm.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(rider->limbBody[UPPER_ARM_L]));
    RGraphicSetTexture(rc, rider->limbGraphic[UPPER_ARM_L], "torso");
    rider->limbGraphic[UPPER_ARM_R] = RGraphicLoadASE(rc,"upperArm.ase",1.0f,1.0f,-1.0f,white,MdtBodyGetTransformPtr(rider->limbBody[UPPER_ARM_R]));
    RGraphicSetTexture(rc, rider->limbGraphic[UPPER_ARM_R], "torso");
    rider->limbGraphic[LOWER_ARM_L] = RGraphicLoadASE(rc,"lowerArm.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(rider->limbBody[LOWER_ARM_L]));
    RGraphicSetTexture(rc, rider->limbGraphic[LOWER_ARM_L], "torso");
    rider->limbGraphic[LOWER_ARM_R] = RGraphicLoadASE(rc,"lowerArm.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(rider->limbBody[LOWER_ARM_R]));
    RGraphicSetTexture(rc, rider->limbGraphic[LOWER_ARM_R], "torso");
}


/***********************************************************************************************
*
***********************************************************************************************/
void InitialiseRider(Rider *rider)
{
    int i,j;
    MeVector3 pos;
    MeMatrix3 R;
    MdtBSJointID bsj;
    MdtHingeID hinge;
    MdtBodyID body1, body2;
    MdtLimitID limit;

    memset(rider, 0, sizeof(Rider));

    rider->data = &riderData;

    /* Create Rider here    */
    /* PASTE_16 */

    /* Add rider joint code here    */
    /* PASTE_17 */

    CreateRiderGraphics(rider);
}

/***********************************************************************************************
*
***********************************************************************************************/
void ResetRider(Rider *rider)
{
    int i;
    MeVector3 pos;
    MeMatrix3 R;

    for(i=0; i<NUM_LIMBS; i++)
    {
        if(rider->limbBody[i])
        {
            MeVector3Add(pos, startPos, rider->data->limbPos[i]);
            MdtBodySetPosition(rider->limbBody[i], pos[0], pos[1], pos[2]);
            MeMatrix3FromEulerAngles(R, rider->data->limbAng[i][0], 
                                        rider->data->limbAng[i][1], 
                                        rider->data->limbAng[i][2]);

            MdtBodySetOrientation(rider->limbBody[i], R);
            MdtBodySetLinearVelocity(rider->limbBody[i], 0,0,0);
            MdtBodySetAngularVelocity(rider->limbBody[i], 0,0,0);
        }
    }
}

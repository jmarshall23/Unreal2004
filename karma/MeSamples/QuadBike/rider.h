#ifndef RIDER_H /* -*- mode: c; -*- */
#define RIDER_H

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:13 $ - Revision: $Revision: 1.2.2.2 $

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

typedef enum
{
    LOWER_LEGS=0,
    UPPER_LEGS,
    TORSO,
    UPPER_ARM_L,
    UPPER_ARM_R,
    LOWER_ARM_L,
    LOWER_ARM_R,
    NUM_LIMBS
} RiderLimbs;

typedef enum
{
    ANKLES=0,
    KNEES,
    HIPS,
    SHOULDER_L,
    SHOULDER_R,
    ELBOW_L,
    ELBOW_R,
    WRIST_L,
    WRIST_R,
    NUM_JOINTS
} RiderJoints;

typedef struct
{
    MeVector3 limbPos[NUM_LIMBS];
    MeVector3 limbAng[NUM_LIMBS];
    MeVector3 limbDim[NUM_LIMBS];
    MeVector3 jointPos[NUM_JOINTS];
    MeVector3 jointAxis[NUM_JOINTS];
    int     jointBodies[NUM_JOINTS][2];
    int     isHinge; //bit shifted by joint number indicates a hinge, otherwise a ball & socket
    MeReal jointParam[NUM_JOINTS][2];

} RiderData;

typedef struct
{
    MdtBodyID           limbBody[NUM_LIMBS];
    McdGeometryID       limbGeom[NUM_LIMBS];
    McdModelID          limbCM[NUM_LIMBS];
    RGraphic*           limbGraphic[NUM_LIMBS];

    MdtConstraintID    joint[NUM_JOINTS];

    RiderData           *data;
//    MdtSpringID         bodyProwSpring;
//    MdtSpringID         elbowSpring[3];

//    RGraphic*           headG;
//    MeMatrix4           headTM;

    int                 currentPose;
} Rider;

extern Rider rider;

extern void InitialiseRider(Rider *rider);
extern void DestroyRider(Rider *rider);
extern void ResetRider(Rider *rider);
extern void UpdateRiderControls(Rider *rider);

#endif //RIDER_H

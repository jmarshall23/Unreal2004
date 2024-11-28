/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:12 $ - Revision: $Revision: 1.9.2.1 $

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

#include "JetSki.h"
#include <MeMath.h>

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif


#define BLUE    {0.2, 0.2, 1.0, 1.0}
#define RED     {1.0, 0.2, 0.2, 1.0}
#define GREEN   {0.0, 1.0, 0.0, 1.0}
#define YELLOW  {1.0, 1.0, 0.0, 1.0}
#define BLACK   {0.0, 0.0, 0.0, 1.0}
#define WHITE   {1.0, 1.0, 1.0, 1.0}

MeVector4 jointColor = WHITE;


MeVector3 boxSize[10] =
{
    {3.5, 0.1, 1.2}, /* hull */
    {0.15, 0.15, 1.0}, /* handle */
    {0.25, 0.7, 0.7}, /* calves */
    {0.3, 0.8, 0.75}, /* thighs */
    {0.4, 0.25, 0.8}, /* groin */
    {0.35, 0.7, 0.8}, /* torso */
    {0.6, 0.2, 0.2}, /* l upper arm */
    {0.6, 0.2, 0.2}, /* r upper arm */
    {0.6, 0.15, 0.15}, /* l fore arm */
    {0.6, 0.15, 0.15}, /* r fore arm */
};

MeVector3 headSize = {0.2, 0.2, 0.2};
MeVector3 seatSize = {3.0, 0.65, 0.3};
MeVector3 prowSize = {0.2, 1.2, 0.4};

MeVector3 boxPos[10] =
{
    {0.0, 0.0, 0.0}, /* hull */
    {0.5, 1.2, 0.0}, /* handle */
    {-0.7, 0.35, 0.0}, /* calves */
    {-0.7, 0.75, 0.0}, /* thighs */
    {-1.0, 0.9, 0.0}, /* groin */
    {-0.8, 1.3, 0.0}, /* torso */
    {-0.3, 1.2, 0.5}, /* l upper arm */
    {-0.3, 1.2, -0.5}, /* r upper arm */
    {0.2, 1.15, 0.5}, /* l fore arm */
    {0.2, 1.15, -0.5}, /* r fore arm */
};

MeVector3 boxRot[10] =
{
    {0.0, 0.0, 0.0}, /* hull */
    {0.0, 0.0, 0.0}, /* handle */
    {0.0, 0.0, ME_PI/3}, /* calves */
    {0.0, 0.0, -ME_PI/3}, /* thighs */
    {0.0, 0.0, 0.0}, /* groin */
    {0.0, 0.0, ME_PI/5}, /* torso */
    {0.0, ME_PI/3, ME_PI/6}, /* l upper arm */
    {0.0, -ME_PI/3, ME_PI/6}, /* r upper arm */
    {0.0, -ME_PI/3, -0.3}, /* l fore arm */
    {0.0, ME_PI/3, -0.3}, /* r fore arm */
};

MeBool boxIsCylinder[10] =
{
    0, /* hull */
    0, /* handle */
    0, /* calves */
    0, /* thighs */
    0, /* groin */
    0, /* torso */
    1, /* l upper arm */
    1, /* r upper arm */
    1, /* l fore arm */
    1, /* r fore arm */
};

MeVector4 boxColor[10] =
{
    RED, /* hull */
    RED, /* handle */
    BLUE, /* calves */
    BLUE, /* thighs */
    YELLOW, /* groin */
    YELLOW, /* torso */
    GREEN, /* l upper arm */
    GREEN, /* r upper arm */
    GREEN, /* l fore arm */
    GREEN, /* r fore arm */
};

MeVector4 headColor = WHITE;
MeVector4 seatColor = RED;
MeVector4 prowColor = RED;


MeMatrix4 bodyToHeadTM =
{
    1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   0, 0.6, 0, 1
};

MeMatrix4 hullToSeatTM =
{
    1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   -0.25, 0.3, 0, 1
};

MeMatrix4 hullToProwTM =
{
    1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   0.8, 0.6, 0, 1
};

MeVector3 jointPos[11] =
{
    {0.7, 1.2, 0.0}, /* steering */
    {-1.05, 0.05, 0.0}, /* ankles */
    {-0.45, 0.55, 0.0}, /* knees */
    {-1.0, 0.9, 0.0}, /* hips */
    {-1.0, 1.1, 0.0}, /* waist */
    {-0.55, 1.4, 0.3}, /* l shoulder */
    {-0.55, 1.4, -0.3}, /* r shoulder */
    {-0.05, 1.1, 0.65}, /* l elbow */
    {-0.05, 1.1, -0.65}, /* r elbow */
    {0.45, 1.2, 0.4}, /* l wrist */
    {0.45, 1.2, -0.4}, /* r wrist */
};

MeVector3 jointAxis[11] =
{
    {0.0, 1.0, 0.0}, /* steering */
    {0.0, 0.0, 1.0}, /* ankles */
    {0.0, 0.0, 1.0}, /* knees */
    {0.0, 0.0, 1.0}, /* hips */
    {1.0, 0.0, 0.0}, /* waist */
    {0.0, 0.0, 1.0}, /* l shoulder */
    {0.0, 0.0, 1.0}, /* r shoulder */
    {0.0, 1.0, 0.0}, /* l elbow */
    {0.0, 1.0, 0.0}, /* r elbow */
    {0.0, 0.0, 1.0}, /* l wrist */
    {0.0, 0.0, 1.0}, /* r wrist */
};

int jointBodies[11][2] =
{
    {kJetSkiHull, kJetSkiHandle}, /* steering */
    {kJetSkiCalves, kJetSkiHull}, /* ankles */
    {kJetSkiThighs, kJetSkiCalves}, /* knees */
    {kJetSkiGroin, kJetSkiThighs}, /* hips */
    {kJetSkiGroin, kJetSkiTorso}, /* waist */
    {kJetSkiTorso, kJetSkiUpperArmL}, /* l shoulder */
    {kJetSkiTorso, kJetSkiUpperArmR}, /* r shoulder */
    {kJetSkiUpperArmL, kJetSkiForeArmL}, /* l elbow */
    {kJetSkiUpperArmR, kJetSkiForeArmR}, /* r elbow */
    {kJetSkiForeArmL, kJetSkiHandle}, /* l wrist */
    {kJetSkiForeArmR, kJetSkiHandle}, /* r wrist */
};

MeBool jointIsHinge[11] =
{
    1, /* steering */
    1, /* ankles */
    1, /* knees */
    1, /* hips */
    1, /* waist */
    0, /* l shoulder */
    0, /* r shoulder */
    0, /* l elbow */
    0, /* r elbow */
    0, /* l wrist */
    0, /* r wrist */
};

MeBool jointIsEnabled[11] =
{
    1, /* steering */
    1, /* ankles */
    1, /* knees */
    1, /* hips */
    1, /* waist */
    1, /* l shoulder */
    1, /* r shoulder */
    1, /* l elbow */
    1, /* r elbow */
    1, /* l wrist */
    1, /* r wrist */
};

#define NPOSES 2

/* ankle angle, knee angle, hip angle */
MeReal pose[NPOSES][3] =
{
    {0.0, 0.0, 0.0},
    {0.25, -0.6, 0.5},
};

/* stiffness, then damping */
MeReal jointParam[11][2] =
{
    {0.0, 0.0}, /* steering */
    {100.0, 5.0}, /* ankles */
    {50.0, 5.0}, /* knees */
    {50.0, 5.0}, /* hips */
    {100.0, 15.0}, /* waist */
    {0.0, 0.0}, /* l shoulder */
    {0.0, 0.0}, /* r shoulder */
    {0.0, 0.0}, /* l elbow */
    {0.0, 0.0}, /* r elbow */
    {0.0, 0.0}, /* l wrist */
    {0.0, 0.0}, /* r wrist */
};

JetSki* JetSkiCreate(MdtWorldID world, RRender* rc)
{
    int i;
    JetSki* jetski = MeMemoryAPI.create(sizeof(JetSki));
    MeMatrix3 R;

    jetski->body = (MdtBodyID*)MeMemoryAPI.create(kJetSkiMaxParts * sizeof(MdtBodyID));
    jetski->bodyG = (RGraphic**)MeMemoryAPI.create(kJetSkiMaxParts * sizeof(RGraphic*));

    jetski->joint = (MdtConstraintID*)MeMemoryAPI.create(kJetSkiMaxJoints * sizeof(MdtConstraintID));
    jetski->jointTM = (MeMatrix4*)MeMemoryAPI.createAligned(kJetSkiMaxJoints * sizeof(MeMatrix4), 16);


    for(i=0; i<kJetSkiMaxParts; i++)
    {
        jetski->body[i] = MdtBodyCreate(world);

        MdtBodySetPosition(jetski->body[i],
            boxPos[i][0], boxPos[i][1], boxPos[i][2]);


        MeMatrix3FromEulerAngles(R,
            boxRot[i][0], boxRot[i][1], boxRot[i][2]);

        MdtBodySetOrientation(jetski->body[i], R);

        if(boxIsCylinder[i])
        {
            jetski->bodyG[i] = RGraphicCylinderCreate(rc,
                boxSize[i][2]*0.5f, boxSize[i][0],
                boxColor[i], MdtBodyGetTransformPtr(jetski->body[i]));
        }
        else
        {
            jetski->bodyG[i] = RGraphicBoxCreate(rc,
                boxSize[i][0], boxSize[i][1], boxSize[i][2],
                boxColor[i], MdtBodyGetTransformPtr(jetski->body[i]));
        }

        MdtBodySetAngularVelocityDamping(jetski->body[i], 0.5f);
        MdtBodySetLinearVelocityDamping(jetski->body[i], 0.5f);

        MdtBodyEnable(jetski->body[i]);
    }

    jetski->headG = RGraphicSphereCreate(rc, headSize[0], headColor, jetski->headTM);
    jetski->seatG = RGraphicBoxCreate(rc, seatSize[0], seatSize[1], seatSize[2], seatColor, jetski->seatTM);
    jetski->prowG = RGraphicBoxCreate(rc, prowSize[0], prowSize[1], prowSize[2], prowColor, jetski->prowTM);



    for(i=0; i<kJetSkiMaxJoints; i++)
    {
        MeMatrix4TMMakeIdentity(jetski->jointTM[i]);

        RGraphicBoxCreate(rc, 0.1, 0.1, 0.1, jointColor, jetski->jointTM[i]);

        if(jointIsHinge[i])
        {
            MdtLimitID limit;
            MdtHingeID hinge = MdtHingeCreate(world);

            MdtHingeSetBodies(hinge, jetski->body[jointBodies[i][0]], jetski->body[jointBodies[i][1]]);
            MdtHingeSetPosition(hinge, jointPos[i][0], jointPos[i][1], jointPos[i][2]);
            MdtHingeSetAxis(hinge, jointAxis[i][0], jointAxis[i][1], jointAxis[i][2]);

            if(jointParam[i][0] > 0.01 || jointParam[i][1] > 0.01)
            {
                limit = MdtHingeGetLimit(hinge);
                MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), jointParam[i][0]);
                MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), jointParam[i][0]);
                MdtSingleLimitSetDamping(MdtLimitGetUpperLimit(limit), jointParam[i][1]);
                MdtSingleLimitSetDamping(MdtLimitGetLowerLimit(limit), jointParam[i][1]);
                MdtLimitActivateLimits(limit, 1);
            }

            jetski->joint[i] = MdtHingeQuaConstraint(hinge);
        }
        else
        {
            MdtBSJointID bs = MdtBSJointCreate(world);

            MdtBSJointSetBodies(bs, jetski->body[jointBodies[i][0]], jetski->body[jointBodies[i][1]]);
            MdtBSJointSetPosition(bs, jointPos[i][0], jointPos[i][1], jointPos[i][2]);

            jetski->joint[i] = MdtBSJointQuaConstraint(bs);
        }

        if(jointIsEnabled[i])
        {
            MdtConstraintEnable(jetski->joint[i]);
        }
    }


    /* whack some springs in the keep things nice and tight */
    jetski->bodyProwSpring = MdtSpringCreate(world);
    MdtSpringSetBodies(jetski->bodyProwSpring, jetski->body[kJetSkiHull], jetski->body[kJetSkiTorso]);
    MdtSpringSetPosition(jetski->bodyProwSpring, 0, jointPos[kJetSkiSteering][0], jointPos[kJetSkiSteering][1], jointPos[kJetSkiSteering][2]);
    MdtSpringSetStiffness(jetski->bodyProwSpring, 100);
    MdtSpringSetDamping(jetski->bodyProwSpring, 10);
    MdtSpringEnable(jetski->bodyProwSpring);

    jetski->elbowSpring[0] = MdtSpringCreate(world);
    MdtSpringSetBodies(jetski->elbowSpring[0], jetski->body[kJetSkiHull], jetski->body[kJetSkiUpperArmL]);
    MdtSpringSetPosition(jetski->elbowSpring[0], 0, 0, 0.4, -0.2);
    MdtSpringSetPosition(jetski->elbowSpring[0], 1, jointPos[kJetSkiElbowL][0], jointPos[kJetSkiElbowL][1], jointPos[kJetSkiElbowL][2]);
    MdtSpringSetStiffness(jetski->elbowSpring[0], 100);
    MdtSpringSetDamping(jetski->elbowSpring[0], 10);
    MdtSpringEnable(jetski->elbowSpring[0]);

    jetski->elbowSpring[1] = MdtSpringCreate(world);
    MdtSpringSetBodies(jetski->elbowSpring[1], jetski->body[kJetSkiHull], jetski->body[kJetSkiUpperArmR]);
    MdtSpringSetPosition(jetski->elbowSpring[1], 0, 0, 0.4, 0.2);
    MdtSpringSetPosition(jetski->elbowSpring[1], 1, jointPos[kJetSkiElbowR][0], jointPos[kJetSkiElbowR][1], jointPos[kJetSkiElbowR][2]);
    MdtSpringSetStiffness(jetski->elbowSpring[1], 100);
    MdtSpringSetDamping(jetski->elbowSpring[1], 10);
    MdtSpringEnable(jetski->elbowSpring[1]);

    jetski->elbowSpring[2] = MdtSpringCreate(world);
    MdtSpringSetBodies(jetski->elbowSpring[2], jetski->body[kJetSkiUpperArmL], jetski->body[kJetSkiUpperArmR]);
    MdtSpringSetPosition(jetski->elbowSpring[2], 0, jointPos[kJetSkiElbowL][0], jointPos[kJetSkiElbowL][1], jointPos[kJetSkiElbowL][2]);
    MdtSpringSetPosition(jetski->elbowSpring[2], 1, jointPos[kJetSkiElbowR][0], jointPos[kJetSkiElbowR][1], jointPos[kJetSkiElbowR][2]);
    MdtSpringSetStiffness(jetski->elbowSpring[2], 100);
    MdtSpringSetDamping(jetski->elbowSpring[2], 10);
    MdtSpringEnable(jetski->elbowSpring[2]);

    jetski->currentPose = 0;

    return jetski;
}

void JetSkiDestroy(JetSki* jetski)
{
    MeMemoryAPI.destroy(jetski->body);
    MeMemoryAPI.destroy(jetski->bodyG);
    MeMemoryAPI.destroy(jetski->joint);
    MeMemoryAPI.destroyAligned(jetski->jointTM);
    MeMemoryAPI.destroy(jetski);
}

/* Update graphic transforms from dynamics and joint positions. */
void JetSkiUpdateMatrices(JetSki* jetski)
{
    int i;

/*  MeMatrix4Multiply(jetski->headTM, MdtBodyGetTransformPtr(jetski->body[kJetSkiTorso]), bodyToHeadTM);*/
    MeMatrix4MultiplyMatrix(jetski->headTM, bodyToHeadTM, MdtBodyGetTransformPtr(jetski->body[kJetSkiTorso]));
/*  MeMatrix4Multiply(jetski->seatTM, MdtBodyGetTransformPtr(jetski->body[kJetSkiHull]), hullToSeatTM); */
    MeMatrix4MultiplyMatrix(jetski->seatTM, hullToSeatTM, MdtBodyGetTransformPtr(jetski->body[kJetSkiHull]));
/*  MeMatrix4Multiply(jetski->prowTM, MdtBodyGetTransformPtr(jetski->body[kJetSkiHull]), hullToProwTM); */
    MeMatrix4MultiplyMatrix(jetski->prowTM, hullToProwTM, MdtBodyGetTransformPtr(jetski->body[kJetSkiHull]));

    for(i=0; i<kJetSkiMaxJoints; i++)
    {
        MeVector3 pos;

        if(jointIsHinge[i])
            MdtHingeGetPosition(MdtConstraintDCastHinge(jetski->joint[i]), pos);
        else
            MdtBSJointGetPosition(MdtConstraintDCastBSJoint(jetski->joint[i]), pos);

        MeMatrix4TMSetPosition(jetski->jointTM[i], pos[0], pos[1], pos[2]);
    }
}

void JetSkiNextPose(JetSki* jetski)
{
    MdtLimitID limit;

    jetski->currentPose = (jetski->currentPose + 1) % NPOSES;

    limit = MdtHingeGetLimit(MdtConstraintDCastHinge(jetski->joint[kJetSkiAnkles]));
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), pose[jetski->currentPose][0]);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), pose[jetski->currentPose][0]);

    limit = MdtHingeGetLimit(MdtConstraintDCastHinge(jetski->joint[kJetSkiKnees]));
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), pose[jetski->currentPose][1]);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), pose[jetski->currentPose][1]);

    limit = MdtHingeGetLimit(MdtConstraintDCastHinge(jetski->joint[kJetSkiHips]));
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), pose[jetski->currentPose][2]);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), pose[jetski->currentPose][2]);
}

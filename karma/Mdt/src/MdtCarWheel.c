/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:59 $ - Revision: $Revision: 1.33.6.3 $

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

#include <string.h>

#include <MeMath.h>
#include <MdtCheckMacros.h>
#include "MdtUtils.h"
#include <MdtConstraint.h>
#include <MdtBody.h>
#include <MdtCarWheel.h>
#include <MdtUtilities.h>

/**
 * Creates a new car wheel constraint with default parameters.
 *
 * Note that the chassis of the car must be the first body.
 *
 * @see MdtCarWheelReset
 */
MdtCarWheelID MEAPI MdtCarWheelCreate(const MdtWorldID w)
{
    MdtCarWheel *j;
    MdtCHECKWORLD(w,"MdtCarWheelCreate");

    j = (MdtCarWheelID)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtCarWheelCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclCARWHEEL;
    j->head.bclFunction = MdtBclAddCarWheel;
    j->head.maxRows = MdtBclGETMAXROWSHINGE;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtCarWheelReset(j);

    return j;
}

/**
 * Sets a car wheel joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * @param j car wheel joint ID
 */
void MEAPI MdtCarWheelReset(const MdtCarWheelID j)
{
    MdtConstraintID c;
    MdtCHECKCARWHEEL(j,"MdtCarWheelReset");

    c= MdtCarWheelQuaConstraint(j);

    if(MdtCarWheelIsEnabled(j))
        MdtCarWheelDisable(j);

    j->head.ref1[3][0] = MDTCARWHEEL_DEFAULT_POS_X;
    j->head.ref1[3][1] = MDTCARWHEEL_DEFAULT_POS_Y;
    j->head.ref1[3][2] = MDTCARWHEEL_DEFAULT_POS_Z;

    MdtConstraintBodySetAxesRel(c,0,
        MDTCARWHEEL_DEFAULT_STEER_AXIS_X,
        MDTCARWHEEL_DEFAULT_STEER_AXIS_Y,
        MDTCARWHEEL_DEFAULT_STEER_AXIS_Z,
        MDTCARWHEEL_DEFAULT_HINGE_AXIS_X,
        MDTCARWHEEL_DEFAULT_HINGE_AXIS_Y,
        MDTCARWHEEL_DEFAULT_HINGE_AXIS_Z);

    MdtConstraintBodySetAxesRel(c,1,
        MDTCARWHEEL_DEFAULT_STEER_AXIS_X,
        MDTCARWHEEL_DEFAULT_STEER_AXIS_Y,
        MDTCARWHEEL_DEFAULT_STEER_AXIS_Z,
        MDTCARWHEEL_DEFAULT_HINGE_AXIS_X,
        MDTCARWHEEL_DEFAULT_HINGE_AXIS_Y,
        MDTCARWHEEL_DEFAULT_HINGE_AXIS_Z);

    j->skp = MDTCARWHEEL_DEFAULT_SUSP_STIFF;
    j->skd = MDTCARWHEEL_DEFAULT_SUSP_DAMP;
    j->svel = MDTCARWHEEL_DEFAULT_STEER_DESVEL;
    j->sfmax = MDTCARWHEEL_DEFAULT_STEER_MAXFORCE;
    j->hvel = MDTCARWHEEL_DEFAULT_HINGE_DESVEL;
    j->hfmax = MDTCARWHEEL_DEFAULT_HINGE_MAXFORCE;
    j->slock = MDTCARWHEEL_DEFAULT_STEER_LOCK;
    j->shi = MDTCARWHEEL_DEFAULT_SUSP_HI;
    j->slo = MDTCARWHEEL_DEFAULT_SUSP_LO;
    j->sref = MDTCARWHEEL_DEFAULT_SUSP_REF;
    j->slsoft = MDTCARWHEEL_DEFAULT_SUSP_SOFT;

    BaseConstraintReset(c);
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtCarWheelQuaConstraint(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelQuaConstraint");
    return (MdtBaseConstraint *) j;
}

/**
 * Gets an MdtCarWheelID from an MdtConstraintID.
 * If this constraint is not a CarWheel, returns 0.
 */
MdtCarWheelID MEAPI MdtConstraintDCastCarWheel(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastCarWheel");
    if(c->head.tag == MdtBclCARWHEEL)
        return (MdtCarWheel *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * Returns the wheel joint's steering angle.
 */
MeReal MEAPI MdtCarWheelGetSteeringAngle(const MdtCarWheelID j)
{
    MeVector3 h, h2, foo;
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSteeringAngle");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetSteeringAngle");

    MdtConvertVector(j->head.mdtbody[0], j->head.ref1[1], 0, h2);
    MdtConvertVector(j->head.mdtbody[1], j->head.ref2[1], 0, h);
    MdtConvertVector(j->head.mdtbody[0], j->head.ref1[2], 0, foo);

    return MeAtan2(-MeVector3Dot(h, foo), MeVector3Dot(h, h2));
}

/**
 * Returns the wheel joint's steering angle rate.
 */
MeReal MEAPI MdtCarWheelGetSteeringAngleRate(const MdtCarWheelID j)
{
    MeVector3 sa, w_rel;
    int k;
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSteeringAngleRate");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetSteeringAngleRate");

    MdtConvertVector(j->head.mdtbody[0], j->head.ref1[0], 0, sa);

    for (k = 0; k < 3; k++)
        w_rel[k] =
            j->head.mdtbody[1]->keaBody.velrot[k] -
            j->head.mdtbody[0]->keaBody.velrot[k];

    return MeVector3Dot(w_rel, sa);
}

/**
 * The wheel joint's steering axis is returned in @a v.
 */
void MEAPI MdtCarWheelGetSteeringAxis(const MdtCarWheelID j, MeVector3 v)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSteeringAxis");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetSteeringAxis");

    MdtConvertVector(j->head.mdtbody[0], j->head.ref1[0], 0, v);
}

/**
 * Returns the wheel joint's current hinge angle as a value between zero
 * and pi radians, inclusive.
 */
MeReal MEAPI MdtCarWheelGetHingeAngle(const MdtCarWheelID j)
{
    MeVector3 s1, s2, s3;
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetHingeAngle");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetHingeAngle");

    /*
      Find the angle of rotation between the two body reference
      frames.
    */
    MdtConvertVector(j->head.mdtbody[0], j->head.ref1[0], 0, s1);
    MdtConvertVector(j->head.mdtbody[1], j->head.ref2[0], 0, s2);
    MdtConvertVector(j->head.mdtbody[1], j->head.ref2[1], 0, s3);

    return MeAtan2(MeVector3Dot(s2, s3), MeVector3Dot(s1, s2));
}

/**
 * Returns the wheel joint's hinge angle rate.
 */
MeReal MEAPI MdtCarWheelGetHingeAngleRate(const MdtCarWheelID j)
{
    MeVector3 ha, w_rel;
    int k;
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetHingeAngleRate");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetHingeAngleRate");

    MdtConvertVector(j->head.mdtbody[1], j->head.ref2[1], 0, ha);

    for (k = 0; k < 3; k++)
        w_rel[k] =
            j->head.mdtbody[1]->keaBody.velrot[k] -
            j->head.mdtbody[0]->keaBody.velrot[k];
    return MeVector3Dot(w_rel, ha);
}

/**
 * The wheel joint's Hinge axis is returned in @a v
 */
void MEAPI MdtCarWheelGetHingeAxis(const MdtCarWheelID j, MeVector3 v)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetHingeAxis");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetHingeAxis");

    MdtConvertVector(j->head.mdtbody[1], j->head.ref2[1], 0, v);
}

/**
 * Returns the wheel joint's suspension height.
 */
MeReal MEAPI MdtCarWheelGetSuspensionHeight(const MdtCarWheelID j)
{
    MeVector3 sa, p_rel;
    int k;
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionHeight");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetSuspensionHeight");

    MdtConvertVector(j->head.mdtbody[0], j->head.ref1[0], 0, sa);

    for (k = 0; k < 3; k++)
        p_rel[k] =
            j->head.mdtbody[0]->comTM[3][k] - j->head.mdtbody[1]->comTM[3][k];

    return MeVector3Dot(p_rel, sa);
}

/**
 * Returns the wheel joint's suspension rate.
 */
MeReal MEAPI MdtCarWheelGetSuspensionRate(const MdtCarWheelID j)
{
    MeVector3 sa, v_rel, at, at_cross_sa;
    int k;

    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionRate");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelGetSuspensionRate");

    MdtConvertVector(j->head.mdtbody[0], j->head.ref1[0], 0, sa);
    MdtConvertPositionVector(j->head.mdtbody[0], j->head.ref1[3], 0, at);
    MeVector3Cross(at_cross_sa, at, sa);
    for (k = 0; k < 3; k++)
        v_rel[k] =
            j->head.mdtbody[0]->keaBody.vel[k] -
            j->head.mdtbody[1]->keaBody.vel[k];

    return MeVector3Dot(v_rel, sa) +
           MeVector3Dot(j->head.mdtbody[0]->keaBody.velrot, at_cross_sa);
}

/**
 * Returns the desired velocity of the steering motor
 */
MeReal MEAPI MdtCarWheelGetSteeringMotorDesiredVelocity(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSteeringMotorDesiredVelocity");
    return j->svel;
}

/**
 * Returns the maximum force that the steering motor
 * is allowed to use to attain its desired velocity.
 */
MeReal MEAPI MdtCarWheelGetSteeringMotorMaxForce(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSteeringMotorMaxForce");
    return j->sfmax;
}


/**
 * Returns the lock state of the steering angle.
 *
 * lock is 1 if steering axis is locked at angle 0.
 */
MeBool MEAPI MdtCarWheelIsSteeringLocked(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelIsSteeringLocked");
    return j->slock;
}

/**
 * Returns the desired velocity of the hinge motor.
 */
MeReal MEAPI MdtCarWheelGetHingeMotorDesiredVelocity(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetHingeMotorDesiredVelocity");
    return j->hvel;
}

/**
 * Returns the maximum force that the hinge motor
 * is allowed to use to attain its desired velocity.
 */
MeReal MEAPI MdtCarWheelGetHingeMotorMaxForce(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetHingeMotorMaxForce");
    return j->hfmax;
}

/**
 * Returns the suspension upper limit.
 */
MeReal MEAPI MdtCarWheelGetSuspensionHighLimit(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionHighLimit");
    return j->shi;
}

/**
 * Returns the suspension lower limit.
 */
MeReal MEAPI MdtCarWheelGetSuspensionLowLimit(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionLowLimit");
    return j->slo;
}

/**
 * Returns the suspension limit softness.
 */
MeReal MEAPI MdtCarWheelGetSuspensionLimitSoftness(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionLimitSoftness");
    return j->slsoft;
}

/**
 * Returns the suspension attachment point ("reference").
 */
MeReal MEAPI MdtCarWheelGetSuspensionReference(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionReference");
    return j->sref;
}


/**
 * Returns the suspension "proportionality constant".
 *
 * This gives rise to the spring term in the suspension
 * force equation: f = kp*displacement + kd*velocity.
 */
MeReal MEAPI MdtCarWheelGetSuspensionKp(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionKp");
    return j->skp;
}


/**
 * Returns the suspension "damping constant" (also known as the
 * "derivative constant").
 *
 * This gives rise to the damping term in the suspension force equation:
 * f = kp*displacement + kd*velocity
 */
MeReal MEAPI MdtCarWheelGetSuspensionKd(const MdtCarWheelID j)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelGetSuspensionKd");
    return j->skd;
}

/*
 * Mutators
 */

/**
 * Sets the limited force motor parameters of  car wheel joint.
 */
void MEAPI MdtCarWheelSetSteeringLimitedForceMotor(const MdtCarWheelID j,
    const MeReal desiredVelocity, const MeReal forceLimit)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelSetSteeringLimitedForceMotor");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelSetSteeringLimitedForceMotor");

    j->svel = desiredVelocity;
    j->sfmax = forceLimit;

#ifdef _MECHECK
    /* warn if user is trying to use motor but bodies are disabled
    (they must enable them themselves) */
    if(!MdtBodyIsEnabled(j->head.mdtbody[0]) || !MdtBodyIsEnabled(j->head.mdtbody[1]))
    {
        MeWarning(12, "Using Car Wheel Motor but bodies are Disabled.");
    }
#endif
}

/**
 * Locks or unlocks the steering angle.
 *
 * @arg lock is 1 if steering axis is locked at angle 0
 */
void MEAPI MdtCarWheelSetSteeringLock(const MdtCarWheelID j, const MeBool lock)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelSetSteeringLock");
    j->slock = lock;
}


/**
 * Sets the hinge limited force motor parameters.
 */
void MEAPI MdtCarWheelSetHingeLimitedForceMotor(const MdtCarWheelID j,
    const MeReal desiredVelocity, const MeReal forceLimit)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelSetHingeLimitedForceMotor");
    MdtCHECKCONSTRAINTBODIES(j,"MdtCarWheelSetHingeLimitedForceMotor");

    j->hvel = desiredVelocity;
    j->hfmax = forceLimit;

#ifdef _MECHECK
    /* warn if user is trying to use motor but bodies are disabled
    (they must enable them themselves) */
    if(!MdtBodyIsEnabled(j->head.mdtbody[0]) || !MdtBodyIsEnabled(j->head.mdtbody[1]))
    {
        MeWarning(12, "Using Car Wheel Motor but bodies are Disabled.");
    }
#endif
}

/**
 * Sets the suspension parameters.
 */
void MEAPI MdtCarWheelSetSuspension(const MdtCarWheelID j, const MeReal Kp,
                              const MeReal Kd, const MeReal limit_softness,
                              const MeReal lolimit, const MeReal hilimit,
                              const MeReal reference)
{
    MdtCHECKCARWHEEL(j,"MdtCarWheelSetSuspension");

    j->skp = Kp;
    j->skd = Kd;
    j->slsoft = limit_softness;
    j->slo = lolimit;
    j->shi = hilimit;
    j->sref = reference;
}




/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/16 10:20:41 $ - Revision: $Revision: 1.5.2.3 $

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

#include <MePrecision.h>
#include <MeMath.h>
#include "MdtUtils.h"
#include <MdtCheckMacros.h>
#include <MdtConstraint.h>
#include <MdtSkeletal.h>
#include <MdtLimit.h>
#include <MdtBody.h>

/**
 * Creates a new skeletal joint with default parameters.
 *
 * @see MdtSkeletalReset
 */
MdtSkeletalID MEAPI MdtSkeletalCreate(const MdtWorldID w)
{
    MdtSkeletal *j;
    MdtCHECKWORLD(w,"MdtSkeletalCreate");

    j = (MdtSkeletal*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSkeletalCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclSKELETAL;
    j->head.bclFunction = MdtBclAddSkeletal;
    j->head.maxRows = MdtBclGETMAXROWSSKELETAL;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtSkeletalReset(j);

    return j;
}

/**
 * Initialises a skeletal joint and sets default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg combinedLimits = FALSE
 * @arg twistOption = MdtSkeletalTwistOptionFixed
 * @arg coneOption = MdtSkeletalConeOptionCone
 * @arg twist_limit_angle = ME_PI/4;
 * @arg cone_limit_angle_1 = ME_PI/4;
 * @arg cone_limit_angle_2 = ME_PI/4;
 * @arg twist_stiffness = MEINFINITY;
 * @arg cone_stiffness = MEINFINITY;
 * @arg twist_damping = MEINFINTY;
 * @arg cone_damping = MEINFINTY;
 * @param j skeletal joint ID
 */
void MEAPI MdtSkeletalReset(const MdtSkeletalID j)
{
    MdtCHECKSKELETAL(j,"MdtSkeletalReset");

    if(MdtSkeletalIsEnabled(j))
        MdtSkeletalDisable(j);

    j->combinedLimits = 0;
    j->twistOption = MdtSkeletalTwistOptionFixed;
    j->coneOption = MdtSkeletalConeOptionCone;
    j->cos_half_twist_limit_angle =
        MeCos((MeReal)0.5 * MDTSKELETAL_DEFAULT_TWIST_ANGLE);
    j->cos_half_cone_limit_angle_1 =
        MeCos((MeReal)0.5 * MDTSKELETAL_DEFAULT_PRI_ANGLE);
    j->cos_half_cone_limit_angle_2 =
        MeCos((MeReal)0.5 * MDTSKELETAL_DEFAULT_SEC_ANGLE);
    j->twist_stiffness = MEINFINITY;
    j->cone_stiffness = MEINFINITY;
    j->twist_damping = MEINFINITY;
    j->cone_damping = MEINFINITY;

    BaseConstraintReset(MdtSkeletalQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtSkeletalQuaConstraint(const MdtSkeletalID j)
{
    MdtCHECKSKELETAL(j,"MdtSkeletalQuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtSkeletalID from an MdtConstraintID.
 * If this constraint is not a Skeletal, returns 0.
 */
MdtSkeletalID MEAPI MdtConstraintDCastSkeletal(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastSkeletal");
    if(c->head.tag == MdtBclSKELETAL)
        return (MdtSkeletal *) c;
    else
        return 0;
}


/*
  Accessors
*/

/** Returns the current type of cone limit. */
MdtSkeletalConeOption MEAPI MdtSkeletalGetConeOption(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetConeOption");
    return j->coneOption;
}

/** 
 *  Returns the primary angle used for the cone limit. 
 *  Only used if ConeOption is MdtSkeletalConeOptionCone or 
 *  MdtSkeletalConeOptionSlot.
 */
MeReal MEAPI MdtSkeletalGetConePrimaryLimitAngle(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetConePrimaryLimitAngle");
    return 2*MeAcos(j->cos_half_cone_limit_angle_1);
}

/** 
 *  Returns the secondary angle used for the cone limit. 
 *  Only used if ConeOption is MdtSkeletalConeOptionCone.
 */
MeReal MEAPI MdtSkeletalGetConeSecondaryLimitAngle(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetConeSecondaryLimitAngle");
    return 2*MeAcos(j->cos_half_cone_limit_angle_2);
}

/**
 * Get cone 'stiffness' of the skeletal joint
 * @see MdtSkeletalSetConeStiffness
 */
MeReal MEAPI MdtSkeletalGetConeStiffness(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetConeStiffness");
    return j->cone_stiffness;
}

/**
 * Get cone 'damping' of the skeletal joint
 * @see MdtSkeletalSetConeDamping
 */
MeReal MEAPI MdtSkeletalGetConeDamping(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetConeDamping");
    return j->cone_damping;
}

/** Returns the current twist limit type. */
MdtSkeletalTwistOption MEAPI MdtSkeletalGetTwistOption(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetTwistOption");
    return j->twistOption;
}

/** 
 *  Returns the angle to which twist is limited +/- 
 *  Only used if TwistOption is MdtSkeletalTwistOptionLimited
 */
MeReal MEAPI MdtSkeletalGetTwistLimitAngle(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetTwistLimitAngle");
    return 2*MeAcos(j->cos_half_twist_limit_angle);
}

/**
 * Get twist 'stiffness' of the skeletal joint
 * @see MdtSkeletalSetTwistStiffness
 */
MeReal MEAPI MdtSkeletalGetTwistStiffness(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetTwistStiffness");
    return j->twist_stiffness;
}

/**
 * Get twist 'damping' of the skeletal joint
 * @see MdtSkeletalSetTwistDamping
 */
MeReal MEAPI MdtSkeletalGetTwistDamping(const MdtSkeletalID j)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalGetTwistDamping");
    return j->twist_damping;
}   


/*************/
/*
  Mutators
*/

/**
 *  An alternative to setting body0's orthogonal vector is to call this 
 *  function.  It defines the perpendiculars such that the present 
 *  configuration is zero-twist.
 */
void MEAPI MdtSkeletalZeroTwist(const MdtSkeletalID j)
{
    MeReal cosTheta;

    MdtCHECKSKELETAL(j,"MdtSkeletalZeroTwist");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtSkeletalZeroTwist");

    cosTheta = MeVector3Dot(j->head.ref1[0], j->head.ref2[0]);
    if (cosTheta > 1-ME_SMALL_EPSILON)
    {
        /* Identity */
        MeMatrix4Copy(j->head.ref2, j->head.ref1);
    } else
    if (cosTheta > ME_SMALL_EPSILON-1)
    {
        MeMatrix4 rot;
        MeVector4 qRot;
        MeVector3 axis;

        /* Proper rotation */
        MeReal sinHalfTheta = MeSqrt(((MeReal)0.5)*(1-cosTheta));
        MeVector3Cross(axis, j->head.ref1[0], j->head.ref2[0]);
        MeVector3Normalize(axis);
        MeVector3Scale(axis, sinHalfTheta);
        MeQuaternionSet(qRot, 
            MeSqrt(1-sinHalfTheta*sinHalfTheta), 
            axis[0], axis[1], axis[2]);
        MeQuaternionToTM(rot, qRot);

        MeMatrix4TMRotate(j->head.ref2[2], rot, j->head.ref1[2]);
        MeVector3Cross(j->head.ref2[1], j->head.ref2[2], j->head.ref2[0]);
        MeVector3Normalize(j->head.ref2[1]);
        MeVector3Cross(j->head.ref2[2], j->head.ref2[0], j->head.ref2[1]);
    }
}

/** 
 *  Set the current cone limit type used. 
 *  @see MdtSkeletalConeOption
 */
void MEAPI MdtSkeletalSetConeOption(const MdtSkeletalID j, 
                                    MdtSkeletalConeOption co)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetConeOption");
    j->coneOption = co;
}

/** 
 *  Set the primary angle used for the cone limit. 
 *  Only used if ConeOption is MdtSkeletalConeOptionCone or 
 *  MdtSkeletalConeOptionSlot.
 */
void MEAPI MdtSkeletalSetConePrimaryLimitAngle(const MdtSkeletalID j, 
                                               const MeReal theta)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetConePrimaryLimitAngle");
    j->cos_half_cone_limit_angle_1 = MeCos((MeReal)0.5 * theta);
}

/** 
 *  Set the secondary angle used for the cone limit. 
 *  Only used if ConeOption is MdtSkeletalConeOptionCone.
 */
void MEAPI MdtSkeletalSetConeSecondaryLimitAngle(const MdtSkeletalID j, 
                                                 const MeReal theta)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetConeSecondaryLimitAngle");
    j->cos_half_cone_limit_angle_2 = MeCos((MeReal)0.5 * theta);
}

/**
 * Set cone 'stiffness' of the skeletal joint
 * By default this is MEINFINITY.
 * @see MdtSkeletalSetConeDamping
 */
void MEAPI MdtSkeletalSetConeStiffness(const MdtSkeletalID j, const MeReal kp)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetConeStiffness");
    j->cone_stiffness = kp;
}

/**
 * Set cone 'damping' of the skeletal joint
 * By default is MEINFINITY.
 * @see MdtSkeletalSetConeStiffness
 */
void MEAPI MdtSkeletalSetConeDamping(const MdtSkeletalID j, const MeReal kd)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetConeDamping");
    j->cone_damping = kd;
}

/** 
 *  Set the curent twist limit type to use. 
 *  @see MdtSkeletalTwistOption
 */
void MEAPI MdtSkeletalSetTwistOption(const MdtSkeletalID j, 
                                     MdtSkeletalTwistOption to)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetTwistOption");
    j->twistOption = to;
}

/** 
 *  Set the angle to which twist is limited +/- 
 *  Only used if TwistOption is MdtSkeletalTwistOptionLimited
 */
void MEAPI MdtSkeletalSetTwistLimitAngle(const MdtSkeletalID j, 
                                         const MeReal theta)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetTwistLimitAngle");
    j->cos_half_twist_limit_angle = MeCos((MeReal)0.5 * theta);
}

/**
 * Set twist 'stiffness' of the skeletal joint
 * By default this is MEINFINITY.
 * @see MdtSkeletalSetTwistDamping
 */
void MEAPI MdtSkeletalSetTwistStiffness(const MdtSkeletalID j, const MeReal kp)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetTwistStiffness");
    j->twist_stiffness = kp;
}

/**
 * Set twist 'damping' of the skeletal joint
 * By default is MEINFINITY.
 * @see MdtSkeletalSetTwistStiffness
 */
void MEAPI MdtSkeletalSetTwistDamping(const MdtSkeletalID j, const MeReal kd)
{
    MdtCHECKCONSTRAINT(j, "MdtSkeletalSetTwistDamping");
    j->twist_damping = kd;
}


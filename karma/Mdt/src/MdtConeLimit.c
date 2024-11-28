/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.13.6.1 $

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
#include "MdtUtils.h"
#include <MdtCheckMacros.h>
#include <MdtConstraint.h>
#include <MdtConeLimit.h>


/**
 * Creates a new cone limit constraint with default parameters.
 * @see MdtConeLimitReset
 */
MdtConeLimitID MEAPI MdtConeLimitCreate(const MdtWorldID w)
{
    MdtConeLimit *j;
    MdtCHECKWORLD(w,"MdtConeLimitCreate");

    j = (MdtConeLimit*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtConeLimitCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclCONELIMIT;
    j->head.bclFunction = MdtBclAddConeLimit;
    j->head.maxRows = MdtBclGETMAXROWSCONELIMIT;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtConeLimitReset(j);

    return j;
}

/**
 * Sets a cone limit constraint to its default values.
 * Note that the constraint's bodies are not reset.
 * This will disable the constraint.
 *
 * A default cone limit constraint has the following characteristics:
 *
 * @arg Position  = {0,0,0} (in both bodies' reference frame)
 * @arg Primary axis = {1,0,0} (in both bodies' reference frame)
 * @arg Orthogonal axis = {0,1,0} (in both bodies' reference frame)
 * @arg stiffness = INFINITY
 * @arg damping = INFINITY
 *
 * @param j cone limit constraint ID
 */
void MEAPI MdtConeLimitReset(const MdtConeLimitID j)
{
    MdtCHECKCONELIMIT(j,"MdtConeLimitReset");

    if(MdtConeLimitIsEnabled(j))
        MdtConeLimitDisable(j);

    j->cone_half_angle = ME_PI;
    j->cos_cone_half_angle = -1;
    j->stiffness = MEINFINITY;
    j->damping = MEINFINITY;

    /* Call base-class reset function. */
    BaseConstraintReset(MdtConeLimitQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtConeLimitQuaConstraint(const MdtConeLimitID j)
{
    MdtCHECKCONELIMIT(j,"MdtConeLimitQuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtConeLimitID from an MdtConstraintID.
 * If this constraint is not a ConeLimit, returns 0.
 */
MdtConeLimitID MEAPI MdtConstraintDCastConeLimit(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastConeLimit");
    if(c->head.tag == MdtBclCONELIMIT)
        return (MdtConeLimit *) c;
    else
        return 0;
}

/**
 * Sets the cone angle of the ConeLimit, actually the half-angle
 * between the symmetry axis and the side of the cone.
 */
void MEAPI MdtConeLimitSetConeHalfAngle(const MdtConeLimitID c, const MeReal theta)
{
    MdtCHECKCONELIMIT(c,"MdtConeLimitSetConeHalfAngle");
    c->cone_half_angle = theta;
    c->cos_cone_half_angle = MeCos(theta);
}

/**
 * Gets the cone angle of the ConeLimit, actually the half-angle
 * between the symmetry axis and the side of the cone.
 */
MeReal MEAPI MdtConeLimitGetConeHalfAngle(const MdtConeLimitID c)
{
    MdtCHECKCONELIMIT(c,"MdtConeLimitGetConeHalfAngle");
    return c->cone_half_angle;
}

/**
 * Set 'stiffness' of the ConeLimit
 * By default this is MEINFINITY.
 * @see MdtConeLimitSetDamping
 */
void MEAPI MdtConeLimitSetStiffness(const MdtConeLimitID c, const MeReal kp)
{
    MdtCHECKCONELIMIT(c,"MdtConeLimitSetStiffness");
    c->stiffness = kp;
}

MeReal MEAPI MdtConeLimitGetStiffness(const MdtConeLimitID c)
{
    MdtCHECKCONELIMIT(c,"MdtConeLimitGetStiffness");
    return c->stiffness;
}

/**
 * Set 'damping' of the ConeLimit
 * By default is MEINFINITY.
 * @see MdtConeLimitSetStiffness
 */
void MEAPI MdtConeLimitSetDamping(const MdtConeLimitID c, const MeReal kd)
{
    MdtCHECKCONELIMIT(c,"MdtConeLimitSetDamping");
    c->damping = kd;
}

MeReal MEAPI MdtConeLimitGetDamping(const MdtConeLimitID c)
{
    MdtCHECKCONELIMIT(c,"MdtConeLimitGetDamping");
    return c->damping;
}


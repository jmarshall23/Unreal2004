/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.28.6.3 $

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
#include <MdtLimit.h>
#include <MdtConstraint.h>
#include <MdtPrismatic.h>

void MEAPI PrismaticSetAxis(const MdtConstraintID c,
               const MeReal px, const MeReal py, const MeReal pz);

/**
 * Creates a new prismatic constraint with default parameters.
 *
 * @see MdtPrismaticReset
 */
MdtPrismaticID MEAPI MdtPrismaticCreate(const MdtWorldID w)
{
    MdtPrismatic *j;
    MdtCHECKWORLD(w,"MdtPrismaticCreate");

    j = (MdtPrismatic*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtPrismaticCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclPRISMATIC;
    j->head.bclFunction = MdtBclAddPrismatic;
    j->head.maxRows = MdtBclGETMAXROWSPRISMATIC;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = PrismaticSetAxis;

    MdtPrismaticReset(j);

    return j;
}

/**
 * Sets a prismatic joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg sliding axis = {0,1,0} (in first body's reference frame)
 * @arg first body's initial position = {0,0,0} (in inertial
 *  reference frame)
 *
 * @param j prismatic joint ID
 */
void MEAPI MdtPrismaticReset(const MdtPrismaticID j)
{
    MdtCHECKPRISMATIC(j,"MdtPrismaticReset");

    if(MdtPrismaticIsEnabled(j))
        MdtPrismaticDisable(j);

    /* Initialize the joint limit. */
    MdtLimitReset(&j->limit);

    BaseConstraintReset(MdtPrismaticQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtPrismaticQuaConstraint(const MdtPrismaticID j)
{
    MdtCHECKPRISMATIC(j,"MdtPrismaticQuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtPrismaticID from an MdtConstraintID.
 * If this constraint is not a Prismatic, returns 0.
 */
MdtPrismaticID MEAPI MdtConstraintDCastPrismatic(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastPrismatic");
    if(c->head.tag == MdtBclPRISMATIC)
        return (MdtPrismatic *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * The Prismatic joint's axis is returned in @a v.
 */
/*  WW replaced with a #define
void MEAPI MdtPrismaticGetAxis(const MdtPrismaticID j, MeVector3 v)
{
    MdtCHECKPRISMATIC(j,"MdtPrismaticGetAxis");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtPrismaticGetAxis");

    ConvertCOMVector(j->head.body[0], j->axis1, 0, v);
}
*/

/**
 * Provides read/write access to the constraint limit.
 */
MdtLimitID MEAPI MdtPrismaticGetLimit(const MdtPrismaticID j)
{
    MdtCHECKPRISMATIC(j,"MdtPrismaticGetLimit");
    return &j->limit;
}

/*
  Mutators
*/

/**
 * Set the sliding axis direction of the prismatic joint in the
 * world ref. frame.
 */
/* Initializes constraint frames in the primary and secondary bodies;
 * the primary frame is fixed at its CoM with the x-axis defining the
 * sliding axis, the secondary frame has the same orientation and it's
 * position is offset to also to originate at the primary CoM
 */

void MEAPI PrismaticSetAxis(const MdtConstraintID c,
               const MeReal px, const MeReal py, const MeReal pz)
{
    MdtPrismaticID j = MdtConstraintDCastPrismatic(c);

    MeVector3 axis, r_axis1, r_axis2, rel_pos;
    MeReal magSqr;

    MdtCHECKPRISMATIC(j,"PrismaticSetAxis");

    axis[0] = px;
    axis[1] = py;
    axis[2] = pz;

    /* Calculate relative position */
    rel_pos[0] = (j->head.mdtbody[0])->comTM[3][0];
    rel_pos[1] = (j->head.mdtbody[0])->comTM[3][1];
    rel_pos[2] = (j->head.mdtbody[0])->comTM[3][2];

    if (c->head.mdtbody[1] != 0) {
        rel_pos[0] -= (j->head.mdtbody[1])->comTM[3][0];
        rel_pos[1] -= (j->head.mdtbody[1])->comTM[3][1];
        rel_pos[2] -= (j->head.mdtbody[1])->comTM[3][2];
    }

    magSqr = MeVector3MagnitudeSqr(axis);

    /* Check axis is unit length. */
    if(!ME_IS_ZERO_TOL(magSqr - 1, ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12,
            "PrismaticSetAxis: Constraint axis should be unit length.");
#endif
        MeVector3Normalize(axis);
    }

    /* Use PlaneSpace to calculate the rest of the rotation matrix. */
    MeVector3PlaneSpace(axis, r_axis1, r_axis2);

    ConvertCOMVector(0, axis,    j->head.mdtbody[0], j->head.ref1[0]);
    ConvertCOMVector(0, r_axis1, j->head.mdtbody[0], j->head.ref1[1]);
    ConvertCOMVector(0, r_axis2, j->head.mdtbody[0], j->head.ref1[2]);

    ConvertCOMVector(0, axis,    j->head.mdtbody[1], j->head.ref2[0]);
    ConvertCOMVector(0, r_axis1, j->head.mdtbody[1], j->head.ref2[1]);
    ConvertCOMVector(0, r_axis2, j->head.mdtbody[1], j->head.ref2[2]);

    ConvertCOMVector(0, rel_pos, j->head.mdtbody[1], j->head.ref2[3]);

#ifdef _MECHECK
    {
        MeMatrix3 R;

        MeMatrix4TMGetRotation( R, j->head.ref1);
        if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
            MeWarning(12, "PrismaticSetAxis: Constraint (body1) "
                "matrix is invalid.");

        MeMatrix4TMGetRotation( R, j->head.ref2);
        if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
            MeWarning(12, "PrismaticSetAxis: Constraint (body2) "
                "matrix is invalid.");
    }
#endif
}

/**
 * Resets the joint limit and then copies the public attributes of NewLimit.
 */
void MEAPI MdtPrismaticSetLimit(const MdtPrismaticID j,
    const MdtLimitID NewLimit)
{
    MdtLimitID Limit;
    MdtCHECKPRISMATIC(j,"MdtPrismaticSetLimit");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtPrismaticSetLimit");

#ifdef _MECHECK
    if (NewLimit == 0)
        MeFatalError(0,
            "Invalid NewLimit supplied to MdtPrismaticSetLimit()");
#endif

    Limit = MdtPrismaticGetLimit(j);

    MdtLimitReset(Limit);
    MdtLimitSetLowerLimit(Limit, MdtLimitGetLowerLimit(NewLimit));
    MdtLimitSetUpperLimit(Limit, MdtLimitGetUpperLimit(NewLimit));
    MdtLimitActivateLimits(Limit, MdtLimitIsActive(NewLimit));
    MdtLimitSetLimitedForceMotor(Limit,
    MdtLimitGetMotorDesiredVelocity(NewLimit),
    MdtLimitGetMotorMaxForce(NewLimit));
    MdtLimitActivateMotor(Limit, MdtLimitIsMotorized(NewLimit));
}


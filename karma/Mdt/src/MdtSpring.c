/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.28.6.2 $

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
#include <MdtLimit.h>
#include <MdtSpring.h>

/**
 * Creates a new spring with default parameters.
 *
 * @see MdtSpringReset
 */
MdtSpringID MEAPI MdtSpringCreate(const MdtWorldID w)
{
    MdtSpring *j;
    MdtCHECKWORLD(w,"MdtSpringCreate");

    j = (MdtSpring*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpringCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclSPRING;
    j->head.bclFunction = MdtBclAddSpring;
    j->head.maxRows = MdtBclGETMAXROWSSPRING;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtSpringReset(j);

    return j;
}

/**
 * Sets a spring to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg pos1 = {0,0,0}
 * @arg pos2 = {0,0,0}
 *
 * @param j spring joint ID
 */
void MEAPI MdtSpringReset(const MdtSpringID j)
{
    int i;
    MdtCHECKSPRING(j,"MdtSpringReset");

    if(MdtSpringIsEnabled(j))
        MdtSpringDisable(j);

    for (i = 0; i < 3; i++)
    {
        j->pos1[i] = j->pos2[i] = 0;
    }

    /* initialize the joint limit */
    MdtLimitReset( &j->limit );
    /* initialize as the softest possible spring: */
    MdtSpringSetStiffness( j, 1 );
    /* Enable the spring by default: */
    MdtLimitActivateLimits( &j->limit, 1 );

    BaseConstraintReset(MdtSpringQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtSpringQuaConstraint(const MdtSpringID j)
{
    MdtCHECKSPRING(j,"MdtSpringQuaConstraint");
    return (MdtBaseConstraint *) j;
}

/**
 * Gets an MdtSpringID from an MdtConstraintID.
 * If this constraint is not a Spring, returns 0.
 */
MdtSpringID MEAPI MdtConstraintDCastSpring(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastSpring");
    if(c->head.tag == MdtBclSPRING)
        return (MdtSpring *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * The spring joint's attachment position to the given body
 * is returned in @a v.
 */
void MEAPI MdtSpringGetPosition(const MdtSpringID j, MeVector3 v,
                                const unsigned int bodyindex)
{
    MdtCHECKSPRING(j,"MdtSpringGetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtSpringGetPosition");
#ifdef _MECHECK
    if (bodyindex > 1)
        MeFatalError(0, "bodyindex is not valid.");
#endif

    if( bodyindex == 0 )
    {
        ConvertCOMPositionVector(j->head.mdtbody[0], j->pos1, 0, v);
    }
    else
    {
        ConvertCOMPositionVector(j->head.mdtbody[1], j->pos2, 0, v);
    }
}

/**
 * Provides read/write access to the constraint limit.
 */
MdtLimitID MEAPI MdtSpringGetLimit(const MdtSpringID j)
{
    MdtCHECKSPRING(j,"MdtSpringGetLimit");
    return &(j->limit);
}

/*
  Mutators
*/

/**
 * Sets the joint position in world coordinates.
 */
void MEAPI MdtSpringSetPosition(const MdtSpringID j,
                                const unsigned int bodyindex,
                                const MeReal x, const MeReal y, const MeReal z )
{
    MeVector3 tmp;

    MdtCHECKSPRING(j,"MdtSpringSetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtSpringSetPosition");
#ifdef _MECHECK
    if (bodyindex > 1)
        MeFatalError(0, "bodyindex is not valid.");
#endif

    tmp[0] = x;
    tmp[1] = y;
    tmp[2] = z;

    /*
      Convert joint's position from world space to vectors in body1's
      or body2's reference frames as appropriate.
    */
    if( bodyindex == 0 )
    {
        ConvertCOMPositionVector(0, tmp,
            j->head.mdtbody[0], j->pos1);
    }
    else
    {
        ConvertCOMPositionVector(0, tmp,
            j->head.mdtbody[1], j->pos2);
    }

    MdtConstraintBodySetPosition(MdtSpringQuaConstraint(j), bodyindex, x, y, z);
}


/**
 * Resets the joint limit and then copies the public attributes of
 * NewLimit.
 */
void MEAPI MdtSpringSetLimit(const MdtSpringID j, const MdtLimitID NewLimit)
{
    MdtLimitID Limit;
    MdtCHECKSPRING(j,"MdtSpringSetLimit");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtSpringSetLimit");

#ifdef _MECHECK
    if (NewLimit == 0)
        MeFatalError(0, "Invalid NewLimit supplied to MdtSpringSetLimit()");
#endif

    Limit = MdtSpringGetLimit(j);

    MdtLimitReset(Limit);
    MdtLimitSetLowerLimit(Limit, MdtLimitGetLowerLimit(NewLimit));
    MdtLimitSetUpperLimit(Limit, MdtLimitGetUpperLimit(NewLimit));
    MdtLimitActivateLimits(Limit, MdtLimitIsActive(NewLimit));
    MdtLimitSetLimitedForceMotor(Limit,
    MdtLimitGetMotorDesiredVelocity(NewLimit),
    MdtLimitGetMotorMaxForce(NewLimit));
    MdtLimitActivateMotor(Limit, MdtLimitIsMotorized(NewLimit));
}

/*
  Spring "ease-of-use" mutators, which set the given property in both the upper
  and lower limit.
*/
void MEAPI MdtSpringSetNaturalLength( const MdtSpringID j,  const MeReal NewNaturalLength )
{
    MdtLimitID Limit;
    MdtCHECKSPRING(j,"MdtSpringSetNaturalLength");
    /* We do not need the bodies to be attached to set the spring length. */
#ifdef _MECHECK
    if (NewNaturalLength < 0)
        MeFatalError(0, "NewNaturalLength is negative.");
#endif

    Limit = MdtSpringGetLimit(j);
    Limit->limit[0].stop = NewNaturalLength;
    Limit->limit[1].stop = NewNaturalLength;
}

void MEAPI MdtSpringSetStiffness( const MdtSpringID j,  const MeReal NewStiffness )
{
    MdtLimitID Limit;
    MdtCHECKSPRING(j,"MdtSpringSetStiffness");
    /* We do not need the bodies to be attached to set the spring stiffness. */
#ifdef _MECHECK
    if (NewStiffness < 0)
        MeFatalError(0, "NewStiffness is negative.");
#endif

    Limit = MdtSpringGetLimit(j);
    Limit->limit[0].stiffness = NewStiffness;
    Limit->limit[1].stiffness = NewStiffness;
}

void MEAPI MdtSpringSetDamping( const MdtSpringID j,  const MeReal NewDamping )
{
    MdtLimitID Limit;
    MdtCHECKSPRING(j,"MdtSpringSetDamping");
    /* We do not need the bodies to be attached to set the spring damping. */
#ifdef _MECHECK
    if (NewDamping < 0)
        MeFatalError(0, "NewDamping is negative.");
#endif

    Limit = MdtSpringGetLimit(j);
    Limit->limit[0].damping = NewDamping;
    Limit->limit[1].damping = NewDamping;
}


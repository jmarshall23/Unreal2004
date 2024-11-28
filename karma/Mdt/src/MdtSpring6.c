/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.2.2.2 $

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
#include <MdtSpring6.h>
#include <MdtConstraint.h>
#include <MdtBody.h>

void MEAPI Spring6SetBodies(const MdtConstraintID c,
              const MdtBodyID b1, const MdtBodyID b2);

/**
 * Creates a new Spring6 joint with default parameters.
 *
 * @see MdtSpring6Reset
 */
MdtSpring6ID MEAPI MdtSpring6Create(const MdtWorldID w)
{
    MdtSpring6 *j;
    MdtCHECKWORLD(w,"MdtSpring6Create");

    j = (MdtSpring6*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6Create: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclSPRING6;
    j->head.bclFunction = MdtBclAddSpring6;
    j->head.maxRows = MdtBclGETMAXROWSSPRING6;

    j->head.setBodyFunc = Spring6SetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtSpring6Reset(j);

    return j;
}

/**
 * Sets an Spring6 joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 * 
 * @arg linearStiffness = {INFINITY, INFINITY, INFINITY}
 * @arg linearDamping = {0, 0, 0}
 * @arg angularStiffness = {INFINITY, INFINITY, INFINITY}
 * @arg angularDamping = {0, 0, 0}
 * @arg worldVel = {0, 0, 0}
 *
 * @param j Spring6 joint ID
 */
void MEAPI MdtSpring6Reset(const MdtSpring6ID j)
{
    int i;

    MdtCHECKSPRING6(j,"MdtSpring6Reset");

    if(MdtSpring6IsEnabled(j))
        MdtSpring6Disable(j);

    j->head.bodyindex[0] = j->head.bodyindex[1] = 0;

    for(i=0; i<3; i++)
    {
        j->linearStiffness[i] = MEINFINITY;
        j->linearDamping[i] = 0;
        j->angularStiffness[i] = MEINFINITY;
        j->angularDamping[i] = 0;
        j->worldLinearVel[i] = 0;
        j->worldAngularVel[i] = 0;
    }

    /* Call base-class reset function. */
    BaseConstraintReset(MdtSpring6QuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtSpring6QuaConstraint(const MdtSpring6ID j)
{
    MdtCHECKSPRING6(j,"MdtSpring6QuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtSpring6ID from an MdtConstraintID.
 * If this constraint is not an Spring6, returns 0.
 */
MdtSpring6ID MEAPI MdtConstraintDCastSpring6(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastSpring6");
    if(c->head.tag == MdtBclSPRING6)
        return (MdtSpring6 *) c;
    else
        return 0;
}



/*
  Accessors
*/


/**
 *  Returns current linear spring damping along 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
MeReal MEAPI MdtSpring6GetLinearStiffness(const MdtSpring6ID j, int axisindex)
{
    MdtCHECKSPRING6(j,"MdtSpring6GetLinearStiffness");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6GetLinearStiffness: axisindex must be "
            "0, 1 or 2.");
#endif
        return 0;
    }

    return j->linearStiffness[axisindex];
}

/**
 *  Returns current linear spring damping along 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
MeReal MEAPI MdtSpring6GetLinearDamping(const MdtSpring6ID j, int axisindex)
{
    MdtCHECKSPRING6(j,"MdtSpring6GetLinearDamping");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6GetLinearDamping: axisindex must be "
            "0, 1 or 2.");
#endif
        return 0;
    }

    return j->linearDamping[axisindex];
}

/**
 *  Returns current angular spring stiffness around 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
MeReal MEAPI MdtSpring6GetAngularStiffness(const MdtSpring6ID j, int axisindex)
{
    MdtCHECKSPRING6(j,"MdtSpring6GetAngularStiffness");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6GetAngularStiffness: axisindex must be "
            "0, 1 or 2.");
#endif
        return 0;
    }

    return j->angularStiffness[axisindex];
}

/**
 *  Returns current angular spring damping around 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
MeReal MEAPI MdtSpring6GetAngularDamping(const MdtSpring6ID j, int axisindex)
{
    MdtCHECKSPRING6(j,"MdtSpring6GetAngularDamping");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6GetAngularDamping: axisindex must be "
            "0, 1 or 2.");
#endif
        return 0;
    }

    return j->angularDamping[axisindex];
}

/** 
 *  Return in 'v' current world linear velocity at attachment point.
 *  @see MdtSpring6SetWorldVelocity
 */
void MEAPI MdtSpring6GetWorldLinearVelocity(const MdtSpring6ID j, MeVector3 v)
{
    MdtCHECKSPRING6(j,"MdtSpring6GetWorldLinearVelocity");

    MeVector3Copy(v, j->worldLinearVel);
}

/** 
 *  Return in 'v' current world angular velocity at attachment point.
 *  @see MdtSpring6SetWorldVelocity
 */
void MEAPI MdtSpring6GetWorldAngularVelocity(const MdtSpring6ID j, MeVector3 v)
{
    MdtCHECKSPRING6(j,"MdtSpring6GetWorldAngularVelocity");

    MeVector3Copy(v, j->worldAngularVel);
}

/*
  Mutators
*/

/** 
 * Set stiffness of linear spring component along 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
void MEAPI MdtSpring6SetLinearStiffness(const MdtSpring6ID j, int axisindex,
                MeReal s)
{
    MdtCHECKSPRING6(j,"MdtSpring6SetLinearStiffness");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6SetLinearStiffness: axisindex must be "
            "0, 1 or 2.");
#endif
        return;
    }

    j->linearStiffness[axisindex] = s;
}

/** 
 * Set damping of linear spring component along 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
void MEAPI MdtSpring6SetLinearDamping(const MdtSpring6ID j, int axisindex,
                MeReal d)
{
    MdtCHECKSPRING6(j,"MdtSpring6SetLinearDamping");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6SetLinearDamping: axisindex must be "
            "0, 1 or 2.");
#endif
        return;
    }

    j->linearDamping[axisindex] = d;
}

/** 
 * Set stiffness of angular spring component about 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
void MEAPI MdtSpring6SetAngularStiffness(const MdtSpring6ID j, int axisindex,
                MeReal s)
{
    MdtCHECKSPRING6(j,"MdtSpring6SetAngularStiffness");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6SetAngularStiffness: axisindex must be "
            "0, 1 or 2.");
#endif
        return;
    }

    j->angularStiffness[axisindex] = s;
}

/** 
 * Set damping of angular spring component about 1 axis.
 * 'axisindex' is 0 for x, 1 for y and 2 for z
 */
void MEAPI MdtSpring6SetAngularDamping(const MdtSpring6ID j, int axisindex,
                MeReal d)
{
    MdtCHECKSPRING6(j,"MdtSpring6SetAngularDamping");

    if(axisindex < 0 || axisindex > 2)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtSpring6SetAngularDamping: axisindex must be "
            "0, 1 or 2.");
#endif
        return;
    }

    j->angularDamping[axisindex] = d;
}

/**
 *  Set linear velocity of world at spring attachement point.
 *  Only used if constraint is to the world (not another body).
 *  Velocity is given in the world ref. frame.
 */
void MEAPI MdtSpring6SetWorldLinearVelocity(const MdtSpring6ID j, 
                MeReal vx, MeReal vy, MeReal vz)
{
    MdtCHECKSPRING6(j,"MdtSpring6SetWorldLinearVelocity");

    j->worldLinearVel[0] = vx;
    j->worldLinearVel[1] = vy;
    j->worldLinearVel[2] = vz;
}

/**
 *  Set angular velocity of world at spring attachement point.
 *  Only used if constraint is to the world (not another body).
 *  Velocity is given in the world ref. frame.
 */
void MEAPI MdtSpring6SetWorldAngularVelocity(const MdtSpring6ID j, 
                MeReal vx, MeReal vy, MeReal vz)
{
    MdtCHECKSPRING6(j,"MdtSpring6SetWorldAngularVelocity");

    j->worldAngularVel[0] = vx;
    j->worldAngularVel[1] = vy;
    j->worldAngularVel[2] = vz;
}

/**
 * Set constraints bodies and note current orientation of bodies
 * as default orientation to maintain.
 */
void MEAPI Spring6SetBodies(const MdtConstraintID c,
              const MdtBodyID b1, const MdtBodyID b2)
{
    MeMatrix4 tm;
    MdtSpring6ID j = MdtConstraintDCastSpring6(c);

    MdtCHECKSPRING6(j,"Spring6SetBodies");

    BaseConstraintSetBodies(c, b1, b2);

    /* Set axis up so default spring orientation is current orientation. */
    MdtBodyGetTransform(b1, tm);
    MdtConstraintSetAxis(c, tm[0][0], tm[0][1], tm[0][2]);
}
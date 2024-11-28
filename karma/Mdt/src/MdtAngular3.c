/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:59 $ - Revision: $Revision: 1.31.2.1 $

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
#include <MdtAngular3.h>
#include <MdtConstraint.h>
#include <MdtBody.h>

void MEAPI Angular3SetBodies(const MdtConstraintID c,
              const MdtBodyID b1, const MdtBodyID b2);

/**
 * Creates a new Angular3 joint with default parameters.
 *
 * @see MdtAngular3Reset
 */
MdtAngular3ID MEAPI MdtAngular3Create(const MdtWorldID w)
{
    MdtAngular3 *j;
    MdtCHECKWORLD(w,"MdtAngular3Create");

    j = (MdtAngular3*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtAngular3Create: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclANGULAR3;
    j->head.bclFunction = MdtBclAddAngular3;
    j->head.maxRows = MdtBclGETMAXROWSANGULAR3;

    j->head.setBodyFunc = Angular3SetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtAngular3Reset(j);

    return j;
}

/**
 * Sets an Angular3 joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg bEnableRotation = 0
 * @arg axis1 = {1,0,0}
 * @arg axis2 = {1,0,0}
 * @arg ref_axis1 = {0,1,0}
 * @arg ref_axis2 = {0,1,0}
 * @arg stiffness = INFINITY
 * @arg damping = INFINITY
 *
 * @param j Angular3 joint ID
 */
void MEAPI MdtAngular3Reset(const MdtAngular3ID j)
{
    MdtCHECKANGULAR3(j,"MdtAngular3Reset");

    if(MdtAngular3IsEnabled(j))
        MdtAngular3Disable(j);

    j->head.bodyindex[0] = j->head.bodyindex[1] = 0;
    j->bEnableRotation = 0;

    j->stiffness = MEINFINITY;
    j->damping = MEINFINITY;

    /* Call base-class reset function. */
    BaseConstraintReset(MdtAngular3QuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtAngular3QuaConstraint(const MdtAngular3ID j)
{
    MdtCHECKANGULAR3(j,"MdtAngular3QuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtAngular3ID from an MdtConstraintID.
 * If this constraint is not an Angular3, returns 0.
 */
MdtAngular3ID MEAPI MdtConstraintDCastAngular3(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastAngular3");
    if(c->head.tag == MdtBclANGULAR3)
        return (MdtAngular3 *) c;
    else
        return 0;
}

/**
 * Return current 'stiffness' of this angular constraint.
 */
MeReal MEAPI MdtAngular3GetStiffness( const MdtAngular3ID j )
{
    MdtCHECKCONSTRAINT(j, "MdtAngular3GetStiffness");
    return j->stiffness;
}

/**
 * Return current spring 'damping' of this angular constraint.
 */
MeReal MEAPI MdtAngular3GetDamping( const MdtAngular3ID j )
{
    MdtCHECKCONSTRAINT(j, "MdtAngular3GetDamping");
    return j->damping;
}

/*
  Accessors
*/

/**
 * Returns the current state of the bEnableRotation flag.
 * If this flag is set (true), this constraint is effectively an
 * Angular 2 joint.
 */
MeBool MEAPI MdtAngular3RotationIsEnabled( const MdtAngular3ID j )
{
    MdtCHECKANGULAR3(j,"MdtAngular3RotationIsEnabled");
    return j->bEnableRotation;
}

/*
  Mutators
*/

/**
 * Set constraints bodies and note current orientation of bodies
 * as default orientation to maintain.
 */
void MEAPI Angular3SetBodies(const MdtConstraintID c,
              const MdtBodyID b1, const MdtBodyID b2)
{
    MeVector3 v;
    MeMatrix4 tm;
    MdtAngular3ID j = MdtConstraintDCastAngular3(c);

    MdtCHECKANGULAR3(j,"Angular3SetBodies");

    BaseConstraintSetBodies(c, b1, b2);

    /*
        As we set the bodies, set the axis to 'x' of the first body
        This will fix bodt in its current orientation.
     */
    MdtBodyGetTransform(b1, tm);
    v[0] = tm[0][0];
    v[1] = tm[0][1];
    v[2] = tm[0][2];

    MdtAngular3SetAxis(j, v[0], v[1], v[2]);
}

/**
 * Sets or clears the joint bEnableRotation flag to NewRotationState..
 */
void MEAPI MdtAngular3EnableRotation( const MdtAngular3ID j,
                                const MeBool NewRotationState )
{
    MdtCHECKANGULAR3(j,"MdtAngular3EnableRotation");
    j->bEnableRotation = NewRotationState;
}

/**
 * Set 'stiffness' of angular constraint about all enabled axis.
 * By default this is MEINFINITY.
 * @see MdtAngular3SetDamping
 */
void MEAPI MdtAngular3SetStiffness( const MdtAngular3ID j, const MeReal s )
{
    MdtCHECKANGULAR3(j,"MdtAngular3SetStiffness");
    j->stiffness = s;
}

/**
 * Set 'damping' of angular constraint about all enabled axis.
 * By default is MEINFINITY.
 * @see MdtAngular3SetStiffness
 */
void MEAPI MdtAngular3SetDamping( const MdtAngular3ID j, const MeReal d )
{
    MdtCHECKANGULAR3(j,"MdtAngular3SetDamping");
    j->damping = d;
}

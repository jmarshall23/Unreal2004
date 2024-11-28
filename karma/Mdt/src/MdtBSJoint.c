/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:59 $ - Revision: $Revision: 1.32.2.1 $

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
#include <MdtBSJoint.h>


/**
 * Creates a new ball and socket joint with default parameters.
 * @see MdtBSJointReset
 */
MdtBSJointID MEAPI MdtBSJointCreate(const MdtWorldID w)
{
    MdtBSJoint *j;
    MdtCHECKWORLD(w,"MdtBSJointCreate");

    j = (MdtBSJoint*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtBSJointCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclBSJOINT;
    j->head.bclFunction = MdtBclAddBSJoint;
    j->head.maxRows = MdtBclGETMAXROWSBSJOINT;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtBSJointReset(j);

    return j;
}

/**
 * Sets a ball and socket joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * A default ball and socket joint has the following characteristics:
 *
 * @arg Position  = {0,0,0} (in both bodies' reference frame)
 * @arg Primary axis = {1,0,0} (in both bodies' reference frame)
 * @arg Orthogonal axis = {0,1,0} (in both bodies' reference frame)
 *
 * @param j ball and socket joint ID
 */
void MEAPI MdtBSJointReset(const MdtBSJointID j)
{
    MdtCHECKBSJOINT(j,"MdtBSJointReset");

    if(MdtBSJointIsEnabled(j))
        MdtBSJointDisable(j);


    /* Call base-class reset function. */
    BaseConstraintReset(MdtBSJointQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtBSJointQuaConstraint(const MdtBSJointID j)
{
    MdtCHECKBSJOINT(j,"MdtBSJointQuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtBSJointID from an MdtConstraintID.
 * If this constraint is not a BSJoint, returns 0.
 */
MdtBSJointID MEAPI MdtConstraintDCastBSJoint(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastBSJoint");
    if(c->head.tag == MdtBclBSJOINT)
        return (MdtBSJoint *) c;
    else
        return 0;
}



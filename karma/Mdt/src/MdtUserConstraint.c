/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.21.6.1 $

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

#include "MdtUtils.h"
#include <MdtCheckMacros.h>
#include <MdtConstraint.h>
#include <MdtUserConstraint.h>

/**
 * Creates a new user constraint.
 */
MdtUserConstraintID MEAPI MdtUserConstraintCreate(const MdtWorldID w)
{
    MdtUserConstraint *j;
    MdtCHECKWORLD(w,"MdtUserConstraintCreate");

    j = (MdtUserConstraint*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtUserConstraintCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclUSER;
    j->userConstraint = 0;
    j->head.bclFunction = 0;
    j->head.maxRows = MdtBclGETMAXROWS;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    BaseConstraintReset(MdtUserConstraintQuaConstraint(j));

    return j;
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtUserConstraintQuaConstraint(const MdtUserConstraintID j)
{
    MdtCHECKUSERCONSTRAINT(j,"MdtUserConstraintQuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtUserConstraintID from an MdtConstraintID.
 * If this constraint is not a UserConstraint, returns 0.
 */
MdtUserConstraintID MEAPI MdtConstraintDCastUserConstraint(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastUserConstraint");
    if(c->head.tag == MdtBclUSER)
        return (MdtUserConstraint *) c;
    else
        return 0;
}

/**
 * Get the function assigned to this user constraint.
 * @see MdtUserConstraintSetFunction
 */
MdtBclAddConstraintFn MEAPI MdtUserConstraintGetFunction(const MdtUserConstraintID j)
{
    MdtCHECKUSERCONSTRAINT(j,"MdtUserConstraintGetFunction");
    return j->head.bclFunction;
}

/**
 * Get the pointer to the User Constraint Data for this constraint.
 */
void *MEAPI MdtUserConstraintGetConstraintData(const MdtUserConstraintID j)
{
    MdtCHECKUSERCONSTRAINT(j,"MdtUserConstraintGetConstraintData");
    return j->userConstraint;
}

/**
 * Set the function assigned to this user constraint.
 * The constraint function converts the User Constraint Data into
 * constraint equation rows to add to MdtKeaConstraints when it is called
 * from MdtPackWorld/Partition.
 */
void MEAPI MdtUserConstraintSetFunction(const MdtUserConstraintID j, MdtBclAddConstraintFn f)
{
    MdtCHECKUSERCONSTRAINT(j,"MdtUserConstraintSetFunction");
    j->head.bclFunction = f;
}

/**
 * Set the User Constraint Data pointer.
 * This data will be used by the UserConstraint function to add user
 * constraint equations into MdtKeaConstraints during MdtPackWorld/Partition.
 */
void MEAPI MdtUserConstraintSetConstraintData(const MdtUserConstraintID j, void* d)
{
    MdtCHECKUSERCONSTRAINT(j,"MdtUserConstraintSetConstraintData");
    j->userConstraint = d;
}

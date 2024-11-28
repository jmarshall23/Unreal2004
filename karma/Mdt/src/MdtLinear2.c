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
#include <MdtLinear2.h>

void MEAPI Linear2SetBodies(const MdtConstraintID c,
               const MdtBodyID b1, const MdtBodyID b2);

/**
 * Creates a new linear 2 joint with default parameters.
 *
 * @see MdtLinear2Reset
 */
MdtLinear2ID MEAPI MdtLinear2Create(const MdtWorldID w)
{
    MdtLinear2 *j;
    MdtCHECKWORLD(w,"MdtLinear2Create");

    j = (MdtLinear2*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtLinear2Create: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclLINEAR2;
    j->head.bclFunction = MdtBclAddLinear2;
    j->head.maxRows = MdtBclGETMAXROWSLINEAR2;

    j->head.setBodyFunc = Linear2SetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtLinear2Reset(j);

    return j;
}

/**
 * Sets a linear2 joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg position = {0,0,0}
 * @arg direction = {0,0,0}
 *
 * @param j linear 2 joint ID
 */
void MEAPI MdtLinear2Reset(const MdtLinear2ID j)
{
    int i;
    MdtCHECKLINEAR2(j,"MdtLinear2Reset");

    if(MdtLinear2IsEnabled(j))
        MdtLinear2Disable(j);

    for (i = 0; i < 3; i++)
    {
        j->pos1[i] = j->pos2[i] = 0;
        j->vec1[i] = j->vec2[i] = 0;
        j->direction[i] = 0;
    }

    BaseConstraintReset(MdtLinear2QuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtLinear2QuaConstraint(const MdtLinear2ID j)
{
    MdtCHECKLINEAR2(j,"MdtLinear2QuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtLinear2ID from an MdtConstraintID.
 * If this constraint is not a Linear2, returns 0.
 */
MdtLinear2ID MEAPI MdtConstraintDCastLinear2(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastLinear2");
    if(c->head.tag == MdtBclLINEAR2)
        return (MdtLinear2 *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * The linear 2 joint's position vector is returned in @a v.
 */
void MEAPI MdtLinear2GetPosition(const MdtLinear2ID j, MeVector3 position)
{
    MdtCHECKLINEAR2(j,"MdtLinear2GetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtLinear2GetPosition");

    ConvertCOMPositionVector(j->head.mdtbody[0], j->pos1, 0,
        position);
}

/**
 * The linear 2 joint's direction vector is returned in @a v.
 */
void MEAPI MdtLinear2GetDirection(const MdtLinear2ID j, MeVector3 direction)
{
    MdtCHECKLINEAR2(j,"MdtLinear2GetDirection");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtLinear2GetDirection");

    if (j->head.mdtbody[1] != 0)
        ConvertCOMVector(j->head.mdtbody[1],
            j->direction, 0, direction);
    else
    {
        direction[0] = j->direction[0];
        direction[1] = j->direction[1];
        direction[2] = j->direction[2];
    }
}

/*
  Mutators
*/

/**
 * Set bodies of Linear2 constraint, and sets default direction of
 * constraint using current orientation.
 */
void MEAPI Linear2SetBodies(const MdtConstraintID c,
               const MdtBodyID b1, const MdtBodyID b2)
{
    MdtLinear2ID j = MdtConstraintDCastLinear2(c);

    MdtCHECKLINEAR2(j,"Linear2SetBodies");

    BaseConstraintSetBodies(c, b1, b2);

    /*
     * Maintain an orthogonal set of vectors at all times once the
     * bodies are set:
     */
    MdtLinear2SetDirection(j, 1, 0, 0);
}

/**
 * Sets the joint position in world coordinates.
 */
void MEAPI MdtLinear2SetPosition(const MdtLinear2ID j, const MeReal x,
                           const MeReal y, const MeReal z)
{
    MdtCHECKLINEAR2(j,"MdtLinear2SetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtLinear2SetPosition");

    j->pos2[0] = x;
    j->pos2[1] = y;
    j->pos2[2] = z;

    /*
      Convert joint's position from world space to vectors in body1's
      and body2's reference frames.
    */
    ConvertCOMPositionVector(0,
        j->pos2, j->head.mdtbody[0], j->pos1);

    if (j->head.mdtbody[1] != 0)
        ConvertCOMPositionVector(0,
            j->pos2, j->head.mdtbody[1], j->pos2);

    MdtConstraintSetPosition(MdtLinear2QuaConstraint(j), x, y, z);
}

/**
 * Sets the joint direction in world coordinates.
 *
 * N.B. Set attributes x, y and z *should* be const.
 */
void MEAPI MdtLinear2SetDirection(const MdtLinear2ID j, const MeReal x,
                            const MeReal y, const MeReal z)
{
    MeVector3 direction;
    MeVector3 vector1;
    MeVector3 vector2;
    MdtCHECKLINEAR2(j,"MdtLinear2SetDirection");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtLinear2SetDirection");

#ifdef _MECHECK
    if ((x == 0) && (y == 0) && (z == 0))
        MeFatalError(0,
            "The Linear2 direction vector must not be zero.");
#endif

    /*
      Convert direction from world space to body2's reference frame:
    */

    direction[0] = x;
    direction[1] = y;
    direction[2] = z;

    if (j->head.mdtbody[1] != 0)
        ConvertCOMVector(0, direction,
            j->head.mdtbody[1], j->direction);
    else
    {
        /*
          ..."body 2" is the inertial reference  frame:
        */
        j->direction[0] = x;
        j->direction[1] = y;
        j->direction[2] = z;
    }

    /*
      Now set the perpendicular vectors vec1 and vec2 in body2's
      reference frame:
    */

    vector1[2] = 0;

    if (x == 0)
    {
        vector1[0] = 1;
        vector1[1] = 0;
    }
    else if (y == 0)
    {
        vector1[0] = 0;
        vector1[1] = 1;
    }
    else
    {
        /* x != 0,  y != 0 */
        vector1[0] = 1 / x;
        vector1[1] = -1 / y;
    }


    /*
      We assume here that direction is a non-zero vector!
    */
    MeVector3Cross( vector2, direction,  vector1);

    if (j->head.mdtbody[1] != 0)
    {
        ConvertCOMVector(0, vector1, j->head.mdtbody[1],
            j->vec1);
        ConvertCOMVector(0, vector2, j->head.mdtbody[1],
            j->vec2);
    }
    else
    {
        /*
          ..."body 2" is the inertial reference frame:
        */

        unsigned int i;

        for (i = 0; i < 3; ++i)
        {
            j->vec1[i] = vector1[i];
            j->vec2[i] = vector2[i];
        }
    }

#ifdef _MECHECK
    /*
      Verify the preceding calculations:
    */
    if (!ME_IS_ZERO( MeVector3Dot(vector1, vector2) )   ||
        !ME_IS_ZERO( MeVector3Dot(direction, vector2) ) ||
        !ME_IS_ZERO( MeVector3Dot(j->vec1,j->vec2))
    )
        MeFatalError(0,
            "Invalid calculation of Linear2 perpendicular vectors.");
#endif

    MdtConstraintSetAxis(MdtLinear2QuaConstraint(j), x, y, z);
}



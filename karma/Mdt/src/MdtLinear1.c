/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 17:44:00 $ - Revision: $Revision: 1.27.6.3 $

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
#include <MdtBody.h>
#include <MdtLinear1.h>

/**
 * Creates a new linear 1 joint with default parameters.
 *
 * @see MdtLinear1Reset
 */
MdtLinear1ID MEAPI MdtLinear1Create(const MdtWorldID w)
{
    MdtLinear1 *j;
    MdtCHECKWORLD(w,"MdtLinear1Create");

    j = (MdtLinear1*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtLinear1Create: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclLINEAR1;
    j->head.bclFunction = MdtBclAddLinear1;
    j->head.maxRows = MdtBclGETMAXROWSLINEAR1;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtLinear1Reset(j);

    return j;
}

/**
 * Sets a linear1 joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg position = {0,0,0}
 *
 * @param j linear 1 joint ID
 */
void MEAPI MdtLinear1Reset(const MdtLinear1ID j)
{
    int i;
    MdtCHECKLINEAR1(j,"MdtLinear1Reset");

    if(MdtLinear1IsEnabled(j))
        MdtLinear1Disable(j);

    for (i = 0; i < 3; i++)
        j->pos1[i] = j->pos2[i] = 0;

    BaseConstraintReset(MdtLinear1QuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtLinear1QuaConstraint(const MdtLinear1ID j)
{
    MdtCHECKLINEAR1(j,"MdtLinear1QuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtLinear1ID from an MdtConstraintID.
 * If this constraint is not a Linear1, returns 0.
 */
MdtLinear1ID MEAPI MdtConstraintDCastLinear1(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastLinear1");
    if(c->head.tag == MdtBclLINEAR1)
        return (MdtLinear1 *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * The linear 1 joint's position vector is returned in @a v.
 */
void MEAPI MdtLinear1GetPosition(const MdtLinear1ID j, MeVector3 position)
{
    MdtCHECKLINEAR1(j,"MdtLinear1GetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtLinear1GetPosition");

    ConvertCOMPositionVector(j->head.mdtbody[0],
        j->pos1, 0, position);
}

/*
  Mutators
*/

/**
 * Sets the joint position in world coordinates.
 *
 * The joint position with respect to the secondary body defines the
 * plane normal for this joint.  The joint position is therefore not
 * allowed to be coincident with the secondary body, or with the origin
 * of the inertial frame if there is no secondary body.
 *
 * N.B. Set attributes x, y and z *should* be const.
 */
void MEAPI MdtLinear1SetPosition(const MdtLinear1ID j, const MeReal x,
                           const MeReal y, const MeReal z)
{
    MeVector3 position;
    MeVector3 normal;
    /* The displacement vector, in the inertial frame.  */
    MeVector3 p1p2;

    MdtCHECKLINEAR1(j,"MdtLinear1SetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtLinear1SetPosition");

    /*
      This test should be for points which are "near" to each other,
      rather than coincident, but for simplicity we test for the latter:
    */
    if (j->head.mdtbody[1] != 0)
    {
        MeVector3 Body2Pos;

        MdtBodyGetPosition(j->head.mdtbody[1], Body2Pos);
#ifdef _MECHECK
        if ((Body2Pos[0] == x) && (Body2Pos[1] == y)
            && (Body2Pos[2] == z))
/*            MeFatalError(0,
                "The joint position with respect to the second"
                " body determines the plane normal: they may not"
                " therefore be coincident.");
*/
              MeFatalError(0,
                "The joint position cannot be the same as the position"
                "of the second body because they are used to define the"
                "normal of the constrained plane.");
#endif
        normal[0] = x - Body2Pos[0];
        normal[1] = y - Body2Pos[1];
        normal[2] = z - Body2Pos[2];
    }
#ifdef _MECHECK
    else if ((x == 0) && (y == 0) && (z == 0))
        MeFatalError(0,
            "The joint position with respect to the coordinate"
            " origin determines the plane normal: they may not"
            " therefore be coincident.\n");
#endif
    else
    {
        normal[0] = x;
        normal[1] = y;
        normal[2] = z;
    }
    position[0] = x;
    position[1] = y;
    position[2] = z;

    /*
      Convert joint's position from world space to vectors in body1's
      and body2's reference frames.
    */

    ConvertCOMPositionVector(0, position, j->head.mdtbody[0],
        j->pos1);

    if (j->head.mdtbody[1] != 0)
        ConvertCOMPositionVector(0, position,
            j->head.mdtbody[1], j->pos2);
    else
    {
        /* ..."body 2" is the inertial reference frame: */
        j->pos2[0] = x;
        j->pos2[1] = y;
        j->pos2[2] = z;
    }

    /*
      Calculate the dot product of the displacement vector and the plane
      normal - this displacement value is invariant for this joint.
    */

    MdtBodyGetPosition(MdtLinear1GetBody(j,0), p1p2);
    if (j->head.mdtbody[1] != 0)
    {
        MeVector3 p2;

        MdtBodyGetPosition(MdtLinear1GetBody(j,1), p2);
        {
            unsigned int i = 0;

            for (i = 0; i < 3; ++i)
                p1p2[i] -= p2[i];
        }
    }

    j->displacement = MeVector3Dot(normal, p1p2);

    MdtConstraintSetPosition(MdtLinear1QuaConstraint(j), x, y, z);
}



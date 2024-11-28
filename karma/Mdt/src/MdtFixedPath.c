/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.26.6.1 $

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
#include <MdtFixedPath.h>

/**
 * Creates a new fixed path constraint with default parameters.
 *
 * @see MdtFixedPathReset
 */
MdtFixedPathID MEAPI MdtFixedPathCreate(const MdtWorldID w)
{
    MdtFixedPath *j;
    MdtCHECKWORLD(w,"MdtFixedPathCreate");

    j = (MdtFixedPathID)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtFixedPathCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclFIXEDPATH;
    j->head.bclFunction = MdtBclAddFixedPath;
    j->head.maxRows = MdtBclGETMAXROWSFIXEDPATH;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtFixedPathReset(j);

    return j;
}

/**
 * Sets a fixed path joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg position = {0,0,0}
 * @arg velocity = {0,0,0}
 *
 * @param j fixed path joint ID
 */
void MEAPI MdtFixedPathReset(const MdtFixedPathID j)
{
    int i;
    MdtCHECKFIXEDPATH(j,"MdtFixedPathReset");

    if(MdtFixedPathIsEnabled(j))
        MdtFixedPathDisable(j);

    for (i = 0; i < 3; i++)
    {
        j->pos1[i] = j->pos2[i] = 0;
        j->vel1[i] = j->vel2[i] = 0;
    }

    BaseConstraintReset(MdtFixedPathQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtFixedPathQuaConstraint(const MdtFixedPathID j)
{
    MdtCHECKFIXEDPATH(j,"MdtFixedPathQuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtFixedPathID from an MdtConstraintID.
 * If this constraint is not a FixedPath, returns 0.
 */
MdtFixedPathID MEAPI MdtConstraintDCastFixedPath(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastFixedPath");
    if(c->head.tag == MdtBclFIXEDPATH)
        return (MdtFixedPath *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * The fixed path joint's position with respect to one of the constrained
 * bodies is returned in @a v.
 *
 * The reference frame is selected by the third parameter (BodyID).
 */
void MEAPI MdtFixedPathGetPosition(const MdtFixedPathID j, const unsigned int bodyindex,
                                   MeVector3 position)
{
    MdtCHECKFIXEDPATH(j,"MdtFixedPathGetPosition");
    MdtCHECKBODYINDEX(bodyindex,"MdtFixedPathGetPosition");

    {
        const MeReal *const pos =
        (bodyindex == 0) ? j->pos1 : j->pos2;

        unsigned int i = 0;

        for (i = 0; i < 3; ++i)
            position[i] = pos[i];
    }
}

/**
 * The fixed path joint's velocity with respect to one of the constrained
 * bodies is returned in @a v.
 *
 * The reference frame is selected by the third parameter (BodyID).
 */
void MEAPI MdtFixedPathGetVelocity(const MdtFixedPathID j, const unsigned int bodyindex,
                                   MeVector3 velocity)
{
    MdtCHECKFIXEDPATH(j,"MdtFixedPathGetVelocity");
    MdtCHECKBODYINDEX(bodyindex,"MdtFixedPathGetVelocity");

    {
        const MeReal *const vel =
            (bodyindex == 0) ? j->vel1 : j->vel2;

        unsigned int i = 0;

        for (i = 0; i < 3; ++i)
            velocity[i] = vel[i];
    }
}


/*
  Mutators
*/

/**
 * Sets the fixed path joint's position with respect to one
 * of the constrained bodies.
 *
 * Unlike the ball-and-socket joint, this joint's position is set in the
 * bodies' reference frames
 *
 * The reference frame is selected by the third parameter (BodyID).
 */
void MEAPI MdtFixedPathSetPosition(const MdtFixedPathID j, const unsigned int bodyindex,
                                   const MeReal x, const MeReal y, const MeReal z)
{

    MdtCHECKFIXEDPATH(j,"MdtFixedPathSetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtFixedPathSetPosition");
    MdtCHECKBODYINDEX(bodyindex,"MdtFixedPathSetPosition");

    /*
     * Note that the user is expected to set the joint position in the
     * bodies' reference frames, * so we do *not* convert them here.
     */
    {
        MeReal *const pos =
            (bodyindex == 0) ? j->pos1 : j->pos2;

        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
    {
        MeReal *const pos =
            (bodyindex == 0) ? j->head.ref1[3] :
                               j->head.ref2[3];
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
}

/**
 * Sets the fixed path joint's velocity with respect to one of the
 * constrained bodies.
 *
 * This joint's velocity is set in the bodies' reference frames.
 *
 * The reference frame is selected by the third parameter (BodyID).
 */
void MEAPI MdtFixedPathSetVelocity(const MdtFixedPathID j, const unsigned int bodyindex,
                                   const MeReal dx, const MeReal dy, const MeReal dz)
{
    MdtCHECKFIXEDPATH(j,"MdtFixedPathSetVelocity");
    MdtCHECKBODYINDEX(bodyindex,"MdtFixedPathSetVelocity");

    /*
      Note that the user is expected to set the joint velocity in the
      bodies' reference frames, * so we do *not* convert them here.
    */
    {
        MeReal *const vel =
            (bodyindex == 0) ? j->vel1 : j->vel2;

        vel[0] = dx;
        vel[1] = dy;
        vel[2] = dz;
    }
}


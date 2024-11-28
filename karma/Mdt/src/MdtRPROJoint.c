/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/19 09:50:50 $ - Revision: $Revision: 1.24.6.3 $

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
#include <MdtRPROJoint.h>


/**
 * Creates a new full constraint joint with default parameters.
 *
 * @see MdtRPROJointReset
 */
MdtRPROJointID MEAPI MdtRPROJointCreate(const MdtWorldID w)
{
    MdtRPROJoint *j;
    MdtCHECKWORLD(w,"MdtRPROJointCreate");

    j = (MdtRPROJoint*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtRPROJointCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclRPROJOINT;
    j->head.bclFunction = MdtBclAddRPROJoint;
    j->head.maxRows = MdtBclGETMAXROWSRPROJOINT;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtRPROJointReset(j);

    return j;
}



/**
 * Sets a full constraint joint to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg position = {0,0,0}
 * @arg relative orientation = {0,0,0}
 *
 * @param j full constraint joint ID
 */
void MEAPI MdtRPROJointReset(const MdtRPROJointID j)
{
    int i;
    MdtCHECKRPROJOINT(j,"MdtRPROJointReset");

    if(MdtRPROJointIsEnabled(j))
        MdtRPROJointDisable(j);

    j->q_rel[0] = (MeReal)1;
    j->q1[0]  = j->q2[0]  = (MeReal)1;
    j->use_q1 = j->use_q2 = 0;
    for(i=0;i<3;++i)
      {
        j->q_rel[i+1] = (MeReal)0;
        j->omega[i] = (MeReal)0;
        j->q1[i+1]  = j->q2[i+1]  = (MeReal)0;
        j->p_rel[i] = (MeReal)0;
        j->v_rel[i] = (MeReal)0;
      }

    j->angular_fmax[0] = MEINFINITY;
    j->angular_fmax[1] = MEINFINITY;
    j->angular_fmax[2] = MEINFINITY;
    j->linear_fmax[0] = MEINFINITY;
    j->linear_fmax[1] = MEINFINITY;
    j->linear_fmax[2] = MEINFINITY;

    BaseConstraintReset(MdtRPROJointQuaConstraint(j));
}




/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtRPROJointQuaConstraint(const MdtRPROJointID j)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointQuaConstraint");
    return (MdtBaseConstraint*)j;
}


/**
 * Gets an MdtRPROJointID from an MdtConstraintID.
 * If this constraint is not a RPROJoint, returns 0.
 */
MdtRPROJointID MEAPI MdtConstraintDCastRPROJoint(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastRPROJoint");
    if(c->head.tag == MdtBclRPROJOINT)
        return (MdtRPROJoint *) c;
    else
        return 0;
}


/*
 * Accessors
 */


/**
 * The full constraint joint's position with respect to one of the
 * constrained bodies is returned in @a v.
 *
 * The reference frame is selected by the third parameter (BodyID).
 */
void MEAPI MdtRPROJointGetAttachmentPosition(const MdtRPROJointID j,
                            const unsigned int bodyindex, MeVector3 position)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetAttachmentPosition");
    MdtCHECKBODYINDEX(bodyindex,"MdtRPROJointGetAttachmentPosition");
#ifdef _MECHECK
    if (bodyindex > 1)
    {
        MeFatalError(0,
            "Invalid value of bodyindex in MdtRPROJointGetAttachmentPosition()");
    }
#endif
    {
        const MeReal *pos =
            (bodyindex == 0 ? j->head.ref1[3] : j->head.ref2[3]);

        unsigned int i = 0;

        for (i = 0; i < 3; ++i)
        {
            position[i] = pos[i];
        }
    }
}


/**
 * The full constraint joint's attachment quaternion respect to one of the
 * constrained bodies is returned in @a v.
 *
 * The reference frame is selected by the third parameter (bodyindex).
 */
void MEAPI MdtRPROJointGetAttachmentQuaternion(const MdtRPROJointID j,
                            const unsigned int bodyindex, MeVector4 quaternion)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetAttachmentQuaternion");
    MdtCHECKBODYINDEX(bodyindex,"MdtRPROJointGetAttachmentQuaternion");
#ifdef _MECHECK
    if (bodyindex > 1)
    {
        MeFatalError(0,
            "Invalid value of bodyindex in MdtRPROJointGetAttachmentQuaternion()");
    }
#endif
    {
        const MeReal *q = (bodyindex == 0 ? j->q1 : j->q2);
        unsigned int i = 0;

        for (i = 0; i < 4; ++i)
        {
            quaternion[i] = q[i];
        }
    }
}


/**
 * Gets the relative position.
 */
void MEAPI
MdtRPROJointGetRelativePosition(const MdtRPROJointID j, MeVector3 rel_pos)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetRelativePosition");
    MeVector3Copy( rel_pos, j->p_rel);
    return;
}

/**
 * Gets the relative velocity.
 */
void MEAPI
MdtRPROJointGetRelativeVelocity(const MdtRPROJointID j, MeVector3 rel_vel)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetRelativeVelocity");
    MeVector3Copy( rel_vel, j->v_rel);
    return;
}

/**
 *  Returns the relative quaternion
 */
void MEAPI MdtRPROJointGetRelativeQuaternion(const MdtRPROJointID j, MeVector4 q)
{
    int i;
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetRelativeQuaternion");
    for(i=0;i<4;++i)
    {
        q[i] = j->q_rel[i];
    }
}


/**
 * Returns the relative angular velocity
 */
void MEAPI MdtRPROJointGetRelativeAngularVelocity(const MdtRPROJointID j, MeVector3 w)
{
    int i;
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetRelativeAngularVelocity");
    for(i=0;i<3;++i)
    {
        w[i] = j->omega[i];
    }
}

/**
 * Returns the angular strength of the constraint
 */
void MEAPI MdtRPROJointGetAngularStrength(const MdtRPROJointID j, MeVector3 v)
{
    int i;
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetAngularStrength");
    for(i=0;i<3;++i)
    {
        v[i] = j->angular_fmax[i];
    }
}
/**
 * Returns the linear strength of the constraint
 */
void MEAPI MdtRPROJointGetLinearStrength(const MdtRPROJointID j, MeVector3 v)
{
    int i;
    MdtCHECKRPROJOINT(j,"MdtRPROJointGetLinearStrength");
    for(i=0;i<3;++i)
    {
        v[i] = j->linear_fmax[i];
    }
}


/*
 * Mutators
 */


/**
 * Sets the constraint joint's position with respect to one of
 * the constrained bodies.
 *
 * The reference frame is selected by the third parameter (bodyindex).
 */
void MEAPI MdtRPROJointSetAttachmentPosition(const MdtRPROJointID j, const MeReal x,
                             const MeReal y, const MeReal z,
                             const unsigned int bodyindex)
{
    MeReal *pos = (bodyindex == 0 ? j->head.ref1[3] : j->head.ref2[3]);
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetAttachmentPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtRPROJointSetAttachmentPosition");
#ifdef _MECHECK
    if (bodyindex > 1)
    {
        MeFatalError(0,
            "Invalid value of bodyindex in MdtRPROJointSetAttachmentPosition()");
    }
#endif
    /*
     * Note that the user is expected to set the joint position in the
     * bodies' reference frames, * so we do *not* convert them here.
     */
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

/**
 * Sets the full constraint joint's attachment orientation with respect to one of
 * the constrained bodies.
 *
 * The reference frame is selected by the third parameter (bodyindex).
 */


void MEAPI
MdtRPROJointSetAttachmentQuaternion( const MdtRPROJointID j, const MeReal q0,
             const MeReal q1, const MeReal q2, const MeReal q3,
             const unsigned int bodyindex)
{
    MeReal *q = (bodyindex == 0 ? j->q1 : j->q2);
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetAttachmentQuaternion");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtRPROJointSetAttachmentQuaternion");
#ifdef _MECHECK
    if (bodyindex > 1)
    {
        MeFatalError(0,
            "Invalid value of bodyindex in MdtRPROJointSetAttachmentQuaternion()");
    }
#endif
    /*
     * Note that the user is expected to set the joint position in the
     * bodies' reference frames, * so we do *not* convert them here.
     */
    if (bodyindex == 0) {
      j->use_q1 = 1;
    } else {
      j->use_q2 = 1;
    }
    q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3;
}

/**
 * Sets the relative position.
 */
void MEAPI
MdtRPROJointSetRelativePosition(const MdtRPROJointID j, const MeReal rpx,
                                const MeReal rpy, const MeReal rpz)
{
    MeVector3 rel_pos;
    rel_pos[0] = rpx;
    rel_pos[1] = rpy;
    rel_pos[2] = rpz;
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetRelativePosition");
    MeVector3Copy( j->p_rel, rel_pos);
    return;
}

/**
 * Sets the relative velocity.
 */
void MEAPI
MdtRPROJointSetRelativeVelocity(const MdtRPROJointID j, const MeReal rvx,
                                const MeReal rvy, const MeReal rvz)
{
    MeVector3 rel_vel;
    rel_vel[0] = rvx;
    rel_vel[1] = rvy;
    rel_vel[2] = rvz;
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetRelativeVelocity");
    MeVector3Copy( j->v_rel, rel_vel);
    return;
}

/**
 * Sets the relative orientation quaternion.
 */
void MEAPI MdtRPROJointSetRelativeQuaternion(const MdtRPROJointID j, const MeVector4 q)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetRelativeQuaternion");
    MeVector4Copy( j->q_rel, q);
    return;
}


/**
 * Sets the relative orientation quaternion.
 */
void MEAPI MdtRPROJointSetRelativeAngularVelocity(MdtRPROJointID j, MeVector3 w)
{
    int i;
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetRelativeAngularVelocity");
    for(i=0;i<3;++i)
    {
        j->omega[i] =w[i];
    }
    return;
}

/**
 *
 *  Set limit on the maximum force that can be applied to maintain the
 *  constraints.   If all values are set at MEINFINITY, the constraint
 *  will always be maintained.  If some finite (positive) limit is set,
 *  then, the constraint will become violated if the force required to
 *  maintain it becomes larger than the threshold.
 *
 */
void MEAPI MdtRPROJointSetAngularStrength(const MdtRPROJointID j,
                                            const MeReal sX, const MeReal sY, const MeReal sZ)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetAngularStrength");
    j->angular_fmax[0] = sX;
    j->angular_fmax[1] = sY;
    j->angular_fmax[2] = sZ;

}

/**
 *  Set the linear strength of the constraint.
 */
void MEAPI MdtRPROJointSetLinearStrength(const MdtRPROJointID j,
                                            const MeReal sX, const MeReal sY, const MeReal sZ)
{
    MdtCHECKRPROJOINT(j,"MdtRPROJointSetLinearStrength");
    j->linear_fmax[0] = sX;
    j->linear_fmax[1] = sY;
    j->linear_fmax[2] = sZ;

}




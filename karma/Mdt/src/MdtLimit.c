/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.18.2.2 $

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

#include <MdtTypes.h>
#include <MdtCheckMacros.h>
#include <MdtLimit.h>

/*
  Individual limit initialisation.
*/

/**
 * Initialises the individual limit data and sets default values.
 *
 * Default values:
 *
 * @arg position = 0
 * @arg restitution = 1
 * @arg stiffness = MEINFINITY
 * @arg damping = 0
 */
void MEAPI MdtSingleLimitReset(const MdtSingleLimitID limit)
{
    MdtCHECKSINGLELIMIT(limit,"MdtSingleLimitReset");

    limit->stop = (MeReal) 0.0f;

    /* A perfectly elastic limit - only used with hard limits. */
    limit->restitution = (MeReal) 1.0f;

    /* A hard limit. */
    limit->stiffness = MEINFINITY;

    /* No damping - only used with soft limits. */
    limit->damping = (MeReal) 0.0f;
}

/*
  Individual limit accessors.
*/

/**
 * Returns the minimum (for lower limit) or maximum (for upper limit)
 * linear or angular separation of the attached bodies.
 */
MeReal MEAPI MdtSingleLimitGetStop(const MdtSingleLimitID sl)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitGetStop");
    return sl->stop;
}

/**
 * Returns the spring constant (kp) used for restitution force when a
 * limited joint reaches one of its stops.
 */
MeReal MEAPI MdtSingleLimitGetStiffness(const MdtSingleLimitID sl)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitGetStiffness");
    return sl->stiffness;
}

/**
 * Returns the damping term (kd) for this limit.
 */
MeReal MEAPI MdtSingleLimitGetDamping(const MdtSingleLimitID sl)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitGetDamping");
    return sl->damping;
}

/**
 * Returns the restitution of this limit.
 */
MeReal MEAPI MdtSingleLimitGetRestitution(const MdtSingleLimitID sl)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitGetRestitution");
    return sl->restitution;
}

/*
  Individual limit mutators.
*/

/**
 * Sets a limit on the linear or angular separation of the attached
 * bodies.
 */
void MEAPI MdtSingleLimitSetStop(const MdtSingleLimitID sl,
    const MeReal NewStop)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitSetStop");
    sl->stop = NewStop;
}

/**
 * Sets the stiffness property of the limit.
 *
 * Stiffness is enforced to be non-negative: the initial value is
 * MEINFINITY.
 */
void MEAPI MdtSingleLimitSetStiffness(const MdtSingleLimitID sl,
    const MeReal NewStiffness)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitSetStiffness");

    if (NewStiffness >= 0)
        sl->stiffness = NewStiffness;
    else
        sl->stiffness = -NewStiffness;
}

/**
 * Sets the damping property of the limit.
 *
 * Damping is enforced to be non-negative: the initial value is zero.
 */
void MEAPI MdtSingleLimitSetDamping(const MdtSingleLimitID sl,
    const MeReal NewDamping)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitSetDamping");

    if (NewDamping >= 0)
        sl->damping = NewDamping;
    else
        sl->damping = -NewDamping;
}

/**
 * Sets the restitution property of the limit.
 *
 * Restitution is enforced to be in the range zero to one inclusive: the
 * initial value is one.
 */
void MEAPI MdtSingleLimitSetRestitution(const MdtSingleLimitID sl,
    const MeReal NewRestitution)
{
    MdtCHECKSINGLELIMIT(sl,"MdtSingleLimitSetRestitution");

    if ((NewRestitution >= 0) && (NewRestitution <= 1))
        sl->restitution = NewRestitution;
    else if (NewRestitution < 0)
        sl->restitution = 0;
    else
        sl->restitution = 1;
}


/*
  Joint limit initialisation.
*/

/**
 * Initialises the joint limit data and sets default values.
 *
 * A 'limit' containts two 'single limits', a bottom and a top
 * stop. This function initialises both of these to their default
 * values as well.
 *
 * Default values:
 *
 * @arg powered = 0
 * @arg desired velocity = 0
 * @arg max force = 0
 *
 * @see MdtSingleLimitReset
 */
void MEAPI MdtLimitReset(const MdtLimitID  limit)
{
    MdtCHECKLIMIT(limit,"MdtLimitReset");

    /* between limits. */
    limit->overshoot = (MeReal) 0.0f;

    limit->desired_vel = (MeReal) 0.0f;
    limit->fmax = (MeReal) 0.0f;
    limit->offset = (MeReal) 0.0f;
    limit->position = (MeReal) 0.0f;
    limit->previous_position = (MeReal) 0.0f;
    limit->velocity = (MeReal) 0.0f;

    /*
      Set default threshold so that all limits are "soft" unless limit
      stiffness is set to MEINFINITY.
    */
    limit->damping_thresh = MEINFINITY;

    /* All boolean flags set to false on initialisation: */
    limit->bPowered = 0;
    limit->is_locked = 0;
    limit->bLimited = 0;
    limit->bCalculatePosition = 0;
    limit->bPositionInitialised = 0;
    limit->bRelaxingToLimit = 0;

    /*
       Initialize the properties of the individual joint limits:
    */

    /* Initialise lower limit */
    MdtSingleLimitReset(limit->limit);
    /* Initialise upper limit */
    MdtSingleLimitReset(limit->limit + 1);
}


/*
  Joint limit accessors.
*/

/**
 * Returns non-zero if the corresponding degree of freedom of the joint
 * (i.e. the joint position or angle) has a limit imposed on it, and
 * zero if it does not.  Most joints have more than one degree of freedom.
 *
 * Joint limits are inactive by default, and will not affect the attached
 * bodies until activated and non-zero stiffness and/or damping properties
 * are set.
 */
MeBool MEAPI MdtLimitIsActive(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitIsActive");
    return l->bLimited;
}

/**
 * Returns non-zero if the position or angle of the corresponding degree
 * of freedom of the joint is to be calculated, and zero if it is not
 * calculated.  If the degree of freedom is either limited or actuated
 * (i.e. powered), the joint position must be calculated.
 *
 * By default, joint positions are not calculated.
 */
MeBool MEAPI MdtLimitPositionIsCalculated(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitPositionIsCalculated");
    return l->bCalculatePosition;
}

/**
 * Provides read/write access to the constraint's lower limit.
 */
MdtSingleLimitID MEAPI MdtLimitGetLowerLimit(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetLowerLimit");
    return l->limit;
}

/**
 * Provides read/write access to the constraint's upper limit.
 */
MdtSingleLimitID MEAPI MdtLimitGetUpperLimit(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetUpperLimit");
    return l->limit + 1;
}

/**
 * This service returns the relative position of the bodies attached to the
 * joint.
 */
MeReal MEAPI MdtLimitGetPosition(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetPosition");
    return l->position + l->offset;
}

/**
 * This service returns the overshoot of the relative position of the
 * attached bodies past one of the limits.
 *
 * This value is positive if the upper limit has been passed, negative
 * if the lower limit has been passed, and zero if neither limit has
 * been passed.
 */
MeReal MEAPI MdtLimitGetOvershoot(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetOvershoot");
    return l->overshoot;
}

/**
 * This service returns the relative velocity of the bodies attached to the
 * joint.
 */
MeReal MEAPI MdtLimitGetVelocity(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetVelocity");
    return l->velocity;
}

/**
 * Returns the limit stiffness threshold.
 *
 * See comment on MdtLimitSetStiffnessThreshold service.
 */
MeReal MEAPI MdtLimitGetStiffnessThreshold(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetStiffnessThreshold");
    return l->damping_thresh;
}

/**
 * Returns non-zero if the limit is motorised, and zero if it is not.
 *
 * Joint limits are unmotorised by default.
 */
MeBool MEAPI MdtLimitIsMotorized(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitIsMotorized");
    return l->bPowered;
}

/**
 * Returns the desired velocity of the motor.
 *
 * A lower limiting velocity may be achieved if the attached bodies are
 * subject to velocity or angular velocity damping.
 */
MeReal MEAPI MdtLimitGetMotorDesiredVelocity(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetMotorDesiredVelocity");
    return l->desired_vel;
}

/**
 * Returns the maximum force that the motor is allowed to use to attain
 * its desired velocity.
 */
MeReal MEAPI MdtLimitGetMotorMaxForce(const MdtLimitID l)
{
    MdtCHECKLIMIT(l,"MdtLimitGetMotorMaxForce");
    return l->fmax;
}

/*
  Joint limit mutators.
*/

/**
 * Sets the lower limit properties by copying the single limit data into
 * the MdtBclLimit structure.
 *
 * If the lower limit stop is higher than the current upper limit stop,
 * the later is also reset to the new stop value.
 */
void MEAPI MdtLimitSetLowerLimit(const MdtLimitID l,
    const MdtSingleLimitID sl)
{
    MdtSingleLimitID low_limit;
    MdtSingleLimitID hi_limit;
    MdtCHECKLIMIT(l,"MdtLimitSetLowerLimit");

    low_limit = MdtLimitGetLowerLimit(l);
    hi_limit = MdtLimitGetUpperLimit(l);
    {
        const MeReal NewStop = MdtSingleLimitGetStop(sl);

        MdtSingleLimitSetStop(low_limit, NewStop);

        MdtSingleLimitSetStiffness(low_limit,
            MdtSingleLimitGetStiffness(sl));
        MdtSingleLimitSetDamping(low_limit,
            MdtSingleLimitGetDamping(sl));
        MdtSingleLimitSetRestitution(low_limit,
            MdtSingleLimitGetRestitution(sl));

        if (NewStop > MdtSingleLimitGetStop(hi_limit))
            MdtSingleLimitSetStop(hi_limit, NewStop);
    }
}

/**
 * Sets the upper limit properties by copying the single limit data into
 * the MdtBclLimit structure.
 *
 * If the upper limit stop is higher than the current lower limit stop,
 * the later is also reset to the new stop value.
 */
void MEAPI MdtLimitSetUpperLimit(const MdtLimitID l,
    const MdtSingleLimitID sl)
{
    MdtSingleLimitID low_limit;
    MdtSingleLimitID hi_limit;
    MdtCHECKLIMIT(l,"MdtLimitSetUpperLimit");

    low_limit = MdtLimitGetLowerLimit(l);
    hi_limit = MdtLimitGetUpperLimit(l);
    {
        const MeReal NewStop = MdtSingleLimitGetStop(sl);

        MdtSingleLimitSetStop(hi_limit, NewStop);

        MdtSingleLimitSetStiffness(hi_limit,
            MdtSingleLimitGetStiffness(sl));
        MdtSingleLimitSetDamping(hi_limit,
            MdtSingleLimitGetDamping(sl));
        MdtSingleLimitSetRestitution(hi_limit,
            MdtSingleLimitGetRestitution(sl));

        if (NewStop < MdtSingleLimitGetStop(low_limit))
            MdtSingleLimitSetStop(low_limit, NewStop);
    }
}

/**
 * This sets an offset used to transform the measured relative
 * position coordinate into the user's coordinate system.
 * It does not change the actual position or orientation of any
 * modelled object.
 */
void MEAPI MdtLimitSetPosition( MdtLimitID l,  const MeReal NewPosition )
{
    l->offset = NewPosition - l->position;
}

/**
 * Activates (if NewActivationState is non-zero) or deactivates (if
 * zero) the limit, without changing any other limit property.
 */
void MEAPI MdtLimitActivateLimits(const MdtLimitID l,
    const MeBool NewActivationState)
{
    MdtCHECKLIMIT(l,"MdtLimitActivateLimits");
    l->bLimited = NewActivationState;
    l->bCalculatePosition = NewActivationState || l->bPowered || l->is_locked;
}


/**
 * Set or clear the "calculate position" flag without changing the limit's
 * activation state.  Note that if the limit is currently activated or powered,
 * the "calculate position" flag cannot be cleared.
 */
void MEAPI MdtLimitCalculatePosition(const MdtLimitID l,  const MeBool NewState)
{
    MdtCHECKLIMIT(l,"MdtLimitCalculatePosition");
    l->bCalculatePosition = NewState || l->bLimited || l->bPowered || l->is_locked;
}


/**
 * Sets the limit stiffness threshold.
 *
 * When a limit stiffness exceeds this value, damping is ignored and
 * only the restitution property is used.
 *
 * When the limit stiffness is at or below this threshold, restitution
 * is ignored, and the stiffness and damping terms are used to simulate
 * a damped spring.
 *
 * The stiffness threshold is enforced to be non-negative: the initial value
 * is MEINFINITY.
 */
void MEAPI MdtLimitSetStiffnessThreshold(const MdtLimitID l,
                                         const MeReal NewStiffnessThreshold )
{
    MdtCHECKLIMIT(l,"MdtLimitSetStiffnessThreshold");
    if (NewStiffnessThreshold >= 0)
        l->damping_thresh = NewStiffnessThreshold;
    else
        l->damping_thresh = -NewStiffnessThreshold;
}

/**
 * Activates (if NewActivationState is non-zero) or deactivates (if
 * zero) the limited-force motor on this joint axis, without changing
 * any other limit property.
 */
void MEAPI MdtLimitActivateMotor(const MdtLimitID  l,
    const MeBool NewActivationState)
{
    MdtCHECKLIMIT(l,"MdtLimitActivateMotor");
    l->bPowered = NewActivationState;
    if ( l->bPowered ) l->is_locked = 0;
    l->bCalculatePosition = NewActivationState || l->bLimited ||  l->is_locked;
}

/**
 * Sets the limited-force motor parameters, enforcing a non-negative
 * value of forceLimit.
 *
 * If the latter is zero, this service deactivates the motor: otherwise,
 * the motor is activated.  This service does *not* wake attached
 * disabled bodies.
 */
void MEAPI MdtLimitSetLimitedForceMotor(const MdtLimitID  l,
    const MeReal desiredVelocity, const MeReal forceLimit)
{
    MdtCHECKLIMIT(l,"MdtLimitSetLimitedForceMotor");

    l->desired_vel = desiredVelocity;
    l->bPowered = (forceLimit != 0);
    l->is_locked = 0;

    if (forceLimit < 0)
        l->fmax = -forceLimit;
    else
        l->fmax = forceLimit;
}


/**
 * Sets the limited-force position lock parameters, enforcing a non-negative
 * value of forceLimit.
 *
 * If the latter is zero, this service deactivates the lock: otherwise, the
 * lock is activated. This service does *not* wake attached disabled
 * bodies.
 *
 * The joint degree of freedom will subsequently be locked at the given
 * value up to the limited force specified.
 */
void MEAPI MdtLimitSetLock(const MdtLimitID  l,
    const MeReal position, const MeReal forceLimit)
{
    MdtCHECKLIMIT(l,"MdtLimitSetLimitedForceMotor");

    l->desired_vel = (MeReal)0;
    l->is_locked = (forceLimit != 0);
    l->bPowered =  0;
    l->position_lock = position;

    if (forceLimit < 0)
        l->fmax = -forceLimit;
    else
        l->fmax = forceLimit;
}


/**
 * Returns non-zero if the limit is locked, and zero if it is not.
 *
 * Joint limits are unlocked by default.
 */
MeBool MEAPI MdtLimitIsLocked(const MdtLimitID l)
{
  return l->is_locked;
}


/**
 * Activates (if NewActivationState is non-zero) or deactivates (if
 * zero) the limited-force lock on this joint axis, without changing
 * any other limit property.
 */
void MEAPI MdtLimitActivateLock(const MdtLimitID  l,
    const MeBool NewActivationState)
{
    MdtCHECKLIMIT(l,"MdtLimitActivateLock");
    l->is_locked = NewActivationState;
    if(l->is_locked) l->bPowered = 0;
    l->bCalculatePosition = NewActivationState || l->bLimited || l->is_locked;
}


/**
 * Resets the state parameters of a limit:
 * bPositionInitialised, bRelaxingToLimit,  overshoot, previous_position
 * 
 */
void MEAPI MdtLimitResetState(const MdtLimitID  l)
{
    MdtCHECKLIMIT(l,"MdtLimitResetState");
    l->bPositionInitialised = 0;
    l->bRelaxingToLimit = 0;
    l->overshoot = 0;
    l->previous_position = 0;
    l->position = 0;
}

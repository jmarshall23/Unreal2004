/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/19 10:30:26 $ - Revision: $Revision: 1.114.2.14 $

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
#include <MdtCheckMacros.h>
#include <MeProfile.h>
#include <MeMemory.h>
#include <MdtWorld.h>
#include <MdtBody.h>
#include <MeVersion.h>
#include <Mdt.h>
#include "MdtUtils.h"
#include <MeDebugDraw.h>
#include <MdtDefaults.h>
#include "MdtAlignment.h"

/**
 * Allocates memory for a new MdtWorld, and resets to default values.
 *
 * @param maxBodies      Maximum number of bodies in this world.
 * @param maxConstraints Maximum number of constraints in this world.
 */
MdtWorldID MEAPI MdtWorldCreate(const unsigned int maxBodies,
                                const unsigned int maxConstraints,
                                const MeReal lengthScale, 
                                const MeReal massScale)
{
    MdtWorld *w;
    int largestConstraintSize = 0;

    if(maxBodies == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtWorldCreate: The maximum number of bodies must be "
            "greater than zero.\n");
#endif
        return 0;
    }

#ifdef _MECHECK
    if (!MeMemoryAPI.create)
    {
        MeFatalError(0,"MdtWorldCreate: MeMemory has not been initialized "
            "correctly.");
    }
#endif

    w = (MdtWorld *) MeMemoryAPI.create(sizeof (MdtWorld));

    if(!w)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtWorldCreate: Could not allocate memory for world.");
#endif
        return w;
    }

    /* write default length, mass, density */

    w->params.lengthScale = lengthScale;
    w->params.massScale = massScale;
    w->params.defaultDensity = massScale/(lengthScale * lengthScale * lengthScale);

    /* Work out what processor resources this machine has (PC only) */
    w->keaParams.cpu_resources = MdtKeaQueryCPUResources();

    /* Initialise Kea memory pool to 1K but allow to auto-resize. */
    MeChunkInit(&w->keaPool, 16);

    /* Initialise body pool/arrays. */
    w->nBodies = 0;
    w->nEnabledBodies = 0;
    w->maxBodies = maxBodies;

    MeChunkInit(&w->keaTMChunk, MDTKEATMARRAY_ALIGN);
    
    (*MePoolAPI.init)(&w->bodyPool, w->maxBodies, sizeof(MdtBody), 16);

    /* Initialise constraint pool. */
    w->maxConstraints = maxConstraints;
    w->nEnabledConstraints = 0;

    /* URGH!! Work out what the size of the largest constraint is. */
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtContact));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtContactGroup));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtBSJoint));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtHinge));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtPrismatic));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtCarWheel));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtFixedPath));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtRPROJoint));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtUniversal));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtLinear1));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtLinear2));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtAngular3));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtSpring));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtConeLimit));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtUserConstraint));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtBaseConstraint));
    largestConstraintSize = MeMAX(largestConstraintSize, sizeof(MdtSkeletal));

    (*MePoolAPI.init)(&w->constraintPool,
        w->maxConstraints, largestConstraintSize, 0);

    /* Initialise dictionaries. */

    MeDictInit(&w->bodyDict,1000000,MdtDictCompare);
    MeDictAllowDupes(&w->bodyDict);
    MeDictInit(&w->enabledBodyDict,1000000,MdtDictCompare);
    MeDictAllowDupes(&w->enabledBodyDict);
    MeDictInit(&w->constraintDict,1000000,MdtDictCompare);
    MeDictAllowDupes(&w->constraintDict);

    MeChunkInit(&w->partOutChunk, 0); /* unaligned */


    MeChunkInit(&w->keaConstraintsChunk, 0);

    w->bodyDisableCallback = 0;
    w->bodyEnableCallback = 0;
    w->contactGroupDestroyCallback = 0;
  
    /* Reset the world to its default parameters. */
    MdtWorldReset(w);

    w->toolkitVersionString = MeToolkitVersionString();
    return w;
}


/**
 *  Destroys an MdtWorld and all bodies, constraints etc.
 */
void MEAPI MdtWorldDestroy(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldDestroy");

    MeChunkTerm(&w->keaTMChunk);

    (*MePoolAPI.destroy)(&w->bodyPool);

    MeChunkTerm(&w->partOutChunk);

    (*MePoolAPI.destroy)(&w->constraintPool);
    
    MeChunkTerm(&w->keaConstraintsChunk);

    MeChunkTerm(&w->keaPool);

    MeMemoryAPI.destroy(w);
}

/**
 * Initialises a world and sets default values.
 *
 * Default values:
 *
 * @arg gravity = {0, 0, 0} (in world reference frame)
 * @arg epsilon = 0.01
 * @arg gamma = 0.2
 *
 * @arg MinSafeTime = 0.001
 *
 * @arg AutoDisable = 1
 * @arg AutoDisableVelocityThreshold = 0.02
 * @arg AutoDisableAngularVelocityThreshold = 0.001
 * @arg AutoDisableAccelerationThreshold = 0.5
 * @arg AutoDisableAngularAccelerationThreshold = 0.002
 *
 * @arg MaxMatrixSize = INFINITY
 *
 * @arg DebugDrawOptions = 0
 *
 * @see struct MdtWorld
 *
 */
void MEAPI MdtWorldReset(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldReset");

    w->keaParams.epsilon = MDTWORLD_DEFAULT_EPSILON / w->params.massScale;

    w->constantGamma = w->keaParams.gamma = MDTWORLD_DEFAULT_GAMMA;
    w->keaParams.velocityZeroTol = MDTWORLD_DEFAULT_VELOCITY_ZERO_TOL * w->params.lengthScale;

    w->minSafeTime = MDTWORLD_DEFAULT_MIN_SAFETIME;

    /* Default auto-disable parameters */
    w->partitionParams.vel_thresh = MDTWORLD_DEFAULT_VEL_THRESH * w->params.lengthScale;
    w->partitionParams.velrot_thresh = MDTWORLD_DEFAULT_VELROT_THRESH;
    w->partitionParams.acc_thresh = MDTWORLD_DEFAULT_ACC_THRESH  * w->params.lengthScale;
    w->partitionParams.accrot_thresh = MDTWORLD_DEFAULT_ACCROT_THRESH;
    w->partitionParams.alive_time_thresh = MDTWORLD_DEFAULT_ALIVE_TIME_THRESH;

    /* Default partition-LOD parameters. */
    w->partitionParams.lodParams.frictionRatio = MDTWORLD_DEFAULT_FRICTION_RATIO;
    w->partitionParams.lodParams.zeroRowBonus = MDTWORLD_DEFAULT_ZERO_ROW_BONUS;
    w->partitionParams.lodParams.toWorldBonus = MDTWORLD_DEFAULT_TO_WORLD_BONUS;
    w->partitionParams.lodParams.penetrationBias = MDTWORLD_PENETRATION_BIAS * w->params.lengthScale;
    w->partitionParams.lodParams.normVelBias = MDTWORLD_NORM_VEL_BIAS * w->params.lengthScale;
    w->partitionParams.lodParams.rowCountBias = MDTWORLD_ROW_COUNT_BIAS;
    w->partitionParams.lodParams.nonAutoBonus = MDTWORLD_DEFAULT_NON_AUTO_BONUS;

    /* Default matrix-size capping parameters */
    w->partitionParams.maxMatrixSize = MDTMAXMATRIXSIZE_MAX;

    w->partitionParams.autoDisable = 1;

    w->params.gravity[0] = MDTWORLD_DEFAULT_GRAVITY_X;
    w->params.gravity[1] = MDTWORLD_DEFAULT_GRAVITY_Y;
    w->params.gravity[2] = MDTWORLD_DEFAULT_GRAVITY_Z;

    w->partitionParams.debugOptions = 0;

    w->params.matrixSizeLog = 0;
    w->params.matrixSizeLogSize = 0;

    w->keaParams.max_iterations = 10;

    /* By default, dont dump any kea debugging info */
    w->keaParams.debug.readKeaInputData   = MEFALSE;
    w->keaParams.debug.writeKeaInputData  = MEFALSE;
    w->keaParams.debug.writeKeaInterData  = MEFALSE;
    w->keaParams.debug.writeKeaOutputData = MEFALSE;

    /* Sim checking */

#ifdef _MECHECK
    w->checkSim = 1;
#else
    w->checkSim = 0;
#endif

    w->simErrorCallback = MdtDefaultSimErrorCallBack;
    w->simErrorUserData = 0;
}


/*
  Accessors
*/

/**
 * Returns maximum number of MdtBody's allowed in this world.
 */
int MEAPI MdtWorldGetMaxBodies(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetMaxBodies");
    return w->maxBodies;
}

/**
 * Returns maximum number of constraints allowed in this world.
 */
int MEAPI MdtWorldGetMaxConstraints(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetMaxConstraints");
    return w->maxConstraints;
}

/**
 * Returns the maximum amount of the Kea memory pool that has been
 * used on any solve so far.
 * This function is deprecated. Use MdtWorldGetKeaPoolChunk then 
 * MeChunkGetMaxSize.
 *
 * @see MdtWorldGetKeaPoolChunk
 * @see MeChunkGetMaxSize
 */
int MEAPI MdtWorldGetMaxMemoryPoolUsed(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetMaxMemoryPoolUsed");
    return MeChunkGetMaxSize(&w->keaPool);
}

/**
 *  Return the MeChunk structure currently being used for the Kea memory pool.
 *  This is temporary memory only used during Kea solves.
 */
MeChunk* MEAPI MdtWorldGetKeaPoolChunk(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetKeaPoolChunk");
    return &(w->keaPool);
}

/**
 *  Return the MeChunk structure currently being used for the 
 *  Kea body transform array (low-level input to Kea) each frame.
 *  This is temporary memory only used during Kea solves.
 */
MeChunk* MEAPI MdtWorldGetKeaTMChunk(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetKeaTMChunk");
    return &(w->keaTMChunk);
}

/**
 *  Return the MeChunk structure currently being used for the 
 *  MdtKeaConstraints structure (low-level input to Kea) each frame.
 *  This is temporary memory only used during Kea solves.
 */
MeChunk* MEAPI MdtWorldGetKeaConstraintsChunk(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetKeaConstraintsChunk");
    return &(w->keaConstraintsChunk);
}

/**
 *  Return the MeChunk structure currently being used to provide temporary
 *  storage for the partitioned world within each MdtWorldStep.
 */
MeChunk* MEAPI MdtWorldGetPartitionOutputChunk(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetPartitionOutputChunk");
    return &(w->partOutChunk);
}

/**
 * Returns the gravity vector, in world reference frame.
 */
void MEAPI MdtWorldGetGravity(const MdtWorldID w, MeVector3 g)
{
    MdtCHECKWORLD(w,"MdtWorldGetGravity");

    g[0] = w->params.gravity[0];
    g[1] = w->params.gravity[1];
    g[2] = w->params.gravity[2];

}

/**
 * Returns the amount of allowed constraint violation.
 */
MeReal MEAPI MdtWorldGetEpsilon(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetEpsilon");
    return w->keaParams.epsilon;
}

/**
 * Returns the amount to restore violated constraints.
 */
MeReal MEAPI MdtWorldGetGamma(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetGamma");
    return w->constantGamma;
}

/** 
 * Returns the minimum amount to step a body by when using 
 * MdtWorldStepSafeTime. If using MdtWorldStep, this number is ignored.
 */
MeReal MEAPI MdtWorldGetMinSafeTime(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetMinSafeTime");
    return w->minSafeTime;
}


/**
 * Returns the total number of bodies in the world
 * regardless of whether they are enabled or not.
 */
int MEAPI MdtWorldGetTotalBodies(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetTotalBodies");
    return w->nBodies;
}

/**
 * Returns the number of enabled bodies in the world.
 */
int MEAPI MdtWorldGetEnabledBodies(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetEnabledBodies");
    return w->nEnabledBodies;
}

/**
 * Returns the number of constraints in the world
 * regardless of whether they are enabled or not.
 */
int MEAPI MdtWorldGetTotalConstraints(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetTotalConstraints");
    return (*MePoolAPI.getUsed)(&w->constraintPool);
}

/**
 * Returns the number of enabled constraints in the world.
 */
int MEAPI MdtWorldGetEnabledConstraints(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetEnabledConstraints");
    return w->nEnabledConstraints;
}
/**
 * Returns an integer indicating whether AutoDisable is on or off.
 * 0 indicates off, 1 indicates on.
 */
MeBool MEAPI MdtWorldGetAutoDisable(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetAutoDisable");
    return w->partitionParams.autoDisable;
}

/**
 * Returns the minimum velocity before auto-disabling objects.
 */
MeReal MEAPI MdtWorldGetAutoDisableVelocityThreshold(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetAutoDisableVelocityThreshold");
    return w->partitionParams.vel_thresh;
}

/**
 * Returns the minimum angular velocity before auto-disabling objects.
 */
MeReal MEAPI MdtWorldGetAutoDisableAngularVelocityThreshold(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetAutoDisableAngularVelocityThreshold");
    return w->partitionParams.velrot_thresh;
}

/**
 * Returns the minimum acceleration before auto-disabling objects.
 */
MeReal MEAPI MdtWorldGetAutoDisableAccelerationThreshold(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetAutoDisableAccelerationThreshold");
    return w->partitionParams.acc_thresh;
}

/**
 * Returns the minimum angular acceleration before auto-disabling objects.
 */
MeReal MEAPI MdtWorldGetAutoDisableAngularAccelerationThreshold(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetAutoDisableAngularAccelerationThreshold");
    return w->partitionParams.accrot_thresh;
}

/**
 * Returns the minimum amount of evolved time before auto-disabling
 * a body, once enabled.
 */
MeReal MEAPI MdtWorldGetAutoDisableAliveTime(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetAutoDisableAliveWindow");
    return w->partitionParams.alive_time_thresh;
}

/**
 *  Returns the maximum allowed matrix size passed to Kea to solve.
 *  A value of MDTMAXMATRIXSIZE_MAX indicates no capping is being performed.
 *  @see MdtWorldSetMaxMatrixSize
 */
int MEAPI MdtWorldGetMaxMatrixSize(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetMaxMatrixSize");
    return w->partitionParams.maxMatrixSize;
}

/**
 *  Returns the maximum number of LCP iterations that will be performed inside
 *  the Kea solver. Default is 10.
 */
int MEAPI MdtWorldGetMaxLCPIterations(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetMaxLCPIterations");
    return w->keaParams.max_iterations;
}

/**
 *  Returns current parameters used when re-sizing a partition that is
 *  larger than the current MaxMatrixSize allowed.
 */
void MEAPI MdtWorldGetLODParams(const MdtWorldID w, 
                                MdtLODParams* const lodParams)
{
    MdtCHECKWORLD(w,"MdtWorldGetLODParams");
    *lodParams = w->partitionParams.lodParams;
}

/** Return whether simulation checking is currently enabled. */
MeBool MEAPI MdtWorldGetCheckSim(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetCheckSim");
    return w->checkSim;
}

/** 
 *  Return current function executed on a simulation error.
 *  @see MdtWorldSetSimErrorCB
 */
MdtSimErrorCBPtr MEAPI MdtWorldGetSimErrorCB(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetSimErrorCB");
    return w->simErrorCallback;
}

MeReal MEAPI MdtWorldGetDefaultDensity(const MdtWorldID w)
{
    MdtCHECKWORLD(w,"MdtWorldGetDefaultDensity");
    return w->params.defaultDensity;
}


/*
  Mutators
*/


/**
 * Sets epsilon for a world.
 */
void MEAPI MdtWorldSetEpsilon(const MdtWorldID w, const MeReal e)
{
    MdtCHECKWORLD(w,"MdtWorldSetEpsilon");
    w->keaParams.epsilon = e;
}

/**
 * Sets gamma for a world.
 *
 * Gamma is the relaxation rate of constraints.
 */
void MEAPI MdtWorldSetGamma(const MdtWorldID w, const MeReal g)
{
    MdtCHECKWORLD(w,"MdtWorldSetGamma");
    w->keaParams.gamma = g;
    w->constantGamma = g;
}

/**
 * Sets gamma for a world with reference time step.
 *
 * Gamma is the relaxation rate of constraints.
 */

void  MEAPI MdtWorldSetGammaWithRefTimeStep(const MdtWorldID w,
                const MeReal aGamma, const MeReal aRefStep, 
                const MeReal aTimeStep)
{
    MdtCHECKWORLD(w,"MdtWorldSetGammaWithRefTimeStep");
    w->constantGamma = w->keaParams.gamma = aGamma * aTimeStep/aRefStep;
}

/**
 *  Set the minimum value to evolve a body by using MdtWorldSafeTime. Even if
 *  a partition has a body with a 'safetime' that is less than this number,
 *  the partition will be stepped by no less than this amount. If using
 *  MdtWorldStep, this value is ignored.
 */
void MEAPI MdtWorldSetMinSafeTime(const MdtWorldID w, const MeReal t)
{
    MdtCHECKWORLD(w,"MdtWorldSetGammaWithRefTimeStep");
    w->minSafeTime = t;
}


/**
 * Turns on and off the AutoDisable feature.
 * 0 indicated off, 1 indicates on.
 */
void MEAPI MdtWorldSetAutoDisable(const MdtWorldID w, const MeBool d)
{
    MdtCHECKWORLD(w,"MdtWorldSetAutoDisableMode");
    w->partitionParams.autoDisable = d;
}


/**
 * Sets the velocity threshold below which an object will be auto-disabled.
 *
 * The value represents the sum of the squared elements of the objects
 * velocity.
 */
void MEAPI MdtWorldSetAutoDisableVelocityThreshold(const MdtWorldID w, const MeReal vt)
{
    MdtCHECKWORLD(w,"MdtWorldSetAutoDisableVelocityThreshold");
    w->partitionParams.vel_thresh = vt;
}

/**
 * Sets the angular velocity threshold below which
 * an object will be auto-disabled.
 *
 * The value represents the sum of the squared elements of the objects
 * angular velocity.
 */
void MEAPI MdtWorldSetAutoDisableAngularVelocityThreshold(const MdtWorldID w, const MeReal avt)
{
    MdtCHECKWORLD(w,"MdtWorldSetAutoDisableAngularVelocityThreshold");
    w->partitionParams.velrot_thresh = avt;
}

/**
 * Sets the acceleration threshold below
 * which an object will be auto-disabled.
 *
 * The value represents the sum of the squared elements of the objects
 * acceleration.
 */
void MEAPI MdtWorldSetAutoDisableAccelerationThreshold(const MdtWorldID w, const MeReal at)
{
    MdtCHECKWORLD(w,"MdtWorldSetAutoDisableAccelerationThreshold");
    w->partitionParams.acc_thresh = at;
}

/**
 * Sets the angular acceleration threshold below which an object will be
 * auto-disabled.
 *
 * The value represents the sum of the squared elements of the objects
 * angular acceleration.
 */
void MEAPI MdtWorldSetAutoDisableAngularAccelerationThreshold(const MdtWorldID w,
                                                              const MeReal aat)
{
    MdtCHECKWORLD(w,"MdtWorldSetAutoDisableAngularAccelerationThreshold");
    w->partitionParams.accrot_thresh = aat;
}

/**
 * Sets the minimum amount of evolved time before auto-disabling
 * a body, once enabled.
 */
void MEAPI MdtWorldSetAutoDisableAliveTime(const MdtWorldID w, 
                                             const MeReal aw)
{
    MdtCHECKWORLD(w,"MdtWorldSetAutoDisableAliveWindow");
    w->partitionParams.alive_time_thresh = aw;
}

/**
 *  Set the maximum single matrix size that will ever be solved by Kea.
 *  If very large systems are formed (ie. very large piles of boxes) a very
 *  large matrix can result. This both takes a long time to solve, and takes 
 *  a large amount of memory to store. Using this function tells Mdt to take
 *  measures (turning off friction, discarding contacts etc.) to reduce the
 *  complexity of matrices over this the given size. This can obviously cause
 *  things to behave in an undesirable way, but does guarantee an upper bound
 *  on memory usage.
 *  The default MaxMatrixSize is MDTMAXMATRIXSIZE_MAX 
 *  ie. no capping is performed.
 */
void MEAPI MdtWorldSetMaxMatrixSize(const MdtWorldID w, const int size)
{
    MdtCHECKWORLD(w,"MdtWorldSetMaxMatrixSize");

    /* Ensure max matrix size is at least 4 */
    w->partitionParams.maxMatrixSize = MeMAX(MeMathCEIL4(size), 4);

    /* TODO: Work out maximum memory needed for this matrix size and 
       allocate it. Can't at the moment because of MdtKeaMemoryRequired
       needing rows per partition rather than just total rows.*/

    /* MdtKeaMemoryRequired(...) */
}

/**
 * Sets the gravity for the world in the world reference frame.
 */
void MEAPI MdtWorldSetGravity(const MdtWorldID w, const MeReal gx,
                              const MeReal gy, const MeReal gz)
{
    MdtCHECKWORLD(w,"MdtWorldSetGravity");
    w->params.gravity[0] = gx;
    w->params.gravity[1] = gy;
    w->params.gravity[2] = gz;
}

/**
 * Enables optional debug information to be drawn graphically using a 
 * user-registered function as part of MeDebugDrawAPI.
 * By default no debug information is drawn. Any boolean combination of
 * MdtDebugDrawOptions is valid.
 *
 * @see MdtDebugDrawOptions
 * @see MeDebugDrawAPI
 */
void MEAPI MdtWorldSetDebugDrawing(const MdtWorldID w,
               const MdtDebugDrawOptions drawOptions)
{
    MdtCHECKWORLD(w,"MdtWorldSetDebugDrawing");
    w->partitionParams.debugOptions = drawOptions;
}
/**
 * Enables debug information from kea to be written to a file
 * Also enables kea to write and read its input to a file
 * As kea is stateless, this is useful for debugging
 * 
 * @see MdtKeaDebugDataRequest
 */

void MEAPI MdtWorldSetKeaDebugRequest(const MdtWorldID w,
               const MdtKeaDebugDataRequest debugDataRequest)
{
    MdtCHECKWORLD(w,"MdtWorldSetKeaDebugRequest");
    w->keaParams.debug = debugDataRequest;
}

/**
* Reset all forces on all bodies in the world.
*/
void MEAPI MdtWorldResetForces(const MdtWorldID w)
{
    MeDict *dict = &w->bodyDict;
    MeDictNode *node;
    
    MdtCHECKWORLD(w,"MdtWorldResetForces");    
    
    for(node = MeDictFirst(&w->bodyDict); node!=0; node = MeDictNext(dict, node))
    {
        MdtBodyResetForces((MdtBodyID)MeDictNodeGet(node));
    }
}

/** 
 *  Set memory for optional matrix size-logging, enabling logging.
 *  It can be useful to log what the distribution of matrix sizes are during 
 *  the lifetime of the MdtWorld. The user provides an array of integers for 
 *  storing this cumulative log. Each integer indicates a particular matrix 
 *  size, rising in multiples of four. For example, if the array were 4 long,
 *  the elements would indicate number of matrices of size 4, 8, 12, and 16 
 *  solved during the lifetime of the MdtWorld. If a matrix is encountered 
 *  larger than will fit into this log, it will be added to the final element.
 *  In this example, the final element would indicate 'matrices of size 16 or
 *  larger'.
 *  Setting both 'sizeLog' and 'logSize' to zero will disable matrix size 
 *  logging.
 */
void MEAPI MdtWorldSetMatrixSizeLog(const MdtWorldID w, int *const sizeLog, 
                                    const int logSize)
{
    int i;
    
    MdtCHECKWORLD(w,"MdtWorldSetMatrixSizeLog");        

    w->params.matrixSizeLog = sizeLog;
    w->params.matrixSizeLogSize = logSize;

    /* Reset all counts to zero. */
    for(i=0; i<w->params.matrixSizeLogSize; i++)
    {
        w->params.matrixSizeLog[i] = 0;
    }
}

/** 
 *  Set the maximum number of LCP iterations used inside the Kea solver.
 *  Setting this lower can result in 'sticky' behaviour, but can increase
 *  performance.
 *  The default value is 10.
 */
void MEAPI MdtWorldSetMaxLCPIterations(const MdtWorldID w, const int mi)
{
    MdtCHECKWORLD(w,"MdtWorldSetMaxLCPIterations");

    w->keaParams.max_iterations = mi;
}

/** 
 *  Set parameters used for re-sizing a partition when it is larger than
 *  MaxMatrixSize
 *  @see MdtWorldSetMaxMatrixSize
 */
void MEAPI MdtWorldSetLODParams(const MdtWorldID w,
                                const MdtLODParams* const lodParams)
{
    MdtCHECKWORLD(w,"MdtWorldSetLODParams");

    w->partitionParams.lodParams = *lodParams;


}

/** 
 *  Set callback that is executed when an error occurs in the simulation.
 *  This will happen when the physical system passed to Kea was invalid for 
 *  some reason, and forces to satisfy the constraints could not be calculated.
 *  This callback gives the user to oppurtunity to reset the invalid 
 *  forces/accelerations produced by Kea, as well as notifying them of the 
 *  error. The user should _not_ call any Mdt functions inside here (like
 *  MdtWorldDestroy), but should set a flag and do things after MdtWorldStep
 *  has finished. See MdtDefaultSimErrorCallBack in MdtMainLoop.c for
 *  an example of safely resetting the Kea output.
 *  
 *  @see MdtWorldSetCheckSim
 */
void MEAPI MdtWorldSetSimErrorCB(const MdtWorldID w,
                                     MdtSimErrorCBPtr cb, 
                                     void* secbdata)
{
    MdtCHECKWORLD(w,"MdtWorldSetSimErrorCB");

    w->simErrorCallback = cb;
    w->simErrorUserData = secbdata;
}

/** 
 *  Enable/disable sim checking.
 *  If Kea produces invalid results, the SimError callback will be executed.
 *  By default, this is enabled if _MECHECK is defined.
 *
 *  @see MdtWorldSetCheckSim
 */
void MEAPI MdtWorldSetCheckSim(const MdtWorldID w,
                               const MeBool c)
{
    MdtCHECKWORLD(w,"MdtWorldSetCheckSim");

    w->checkSim = c;
}


/* ******************* INTERNAL ***************************** */
/* Draw all the contacts in a partition using MeDebugDrawAPI. */
static void DrawPartitionContacts(MdtPartitionOutput* po, 
                                  int pid, MdtDebugDrawOptions o)
{
    int c;
    
    for(c = po->constraintsStart[pid];
    c < (po->constraintsStart[pid] + 
        po->constraintsSize[pid]); c++)
    {
        MdtConstraintID constraint = po->constraints[c];
        MdtContactGroupID group = 
            MdtConstraintDCastContactGroup(constraint);
        
        if(group)
        {
            MdtContactID contact;
            
            for(contact = group->first; contact; contact=contact->nextContact)
            {
                MeVector3 start;
                MdtContactGetPosition(contact, start);

                if(o & MdtDebugDrawContacts)
                {
                    MeVector3 end;
                    MdtContactGetNormal(contact, end);
                    MeVector3Scale(end, 
                        1 + (50 * MdtContactGetPenetration(contact)));
                    MeVector3Add(end, start, end);
                    MeDebugDrawAPI.line(start, end, 1, 0, 0);
                }

                if(o & MdtDebugDrawContactForce)
                {
                    MeVector3 end;
                    MdtContactGetForce(contact, 0, end);
                    MeVector3Scale(end, 1);
                    MeVector3Add(end, start, end);
                    MeDebugDrawAPI.line(start, end, 0, 1, 0);
                }
            }
        }
    }
}

static void CheckSim(MdtWorldID w,  
                     MdtKeaBody** keabodyArray, int nBodies, 
                     MdtKeaConstraints* keaCon)
{
    int i, j;
    MeBool isError = 0;
    
    /* Check each body. */
    for(i=0; i<nBodies && !isError; i++)
    {
        MdtKeaBody* b = keabodyArray[i];

        for(j=0; j<3; j++)
        {
            if(!MeRealIsValid(b->accel[j]))
                isError = 1;

            if(!MeRealIsValid(b->accelrot[j]))
                isError = 1;

            if(!MeRealIsValid(b->force[j]))
                isError = 1;
            
            if(!MeRealIsValid(b->torque[j]))
                isError = 1;
        }
    }
    
    /* Check each constraint. */
    for(i=0; i<keaCon->num_constraints && !isError; i++)
    {
        MdtKeaForcePair* fp = &(keaCon->force[i]);

        for(j=0; j<3; j++)
        {
            if(!MeRealIsValid(fp->primary_body.force[j]))
                isError = 1;

            if(!MeRealIsValid(fp->primary_body.torque[j]))
                isError = 1;

            if(!MeRealIsValid(fp->secondary_body.force[j]))
                isError = 1;

            if(!MeRealIsValid(fp->secondary_body.torque[j]))
                isError = 1;
        }
    }
    
    if(isError)
    {
        if(w->simErrorCallback)
        {
            w->simErrorCallback(keaCon, keabodyArray, 
                nBodies, w->simErrorUserData);
        }
#ifdef _MECHECK
        else
        {
            MeWarning(0, "MdtWorldStep: Error in sim, "
                "but no Sim Error callback.");
        }
#endif
    }
}

/** 
 *  Mdt 'Main Loop' loop function. This version packs all partitions in the
 *  MdtWorld into one set of Kea input and steps them all with the same
 *  step calling the Kea solver and integrator just once.
 *
 *  -   Run partitioner to divide world into partitions of bodies and 
 *      constraints that can be simulated seperately. The partitioner calls
 *      and optional callback at the end of each partition which is used 
 *      to auto-disable partitions when they come to rest.
 *  -   For each partition, update the forces on each body (this includes
 *      gravity and damping). Then, for each constraint, make it address bodies
 *      by index rather than pointer, and use its BCL function to turn it into
 *      MdtKeaConstraints Jacobean equation form.
 *  -   Once the Kea input is built, call the Kea constraint solver to
 *      calculate the forces that need to be applied to each body to satisfy
 *      all the constraints.
 *  -   Use the Kea integrator to update the positions of all the bodies.
 *  -   Finally, read out the forces generated by each constraint and store
 *      in Mdt constraint structs, to be used by user if desired.
 */
void MEAPI MdtWorldStep(const MdtWorldID w, const MeReal stepSize)
{
    int i, sizeRequired;
    int maxRows, totalConCount;
	MdtKeaForcePair *forceArray;
    MdtBody *b;
    MdtKeaBody **keabodyArray;
    MeDictNode *node;
    MeDict *dict;
    MdtPartitionOutput* po;

    MdtCHECKWORLD(w,"MdtWorldStep");

#ifdef PROFILE_MDT
    MeProfileStartSection("Pre-kea dynamics time",0);
#endif

    po = MdtPartOutCreateFromChunk(&w->partOutChunk, 
        w->nBodies, w->nEnabledConstraints);

    /* call the partitioner */    
    MdtUpdatePartitions(&w->enabledBodyDict, po,
        MdtAutoDisableLastPartition, &w->partitionParams);

#ifdef PROFILE_MDT
    MeProfileEndSection("Pre-kea dynamics time");
    MeProfileStartSection("pack",0);
#endif

    if(po->nPartitions > 0)
    {
        MdtKeaTransformation *keatmArray;
        MdtKeaConstraints* keaCon;
        
        keatmArray = (MdtKeaTransformation*)MeChunkGetMem(&w->keaTMChunk, 
            po->totalBodies * sizeof(MdtKeaTransformation));
        
        /* Create temporary input structure MdtKeaConstraints from MeChunk. */
        keaCon = MdtKeaConstraintsCreateFromChunk(
            &w->keaConstraintsChunk, 
            po->nPartitions, 
            po->overallInfo.contactCount + po->overallInfo.jointCount, 
            po->overallInfo.rowCount);

        /* Pack all of the partitions into one set of Kea input. */
        maxRows = MdtPackAllPartitions(po,
            stepSize, &w->params, &w->keaParams,
            keatmArray,
            keaCon);
        
            /*  Because we can cast from MdtBody* to MdtKeaBody*, this is how
        we obtain the array of MdtKeaBody* pointers. */
        keabodyArray = (MdtKeaBody**)(po->bodies);
        
        /* MAKE KEA PARAMETERS */
        
        /* Use non-timestep scaled gamma. */
        w->keaParams.gamma = w->constantGamma;
        w->keaParams.stepsize = stepSize;
        
        /*  Use kea utility to figure out how much memory pool we need, then 
        grab it from the MeChunk. */
        sizeRequired = MdtKeaMemoryRequired(
            keaCon->num_rows_exc_padding_partition,
            keaCon->num_partitions,
            maxRows,
            po->totalBodies);
        
        w->keaParams.memory_pool = MeChunkGetMem(&w->keaPool, sizeRequired);
        w->keaParams.memory_pool_size = sizeRequired;
                
#ifdef PROFILE_MDT
        MeProfileEndSection("pack");
        MeProfileStartSection("flush",0);
#endif
        
        MdtFlushCache(0);
        
#ifdef PS2
        /* On PS2, the lambda vector and constraint forces vector must be
        accessed in uncached or uncached accelerated mode. */
        
        keaCon->force  = (MdtKeaForcePair *) (
                             (unsigned int)(keaCon->force)|0x30000000
                         );

        keaCon->lambda = (MeReal *)(
                             (unsigned int)(keaCon->lambda)|0x30000000
                         );
#endif                
        
#ifdef PROFILE_MDT
        MeProfileEndSection("flush");
        MeProfileStartSection("kea",0);
#endif
        
        /* Run constraint solver to calculate forces required 
        to satisfy all constraints (in all partitions). */
        
        MdtKeaAddConstraintForces(*keaCon, 
            keabodyArray,
            keatmArray,
            po->totalBodies, 
            w->keaParams);

        /* If we are doing sim checking, do it here. */
        if(w->checkSim)
        {
            CheckSim(w, keabodyArray, po->totalBodies, keaCon);
        }

#ifdef PROFILE_MDT
        MeProfileEndSection("kea");
        MeProfileStartSection("integrate",0);
#endif
        
        /* Run integrator to update body positions. */
        MdtKeaIntegrateSystem(keabodyArray, 
            keatmArray,
            po->totalBodies, 
            w->keaParams);
        
        /* We are done with Kea's workspace now - put back into MeChunk. */
        MeChunkPutMem(&(w->keaPool), w->keaParams.memory_pool);
        
#ifdef PROFILE_MDT
        MeProfileEndSection("integrate");
        MeProfileStartSection("unpack",0);
#endif
        
        /* Copy transforms from Kea output back to MdtBody structs. */
        for(i = 0; i < po->nPartitions; i++)
        {
            MdtUnpackBodies(keatmArray + po->bodiesStart[i], i, po);
        }
        
        /* Finished with keatmArray, so we  return it to its Chunk. */
        MeChunkPutMem(&w->keaTMChunk, keatmArray);
        
        /* 
        Copy resulting forces from output to Mdt constraint structs.
        There is a subtlety here. Because one MdtContactGroup can generate
        multiple MdtKea consraints, we have to count how many constraints we
        have read forces from out of the MdtKeaConstraints.
        */
        forceArray = keaCon->force;
        
        totalConCount = 0;
        for(i = 0; i < po->nPartitions; i++)
        {
            int conCount = MdtUnpackForces(forceArray, i, po);
            forceArray = forceArray + conCount;
            totalConCount += conCount;
            
            /* Finally - draw contacts if desired. */
            if(w->partitionParams.debugOptions & 
                (MdtDebugDrawContacts | MdtDebugDrawContactForce))
            {
                DrawPartitionContacts(po, i,
                    w->partitionParams.debugOptions);
            }
        }
        MEASSERT(totalConCount == keaCon->num_constraints);
            
        MeChunkPutMem(&w->keaConstraintsChunk, keaCon);
    }
    
    
    MeChunkPutMem(&w->partOutChunk, po);

    /* Finally, reset forces on enabled bodies. */
    dict = &w->enabledBodyDict;
    for(node = MeDictFirst(dict); node != 0; node = MeDictNext(dict, node))
    {
        b = (MdtBodyID)MeDictNodeGet(node);
        MEASSERT(b->flags & MdtEntityEnabledFlag); /* Check its enabled */
        MdtBodyResetForces(b);
    }

#ifdef PROFILE_MDT
    MeProfileEndSection("unpack");
#endif

}

/** 
 *  Evolve all enabled bodies in the world, but evolving each partition with 
 *  seperate call to Kea with the timestep capped by each partitions 
 *  'safetime'. If you set the 'safetime' of a body, its partition will not be 
 *  stepped by more than that amount. This function is used to avoid objects
 *  missing collisions etc. NOTE: this function scales 'gamma' for each
 *  partition automatically (in previous versions on Karma it was the users
 *  responsibility).
 *  This is obviously a very crude method for handling fast-moving objects 
 *  'missing' collisions. A more sophisticated approach would iterate several
 *  times until each body has been evolved by the full 'stepSize'.
 */
void MEAPI MdtWorldStepSafeTime(const MdtWorldID w, const MeReal stepSize)
{
    int partitionindex, constraintRows;
    MdtBody *b;
    MeDictNode *node;
    MeDict *dict;
    MdtPartitionOutput* po;
    
    MdtCHECKWORLD(w,"MdtWorldStep");

    po = MdtPartOutCreateFromChunk(&w->partOutChunk, 
        w->nBodies, w->nEnabledConstraints);

    /* call the partitioner */
    MdtUpdatePartitions(&w->enabledBodyDict, po,
            MdtAutoDisableLastPartition, &w->partitionParams);

    /* For _each_ partition pack, simulate and unpack. */
    for(partitionindex=0; 
        partitionindex < po->nPartitions; partitionindex++)
    {
        int conCount, partStart, sizeRequired;
        MeReal safeTime, timeStep;
        MdtKeaBody **keabodyArray;
        MdtKeaTransformation *keatmArray;
        MdtKeaConstraints* keaCon;    
        
        /* Get temporary memory to hold array of body transforms for Kea. */
        keatmArray = (MdtKeaTransformation*)MeChunkGetMem(
            &w->keaTMChunk, 
            po->bodiesSize[partitionindex] * sizeof(MdtKeaTransformation));

        /* Find the maximum time we are allowed to step this partition. 
           If this is infinity, it will have no effect, but if its less than
           'stepSize', it will reduce the amount of time bodies are evolved
           for, to avoid missing collisions etc. */
        safeTime = MdtPartitionGetSafeTime(po, partitionindex);

        /* Ensure safetime is greater than world min safe time */
        safeTime = MeMAX(safeTime, w->minSafeTime);

        timeStep = MeMIN(safeTime, stepSize);

        /* Create temporary MdtKeaConstraints structure for feeding Kea. */
        keaCon = MdtKeaConstraintsCreateFromChunk(
            &w->keaConstraintsChunk, 
            1, 
            po->info[partitionindex].contactCount + po->info[partitionindex].jointCount,
            po->info[partitionindex].rowCount);


        /* Pack one partition into one set of Kea input. */
        constraintRows = MdtPackPartition(po,
            partitionindex, timeStep, &w->params,  &w->keaParams,
            keatmArray,
            keaCon);

        /*  Because we can cast from MdtBody* to MdtKeaBody*, this is how
            we obtain the array of MdtKeaBody* pointers. */
        partStart = po->bodiesStart[partitionindex];
        keabodyArray = (MdtKeaBody**)&(po->bodies[partStart]);
        
        /* MAKE KEA PARAMETERS */
        
        /* Scale 'gamma' with timestep. If stepSize is unchanged, gamma will be
           the user-defined value. */
        w->keaParams.gamma = w->constantGamma/stepSize * timeStep;
        w->keaParams.stepsize = timeStep;
        
        /*  Use kea utility to figure out how much memory pool we need, then 
        grab it from the MeChunk. */
        sizeRequired = MdtKeaMemoryRequired(
            keaCon->num_rows_exc_padding_partition,
            keaCon->num_partitions,
            constraintRows,
            po->bodiesSize[partitionindex]);
        
        w->keaParams.memory_pool = MeChunkGetMem(&w->keaPool, sizeRequired);
        w->keaParams.memory_pool_size = sizeRequired;

        MdtFlushCache(0);

#ifdef PS2
        /* On PS2, the lambda vector and constraint forces vector must be 
           accessed in uncached or uncached accelerated mode. */
        
        keaCon->force  = (MdtKeaForcePair *) (
                             (unsigned int)(keaCon->force)|0x30000000
                         );

        keaCon->lambda = (MeReal *) (
                             (unsigned int)(keaCon->lambda)|0x30000000
                         );
#endif                

        /* Run constraint solver to calculate forces required 
        to satisfy all constraints (in all partitions). */

        MdtKeaAddConstraintForces(*keaCon, 
            keabodyArray,
            keatmArray,
            po->bodiesSize[partitionindex], 
            w->keaParams);
                
        if(w->checkSim)
        {
            CheckSim(w, keabodyArray, 
                po->bodiesSize[partitionindex], keaCon);
        }


        /* Run integrator to update body positions. */
        MdtKeaIntegrateSystem(keabodyArray, 
            keatmArray,
            po->bodiesSize[partitionindex], 
            w->keaParams);
        
        /* We are done with Kea's workspace now - put back into MeChunk. */        
        MeChunkPutMem(&(w->keaPool), w->keaParams.memory_pool);        


        /* Copy transforms from Kea output back to MdtBody structs. */
        MdtUnpackBodies(keatmArray, partitionindex, po);
        
        /* 
            Copy resulting forces from output to Mdt constraint structs.
            There is a subtlety here. Because one MdtContactGroup can generate
            multiple MdtKea consraints, we have to count how many constraints 
            we have read forces from out of the MdtKeaConstraints.
        */
        
        conCount = MdtUnpackForces(keaCon->force, 
            partitionindex, po);
        MEASSERT(conCount == keaCon->num_constraints);

        /* Finished with KeaConstraints - return to MeChunk. */
        MeChunkPutMem(&w->keaConstraintsChunk, keaCon);
        
        /* Finished with keatmArray, so we  return it to its Chunk. */
        MeChunkPutMem(&w->keaTMChunk, keatmArray);
    }

    /* We are done with the output of the partitioner, return to MeChunk. */
    MeChunkPutMem(&w->partOutChunk, po);

    /* Finally, reset forces on enabled bodies. */
    dict = &w->enabledBodyDict;
    for(node = MeDictFirst(dict); node != 0; node = MeDictNext(dict, node))
    {
        b = (MdtBodyID)MeDictNodeGet(node);
        MEASSERT(b->flags & MdtEntityEnabledFlag); /* Check its enabled */
        MdtBodyResetForces(b);

        /* Reset safe-time to inifinity (no upper limit for next frame) */
        b->safeTime = MEINFINITY; 
    }

}

/**
 * Iterate over all 'enabled' constraints in the world, calling
 * the supplied function on each.
 */
void MEAPI MdtWorldForAllConstraints(const MdtWorldID w,
               MdtConstraintIteratorCBPtr cb, void* ccbdata)
{
    MeDictNode *node;
    MeDict *dict = &w->constraintDict;

    MdtCHECKWORLD(w,"MdtWorldForAllConstraint");

    for(node = MeDictFirst(dict); node !=0; node = MeDictNext(dict, node))
    {
        (*cb)((MdtConstraintID)MeDictNodeGet(node), ccbdata);
    }
}



/* Never been a fake and I'm never phony,
   I got more flavour than the packet in macaroni. */



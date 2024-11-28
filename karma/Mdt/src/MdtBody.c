/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/08 11:54:05 $ - Revision: $Revision: 1.77.2.8 $

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
#include <stdio.h>
#include <MeMath.h>
#include "MdtUtils.h"
#include <MdtCheckMacros.h>
#include <MeMemory.h>
#include <MdtConstraint.h>
#include <MdtBody.h>


/**
 *  Creates a new body with default parameters.
 *  @see MdtBodyReset
 */
MdtBodyID MEAPI MdtBodyCreate(const MdtWorldID w)
{
    MdtBody* b;
    MdtCHECKWORLD(w,"MdtBodyCreate");

    b = (MdtBody*)(*MePoolAPI.getStruct)(&w->bodyPool);

    /* FixMe: Fails here on Linux */
    MEASSERT( (long unsigned)b % 16  == 0);

    if(!b)
    {
#ifdef _MECHECK
        ME_REPORT(MeWarning(0, "MdtBodyCreate: Body Pool Too Small."));
#endif
        return b;
    }

    b->world = w;
    b->keaBody.tag = MdtKeaBODYVER1;
    b->keaBody.len = sizeof (MdtKeaBody);
    b->sortKey = 0;
    b->model = NULL;
    b->flags = 0;

    /* body is disabled when created */

    MdtBodyReset(b);

    MeDictNodeInit(&b->worldNode,(void *)b->sortKey);
    MeDictNodePut(&b->worldNode,b);
    MeDictNodeInit(&b->worldEnabledNode,(void *)b->sortKey);
    MeDictNodePut(&b->worldEnabledNode,b);
    MeDictInsert(&w->bodyDict, &b->worldNode, (void *)b->sortKey);

    MeDictInit(&b->constraintDict,1000000,MdtDictCompare);
    MeDictAllowDupes(&b->constraintDict);

    w->nBodies++;

    return b;
}

/**
 * Destroys an MdtBody.
 * This will put the memory used back into the MdtBody pool.
 */
void MEAPI MdtBodyDestroy(const MdtBodyID b)
{
    MeDictNode *node, *next;

    MdtCHECKBODY(b,"MdtBodyDestroy");

    /* Remove body from linked list */
    MeDictDelete(&b->world->bodyDict, &b->worldNode);

    for(node = MeDictFirst(&b->constraintDict); node != 0; node = next)
    {
        MdtBaseConstraint *cp = (MdtBaseConstraint *)MeDictNodeGet(node);
        next = MeDictNext(&b->constraintDict, node);
#ifdef _MECHECK
        if(cp->head.tag!=MdtBclCONTACT && cp->head.tag!=MdtBclCONTACTGROUP)
            ME_REPORT(MeWarning(3, "You are destroying a body that has enabled joints. "
            "Removing automatically."));
#endif
        next = MeDictNext(&b->constraintDict, node);
        MdtConstraintDisable(cp);
        MdtConstraintDestroy(cp);        
    }

    /* if body was enabled, disable  */
    if(MdtBodyIsEnabled(b))
    {
#ifdef _MECHECK
        ME_REPORT(MeWarning(3, "You are destroying a body that is still enabled. "
            "Disabling automatically."));
#endif
        MdtBodyDisable(b);
    }

    /* ensure that body cannot be used again without being
       properly created */
    b->keaBody.tag = MdtBclNO_BODY;

    /* decrement number of bodies in world */
    b->world->nBodies--;

    (*MePoolAPI.putStruct)(&b->world->bodyPool, b);
}

/**
 * Puts a copy of an MdtBody into the supplied MdtWorld.
 */
MdtBodyID MEAPI MdtBodyCopy(const MdtBodyID b, const MdtWorldID w)
{
    MdtBodyID newb;
    MdtCHECKBODY(b,"MdtBodyCopy");
    MdtCHECKWORLD(w,"MdtBodyCopy");

    newb = MdtBodyCreate(w);

    MeFatalError(0, "MdtBodyCopy: NOT YET IMPLEMENTED.");

    return newb;
}

/**
 * Initialises a body and sets default values.
 * This will disable the body before reseting it.
 *
 * For default values see MdtDefaults.h. Everything
 * else defaults to either 0 or to the identity matrix.
 *
 * @see struct MdtBody
 */
void MEAPI MdtBodyReset(const MdtBodyID b)
{
    int i;
    MdtWorldParams *p;
    MdtCHECKBODY(b,"MdtBodyReset");

     p = &b->world->params;

    /* Disable body first */
    if(MdtBodyIsEnabled(b))
        MdtBodyDisable(b);

    b->keaBody.invmass = (MeReal)(1/(MDTBODY_DEFAULT_MASS*p->massScale));
    b->keaBody.flags = 0;

    for (i = 0; i < 4; i++)
    {
        b->keaBody.force[i] = 0;
        b->keaBody.torque[i] = 0;
        b->keaBody.invI0[i] = b->keaBody.invI1[i] =
            b->keaBody.invI2[i] = 0;
        b->keaBody.I0[i] = b->keaBody.I1[i] = b->keaBody.I2[i] = 0;
        b->keaBody.vel[i] = 0;
        b->keaBody.velrot[i] = 0;
        b->keaBody.qrot[i] = 0;
        b->keaBody.accel[i] = 0;
        b->keaBody.accelrot[i] = 0;
        b->keaBody.fastSpinAxis[i] = 0;

        b->impulseLinear[i] = 0;
        b->impulseAngular[i] = 0;
    }

    b->impulseAdded = 0;

    /* default quaternion is the identity quaternion */
    b->keaBody.qrot[0] = 1;

    b->keaBody.I0[0] = b->keaBody.I1[1] = b->keaBody.I2[2] = 
        MDTBODY_DEFAULT_INERTIA * p->massScale;

    /* inverted moment of inertia matrix */
    b->keaBody.invI0[0] = b->keaBody.invI1[1]
        = b->keaBody.invI2[2] = (MeReal)(1/(MDTBODY_DEFAULT_INERTIA * p->massScale));

    b->keaBody.fastSpinAxis[0] = MDTBODY_DEFAULT_FAST_SPIN_X;
    b->keaBody.fastSpinAxis[1] = MDTBODY_DEFAULT_FAST_SPIN_Y;
    b->keaBody.fastSpinAxis[2] = MDTBODY_DEFAULT_FAST_SPIN_Z;

    MeMatrix4TMMakeIdentity(b->comTM);
    MeMatrix4TMMakeIdentity(b->bodyTM);
    MeVector3Set(b->com,0,0,0);
    b->useCom = 0;           /* since com is zero, no need to use it */

    b->arrayIdWorld = -1;
    b->arrayIdPartition = -1;
    b->partitionIndex = -1;
    b->mass = MDTBODY_DEFAULT_MASS * p->massScale;

    b->flags = 0;

    b->damping = MDTBODY_DEFAULT_LINEAR_DAMPING;
    b->angularDamping = MDTBODY_DEFAULT_ANGULAR_DAMPING;

    b->enabledTime = 0;

    /* blank userdata field. */
    b->userData = 0;

    b->safeTime = MEINFINITY;
}


/**
 * Returns the WorldID of the world this body is part of.
 */
MdtWorldID MEAPI MdtBodyGetWorld(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetWorld");
    return b->world;
}

/**
 * Returns an integer indicating the simulation partition this
 * MdtBody belongs to. Returns -1 if constraint only effects
 * disabled bodies and is therefore not simulated.
 */
int MEAPI MdtBodyGetPartition(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetPartition");
    return b->partitionIndex;
}


/**
 * Returns a pointer to a bodies 4x4 transformation matrix.
 * The lifetime of this transformation is not guaranteed.
 */
MeMatrix4Ptr MEAPI MdtBodyGetTransformPtr(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetTransformPtr");
    return b->bodyTM;
}

/**
 * Returns a copy of a bodies 4x4 transformation matrix.
 */
void MEAPI MdtBodyGetTransform(const MdtBodyID b,MeMatrix4 m)
{
    MdtCHECKBODY(b,"MdtBodyGetTransform");
    MeMatrix4Copy( m, *(const MeMatrix4*)&b->bodyTM);
}

/**
 * Returns the body's UserData pointer.
 */
void *MEAPI MdtBodyGetUserData(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetUserData");
    return b->userData;
}


/**
 * Returns the body's mass.
 */
MeReal MEAPI MdtBodyGetMass(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetMass");
    return b->mass;
}


/**
 * The body's 3X3 inertia tensor is returned in @a i.
 * The inertia tensor of a body is given in the bodies reference frame.
 */
void MEAPI MdtBodyGetInertiaTensor(const MdtBodyID b, MeMatrix3 i)
{
    int j;

    MdtCHECKBODY(b,"MdtBodyGetInertiaTensor");

    for (j = 0; j < 3; j++)
    {
        i[0][j] = b->keaBody.I0[j];
        i[1][j] = b->keaBody.I1[j];
        i[2][j] = b->keaBody.I2[j];
    }
}


/**
 * The body's position vector is returned in @a p.
 * The position is of the bodies center of mass, and is given in the
 * world reference frame.
 */
void MEAPI MdtBodyGetPosition(const MdtBodyID b, MeVector3 p)
{
    MdtCHECKBODY(b,"MdtBodyGetPosition");
    MeVector3Copy( p, b->bodyTM[3]);
}

/**
 * The body's quaternion vector is returned in @a q.
 * The quaternion is in the form (w, x, y, z), and is relative to the
 * world reference frame.
 */
void MEAPI MdtBodyGetQuaternion(const MdtBodyID b, MeVector4 q)
{
    MdtCHECKBODY(b,"MdtBodyGetQuaternion");
    /* body and com ref frame have same orientation. */
    MeVector4Copy( q, b->keaBody.qrot);
}


/**
 * The body's 3X3 rotation matrix is returned in @a v.
 * The rotation matrix is relative to the world reference frame.
 */
void MEAPI MdtBodyGetOrientation(const MdtBodyID b, MeMatrix3 R)
{
    MdtCHECKBODY(b,"MdtBodyGetOrientation");
    MeMatrix4TMGetRotation( R, *(const MeMatrix4*)&b->bodyTM);
}

/**
 * The body's linear velocity vector is returned in @a v.
 * The velocity is given in the world reference frame.
 */
void MEAPI MdtBodyGetLinearVelocity(const MdtBodyID b, MeVector3 v)
{
    MdtCHECKBODY(b,"MdtBodyGetLinearVelocity");
    MeVector3Copy( v, b->keaBody.vel);
}


/**
 * A pointer to the body's linear velocity vector is returned.
 * The velocity is given in the world reference frame.
 */
MeReal *MEAPI MdtBodyGetLinearVelocityPtr(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetLinearVelocityPtr");
    return &(b->keaBody.vel[0]);
}



/**
 * The body's angular velocity vector is returned in @a v.
 * The angular velocity is given as as an (x, y, z) vector.
 */
void MEAPI MdtBodyGetAngularVelocity(const MdtBodyID b, MeVector3 v)
{
    MdtCHECKBODY(b,"MdtBodyGetAngularVelocity");
    MeVector3Copy( v, b->keaBody.velrot);
}


/**
 * A pointer to the body's angular velocity vector is returned.
 * The angular velocity is given as as an (x, y, z) vector.
 */
MeReal *MEAPI MdtBodyGetAngularVelocityPtr(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetAngularVelocityPtr");
    return &(b->keaBody.velrot[0]);
}


/**
 * Calculate the linear velocity of a point at a position 'p' given in the
 * world reference frame.
 */
void MEAPI MdtBodyGetVelocityAtPoint(const MdtBodyID b, MeVector3 p,
    MeVector3 v)
{
    MeVector3 tmp, toP;
    MdtCHECKBODY(b,"MdtBodyGetVelocityAtPoint");

    /* make vector from center-of-mass to point (still in world RF) */
    MeVector3Subtract( toP, p,  b->comTM[3]);
    MeVector3Cross( tmp, b->keaBody.velrot,  toP);
    MeVector3Add( v, b->keaBody.vel,  tmp);
}

/**
 * Return's the body's linear acceleration vector in @a v.
 *
 * Acceleration is READ ONLY, and is given in the world reference frame.
 */
void MEAPI MdtBodyGetLinearAcceleration(const MdtBodyID b, MeVector3 v)
{
    MdtCHECKBODY(b,"MdtBodyGetLinearAcceleration");
    MeVector3Copy( v, b->keaBody.accel);
}

/**
 * Return's the body's angular acceleration vector in @a v.
 *
 * Acceleration is READ ONLY, and is given in the world reference frame.
 */
void MEAPI MdtBodyGetAngularAcceleration(const MdtBodyID b, MeVector3 v)
{
    MdtCHECKBODY(b,"MdtBodyGetAngularAcceleration");
    MeVector3Copy( v, b->keaBody.accelrot);
}


/**
 * Return's the body's fast spin axis in @a v.
 */
void MEAPI MdtBodyGetFastSpinAxis(const MdtBodyID b, MeVector3 v)
{
    MdtCHECKBODY(b,"MdtBodyGetFastSpinAxis");
    MeVector3Copy( v, b->keaBody.fastSpinAxis);
}


/**
 * Returns the linear velocity damping of a body.
 *
 * Damping is a force (newtons/unit/sec) opposing the body's velocity.
 * 0 indicates no damping.
 */
MeReal MEAPI MdtBodyGetLinearVelocityDamping(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetLinearVelocityDamping");
    return b->damping;
}

/**
 * Returns the angular velocity damping of a body.
 *
 * Damping is a torque (newton-meters/radian/sec) opposing the body's
 * angular velocity.  0 indicates no damping.
 */
MeReal MEAPI MdtBodyGetAngularVelocityDamping(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetAngularVelocityDamping");
    return b->angularDamping;
}

/** Return's the force accumulated on the body so far. */
void MEAPI MdtBodyGetForce(const MdtBodyID b, MeVector3 v)
{
    MdtCHECKBODY(b,"MdtBodyGetForce");
    MeVector3Copy( v, b->keaBody.force);
}

/** Return's the force accumulated on the body so far. */
void MEAPI MdtBodyGetTorque(const MdtBodyID b, MeVector3 v)
{
    MdtCHECKBODY(b,"MdtBodyGetTorque");
    MeVector3Copy( v, b->keaBody.torque);
}

/** Return's the body's kinetic energy. */
MeReal MEAPI MdtBodyGetKineticEnergy(const MdtBodyID b)
{
    MeReal ke = 0;
    MdtCHECKBODY(b,"MdtBodyGetKineticEnergy");

    /* calculate linear m * v^2 */
    ke += b->mass * b->keaBody.vel[0] * b->keaBody.vel[0];
    ke += b->mass * b->keaBody.vel[1] * b->keaBody.vel[1];
    ke += b->mass * b->keaBody.vel[2] * b->keaBody.vel[2];

    /* calculate angular I * av^2 */
    ke += b->keaBody.I0[0] * b->keaBody.velrot[0] * b->keaBody.velrot[0];
    ke += b->keaBody.I1[0] * b->keaBody.velrot[0] * b->keaBody.velrot[1];
    ke += b->keaBody.I2[0] * b->keaBody.velrot[0] * b->keaBody.velrot[2];

    ke += b->keaBody.I0[1] * b->keaBody.velrot[1] * b->keaBody.velrot[0];
    ke += b->keaBody.I1[1] * b->keaBody.velrot[1] * b->keaBody.velrot[1];
    ke += b->keaBody.I2[1] * b->keaBody.velrot[1] * b->keaBody.velrot[2];

    ke += b->keaBody.I0[2] * b->keaBody.velrot[2] * b->keaBody.velrot[0];
    ke += b->keaBody.I1[2] * b->keaBody.velrot[2] * b->keaBody.velrot[1];
    ke += b->keaBody.I2[2] * b->keaBody.velrot[2] * b->keaBody.velrot[2];

    return (MeReal)(0.5) * ke;
}

/* Iterators */

/** Get first body in linked list of bodies in the MdtWorld. */
MdtBodyID MEAPI MdtBodyGetFirst(const MdtWorldID w)
{
    MeDictNode *first;
    MdtCHECKWORLD(w,"MdtBodyGetFirst");
    first = MeDictFirst(&w->bodyDict);
    if (first)
        return (MdtBody *)MeDictNodeGet(first);
    return NULL;
}

/** Get body following supplied body in list of bodies in the MdtWorld. */
MdtBodyID MEAPI MdtBodyGetNext(const MdtBodyID b)
{
    MeDictNode *node;
    MdtCHECKBODY(b,"MdtBodyGetNext");

    node = MeDictNext(&b->world->bodyDict,&b->worldNode);
    return node ? (MdtBodyID)MeDictNodeGet(node) : 0;
}

/**
 * Returns the number of frames that this body has been enabled for.
 * Returns -1 if body is disabled.
 */
MeReal MEAPI MdtBodyGetEnabledTime(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetEnabledTime");
    return b->enabledTime;
}

/** Get the relative transform from the body to its center of mass. 
  \deprecate
  The transform returned by this API always has zero rotation, because
  by definition the comTM and the bodyTM are parallel.  Hence,
  this API is deprecated.  You should use
  MdtBodyGetCenterOfMassRelativePosition instead.
*/
void MEAPI MdtBodyGetCenterOfMassRelativeTransform(const MdtBodyID b,
               MeMatrix4 t)
{
    MdtCHECKBODY(b,"MdtBodyGetCenterOfMassRelativeTransform");
    MeMatrix4TMMakeIdentity(t);
    if (b->useCom)
        MeVector3MultiplyScalar(t[3], b->com, -1);
}

/** Get the position of center-of-mass in body ref. frame. */
void MEAPI MdtBodyGetCenterOfMassRelativePosition(const MdtBodyID b, MeVector3 pos)
{
    MdtCHECKBODY(b,"MdtBodyGetCenterOfMassRelativePosition");
    MeVector3Copy( pos, b->com);
}

/** Get the position of the bodys center of mass (in world ref. frame). */
void MEAPI MdtBodyGetCenterOfMassPosition(const MdtBodyID b, MeVector3 pos)
{
    MdtCHECKBODY(b,"MdtBodyGetCenterOfMassPosition");
    MeMatrix4TMTransform( pos, b->bodyTM, b->com);
}

/**
 *  Returns body's center of mass transform pointer.
 *  \deprecate
 *  This API will be removed in future versions of Karma toolkit
 *  because the comTM is redundant.
 */
MeMatrix4Ptr MEAPI MdtBodyGetCenterOfMassTransformPtr(const MdtBodyID b)
{
	return b->comTM;
}

/**
 * Get currently set 'safe time' for body.
 * This is ignored by default, but can be used by the user.
 * @see MdtPartitionGetSafeTime
 */
MeReal MEAPI MdtBodyGetSafeTime(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetSafeTime");

    return b->safeTime;
}


/**
 * Get sort key for the dynamics body
 */
MeI32 MEAPI MdtBodyGetSortKey(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyGetSafeTime");

    return b->sortKey;
}



/* Mutators */

/** Sets the body's UserData pointer */
void MEAPI MdtBodySetUserData(const MdtBodyID b, void *d)
{
    MdtCHECKBODY(b,"MdtBodySetUserData");
    b->userData = d;
}


/** Set's the body's 4X4 transformation matrix. */
void MEAPI MdtBodySetTransform(const MdtBodyID b, const MeMatrix4 tm)
{
    MdtCHECKBODY(b,"MdtBodySetTransform");

#ifdef _MECHECK
    {
#if 1
        if(!MeMatrix4IsTM(tm, ME_MEDIUM_EPSILON))
            MeWarning(12, "MdtBodySetTransform: Transformation matrix "
                "of body is invalid.");
#else
        MeMatrix3 Orientation;
        MeMatrix4TMGetRotation( Orientation, tm);

        if( !MeMatrix3IsValidOrientationMatrix(Orientation, ME_SMALL_EPSILON))
            MeWarning(12, "MdtBodySetTransform: Transformation matrix "
                "of body is invalid.");
#endif
    }
#endif

    memcpy(&b->bodyTM, tm, sizeof(MeMatrix4));
    UpdateCOMTransform(b);
}

/**
 * Sets the position of an objects center of mass.
 * The position of an object is set in the world reference frame.
 */
void MEAPI MdtBodySetPosition(const MdtBodyID b, const MeReal x,
                        const MeReal y, const MeReal z)
{
    MdtCHECKBODY(b,"MdtBodySetPosition");

    b->bodyTM[3][0] = x;
    b->bodyTM[3][1] = y;
    b->bodyTM[3][2] = z;
    UpdateCOMTransform(b);
}

/** Sets the orientation of an object using 3X3 rotation matrix. */
void MEAPI MdtBodySetOrientation(const MdtBodyID b, const MeMatrix3 R)
{
    MdtCHECKBODY(b,"MdtBodySetOrientation");
#ifdef _MECHECK
    if( !MeMatrix3IsValidOrientationMatrix( R, ME_SMALL_EPSILON ) )
        MeWarning(12, "Orientation of body is invalid.");
#endif

    MeMatrix4TMSetRotation(b->bodyTM, R);

    UpdateCOMTransform(b);
}

/**
 * Sets the orientation of an object using a quaternion vector.
 * The quaternion is in the form (w, x, y, z).
 */
void MEAPI MdtBodySetQuaternion(const MdtBodyID b, const MeReal qw, const MeReal qx,
                          const MeReal qy, const MeReal qz)
{
    int j;
    MeReal k;
    MeVector4 bodyq;

    MdtCHECKBODY(b,"MdtBodySetQuaternion");

    bodyq[0] = qw;
    bodyq[1] = qx;
    bodyq[2] = qy;
    bodyq[3] = qz;

    /* If quaternion length is zero... */
    k = MeVector4MagnitudeSqr(bodyq);
    if(ME_IS_ZERO_TOL(k, ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12, "Invalid zero quaternion set for body.");
#endif
    }
    /* Else if quaternion is not unit length. */
    else if(!ME_IS_ZERO_TOL((k - 1), ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12, "Quaternion with non-unit magnitude detected.");
#endif
        k = MeRecipSqrt(k);
        for(j=0; j<4; j++) bodyq[j] *= k;
    }

    MeQuaternionToTM( b->bodyTM, bodyq);
    UpdateCOMTransform(b);
}

/**
 * Sets the linear velocity of a body.
 * Linear velocity is set in the world reference frame.
 */
void MEAPI MdtBodySetLinearVelocity(const MdtBodyID b, const MeReal dx,
                              const MeReal dy, const MeReal dz)
{
    MdtCHECKBODY(b,"MdtBodySetLinearVelocity");

    b->keaBody.vel[0] = dx;
    b->keaBody.vel[1] = dy;
    b->keaBody.vel[2] = dz;
}

/**
 * Sets the angular velocity of a body.
 * Angular velocity is set in the world reference frame.
 */
void MEAPI MdtBodySetAngularVelocity(const MdtBodyID b, const MeReal wx,
                               const MeReal wy, const MeReal wz)
{
    MdtCHECKBODY(b,"MdtBodySetAngularVelocity");

    b->keaBody.velrot[0] = wx;
    b->keaBody.velrot[1] = wy;
    b->keaBody.velrot[2] = wz;
}

/**
 * Sets the velocity damping of a body.
 * Damping applies a force (newtons/unit/sec) opposing the body's
 * velocity.  0 indicates no damping.
 * @warning A body can lose stability if the damping is too large.
 */
void MEAPI MdtBodySetLinearVelocityDamping(const MdtBodyID b, const MeReal d)
{
    MdtCHECKBODY(b,"MdtBodySetLinearVelocityDamping");
    b->damping = d;
}

/**
 * Sets the angular velocity damping of a body.
 * Angular velocity damping applies a torque (newton-meters/radian/sec)
 * opposing the body's angular velocity.  0 indicates no damping.
 * @warning A body can lose stability if the damping is too large.
 */
void MEAPI MdtBodySetAngularVelocityDamping(const MdtBodyID b, const MeReal d)
{
    MdtCHECKBODY(b,"MdtBodySetAngularVelocityDamping");
    b->angularDamping = d;
}

/**
 * Sets the fast spin axis of rotation of a body.
 * Fast spin is used for spinning objects like wheels.
 */
void MEAPI MdtBodySetFastSpinAxis(const MdtBodyID b, const MeReal x,
                            const MeReal y, const MeReal z)
{
    MdtCHECKBODY(b,"MdtBodySetFastSpinAxis");
    b->keaBody.flags |= MdtKeaBodyFlagUseFastSpin;
    b->keaBody.fastSpinAxis[0] = x;
    b->keaBody.fastSpinAxis[1] = y;
    b->keaBody.fastSpinAxis[2] = z;

    MeVector3Normalize(b->keaBody.fastSpinAxis);
}

/**
 * Disables fast spin for a body
 */
void MEAPI MdtBodySetNoFastSpinAxis(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodySetNoFastSpinAxis");
    b->keaBody.flags &= ~MdtKeaBodyFlagUseFastSpin;
}


/** Sets the mass of a body. */
void MEAPI MdtBodySetMass(const MdtBodyID b, const MeReal mass)
{
    MdtCHECKBODY(b,"MdtBodySetMass");

    b->mass = mass;

#ifdef _MECHECK
    if (mass <= 0)
        MeWarning(12, "Mass of body is zero or less.");

    if(mass < b->world->params.massScale / MDTBODY_MASS_RANGE_LIMIT)
    {
        MeWarning(12, "Mass of body is below lower range limit. Clamping");
        b->mass = b->world->params.massScale / MDTBODY_MASS_RANGE_LIMIT;
    }


    if(mass > b->world->params.massScale * MDTBODY_MASS_RANGE_LIMIT)
    {
        MeWarning(12, "Mass of body is beyond upper range limit. Clamping");
        b->mass = b->world->params.massScale * MDTBODY_MASS_RANGE_LIMIT;
    }

#endif

    /* Kea uses 1/mass */
    b->keaBody.invmass = MeRecip(b->mass);
}


/* Internal */
static void 
computeInverseInertia(const MeMatrix3 a, MeVector4 inv0, MeVector4 inv1, MeVector4 inv2)
{
    MeReal    r, a00, a10, a20, a11, a21, a22;

    /*
       First, get r which is the triple product of the columns of matrix
       A.
     */
    r = a[0][0] * (a[1][1] * a[2][2] - a[2][1] * a[2][1]) +
        a[1][0] * (a[2][1] * a[2][0] - a[1][0] * a[2][2]) +
        a[2][0] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);

    r = MeRecip(r);

    /*
       Now, we some components of 3 cross products and I can't seem to
       find a way to do that in place.
     */

    a00 = (a[1][1] * a[2][2] - a[2][1] * a[1][2]) * r;
    a10 = (a[2][1] * a[2][0] - a[1][0] * a[2][2]) * r;
    a20 = (a[1][0] * a[2][1] - a[1][1] * a[2][0]) * r;

    /*
       Note that b2[0] is not needed.
     */
    a11 = (a[2][2] * a[0][0] - a[2][0] * a[2][0]) * r;
    a21 = (a[2][0] * a[1][0] - a[2][1] * a[0][0]) * r;

    /*
       Note that b3[0] and b3[2] are not needed.
     */
    a22 = (a[0][0] * a[1][1] - a[1][0] * a[1][0]) * r;

    inv0[0] = a00;
    inv0[1] = a10;
    inv0[2] = a20;

    inv1[1] = a11;
    inv1[2] = a21;

    inv2[2] = a22;

    /*
       Symmetrize if needed.
     */
    inv1[0] = inv0[1];
    inv2[0] = inv0[2];
    inv2[1] = inv1[2];
}

/**
 * Sets the 3X3 inertia tensor of a body.
 * The inertia tensor must be diagonally symmetrical and greater than zero.
 * @warning A body can lose stability if the inertia tensor is too small.
 */
void MEAPI MdtBodySetInertiaTensor(const MdtBodyID b, const MeMatrix3 i)
{
    int j;
    MeMatrix3 clamped;
    MeReal iMag; 
    MdtCHECKBODY(b,"MdtBodySetInertiaTensor");

    iMag = b->world->params.massScale * b->world->params.lengthScale * b->world->params.lengthScale;

    MeMatrix3Copy(clamped,i);

#ifdef _MECHECK
    /* check that the matrix is symmetrical */
    if (clamped[1][0] != clamped[0][1] || 
        clamped[2][0] != clamped[0][2] || 
        clamped[2][1] != clamped[1][2])
         MeWarning(12, "Inertia tensor matrix is non-symmetric.");

    if(clamped[0][0] < iMag / MDTBODY_INERTIA_RANGE_LIMIT ||
       clamped[1][1] < iMag / MDTBODY_INERTIA_RANGE_LIMIT ||
       clamped[2][2] < iMag / MDTBODY_INERTIA_RANGE_LIMIT)
        MeWarning(12, "Inertia of body is below lower range limit. Clamping.");

    if(clamped[0][0] > iMag * MDTBODY_INERTIA_RANGE_LIMIT ||
       clamped[1][1] > iMag * MDTBODY_INERTIA_RANGE_LIMIT ||
       clamped[2][2] > iMag * MDTBODY_INERTIA_RANGE_LIMIT)
        MeWarning(12, "Inertia of body is beyond upper range limit. Clamping.");
#endif

	clamped[0][0] = MeCLAMP(clamped[0][0], iMag / MDTBODY_INERTIA_RANGE_LIMIT, iMag * MDTBODY_INERTIA_RANGE_LIMIT);
	clamped[1][1] = MeCLAMP(clamped[1][1], iMag / MDTBODY_INERTIA_RANGE_LIMIT, iMag * MDTBODY_INERTIA_RANGE_LIMIT);
	clamped[2][2] = MeCLAMP(clamped[2][2], iMag / MDTBODY_INERTIA_RANGE_LIMIT, iMag * MDTBODY_INERTIA_RANGE_LIMIT);

    for (j = 0; j < 3; j++)
    {
        b->keaBody.I0[j] = clamped[0][j];
        b->keaBody.I1[j] = clamped[1][j];
        b->keaBody.I2[j] = clamped[2][j];
    }

    /* invert the matrix and store in a better aligned data structure */
    computeInverseInertia(clamped, b->keaBody.invI0, b->keaBody.invI1,
        b->keaBody.invI2);
}

/** Set the moment of inertia to be isotropic (spherical). */
void MEAPI MdtBodySetSphericalInertiaTensor(const MdtBodyID b, const MeReal i)
{
    int j;
    MeMatrix3 moi;
    MeReal clamped = i;

    /* We can ignore the comToBodyTransform, because the inertia tensor
       is isotropic. */

    MdtCHECKBODY(b,"MdtBodySetSphericalInertiaTensor");

#ifdef _MECHECK
    if(clamped <= 0)
        MeWarning(12, "Inertia Tensor is zero or less.");

        if(clamped < b->world->params.massScale / MDTBODY_INERTIA_RANGE_LIMIT)
        {
            MeWarning(12, "Inertia of body is below lower range limit. Clamping.");
            clamped = b->world->params.massScale/MDTBODY_INERTIA_RANGE_LIMIT;
        }

        if(clamped > b->world->params.massScale * MDTBODY_INERTIA_RANGE_LIMIT)
        {
            MeWarning(12, "Inertia of body is beyond upper range limit. Clamping.");
            clamped = b->world->params.massScale*MDTBODY_INERTIA_RANGE_LIMIT;
        }
#endif

    MeMatrix3MakeIdentity(moi);
    MeMatrix3Scale(moi, clamped);

    for (j = 0; j < 3; j++)
    {
        b->keaBody.I0[j] = moi[0][j];
        b->keaBody.I1[j] = moi[1][j];
        b->keaBody.I2[j] = moi[2][j];
    }

    /* invert the matrix and store in a better aligned data structure */
    computeInverseInertia( *(const MeMatrix3*)&moi, b->keaBody.invI0, b->keaBody.invI1,
        b->keaBody.invI2);
}

/**
 * Set the position of the center-of-mass in the body reference frame.
 */
void MEAPI MdtBodySetCenterOfMassRelativePosition(
                     const MdtBodyID b, const MeVector3 pos)
{
    MdtCHECKBODY(b,"MdtBodySetCenterOfMassRelativePosition");

    if(!MeDictIsEmpty(&b->constraintDict))
    {
#ifdef _MECHECK
        MeWarning(0, "MdtBodySetCenterOfMassRelativePosition: "
            "You currently cannot change the position of the "
            "center of mass of a body with constraints.");
#endif
        return;          /*  RETURN -- DO NOTHING  */
    }
    MeVector3Copy(b->com, pos);
    b->useCom = (pos[0]!=0 || pos[1]!=0 || pos[2]!=0);

    UpdateCOMTransform(b);
    }

/**
 * Set the position of the center-of-mass in world ref frame.
 */
void MEAPI MdtBodySetCenterOfMassPosition(
                     const MdtBodyID b, const MeVector3 pos)
{
    MeVector3 tmp;
    MdtCHECKBODY(b,"MdtBodySetCenterOfMassPosition");
    MeMatrix4TMInverseTransform(tmp, b->bodyTM, pos);
    MdtBodySetCenterOfMassRelativePosition(b, tmp);
}

/**
 * Set the position of the center of mass in the body ref frame.
 * The orientation part of the input transformation is ignored
 *  because the comTM is always parallel to the bodyTM.
 * \deprecated
 *  This API will be removed in future versions of Karma toolkit.
 *  You should use MdtBodySetCenterOfMassRelativePosition instead.
    */
void MEAPI MdtBodySetCenterOfMassRelativeTransform(const MdtBodyID b,
               const MeMatrix4 t)
{
    MdtCHECKBODY(b,"MdtBodySetCenterOfMassRelativeTransform");

    MdtBodySetCenterOfMassRelativePosition(b, t[3]);
}



/**
 *  Returns bodies 'safe time' record.
 *  This is ignored by default, but can be used by the user.
 *  @see MdtPartitionGetSafeTime
 */
void MEAPI MdtBodySetSafeTime(const MdtBodyID b, const MeReal t)
{
    MdtCHECKBODY(b,"MdtBodySetSafeTime");

    b->safeTime = t;
}


/**
 * Set sort key for the dynamics body
 */
void MEAPI MdtBodySetSortKey(const MdtBodyID b, const MeI32 key)
{
    MdtCHECKBODY(b,"MdtBodyGetSafeTime");

    MeDictDelete(&b->world->bodyDict, &b->worldNode);
    if(b->flags&MdtEntityEnabledFlag)
    {
        MeDictDelete(&b->world->enabledBodyDict, &b->worldEnabledNode);
    }

    b->sortKey = key;

    MeDictInsert(&b->world->bodyDict, &b->worldNode, (void *)key);
    if(b->flags&MdtEntityEnabledFlag)
    {
        MeDictInsert(&b->world->enabledBodyDict, &b->worldEnabledNode, (void *)key);
    }
}


/* Other */

/**
 * Removes a body from simulation.
 *
 * A 'disabled' body will not be simulated. You can use the
 * MdtSetAutoDisable function to tell Mdt to automatically disable
 * bodies when they come to rest.
 *
 * @see MdtBodyEnable
 */
void MEAPI MdtBodyDisable(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyDisable");
    if (b->flags & MdtEntityEnabledFlag)         /* only if body is alive */
    {
        MeDictDelete(&b->world->enabledBodyDict, &b->worldEnabledNode);

        b->flags &= ~MdtEntityEnabledFlag;
        b->flags &= ~MdtEntityEnabledByPartitioner;
        b->enabledTime = 0;
        b->partitionIndex = -1; /* body is in no partition if disabled */
        b->world->nEnabledBodies--;

        if(b->world->bodyDisableCallback)
            (*b->world->bodyDisableCallback)(b);

    }
}

/**
 * Enables simulation of this body.
 *
 * By default, bodies are disabled from simulation. This function
 * enables simulation of that body. Bodies are automatically enabled
 * when hit by other enabled bodies, for example.
 *
 * @see MdtBodyDisable
 */
void MEAPI MdtBodyEnable(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyEnable");
    if(!(b->flags&MdtEntityEnabledFlag))        /* only if body is disabled */
    {
        MeDictInsert(&b->world->enabledBodyDict, &b->worldEnabledNode, (void *)b->sortKey);

        b->flags |= MdtEntityEnabledFlag;
        b->world->nEnabledBodies++;

        if(b->world->bodyEnableCallback)
            (*b->world->bodyEnableCallback)(b);

        /*  Reset the enabled time here, otherwise when we reset a 
            simulation and reenable all the bodies, the partitioner can 
            turn them off straight away! */
    }
    b->enabledTime = 0;
    b->flags &= ~MdtEntityEnabledByPartitioner;
}

/** Indicates whether a body has been removed from simulation,
 *  and is not being processed by the solver.
 *
 * 0 if disabled, 1 if enabled.
 */
MeBool MEAPI MdtBodyIsEnabled(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyIsEnabled");
    return b->flags & MdtEntityEnabledFlag ? 1 : 0;
}

/**
 * Reset forces and torques being applied to body to zero.
 *
 * @see MdtBodyAddForce
 * @see MdtBodyAddTorque
 */
void MEAPI MdtBodyResetForces(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyResetForces");

    b->keaBody.force[0] = 0;
    b->keaBody.force[1] = 0;
    b->keaBody.force[2] = 0;
    b->keaBody.force[3] = 0;

    b->keaBody.torque[0] = 0;
    b->keaBody.torque[1] = 0;
    b->keaBody.torque[2] = 0;
    b->keaBody.torque[3] = 0;
}

/**
 * Reset linear and angular impulses being applied to body to zero.
 *
 * @see MdtBodyAddForce
 * @see MdtBodyAddTorque
 */
void MEAPI MdtBodyResetImpulses(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyResetImpulses");

    b->impulseLinear[0] = 0;
    b->impulseLinear[1] = 0;
    b->impulseLinear[2] = 0;
    b->impulseLinear[3] = 0;

    b->impulseAngular[0] = 0;
    b->impulseAngular[1] = 0;
    b->impulseAngular[2] = 0;
    b->impulseAngular[3] = 0;
}


/**
 * Iterate over 'enabled' constraints that affect this body, calling the
 * supplied function on each of them.
 */
void MEAPI MdtBodyForAllConstraints(const MdtBodyID b,
                                    MdtConstraintIteratorCBPtr cb, void* ccbdata)
{
    MeDictNode *node, *nextNode;
    MeDict *dict = &b->constraintDict;

    MdtCHECKBODY(b,"MdtBodyForAllConstraints");

    node = MeDictFirst(dict); 
    while(node != 0)
    {
        MdtBaseConstraint *cp = (MdtBaseConstraint *)MeDictNodeGet(node);
        nextNode = MeDictNext(dict, node);        
        (*cb)(cp, ccbdata);
        node = nextNode;
    }
}

/**
 * Disables all the constraints affecting this body (but does not
 * change the body itself).
 * @see MdtBodyDestroyConstraints.
 */
void MEAPI MdtBodyDisableConstraints(const MdtBodyID b)
{
    MeDictNode *node;
    MeDict *dict = &b->constraintDict;

    MdtCHECKBODY(b,"MdtBodyDisableConstraints");

    for(node = MeDictFirst(dict); node != 0; node = MeDictNext(dict, node))
    {
        MdtConstraintDisable((MdtBaseConstraint *)MeDictNodeGet(node));
    }
}

/**
 * Destroys all the constraints affecting this body (but does not
 * change the body itself).
 * @see MdtBodyDisableConstraints.
 */
void MEAPI MdtBodyDestroyConstraints(const MdtBodyID b)
{
    MeDictNode *node, *nextNode;
    MeDict *dict = &b->constraintDict;

    MdtCHECKBODY(b,"MdtBodyDestroyConstraints");
    for(node = MeDictFirst(dict); node!=0; node = nextNode)
    {
        MdtBaseConstraint *cp = (MdtBaseConstraint *)MeDictNodeGet(node);
        nextNode = MeDictNext(dict, node);
        MdtConstraintDisable(cp);
        MdtConstraintDestroy(cp);        
    }
}

/**
 * Disables all the contacts affecting this body (but does not
 * change the body itself).
 * @see MdtBodyDestroyContacts.
 */
void MEAPI MdtBodyDisableContacts(const MdtBodyID b)
{
    MeDictNode *node;
    MeDict *dict = &b->constraintDict;

    MdtCHECKBODY(b,"MdtBodyDisableContacts");

    for(node = MeDictFirst(dict); node != 0; node = MeDictNext(dict, node))
    {
        MdtBaseConstraint *cp = (MdtBaseConstraint *)MeDictNodeGet(node);
		if ((cp->head.tag==MdtBclCONTACT)||(cp->head.tag==MdtBclCONTACTGROUP))
	        MdtConstraintDisable(cp);
    }
}

/**
 * Destroys all the contacts affecting this body (but does not
 * change the body itself).
 * @see MdtBodyDisableContacts.
 */
void MEAPI MdtBodyDestroyContacts(const MdtBodyID b)
{
    MeDictNode *node, *nextNode;
    MeDict *dict = &b->constraintDict;

    MdtCHECKBODY(b,"MdtBodyDestroyContacts");
    for(node = MeDictFirst(dict); node!=0; node = nextNode)
    {
        MdtBaseConstraint *cp = (MdtBaseConstraint *)MeDictNodeGet(node);
        nextNode = MeDictNext(dict, node);
		if ((cp->head.tag==MdtBclCONTACT)||(cp->head.tag==MdtBclCONTACTGROUP))
		{
			MdtConstraintDisable(cp);
			MdtConstraintDestroy(cp);
		}
    }
}

/**
 * Add a force vector to a body.
 *
 * The force is applied at the centre of mass and is set in the world
 * reference frame. Forces are accumulated on a body until the body is
 * next simulated, when they are used.
*/
void MEAPI MdtBodyAddForce(const MdtBodyID b, const MeReal fx,
                     const MeReal fy, const MeReal fz)
{
    MdtCHECKBODY(b,"MdtBodyAddForce");

    b->keaBody.force[0] += fx;
    b->keaBody.force[1] += fy;
    b->keaBody.force[2] += fz;
}

/**
 * Apply a force at a specific point on a body.
 *
 * @param b Body to apply force to.
 * @param f [xyz] Force to be applied in world reference frame.
 * @param p [xyz] Position to apply force to in world reference frame.
 */

 
void MEAPI MdtBodyAddForceAtPosition(const MdtBodyID b, const MeReal fx,
                               const MeReal fy, const MeReal fz,
                               const MeReal px, const MeReal py,
                               const MeReal pz)
{
    MeVector3 d, f, t;

    MdtCHECKBODY(b,"MdtBodyAddForceAtPosition");

    MeVector3Set(f, fx, fy, fz);
    MeVector3Set(d, px, py, pz);

    MdtBodyGetCenterOfMassPosition(b, t);
    MeVector3Subtract(d, d, t);

    b->keaBody.force[0] += f[0];
    b->keaBody.force[1] += f[1];
    b->keaBody.force[2] += f[2];

    MeVector3Cross(t, d, f);

    b->keaBody.torque[0] += t[0];
    b->keaBody.torque[1] += t[1];
    b->keaBody.torque[2] += t[2];
}


/**
 * Add a torque to a body.
 *
 * The torque vector is set in the world reference frame.
 */
void MEAPI MdtBodyAddTorque(const MdtBodyID b, const MeReal tx,
                      const MeReal ty, const MeReal tz)
{
    MdtCHECKBODY(b,"MdtBodyAddTorque");

    b->keaBody.torque[0] += tx;
    b->keaBody.torque[1] += ty;
    b->keaBody.torque[2] += tz;
}

/**
 * Add an instanteous impulse vector to a body.
 */
void MEAPI MdtBodyAddImpulse(const MdtBodyID b, const MeReal ix,
                       const MeReal iy, const MeReal iz)
{
    MdtCHECKBODY(b,"MdtBodyAddImpulse");

    b->impulseLinear[0] += ix;
    b->impulseLinear[1] += iy;
    b->impulseLinear[2] += iz;
    b->impulseAdded = 1;
}

/**
 * Apply an instantaneous impulse at a specific point on a body.
 *
 * @param b Body to apply force to.
 * @param i [xyz] Impulse to be applied in world reference frame.
 * @param p [xyz] Position to apply impulse to in world reference frame.
 */
void MEAPI MdtBodyAddImpulseAtPosition(const MdtBodyID b, const MeReal ix,
                                 const MeReal iy, const MeReal iz,
                                 const MeReal px, const MeReal py,
                                 const MeReal pz)
{
    MeVector3 d, i, t;

    MdtCHECKBODY(b,"MdtBodyAddImpulseAtPosition");

    MeVector3Set(i, ix, iy, iz);
    MeVector3Set(d, px, py, pz);

    MdtBodyGetCenterOfMassPosition(b, t);
    MeVector3Subtract(d, d, t);

    b->impulseLinear[0] += i[0];
    b->impulseLinear[1] += i[1];
    b->impulseLinear[2] += i[2];

    MeVector3Cross( t, d,  i);

    b->impulseAngular[0] += t[0];
    b->impulseAngular[1] += t[1];
    b->impulseAngular[2] += t[2];
    b->impulseAdded = 1;
}

/**
 * Perform a check to see if this body has 'come to rest' or not.
 * This check angular and linear acceleration and velocity, and sees if they
 * have fallen below the thresholds in the MdtPartitionParams. It also
 * checks that this body has been alive a minimum number of evolves.
 */
MeBool MEAPI MdtBodyIsMovingTest(const MdtBodyID b, const MdtPartitionParams* params)
{
    MeReal ss_vel, ss_velrot, ss_acc, ss_accrot;

    MdtCHECKBODY(b,"MdtBodyIsMovingTest");

    ss_vel = MeVector3MagnitudeSqr(b->keaBody.vel);
    ss_acc = MeVector3MagnitudeSqr(b->keaBody.accel);
    ss_velrot = MeVector3MagnitudeSqr(b->keaBody.velrot);
    ss_accrot = MeVector3MagnitudeSqr(b->keaBody.accelrot);

    /*  Check if its moving... 
        NB: things can be turned off straight away if turned on 
        automatically by partitioner */
    if ((ss_vel > params->vel_thresh)
        || (ss_velrot > params->velrot_thresh)
        || (ss_acc > params->acc_thresh)
        || (ss_accrot > params->accrot_thresh)
        || (b->enabledTime < params->alive_time_thresh && 
            !(b->flags & MdtEntityEnabledByPartitioner)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * Enable non-spherical inertia tensor,
 * in the mass matrix, Note that this does not enable coriolis forces
 * @param b Body to enable non-spherical inertia for
 */

void MEAPI MdtBodyEnableNonSphericalInertia(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyEnableNonSphericalInertia");
    b->keaBody.flags |= MdtKeaBodyFlagIsNonSpherical;
}
/**
 * Disable non-spherical inertia tensor in the mass matrix. Note
 * that this does not disable coriolis forces
 * @param b Body to disable non-spherical inertia for
 */
void MEAPI MdtBodyDisableNonSphericalInertia(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyDisableNonSphericalInertia");
    b->keaBody.flags &= ~MdtKeaBodyFlagIsNonSpherical;
}

/**
 * Test whether non-spherical inertia tensor is used in
 * mass matrix.
 * @param b Body to test
 */

MeBool MEAPI MdtBodyNonSphericalInertiaIsEnabled(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyNonSphericalInertiaIsEnabled");
    return (b->keaBody.flags & MdtKeaBodyFlagIsNonSpherical)?1:0;
}
/**
 * Enable addition of Coriolis Force to a body
 * Note that coriolis forces will always be zero unless non-spherical 
 * inertias are enabled
 * @param b Body to enable non-spherical inertia for
 */

void MEAPI MdtBodyEnableCoriolisForce(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyEnableNonSphericalInertia");
    b->keaBody.flags |= MdtKeaBodyFlagAddCoriolisForce;
}
/**
 * Disable addition of Coriolis force to a body.
 * @param b Body to disable coriolis force
 */
void MEAPI MdtBodyDisableCoriolisForce(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyDisableNonSphericalInertia");
    b->keaBody.flags &= ~MdtKeaBodyFlagAddCoriolisForce;
}

/**
 * Check whether Coriolis force is added for this body
 * @param b Body to test
 */

MeBool MEAPI MdtBodyCoriolisForceIsEnabled(const MdtBodyID b)
{
    MdtCHECKBODY(b,"MdtBodyNonSphericalInertiaIsEnabled");
    return (b->keaBody.flags & MdtKeaBodyFlagAddCoriolisForce)?1:0;
}

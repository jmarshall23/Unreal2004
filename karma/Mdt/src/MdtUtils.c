/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.25.2.1 $

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
#include <MdtLimit.h>
#include <MdtUtilities.h>

#define MePUT_FUNCTIONS_HERE_IF_NOT_INLINED 1
#include "MdtUtils.h"

int MdtDictCompare(const void *v1, const void *v2)
{
    int i1 = (int)v1;
    int i2 = (int)v2;

    if(i1<i2) return -1;
    if(i1>i2) return 1;
    return 0;
}

/**
 * Converts an orientation vector from one body ref. frame to another.
 * The translation of the body is ignored; only the rotation is converted.
 * A null body pointer indicates world space.
 */
void MEAPI MdtConvertVector(const MdtBodyID from_body, const MeVector3 f,
               const MdtBodyID to_body, MeVector3 t)
{
    MeVector3 tmp;

    MdtCHECKBODY_NULL_OK(from_body,"MdtConvertVector");
    MdtCHECKBODY_NULL_OK(to_body,"MdtConvertVector");

    /* need to do it this way to deal with the case when the output pointer is the
       same as the input. Otherwise we could avoid the extra copy.
    */

    if(from_body)
        MeMatrix4TMRotate(tmp, from_body->bodyTM, f);
    else
        MeVector3Copy(tmp, f);

    if(to_body)
        MeMatrix4TMInverseRotate(t, to_body->bodyTM, tmp);
    else
        MeVector3Copy(t, tmp);
}



/**
 * Converts a position from the body ref. frame of one body to another.
 * A null body pointer indicates world space.
 */
void MEAPI MdtConvertPositionVector(const MdtBodyID from_body, const MeVector3 f,
               const MdtBodyID to_body, MeVector3 t)
{
    MdtCHECKBODY_NULL_OK(from_body,"MdtConvertPositionVector");
    MdtCHECKBODY_NULL_OK(to_body,"MdtConvertPositionVector");

    MEASSERT(f!=t);
    if (from_body)
    {
        if (to_body)    /* convert body to body */
        {
            MeVector3 tmp;
            MeMatrix4TMTransform(tmp, from_body->bodyTM, f);
            MeMatrix4TMInverseTransform(t, to_body->bodyTM, tmp);        
        }
        else           /* convert body to world ref frame */
            MeMatrix4TMTransform(t, from_body->bodyTM, f);
    }
    else if (to_body)    /* convert world to body ref frame */
        MeMatrix4TMInverseTransform(t, to_body->bodyTM, f);        
    else            /* no conversion */
        memcpy(t, f, sizeof(MeVector3));
}

/**
 * Converts an orientation vector from one body ref. frame to another.
 * @see MdtConvertVector
 */
void MEAPI ConvertCOMVector(const MdtBodyID from_body, const MeVector3 f,
               const MdtBodyID to_body, MeVector3 t)
{
    MdtConvertVector(from_body, f, to_body, t);
}


/**
 * Converts a position from the com ref. frame of one body to another.
 * A null body pointer indicates world space.
 */
void MEAPI ConvertCOMPositionVector(const MdtBodyID from_body,
               const MeVector3 f, const MdtBodyID to_body, MeVector3 t)
{
    MeVector3 tmp;
    MdtCHECKBODY_NULL_OK(from_body,"ConvertCOMPositionVector");
    MdtCHECKBODY_NULL_OK(to_body,"ConvertCOMPositionVector");

    if (from_body)   /* add from-body COM vector */
    {
        MeVector3Add(tmp, f, from_body->com);
        f = tmp;
    }
    MdtConvertPositionVector(from_body, f, to_body, t);

    if (to_body)    /* subtract to-body COM vector */
        MeVector3Subtract(t, t, to_body->com);
}

/** Creates the inertia tensors of a sphere. */
void MEAPI MdtMakeInertiaTensorSphere(const MeReal mass,
               const MeReal radius, MeMatrix3 i)
{
    int m, n;
    MeReal k = (MeReal)(2.0) / (MeReal)(5.0) * mass * radius * radius;

    for (m = 0; m < 3; m++)
        for (n = 0; n < 3; n++)
            i[m][n] = 0;

    i[0][0] = k;
    i[1][1] = k;
    i[2][2] = k;
}

/** Creates the inertia tensor of a box given the full length of each side. */
void MEAPI MdtMakeInertiaTensorBox(const MeReal mass,
               const MeReal lx, const MeReal ly, const MeReal lz, MeMatrix3 i)
{
    int m, n;

    for (m = 0; m < 3; m++)
        for (n = 0; n < 3; n++)
            i[m][n] = 0;

    i[0][0] = mass / (MeReal)(12.0) * (ly * ly + lz * lz);
    i[1][1] = mass / (MeReal)(12.0) * (lx * lx + lz * lz);
    i[2][2] = mass / (MeReal)(12.0) * (lx * lx + ly * ly);
}

/**
 * Simple 'desired-position' controller for use with MdtLimits.
 * The limit powers the bodies towards the 'desiredPosition' at 'maxSpeed'
 * using 'maxForce' until it gets within +/- 'gap'. In this region the speed
 * is proportional to the error.
 */
void MEAPI MdtLimitController(const MdtLimitID limit,
               const MeReal desiredPosition, const MeReal gap,
               const MeReal maxSpeed, const MeReal maxForce)
{
    MeReal current = MdtLimitGetPosition(limit);
    MeReal error = current - desiredPosition;

    /* Make sure we are calculating the position of the limit. */
    if(!MdtLimitPositionIsCalculated(limit))
        MdtLimitCalculatePosition(limit, 1);

    if(error < -gap)
        MdtLimitSetLimitedForceMotor(limit, maxSpeed, maxForce);
    else if(error > gap)
        MdtLimitSetLimitedForceMotor(limit, -maxSpeed, maxForce);
    else /* we are in the proportional region */
    {
        MeReal vel = (maxSpeed / gap) * -error;
        MdtLimitSetLimitedForceMotor(limit, vel, maxForce);
    }
}

/** Update the 'body' transform from the 'center-of-mass' transform. */
void MEAPI UpdateBodyTransform(MdtBodyID b, const MeMatrix4 keaTM)
{
    memcpy(&b->comTM, keaTM, sizeof(MeMatrix4));
    memcpy(&b->bodyTM, keaTM, sizeof(MeMatrix4));

    if(b->useCom)        /* if the com vector is non-zero */
    {
        MeVector3 tmp;
        MeVector3MultiplyScalar(tmp, b->com, -1);
        MeMatrix4TMTransform(b->bodyTM[3], keaTM, tmp);
    }
}

/** Update the 'center-of-mass' transform from the 'body' transform. */
void MEAPI UpdateCOMTransform(MdtBodyID b)
{
    memcpy(&b->comTM, &b->bodyTM, sizeof(MeMatrix4));

    if(b->useCom)        /* if the com vector is non-zero */
        MeMatrix4TMTransform(b->comTM[3], b->bodyTM, b->com);

    /* Update quaternion as well. */
    MeQuaternionFromTM( b->keaBody.qrot, b->bodyTM);
}

/** Get a copy of the 'center-of-mass' transform. */
void MEAPI GetCOMTransform(MdtBodyID b, MeMatrix4 keaTM)
{
    memcpy(keaTM, &b->comTM, sizeof(MeMatrix4));
}


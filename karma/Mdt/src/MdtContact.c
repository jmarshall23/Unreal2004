/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.35.2.2 $

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
#include "MdtUtils.h"
#include <MdtConstraint.h>
#include <MdtContact.h>
#include <MdtCheckMacros.h>
#include <MdtContactParams.h>
#include <MdtContact.h>
#include <MeMath.h>
#include "MdtBody.h"

/**
 * Invalid ContactID for use with MdtContactGetNext
 */
const MdtContactID MdtContactInvalidID = 0;

/**
 * Creates a new contact and sets default parameters.
 *
 * @see MdtContactReset
 */
MdtContactID MEAPI MdtContactCreate(const MdtWorldID w)
{
    MdtContact* c;
    MdtCHECKWORLD(w,"MdtContactCreate");

    c = (MdtContact*)BaseConstraintCreate(w);

    if(c == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtContactCreate: Constraint Pool Too Small.");
#endif
        return c;
    }

    c->head.tag = MdtBclCONTACT;
    c->head.bclFunction = MdtBclAddContact;
    c->head.maxRows = MdtBclGETMAXROWSCONTACT;

    c->nextContact = MdtContactInvalidID;
    c->prevContact = MdtContactInvalidID;

    c->head.setBodyFunc = BaseConstraintSetBodies;
    c->head.setAxisFunc = BaseConstraintSetAxis;

    MdtContactReset(c);

    return c;
}

/**
 * Sets a contact to its default values.
 * Note that the contact's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg position = {0,0,0}
 * @arg normal = {0,1,0}
 * @arg friction direction = {0,1,0}
 * @arg penetration = 0
 *
 * @param c contact ID
 * @see MdtContactParamsReset
 */
void MEAPI MdtContactReset(const MdtContactID c)
{
    int i;
    MdtCHECKCONTACT(c,"MdtContactReset");

    if(MdtContactIsEnabled(c))
        MdtContactDisable(c);

    c->penetration = 0;

    for (i = 0; i < 3; i++)
    {
        c->cpos[i] = 0;
        c->normal[i] = 0;
        c->direction[i] = 0;
        c->worldVel[i] = 0;
    }

    /* default friction direction is {1,0,0} */
    c->direction[0] = 1;

    /* default normal is {0,1,0} */
    c->normal[1] = 1;

    c->nextContact = MdtContactInvalidID;
    c->prevContact = MdtContactInvalidID;
    c->contactGroup = 0;

    c->importance = MEINFINITY;

    /*
     * contact parameters get their own function so it can be called
     * separately by collision detection.
     */
    MdtContactParamsReset(&c->params);

    BaseConstraintReset(MdtContactQuaConstraint(c));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtContactQuaConstraint(const MdtContactID c)
{
    MdtCHECKCONTACT(c,"MdtContactQuaConstraint");
    return (MdtBaseConstraint *) c;
}

/**
 * Gets an MdtContactID from an MdtConstraintID.
 * If this constraint is not a Contact, returns 0.
 */
MdtContactID MEAPI MdtConstraintDCastContact(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastContact");
    if(c->head.tag == MdtBclCONTACT)
        return (MdtContact *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * Returns the contact position in @a v, in the world reference frame.
 */
void MEAPI MdtContactGetPosition(const MdtContactID c, MeVector3 v)
{
    MdtCHECKCONTACT(c,"MdtContactGetPosition");

    v[0] = c->cpos[0];
    v[1] = c->cpos[1];
    v[2] = c->cpos[2];
}

/**
 * Returns the contact normal in @a v, in the world reference frame.
 */
void MEAPI MdtContactGetNormal(const MdtContactID c, MeVector3 v)
{
    MdtCHECKCONTACT(c,"MdtContactGetNormal");

    v[0] = c->normal[0];
    v[1] = c->normal[1];
    v[2] = c->normal[2];
}

/**
 * Returns the current penetration at this contact.
 */
MeReal MEAPI MdtContactGetPenetration(const MdtContactID c)
{
    MdtCHECKCONTACT(c,"MdtContactGetPenetration");
    return c->penetration;
}

/**
 * The contacts primary direction is returned in @a v, in the world
 * reference frame.
 */
void MEAPI MdtContactGetDirection(const MdtContactID c, MeVector3 v)
{
    MdtCHECKCONTACT(c,"MdtContactGetDirection");

    v[0] = c->direction[0];
    v[1] = c->direction[1];
    v[2] = c->direction[2];
}

/**
 *  The velocity of the world currently being used at this contact is 
 *  returned in @v, in the world reference frame.
 */
void MEAPI MdtContactGetWorldVelocity(const MdtContactID c, MeVector3 v)
{
    MdtCHECKCONTACT(c, "MdtContactGetWorldVelocity");

    MeVector3Copy(v, c->worldVel);
}


/**
 * Returns a pointer to the contacts parameters of this contact
 */
MdtContactParamsID MEAPI MdtContactGetParams(const MdtContactID c)
{
    MdtCHECKCONTACT(c,"MdtContactGetParams");
    return &(c->params);
}

/**
 * Returns the pointer to the next contact associated with
 * this body pair if it was set in collision.
 * If the return value is equal to MdtContactInvalidID then
 * no next contact exists.
 */
MdtContactID MEAPI MdtContactGetNext(const MdtContactID c)
{
    MdtCHECKCONTACT(c,"MdtContactGetNext");
    return c->nextContact;
}

/** 
 *  Returns the MdtContactGroup that this MdtContact currently belongs to
 *  (could be 0, meaning not a member of a group).
 */
MdtContactGroupID MEAPI MdtContactGetContactGroup(const MdtContactID c)
{
    MdtCHECKCONTACT(c,"MdtContactGetContactGroup");
    return c->contactGroup;
}

/**
 *  Calculate the relative velocity between the two bodies at this contact.
 *  Returns velocity vector in world space.
 */
void MEAPI MdtContactGetRelativeVelocity(const MdtContactID c, MeVector3 v)
{
    MeVector3 pos0, pos1, vel0, vel1;
    MdtBody *b0, *b1;
    
    MdtCHECKCONTACT(c,"MdtContactGetRelativeVelocity");

    b0 = MdtContactGetBody(c, 0);
    b1 = MdtContactGetBody(c, 1);

    MdtBodyGetCenterOfMassPosition(b0, pos0);
    MeVector3Subtract(pos0, c->cpos, pos0);
    MdtBodyGetVelocityAtPoint(b0, c->cpos, vel0);

    if (b1!=0)
    {
        MdtBodyGetCenterOfMassPosition(b1, pos1);
        MeVector3Subtract(pos1, c->cpos,pos1);
        MdtBodyGetVelocityAtPoint(b1, c->cpos, vel1);
    }
    else
        vel1[0] = vel1[1] = vel1[2] = 0;
    
    MeVector3Subtract(v, vel0, vel1);
}


/*
 * Mutators
 */

/**
 * Sets the contact position.
 */
void MEAPI MdtContactSetPosition(const MdtContactID c, const MeReal x,
                           const MeReal y, const MeReal z)
{
    MdtCHECKCONTACT(c,"MdtContactSetPosition");

    c->cpos[0] = x;
    c->cpos[1] = y;
    c->cpos[2] = z;
}

/**
 * Sets the contact normal.
 */
void MEAPI MdtContactSetNormal(const MdtContactID c, const MeReal x,
                         const MeReal y, const MeReal z)
{
    MdtCHECKCONTACT(c,"MdtContactSetNormal");

#ifdef _MECHECK
    {
        MeReal normal_magnitude = MeSqrt(x * x + y * y + z * z);

        if( !ME_IS_ZERO_TOL( (normal_magnitude - 1), (MeReal)0.01 ) )
            MeWarning(12,
                "Contact normal vectors must have unit magnitude. "
                "Normal magnitude of %f detected.\n", normal_magnitude);
    }
#endif

    /* could also check that the normal points 'in' to body1 */

    c->normal[0] = x;
    c->normal[1] = y;
    c->normal[2] = z;
}

/**
 * Sets the amount of penetration at the contact.
 */
void MEAPI MdtContactSetPenetration(const MdtContactID c, const MeReal p)
{
    MdtCHECKCONTACT(c,"MdtContactSetPenetration");
    c->penetration = p;
}

/**
 * Utility for setting all contact parameters.
 *
 * Allows the user to set all values in the MdtBclContactParams
 * structure in one go.
 */
void MEAPI MdtContactSetParams(const MdtContactID c, const MdtContactParamsID p)
{
    MdtCHECKCONTACT(c,"MdtContactSetParams");
    c->params = *p;
}

/**
 * Sets the pointer to the next contact. Used by Mcd collision.
 */
void MEAPI MdtContactSetNext(const MdtContactID c, const MdtContactID nc)
{
    MdtCHECKCONTACT(c,"MdtContactSetNext");
    c->nextContact = nc;
}


/**
 * Sets the primary direction for this contact.
 *
 * This is only necessary if you wish to define surface properties to
 * vary depending on the direction. For isotropic (the same in all
 * direction) contacts, this function should not be called, and the
 * primary and secondary parameters should be set to the same value.
 *
 * The direction should always be perpendicular to the given normal, and
 * the secondary direction is perpendicular to the primary direction.
 */
void MEAPI MdtContactSetDirection(const MdtContactID c, const MeReal x,
                                  const MeReal y, const MeReal z)
{
    MdtCHECKCONTACT(c,"MdtContactSetDirection");

#ifdef _MECHECK
    {
        MeReal normal_magnitude = MeSqrt(x * x + y * y + z * z);

        if( !ME_IS_ZERO_TOL( (normal_magnitude - 1), ME_MEDIUM_EPSILON ) )
            MeWarning(12,
                "Contact direction vectors must have unit magnitude. "
                "Direction magnitude of %f detected.", normal_magnitude);
    }
#endif

    c->direction[0] = x;
    c->direction[1] = y;
    c->direction[2] = z;
    c->params.options =
        c->params.options | MdtBclContactOptionUseDirection;
}

/**
 *  Set the velocity of the world at this contact.
 *
 *  This will only be used for contacts between a body and the world (ie the 
 *  second body is NULL). It will automatically set 'slide' parameters for
 *  the primary and secondary directions by projecting the velocity into the
 *  plane of the contact normal. It will also set the desired velocity along 
 *  the contact normal.
 *
 *  @see MdtContactParamsSetPrimarySlide
 */
void MEAPI MdtContactSetWorldVelocity(const MdtContactID c, const MeReal x, 
                                      const MeReal y, const MeReal z)
{
    MdtCHECKCONTACT(c,"MdtContactSetWorldVelocity");

    c->worldVel[0] = x;
    c->worldVel[1] = y;
    c->worldVel[2] = z;

    if(MeVector3MagnitudeSqr(c->worldVel) > 
        ME_SMALL_EPSILON * ME_SMALL_EPSILON)
    {
        c->params.options = 
            c->params.options | MdtBclContactOptionUseWorldVelocity;
    }
    else
    {
        c->params.options = 
            c->params.options & ~MdtBclContactOptionUseWorldVelocity;
    }
}

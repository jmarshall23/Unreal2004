/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.13.4.1 $

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
#include <MdtCheckMacros.h>
#include <MdtContact.h>
#include <MdtContactGroup.h>
#include <MdtConstraint.h>


/**
 * Creates a new contact group and sets default parameters.
 *
 * @see MdtContactReset
 */
MdtContactGroupID MEAPI MdtContactGroupCreate(const MdtWorldID w)
{
    MdtContactGroup* c;
    MdtCHECKWORLD(w,"MdtContactGroupCreate");

    c = (MdtContactGroup*)BaseConstraintCreate(w);

    if(c == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtContactGroupCreate: Constraint Pool Too Small.");
#endif
        return c;
    }

    c->head.tag = MdtBclCONTACTGROUP;
    c->head.bclFunction = MdtBclAddContactGroup;
    c->head.maxRows = 0;
    c->head.setBodyFunc = BaseConstraintSetBodies;
    c->head.setAxisFunc = BaseConstraintSetAxis;

    c->first = MdtContactInvalidID;
    c->last = MdtContactInvalidID;

    MdtContactGroupReset(c);

    return c;
}

/**
 * Destroy a contact group and all the contacts it contains
 *
 * @see MdtContactCreaye
 */
void MEAPI MdtContactGroupDestroy(MdtContactGroupID g)
{
    MdtContactID contact,next;

    MdtCHECKCONTACTGROUP(g,"MdtContactGroupDestroy");

    for(contact = g->first; contact; contact = next)
    {   
        next = contact->nextContact;
        MdtContactDestroy(contact);
    }

    if(g->head.world->contactGroupDestroyCallback)
    {
        (*g->head.world->contactGroupDestroyCallback)(g);
    }

    MdtConstraintDestroy(MdtContactGroupQuaConstraint(g));
}


/**
 * Sets a contactgroup to its default values.
 * Note that this will nuke all the contacts in the group, and disable the contact set
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

void MEAPI MdtContactGroupReset(const MdtContactGroupID c)
{
    MdtContactID contact, next;
    MdtCHECKCONTACTGROUP(c,"MdtContactGroupReset");

    if(MdtContactGroupIsEnabled(c))
        MdtContactGroupDisable(c);

    for(contact = c->first; contact != MdtContactInvalidID; contact = next)
    {   
        if(MdtContactIsEnabled(contact))
            MdtContactDisable(contact);

        next = MdtContactGetNext(contact);

        MdtContactDestroy(contact);
    }

    BaseConstraintReset(MdtContactGroupQuaConstraint(c));
    
    c->first = MdtContactInvalidID;
    c->last = MdtContactInvalidID;
    c->count = 0;
    c->swapped = 0;
    c->normalForce = 0;
    c->generator = 0;
}



/** Get the generator of this contact group. */
void MEAPI MdtContactGroupSetGenerator(MdtContactGroupID c, void *generator)
{
    MdtCHECKCONTACTGROUP(c,"MdtContactGroupSetGenerator");
    c->generator = generator;
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtContactGroupQuaConstraint(const MdtContactGroupID c)
{
    MdtCHECKCONTACTGROUP(c,"MdtContactGroupQuaConstraint");
    return (MdtBaseConstraint *) c;
}

/**
 * Gets an MdtContactGroupID from an MdtConstraintID.
 * If this constraint is not a ContactGroup, returns 0.
 */
MdtContactGroupID MEAPI MdtConstraintDCastContactGroup(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastContact");
    if(c->head.tag == MdtBclCONTACTGROUP)
        return (MdtContactGroup *) c;
    else
        return 0;
}

/*
  Accessors
*/

/** Get the first MdtContact in this MdtContactGroup. */
MdtContactID MEAPI MdtContactGroupGetFirstContact(MdtContactGroupID c)
{
    MdtCHECKCONTACTGROUP(c,"MdtContactGroupGetFirstContact");
    return c->first;
}

/** Get the generator of this contact group. */
void *MEAPI MdtContactGroupGetGenerator(MdtContactGroupID c)
{
    MdtCHECKCONTACTGROUP(c,"MdtContactGroupGetGenerator");
    return c->generator;
}


/** 
 *  Returns whether the bodies of this group were swapped automatically 
 *  to make the second body the world.
 */
MeBool MEAPI MdtContactGroupIsSwapped(MdtContactGroupID g)
{
    MdtCHECKCONTACTGROUP(g,"MdtContactGroupIsSwapped");
    return g->swapped;
}

/** Return the number of contacts currently in this MdtContactGroup */
int MEAPI MdtContactGroupGetCount(MdtContactGroupID g)
{
    MdtCHECKCONTACTGROUP(g, "MdtContactGroupGetCount");
    return g->count;
}

/** Return the MdtContact following the given one in the MdtContactGroup. */
MdtContactID MEAPI MdtContactGroupGetNextContact(MdtContactGroupID g, 
                       MdtContactID c)
{
    MdtCHECKCONTACTGROUP(g,"MdtContactGroupGetNextContact");
    MdtCHECKCONTACT(c,"MdtContactGroupGetNextContact");
    return c->nextContact;
}

/** Returns the total magnitude of the force applied along the contact normal
    directions during the last timestep. */
MeReal MEAPI MdtContactGroupGetNormalForce(MdtContactGroupID g)
{
    MdtCHECKCONTACTGROUP(g,"MdtContactGroupGetNormalForce");
    return g->normalForce;
}


/** Add an MdtContact to this MdtContactGroup. */
void MEAPI MdtContactGroupAppendContact(MdtContactGroupID g, MdtContactID c)
{
    MdtCHECKCONTACTGROUP(g,"MdtContactGroupAppendContact");
    MdtCHECKCONTACT(c,"MdtContactGroupAppendContact");
    if(g->first==MdtContactInvalidID)
    {
        g->first = g->last = c;
        c->nextContact = c->prevContact = MdtContactInvalidID;
    }
    else
    {
        c->prevContact = g->last;
        g->last->nextContact = c;
        g->last = c;
    }

    c->contactGroup = g;
    g->count++;
}

/** Remove an MdtContact from this MdtContactGroup. */
void MEAPI MdtContactGroupRemoveContact(MdtContactGroupID g, MdtContactID c)
{
    MdtCHECKCONTACTGROUP(g,"MdtContactGroupRemoveContact");
    MdtCHECKCONTACT(c,"MdtContactGroupRemoveContact");

    if(g->first==c) 
        g->first = c->nextContact;
    else 
        c->prevContact->nextContact = c->nextContact;

    if(g->last==c)
        g->last = c->prevContact;
    else
        c->nextContact->prevContact = c->prevContact;

    c->nextContact = c->prevContact = MdtContactInvalidID;

    c->contactGroup = 0;
    g->count--;
}

/** Create a new MdtContact as part of this MdtContactGroup. */
MdtContactID MEAPI MdtContactGroupCreateContact(MdtContactGroupID g)
{
    MdtContactID c;

    MdtCHECKCONTACTGROUP(g,"MdtContactGroupCreateContact");

    c = MdtContactCreate(g->head.world);

    if(c!=MdtContactInvalidID)
    {
        MdtContactGroupAppendContact(g,c);
        MdtContactSetBodies(c,g->head.mdtbody[0],g->head.mdtbody[1]);
    }
    return c;
}

/** Destroy an MdtContact that is currently part of this MdtContactGroup. */
void MEAPI MdtContactGroupDestroyContact(MdtContactGroupID g, MdtContactID c)
{
    MdtCHECKCONTACTGROUP(g,"MdtContactGroupDestroyContact");
    MdtCHECKCONTACT(c,"MdtContactGroupDestroyContact");
    MdtContactGroupRemoveContact(g,c);
    MdtContactDestroy(c);
}



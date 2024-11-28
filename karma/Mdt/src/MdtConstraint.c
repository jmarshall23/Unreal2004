/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.64.2.5 $

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
#include <MdtTypes.h>
#include <MdtConstraint.h>
#include <MdtBody.h>
#include "MdtUtils.h"
#include <MdtCheckMacros.h>
#include <MeMemory.h>
#include "MdtContactGroup.h"

/* **************************************************************** */
/*
    First, functions that can be replaced on a per-constraint basis.
    Called internally.
*/

/**
 * Create base constraint and reset to defaults.
 * This is called by a specific constraint ..Create function.
 */
MdtConstraintID MEAPI BaseConstraintCreate(const MdtWorldID w)
{
    MdtConstraintID c = (MdtConstraintID)
        (*MePoolAPI.getStruct)(&w->constraintPool);

    if(!c)
    {
#ifdef _MECHECK
        MeWarning(0, "ConstraintCreate: Failed to allocate memory from "
            "pool for constraint.");
#endif
        return c;
    }

    c->head.mdtbody[0] = c->head.mdtbody[1] = 0;

    c->head.bclFunction = 0;
    c->head.maxRows = 0;

    c->head.world = w;
    c->head.flags = 0;
    c->head.sortKey = 0;

    return c;
}

/**
 * Reset a base constraint to defult values.
 * This is called by a specific constraint ..Reset function.
 */
void MEAPI BaseConstraintReset(const MdtConstraintID c)
{
    int i;

    c->head.flags = 0;
    c->head.userData = 0;
    c->head.bodyindex[0] = c->head.bodyindex[1] = MdtBclNO_BODY;

    for (i = 0; i < 4; i++)
    {
        c->head.resultForce[0][i] = 0;
        c->head.resultTorque[0][i] = 0;

        c->head.resultForce[1][i] = 0;
        c->head.resultTorque[1][i] = 0;
    }

    for(i=0; i<3; i++)
    {
        c->head.worldAngVel[i] = 0;
        c->head.worldLinVel[i] = 0;
    }

    MeMatrix4TMMakeIdentity(c->head.ref1);
    MeMatrix4TMMakeIdentity(c->head.ref2);

    MeDictNodeInit(&c->head.worldNode,(void *)c->head.sortKey);
    MeDictNodePut(&c->head.worldNode,c);

    for(i=0; i<MdtKeaMAXBODYCONSTRAINT; i++)
    {
        MeDictNodeInit(&c->head.bodyNode[i],(void *)c->head.sortKey);
        MeDictNodePut(&c->head.bodyNode[i],c);
    }
}

/**
 * 'Virtual, base class' function for setting a constraints bodies.
 */
void MEAPI BaseConstraintSetBodies(const MdtConstraintID c,
               const MdtBodyID b1, const MdtBodyID b2)
{
   MdtCHECKCONSTRAINT(c,"MdtConstraintSetBodies");

    if(MdtConstraintIsEnabled(c))
    {
#ifdef _MECHECK
        MeWarning(0, "A Constraint must be Disabled before changing its bodies.");
#endif
        return;
    }

#ifdef _MECHECK
    if (b1 == 0 && b2 == 0)
        MeFatalError(0, "A constraint's bodies cannot both be zero.");
    if (b1 == b2)
        MeFatalError(0, "A constraint's bodies cannot be the same body!");
    if (b1 && b2 && (MdtBodyGetWorld(b2) != MdtBodyGetWorld(b1)))
        MeFatalError(0, "Constrained bodies must be in the same world.");
#endif

    MdtCHECKBODY(b1,"MdtConstraintSetBodies");
    MdtCHECKBODY_NULL_OK(b2,"MdtConstraintSetBodies");

    c->head.mdtbody[0] = b1;
    c->head.mdtbody[1] = b2;
}

/**
 * 'Virtual, base class' set constraint axis function.
 */
void MEAPI BaseConstraintSetAxis(const MdtConstraintID c,
               const MeReal px, const MeReal py, const MeReal pz)
{
    MeVector3 axis, r_axis1, r_axis2;
    MeReal magSqr;

    MdtCHECKCONSTRAINT(c,"BaseConstraintSetAxis");

    axis[0] = px;
    axis[1] = py;
    axis[2] = pz;

    magSqr = MeVector3MagnitudeSqr(axis);

    /* Check axis is unit length. */
    if(!ME_IS_ZERO_TOL(magSqr - 1, ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12,
            "BaseConstraintSetAxis: Constraint axis should be unit length.");
#endif
        MeVector3Normalize(axis);
    }

    /* We use PlaneSpace to calculate rest of rotation matrix. */
    MeVector3PlaneSpace(axis, r_axis1, r_axis2);

    ConvertCOMVector(0, axis,    c->head.mdtbody[0], c->head.ref1[0]);
    ConvertCOMVector(0, r_axis1, c->head.mdtbody[0], c->head.ref1[1]);
    ConvertCOMVector(0, r_axis2, c->head.mdtbody[0], c->head.ref1[2]);

    ConvertCOMVector(0, axis,    c->head.mdtbody[1], c->head.ref2[0]);
    ConvertCOMVector(0, r_axis1, c->head.mdtbody[1], c->head.ref2[1]);
    ConvertCOMVector(0, r_axis2, c->head.mdtbody[1], c->head.ref2[2]);

#ifdef _MECHECK
    {
        MeMatrix3 R;

        MeMatrix4TMGetRotation( R, c->head.ref1);
        if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
            MeWarning(12, "BaseConstraintSetAxis: Constraint (body1) "
                "matrix is invalid.");

        MeMatrix4TMGetRotation( R, c->head.ref2);
        if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
            MeWarning(12, "BaseConstraintSetAxis: Constraint (body2) "
                "matrix is invalid.");
    }
#endif
}

/* **************************************************** */
/* Main Constraint function, common to all constraints. */

/** Destroys a constraint. Disables it automatically if necessary. */
void MEAPI MdtConstraintDestroy(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintDestroy");

    /* if constraint was enabled, disable it before destroying */
    if(MdtConstraintIsEnabled(c))
    {
        MdtConstraintDisable(c);
#ifdef _MECHECK
        MeWarning(12, "Destroying a constraint that is still enabled. "
            " Disabling automatically.");
#endif
    }

    /* make constraint is invalid by setting tag to NO_BODY. This
       ensures that it cannot be inadvertently destroyed or
       otherwise used again */

	if(c->head.tag==MdtBclCONTACTGROUP && c->head.world->contactGroupDestroyCallback)
    {
		(*c->head.world->contactGroupDestroyCallback)((MdtContactGroupID)c);
    }

    c->head.tag = MdtBclNO_BODY;

    (*MePoolAPI.putStruct)(&(c->head.world->constraintPool), c);
}

/** Enables simulation of a constraint. */
void MEAPI MdtConstraintEnable(const MdtConstraintID c)
{
    MdtWorldID w;
    int i;

    MdtCHECKCONSTRAINT(c,"MdtConstraintEnable");
    /* error if pointer is zero */

    w = c->head.world;

    MdtCHECKWORLD(w,"MdtConstraintEnable");

    if (MdtConstraintIsEnabled(c))
    {
#ifdef _MECHECK
        MeWarning(12,
            "You are enabling a constraint that is already enabled.");
#endif
        return;
    }
    if (c->head.mdtbody[0] == 0 && c->head.mdtbody[1] == 0)
    {
#ifdef _MECHECK
        MeWarning(12,"You cannot enable a constraint without its bodies set.");
#endif
        return;
    }
    c->head.flags |= MdtEntityEnabledFlag;

    MeDictInsert(&c->head.world->constraintDict,&c->head.worldNode,(void *)c->head.sortKey);
    w->nEnabledConstraints++;

    /*
       Add a reference to this constraint to each of its bodies
       constraint lists.
     */

    for(i=0; i<MdtKeaMAXBODYCONSTRAINT; i++)
    {
        if(c->head.mdtbody[i] != 0)
            MeDictInsert(&c->head.mdtbody[i]->constraintDict,&c->head.bodyNode[i],(void *)c->head.sortKey);
    }
}

/** Removes a constraint from simulation in a world. */
void MEAPI MdtConstraintDisable(const MdtConstraintID c)
{
    int i;

    MdtCHECKCONSTRAINT(c,"MdtConstraintDisable");

    if (!MdtConstraintIsEnabled(c))
    {
#ifdef _MECHECK
        ME_REPORT(MeWarning(12,
            "You are disabling a constraint that is already disabled."));
#endif
        return;
    }

    c->head.flags &= ~MdtEntityEnabledFlag;

    MeDictDelete(&c->head.world->constraintDict, &c->head.worldNode);
    c->head.world->nEnabledConstraints--;

    /* Remove references to this constraint from bodies constraint lists */

    for(i=0; i<MdtKeaMAXBODYCONSTRAINT; i++)
    {
        if(c->head.mdtbody[i] != 0)
        {
            MeDictDelete(&c->head.mdtbody[i]->constraintDict, &c->head.bodyNode[i]);
        }
    }
}

/**
 * Determines if this constraint is currently enabled.
 * Returns 1 if enabled, or 0 if not.
 */
MeBool MEAPI MdtConstraintIsEnabled(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintIsEnabled");
    return (c->head.flags & MdtEntityEnabledFlag) ? 1 : 0;
}

/*
 * Accessors
 */

/** Returns the world that the constraint is in. */
MdtWorldID MEAPI MdtConstraintGetWorld(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetWorld");
    return c->head.world;
}


/** Returns the number of rows (upper bound) that will be added by this 
    constraint. */
int MEAPI MdtConstraintGetRowCount(const MdtConstraintID c)
{
    int rows = 0;
    MdtContactGroupID group = MdtConstraintDCastContactGroup(c);
    
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetRowCount");
    
    
    /*  If constraint is a contact group, iterate over contacts adding
        rows given by contact type. */
    if(group)
    {
        MdtContact* contact;
        for(contact = group->first; contact; contact=contact->nextContact)
        {
            if(contact->params.type == MdtContactTypeFrictionZero)
                rows += 1;
            else if(contact->params.type == MdtContactTypeFriction1D)
                rows += 2;
            else
                rows += 3;
        }
    }
    else /* If not contact group, use max number of rows */
    {
        rows = c->head.maxRows;
    }

    return rows;
}

/**
 * Returns one of the bodies connected to this constraint.
 * 0 for the first body, 1 for the second body etc.
 */
MdtBodyID MEAPI MdtConstraintGetBody(const MdtConstraintID c, const unsigned int bodyindex)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetBody");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintGetBody");
    return c->head.mdtbody[bodyindex];
}

/** Returns the constraints UserData pointer. */
void *MEAPI MdtConstraintGetUserData(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetUserData");
    return c->head.userData;
}

/**
 * Returns the force applied to a body by this constraint (on the last
 * timestep).
 *
 * Forces are returned in the world reference frame.
 */
void MEAPI MdtConstraintGetForce(const MdtConstraintID c,
                                 const unsigned int bodyindex,
                                 MeVector3 v)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetForce");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintGetForce");

    v[0] = c->head.resultForce[bodyindex][0];
    v[1] = c->head.resultForce[bodyindex][1];
    v[2] = c->head.resultForce[bodyindex][2];
}

/**
 * Returns the torque applied to a body by this constraint (on the last
 * timestep).
 *
 * Torques are returned in the world reference frame.
 */
void MEAPI MdtConstraintGetTorque(const MdtConstraintID c,
                                  const unsigned int bodyindex,
                                  MeVector3 v)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetTorque");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintGetTorque");

    v[0] = c->head.resultTorque[bodyindex][0];
    v[1] = c->head.resultTorque[bodyindex][1];
    v[2] = c->head.resultTorque[bodyindex][2];
}

/** Get the constraint position in the world ref. frame.
 *  equivalent to MdtConstraintBodyGetPosition with bodyindex=1 */
void MEAPI MdtConstraintGetPosition(const MdtConstraintID c, MeVector3 p)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetPosition");
    ConvertCOMPositionVector(c->head.mdtbody[1], c->head.ref2[3], 0, p);
}

/** Get the position of the origin of the constraint frame
 *  for the given body in the world. ref. frame. */
void MEAPI MdtConstraintBodyGetPosition(const MdtConstraintID c,
                                        const unsigned int bodyindex, MeVector3 a)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintBodyGetPosition");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodyGetPosition");

    if(bodyindex == 0)
       ConvertCOMPositionVector(c->head.mdtbody[0], c->head.ref1[3], 0, a);
    else
       ConvertCOMPositionVector(c->head.mdtbody[1], c->head.ref2[3], 0, a);
}

/** 
 *  Get the position of the origin of the constraint frame
 *  for the given body relative to it's own reference frame.
 *  @see MdtConstraintBodySetPositionRel
 */
void MEAPI MdtConstraintBodyGetPositionRel(const MdtConstraintID c,
                                       const unsigned int bodyindex, MeVector3 a)
{
    MdtBodyID body;
    MeReal *cpos;

    MdtCHECKCONSTRAINT(c,"MdtConstraintBodyGetPositionRel");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodyGetPositionRel");

    /*  The position is stored as COM-to-constraint, not Body-to-constraint,
        so we need to add on the Body-to-COM offset to it. */

    body = c->head.mdtbody[bodyindex];
    cpos = (bodyindex == 0) ? c->head.ref1[3] : c->head.ref2[3];

    if(body && body->useCom)
        MeVector3Add(a, cpos, body->com);
    else
        MeVector3Copy(a, cpos);
}

/** Get the primary constraint axis in the world. ref. frame. */
void MEAPI MdtConstraintBodyGetAxis(const MdtConstraintID c, 
                                    const unsigned int bodyindex, MeVector3 a)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintBodyGetAxis");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodyGetAxis");

    if(bodyindex == 0)
        ConvertCOMVector(c->head.mdtbody[0], c->head.ref1[0], 0, a);
    else
        ConvertCOMVector(c->head.mdtbody[1], c->head.ref2[0], 0, a);
}

/** Get both the primary constraint axis and one orthogonal constraint axis
 *  in the world. ref. frame for the given body. */
void MEAPI MdtConstraintBodyGetAxes(const MdtConstraintID c,
                                        const unsigned int bodyindex,
                                        MeVector3 p,  MeVector3 o )
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintBodyGetAxes");
    if(bodyindex == 0)
    {
        ConvertCOMVector(c->head.mdtbody[0], c->head.ref1[0], 0, p);
        ConvertCOMVector(c->head.mdtbody[0], c->head.ref1[1], 0, o);
    }
    else
    {
        ConvertCOMVector(c->head.mdtbody[1], c->head.ref2[0], 0, p);
        ConvertCOMVector(c->head.mdtbody[1], c->head.ref2[1], 0, o);
    }
}

/** Get both the primary constraint axis and one orthogonal constraint axis
 *  in the reference frame of the given body. */
void MEAPI MdtConstraintBodyGetAxesRel(const MdtConstraintID c,
                                        const unsigned int bodyindex,
                                        MeVector3 p,  MeVector3 o )
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintBodyGetAxesRel");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodyGetAxesRel");

    if(bodyindex == 0)
    {
        p[0] = c->head.ref1[0][0];
        p[1] = c->head.ref1[0][1];
        p[2] = c->head.ref1[0][2];

        o[0] = c->head.ref1[1][0];
        o[1] = c->head.ref1[1][1];
        o[2] = c->head.ref1[1][2];
    }
    else
    {
        p[0] = c->head.ref2[0][0];
        p[1] = c->head.ref2[0][1];
        p[2] = c->head.ref2[0][2];

        o[0] = c->head.ref2[1][0];
        o[1] = c->head.ref2[1][1];
        o[2] = c->head.ref2[1][2];
    }
}

/** Get both the primary constraint axis and one orthogonal constraint axis
 *  in the world. ref. frame.*/
void MEAPI MdtConstraintGetAxes(const MdtConstraintID c,  MeVector3 p,  MeVector3 o )
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetAxes");
    MdtConstraintBodyGetAxes( c, 1, p, o );
}

/* Iterators */

/** Get first body in linked list of constraints in the MdtWorld. */
MdtConstraintID MEAPI MdtConstraintGetFirst(const MdtWorldID w)
{
    MeDictNode *node;
    MdtCHECKWORLD(w,"MdtConstraintGetFirst");
    node = MeDictFirst(&w->constraintDict);
    return node ? (MdtConstraintID) MeDictNodeGet(node) : 0;
}

/** Get next body in linked list of constraints in the MdtWorld. */
MdtConstraintID MEAPI MdtConstraintGetNext(const MdtConstraintID c)
{
    MeDictNode *node;
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetNext");

    node = MeDictNext(&c->head.world->constraintDict, &c->head.worldNode);
    return node ? (MdtConstraintID) MeDictNodeGet(node) : 0;
}


/** Get sort key for constraint */
MeI32 MEAPI MdtConstraintGetSortKey(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetNext");
    return c->head.sortKey;

}


/** 
 *  Get the currently set linear velocity of world at joint 
 *  attachment position. 
 */
void MEAPI MdtConstraintGetWorldLinearVelocity(const MdtConstraintID c,
               MeVector3 lv)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetWorldLinearVelocity");

    lv[0] = c->head.worldLinVel[0];
    lv[1] = c->head.worldLinVel[1];
    lv[2] = c->head.worldLinVel[2];
}

/** 
 *  Get the currently set angular velocity of world at joint 
 *  attachment position. 
 */
void MEAPI MdtConstraintGetWorldAngularVelocity(const MdtConstraintID c,
               MeVector3 av)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetWorldAngularVelocity");
    
    av[0] = c->head.worldAngVel[0];
    av[1] = c->head.worldAngVel[1];
    av[2] = c->head.worldAngVel[2];
}

/*
 * Mutators
 */

/**
 * Sets the bodies to be constrained.
 * A body must be disabled before its bodies can be changed.
 * An Angular3 (once enabled) will maintain the bodies orientation as they
 * are when this function is called, until a call to MdtAngular3SetAxis.
 */
void MEAPI MdtConstraintSetBodies(const MdtConstraintID c,
                                  const MdtBodyID b1, const MdtBodyID b2)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetBodies");
    
    (*c->head.setBodyFunc)(c, b1, b2);
}

/** Sets the constraint's UserData pointer. */
void MEAPI MdtConstraintSetUserData(const MdtConstraintID c, void *d)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetUserData");
    c->head.userData = d;
}


/** Set the constraint position (for just one body) in the world ref. frame. */
void MEAPI MdtConstraintBodySetPosition(const MdtConstraintID c,
               const unsigned int bodyindex,
               const MeReal x, const MeReal y, const MeReal z)
{
    MeVector3 pos;

    MdtCHECKCONSTRAINT(c,"MdtConstraintBodySetPosition");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodySetPosition");

    MeVector3Set(pos, x, y, z);

    if(bodyindex == 0)
        ConvertCOMPositionVector(0, pos, c->head.mdtbody[0], c->head.ref1[3]);
    else
        ConvertCOMPositionVector(0, pos, c->head.mdtbody[1], c->head.ref2[3]);
}


/** Set the position of the constraint reference frame for each body
    in that MdtBody's reference frame. This will convert the position from
    relative to the MdtBody to relative to the COM and store it. */
void MEAPI MdtConstraintBodySetPositionRel(const MdtConstraintID c,
               const unsigned int bodyindex,
               const MeReal x, const MeReal y, const MeReal z)
{
    MdtBodyID body;
    MeVector3Ptr cpos;

    MdtCHECKCONSTRAINT(c,"MdtConstraintBodySetPositionRel");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(c,"MdtConstraintSetPositionRel");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodySetPositionRel");

    /*  The position is stored as COM-to-constraint, not Body-to-constraint,
        so we need to subtract the Body-to-COM offset from it. */

    body = c->head.mdtbody[bodyindex];
    cpos = (bodyindex == 0) ? c->head.ref1[3] : c->head.ref2[3];

    MeVector3Set(cpos, x, y, z);

    if(body && body->useCom)
        MeVector3Subtract(cpos, cpos, body->com);
}


/** Set the constraint position in the world ref. frame. */
void MEAPI MdtConstraintSetPosition(const MdtConstraintID c,
               const MeReal x, const MeReal y, const MeReal z)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetPosition");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(c,"MdtConstraintSetPosition");

    MdtConstraintBodySetPosition(c, 0, x, y, z);
    MdtConstraintBodySetPosition(c, 1, x, y, z);
}

/**
 * Set the constraint primary axis of this constraint in the
 * world ref. frame.
 */
void MEAPI MdtConstraintSetAxis(const MdtConstraintID c,
               const MeReal px, const MeReal py, const MeReal pz)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetAxis");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(c,"MdtConstraintSetAxis");
    
    (*c->head.setAxisFunc)(c, px, py, pz); 
}

/**
 * Set the primary and orthogonal axis of this constaint (for just one body).
 *
 * @param bodyindex is '0' for primary body or '1' for secondary.
 * @p primary axis in the world ref. frame.
 * @o orthogonal axis in the world ref. frame.
 */
void MEAPI MdtConstraintBodySetAxes(const MdtConstraintID c,
               const unsigned int bodyindex,
               const MeReal px, const MeReal py, const MeReal pz,
               const MeReal ox, const MeReal oy, const MeReal oz)
{
    MeVector3 paxis, oaxis, r_axis;
    MeReal magSqr;

    MdtCHECKCONSTRAINT(c,"MdtConstraintBodySetAxes");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(c,"MdtConstraintBodySetAxes");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodySetAxes");

    paxis[0] = px;
    paxis[1] = py;
    paxis[2] = pz;

    oaxis[0] = ox;
    oaxis[1] = oy;
    oaxis[2] = oz;

    magSqr = MeVector3MagnitudeSqr(paxis);

    /* Check both axis are unit length. */
    if(!ME_IS_ZERO_TOL(magSqr - 1, ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12, "MdtConstraintBodySetAxes: Primary constraint axis "
            "must be unit length. Normalizing..");
#endif
        MeVector3Normalize(paxis);
    }

    magSqr = MeVector3MagnitudeSqr(oaxis);

    if(!ME_IS_ZERO_TOL(magSqr - 1, ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12, "MdtConstraintBodySetAxes: Secondary constraint axis "
            "must be unit length. Normalizing..");
#endif
        MeVector3Normalize(oaxis);
    }

    /* Check axis are perpendicular. */
#ifdef _MECHECK
    if(!ME_IS_ZERO_TOL(MeVector3Dot(paxis, oaxis), ME_MEDIUM_EPSILON))
    {
        MeWarning(12, "MdtConstraintBodySetAxes: Primary and Secondary axis "
            "are not perpendicular.");
    }
#endif

    /* Use cross product to calculate final vector of rotation matrix. */
    MeVector3Cross( r_axis, paxis,  oaxis);

    if(bodyindex == 0)
    {
        ConvertCOMVector(0, paxis,  c->head.mdtbody[0], c->head.ref1[0]);
        ConvertCOMVector(0, oaxis,  c->head.mdtbody[0], c->head.ref1[1]);
        ConvertCOMVector(0, r_axis, c->head.mdtbody[0], c->head.ref1[2]);
    }
    else
    {
        ConvertCOMVector(0, paxis,  c->head.mdtbody[1], c->head.ref2[0]);
        ConvertCOMVector(0, oaxis,  c->head.mdtbody[1], c->head.ref2[1]);
        ConvertCOMVector(0, r_axis, c->head.mdtbody[1], c->head.ref2[2]);
    }

#ifdef _MECHECK
    {
        MeMatrix3 R;

        if(bodyindex == 0)
        {
            MeMatrix4TMGetRotation( R, c->head.ref1);
            if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
                MeWarning(12, "MdtConstraintBodySetAxes: Constraint (body1) "
                "matrix is invalid.");
        }
        else
        {
            MeMatrix4TMGetRotation( R, c->head.ref2);
            if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
                MeWarning(12, "MdtConstraintBodySetAxes: Constraint (body2) "
                "matrix is invalid.");
        }
    }
#endif
}

/**
 * Set the primary and orthogonal axis of this constaint relative to
 * the selected body.
 *
 * @param bodyindex is '0' for primary body or '1' for secondary.
 * @p primary axis in the world ref. frame.
 * @o orthogonal axis in the world ref. frame.
 */
void MEAPI MdtConstraintBodySetAxesRel(const MdtConstraintID c,
               const unsigned int bodyindex,
               const MeReal px, const MeReal py, const MeReal pz,
               const MeReal ox, const MeReal oy, const MeReal oz)
{
    MeVector3 paxis, oaxis, r_axis;
    MeReal magSqr;

    MdtCHECKCONSTRAINT(c,"MdtConstraintBodySetAxesRel");
//    MdtCHECKCONSTRAINTBODIES_NULL_OK(c,"MdtConstraintSetAxesRel");
    MdtCHECKBODYINDEX(bodyindex,"MdtConstraintBodySetAxesRel");

    paxis[0] = px;
    paxis[1] = py;
    paxis[2] = pz;

    oaxis[0] = ox;
    oaxis[1] = oy;
    oaxis[2] = oz;

    magSqr = MeVector3MagnitudeSqr(paxis);

    /* Check both axis are unit length. */
    if(!ME_IS_ZERO_TOL(magSqr - 1, ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12, "MdtConstraintBodySetAxesRel: Primary constraint axis "
            "must be unit length. Normalizing..");
#endif
        MeVector3Normalize(paxis);
    }

    magSqr = MeVector3MagnitudeSqr(oaxis);

    if(!ME_IS_ZERO_TOL(magSqr - 1, ME_MEDIUM_EPSILON))
    {
#ifdef _MECHECK
        MeWarning(12, "MdtConstraintBodySetAxesRel: Secondary constraint axis "
            "must be unit length. Normalizing..");
#endif
        MeVector3Normalize(oaxis);
    }

    /* Check axis are perpendicular. */
#ifdef _MECHECK
    if(!ME_IS_ZERO_TOL(MeVector3Dot(paxis, oaxis), ME_MEDIUM_EPSILON))
    {
        MeWarning(12, "MdtConstraintBodySetAxesRel: Primary and Secondary axis "
            "are not perpendicular.");
    }
#endif

    /* Use cross product to calculate final vector of rotation matrix. */
    MeVector3Cross( r_axis, paxis,  oaxis);

    if(bodyindex == 0)
    {
        c->head.ref1[0][0] = paxis[0];
        c->head.ref1[0][1] = paxis[1];
        c->head.ref1[0][2] = paxis[2];

        c->head.ref1[1][0] = oaxis[0];
        c->head.ref1[1][1] = oaxis[1];
        c->head.ref1[1][2] = oaxis[2];

        c->head.ref1[2][0] = r_axis[0];
        c->head.ref1[2][1] = r_axis[1];
        c->head.ref1[2][2] = r_axis[2];
    }
    else
    {
        c->head.ref2[0][0] = paxis[0];
        c->head.ref2[0][1] = paxis[1];
        c->head.ref2[0][2] = paxis[2];

        c->head.ref2[1][0] = oaxis[0];
        c->head.ref2[1][1] = oaxis[1];
        c->head.ref2[1][2] = oaxis[2];

        c->head.ref2[2][0] = r_axis[0];
        c->head.ref2[2][1] = r_axis[1];
        c->head.ref2[2][2] = r_axis[2];
    }

#ifdef _MECHECK
    {
        MeMatrix3 R;

        if(bodyindex == 0)
        {
            MeMatrix4TMGetRotation( R, c->head.ref1);
            if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
                MeWarning(12, "MdtConstraintBodySetAxesRel: Constraint (body1) "
                "matrix is invalid.");
        }
        else
        {
            MeMatrix4TMGetRotation( R, c->head.ref2);
            if(!MeMatrix3IsValidOrientationMatrix(R,ME_MEDIUM_EPSILON))
                MeWarning(12, "MdtConstraintBodySetAxesRel: Constraint (body2) "
                "matrix is invalid.");
        }
    }
#endif
}


/**
 * Set the primary and secondary constraint axis in the world ref. frame.
 */
void MEAPI MdtConstraintSetAxes(const MdtConstraintID c,
               const MeReal px, const MeReal py, const MeReal pz,
               const MeReal ox, const MeReal oy, const MeReal oz)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetAxes");

    MdtConstraintBodySetAxes(c, 0, px, py, pz, ox, oy, oz);
    MdtConstraintBodySetAxes(c, 1, px, py, pz, ox, oy, oz);
}


/** Set sort key for constraint */
void MEAPI MdtConstraintSetSortKey(const MdtConstraintID c, MeI32 key)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintGetNext");
    if(MdtConstraintIsEnabled(c))
    {
#ifdef _MECHECK
        MeWarning(0, "A Constraint must be Disabled before changing its bodies.");
#endif
        return;
    }
    c->head.sortKey = key;
}

/** Set linear velocity of world at joint attachment position. */
void MEAPI MdtConstraintSetWorldLinearVelocity(const MdtConstraintID c,
               const MeReal vx, const MeReal vy, const MeReal vz)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetWorldLinearVelocity");

    c->head.worldLinVel[0] = vx;
    c->head.worldLinVel[1] = vy;
    c->head.worldLinVel[2] = vz;
}

/** Set angular velocity of world at joint attachment position. */
void MEAPI MdtConstraintSetWorldAngularVelocity(const MdtConstraintID c,
               const MeReal ax, const MeReal ay, const MeReal az)
{
    MdtCHECKCONSTRAINT(c,"MdtConstraintSetWorldAngularVelocity");
    
    c->head.worldAngVel[0] = ax;
    c->head.worldAngVel[1] = ay;
    c->head.worldAngVel[2] = az;
}
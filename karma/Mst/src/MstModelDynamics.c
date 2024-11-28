/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:41 $ - Revision: $Revision: 1.21.2.2 $

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

#include <Mst.h>
#include <MeMath.h>
#include <McdModel.h>

/* *************************************************************** */
/* MUTATORS                                                        */
/* *************************************************************** */


typedef struct {
    MdtBodyID oldBody;
    MdtBodyID newBody;
    McdModelID model;
    McdSpaceID space;
} BodyData;

static void MEAPI transferContactGroups(const MdtConstraintID c, void* ccbdata)
{
    MdtContactGroupID group = MdtConstraintDCastContactGroup(c);
    BodyData *data = (BodyData *)ccbdata;
    McdModelPairID pair;
    int deleteGroup = 0;

    if(!group)
        return;

    pair = (McdModelPairID)MdtContactGroupGetGenerator(group);

    if(!pair || (pair->model1 != data->model && pair->model2 != data->model))
        return;

    if(!data->newBody || !data->space)
        deleteGroup = 1;
    else
    {
        McdModelID otherModel = pair->model1==data->model ? pair->model2 : pair->model1;

        if(McdModelGetSpace(otherModel)!=data->space)
            deleteGroup=1;
        else
        {
            MdtBodyID otherBody = McdModelGetBody(otherModel);

            if(otherBody==data->newBody)
            {
                if(McdSpacePairIsEnabled(data->model,otherModel))
                {
                    MeFatalError(1,"McdModelSetBody: Attempt to assign model A to a body\n"
                        "which has an associated model B which is not pairwise disabled with A");
                }
                else
                    deleteGroup = 1;
            }
        }
    }

    if(deleteGroup)
    {
        MdtContactGroupDisable(group);
        MdtContactGroupDestroy(group);
    }
    else
    {
        MdtBodyID body0 = MdtContactGroupGetBody(group,0);
        MdtBodyID body1 = MdtContactGroupGetBody(group,1);


        MdtContactGroupDisable(group);
        if(MdtContactGroupGetBody(group,0)==data->oldBody)
            MdtContactGroupSetBodies(group,data->newBody,body1);
        else
            MdtContactGroupSetBodies(group,body0,data->newBody);
        MdtContactGroupEnable(group);
    }
}


/**
 * Utility function for assigning a dynamics body to a collision model.
 * This is used so that contacts created during MstBridgeUpdateContact are
 * attached to the correct dynamics body.
 *
 * If you are moving a model from one body to another, any contact groups
 * corresponding to the original model will be transferred to the new body.
 * It is possible for this function to generate a FATAL ERROR, in the case
 * where the model is assigned to a body which has other associated collision
 * models, and the new model and one or more of the other models overlap
 * in the far field.
 *
 * If you are applying a relative transform to the model it is important to
 * remember to call this function BEFORE McdModelSetRelativeTransformPtrs().
 *
 * It is legitimate to set more than one model per body, but the MdtBodyGetModel()
 * function will only return the most recently attatched.
 */

void MEAPI McdModelSetBody(const McdModelID model, const MdtBodyID body)
{

    MdtBodyID oldBody = model->mBody;

    if(body==oldBody && (!body || body->model==model))
        return;

    if(oldBody)
    {	/* transfer all constacts from old body to new body */
        BodyData bp;
        bp.oldBody = oldBody;
        bp.newBody = body;
        bp.model = model;
        bp.space = McdModelGetSpace(model);
        MdtBodyForAllConstraints(oldBody,transferContactGroups,&bp);

        /* make sure that if the old body e.g. gets disabled it doesn't
           take this model with it */
        if(oldBody->model == model)
            oldBody->model = 0;
    }

    if(body)
    {
        /* Tie them together... */
        body->model = model;

        McdModelSetTransformPtr(model, MdtBodyGetTransformPtr(body));
        McdModelSetLinearVelocityPtr(model, MdtBodyGetLinearVelocityPtr(body));
        McdModelSetAngularVelocityPtr(model, MdtBodyGetAngularVelocityPtr(body));

        if(McdModelGetSpace(model))
        {
            if(MdtBodyIsEnabled(body))
                McdSpaceUnfreezeModel(model);
            else
                McdSpaceFreezeModel(model);
        }

    }
    else
    {
        /* Check if this model was pointing to any of the bodies props. */
        if(oldBody)
        {
            if(McdModelGetTransformPtr(model) == MdtBodyGetTransformPtr(oldBody))
                McdModelSetTransformPtr(model, 0);

            if(McdModelGetLinearVelocityPtr(model) == MdtBodyGetLinearVelocityPtr(oldBody))
                McdModelSetLinearVelocityPtr(model, 0);

            if(McdModelGetAngularVelocityPtr(model) == MdtBodyGetAngularVelocityPtr(oldBody))
                McdModelSetAngularVelocityPtr(model, 0);

        }

        /* this model has no body, so freeze it */
        if(McdModelGetSpace(model))
            McdSpaceFreezeModel(model);
    }

    model->mBody = body;
}

/**
 * Reset models dynamics body (if present) position to origin,
 * orientation to default and zeroes velocity..
 */
void MEAPI McdModelDynamicsReset(const McdModelID m)
{
    if(m->mBody)
    {
        MdtBodySetPosition((MdtBodyID)m->mBody, 0, 0, 0);
        MdtBodySetQuaternion((MdtBodyID)m->mBody, 1, 0, 0, 0);
        MdtBodySetLinearVelocity((MdtBodyID)m->mBody, 0, 0, 0);
        MdtBodySetAngularVelocity((MdtBodyID)m->mBody, 0, 0, 0);
    }
    else
        MeWarning(0, "Resetting dynamics on McdModel with no dynamics.");
}


/* *************************************************************** */
/* ACCESSORS                                                       */
/* *************************************************************** */


/** Return the dynamics body associated with this McdModel (if present). */
MdtBodyID MEAPI McdModelGetBody(const McdModelID m)
{
    return (MdtBodyID)m->mBody;
}





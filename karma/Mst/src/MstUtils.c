/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/04/08 11:31:47 $ $Revision: 1.67.2.5 $

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

/* Do this in case build environment cannot handle inline functions.. */

#include <McdProfile.h>

#include <MstUtils.h>
#include <MeProfile.h>
#include <MeMath.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <McdBatch.h>

static MdtContactGroupID createContactGroup(MdtWorldID w, McdModelPairID pair)
{
    MeI32 key1 = McdModelGetSortKey(pair->model1);
    MeI32 key2 = McdModelGetSortKey(pair->model2);
    MdtBodyID body1 = McdModelGetBody(pair->model1);
    MdtBodyID body2 = McdModelGetBody(pair->model2);
    
    MdtContactGroupID g = 0;

    if(body1 || body2)
    {
        if(body1==body2)
        {
#ifdef _MECHECK
            MeFatalError(0, "Attempt to create contact group between two collision models with "
            "the same dynamics body. You should disable collisions between such models in the"
            "McdSpace.");
            return 0;
#endif
        }

        g = MdtContactGroupCreate(w);
        if (g == 0)
        {
#ifdef _MECHECK
            MeFatalError(0, "Constraint pool too small during Mcd->Mdt ContactGroup creation.");
#endif
            return 0;
        }
        
        /* If one of the contacts is with the world, make world the second body. */
        if(body1) 
        {
            MdtContactGroupSetBodies(g, body1, body2);
            MdtContactGroupSetSortKey(g, -((key1<<15)+key2));
            g->swapped = 0;
        }
        else
        {
            MdtContactGroupSetBodies(g, body2, body1);
            MdtContactGroupSetSortKey(g, -((key2<<15)+key1));
            g->swapped = 1;
        }
        MdtContactGroupSetGenerator(g,pair);
        pair->responseData = g;
    }
    return g;
}

/* *************************************************************** */
/* CONSTRUCTORS / DESTRUCTORS                                      */
/* *************************************************************** */

/**
 * Create a 'dynamic' model with the supplied geometry for simulation.
 * This function creates a collision McdModel and a dynamics MdtBody for it.
 * The supplied McdGeometry is used to calculate sensible values for the
 * inertia tensor of this body using the supplied density & geometry.
 * The model is automatically inserted into the universe's collision space
 * @see McdModelAndBodyDestroy
 */
McdModelID MEAPI MstModelAndBodyCreate(const MstUniverseID u,
                     const McdGeometryID g, const MeReal density)
{
    MdtBodyID body;

    McdModelID model = McdModelCreate(g);
    if(!model) return 0;

    body = MdtBodyCreate(u->world);
    if (!body) return 0;

    MdtBodyEnable(body);

    MstAutoSetMassProperties(body, model, density);

    McdModelSetBody(model, body);
    McdSpaceInsertModel(u->space, model);

    return model;
}

/**
 * Destroys the collision McdModel and dynamics MdtBody (if present).
 * @see MstModelAndBodyCreate
 */
void MEAPI MstModelAndBodyDestroy(const McdModelID m)
{
    /* if this model has a body. */
    if(m->mBody)
    {
        /* destroy physical body */
        MdtBodyDisable((MdtBodyID)m->mBody);
        MdtBodyDestroy((MdtBodyID)m->mBody);
    }
    else
    {
#ifdef _MECHECK
        MeWarning(0, "McdModelWithDynamicsDestroy: McdModel has no dynamics "
            "body.");
#endif
    }

    McdSpaceRemoveModel(m);
    McdModelDestroy(m);
}


/**
 * Create a fixed model with the supplied geometry for simulation.
 * This function creates a collision McdModel with no dynamics.
 * The model is automatically inserted into the universe's collision space
 * and frozen
 * @see MstFixedModelDestroy
 */

McdModelID        MEAPI MstFixedModelCreate(const MstUniverseID u,
                                            const McdGeometryID g,
                                            MeMatrix4Ptr transformation)
{
    McdModelID m = McdModelCreate(g);
    McdModelSetTransformPtr(m, transformation);
    McdSpaceInsertModel(MstUniverseGetSpace(u), m);
    McdSpaceUpdateModel(m);
    McdSpaceFreezeModel(m);
    return m;
}

/**
 * Destroys a fixed collision McdModel.
 * @see MstFixedModelCreate
 */

void MEAPI MstFixedModelDestroy(const McdModelID m)
{
    McdSpaceRemoveModel(m);
    McdModelDestroy(m);
}



/* Internal utils used by HandleContacts */

void MEAPI ConvertCollisionContact(MdtBclContactParams* params, McdContact* colC, MdtContactID dynC, int swap)
{
    if(swap)
        MdtContactSetNormal(dynC, -colC->normal[0], -colC->normal[1], -colC->normal[2]);
    else
        MdtContactSetNormal(dynC, colC->normal[0], colC->normal[1], colC->normal[2]);

    MdtContactSetPosition(dynC, colC->position[0], colC->position[1], colC->position[2]);
    MdtContactSetPenetration(dynC,-(colC->separation));
    MdtContactSetParams(dynC, params);
}

void MEAPI DebugOutputContact(McdContact* contact)
{
    MeInfo(0,"  pos: % f % f % f", contact->position[0], contact->position[1], contact->position[2]);
    MeInfo(0,"  nor: % f % f % f", contact->position[0], contact->position[1], contact->position[2]);
    MeInfo(0,"  sep: % f", contact->separation);
}

void MEAPI DebugOutputIntersectResult(McdIntersectResult* result)
{
    int i;
    MeInfo(0,"Intersect Result: %d Contacts", result->contactCount);

    for(i=0; i < result->contactCount; i++)
    {
        MeInfo(0,"Contact %d:", i);
        DebugOutputContact(result->contacts + i);
    }
    MeInfo(0,"max contact count: %d", result->contactMaxCount);
    MeInfo(0,"touch: %d", result->touch);
}

/**
 * Generate contact for potentially intersecting pairs and update dynamics.
 * This will call the relevant geometry-geometry test to generate contacts for
 * each pair, and will use the contact geometry to update the set of dynamics
 * contacts.
 * Call MstHandleTransitions first on the McdModelPairContainer to process
 * initialise 'Hello' pairs and clean up 'Goodbye' pairs.
 * Use McdSpaceGetPairs to fill the McdModelPairContainer with pairs from the
 * farfield McdSpace.
 */


void MEAPI MstHandleCollisions(McdModelPairContainer* pairs,
               const McdSpaceID s, const MdtWorldID w, const MstBridgeID b)
{
    int i, j;

    McdIntersectResult *results
        = MeMemoryALLOCA(pairs->size*sizeof(McdIntersectResult));
    McdContact *contacts = MeMemoryALLOCA(1000*sizeof(McdContact)); 
    int contactCount, resultCount;
    MeBool arrayFinished;

    if(pairs->helloFirst==pairs->stayingEnd)
        return;

    McdBatchContextReset(b->context);
    arrayFinished = 0;
    while(!arrayFinished)
    {
        arrayFinished = McdBatchIntersectEach(b->context, pairs, 
                                              results, &resultCount, pairs->size, 
                                              contacts, &contactCount, 1000);

        /* hello and staying - generate contacts and pass to dynamics */
        for( i = 0; i<resultCount ; i++ )
        {
            MdtContactID dynC;
            McdContact *colC;
            MdtContactGroupID group;
            McdIntersectResult *result = results+i;
            McdModelPairID pair = result->pair;
            
            MdtContactParamsID params;
            MstPerContactCBPtr contactCB;
            MstPerPairCBPtr pairCB;
            
            McdModelID m1 = pair->model1,
                       m2 = pair->model2;
            
            MstMaterialID material1 = McdModelGetMaterial(m1),
                          material2 = McdModelGetMaterial(m2);


            if(result->touch)
	        {
                MstIntersectCBPtr intersectCB = MstBridgeGetIntersectCB(b,
                    material1, material2);

                if(m1->mIntersectFn)
                    (*m1->mIntersectFn)(m1,result);

                if(m2->mIntersectFn)
                    (*m2->mIntersectFn)(m2,result);
                
                if(intersectCB)
                    (*intersectCB)(result);
            }
            
            /* For each contact in result: copy geo data into existing MdtContact.
            If new MdtContacts needed, get, point to params, then copy geo data. */
            colC = result->contacts;
            
            group = (MdtContactGroupID)pair->responseData;
            if(!group)
            {
                // no contact group, either because there was no space to create it
                // at some point, or because this is the first time these objects
                // have actually intersected.
                group = createContactGroup(w,pair);
            }
            if(group)
            {
                dynC = MdtContactGroupGetFirstContact(group);
                params = MstBridgeGetContactParams(b, material1, material2);
                contactCB = MstBridgeGetPerContactCB(b, material1, material2);
                
                /* First try to use contacts already in list */
                
                for( j = 0; j < result->contactCount; j++)
                {
                    if(dynC == MdtContactInvalidID)
                    {
                        dynC = MdtContactGroupCreateContact(group); /* auto-assigns bodies */
                    }
                    
                    if (dynC == MdtContactInvalidID)
                    {
#ifdef _MECHECK
                        MeFatalError(0, "Constraint pool too small during Mcd->Mdt contact creation.");
#endif
                    }
                    else 
                    {
                        ConvertCollisionContact(params,colC+j,dynC,MdtContactGroupIsSwapped(group));
                        
                        /* If there's no contact callback, or the contact callback says keep the contact,
                        advance to next contact. Otherwise we re-use it. */
                        
                        if(!contactCB || (*contactCB)(result, colC+j, dynC))
                            dynC = MdtContactGroupGetNextContact(group,dynC);
                    }
                }
                
                /* nuke all the remaining contacts in this contact group */
                
                while(dynC != MdtContactInvalidID)
                {
                    MdtContactID nextC = MdtContactGroupGetNextContact(group,dynC);
                    MdtContactGroupDestroyContact(group,dynC);
                    dynC = nextC;
                }
                
                /* Call Per Pair Callback (if there is one) */
                pairCB = MstBridgeGetPerPairCB(b, material1, material2);
                
                if(pairCB && !((*pairCB)(result,group)))
                {
                    MdtContactID c = MdtContactGroupGetFirstContact(group);
                    while (c != MdtContactInvalidID)
                    {
                        MdtContactID nextC = MdtContactGroupGetNextContact(group,c);
                        MdtContactGroupDestroyContact(group,c);
                        c = nextC;
                    }
                }
                
                if(group->count > 0)
                { 
                    if(!MdtContactGroupIsEnabled(group))
                        MdtContactGroupEnable(group);
                }
                else
                {
                    if(MdtContactGroupIsEnabled(group))
                        MdtContactGroupDisable(group);
                }
            }                
        }
    }
}

/**
 * Handle 'Hello' and 'Goodbye' farfield pairs.
 * This removes any contacts and data for 'Goodbye' pairs, and initialises
 * 'Hello' pairs. Use after McdSpaceRemoveModel before detroying the model.
 * Use McdSpaceGetPairs to fill the McdModelPairContainer with pairs from the
 * farfield McdSpace.
 **/

void MEAPI MstHandleTransitions(McdModelPairContainer* pairs,
               const McdSpaceID s, const MdtWorldID w, const MstBridgeID b)
{
    int i;

    /* Goodbye */
    for( i = 0 ; i < pairs->goodbyeEnd ; ++i )
    {
        MdtContactGroupID g = (MdtContactGroupID)pairs->array[i]->responseData;

        if(g)
        {
            MdtContactGroupReset(g); // should probably be disable!!!
            MdtContactGroupDestroy(g);
            pairs->array[i]->responseData = 0;
        }
    }

    /* free up Mcd resources of the goodbye pairs, batch mode */
    
    McdGoodbyeEach(pairs);

    for( i = pairs->helloFirst ; i < pairs->helloEndStayingFirst ; ++i )
    {
        McdModelPairID pair = pairs->array[i]; 

        MeI32 key1 = McdModelGetSortKey(pair->model1);
        MeI32 key2 = McdModelGetSortKey(pair->model2);

        McdGeometryType g1 = McdGeometryGetTypeId(McdModelGetGeometry(pair->model1));
        McdGeometryType g2 = McdGeometryGetTypeId(McdModelGetGeometry(pair->model2));

        /* if the geometries are the same, rearrange them before the hello
        so that the one with the lower sort key is first. */

        if(g1==g2 && key2<key1)
        {
            McdModelID tmp = pair->model1;
            pair->model1 = pair->model2;
            pair->model2 = tmp;
        }

        McdHello(pair);
    }
}



/**
 * Utility for setting the mass and inertia tensor of a dynamics body based on
 * the collision geometry and the supplied density.
 */
void MEAPI MstAutoSetMassProperties(const MdtBodyID body,
               const McdModelID model, const MeReal density)
{
    MeMatrix3 I;
    MeMatrix4 com_tm;
    MeReal vol, mass;

    McdGeometryID g = McdModelGetGeometry(model);

    //  This returns an inertia tensor which assumes the mass=1.0

    McdGeometryGetMassProperties(g, com_tm, I, &vol);

    MEASSERT(com_tm[0][0]==1 && com_tm[1][1]==1);  // Rotation must be identity

    MdtBodySetCenterOfMassRelativePosition(body, com_tm[3]);

    mass = vol * density;

    MeMatrix3Scale(I, mass);

    MdtBodySetMass(body, mass);
    MdtBodySetInertiaTensor(body, *(const MeMatrix3*)&I);
}



/**
 * Utility for setting the inertia tensor of a model based on
 * the model's collision geometry and the mass of the model's body.
 */
void MEAPI MstAutoSetInertialTensor(const McdModelID model)
{
	MeMatrix3 I;
	MeMatrix4 com_tm;
	MeReal vol;

	McdGeometryID geom;
	MdtBodyID body;

	// Make sure we have a model, body and geometry
	if ((!model)||(!McdModelGetBody(model))||(!McdModelGetGeometry(model)))
		return;
	
	geom = McdModelGetGeometry(model);
	body = McdModelGetBody(model);
	
	if (McdGeometryGetTypeId(geom)!=McdNullGetTypeId())
	{
		// get the inertial tensor of the geometry
		McdGeometryGetMassProperties(geom, com_tm, I, &vol);
	}
	else
	{
		// if the geometry is a NullGeometry, then assume a unit sphere
		I[0][0]=0.4f;
		I[0][1]=0.0f;
		I[0][2]=0.0f;
		I[1][0]=0.0f;
		I[1][1]=0.4f;
		I[1][2]=0.0f;
		I[2][0]=0.0f;
		I[2][1]=0.0f;
		I[2][2]=0.4f;
	}
	
	// scale the inertal tensor by the mass
	MeMatrix3Scale(I, MdtBodyGetMass(body));

	MdtBodySetInertiaTensor(body, *(const MeMatrix3*)&I);
}


void MEAPI FreezeModelFromBody(const MdtBodyID b)
{
    if(b->model && McdModelGetSpace((McdModelID)b->model))
        McdSpaceFreezeModel((McdModelID)b->model);
}

void MEAPI UnfreezeModelFromBody(const MdtBodyID b)
{
    if(b->model && McdModelGetSpace((McdModelID)b->model))
        McdSpaceUnfreezeModel((McdModelID)b->model);
}

void MEAPI DestroyContactGroupReferences(const MdtContactGroupID c)
{
    McdModelPairID m = (McdModelPairID)c->generator;
    if(m)
        m->responseData = 0;
}
/**
 * Setup callbacks on a world to ensure that when a body in a world stops
 * moving its collision model is frozen, and when an interaction causes it to
 * start moving again, its collision model is unfrozen.
 */
void MEAPI MstSetWorldHandlers(const MdtWorldID world)
{
    world->bodyDisableCallback = FreezeModelFromBody;
    world->bodyEnableCallback = UnfreezeModelFromBody;
    world->contactGroupDestroyCallback = DestroyContactGroupReferences;
}


/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/05/24 14:18:24 $ $Revision: 1.60.2.13.4.3 $

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

#include <MeAssetFactory.h>
#include <Mst.h>

/**
 * Allocates the memory for and initializes an MeAssetFactory struct.
 * An asset factory is tied to a world, a space and a collision framework.
 * Asset instances created using the factory contain a set of MdtBody's,
 * McdModels, McdGeometry's and MdtConstraints.
 * 
 * An MeAssetFactory struct contains a geometry manager which is responible
 * for a pool of shared geometry. @see McdGMCreateGeometry.
 *
 * @see MeAssetInstanceCreate
 */
MeAssetFactory *MEAPI MeAssetFactoryCreate(McdFrameworkID fwk)
{
    MeAssetFactory *af;
    af = (MeAssetFactory*)MeMemoryAPI.create(sizeof(MeAssetFactory));
    af->gm = McdGMCreate(fwk);
    af->geometryPostCreateCB = 0;
    af->geometryPostCreateCBUserdata = 0;
    af->modelCreateFunc = McdModelCreateFromMeFAssetPart;
    af->modelPostCreateCB = 0;
    af->modelPostCreateCBUserdata = 0;
    af->jointCreateFunc = MdtConstraintCreateFromMeFJoint;
    af->jointPostCreateCB = 0;
    af->jointPostCreateCBUserdata = 0;
    return af;
}

/**
 * Deallocates the memory used by the MeAssetFactory struct.
 * Also destroys it's geometry manager.
 */
void MEAPI MeAssetFactoryDestroy(MeAssetFactory *af)
{
    McdGMDestroy(af->gm);
    MeMemoryAPI.destroy(af);
}

/** @internal */
static char *MEAPI CreateHashKey(const char *s)
{
    char *key = MeMemoryAPI.create(strlen(s) + 1);
    strcpy(key, s);
    return key;
}

/**
 * Create an Karma instance of a MeFAsset database structure.
 * The relevant contents of the MeFAsset structure will be
 * converted into McdGeometry, McdModels, MdtBody's and MdtConstraints.
 * 
 * If @param owner is true, these Karma objects will be destroyed
 * (@see MeAssetInstanceDestroy). If @param owner is false, the
 * user is responsible for destroying them.
 *
 * A transform can be passed in to position the whole asset in
 * world space.
 */
MeAssetInstance *MEAPI MeAssetInstanceCreate(MeAssetFactory *af, 
                                             const MeFAsset *asset, 
                                             MeMatrix4Ptr tm, 
                                             MeBool owner, 
                                             MdtWorldID world, 
                                             McdSpaceID space)
{
    MeAssetInstance *ins = (MeAssetInstance*)MeMemoryAPI.create(sizeof(MeAssetInstance));
    
    const char *assetName = MeFAssetGetName(asset);
    char *key = 0;

    ins->af = af;
    ins->asset = (MeFAsset*)asset;
    ins->owner = owner;
    ins->world = world;
    ins->space = space;
    ins->nameToGeometry = MeHashCreate(17);
    ins->nameToModel = MeHashCreate(17);
    ins->nameToJoint = MeHashCreate(17);
    
    MeHashSetKeyFreeFunc(ins->nameToGeometry, MeMemoryAPI.destroy);
    MeHashSetKeyFreeFunc(ins->nameToModel, MeMemoryAPI.destroy);
    MeHashSetKeyFreeFunc(ins->nameToJoint, MeMemoryAPI.destroy);

    {
        MeFGeometryIt it;
        MeFGeometry *geom;

        MeFAssetInitGeometryIterator(asset, &it);
        while (geom = MeFAssetGetGeometry(&it))
        {
            McdGeometryID mcdGeom = McdGMCreateGeometry(ins->af->gm, geom, assetName);
            McdGeometryIncrementReferenceCount(mcdGeom);

            if (af->geometryPostCreateCB)
                af->geometryPostCreateCB(mcdGeom, geom, af->geometryPostCreateCBUserdata);

            key = CreateHashKey(MeFGeometryGetName(geom));
            MeHashInsert(key, mcdGeom, ins->nameToGeometry);
        }
    }

    {
        int i;
        int count = MeFAssetGetPartCount(asset);
        MeFAssetPart **part = (MeFAssetPart**)MeMemoryALLOCA(sizeof(MeFAssetPart*) * count);
        MeFAssetGetPartsSortedByName(asset, part);
        
        for (i = 0; i < count; i++)
        {
            McdModelID model = 0;
            McdGeometryID geom;
            MeFModel *fmodel = MeFAssetPartGetModel(part[i]);
            MeFGeometry *fgeom = MeFModelGetGeometry(fmodel);

            if (MeFModelGetType(fmodel) != kMeFModelTypeDynamicsOnly && fgeom)
                geom = (McdGeometryID)MeHashLookup(MeFGeometryGetName(fgeom), ins->nameToGeometry);
            else
                geom = McdGMGetNullGeometry(af->gm);
        
            key = CreateHashKey(MeFAssetPartGetName(part[i]));

            if (af->modelCreateFunc)
                model = af->modelCreateFunc(part[i], geom, world, tm);

            McdSpaceInsertModel(space, model);

            if (af->modelPostCreateCB)
                af->modelPostCreateCB(model, part[i], af->modelPostCreateCBUserdata);

            MeHashInsert(key, model, ins->nameToModel);
        }

        /* disable collision pairs */
        for (i = 0; i < count; i++)
        {
            int j;
            for (j = i + 1; j < count; j++)
            {
                if (!MeFAssetPartIsCollisionEnabled(part[i], part[j]))
                {
                    McdModelID model1 = (McdModelID)MeHashLookup(MeFAssetPartGetName(part[i]), ins->nameToModel);
                    McdModelID model2 = (McdModelID)MeHashLookup(MeFAssetPartGetName(part[j]), ins->nameToModel);
                    
                    if (model1 && model2)
                    {
                        if (McdModelGetSpace(model1) && McdModelGetSpace(model2))
                            McdSpaceDisablePair(model1, model2);
                    }
#ifdef _MECHECK
                    else
                    {
                        ME_REPORT(MeWarning(3,"MeAssetInstanceCreate: Invalid collision pair."));
                    }
#endif
                }
            }
        }
    }

        
    {
        MeFJointIt it;
        MeFJoint *joint;
        MeFAssetInitJointIterator(asset, &it);
    
        while (joint = MeFAssetGetJoint(&it))
        {
            MdtConstraintID constraint;
            McdModelID model1 = 0, model2 = 0;
            MeFAssetPart *part1, *part2;
    
            part1 = MeFJointGetPart(joint, 0);
            part2 = MeFJointGetPart(joint, 1);
        
            if (part1)
                model1 = (McdModelID)MeHashLookup(MeFAssetPartGetName(part1), ins->nameToModel);

            if (part2)
                model2 = (McdModelID)MeHashLookup(MeFAssetPartGetName(part2), ins->nameToModel);

            if ( (model1 && McdModelGetBody(model1) ) || (model2 && McdModelGetBody(model2)) )
            {
                if (af->jointCreateFunc)
                    constraint = af->jointCreateFunc(joint, world, model1, model2, tm);

                if (constraint)
                    MdtConstraintEnable(constraint);
                
                if (af->jointPostCreateCBUserdata)
                    af->jointPostCreateCB(constraint, joint, af->jointPostCreateCBUserdata);

                key = CreateHashKey(MeFJointGetName(joint));
                MeHashInsert(key, constraint, ins->nameToJoint);
            }
        }
    }
    
    return ins;
}

/**
 * Destroys an asset instance. If the Karma objects are owned
 * (@see MeAssetInstanceCreate), they will be destroyed, otherwise
 * the user must do it.
 */
void MEAPI MeAssetInstanceDestroy(MeAssetInstance *ins)
{
    MeHashIterator it;

    if (ins->owner)
    {
        MdtConstraintID joint;
        MeHashInitIterator(&it, ins->nameToJoint);
        
        while (joint = (MdtConstraintID)MeHashGetDatum(&it))
        {
            MdtConstraintDisable(joint);
            MdtConstraintDestroy(joint);
        }
    }

    if (ins->owner)
    {
        McdModelID model;
        MeHashInitIterator(&it, ins->nameToModel);

        while (model = (McdModelID)MeHashGetDatum(&it))
        {
            MdtBodyID body;
            MeMatrix4Ptr tm = 0;
            
            if(McdModelGetSpace(model))
            {
                McdSpaceRemoveModel(model);
				MstBridgeUpdateTransitions(ins->bridge,ins->space,ins->world);
            }

            if(body = McdModelGetBody(model))
            {
                MdtBodyDisable(body);
                MdtBodyDestroy(body);
            }
            else
            {
                tm = McdModelGetTransformPtr(model);
            }

            McdModelDestroy(model);

            if (tm)
                MeMemoryAPI.destroyAligned(tm);

        }
    }

    if (ins->owner)
    {
        McdGeometryID geom;
        MeHashInitIterator(&it, ins->nameToGeometry);

        while (geom = (McdGeometryID)MeHashGetDatum(&it))
        {
			McdGeometryDecrementReferenceCount(geom);
            McdGMDestroyGeometry(ins->af->gm, geom);
        }
    }

    MeHashDestroy(ins->nameToGeometry);
    MeHashDestroy(ins->nameToModel);
    MeHashDestroy(ins->nameToJoint);

    MeMemoryAPI.destroy(ins);
}

/**
 * Sets a callback which gets called after geometry creation
 * (@see McdGMCreateGeometry), giving the user a chance to
 * customize the McdGeometry.
 */
void MEAPI MeAssetFactorySetGeometryPostCreateCB(MeAssetFactory *af, GeometryPostCreateCB cb, void *userdata)
{
    af->geometryPostCreateCB = cb;
    af->geometryPostCreateCBUserdata = userdata;
}

/**
 * Sets a callback to replace the default model creation function 
 * (@see McdModelCreateFromMeFAssetPart).
 */ 
void MEAPI MeAssetFactorySetModelCreateFunction(MeAssetFactory *af, ModelCreateFunc func)
{
    af->modelCreateFunc = func;
}

/**
 * Sets a callback which gets called after the default model create
 * function has been called, giving the user a chance to customize
 * the McdModel/MdtBody.
 */
void MEAPI MeAssetFactorySetModelPostCreateCB(MeAssetFactory *af, ModelPostCreateCB cb, void *userdata)
{
    af->modelPostCreateCB = cb;
    af->modelPostCreateCBUserdata = userdata;
}

/**
 * Sets a callback to replace the default joint creation function
 * (@see MdtConstraintCreateFromMeFJoint).
 */
void MEAPI MeAssetFactorySetJointCreateFunction(MeAssetFactory *af, JointCreateFunc func)
{
    af->jointCreateFunc = func;
}

/**
 * Sets a callback which gets called after the default joint create
 * function has been called, giving the user a chance to customize
 * the MdtConstraint. Alternatively the whole creation function
 * can be replaced (@see MeAssetFactorySetJointCreateFunction),
 */
void MEAPI MeAssetFactorySetJointPostCreateCB(MeAssetFactory *af, JointPostCreateCB cb, void *userdata)
{
    af->jointPostCreateCB = cb;
    af->jointPostCreateCBUserdata = userdata;
}

/**
 * Given a geometry name, return an McdGeometryID.
 */
McdGeometryID MEAPI MeAssetInstanceGetGeometry(MeAssetInstance *ins, char *name)
{
    return (McdGeometryID)MeHashLookup(name, ins->nameToGeometry);
}

/**
 * Given a model name, return an McdModelID.
 */
McdModelID MEAPI MeAssetInstanceGetModel(MeAssetInstance *ins, char *name)
{
    return (McdModelID)MeHashLookup(name, ins->nameToModel);
}

/**
 * Given a joint name, return an MdtConstraint.
 */
MdtConstraintID MEAPI MeAssetInstanceGetJoint(MeAssetInstance *ins, char *name)
{
    return (MdtConstraintID)MeHashLookup(name, ins->nameToJoint);
}

/**
 * Initialize an iterator to iterate over the geometry in the asset instance.
 * Because of the nature of the asset database, all geometry is an aggregate,
 * so only aggregates will be returned and not their children.
 */
void MEAPI MeAssetInstanceInitGeometryIterator(MeAssetInstance *ins, MeAIGeomIt *it)
{
    MeHashInitIterator(&it->hashIt, ins->nameToGeometry);
}

/**
 * Return the McdGeometry associated with the iterator and advance the iterator.
 * Returns NULL when no more.
 */
McdGeometryID MEAPI MeAssetInstanceGetNextGeometry(MeAIGeomIt *it)
{
    return (McdGeometryID)MeHashGetDatum(&it->hashIt);
}

/**
 * Initialize an iterator to iterate over all McdModels used by this
 * asset instance,
 */
void MEAPI MeAssetInstanceInitModelIterator(MeAssetInstance *ins, MeAIModelIt *it)
{
    MeHashInitIterator(&it->hashIt, ins->nameToModel);
}

/**
 * Return the McdModel associated with the iterator and advance the iterator.
 * Returns NULL when no more.
 */  
McdModelID MEAPI MeAssetInstanceGetNextModel(MeAIModelIt *it)
{
    return (McdModelID)MeHashGetDatum(&it->hashIt);
}

/**
 * Initialize an iterator to iterate over all MdtConstraints in this
 * asset instance.
 */
void MEAPI MeAssetInstanceInitJointIterator(MeAssetInstance *ins, MeAIJointIt *it)
{
    MeHashInitIterator(&it->hashIt, ins->nameToJoint);
}

/**
 * Return the MdtConstraint associated with the iterator and advance the iterator.
 * Returns NULL when no more.
 */  
MdtConstraintID MEAPI MeAssetInstanceGetNextJoint(MeAIJointIt *it)
{
    return (MdtConstraintID)MeHashGetDatum(&it->hashIt);
}

void MEAPI MeAssetInstanceEnableDynamics(MeAssetInstance *ins)
{

}

void MEAPI MeAssetInstanceDisableDynamics(MeAssetInstance *ins)
{

}

void *MEAPI MeAssetInstanceGetUserData(MeAssetInstance *ins)
{
    return ins->userdata;
}

void MEAPI MeAssetInstanceSetUserData(MeAssetInstance *ins, void *userdata)
{
    ins->userdata = userdata;
}

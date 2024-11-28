/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:05 $ - Revision: $Revision: 1.32.2.10 $

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
#include <MeAssetDB.h>
#include <MeMath.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include <MdtDefaults.h>
#include "MeAssetDBInternal.h"

/*----------------------- Creation and destruction ---------------------- */

/** @internal */
static void MEAPI _MeFModelInit(MeFModel *const fm)
{
    fm->id = 0;
    fm->asset = 0;
    fm->geometry = 0;
    fm->mass_offset[0] = fm->mass_offset[1] = fm->mass_offset[2] = 0;

    /* Defaults are the same as the toolkit */
    fm->angularDamp = MDTBODY_DEFAULT_ANGULAR_DAMPING;
    fm->linearDamp = MDTBODY_DEFAULT_LINEAR_DAMPING;
    fm->fastSpin[0] = MDTBODY_DEFAULT_FAST_SPIN_X;
    fm->fastSpin[1] = MDTBODY_DEFAULT_FAST_SPIN_Y;
    fm->fastSpin[2] = MDTBODY_DEFAULT_FAST_SPIN_Z;
    fm->useFastSpin = 0;
    fm->inertia[0] = MDTBODY_DEFAULT_INERTIA;
    fm->inertia[1] = 0;
    fm->inertia[2] = 0;
    fm->inertia[3] = MDTBODY_DEFAULT_INERTIA;
    fm->inertia[4] = 0;
    fm->inertia[5] = MDTBODY_DEFAULT_INERTIA;
    fm->mass = MDTBODY_DEFAULT_MASS;
    fm->density = 1;
}

/**
 * Creates a MeFModel.
 */
MeFModel *MEAPI MeFModelCreate(const char *const name, MeFModelType type)
{
    MeFModel *fm = (MeFModel*)MeMemoryAPI.createZeroed(sizeof(MeFModel));
    
    _MeFModelInit(fm);
    _FSetStringProperty(&fm->id, name);
    fm->type = type;
    
    return fm;
}

/**
 * Return a copy of the specified model. The model must be inserted into an
 * asset.
 */
MeFModel *MEAPI MeFModelCreateCopy(const MeFModel *const model, MeBool recurse)
{
    MeFModel *copy = (MeFModel*)MeMemoryAPI.createZeroed(sizeof(MeFModel));

    _FSetStringProperty(&copy->id, model->id);
    copy->type = model->type;
    _FSetStringProperty(&copy->geometry, model->geometry);

    MeVector3Copy(copy->mass_offset, model->mass_offset);
    copy->angularDamp = model->angularDamp;
    copy->linearDamp = model->linearDamp;
    MeVector3Copy(copy->fastSpin, model->fastSpin);
    copy->useFastSpin = model->useFastSpin;
    copy->inertia[0] = model->inertia[0];
    copy->inertia[1] = model->inertia[1];
    copy->inertia[2] = model->inertia[2]; 
    copy->inertia[3] = model->inertia[3];
    copy->inertia[4] = model->inertia[4];
    copy->inertia[5] = model->inertia[5];
    copy->mass = model->mass;
    copy->density = model->density;
    
    return copy;
}

/**
 * Copy the properties of src model to dst model.
 */
void MEAPI MeFModelCopy(MeFModel *dst, const MeFModel *const src, MeBool setGeometry)
{
    dst->type = src->type;
    if(setGeometry)
        _FSetStringProperty(&dst->geometry, src->geometry);

    MeVector3Copy(dst->mass_offset, src->mass_offset);
    dst->angularDamp = src->angularDamp;
    dst->linearDamp = src->linearDamp;
    MeVector3Copy(dst->fastSpin, src->fastSpin);
    dst->useFastSpin = src->useFastSpin;
    dst->inertia[0] = src->inertia[0];
    dst->inertia[1] = src->inertia[1];
    dst->inertia[2] = src->inertia[2]; 
    dst->inertia[3] = src->inertia[3];
    dst->inertia[4] = src->inertia[4];
    dst->inertia[5] = src->inertia[5];
    dst->mass = src->mass;
    dst->density = src->density;
}

/**
 * Destroys an MeFModel.
 */
void MEAPI MeFModelDestroy(MeFModel *const fm)
{
    MeMemoryAPI.destroy(fm->id);

    if (fm->geometry)
        MeMemoryAPI.destroy(fm->geometry);

    MeMemoryAPI.destroy(fm);
}

/*--------------------------------- Accessors ---------------------------------- */

/**
 * Returns an MeFModel's name.
 */
char *MEAPI MeFModelGetName(const MeFModel *const fm)
{
    return fm->id;
}

/**
 * Returns the model's type: Dynamics and geometry, dynamics only or geometry only.
 */
MeFModelType MEAPI MeFModelGetType(const MeFModel *const fm)
{
    return fm->type;
}

/**
 * Returns an the name of the model's geometry.
 */
char *MEAPI MeFModelGetGeometryName(const MeFModel *const fm)
{
    return fm->geometry;
}

/**
 * Returns an the model's geometry. The model must be inserted into an
 * asset for this to work. Returns 0 if the model is not in an asset.
 */
MeFGeometry *MEAPI MeFModelGetGeometry(const MeFModel *const fm)
{
    MeFGeometry *g = 0;

    if (!fm->asset)
        return 0;

    if (fm->geometry)
        g = MeFAssetLookupGeometry(fm->asset, fm->geometry);
    return g;
}

/**
 * Yields the MeFModel's centre of mass offset.
 */
void MEAPI MeFModelGetMassOffset(const MeFModel *const fm, MeVector3 o)
{
    MeVector3Copy(o, fm->mass_offset);
}

/**
 * Returns the MeFModel's mass.
 */
MeReal MEAPI MeFModelGetMass(const MeFModel *const fm)
{
    return fm->mass;
}

/**
 * Returns the MeFModel's density.
 */
MeReal MEAPI MeFModelGetDensity(const MeFModel *const fm)
{
    return fm->density;
}

/**
 * Yields the MeFModel's inertia tensor.
 */
void MEAPI MeFModelGetInertiaTensor(const MeFModel *const fm, const MeMatrix3Ptr I)
{
    MeMatrix3MakeIdentity(I);
    I[0][0] = fm->inertia[0];
    I[1][1] = fm->inertia[3]; 
    I[2][2] = fm->inertia[5];

    I[1][0] = I[0][1] = fm->inertia[1]; 
    I[2][0] = I[0][2] = fm->inertia[2]; 
    I[2][1] = I[1][2] = fm->inertia[4]; 
}

/**
 * Returns the MeFModel's linear velocity damping.
 */
MeReal MEAPI MeFModelGetLinearVelocityDamping(const MeFModel *const fm)
{
    return fm->linearDamp;
}

/**
 * Returns MeFModel's angular velocity damping.
 */
MeReal MEAPI MeFModelGetAngularVelocityDamping(const MeFModel *const fm)
{
    return fm->angularDamp;
}

/**
 * Yields the MeFModel's fast spin axis.
 */
void MEAPI MeFModelGetFastSpinAxis(const MeFModel *const fm, MeVector3 axis)
{
    MeVector3Copy(axis, fm->fastSpin);
}

/**
 * Returns true if the fast spin axis is enabled,
 */
MeBool MEAPI MeFModelIsFastSpinAxisEnabled(const MeFModel *const model)
{
    return model->useFastSpin;
}


/*--------------------------------- Mutators ---------------------------------- */

/**
 * Renames an MeFModel. If the model is inserted into an asset, all
 * references will be resolved - ie any parts referencing this model
 * will have their references updated.
 */
void MEAPI MeFModelRename(MeFModel *const fm, const char *const newName)
{
    if (strcmp(fm->id, newName) != 0)
    {
        if (fm->asset)
        {
            MeFAssetResolveModelReferences(fm->asset, fm->id, newName);
            MeHashDelete(fm->id, fm->asset->nameToModel);
        }

        _FSetStringProperty(&fm->id, newName);
        
        if (fm->asset)
            MeHashInsert(fm->id, fm, fm->asset->nameToModel);
    }
}

/**
 * Sets the model's type: dynamics and geometry, dynamics only or geometry only.
 */
void MEAPI MeFModelSetType(MeFModel *const fm, MeFModelType type)
{
    fm->type = type;
}

/**
 * Sets MeFModel geometry.
 */
void MEAPI MeFModelSetGeometry(MeFModel *const fm, const MeFGeometry *const fg)
{
    if(fg)
        MeFModelSetGeometryByName(fm, fg->id);
}

/**
 * Sets the model's geometry by name.
 */
void MEAPI MeFModelSetGeometryByName(MeFModel *const model, const char *const name)
{
    _FSetStringProperty(&model->geometry, name);
}

/**
 * Sets MeFModel centre of mass offset.
 */
void MEAPI MeFModelSetMassOffset(MeFModel *const fm, MeVector3 v)
{
    MeVector3Copy(fm->mass_offset, v);
}

/**
 * Sets MeFModel mass.
 */
void MEAPI MeFModelSetMass(MeFModel *const fm, MeReal mass)
{
    fm->mass = mass;
}

/**
 * Sets MeFModel density.
 */
void MEAPI MeFModelSetDensity(MeFModel *const fm, MeReal density)
{
    fm->density = density;
}

/**
 * Sets MeFModel inertia tensor.
 */
void MEAPI MeFModelSetInertiaTensor(MeFModel *const fm, const MeMatrix3Ptr I)
{
    fm->inertia[0] = I[0][0];
    fm->inertia[3] = I[1][1]; 
    fm->inertia[5] = I[2][2];
    fm->inertia[1] = I[1][0]; 
    fm->inertia[2] = I[2][0]; 
    fm->inertia[4] = I[2][1]; 
}

/**
 * Sets MeFModel linear velocity damping.
 */
void MEAPI MeFModelSetLinearVelocityDamping(MeFModel *const fm, MeReal d)
{
    fm->linearDamp = d;
}

/**
 * Sets MeFModel angular velocity damping.
 */
void MEAPI MeFModelSetAngularVelocityDamping(MeFModel *const fm, MeReal d)
{
    fm->angularDamp = d;
}

/**
 * Sets MeFModel fast spin axis.
 */
void MEAPI MeFModelSetFastSpinAxis(MeFModel *const fm, MeReal x, MeReal y, MeReal z)
{
    fm->fastSpin[0] = x;
    fm->fastSpin[1] = y;
    fm->fastSpin[2] = z;
}

/**
 * Enables/disables fast spin axis.
 */
void MEAPI MeFModelEnableFastSpinAxis(MeFModel *const model, MeBool b)
{
    model->useFastSpin = b;
}

/**
 * Scales the model by the given scale factor. Doesn't scale its geometry but
 * does scale the model's graphic scale.
 */
void MEAPI MeFModelScale(MeFModel *const model, MeReal scale)
{
    int i;
    MeMatrix3 I;

    for (i = 0; i < 3; i++)
        model->mass *= scale;

    MeFModelGetInertiaTensor(model, I);
    for (i = 0; i < 5; i++)
        MeMatrix3Scale(I, scale);
    MeFModelSetInertiaTensor(model, I);
    
    MeVector3Scale(model->mass_offset, scale);
}

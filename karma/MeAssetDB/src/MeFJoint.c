/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 12:29:56 $ - Revision: $Revision: 1.43.2.16 $

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
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeMath.h>
#include <MdtDefaults.h>
#include "MeAssetDBInternal.h"

/*----------------------- Creation and destruction ---------------------- */

/**
 *  Creates an MeFJoint.
 */
MeFJoint *MEAPI MeFJointCreate(const char *const name, MeFJointType type)
{
    MeFJoint *fj = (MeFJoint*)MeMemoryAPI.createZeroed(sizeof(MeFJoint));

    _FSetStringProperty(&fj->id, name);
    fj->type = type;
    
    MeFJointSetGenericDefaults(fj);
    MeFJointSetPerTypeDefaults(fj);
    return fj;
}

/**
 * Creates and returns a copy of the specified MeFJoint. The joint must
 * be inserted into an asset.
 */
MeFJoint *MEAPI MeFJointCreateCopy(const MeFJoint *const fj, MeBool recurse)
{
    MeFJoint *copy = (MeFJoint*)MeMemoryAPI.createZeroed(sizeof(MeFJoint));
    int i;

    copy->type = fj->type;
    _FSetStringProperty(&copy->id, fj->id);
    _FSetStringProperty(&copy->part[0], fj->part[0]);
    _FSetStringProperty(&copy->part[1], fj->part[1]);

    for (i = 0; i < 2; i++)
    {
        MeVector3Copy(copy->pos[i], fj->pos[i]);
        MeVector3Copy(copy->pax[i], fj->pax[i]);
        MeVector3Copy(copy->oax[i], fj->oax[i]);    
    }

    copy->stop[0] = fj->stop[0];
    copy->stop[1] = fj->stop[1];
    copy->stop[2] = fj->stop[2];
    copy->stiff[0] = fj->stiff[0];
    copy->stiff[1] = fj->stiff[1];
    copy->stiff[2] = fj->stiff[2];
    copy->stiff[3] = fj->stiff[3];
    copy->stiff[4] = fj->stiff[4];
    copy->stiff[5] = fj->stiff[5];
    copy->damp[0] = fj->damp[0];
    copy->damp[1] = fj->damp[1]; 
    copy->damp[2] = fj->damp[2]; 
    copy->damp[3] = fj->damp[3];
    copy->damp[4] = fj->damp[4]; 
    copy->damp[5] = fj->damp[5]; 
    copy->bLimited[0] = fj->bLimited[0];
    copy->bMotorized[0] = fj->bMotorized[0];
    copy->strength[0] = fj->strength[0];
    copy->strength[1] = fj->strength[1];
    copy->strength[2] = fj->strength[2];
    copy->strength[3] = fj->strength[3];
    copy->strength[4] = fj->strength[4];
    copy->strength[5] = fj->strength[5]; 
    copy->desVel[0] = fj->desVel[0];
    copy->desVel[1] = fj->desVel[1]; 
    copy->special_f[0] = fj->special_f[0];
    copy->special_i[0] = fj->special_i[0];
    copy->special_i[1] = fj->special_i[1];
    copy->special_b[0] = fj->special_b[0];

    return copy;
}

/**
 * Copies one joint's properties to another.
 */
void MEAPI MeFJointCopy(MeFJoint *const dst, const MeFJoint *const src, MeBool copyAxes)
{
    int i;

    dst->type = src->type;

    if(copyAxes)
    {
        for (i = 0; i < 2; i++)
        {
            MeVector3Copy(dst->pos[i], src->pos[i]);
            MeVector3Copy(dst->pax[i], src->pax[i]);
            MeVector3Copy(dst->oax[i], src->oax[i]);    
        }
    }

    dst->stop[0] = src->stop[0];
    dst->stop[1] = src->stop[1];
    dst->stop[2] = src->stop[2];
    dst->stiff[0] = src->stiff[0];
    dst->stiff[1] = src->stiff[1];
    dst->stiff[2] = src->stiff[2];
    dst->stiff[3] = src->stiff[3];
    dst->stiff[4] = src->stiff[4];
    dst->stiff[5] = src->stiff[5];
    dst->damp[0] = src->damp[0];
    dst->damp[1] = src->damp[1];
    dst->damp[2] = src->damp[2]; 
    dst->damp[3] = src->damp[3];
    dst->damp[4] = src->damp[4];
    dst->damp[5] = src->damp[5]; 
    dst->bLimited[0] = src->bLimited[0];
    dst->bMotorized[0] = src->bMotorized[0];
    dst->strength[0] = src->strength[0];
    dst->strength[1] = src->strength[1];
    dst->strength[2] = src->strength[2];
    dst->strength[3] = src->strength[3];
    dst->strength[4] = src->strength[4];
    dst->strength[5] = src->strength[5]; 
    dst->desVel[0] = src->desVel[0];
    dst->desVel[1] = src->desVel[1]; 
    dst->special_f[0] = src->special_f[0];
    dst->special_i[0] = src->special_i[0];
    dst->special_i[1] = src->special_i[1];
    dst->special_b[0] = src->special_b[0];
}

/**
 * Destroys an MeFJoint. The joint must be removed from an asset
 * database before it can be destroyed.
 */
void MEAPI MeFJointDestroy(MeFJoint *const fj)
{
    MeMemoryAPI.destroy(fj->part[0]);
    
    if (fj->part[1])
        MeMemoryAPI.destroy(fj->part[1]);
    
    MeMemoryAPI.destroy(fj->id);
    MeMemoryAPI.destroy(fj);
}

/*--------------------------------- Accessors ---------------------------------- */

/**
 * Returns an MeFJoint's name.
 */
char *MEAPI MeFJointGetName(const MeFJoint *const joint)
{
    return joint->id;
}

/**
 * Returns the joint's type.
 */
MeFJointType MEAPI MeFJointGetType(const MeFJoint *const joint)
{
    return joint->type;
}

/**
 * Returns the part name associated with the joint.
 */
char *MEAPI MeFJointGetPartName(const MeFJoint *const joint, int index)
{
    return  joint->part[index];
}

/**
 * Returns the part associated with the joint. The joint must have been inserted
 * into an asset for this to work. Returns 0 if the joint is not in an asset.
 */
MeFAssetPart *MEAPI MeFJointGetPart(const MeFJoint *const joint, int index)
{
    MeFAssetPart *p = 0;
    if (!joint->asset)
        return 0;

    if (joint->part[index])
        p = MeFAssetLookupPart(joint->asset, joint->part[index]);
    return p;
}

/**
 * Returns the position of the joint. @param index determines which part's reference
 * frame the position is in.
 */
void MEAPI MeFJointGetPosition(const MeFJoint *const j, int index, MeVector3 pos)
{
    MEASSERT(index >=0 && index < 2);
    MeVector3Copy(pos, j->pos[index]);
}

/**
 * Returns the primary axis of the joint. @param index determines which part's reference
 * frame it is in.
 */
void MEAPI MeFJointGetPrimaryAxis(const MeFJoint *const j, int index, MeVector3 axis)
{
    MEASSERT(index >=0 && index < 2);
    MeVector3Copy(axis, j->pax[index]);
}

/**
 * Returns the orthogonal axis of the joint. @param index determines which part's reference
 * frame it is in.
 */
void MEAPI MeFJointGetOrthogonalAxis(const MeFJoint *const j, int index, MeVector3 axis)
{
    MEASSERT(index >=0 && index < 2);
    MeVector3Copy(axis, j->oax[index]);
}

MeBool MEAPI MeFJointGetProperty1i(const MeFJoint *const joint, MeFJointProperty p, int *i)
{
    switch (p)
    {
    case kMeFJointPropertySpecialInt1:
        *i = joint->special_i[0];
        return 1;

    case kMeFJointPropertySpecialInt2:
        *i = joint->special_i[1];
        return 1;
    }
    
    return 0;
}

MeBool MEAPI MeFJointGetProperty1ui(const MeFJoint *const joint, MeFJointProperty p, unsigned int *u)
{
    return 0;
}

MeBool MEAPI MeFJointGetProperty1b(const MeFJoint *const joint, MeFJointProperty p, MeBool *b)
{
    switch (p)
    {
    case kMeFJointPropertyLimited1:
        *b = joint->bLimited[0];
        return 1;

    case kMeFJointPropertyMotorized1:
        *b = joint->bMotorized[0];
        return 1;
    
    case kMeFJointPropertySpecialBool1:
        *b = joint->special_b[0];
        return 1;
    }

    return 0;
}

MeBool MEAPI MeFJointGetProperty1f(const MeFJoint *const joint, MeFJointProperty p, MeReal *x)
{
    switch (p)
    {
    case kMeFJointPropertyStop1:
        *x = joint->stop[0];
        return 1;

    case kMeFJointPropertyStop2:
        *x = joint->stop[1];
        return 1;

    case kMeFJointPropertyStop3:
        *x = joint->stop[2];
        return 1;

    case kMeFJointPropertyStiffness1:
        *x = joint->stiff[0];
        return 1;

    case kMeFJointPropertyStiffness2:
        *x = joint->stiff[1];
        return 1;

    case kMeFJointPropertyStiffness3:
        *x = joint->stiff[2];
        return 1;

    case kMeFJointPropertyStiffness4:
        *x = joint->stiff[3];
        return 1;

    case kMeFJointPropertyStiffness5:
        *x = joint->stiff[4];
        return 1;

    case kMeFJointPropertyStiffness6:
        *x = joint->stiff[5];
        return 1;

    case kMeFJointPropertyDamping1:
        *x = joint->damp[0];
        return 1;

    case kMeFJointPropertyDamping2:
        *x = joint->damp[1];
        return 1;

    case kMeFJointPropertyDamping3:
        *x = joint->damp[2];
        return 1;

    case kMeFJointPropertyDamping4:
        *x = joint->damp[3];
        return 1;

    case kMeFJointPropertyDamping5:
        *x = joint->damp[4];
        return 1;

    case kMeFJointPropertyDamping6:
        *x = joint->damp[5];
        return 1;

    case kMeFJointPropertyStrength1:
        *x = joint->strength[0];
        return 1;

    case kMeFJointPropertyStrength2:
        *x = joint->strength[1];
        return 1;
    
    case kMeFJointPropertyStrength3:
        *x = joint->strength[2];
        return 1;

    case kMeFJointPropertyStrength4:
        *x = joint->strength[3];
        return 1;

    case kMeFJointPropertyStrength5:
        *x = joint->strength[4];
        return 1;

    case kMeFJointPropertyStrength6:
        *x = joint->strength[5];
        return 1;

    case kMeFJointPropertyDesiredVelocity1:
        *x = joint->desVel[0];
        return 1;

    case kMeFJointPropertyDesiredVelocity2:
        *x = joint->desVel[1];
        return 1;

    case kMeFJointPropertySpecialFloat1:
        *x = joint->special_f[0];
        return 1;
    }

    return 0;
}

/*--------------------------------- Mutators ---------------------------------- */

/**
 * Sets the defaults for only those properties common to all joints.
 * To set per-type defaults use MeFJointSetPerTypeDefaults().
 */
void MEAPI MeFJointSetGenericDefaults(MeFJoint *const fj)
{
    int i;
    fj->part[0] = fj->part[1] = 0;
    fj->asset = 0;

    for (i = 0; i < 2; i++)
    {
        MeVector3Set(fj->pos[i], 0, 0, 0);
        MeVector3Set(fj->pax[i], 1, 0, 0);
        MeVector3Set(fj->oax[i], 0, 0, 1);
    }
}

/**
 * Sets per-type joint defaults. This does not include generic properties such
 * as parts and axes.
 */
void MEAPI MeFJointSetPerTypeDefaults(MeFJoint *const fj)
{
    fj->stop[0] = 0; fj->stop[1] = fj->stop[2] = 0;
    fj->stiff[0] = fj->stiff[1] = fj->stiff[2] = 0;
    fj->stiff[3] = fj->stiff[4] = fj->stiff[5] = 0;
    fj->damp[0] = fj->damp[1] = fj->damp[2] = 0; 
    fj->damp[3] = fj->damp[4] = fj->damp[5] = 0; 
    fj->bLimited[0] = 0;
    fj->bMotorized[0] = 0;
    fj->strength[0] = fj->strength[1] = fj->strength[2] = 0;
    fj->strength[3] = fj->strength[4] = fj->strength[5] = 0; 
    fj->desVel[0] = fj->desVel[1] = 0;
    fj->special_f[0] = 0;
    fj->special_i[0] = 0; fj->special_i[1] = 0;
    fj->special_b[0] = 0;

    switch (fj->type)
    {
    case kMeFJointTypeBallAndSocket:
        break;

    case kMeFJointTypeHinge:
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, ME_PI/2); /* upper limit */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, -ME_PI/2); /* lower limit */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, 1000);
        MeFJointSetProperty1b(fj, kMeFJointPropertyLimited1, 0);
        MeFJointSetProperty1b(fj, kMeFJointPropertyMotorized1, 0);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity1, 1);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, 1000);
        break;

    case kMeFJointTypeCarwheel:        
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity1, 1); /* hinge motor desired velocity */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, 1000); /* hinge max motor force */
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity2, 1); /* steering motor desired velocity */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength2, 1000); /* steering max motor force */
        MeFJointSetProperty1b(fj, kMeFJointPropertySpecialBool1, 0); /* steering lock */
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, 1); /* suspension damping */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, 0.5f); /* suspension upper limit */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, -0.5f); /* suspension lower limit */
        MeFJointSetProperty1f(fj, kMeFJointPropertySpecialFloat1, 0); /* suspension reference */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, 1000); /* suspension stiffness */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, 1000); /* suspension limit softness */
        break;

    case kMeFJointTypeConeLimit:
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, ME_PI/2);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, 1000);
        break;

    case kMeFJointTypeUniversal:        
        break;

    case kMeFJointTypeRpro:        
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength2, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength3, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength4, 0);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength5, 0);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength6, 0);
        break;

    case kMeFJointTypePrismatic:       
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, 0.5f);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, -0.5f);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, 1000);
        MeFJointSetProperty1b(fj, kMeFJointPropertyLimited1, 0);
        MeFJointSetProperty1b(fj, kMeFJointPropertyMotorized1, 0);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity1, 1);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, 1000);
        break;

    case kMeFJointTypeSkeletal:
        MeFJointSetProperty1i(fj, kMeFJointPropertySpecialInt1, 2); /* cone type */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, ME_PI/4); /* x half angle */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, ME_PI/4); /* y half angle */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, 1);
        MeFJointSetProperty1i(fj, kMeFJointPropertySpecialInt2, 2); /* twist type */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop3, ME_PI/2); /* amount cone can twist */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, 1000); /* cone twist stiffness */
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping2, 1); /* cone twist damping */
        break;

    case kMeFJointTypeAngular3:
        MeFJointSetProperty1b(fj, kMeFJointPropertySpecialBool1, 0); /* rotation enabled */
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, 1);
        break;

    case kMeFJointTypeSpring6:
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness3, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness4, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness5, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness6, 1000);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, 1);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping2, 1);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping3, 1);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping4, 1);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping5, 1);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping6, 1);
        break;
    }
}

/**
 * Sets the type of a MeFJoint.
 */
void MEAPI MeFJointSetType(MeFJoint *const fj, MeFJointType type)
{
    fj->type = type;
    MeFJointSetPerTypeDefaults(fj);
}

/**
 * Changes the name of an MeFJoint. Only needs to be called if renaming.
 */
void MEAPI MeFJointRename(MeFJoint *const fj, const char *const newName)
{
    if (strcmp(fj->id, newName) != 0)
    {
        if (fj->asset)
            MeHashDelete(fj->id, fj->asset->nameToJoint);

        _FSetStringProperty(&fj->id, newName);
        
        if (fj->asset)
            MeHashInsert(fj->id, fj, fj->asset->nameToJoint);
    }
}

/**
 * Set the joint's parts. Part can be zero when index is 1, meaning that the second body is the world.
 */
void MEAPI MeFJointSetPart(MeFJoint *const j, const MeFAssetPart *const part, int index)
{
    char *name = part ? part->id : 0;
    MeFJointSetPartByName(j, name, index);
}

/**
 * Sets the joints parts by name.
 */
void MEAPI MeFJointSetPartByName(MeFJoint *const j, const char *const part, int index)
{
    if (index == 0 && !part)
    {
        ME_REPORT(MeWarning(3,"Invalid part in joint '%s'", j->id));
        return;
    }
    
    _FSetStringProperty(&j->part[index], part);
}

/**
 * Sets the position of the joint. @param index determines which part's reference
 * frame it is in.
 */
void MEAPI MeFJointSetPosition(MeFJoint *const j, int index, MeReal x, MeReal y, MeReal z)
{
    MEASSERT(index >=0 && index < 2);
    MeVector3Set(j->pos[index], x, y, z);
}

/**
 * Sets the primary axis of the joint. @param index determines which part's reference
 * frame it is in.
 */
void MEAPI MeFJointSetPrimaryAxis(MeFJoint *const j, int index, MeReal x, MeReal y, MeReal z)
{
    MEASSERT(index >=0 && index < 2);
    MeVector3Set(j->pax[index], x, y, z);
}

/**
 * Sets the orthogonal axis of the joint. @param index determines which part's reference
 * frame it is in.
 */
void MEAPI MeFJointSetOrthogonalAxis(MeFJoint *const j, int index, MeReal x, MeReal y, MeReal z)
{
    MEASSERT(index >=0 && index < 2);
    MeVector3Set(j->oax[index], x, y, z);
}

/**
 * Scales the joint by the specified scale factor. Only scales the position and
 *  not limits/stiffness etc.
 */
void MEAPI MeFJointScale(MeFJoint *const j, MeReal scale)
{
    MeVector3Scale(j->pos[0], scale);
    MeVector3Scale(j->pos[1], scale);
    if (j->type == kMeFJointTypePrismatic) {
	j->stop[0] *= scale;
	j->stop[1] *= scale;
    }
}

void MEAPI MeFJointSetProperty1i(MeFJoint *const joint, MeFJointProperty p, int i)
{
    switch (p)
    {
    case kMeFJointPropertySpecialInt1:
        joint->special_i[0] = i;
        break;

    case kMeFJointPropertySpecialInt2:
        joint->special_i[1] = i;
        break;
    }
}

void MEAPI MeFJointSetProperty1ui(MeFJoint *const joint, MeFJointProperty p, unsigned int i)
{
}

void MEAPI MeFJointSetProperty1b(MeFJoint *const joint, MeFJointProperty p, MeBool b)
{
    switch (p)
    {
    case kMeFJointPropertyLimited1:
        joint->bLimited[0] = b;
        break;

    case kMeFJointPropertyMotorized1:
        joint->bMotorized[0] = b;
        break;

    case kMeFJointPropertySpecialBool1:
        joint->special_b[0] = b;
        break;
    }
}

void MEAPI MeFJointSetProperty1f(MeFJoint *const joint, MeFJointProperty p, MeReal x)
{
    switch (p)
    {
    case kMeFJointPropertyStop1:
        joint->stop[0] = x;
        break;

    case kMeFJointPropertyStop2:
        joint->stop[1] = x;
        break;

    case kMeFJointPropertyStop3:
        joint->stop[2] = x;
        break;

    case kMeFJointPropertyStiffness1:
        joint->stiff[0] = x;
        break;

    case kMeFJointPropertyStiffness2:
        joint->stiff[1] = x;
        break;

    case kMeFJointPropertyStiffness3:
        joint->stiff[2] = x;
        break;

    case kMeFJointPropertyStiffness4:
        joint->stiff[3] = x;
        break;

    case kMeFJointPropertyStiffness5:
        joint->stiff[4] = x;
        break;

    case kMeFJointPropertyStiffness6:
        joint->stiff[5] = x;
        break;
    
    case kMeFJointPropertyDamping1:
        joint->damp[0] = x;
        break;

    case kMeFJointPropertyDamping2:
        joint->damp[1] = x;
        break;

    case kMeFJointPropertyDamping3:
        joint->damp[2] = x;
        break;

    case kMeFJointPropertyDamping4:
        joint->damp[3] = x;
        break;

    case kMeFJointPropertyDamping5:
        joint->damp[4] = x;
        break;

    case kMeFJointPropertyDamping6:
        joint->damp[5] = x;
        break;
    
    case kMeFJointPropertyStrength1:
        joint->strength[0] = x;
        break;

    case kMeFJointPropertyStrength2:
        joint->strength[1] = x;
        break;
    
    case kMeFJointPropertyStrength3:
        joint->strength[2] = x;
        break;

    case kMeFJointPropertyStrength4:
        joint->strength[3] = x;
        break;

    case kMeFJointPropertyStrength5:
        joint->strength[4] = x;
        break;

    case kMeFJointPropertyStrength6:
        joint->strength[5] = x;
        break;

    case kMeFJointPropertyDesiredVelocity1:
        joint->desVel[0] = x;
        break;

    case kMeFJointPropertyDesiredVelocity2:
        joint->desVel[1] = x;
        break;

    case kMeFJointPropertySpecialFloat1:
        joint->special_f[0] = x;
        break;
    }
}


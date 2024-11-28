/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/22 15:03:28 $ - Revision: $Revision: 1.11.2.12 $

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
#include <MeAssetDBXMLIO.h>
#include "MeAssetDBXMLIO_1_0.h"
#include <MeMessage.h>
#include <MeMath.h>
#include <MeMemory.h>
#include <MdtDefaults.h>

/*************************************************************\
  INPUT FOR VERSION 1.0 OF THE FILE FORMAT 
\*************************************************************/

/** @internal */
MeFAsset *MEAPI KaFileCreate_1_0(MeAssetDB *parent, MeIDPool *IDPool, PElement *e)
{
    char *name, *graphic, *scale, *refPart;
    int id;
    MeFAsset *asset;
    PElementNode *node = e->childHead;
    PElement *childElem;

    name = PElementGetAttributeValue(e, ME_ATTR_ID);

    if (!name)
    {
        ME_REPORT(MeWarning(3,"Asset must have an id."));
        return 0;
    }
    
    id = IDPool ? MeIDPoolRequestID(IDPool) : 0;
    
    asset = MeFAssetCreate(name, id);

    while (node)
    {
        childElem = node->current;

        if (childElem->type == kPElementTypeGeometry)
        {
            MeFGeometry *geom = MeFGeometryCreateFromFile_1_0(asset, childElem);
            if (geom)
                MeFAssetInsertGeometry(asset, geom);
        }
        node = node->next;
    }

    node = e->childHead;

    while (node)
    {
        childElem = node->current;

        if (childElem->type == kPElementTypeModel)
        {
            MeFModel *model = MeFModelCreateFromFile_1_0(asset, childElem);
            if (model)
                MeFAssetInsertModel(asset, model);
        }
        node = node->next;
    }
    
    node = e->childHead;

    while (node)
    {
        childElem = node->current;

        if (childElem->type == kPElementTypeAssetPart)
        {
            MeFAssetPart *part = MeFAssetPartCreateFromFile_1_0(asset, childElem);
            if (part)
                MeFAssetInsertPart(asset, part);
        }
        node = node->next;
    }

    node = e->childHead;

    while (node)
    {
        childElem = node->current;

        if (childElem->type == kPElementTypeJoint)
        {
            MeFJoint *joint = MeFJointCreateFromFile_1_0(asset, childElem);
            if (joint)
                MeFAssetInsertJoint(asset, joint);
        }
        node = node->next;
    }
    
    node = e->childHead;

    while (node)
    {
        childElem = node->current;

        if (childElem->type == kPElementTypeNoCollision)
        {
            char *p1 = PElementGetAttributeValue(childElem, ME_ATTR_PART1);
            char *p2 = PElementGetAttributeValue(childElem, ME_ATTR_PART2);
            MeFAssetPart *part1 = MeFAssetLookupPart(asset, p1);
            MeFAssetPart *part2 = MeFAssetLookupPart(asset, p2);

            if (part1 && part2)
                MeFAssetPartEnableCollision(part1, part2, 0);
        }
        node = node->next;
    }
    
    graphic = PElementGetAttributeValue(e, ME_ATTR_GRAPHIC);

    if (graphic)
        MeFAssetSetGraphicHint(asset, graphic);
    
    scale = PElementGetAttributeValue(e, ME_ATTR_SCALE);
    
    if (scale)
    {
        MeReal s;
        s = (MeReal)strtod(scale,0);
        MeFAssetSetGraphicScale(asset, s);
    }
    
    refPart = PElementGetAttributeValue(e, ME_ATTR_REF_PART);

    if (refPart)
        MeFAssetSetReferencePart(asset, refPart);

    {
        char *massScale, *lengthScale;

        massScale = PElementGetAttributeValue(e, ME_ATTR_MASS_SCALE);
        if (massScale)
        {
            MeReal s = (MeReal)strtod(massScale, 0);
            MeFAssetSetMassScale(asset, s);
        }

        lengthScale = PElementGetAttributeValue(e, ME_ATTR_LENGTH_SCALE);
        if (lengthScale)
        {
            MeReal s = (MeReal)strtod(lengthScale, 0);
            MeFAssetSetLengthScale(asset, s);
        }
    }
    return asset;
}

/** @internal */
MeFGeometry *MEAPI MeFGeometryCreateFromFile_1_0(MeFAsset *parent, PElement *e)
{
    MeFGeometry *fg;
    char *name, *graphic, *scale, *offset;
    PElementNode *node = e->childHead;
    PElement *childElem;
    
    name = PElementGetAttributeValue(e, ME_ATTR_ID);

    fg = MeFGeometryCreate(name);

    while (node)
    {
        childElem = node->current;

        if (childElem->type == kPElementTypeGeometryPrimitive)
        {
            MeFPrimitive *prim = MeFPrimitiveCreateFromFile_1_0(fg, childElem);
            if (prim)
                MeFGeometryInsertPrimitive(fg, prim);
        }
        node = node->next;
    }
    
    graphic = PElementGetAttributeValue(e, ME_ATTR_GRAPHIC);

    if (graphic)
        MeFGeometrySetGraphicHint(fg, graphic);

    scale = PElementGetAttributeValue(e, ME_ATTR_SCALE);
    
    if (scale)
    {
        MeReal s;
        s = (MeReal)strtod(scale, 0);
        MeFGeometrySetGraphicScale(fg, s);
    }

    offset = PElementGetAttributeValue(e, ME_ATTR_GRAPHIC_OFFSET);
    if (offset)
    {
        MeVector3 off;
        char *c = offset, *d;
        int i = 0;
        while(*c && i < 3) 
        {
            off[i++] = (MeReal)strtod(c,&d);
            if(c == d || MeXMLParseComma(d, &c)) 
                ME_REPORT(MeWarning(0,"Invalid graphic offset on geometry '%s'", name));
        }
        MeFGeometrySetGraphicOffset(fg, off[0], off[1], off[2]);
    };

    return fg;
}

/** @internal */
MeFPrimitive *MEAPI MeFPrimitiveCreateFromFile_1_0(MeFGeometry *parent, PElement *e)
{
    MeFPrimitive *p = 0;
    PPrimitive *source = e->cdata;
    char *name, *type;
    
    name = PElementGetAttributeValue(e, ME_ATTR_ID);
    type = PElementGetAttributeValue(e, ME_ATTR_TYPE);

    if (!type)
    {
        ME_REPORT(MeWarning(3, "Primitive geometry type not specified for geometry '%s'. "
            "Primitive not created.", name));
        return 0;
    }

    if (strcmp(type, ME_GEOM_TYPE_SPHERE) == 0)
    {
        p = MeFPrimitiveCreate(name, kMeFPrimitiveTypeSphere);
        MeFPrimitiveSetRadius(p, source->dims[0]);
    }
    else if (strcmp(type, ME_GEOM_TYPE_BOX) == 0)
    {
        p = MeFPrimitiveCreate(name, kMeFPrimitiveTypeBox);
        MeFPrimitiveSetDimensions(p, source->dims[0], source->dims[1], source->dims[2]);
    }
    else if (strcmp(type, ME_GEOM_TYPE_CYLINDER) == 0)    
    {
        p = MeFPrimitiveCreate(name, kMeFPrimitiveTypeCylinder);
        MeFPrimitiveSetRadius(p, source->dims[0]);
        MeFPrimitiveSetHeight(p, source->dims[1]);
    }
    else if (strcmp(type, ME_GEOM_TYPE_SPHYL) == 0)    
    {
        p = MeFPrimitiveCreate(name, kMeFPrimitiveTypeSphyl);
        MeFPrimitiveSetRadius(p, source->dims[0]);
        MeFPrimitiveSetHeight(p, source->dims[1]);
    }
    else if (strcmp(type, ME_GEOM_TYPE_PLANE) == 0)
    {
        p = MeFPrimitiveCreate(name, kMeFPrimitiveTypePlane);
    }
    else if (strcmp(type, ME_GEOM_TYPE_CONVEX) == 0)
    {
        MeReal *v;
        int i;
        p = MeFPrimitiveCreate(name, kMeFPrimitiveTypeConvex);

        for (i = 0; i < source->nVertices; i++)
        {
            v = source->vertices[i];
            MeFPrimitiveAddVertex(p, v);
        }
    }

    if (p)
        MeFPrimitiveSetTransform(p, source->tm);

    return p;
}

/** @internal */
MeFModel *MEAPI MeFModelCreateFromFile_1_0(MeFAsset *parent, PElement *e)
{
    char *name, *geometryId, *type;
    MeFModel *fm;
    MeFModelType modelType = kMeFModelTypeDynamicsOnly;
    PElementNode *dynamics;
    MeFGeometry *fg = 0;

    name = PElementGetAttributeValue(e, ME_ATTR_ID);

    geometryId = PElementGetAttributeValue(e, ME_ATTR_GEOMETRY);
    
    if (geometryId)
        fg = MeFAssetLookupGeometry(parent, geometryId);

    dynamics = e->childHead;

    type = PElementGetAttributeValue(e, ME_ATTR_TYPE);

    // This is a backwards compatability hack. The new system uses a 'type' attribute,
    // but models without this type flag are still acceptable.
    if (type)
    {
        if (strcmp(type, "dynamics_only") == 0)
            modelType = kMeFModelTypeDynamicsOnly;
        else if (strcmp(type, "geometry_only") == 0)
            modelType = kMeFModelTypeGeometryOnly;
        else if (strcmp(type, "dynamics_and_geometry") == 0)
            modelType = kMeFModelTypeDynamicsAndGeometry;
    }
    else    /* early versions of file format don't have a model type attribute */
    {
        if (fg && dynamics)
            modelType = kMeFModelTypeDynamicsAndGeometry;
        else if (dynamics && !fg)
            modelType = kMeFModelTypeDynamicsOnly;
        else if (fg && !dynamics)
            modelType = kMeFModelTypeGeometryOnly;
        else
        {
            ME_REPORT(MeWarning(0, "Couldn't create model '%s' because it had no dynamics "
                "or an invalid geometry", name));
            return 0;
        }
    }
    
    fm = MeFModelCreate(name, modelType);

    if (dynamics)
    {
        PDynamics *dyn;
        dyn = dynamics->current->cdata;

        MeFModelSetMassOffset(fm, dyn->mass_offset);

        MeFModelSetMass(fm, dyn->mass);

        MeFModelSetDensity(fm, dyn->density);
        {
            MeMatrix3 I;
            I[0][0] = dyn->inertia[0]; I[1][1] = dyn->inertia[3]; I[2][2] = dyn->inertia[5]; /* diagonal */
            I[1][0] = I[0][1] = dyn->inertia[1];
            I[2][0] = I[0][2] = dyn->inertia[2];
            I[2][1] = I[1][2] = dyn->inertia[4];
    
            MeFModelSetInertiaTensor(fm, I);
        }

        MeFModelSetLinearVelocityDamping(fm, dyn->linearDamp);
        MeFModelSetAngularVelocityDamping(fm, dyn->angularDamp);
        MeFModelSetFastSpinAxis(fm, dyn->fastSpin[0], dyn->fastSpin[1], dyn->fastSpin[2]);
        MeFModelEnableFastSpinAxis(fm, dyn->useFastSpin);
    }
    
    if (fg)
        MeFModelSetGeometry(fm, fg);
    
    return fm;
}

/** @internal */
MeFAssetPart *MEAPI MeFAssetPartCreateFromFile_1_0(MeFAsset *asset, PElement *e)
{
    char *name, *modelId, *graphic, *scale, *parent, *offset;
    PAssetPart *data = e->cdata;
    MeFAssetPart *part;
    MeFModel *model;
    
    name = PElementGetAttributeValue(e, ME_ATTR_ID);
    modelId = PElementGetAttributeValue(e, ME_ATTR_MODEL);

    model = MeFAssetLookupModel(asset, modelId);
    
    if (!model)
    {
        ME_REPORT(MeWarning(0, "Couldn't create part '%s' due to an invalid model", name));
        return 0;
    } 

    part = MeFAssetPartCreate(name, model, data->tm);

    graphic = PElementGetAttributeValue(e, ME_ATTR_GRAPHIC);

    if (graphic)
        MeFAssetPartSetGraphicHint(part, graphic);
    
    scale = PElementGetAttributeValue(e, ME_ATTR_SCALE);
    
    if (scale)
    {
        MeReal s;
        s = (MeReal)strtod(scale,0);
        MeFAssetPartSetGraphicScale(part, s);
    }

    parent = PElementGetAttributeValue(e, ME_ATTR_PARENT);

    if (parent)
        MeFAssetPartSetParentPartByName(part, parent);
    
    offset = PElementGetAttributeValue(e, ME_ATTR_GRAPHIC_OFFSET);
    if (offset)
    {
        MeVector3 off;
        char *c = offset, *d;
        int i = 0;
        while(*c && i < 3) 
        {
            off[i++] = (MeReal)strtod(c,&d);
            if(c == d || MeXMLParseComma(d, &c)) 
                ME_REPORT(MeWarning(0,"Invalid graphic offset on geometry '%s'", name));
        }
        MeFAssetPartSetGraphicOffset(part, off[0], off[1], off[2]);
    };

    return part;
}

/** @internal */
MeFJoint *MEAPI MeFJointCreateFromFile_1_0(MeFAsset *asset, PElement *e)
{
    MeFJoint *fj = 0;
    PJoint *source;
    char *name, *type, *part1, *part2;
    int i;
    
    source = e->cdata;

    name = PElementGetAttributeValue(e, ME_ATTR_ID);
    type = PElementGetAttributeValue(e, ME_ATTR_TYPE);

    if (!type)
    {
        ME_REPORT(MeWarning(3, "Joint type not specified for joint '%s'. "
            "Joint not created.",name));
        return 0;
    }

    part1 = PElementGetAttributeValue(e, ME_ATTR_PART1);

    if (!part1)
    {
        ME_REPORT(MeWarning(3, "Joint part1 not specified for joint '%s'. "
            "Joint not created.",name));
        return 0;
    }
    
    if (strcmp(type, ME_JOINT_TYPE_BALLANDSOCKET) == 0)
    {
        fj = MeFJointCreate(name, kMeFJointTypeBallAndSocket);
    }
    else if (strcmp(type, ME_JOINT_TYPE_HINGE) == 0)
    {
        fj = MeFJointCreate(name, kMeFJointTypeHinge);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, source->stop[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, source->stop[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, source->stiff[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, source->stiff[1]);
        MeFJointSetProperty1b(fj, kMeFJointPropertyLimited1, source->bLimited[0]);
        MeFJointSetProperty1b(fj, kMeFJointPropertyMotorized1, source->bMotorized[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity1, source->desVel[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, source->strength[0]);
    }
    else if (strcmp(type, ME_JOINT_TYPE_CARWHEEL) == 0)        
    {
        fj = MeFJointCreate(name, kMeFJointTypeCarwheel);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity1, source->desVel[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, source->strength[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity2, source->desVel[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength2, source->strength[1]);
        MeFJointSetProperty1b(fj, kMeFJointPropertySpecialBool1, source->special_b[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, source->damp[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, source->stop[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, source->stop[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertySpecialFloat1, source->special_f[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, source->stiff[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, source->stiff[1]);
    }
    else if (strcmp(type, ME_JOINT_TYPE_CONELIMIT) == 0)
    {
        fj = MeFJointCreate(name, kMeFJointTypeConeLimit);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, source->stop[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, source->stiff[0]);
    }
    else if (strcmp(type, ME_JOINT_TYPE_UNIVERSAL) == 0)        
    {
        fj = MeFJointCreate(name, kMeFJointTypeUniversal);
    }
    else if (strcmp(type, ME_JOINT_TYPE_RPRO) == 0)        
    {
        fj = MeFJointCreate(name, kMeFJointTypeRpro);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, source->strength[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength2, source->strength[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength3, source->strength[2]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength4, source->strength[3]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength5, source->strength[4]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength6, source->strength[5]);
    }
    else if (strcmp(type, ME_JOINT_TYPE_PRISMATIC) == 0)        
    {
        fj = MeFJointCreate(name, kMeFJointTypePrismatic);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, source->stop[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, source->stop[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, source->stiff[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, source->stiff[1]);
        MeFJointSetProperty1b(fj, kMeFJointPropertyLimited1, source->bLimited[0]);
        MeFJointSetProperty1b(fj, kMeFJointPropertyMotorized1, source->bMotorized[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDesiredVelocity1, source->desVel[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStrength1, source->strength[0]);
    }
    else if (strcmp(type, ME_JOINT_TYPE_SKELETAL) == 0)
    {
        fj = MeFJointCreate(name, kMeFJointTypeSkeletal);
        MeFJointSetProperty1i(fj, kMeFJointPropertySpecialInt1, source->special_i[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop1, source->stop[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop2, source->stop[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, source->stiff[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, source->damp[0]);
        MeFJointSetProperty1i(fj, kMeFJointPropertySpecialInt2, source->special_i[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStop3, source->stop[2]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, source->stiff[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping2, source->damp[1]);
        MeFJointSetProperty1b(fj, kMeFJointPropertySpecialBool1, source->special_b[0]);
    }
    else if (strcmp(type, ME_JOINT_TYPE_ANGULAR3) == 0)
    {
        fj = MeFJointCreate(name, kMeFJointTypeAngular3);
        MeFJointSetProperty1b(fj, kMeFJointPropertySpecialBool1, source->special_b[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, source->stiff[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, source->damp[0]);
    }
    else if (strcmp(type, ME_JOINT_TYPE_SPRING6) == 0)
    {
        fj = MeFJointCreate(name, kMeFJointTypeSpring6);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness1, source->stiff[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness2, source->stiff[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness3, source->stiff[2]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness4, source->stiff[3]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness5, source->stiff[4]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyStiffness6, source->stiff[5]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping1, source->damp[0]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping2, source->damp[1]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping3, source->damp[2]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping4, source->damp[3]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping5, source->damp[4]);
        MeFJointSetProperty1f(fj, kMeFJointPropertyDamping6, source->damp[5]);

    }
    
    if (fj)
    {
        for (i = 0; i < 2; i++)
        {
            MeFJointSetPosition(fj, i, source->pos[i][0], source->pos[i][1], source->pos[i][2]);
            MeFJointSetPrimaryAxis(fj, i, source->pax[i][0], source->pax[i][1], source->pax[i][2]);
            MeFJointSetOrthogonalAxis(fj, i, source->oax[i][0], source->oax[i][1], source->oax[i][2]);
        }

        part2 = PElementGetAttributeValue(e, ME_ATTR_PART2);
    
        MeFJointSetPartByName(fj, part1, 0);
    
        if (part2)
            MeFJointSetPartByName(fj, part2, 1);
    }

    return fj;
}

static MeXMLError MEAPI AddVertex_1_0(MeXMLElement *elem,
    void *prim, void *parent)
{
    PPrimitive *cx = prim;

    MeVector3Copy(cx->vertices[cx->nVertices], cx->tempVertex);

    cx->nVertices++;
    if (cx->nVertices >= cx->maxVertices)
    {
        cx->maxVertices *= 2;
        cx->vertices = (MeVector3*)MeMemoryAPI.resize(cx->vertices,
            sizeof(MeVector3) * cx->maxVertices);
    }

    return MeXMLErrorNone;
}

static void MEAPI FreePrimitive(void *const m)
{
    PPrimitive *p = m;
    MeMemoryAPI.destroy(p->vertices);
    MeMemoryAPI.destroy(p);
}

static MeXMLError MEAPI Handle_GeometryPrimitive_1_0(
    MeXMLElement *elem, PElement *parent)
{
    PPrimitive *data;
    PElement *e;    
    
    MeXMLHandler handlers[] = 
    {
        ME_XML_MEREAL_HANDLER(ME_ELEM_GEOMETRY_RADIUS, PPrimitive, dims[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_GEOMETRY_HEIGHT, PPrimitive, dims[1], 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_GEOMETRY_DIMS,
            PPrimitive, dims, 3, 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_GEOMETRY_VERTEX,
            PPrimitive, tempVertex, 3, AddVertex_1_0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_GEOMETRY_TM, PPrimitive, tm, 16, 0),
        ME_XML_HANDLER_END
    };
    
    data = (PPrimitive*)MeMemoryAPI.createZeroed(sizeof(PPrimitive));
    data->nVertices = 0;
    data->maxVertices = 1;
    data->vertices = (MeVector3*)MeMemoryAPI.create(
        sizeof(MeVector3) * data->maxVertices);
    e = PElementCreate(kPElementTypeGeometryPrimitive,
        elem->name, data, FreePrimitive, elem->attr);
    PElementInsert(e, parent);

    return MeXMLElementProcess(elem, handlers, data, e);
}

static MeXMLError MEAPI Handle_Geometry_1_0(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_ELEMENT_HANDLER(ME_ELEM_GEOMETRY_PART, Handle_GeometryPrimitive_1_0),        
        ME_XML_HANDLER_END
    };

    PElement *e = PElementCreate(kPElementTypeGeometry, elem->name, 0, 0, elem->attr);
    PElementInsert(e, parent);

    return MeXMLElementProcess(elem, handlers, 0, e);
}

static MeXMLError MEAPI Handle_Dynamics_1_0(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_DYNAMICS_MASS_OFFSET, PDynamics, mass_offset, 3, 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_DYNAMICS_MASS, PDynamics, mass, 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_DYNAMICS_DENSITY, PDynamics, density, 0),
        ME_XML_MEREAL_ARRAY_HANDLER (ME_ELEM_DYNAMICS_INERTIA, PDynamics, inertia[0], 6, 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_DYNAMICS_LIN_DAMP, PDynamics, linearDamp, 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_DYNAMICS_ANG_DAMP, PDynamics, angularDamp, 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_DYNAMICS_FAST_SPIN, PDynamics, fastSpin, 3, 0),
        ME_XML_INT_HANDLER(ME_ELEM_DYNAMICS_USE_FAST_SPIN, PDynamics, useFastSpin, 0),
        ME_XML_HANDLER_END
    };
    
    PDynamics *data = (PDynamics*)MeMemoryAPI.createZeroed(sizeof(PDynamics));
    PElement *e = PElementCreate(kPElementTypeDynamics, elem->name, data, MeMemoryAPI.destroy, elem->attr);
    PElementInsert(e, parent);

    /* sensible defaults */
    data->density = 1;
    data->mass = MDTBODY_DEFAULT_MASS;
    data->angularDamp = MDTBODY_DEFAULT_ANGULAR_DAMPING;
    data->linearDamp = MDTBODY_DEFAULT_LINEAR_DAMPING;
    data->fastSpin[0] = MDTBODY_DEFAULT_FAST_SPIN_X;
    data->fastSpin[1] = MDTBODY_DEFAULT_FAST_SPIN_Y;
    data->fastSpin[2] = MDTBODY_DEFAULT_FAST_SPIN_Z;
    data->useFastSpin = 0;
    data->inertia[0] = MDTBODY_DEFAULT_INERTIA;
    data->inertia[3] = MDTBODY_DEFAULT_INERTIA;
    data->inertia[5] = MDTBODY_DEFAULT_INERTIA;


    return MeXMLElementProcess(elem, handlers, data, e);
}

static MeXMLError MEAPI Handle_Model_1_0(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_ELEMENT_HANDLER(ME_ELEM_DYNAMICS, Handle_Dynamics_1_0),        
        ME_XML_HANDLER_END
    };
    
    PElement *e = PElementCreate(kPElementTypeModel, elem->name, 0, 0, elem->attr);
    PElementInsert(e, parent);

    return MeXMLElementProcess(elem, handlers, 0, e);
}

static MeXMLError MEAPI Handle_Joint_1_0(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        /* common to all joints */
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_JOINT_POS1, PJoint, pos[0], 3, 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_JOINT_POS2, PJoint, pos[1], 3, 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_JOINT_PRIMARY_AXIS1, PJoint, pax[0], 3, 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_JOINT_PRIMARY_AXIS2, PJoint, pax[1], 3, 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_JOINT_ORTHOGONAL_AXIS1, PJoint, oax[0], 3, 0),
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_JOINT_ORTHOGONAL_AXIS2, PJoint, oax[1], 3, 0),

        /* carwheel */
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_H_MAXFORCE, PJoint, strength[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_H_DESVEL, PJoint, desVel[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_S_MAXFORCE, PJoint, strength[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_S_DESVEL, PJoint, desVel[1], 0),
        ME_XML_INT_HANDLER(ME_ELEM_JOINT_S_LOCK, PJoint, special_b[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_SUSP_HIGH, PJoint, stop[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_SUSP_LOW, PJoint, stop[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_SUSP_STIFF, PJoint, stiff[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_SUSP_SOFT, PJoint, stiff[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_SUSP_DAMP, PJoint, damp[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_SUSP_REF, PJoint, special_f[0], 0),

        /* hinge/prismatic */
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_HIGH_LIMIT, PJoint, stop[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LOW_LIMIT, PJoint, stop[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_HIGH_STIFF, PJoint, stiff[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LOW_STIFF, PJoint, stiff[1], 0),
        ME_XML_INT_HANDLER(ME_ELEM_JOINT_LIMITED, PJoint, bLimited[0], 0),
        ME_XML_INT_HANDLER(ME_ELEM_JOINT_MOTORIZED, PJoint, bMotorized[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_DES_VEL, PJoint, desVel[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_MAX_FORCE, PJoint, strength[0], 0),

        /* rpro */
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STRENGTH_L0, PJoint, strength[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STRENGTH_L1, PJoint, strength[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STRENGTH_L2, PJoint, strength[2], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STRENGTH_A0, PJoint, strength[3], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STRENGTH_A1, PJoint, strength[4], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STRENGTH_A2, PJoint, strength[5], 0),

        /* cone limit */
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_HALF_ANGLE, PJoint, stop[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STIFFNESS, PJoint, stiff[0], 0),

        /* skeletal */
        ME_XML_INT_HANDLER(ME_ELEM_JOINT_CONETYPE, PJoint, special_i[0], 0),          
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_CONE_HALF_ANGLE_X, PJoint, stop[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_CONE_HALF_ANGLE_Y, PJoint, stop[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_CONE_STIFF, PJoint, stiff[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_CONE_DAMP, PJoint, damp[0], 0),
        ME_XML_INT_HANDLER(ME_ELEM_JOINT_TWISTTYPE, PJoint, special_i[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_TWIST_HALF_ANGLE, PJoint, stop[2], 0), 
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_TWIST_STIFF, PJoint, stiff[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_TWIST_DAMP, PJoint, damp[1], 0),

        /* angular3 */
        ME_XML_INT_HANDLER(ME_ELEM_JOINT_ROTATION_ENABLED, PJoint, special_b[0], 0),          
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_STIFFNESS, PJoint, stiff[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_DAMPING, PJoint, damp[0], 0),

        /* spring6 */
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LINEAR_STIFF_X, PJoint, stiff[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LINEAR_STIFF_Y, PJoint, stiff[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LINEAR_STIFF_Z, PJoint, stiff[2], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LINEAR_DAMP_X, PJoint, damp[0], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LINEAR_DAMP_Y, PJoint, damp[1], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_LINEAR_DAMP_Z, PJoint, damp[2], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_ANGULAR_STIFF_X, PJoint, stiff[3], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_ANGULAR_STIFF_Y, PJoint, stiff[4], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_ANGULAR_STIFF_Z, PJoint, stiff[5], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_ANGULAR_DAMP_X, PJoint, damp[3], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_ANGULAR_DAMP_Y, PJoint, damp[4], 0),
        ME_XML_MEREAL_HANDLER(ME_ELEM_JOINT_ANGULAR_DAMP_Z, PJoint, damp[5], 0),

        ME_XML_HANDLER_END
    };
        
    PJoint *data = (PJoint*)MeMemoryAPI.createZeroed(sizeof(PJoint));
    PElement *e = PElementCreate(kPElementTypeJoint,
        elem->name, data, MeMemoryAPI.destroy, elem->attr);
    PElementInsert(e, parent);

    return MeXMLElementProcess(elem, handlers, data, e);
}

static MeXMLError MEAPI Handle_AssetPart_1_0(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_MEREAL_ARRAY_HANDLER(ME_ELEM_ASSET_PART_TM, PAssetPart, tm, 16, 0),
        ME_XML_HANDLER_END
    };
    
    PAssetPart *data = (PAssetPart*)
        MeMemoryAPI.createZeroed(sizeof(PAssetPart));
    PElement *e = PElementCreate(kPElementTypeAssetPart,
        elem->name, data, MeMemoryAPI.destroy, elem->attr);
    PElementInsert(e, parent);

    return MeXMLElementProcess(elem, handlers, data, e);
}

static MeXMLError MEAPI Handle_NoCollision_1_0(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_HANDLER_END
    };
    
    PElement *e = PElementCreate(kPElementTypeNoCollision, elem->name, 0, 0, elem->attr);
    PElementInsert(e, parent);

    return MeXMLElementProcess(elem, handlers, 0, e);
}

MeXMLError MEAPI Handle_Asset_1_0(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_ELEMENT_HANDLER(ME_ELEM_GEOMETRY, Handle_Geometry_1_0),
        ME_XML_ELEMENT_HANDLER(ME_ELEM_MODEL, Handle_Model_1_0),
        ME_XML_ELEMENT_HANDLER(ME_ELEM_JOINT, Handle_Joint_1_0),
        ME_XML_ELEMENT_HANDLER(ME_ELEM_ASSET_PART, Handle_AssetPart_1_0),
        ME_XML_ELEMENT_HANDLER(ME_ELEM_NO_COLLISION, Handle_NoCollision_1_0),
        ME_XML_HANDLER_END
    };

    MeAssetDBXMLInput *input = MeXMLInputGetUserData(MeXMLElementGetInput(elem));

    PElement *e = PElementCreate(kPElementTypeAsset, elem->name, 0, 0, elem->attr);

    PElementInsert(e, parent);

    return MeXMLElementProcess(elem, handlers, 0, e);
}

MeXMLError MEAPI Handle_KaFile_0_1(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_ELEMENT_HANDLER(ME_ELEM_ASSET, Handle_Asset_1_0),
        ME_XML_HANDLER_END
    };

    return MeXMLElementProcess(elem, handlers, 0, parent);
}


/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 12:29:56 $ - Revision: $Revision: 1.8.2.9 $

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
#include <MeAssetDBXMLIOTypes.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeMath.h>
#include <MeXMLParser.h>

/*************************************************************\
  OUTPUT FOR VERSION 1.0 OF THE FILE FORMAT 
\*************************************************************/

/** @internal */
MeXMLElementID MEAPI KaFileWriteXML_1_0(MeXMLOutput *op, MeFAsset *fa, MeXMLElementID parent)
{
    char buffer[1024];
    char *pBuf = buffer;
    char *p;
    MeXMLElementID asset;
    
    pBuf += sprintf(pBuf, ME_ELEM_ASSET" "ME_ATTR_ID"=\"%s\"", MeFAssetGetName(fa));

    if (p = MeFAssetGetReferencePart(fa))
        pBuf += sprintf(pBuf, " "ME_ATTR_REF_PART"=\"%s\"", p);

    if (p = MeFAssetGetGraphicHint(fa))
        pBuf += sprintf(pBuf, " "ME_ATTR_GRAPHIC"=\"%s\" "ME_ATTR_SCALE"=\"%.7g\"", 
        p, MeFAssetGetGraphicScale(fa));

    pBuf += sprintf(pBuf, " "ME_ATTR_MASS_SCALE"=\"%.7g\" "ME_ATTR_LENGTH_SCALE"=\"%.7g\"",
        MeFAssetGetMassScale(fa), MeFAssetGetLengthScale(fa));

    asset = MeXMLWriteElement(op, parent, buffer);

    {
		int i;
		int count = MeFAssetGetGeometryCount(fa);
		MeFGeometry **geomArray = (MeFGeometry**)MeMemoryALLOCA(sizeof(MeFGeometry*) * count);
		MeFAssetGetGeometrySortedByName(fa, geomArray);
        
		for (i = 0; i < count; i++)
            MeFGeometryWriteXML_1_0(op, geomArray[i], asset);
    }

    {
        int i;
		int count = MeFAssetGetModelCount(fa);
		MeFModel **modelArray = (MeFModel**)MeMemoryALLOCA(sizeof(MeFModel*) * count);
        MeFAssetGetModelsSortedByName(fa, modelArray);
    
        for (i = 0; i < count; i++)
            MeFModelWriteXML_1_0(op, modelArray[i], asset);
    }
    
    {
        int i;
        int count = MeFAssetGetPartCount(fa);
		MeFAssetPart **partArray = (MeFAssetPart**)MeMemoryALLOCA(sizeof(MeFAssetPart*) * count);
        MeFAssetGetPartsSortedByName(fa, partArray);
    
        for (i = 0; i < count; i++)
            MeFAssetPartWriteXML_1_0(op, partArray[i], asset);
        
        for (i = 0; i < count; i++)
        {
            int j;
			for (j = i + 1; j < count; j++)
            {
                if (!MeFAssetPartIsCollisionEnabled(partArray[i], partArray[j]))
				{
					pBuf = buffer;
					pBuf += sprintf(pBuf, ME_ELEM_NO_COLLISION" "ME_ATTR_PART1"=\"%s\" "ME_ATTR_PART2"=\"%s\"", 
						MeFAssetPartGetName(partArray[i]), MeFAssetPartGetName(partArray[j]));
					MeXMLWriteElement(op, asset, buffer);
				}
            }
        }
 
    }

    {
        int i;
        int count = MeFAssetGetJointCount(fa);
		MeFJoint **jointArray = (MeFJoint**)MeMemoryALLOCA(sizeof(MeFJoint*) * count);
        MeFAssetGetJointsSortedByName(fa, jointArray);

        for (i = 0; i < count; i++)
            MeFJointWriteXML_1_0(op, jointArray[i], asset);
    }

    return asset;
}

/** @interal */
static MeXMLElementID MEAPI MeFGeometryWriteXML_1_0(MeXMLOutput *op, MeFGeometry *fg, MeXMLElementID parent)
{
    char buffer[1024];
    char *pBuf = buffer;
    char *p;
    MeXMLElementID elem;

    pBuf = buffer;

    pBuf += sprintf(pBuf, ME_ELEM_GEOMETRY" "ME_ATTR_ID"=\"%s\"", MeFGeometryGetName(fg));

    if (p = MeFGeometryGetGraphicHint(fg))
    {
        MeVector3 v;
        MeFGeometryGetGraphicOffset(fg, v);
        pBuf += sprintf(pBuf, " "ME_ATTR_GRAPHIC"=\"%s\" "ME_ATTR_SCALE"=\"%.7g\" "ME_ATTR_GRAPHIC_OFFSET"=\"%.7g,%.7g,%.7g\"", 
        p, MeFGeometryGetGraphicScale(fg), v[0], v[1], v[2]);
    }

    elem = MeXMLWriteElement(op, parent, buffer);

    {
        MeFPrimitiveIt it;
        MeFPrimitive *prim;

        MeFGeometryInitPrimitiveIterator(fg, &it);
    
        while(prim = MeFGeometryGetPrimitive(&it))
            MeFPrimitiveWriteXML_1_0(op, prim, elem);
    }
    return elem;
}

/** @internal */
static void MEAPI SphereWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *p, MeXMLElementID parent)
{
    MeXMLWriteElement(op, parent, ME_ELEM_GEOMETRY_RADIUS);
    MeXMLWritePCDATA(op, "%.7g", MeFPrimitiveGetRadius(p));
}

/** @internal */
static void MEAPI BoxWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *p, MeXMLElementID parent)
{
    MeVector3 v;
    MeFPrimitiveGetDimensions(p, v);
    MeXMLWriteElement(op, parent, ME_ELEM_GEOMETRY_DIMS);
    MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", v[0], v[1], v[2]);
}

/** @internal */
static void MEAPI CylinderWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *p, MeXMLElementID parent)
{
    MeXMLWriteElement(op, parent, ME_ELEM_GEOMETRY_RADIUS);
    MeXMLWritePCDATA(op, "%.7g", MeFPrimitiveGetRadius(p));
    MeXMLWriteElement(op, parent, ME_ELEM_GEOMETRY_HEIGHT);
    MeXMLWritePCDATA(op, "%.7g", MeFPrimitiveGetHeight(p));
}

/** @internal */
static void MEAPI SphylWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *p, MeXMLElementID parent)
{
    MeXMLWriteElement(op, parent, ME_ELEM_GEOMETRY_RADIUS);
    MeXMLWritePCDATA(op, "%.7g", MeFPrimitiveGetRadius(p));
    MeXMLWriteElement(op, parent, ME_ELEM_GEOMETRY_HEIGHT);
    MeXMLWritePCDATA(op, "%.7g", MeFPrimitiveGetHeight(p));
}

/** @internal */
static void MEAPI PlaneWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *p, MeXMLElementID parent)
{
}

/** @internal */
static void MEAPI ConvexWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *p, MeXMLElementID parent)
{
    MeFVertexIt it;
    MeVector3Ptr v;
    
    if (MeFPrimitiveGetVertexCount(p) < 1)
    {
        ME_REPORT(MeWarning(3,"Writing convex geometry '%s' with "
            "no elements.", p->id));
    }
    
    MeFPrimitiveInitVertexIterator(p, &it);

    while (v = MeFPrimitiveGetVertex(&it))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_GEOMETRY_VERTEX);
        MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g",
            v[0], v[1], v[2] );
    }
}

/** @interal */
static MeXMLElementID MEAPI MeFPrimitiveWriteXML_1_0(MeXMLOutput *op, MeFPrimitive *prim, MeXMLElementID parent)
{
    MeXMLElementID part;
    char buffer[1024];
    char *pBuf = buffer;
    MeMatrix4Ptr tm;
    
    pBuf += sprintf(pBuf, ME_ELEM_GEOMETRY_PART" "ME_ATTR_ID"=\"%s\"", MeFPrimitiveGetName(prim));

    switch (prim->type)
    {
    case kMeFPrimitiveTypeSphere:
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_GEOM_TYPE_SPHERE"\"");
        part = MeXMLWriteElement(op, parent, buffer);
        SphereWriteXML_1_0(op, prim, part);
        break;

    case kMeFPrimitiveTypeBox:
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_GEOM_TYPE_BOX"\"");
        part = MeXMLWriteElement(op, parent, buffer);
        BoxWriteXML_1_0(op, prim, part);
        break;

    case kMeFPrimitiveTypeCylinder:
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_GEOM_TYPE_CYLINDER"\"");
        part = MeXMLWriteElement(op, parent, buffer);
        CylinderWriteXML_1_0(op, prim, part);
        break;

    case kMeFPrimitiveTypeSphyl:
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_GEOM_TYPE_SPHYL"\"");
        part = MeXMLWriteElement(op, parent, buffer);
        SphylWriteXML_1_0(op, prim, part);
        break;
    
    case kMeFPrimitiveTypePlane:
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_GEOM_TYPE_PLANE"\"");
        part = MeXMLWriteElement(op, parent, buffer);
        PlaneWriteXML_1_0(op, prim, part);
        break;

    case kMeFPrimitiveTypeConvex:
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_GEOM_TYPE_CONVEX"\"");
        part = MeXMLWriteElement(op, parent, buffer);
        ConvexWriteXML_1_0(op, prim, part);
        break;
    }

    MeXMLWriteElement(op, part, ME_ELEM_GEOMETRY_TM);

    tm = MeFPrimitiveGetTransformPtr(prim);

    MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,"
        "%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g",
        tm[0][0],tm[0][1],tm[0][2],tm[0][3],tm[1][0],tm[1][1],
        tm[1][2],tm[1][3],tm[2][0],tm[2][1],tm[2][2],tm[2][3],
        tm[3][0],tm[3][1],tm[3][2],tm[3][3]);
    
    return part;
}

/** @internal */    
static MeXMLElementID MEAPI MeFModelWriteXML_1_0(MeXMLOutput *op, MeFModel *fm, MeXMLElementID parent)
{
    MeXMLElementID model;
    
    char buffer[1024];
    char *pBuf = buffer;
    char *p;
	MeFModelType type = MeFModelGetType(fm);

    pBuf += sprintf(pBuf, ME_ELEM_MODEL" "ME_ATTR_ID"=\"%s\"", MeFModelGetName(fm));
    
	if (type == kMeFModelTypeDynamicsOnly)
        pBuf += sprintf(pBuf," "ME_ATTR_TYPE"=\"dynamics_only\"");
	else if (type == kMeFModelTypeGeometryOnly)
        pBuf += sprintf(pBuf," "ME_ATTR_TYPE"=\"geometry_only\"");
	else
        pBuf += sprintf(pBuf," "ME_ATTR_TYPE"=\"dynamics_and_geometry\"");

    if (p = MeFModelGetGeometryName(fm))
        pBuf += sprintf(pBuf," "ME_ATTR_GEOMETRY"=\"%s\"", p);

    model = MeXMLWriteElement(op, parent, buffer);

    if (fm->type == kMeFModelTypeDynamicsAndGeometry || fm->type == kMeFModelTypeDynamicsOnly)
    {
        MeXMLElementID dynamics;
        dynamics = MeXMLWriteElement(op, model, ME_ELEM_DYNAMICS);

        MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_MASS);
        MeXMLWritePCDATA(op, "%.7g", MeFModelGetMass(fm));

        MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_DENSITY);
        MeXMLWritePCDATA(op, "%.7g", MeFModelGetDensity(fm));

        MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_MASS_OFFSET);
        {
            MeVector3 offset;
            MeFModelGetMassOffset(fm, offset);
            MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g", offset[0], offset[1], offset[2]);
        }
        {
            MeMatrix3 I;
            MeFModelGetInertiaTensor(fm, I);
            MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_INERTIA);
            MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g,%.7g,%.7g,%.7g",
                    I[0][0], I[1][0], I[2][0], I[1][1], I[2][1], I[2][2]);
        }

        MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_LIN_DAMP);
        MeXMLWritePCDATA(op, "%.7g", MeFModelGetLinearVelocityDamping(fm));

        MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_ANG_DAMP);
        MeXMLWritePCDATA(op, "%.7g", MeFModelGetAngularVelocityDamping(fm));
    
        {
            MeVector3 fast;
            MeFModelGetFastSpinAxis(fm, fast);
            MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_FAST_SPIN);
            MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", fast[0], fast[1], fast[2]);

			MeXMLWriteElement(op, dynamics, ME_ELEM_DYNAMICS_USE_FAST_SPIN);
			MeXMLWritePCDATA(op, "%d", MeFModelIsFastSpinAxisEnabled(fm));
        }
    }
    return model;
}

/** @interal */
static MeXMLElementID MEAPI MeFAssetPartWriteXML_1_0(MeXMLOutput *op, MeFAssetPart *part, MeXMLElementID parent)
{
    char buffer[1024];
    char *pBuf = buffer;
    char *p;
    MeXMLElementID elem;
    MeMatrix4Ptr tm;

    pBuf = buffer;

    pBuf += sprintf(pBuf, ME_ELEM_ASSET_PART" "ME_ATTR_ID"=\"%s\" "ME_ATTR_MODEL"=\"%s\"",
        MeFAssetPartGetName(part), MeFAssetPartGetModelName(part));

    if (p = MeFAssetPartGetGraphicHint(part))
    {
        MeVector3 v;
        MeFAssetPartGetGraphicOffset(part, v);
        
        pBuf += sprintf(pBuf, " "ME_ATTR_GRAPHIC"=\"%s\" "ME_ATTR_SCALE"=\"%.7g\" "ME_ATTR_GRAPHIC_OFFSET"=\"%.7g,%.7g,%.7g\"",  
        p, MeFAssetPartGetGraphicScale(part), v[0], v[1], v[2]);
    }

    if (p = MeFAssetPartGetParentPartName(part))
        pBuf += sprintf(pBuf, " "ME_ATTR_PARENT"=\"%s\"", p);

    elem = MeXMLWriteElement(op, parent, buffer);

    MeXMLWriteElement(op, elem, ME_ELEM_ASSET_PART_TM);

    tm = MeFAssetPartGetTransformPtr(part);
    MeXMLWritePCDATA(op,"%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,"
        "%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g",
        tm[0][0],tm[0][1],tm[0][2],tm[0][3],tm[1][0],tm[1][1],
        tm[1][2],tm[1][3],tm[2][0],tm[2][1],tm[2][2],tm[2][3],
        tm[3][0],tm[3][1],tm[3][2],tm[3][3]);

    return elem;
}

/** @internal */
static void MEAPI BallAndSocketWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
}

/** @internal */
static void MEAPI UniversalWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
}

/** @internal */
static void MEAPI RproWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;
    
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STRENGTH_L0);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STRENGTH_L1);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength3, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STRENGTH_L2);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength4, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STRENGTH_A0);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength5, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STRENGTH_A1);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength6, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STRENGTH_A2);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */
static void MEAPI HingeWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;
    MeBool b = 0;

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_HIGH_LIMIT);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LOW_LIMIT);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_HIGH_STIFF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LOW_STIFF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1b(j, kMeFJointPropertyLimited1, &b))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LIMITED);
        MeXMLWritePCDATA(op, "%d", b);
    }

    if (MeFJointGetProperty1b(j, kMeFJointPropertyMotorized1, &b))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_MOTORIZED);
        MeXMLWritePCDATA(op, "%d", b);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDesiredVelocity1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_DES_VEL);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_MAX_FORCE);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */
static void MEAPI CarWheelWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;
    MeBool b = 0;
    
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_H_MAXFORCE);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDesiredVelocity1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_H_DESVEL);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_S_MAXFORCE);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDesiredVelocity2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_S_DESVEL);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1b(j, kMeFJointPropertySpecialBool1, &b))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_S_LOCK);
        MeXMLWritePCDATA(op, "%d", b);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_SUSP_HIGH);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_SUSP_LOW);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_SUSP_STIFF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_SUSP_SOFT);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_SUSP_DAMP);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertySpecialFloat1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_SUSP_REF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */
static void MEAPI ConeLimitWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_HALF_ANGLE);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STIFFNESS);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */
static void MEAPI PrismaticWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;
    MeBool b = 0;

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_HIGH_LIMIT);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LOW_LIMIT);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_HIGH_STIFF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LOW_STIFF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1b(j, kMeFJointPropertyLimited1, &b))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LIMITED);
        MeXMLWritePCDATA(op, "%d", b);
    }

    if (MeFJointGetProperty1b(j, kMeFJointPropertyMotorized1, &b))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_MOTORIZED);
        MeXMLWritePCDATA(op, "%d", b);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDesiredVelocity1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_DES_VEL);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStrength1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_MAX_FORCE);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */
static void MEAPI SkeletalWriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;
    MeBool b = 0;
    int i = 0;

    if (MeFJointGetProperty1i(j, kMeFJointPropertySpecialInt1, &i))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_CONETYPE);
        MeXMLWritePCDATA(op, "%d", i);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_CONE_HALF_ANGLE_X);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_CONE_HALF_ANGLE_Y);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_CONE_STIFF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_CONE_DAMP);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1i(j, kMeFJointPropertySpecialInt2, &i))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_TWISTTYPE);
        MeXMLWritePCDATA(op, "%d", i);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStop3, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_TWIST_HALF_ANGLE);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_TWIST_STIFF);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_TWIST_DAMP);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */
static void MEAPI Angular3WriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;
    MeBool b = 0;
    int i = 0;

    if (MeFJointGetProperty1b(j, kMeFJointPropertySpecialBool1, &b))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_ROTATION_ENABLED);
        MeXMLWritePCDATA(op, "%d", b);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_STIFFNESS);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_DAMPING);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */
static void MEAPI Spring6WriteXML_1_0(MeXMLOutput *op, MeFJoint *j, MeXMLElementID parent)
{
    MeReal p = 0;

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LINEAR_STIFF_X);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LINEAR_STIFF_Y);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness3, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LINEAR_STIFF_Z);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping1, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LINEAR_DAMP_X);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping2, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LINEAR_DAMP_Y);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping3, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_LINEAR_DAMP_Z);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness4, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_ANGULAR_STIFF_X);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness5, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_ANGULAR_STIFF_Y);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyStiffness6, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_ANGULAR_STIFF_Z);
        MeXMLWritePCDATA(op, "%.7g", p);
    }

    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping4, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_ANGULAR_DAMP_X);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping5, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_ANGULAR_DAMP_Y);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
    if (MeFJointGetProperty1f(j, kMeFJointPropertyDamping6, &p))
    {
        MeXMLWriteElement(op, parent, ME_ELEM_JOINT_ANGULAR_DAMP_Z);
        MeXMLWritePCDATA(op, "%.7g", p);
    }
}

/** @internal */    
static MeXMLElementID MEAPI MeFJointWriteXML_1_0(MeXMLOutput *op, MeFJoint *fj, MeXMLElementID parent)
{
    char buffer[1024];
    MeVector3 v;
    char *pBuf = buffer;
    MeXMLElementID elem;
    MeFJointType type = MeFJointGetType(fj);
    pBuf = buffer;

    pBuf += sprintf(pBuf, ME_ELEM_JOINT" "ME_ATTR_ID"=\"%s\"", MeFJointGetName(fj));
    pBuf += sprintf(pBuf, " "ME_ATTR_PART1"=\"%s\"", MeFJointGetPartName(fj, 0));

    if (MeFJointGetPart(fj, 1))
        pBuf += sprintf(pBuf, " "ME_ATTR_PART2"=\"%s\"", MeFJointGetPartName(fj,1));

    if (type == kMeFJointTypeBallAndSocket)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_BALLANDSOCKET"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        BallAndSocketWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeHinge)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_HINGE"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        HingeWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeCarwheel)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_CARWHEEL"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        CarWheelWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeConeLimit)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_CONELIMIT"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        ConeLimitWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeUniversal)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_UNIVERSAL"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        UniversalWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeRpro)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_RPRO"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        RproWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypePrismatic)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_PRISMATIC"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        PrismaticWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeSkeletal)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_SKELETAL"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        SkeletalWriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeAngular3)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_ANGULAR3"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        Angular3WriteXML_1_0(op, fj, elem);
    }
    else if (type == kMeFJointTypeSpring6)
    {
        pBuf += sprintf(pBuf, " "ME_ATTR_TYPE"=\""ME_JOINT_TYPE_SPRING6"\"");
        elem = MeXMLWriteElement(op, parent, buffer);
        Spring6WriteXML_1_0(op, fj, elem);
    }
    
    MeFJointGetPosition(fj, 0, v);
    MeXMLWriteElement(op, elem, ME_ELEM_JOINT_POS1);
    MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", v[0], v[1], v[2]);

    MeFJointGetPosition(fj, 1, v);
    MeXMLWriteElement(op, elem, ME_ELEM_JOINT_POS2);
    MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", v[0], v[1], v[2]);
    
    MeFJointGetPrimaryAxis(fj, 0, v);
    MeXMLWriteElement(op, elem, ME_ELEM_JOINT_PRIMARY_AXIS1);
    MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", v[0], v[1], v[2]);

    MeFJointGetPrimaryAxis(fj, 1, v);
    MeXMLWriteElement(op, elem, ME_ELEM_JOINT_PRIMARY_AXIS2);
    MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", v[0], v[1], v[2]);

    MeFJointGetOrthogonalAxis(fj, 0, v);
    MeXMLWriteElement(op, elem, ME_ELEM_JOINT_ORTHOGONAL_AXIS1);
    MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", v[0], v[1], v[2]);
    
    MeFJointGetOrthogonalAxis(fj, 1, v);
    MeXMLWriteElement(op, elem, ME_ELEM_JOINT_ORTHOGONAL_AXIS2);
    MeXMLWritePCDATA(op, "%.7g,%.7g,%.7g", v[0], v[1], v[2]);

    return elem;
}


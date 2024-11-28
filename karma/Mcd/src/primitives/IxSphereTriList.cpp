/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/09 12:01:26 $ - Revision: $Revision: 1.31.2.4 $

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

#include <MePrecision.h>
#include <MeMath.h>

#include <McdSphere.h>
#include <McdCheck.h>
#include <McdInteractionTable.h>
//#include <CxTriangleNE.h>
#include <lsTransform.h>
#include <McdTriangleList.h>
#include <McdContact.h>
#include <McdModelPair.h>
#include <McdModel.h>
#include <McdPrimitives.h>
#include <vectormath.h>

/*
    returns dimensionality: 0 for endpoints, 1 for mid-segment.
*/
static inline MeI16 McdVanillaClosestPointOnSegment(lsVec3 &outPoint,
    const lsVec3 &inLineOrig,const lsVec3 &inLineDisp,
    const lsVec3 &inPointDisp)
{
    const MeReal num = inLineDisp.dot(inPointDisp);
    if (num <= (MeReal)(0.0))
    {
        outPoint = inLineOrig;
        return 0;
    }

    const MeReal den = inLineDisp.square_norm();

    if (num >= den)
    {
        outPoint = inLineOrig+inLineDisp;
        return 0;
    }

    outPoint = inLineOrig+(num/den)*inLineDisp;
    return 1;
}

static inline bool McdVanillaOverlapSphereTri(MeReal &outSep,
    lsVec3 &outN,  lsVec3 &outPos, MeI16 &outDims,
    const MeReal inEps, const MeReal inR, const McdUserTriangle *inTri, const MeVector3 edge[3], const MeReal scale)
{
    int i;

    lsVec3
        &v0 = *(lsVec3*)inTri->vertices[0],
        &v1 = *(lsVec3*)inTri->vertices[1],
        &v2 = *(lsVec3*)inTri->vertices[2];

    // face/sphere

    MeReal eps = (MeReal)1e-6 * scale;

    MeReal normD = (*(lsVec3*)inTri->normal).dot(v0);
    MeReal maxSeparation = MeFabs(normD)-inR;
    if (maxSeparation > inEps)
        return false;

    /* this tests whether the origin is within the voronoi region of the face
    (i.e. the prism defined by the extrusion of the face */

    MeReal d0 = ScalarTripleProduct(*(lsVec3*)inTri->normal,v0,v1),
           d1 = ScalarTripleProduct(*(lsVec3*)inTri->normal,v1,v2),
           d2 = ScalarTripleProduct(*(lsVec3*)inTri->normal,v2,v0);

    if(d0>-eps && d1>-eps && d2>-eps || d0<eps && d1<eps && d2<eps)
    {
        // intersection point is inside the triangle
        outPos = (*(lsVec3*)inTri->normal) * normD;
        outDims = (2<<8)|3;

        if(inTri->flags & kMcdTriangleTwoSided)
        {
            outN = normD > (MeReal)(0.0) ? - (*(lsVec3*)inTri->normal) : (*(lsVec3*)inTri->normal);
            outSep = maxSeparation;
        }
        else
        {
            outN = *(lsVec3*)inTri->normal;
            outSep = - normD - inR;
        }
        return true;
    }

    // edge/sphere
    lsVec3 origin(0,0,0),point;
    MeReal distanceSq = MEINFINITY;

    for(i=0;i<3;i++)
    {
        lsVec3 vertex = *(lsVec3*)inTri->vertices[i];
        MeI16 d = McdVanillaClosestPointOnSegment(point,vertex,
            *(lsVec3*)&edge[i],-vertex);
        MeReal n = point.square_norm();
        if(n<distanceSq)
        {
            distanceSq = n;
            outPos = point;
            outDims = (d<<8)|3;
        }
    }

    if(distanceSq > inR * inR)
        return false;

    MeReal distance = MeSqrt(distanceSq);
    outN = -outPos/distance;
    outSep = distance-inR;
#if 0
    printf("edge - normal: (%f, %f, %f), penetration = %f\n",
        outN[0], outN[1], outN[2], outSep);
#endif

    return true;
}

/*
 * Sphere to HwWorld collision detection
 */

int MEAPI
McdSphereTriangleListIntersect( McdModelPair* p, McdIntersectResult *result )
{

    McdUserTriangle ct;
    MeI16 dims;
    lsVec3 pos;
    MeReal separation;
    lsVec3 normal;
    int i;
    lsVec3 *spherePos, transPos, vector[4];
    MeReal sphereRadius;
    McdUserTriangle *triNE;
    MeVector3 edge[3];
    int count;

    lsTransform *triListTM;
    lsTransform triToSphere;

    McdSphereID spheregeom = (McdSphereID)McdModelGetGeometry( p->model1 );
    McdTriangleListID trilistgeom = (McdTriangleListID)McdModelGetGeometry( p->model2 );

    McdFramework *fwk = p->model1->frame;

    // Sphere's global information
    sphereRadius = McdSphereGetRadius(spheregeom);
    spherePos = &((lsTransform*)McdModelGetTransformPtr(p->model1))->t();
    triListTM = (lsTransform *)McdModelGetTransformPtr(p->model2);

    // work in global frame transformed by sphere's tranlation only
    triToSphere = *triListTM;
    triToSphere.t() -= *spherePos;

    const MeReal eps = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );

    McdTriangleList* triList = (McdTriangleList*)trilistgeom;
    triList->list = (McdUserTriangle *)MeMemoryALLOCA(triList->triangleMaxCount * sizeof(McdUserTriangle));
    triListTM->inverseTransform(*spherePos, &transPos);
    count = (*triList->triangleListGenerator)(p, triList->list, transPos.v, sphereRadius+eps,
        triList->triangleMaxCount);

    result->contactCount = 0;
    result->touch = 0;

    ((lsVec3*)result->normal)->setValue(0,0,0);

    if(count==0)
        return 0;

    ct.normal = (MeVector3*)&vector[0];
    ct.vertices[0] = (MeVector3*)&vector[1];
    ct.vertices[1] = (MeVector3*)&vector[2];
    ct.vertices[2] = (MeVector3*)&vector[3];
    
    McdContact *c = result->contacts;

    for(i=0; i<count && result->contactCount<200; i++)
    {
        triNE = triList->list+i;
        /* Map vertices back into collision then box space  */
        triToSphere.transform((lsVec3 &)*triNE->vertices[0][0], (lsVec3*)ct.vertices[0]);
        triToSphere.transform((lsVec3 &)*triNE->vertices[1][0], (lsVec3*)ct.vertices[1]);
        triToSphere.transform((lsVec3 &)*triNE->vertices[2][0], (lsVec3*)ct.vertices[2]);
        triToSphere.transformWithoutTranslate((lsVec3 &)*triNE->normal[0], (lsVec3*)ct.normal);
        ct.flags = triNE->flags;
        ct.triangleData = triNE->triangleData;
        
        /* Edges */
        MeVector3Subtract(edge[0], *ct.vertices[1], *ct.vertices[0]);
        MeVector3Subtract(edge[1], *ct.vertices[2], *ct.vertices[1]);
        MeVector3Subtract(edge[2], *ct.vertices[0], *ct.vertices[2]);

        // Overlap test, return overlap values

        if (McdVanillaOverlapSphereTri(separation,
              normal,pos,dims,eps,sphereRadius,&ct,edge,fwk->mScale))
        {
            // Transform footprint into global frame, and find depth
            *(lsVec3*)c->position = pos+*spherePos;
            c->dims = dims;

#ifndef T_RELEASE_1_1_0
            *(lsVec3*)c->normal = normal;
            c->separation = separation;
#else
            if(!(triNE->flags & kMcdTriangleTwoSided) && c->dims>>8 == 2)
            {
                /* Use poly normal for surface contacts */
                *(lsVec3*)c->normal = ct.mNormal;
                if(ct.mNormal.dot(normal) < 0) /* Normal has been flipped */
                    c->separation = -2.0f*sphereRadius - separation;
                else
                    c->separation = separation;
            }
            else
            {
                *(lsVec3*)c->normal = normal;
                c->separation = separation;
            }
#endif

#ifdef MCD_CURVATURE
            c->curvature1 = sphereRadius;
            c->curvature2 = 0;
#endif

            (*(lsVec3*)result->normal) += normal;

            /* Copy user data into McdContact */
            c->element2.ptr = triNE->triangleData.ptr;

            result->contactCount++;
            c++;
        }
    }

    MeVector3Normalize(result->normal);
    return result->touch = result->contactCount > 0;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Sphere, TriangleList,1);


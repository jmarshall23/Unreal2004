/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/24 09:53:18 $ - Revision: $Revision: 1.38.2.11 $

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

#include <stdio.h>
#include <MeMath.h>
#include <McdBox.h>
#include <McdCheck.h>
#include <McdInteractionTable.h>
#include <McdModel.h>
#include <McdContact.h>
#if 0
#include <CxTriangleNE.h>
#endif
#include <lsTransform.h>
#include <McdTriangleList.h>
#include <McdModelPair.h>
#include <McdPrimitives.h>
#include <vectormath.h>

/*
    Temporary fix; probably required only for the 991111 PS2 compiler.
*/

#ifdef PS2
#   define inline
#endif

static inline int McdVanillaSegmentCubeIntersect(
    MeReal *tInMax,MeReal *tOutMin,
    const lsVec3 &orig,const lsVec3 &disp,
    const lsVec3 &invDisp,const lsVec3 &inR,const MeReal eps)
{
    int j;
    MeReal tIn, tOut;

    for (j = 0; j < 3; j++)
    { // j = axis of box A

        if(MeFabs(disp[j])<eps)
        {
            if(MeFabs(orig[j]) > inR[j])
                return 0;
        }
        else
        {
            tIn =  (- (orig[j] * invDisp[j])) - inR[j] * MeFabs(invDisp[j]);
            tOut = (- (orig[j] * invDisp[j])) + inR[j] * MeFabs(invDisp[j]);

            if (tIn > *tInMax)
                *tInMax = tIn;
            if (tOut < *tOutMin)
                *tOutMin = tOut;
            if (*tOutMin < *tInMax)
                return 0;
        }
    }
    return 1;
}

static inline void McdVanillaAddBoxEndSegmentPoints(lsVec3* &outList,
    const lsVec3 &orig, const lsVec3 &disp,
    const lsVec3 &invDisp, const lsVec3 &inR, const MeReal eps)
{
    MeReal tIn = 0;
    MeReal tOut = 1;

    if (McdVanillaSegmentCubeIntersect(&tIn,&tOut,orig,disp,invDisp,inR,eps))
    {
        *outList++ = orig+tIn*disp;
        if (tOut < (MeReal)1.0)
            *outList++ = orig+tOut*disp;
    }
}

static inline void McdVanillaAddTriBoxSegmentPoints(lsVec3* &outList,
    const lsVec3 &inRBox, const int i0, const int i1, const int i2,
    const McdUserTriangle * inTri, const MeVector3 edge[3], const lsVec3 axb[], const MeReal inTriD)
{
    const MeReal r0 = inRBox[i0];
    const MeReal den = (*inTri->normal)[i0];
    const MeReal recipDen = MeSafeRecip(den);
    const bool ccw = den < (MeReal)(0.0);

    lsVec3 x[3];
    x[0] = Vec3CrossAxis(*(lsVec3*)edge[0],i0);
    x[0][i1] *= inRBox[i1];
    x[0][i2] *= inRBox[i2];
    x[1] = Vec3CrossAxis(*(lsVec3*)edge[1],i0);
    x[1][i1] *= inRBox[i1];
    x[1][i2] *= inRBox[i2];
    x[2] = Vec3CrossAxis(*(lsVec3*)edge[2],i0);
    x[2][i1] *= inRBox[i1];
    x[2][i2] *= inRBox[i2];

    lsVec3 dn;
    dn[i0] = 0;
    dn[i1] = inRBox[i1]*(*inTri->normal)[i1];
    dn[i2] = inRBox[i2]*(*inTri->normal)[i2];

    // ++ edge
    if (axb[0][i0]-x[0][i1]-x[0][i2] < (MeReal)(0.0) == ccw &&
        axb[1][i0]-x[1][i1]-x[1][i2] < (MeReal)(0.0) == ccw &&
        axb[2][i0]-x[2][i1]-x[2][i2] < (MeReal)(0.0) == ccw)
    {   // points at triangle
        MeReal r = (inTriD-dn[i1]-dn[i2])*recipDen;
        if (r >= -r0 && r <= r0)
        {  // intersects segment
            outList->operator[](i0) = r;
            outList->operator[](i1) = inRBox[i1];
            outList->operator[](i2) = inRBox[i2];
            outList++;
        }
    }

    // +- edge
    if (axb[0][i0]-x[0][i1]+x[0][i2] < (MeReal)(0.0) == ccw &&
        axb[1][i0]-x[1][i1]+x[1][i2] < (MeReal)(0.0) == ccw &&
        axb[2][i0]-x[2][i1]+x[2][i2] < (MeReal)(0.0) == ccw)
    {   // points at triangle
        MeReal r = (inTriD-dn[i1]+dn[i2])*recipDen;
        if (r >= -r0 && r <= r0)
        {  // intersects segment
            outList->operator[](i0) = r;
            outList->operator[](i1) = inRBox[i1];
            outList->operator[](i2) = -inRBox[i2];
            outList++;
        }
    }

    // -- edge
    if (axb[0][i0]+x[0][i1]+x[0][i2] < (MeReal)(0.0) == ccw &&
        axb[1][i0]+x[1][i1]+x[1][i2] < (MeReal)(0.0) == ccw &&
        axb[2][i0]+x[2][i1]+x[2][i2] < (MeReal)(0.0) == ccw)
    {   // points at triangle
        MeReal r = (inTriD+dn[i1]+dn[i2])*recipDen;
        if (r >= -r0 && r <= r0)
        {  // intersects segment
            outList->operator[](i0) = r;
            outList->operator[](i1) = -inRBox[i1];
            outList->operator[](i2) = -inRBox[i2];
            outList++;
        }
    }

    // -+ edge
    if (axb[0][i0]+x[0][i1]-x[0][i2] < (MeReal)(0.0) == ccw &&
        axb[1][i0]+x[1][i1]-x[1][i2] < (MeReal)(0.0) == ccw &&
        axb[2][i0]+x[2][i1]-x[2][i2] < (MeReal)(0.0) == ccw)
    {   // points at triangle
        MeReal r = (inTriD+dn[i1]-dn[i2])*recipDen;
        if (r >= -r0 && r <= r0) {  // intersects segment
            outList->operator[](i0) = r;
            outList->operator[](i1) = -inRBox[i1];
            outList->operator[](i2) = inRBox[i2];
            outList++;
        }
    }
}

static inline void McdVanillaBoxTriIntersect(lsVec3* &outList,
    const lsVec3 &inRBox, const McdUserTriangle *inTri, const MeVector3 edge[3], const MeReal scale)
{
    if(inTri->flags&kMcdTriangleUseEdge0)
    {
        const lsVec3 invE(MeSafeRecip(edge[0][0]),
                          MeSafeRecip(edge[0][1]),
                          MeSafeRecip(edge[0][2]));
        McdVanillaAddBoxEndSegmentPoints(outList,
                               *(lsVec3*)inTri->vertices[0], *(lsVec3*)edge[0],
                               invE, inRBox, (MeReal)1e-6 * scale);
    }

    if(inTri->flags&kMcdTriangleUseEdge1)
    {
        const lsVec3 invE(MeSafeRecip(edge[1][0]),
                          MeSafeRecip(edge[1][1]),
                          MeSafeRecip(edge[1][2]));
        McdVanillaAddBoxEndSegmentPoints(outList,
                               *(lsVec3*)inTri->vertices[1], *(lsVec3*)edge[1],
                               invE, inRBox, (MeReal)1e-6 * scale);
    }

    if(inTri->flags&kMcdTriangleUseEdge2)
    {
        const lsVec3 invE(MeSafeRecip(edge[2][0]),
                          MeSafeRecip(edge[2][1]),
                          MeSafeRecip(edge[2][2]));
        McdVanillaAddBoxEndSegmentPoints(outList,
                               *(lsVec3*)inTri->vertices[2], *(lsVec3*)edge[2],
                               invE, inRBox, (MeReal)1e-6 * scale);
    }

    lsVec3 axb[3];
    axb[0] = ((lsVec3*)inTri->vertices[0])->cross(*(lsVec3*)inTri->vertices[1]);
    axb[1] = ((lsVec3*)inTri->vertices[1])->cross(*(lsVec3*)inTri->vertices[2]);
    axb[2] = ((lsVec3*)inTri->vertices[2])->cross(*(lsVec3*)inTri->vertices[0]);

    const MeReal triD = ((lsVec3*)inTri->vertices[0])->dot((*(lsVec3*)inTri->normal));

    if ((*inTri->normal)[0] != (MeReal)(0.0))  // x-direction edges
        McdVanillaAddTriBoxSegmentPoints(outList,inRBox,0,1,2,inTri,edge,axb,triD);

    if ((*inTri->normal)[1] != (MeReal)(0.0))  // y-direction edges
        McdVanillaAddTriBoxSegmentPoints(outList,inRBox,1,2,0,inTri,edge,axb,triD);

    if ((*inTri->normal)[2] != (MeReal)(0.0))  // z-direction edges
        McdVanillaAddTriBoxSegmentPoints(outList,inRBox,2,0,1,inTri,edge,axb,triD);
}

static inline bool McdVanillaOverlapOBBTri(MeReal &outSep,
    lsVec3 &outN, MeReal &outPN, lsVec3* &outPos, MeI16 &outDims,
    const MeReal inEps, const lsVec3 &inR, const McdUserTriangle *inTri, const MeVector3 edge[3],
    const MeReal scale)
{
    int i;
    int j;

    const MeReal eps = inEps<(MeReal)(0.0)?(MeReal)(0.0):inEps;
    const MeReal eps2 = eps*eps;

    // Find maximum separation (early-out for positive separation)
    MeReal maxSeparation;
    MeReal normalSign;
    MeU8 normInfo;
    MeReal nRLen = (MeReal)(1.0);
    lsVec3 aE[3];
    MeReal sqE[3];
    MeReal PN;
    lsVec3 minCoord;

    lsVec3 aNorm(MeFabs((*inTri->normal)[0]),
                 MeFabs((*inTri->normal)[1]),
                 MeFabs((*inTri->normal)[2]));

    /*
        normInfo = (inClientInfo<<2) | thisClientInfo;
        xInfo = { 0,1,2 => axis #, 3 => other info gives normal }
        If both thisClientInfo and inClientInfo are less than 3,
        then normal is cross product of two axes.
    */

    // Face separation (face of triangle):
    MeReal sumR = inR[0]*aNorm[0]+inR[1]*aNorm[1]+inR[2]*aNorm[2];
    MeReal normD = (*(lsVec3*)inTri->normal).dot(*(lsVec3*)inTri->vertices[0]);
    MeReal aNormD = MeFabs(normD);
    maxSeparation = aNormD-sumR;
    PN = -aNormD;
    normalSign = normD > (MeReal)(0.0) ? -(MeReal)(1.0) : (MeReal)(1.0);
    normInfo = 3;

    if (maxSeparation > inEps)
        return false;

    if (!(inTri->flags & kMcdTriangleTwoSided) && normalSign < 0)
    {
        normalSign = 1;
        maxSeparation = -maxSeparation - 2*sumR;
    }

    /* calculate the default separating plane */
    outN = normalSign*(*(lsVec3*)inTri->normal);
    outPN = outN.dot(*(lsVec3*)inTri->vertices[0]);
    outDims = (2<<8)|((MeI16)(aNorm[0] < (MeReal)(1.0e-4))+
                      (MeI16)(aNorm[1] < (MeReal)(1.0e-4))+
                      (MeI16)(aNorm[2] < (MeReal)(1.0e-4)));

    // Face separation (face of OBB):
    for (i = 0; i < 3; i++) {
        if (MeFabs((*inTri->normal)[i]) > 1-inEps)
        {
            // already handled by triangle normal
            continue;
        }
        minCoord[i] = ((lsVec3*)inTri->vertices[0])->operator[](i);
        MeReal maxCoord = ((lsVec3*)inTri->vertices[0])->operator[](i);

        if (((lsVec3*)inTri->vertices[1])->operator[](i) < minCoord[i])
            minCoord[i] = ((lsVec3*)inTri->vertices[1])->operator[](i);

        if (((lsVec3*)inTri->vertices[1])->operator[](i) > maxCoord)
            maxCoord = ((lsVec3*)inTri->vertices[1])->operator[](i);

        if (((lsVec3*)inTri->vertices[2])->operator[](i) < minCoord[i])
            minCoord[i] = ((lsVec3*)inTri->vertices[2])->operator[](i);

        if (((lsVec3*)inTri->vertices[2])->operator[](i) > maxCoord)
            maxCoord = ((lsVec3*)inTri->vertices[2])->operator[](i);

        sumR = inR[i]+(MeReal)(0.5)*(maxCoord-minCoord[i]);
        normD = (MeReal)(0.5)*(maxCoord+minCoord[i]);
        MeReal separation = MeFabs(normD)-sumR;
        if (separation > maxSeparation)
        {
            maxSeparation = separation;
            PN = -inR[i]-separation;
            normalSign = normD > (MeReal)(0.0f)
                ? -(MeReal)(1.0f) : (MeReal)(1.0f);
            normInfo = 0xC|i;
            if(separation > inEps)
                return false;
        }
    }

    aE[0] = lsVec3::abs(*(lsVec3*)edge[0]);
    aE[1] = lsVec3::abs(*(lsVec3*)edge[1]);
    aE[2] = lsVec3::abs(*(lsVec3*)edge[2]);

    sqE[0] = (*(lsVec3*)edge[0]).square_norm();
    sqE[1] = (*(lsVec3*)edge[1]).square_norm();
    sqE[2] = (*(lsVec3*)edge[2]).square_norm();


    // Edge separation:
    for (i = 0; i < 3; i++)
    { // Triangle edges
        int i1 = NextMod3(i);
        for (j = 0; j < 3; j++)
        { // Box axes
            int j1 = NextMod3(j);
            int j2 = NextMod3(j1);
            MeReal sR = (MeReal)(0.5)*(edge[i1][j1]*edge[i][j2]-
                                       edge[i1][j2]*edge[i][j1]);
            MeReal rB = MeFabs(sR);
            sumR = inR[j1]*aE[i][j2]+inR[j2]*aE[i][j1]+rB;
            normD = ((lsVec3*)inTri->vertices[i])->operator[](j1)*edge[i][j2]-
                    ((lsVec3*)inTri->vertices[i])->operator[](j2)*edge[i][j1]+sR;
            MeReal rLen = sqE[i]-aE[i][j]*aE[i][j];
            if (rLen > eps2 * sqE[i])
            {
                rLen = MeSafeRecip(MeSqrt(rLen));
/*
    This test is redundant, since  the conditional if (separation-maxSeparation > eps), below will eliminate
    edge collisions which are too close to being a face collision.  I'm chucking it because the test seems
    to be failing, and throwing out needed edge normals.  BRG 24Apr02
                if (MeFabs(rLen*(*(lsVec3*)inTri->normal).dot(Vec3CrossAxis(*(lsVec3*)edge[i],j))) > 1-inEps)
                {
                  // already handled by triangle normal
                    continue;
                }
 */
                MeReal aNormD = MeFabs(normD)*rLen;
                MeReal separation = aNormD-sumR*rLen;
                if (separation-maxSeparation > eps)
                {
                    maxSeparation = separation;
                    PN = rB*rLen-aNormD;
                    normalSign = normD > (MeReal)(0.0f)
                        ? -(MeReal)(1.0f) : (MeReal)(1.0f);
                    normInfo = (i<<2)|j;
                    nRLen = rLen;
                    if (separation > inEps)
                        return false;
                }
            }
        }
    }

    outSep = maxSeparation;

    if(inTri->flags & kMcdTriangleUseSmallestPenetration)
    {
        // normal points from Tri to OBB
        if ((normInfo&0xC) == 0xC)
        { // Normal is from OBB
            MeI8 axis = normInfo&3;
            outN.setValue((MeReal)(0.0f),(MeReal)(0.0f),(MeReal)(0.0f));
            outN[axis] = normalSign;
            outPN = -inR[axis]-maxSeparation;
            MeI16 dimB =
                (MeI16)(((lsVec3*)inTri->vertices[0])->operator[](axis)-minCoord[axis]
                    <= eps)
                + (MeI16)(((lsVec3*)inTri->vertices[1])->operator[](axis)-minCoord[axis]
                    <= eps)
                + (MeI16)(((lsVec3*)inTri->vertices[2])->operator[](axis)-minCoord[axis]
                    <= eps)
                - 1;

            MCD_CHECK_ASSERT_(dimB >= 0 && dimB <= 2, "overlapOBBTri");

            outDims = (dimB<<8)|2;
        }
        else if(normInfo!=3)
        { // Normal is from crossed edges
            if (normalSign > (MeReal)(0.0))
                outN = Vec3CrossAxis(*(lsVec3*)edge[(normInfo&0xC)>>2],normInfo&3);
            else
                outN = AxisCrossVec3(normInfo&3,*(lsVec3*)edge[(normInfo&0xC)>>2]);
            outPN = PN;
            outDims = (1<<8)|1;
            outN *= nRLen;
        }
    }

    /*
        all the contacts are generated by intersections with the
        triangle, so if the plane is the minimal separating distance,
        they'll all have penetration zero. So we hack PN. But this is
        only a good idea if the minimal separating plane really is the
        triangle face, otherwise it generates artifically large contacts
        if we hit the triangle edge-on.
    */
    if(normInfo==3)
        outPN -= maxSeparation;


    lsVec3 *posList = outPos;
    McdVanillaBoxTriIntersect(outPos,inR,inTri,edge,scale);

    return outPos != posList;
}

/*
 * Box to Tri List collision detection
 */

/*
   Utility function to produce a bsphere in local coords
*/

#if 0
McdModelGetBSphereInLocalFrame(MeMatrix4Ptr frame,
                   McdModelID m, MeVector3 center, MeReal* radius)
{
    lsTransform *framet = (lsTransform*)frame;
    lsVec3 scenter;
    McdModelGetBSphere(m,scenter,radius);
    framet->transform(scenter, (lsVec3*)center);
}
#endif

MeBool MEAPI McdBoxTriangleListIntersect( McdModelPair* p,
    McdIntersectResult *result ) 
{
    lsVec3 *boxRadii;
    MeVector3 boxCentre;
    lsTransform triToBox;
    lsVec3 boxPosTrans, vector[4];
    McdUserTriangle ct;
    MeVector3 edge[3];
    MeI16 dims;
    lsVec3 footprint[18], *verts, normal, v;
    MeReal separation, PN, boxRadius;
    McdUserTriangle *triNE;
    int count;
    int j = 0;
    
    McdBoxID boxgeom = (McdBoxID)McdModelGetGeometry(p->model1);
    McdTriangleListID trilistgeom = (McdTriangleListID)McdModelGetGeometry(p->model2);
    
    const MeReal eps = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    // Box's global information
    lsTransform &boxTM = *(lsTransform*)McdModelGetTransformPtr(p->model1);
    lsTransform &triListTM = *(lsTransform*)McdModelGetTransformPtr(p->model2);

    McdFramework *fwk = p->model1->frame;

    result->contactCount = 0;
    result->touch = 0;

    boxRadii = (lsVec3*)McdBoxGetRadii( boxgeom );
    
    /* Transform box into tri list space    */
    triListTM.inverseTransform(boxTM.t(), &boxPosTrans);    
    McdBoxGetBSphere(boxgeom,boxCentre,&boxRadius);
    McdTriangleList* triList = (McdTriangleList*)trilistgeom;

    triList->list = (McdUserTriangle *)MeMemoryALLOCA(triList->triangleMaxCount * sizeof(McdUserTriangle));
    count = (*triList->triangleListGenerator)(p, triList->list, boxPosTrans.v, boxRadius+eps, 
        triList->triangleMaxCount);

    ((lsVec3*)result->normal)->setValue(0,0,0);
    if(count==0)
        return 0;

    ct.normal = (MeVector3*)&vector[0];
    ct.vertices[0] = (MeVector3*)&vector[1];
    ct.vertices[1] = (MeVector3*)&vector[2];
    ct.vertices[2] = (MeVector3*)&vector[3];
    
    McdContact *c = result->contacts;

    triToBox.thisIsFirstThenInverseSecond(triListTM,boxTM);

    for(j=0; j<count; j++)
    {
        triNE = triList->list+j;

        /* Map vertices back into collision then box space  */
        triToBox.transform((lsVec3 &)*triNE->vertices[0][0], (lsVec3*)ct.vertices[0]);
        triToBox.transform((lsVec3 &)*triNE->vertices[1][0], (lsVec3*)ct.vertices[1]);
        triToBox.transform((lsVec3 &)*triNE->vertices[2][0], (lsVec3*)ct.vertices[2]);
        triToBox.transformWithoutTranslate((lsVec3 &)*triNE->normal[0], (lsVec3*)ct.normal);
        ct.flags = triNE->flags;
        ct.triangleData = triNE->triangleData;

        /* Edges */
        MeVector3Subtract(edge[0], *ct.vertices[1], *ct.vertices[0]);
        MeVector3Subtract(edge[1], *ct.vertices[2], *ct.vertices[1]);
        MeVector3Subtract(edge[2], *ct.vertices[0], *ct.vertices[2]);

        verts = footprint;

        if (McdVanillaOverlapOBBTri(separation,
              normal,PN,verts,dims,eps,*boxRadii,&ct,edge,fwk->mScale))
        {
            // Transform normal into global frame
            lsVec3 globalNormal;
            boxTM.transformWithoutTranslate(normal,&globalNormal);

            // Transform footprint into global frame, and find depths
            lsVec3 *v = footprint;
            // McdContact *c = result->contacts+result->contactCount;
            while (v != verts && result->contactCount < 400) {
                //MeReal s = v->dot(normal)-PN;

                //if (s < eps*separation)
                {
                    lsVec3 *cpos = (lsVec3*)c->position;
                    boxTM.transform(*v,cpos);
                    c->dims = dims;
                    //c->separation = s;
                    c->separation = separation;

#ifdef MCD_CURVATURE
                    c->curvature1 = 0;
                    c->curvature2 = 0;
#endif

                    /* Copy user data into McdContact */
                    c->element2.ptr = triNE->triangleData.ptr;

                    *(lsVec3*)c->normal = globalNormal;
                    c++;
                    result->contactCount++;
                }
                v++;
            }

            (*(lsVec3*)result->normal) += globalNormal;
        }
    }
    if (result->contactCount > 0)
    {
        MeVector3Normalize(result->normal);
        result->touch = 1;
    }
    else
        result->touch = 0;   

    return result->touch;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Box, TriangleList,1);
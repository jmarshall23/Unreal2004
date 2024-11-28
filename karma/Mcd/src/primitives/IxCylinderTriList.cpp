/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/24 09:53:18 $ - Revision: $Revision: 1.23.2.9 $

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

#include <MePrecision.h>
#include <MeMath.h>

#include <McdCylinder.h>
#include <McdCheck.h>
//#include <CxTriangleNE.h>
#include <lsTransform.h>
#include <McdTriangleList.h>
#include <McdContact.h>
#include <McdModel.h>
#include <McdPrimitives.h>
#include <vectormath.h>

static bool McdVanillaSegmentCylinderIntersect(MeReal &tInMax,MeReal &tOutMin,
    const lsVec3 &orig, const lsVec3 &disp,
    const MeReal inR,const MeReal inHH, const MeReal scale)
{

    // First, test the ray's line against the infinite cylinder:
    MeReal X2minusR2 = orig[0]*orig[0]+orig[1]*orig[1]-inR*inR;
    MeReal XdotDisp = orig[0]*disp[0]+orig[1]*disp[1];

    MeReal r2 = disp[0]*disp[0]+disp[1]*disp[1];
    MeReal XdotDisp2 = XdotDisp*XdotDisp;
    MeReal arg = XdotDisp2-r2*X2minusR2;

    if (arg < (MeReal)(0.0)) // line misses the cylinder
        return false;

    MeReal test = -XdotDisp-tOutMin*r2;
    // entrance time is greater than tOutMin
    if (test >= (MeReal)(0.0) && test*test > arg)
        return false;

    test = XdotDisp+tInMax*r2;
    // exit time is less than tInMax
    if (test >= (MeReal)(0.0) && test*test > arg)
        return false;

    MeReal MeSqrtArg = MeSqrt(arg);
    MeReal tEnterNum = -XdotDisp-MeSqrtArg;
    MeReal tExitNum = -XdotDisp+MeSqrtArg;
#if 0
    tEnterDen = tExitDen = r2
#endif

    // Clip the ray against the implicit slab:
    const MeReal origZ = orig[2];
    const MeReal dispZ = disp[2];
    //  const destZ = origZ+dispZ;

    MeReal numZ0;
    MeReal denZ;
    if (dispZ < (MeReal)(0.0))
    {
        numZ0 = origZ;
        denZ = -dispZ;
    } else {
        numZ0 = -origZ;
        denZ = dispZ;
    }

    MeReal tEnterNumZ = numZ0-inHH;
    if (tEnterNumZ > denZ*tOutMin)
        // entrance time is greater than tOutMin
        return false;

    MeReal tExitNumZ = numZ0+inHH;
    if (tExitNumZ < denZ*tInMax)
        // exit time is less than tInMax
        return false;

    // Now perform set intersection of slab and cylinder
    if (tExitNum*denZ < tEnterNumZ*r2 || tExitNumZ*r2 < tEnterNum*denZ)
        // no intersection
        return false;

    MeReal invR2;
    MeReal invZ;

    MeReal tIn;
    MeReal tOut;

    if (r2 < (MeReal)(1.0e-16) * scale * scale )
    {
        // Line segment parallel w/cyl axis
        if (X2minusR2 > (MeReal)(0.0))
            return false;
        invZ = MeSafeRecip(denZ); // Latest entrance is the slab
        tIn = tEnterNumZ*invZ;     // Earliest exit is the slab
        tOut = tExitNumZ*invZ;
    }
    else if (MeFabs(denZ) < (MeReal)(1.0e-8) * scale)
    {
        // Line segment perpendicular w/cyl axis
        if (MeFabs(origZ) > inHH)
            return false;

        invR2 = MeSafeRecip(r2);
        tIn = tEnterNum*invR2; // Latest entrance is the cylinder
        tOut = tExitNum*invR2; // Earliest exit is the cylinder
    }
    else
    {
        invZ = MeSafeRecip(denZ);
        invR2 = MeSafeRecip(r2);

        tIn = (tEnterNum*denZ < tEnterNumZ*r2)
            ?  tEnterNumZ*invZ : tEnterNum*invR2;
        tOut = (tExitNumZ*r2 < tExitNum*denZ)
            ? tExitNumZ*invZ : tExitNum*invR2;
    }


    if (tIn > tInMax)
        tInMax = tIn;
    if (tOut < tOutMin)
        tOutMin = tOut;

    return true;
}

static inline void McdVanillaAddCylEndSegmentPoints(lsVec3* &outList,
    const lsVec3 &orig,const lsVec3 &disp,const MeReal inR,
    const MeReal inHH,const MeReal minT,const MeReal maxT, const MeReal scale)
{
    MeReal tIn = minT;
    MeReal tOut = maxT;

    if (McdVanillaSegmentCylinderIntersect(tIn,tOut,orig,disp,inR,inHH, scale))
    {
        *outList++ = orig+tIn*disp;
        if (tOut < maxT)
            *outList++ = orig+tOut*disp;
    }
}

static void McdVanillaAddTriCylSegmentPoints(lsVec3* &outList,
    const MeReal inRCyl,const MeReal inHHCyl,const lsVec3 &e0,const lsVec3 &e1,
    const McdUserTriangle * inTri, const MeVector3 edge[3],const lsVec3 axb[],const MeReal inTriD)
{
    const MeReal den = (*(lsVec3*)inTri->normal)[2];
    const bool ccw = den < (MeReal)(0.0);

    if (den != (MeReal)(0.0))
    {
        const MeReal recipDen = MeSafeRecip(den);
        MeReal x[3];
        MeReal y[3];

        x[0] = inRCyl*(e0[0]*edge[0][1]-e0[1]*edge[0][0]);
        x[1] = inRCyl*(e0[0]*edge[1][1]-e0[1]*edge[1][0]);
        x[2] = inRCyl*(e0[0]*edge[2][1]-e0[1]*edge[2][0]);
        y[0] = inRCyl*(e1[0]*edge[0][1]-e1[1]*edge[0][0]);
        y[1] = inRCyl*(e1[0]*edge[1][1]-e1[1]*edge[1][0]);
        y[2] = inRCyl*(e1[0]*edge[2][1]-e1[1]*edge[2][0]);

        MeReal dx = inRCyl*((*(lsVec3*)inTri->normal).dot(e0));
        MeReal dy = inRCyl*((*(lsVec3*)inTri->normal).dot(e1));

        // +0 edge
        if (axb[0][2]-x[0] < (MeReal)(0.0) == ccw &&
            axb[1][2]-x[1] < (MeReal)(0.0) == ccw &&
            axb[2][2]-x[2] < (MeReal)(0.0) == ccw)
        {  // points at triangle
            MeReal r = (inTriD-dx)*recipDen;
            if (r >= -inHHCyl && r <= inHHCyl)
            {  // intersects segment
                outList->operator[](0) = inRCyl*e0[0];
                outList->operator[](1) = inRCyl*e0[1];
                outList->operator[](2) = r;
                outList++;
            }
        }

        // -0 edge
        if (axb[0][2]+x[0] < (MeReal)(0.0) == ccw &&
            axb[1][2]+x[1] < (MeReal)(0.0) == ccw &&
            axb[2][2]+x[2] < (MeReal)(0.0) == ccw)
        {  // points at triangle
            MeReal r = (inTriD+dx)*recipDen;
            if (r >= -inHHCyl && r <= inHHCyl)
            {  // intersects segment
                outList->operator[](0) = -inRCyl*e0[0];
                outList->operator[](1) = -inRCyl*e0[1];
                outList->operator[](2) = r;
                outList++;
            }
        }

        // 0+ edge
        if (axb[0][2]-y[0] < (MeReal)(0.0) == ccw &&
            axb[1][2]-y[1] < (MeReal)(0.0) == ccw &&
            axb[2][2]-y[2] < (MeReal)(0.0) == ccw)
        {  // points at triangle
            MeReal r = (inTriD-dy)*recipDen;
            if (r >= -inHHCyl && r <= inHHCyl)
            {  // intersects segment
                outList->operator[](0) = inRCyl*e1[0];
                outList->operator[](1) = inRCyl*e1[1];
                outList->operator[](2) = r;
                outList++;
            }
        }

        // 0- edge
        if (axb[0][2]+y[0] < (MeReal)(0.0) == ccw &&
            axb[1][2]+y[1] < (MeReal)(0.0) == ccw &&
            axb[2][2]+y[2] < (MeReal)(0.0) == ccw)
        {  // points at triangle
            MeReal r = (inTriD+dy)*recipDen;
            if (r >= -inHHCyl && r <= inHHCyl)
            {  // intersects segment
                outList->operator[](0) = -inRCyl*e1[0];
                outList->operator[](1) = -inRCyl*e1[1];
                outList->operator[](2) = r;
                outList++;
            }
        }
    }

    MeReal denPerp = (*(lsVec3*)inTri->normal).dot(e0);

    bool ccwPerp = denPerp < (MeReal)(0.0);

    if (denPerp != (MeReal)(0.0))
    {
        MeReal recipDenPerp = MeSafeRecip(denPerp);

        // perp edges
        MeReal p0 = axb[0].dot(e0);
        MeReal p1 = axb[1].dot(e0);
        MeReal p2 = axb[2].dot(e0);

        MeReal x0 = inHHCyl*(edge[0][1]*e0[0]-edge[0][0]*e0[1]);
        MeReal x1 = inHHCyl*(edge[1][1]*e0[0]-edge[1][0]*e0[1]);
        MeReal x2 = inHHCyl*(edge[2][1]*e0[0]-edge[2][0]*e0[1]);

        MeReal dz = inHHCyl*((*(lsVec3*)inTri->normal)[2]);

        if (p0+x0 < (MeReal)(0.0) == ccwPerp &&
            p1+x1 < (MeReal)(0.0) == ccwPerp &&
            p2+x2 < (MeReal)(0.0) == ccwPerp)
        { // points at triangle
            MeReal r = (inTriD-dz)*recipDenPerp;
            if (r >= -inRCyl && r <= inRCyl)
            {  // intersects segment
                *outList = r*e0;
                outList->operator[](2) += inHHCyl;
                outList++;
            }
        }

        if (p0-x0 < (MeReal)(0.0) == ccwPerp &&
            p1-x1 < (MeReal)(0.0) == ccwPerp &&
            p2-x2 < (MeReal)(0.0) == ccwPerp)
        { // points at triangle
            MeReal r = (inTriD+dz)*recipDenPerp;
            if (r >= -inRCyl && r <= inRCyl)
            {  // intersects segment
                *outList = r*e0;
                outList->operator[](2) -= inHHCyl;
                outList++;
            }
        }
    }
}

static inline bool McdVanillaOverlapCylTri(MeReal &outSep,
    lsVec3 &outN,  MeReal &outPN, lsVec3* &outPos, MeI16 &outDims,
    const MeReal inEps, const MeReal inRCyl, const MeReal inHHCyl,
    const McdUserTriangle * inTri, const MeVector3 edge[3], const MeReal scale)
{
    int i;

    const MeReal eps2 = inEps*inEps;

    // Find maximum separation (early-out for positive separation)

    MeReal maxSeparation;
    MeReal normalSign = 1.0;
    MeU8 normInfo = 0xF;
    // MeReal nLen = 1.0;
    //lsVec3 d2;
    MeReal separation;
    MeReal sumR;
    MeReal normD;
    MeReal PN;
    MeI16 dimA;

    MeReal nRLen = 1.0;

    MeReal minCoord;
    MeReal maxCoord;

    //  lsVec3 nBExET[3];
    lsVec3 nParaN;
    lsVec3 nPerpN;

    lsVec3 aE[3];
    MeReal sqE[3];

    lsVec3 nPara;
    lsVec3 nPerp;

    // Face separation:
    nParaN = AxisCrossVec3(2,(*(lsVec3*)inTri->normal));
    if (nParaN.square_norm() < 1.0e-15)
    {
        nParaN.setValue(0.0,0.0,0.0);
        nParaN[1] = 1.0;
        nPerpN.setValue(0.0,0.0,0.0);
        nPerpN[0] = 1.0;
    }
    else
    {
        nParaN.normalize();
        nPerpN = Vec3CrossAxis(nParaN,2);
    }
    // triangle face separation

    sumR = MeFabs((*(lsVec3*)inTri->normal)[2])*inHHCyl +
             MeFabs(nPerpN.dot((*(lsVec3*)inTri->normal)))*inRCyl;
    normD = ((lsVec3*)inTri->vertices[0])->dot((*(lsVec3*)inTri->normal));
    maxSeparation = MeFabs(normD)-sumR;
    normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
    normInfo = 0xC;
    if (maxSeparation > inEps)
        return 0;

    if (!(inTri->flags & kMcdTriangleTwoSided) && normalSign < 0)
    {
        normalSign = 1;
        maxSeparation = -maxSeparation - 2*sumR;
    }

    nPara = nParaN;
    nPerp = nPerpN;
    outN = normalSign*(*(lsVec3*)inTri->normal);
    outPN = outN.dot(*(lsVec3*)inTri->vertices[0])-maxSeparation;

    MeReal ZZ2 = outN[2]*outN[2];
    if (ZZ2 > (MeReal)(0.999999))
        dimA = 2;
    else if (ZZ2 < (MeReal)(0.000001))
        dimA = 3; // usually reserved for sphere - need a different type?
    else
        dimA = 1;
    outDims = (2<<8)|dimA;

    // Cylinder's endcap separation:
    minCoord = ((lsVec3*)inTri->vertices[0])->operator[](2);
    maxCoord = ((lsVec3*)inTri->vertices[0])->operator[](2);
    if (((lsVec3*)inTri->vertices[1])->operator[](2) < minCoord)
        minCoord = ((lsVec3*)inTri->vertices[1])->operator[](2);

    if (((lsVec3*)inTri->vertices[1])->operator[](2) > maxCoord)
        maxCoord = ((lsVec3*)inTri->vertices[1])->operator[](2);

    if (((lsVec3*)inTri->vertices[2])->operator[](2) < minCoord)
        minCoord = ((lsVec3*)inTri->vertices[2])->operator[](2);

    if (((lsVec3*)inTri->vertices[2])->operator[](2) > maxCoord)
        maxCoord = ((lsVec3*)inTri->vertices[2])->operator[](2);

/*
    This test is redundant, since  the conditional if (separation-maxSeparation > eps), below will eliminate
    edge collisions which are too close to being a face collision.  I'm chucking it because the test seems
    to be failing, and throwing out needed edge normals.  BRG 24Apr02
 */
//    if (MeFabs((*inTri->normal)[2]) < 1-inEps)
    {
        // not already handled by triangle normal
        sumR = inHHCyl+(MeReal)(0.5)*(maxCoord-minCoord);
        normD = (MeReal)(0.5)*(maxCoord+minCoord);
        separation = MeFabs(normD)-sumR;
        if (separation-maxSeparation > inEps)
        {
            maxSeparation = separation;
            normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
            normInfo = (2<<2)|3;
            if (separation > inEps) return 0;
        }
    }

    aE[0] = lsVec3::abs(*(lsVec3*)edge[0]);
    aE[1] = lsVec3::abs(*(lsVec3*)edge[1]);
    aE[2] = lsVec3::abs(*(lsVec3*)edge[2]);

    sqE[0] = (*(lsVec3*)edge[0]).square_norm();
    sqE[1] = (*(lsVec3*)edge[1]).square_norm();
    sqE[2] = (*(lsVec3*)edge[2]).square_norm();

    // Edges crossed w/cyl axis:
    for (i = 0; i < 3; i++)
    { // Triangle edges
        int i1 = NextMod3(i);
        MeReal sR = (MeReal)(0.5)*(edge[i1][0]*edge[i][1]-
                                   edge[i1][1]*edge[i][0]);
        normD = ((lsVec3*)inTri->vertices[i])->operator[](0)*(*(lsVec3*)edge[i])[1]-
                ((lsVec3*)inTri->vertices[i])->operator[](1)*(*(lsVec3*)edge[i])[0]+sR;
        MeReal rLen = sqE[i]-aE[i][2]*aE[i][2];
        if (rLen > eps2)
        {
            rLen = MeRecipSqrt(rLen);
/*
    This test is redundant, since  the conditional if (separation-maxSeparation > eps), below will eliminate
    edge collisions which are too close to being a face collision.  I'm chucking it because the test seems
    to be failing, and throwing out needed edge normals.  BRG 24Apr02
            if (MeFabs(rLen*(*(lsVec3*)inTri->normal).dot(Vec3CrossAxis(*(lsVec3*)edge[i],2))) > 1-inEps)
            {
                // already handled by triangle normal
                continue;
            }
 */
            MeReal aNormD = MeFabs(normD)*rLen;
            MeReal rB = MeFabs(sR)*rLen;
            MeReal separation = aNormD-(rB+inRCyl);
            if (separation-maxSeparation > inEps)
            {
                maxSeparation = separation;
                normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
                normInfo = (2<<2)|i;
                nRLen = rLen;
                PN = rB-aNormD;
                if (separation > inEps) return 0;
            }
        }
    }

    outSep = maxSeparation;
    lsVec3 *posList = outPos;

    if(inTri->flags & kMcdTriangleUseSmallestPenetration)
    {
        if ((normInfo & 3) == 3)
        {
            // cylinder face separation
            //      MeI8 axis = (normInfo&0xC)>>2;
            //      if (axis == 2) {
            // end cap separation
            nPerp.setValue(1,0,0);
            nPara.setValue(0,normalSign,0);
            outN.setValue(0,0,normalSign);
            outPN = -inHHCyl;
            MeI16 dimB =
                (MeI16)(((lsVec3*)inTri->vertices[0])->operator[](2)-minCoord <= inEps)
                + (MeI16)(((lsVec3*)inTri->vertices[1])->operator[](2)-minCoord <= inEps)
                + (MeI16)(((lsVec3*)inTri->vertices[2])->operator[](2)-minCoord <= inEps)
                - 1;

            MCD_CHECK_ASSERT_(dimB >= 0 && dimB <= 2, "overlapCylTri");
            outDims = (dimB<<8)|2;
        }
#if 0
        else
        {
            // cylinder wall separation
            nPerp = nCW;
            nPara = cylAxis.cross(nPerp);
            outN = nCW;
        }
#endif
        else if(normInfo!=0xC)
        {
            // tri edge crossed with cylinder primary axis
            nPerp = Vec3CrossAxis(*(lsVec3*)edge[normInfo&3],2)*nRLen;
            nPara = AxisCrossVec3(2,nPerp);
            outN = normalSign*nPerp;
            outPN = PN;
            outDims = (1<<8)|3; // 3 normally reserved for sphere
        }
    }

    // cylinder "edges" vs. tri:
    lsVec3 axb[3];
    axb[0] = ((lsVec3*)inTri->vertices[0])->cross(*(lsVec3*)inTri->vertices[1]);
    axb[1] = ((lsVec3*)inTri->vertices[1])->cross(*(lsVec3*)inTri->vertices[2]);
    axb[2] = ((lsVec3*)inTri->vertices[2])->cross(*(lsVec3*)inTri->vertices[0]);
    const MeReal triD = ((lsVec3*)inTri->vertices[0])->dot((*(lsVec3*)inTri->normal));
    McdVanillaAddTriCylSegmentPoints(outPos,
        inRCyl,inHHCyl,nPerp, nPara,inTri,edge,axb,triD);
    // tri edges vs cylinder:
    if(inTri->flags & kMcdTriangleUseEdge0)
        McdVanillaAddCylEndSegmentPoints(outPos,
            *(lsVec3*)inTri->vertices[0],*(lsVec3*)edge[0],inRCyl, inHHCyl, 0.0, 1.0,scale);
    if(inTri->flags & kMcdTriangleUseEdge1)
        McdVanillaAddCylEndSegmentPoints(outPos,
            *(lsVec3*)inTri->vertices[1],*(lsVec3*)edge[1],inRCyl, inHHCyl, 0.0, 1.0,scale);
    if(inTri->flags & kMcdTriangleUseEdge2)
        McdVanillaAddCylEndSegmentPoints(outPos,
            *(lsVec3*)inTri->vertices[2],*(lsVec3*)edge[2],inRCyl, inHHCyl, 0.0, 1.0,scale);

    return outPos != posList;
}

/*
 * Cylinder to Tri List collision detection
 */

/*
   Utility function to produce a bsphere in local coords
*/

#if 0
static void McdModelGetBSphereInLocalFrame(MeMatrix4Ptr frame,
                   McdModelID m, MeVector3 center, MeReal* radius)
{
    lsTransform *framet = (lsTransform*)frame;
    lsVec3 scenter;
    McdModelGetBSphere(m,scenter,radius);
    framet->transform(scenter, (lsVec3*)center);
}
#endif

int MEAPI McdCylinderTriangleListIntersect( McdModelPair* p,
    McdIntersectResult *result )
{
    lsVec3 temp;
    lsTransform triToCyl;

    lsVec3 cylPosTrans, vector[4];
    McdUserTriangle ct;
    MeI16 dims;
    lsVec3 footprint[18], *verts, normal, v;
    MeReal separation, PN;
    McdUserTriangle *triNE;
    MeVector3 edge[3];
    int j;
    int count;

    McdCylinderID cylgeom = (McdCylinderID)McdModelGetGeometry(p->model1);
    McdTriangleList* triList = (McdTriangleList*)McdModelGetGeometry(p->model2);
    
    McdFramework *fwk = p->model1->frame;
    MeVector3 cylBSCentre;

    const MeReal eps = McdModelGetContactTolerance( p->model1 )
                     + McdModelGetContactTolerance( p->model2 );
    
    result->contactCount = 0;
    result->touch = 0;

    // Cylinder's global information
    MeReal cylRadius = McdCylinderGetRadius(cylgeom);
    MeReal cylHH = McdCylinderGetHalfHeight(cylgeom);
    MeReal cylBSRad;

    lsTransform &cylTM = *(lsTransform*)McdModelGetTransformPtr(p->model1);;
    lsTransform &triListTM = *(lsTransform*)McdModelGetTransformPtr(p->model2);
    
    McdCylinderGetBSphere(cylgeom,cylBSCentre,&cylBSRad);
    cylBSRad+=eps;

    /* Transform cyl into tri list space    */
    triListTM.inverseTransform(cylTM.t(), &cylPosTrans);
    
    triList->list = (McdUserTriangle *)MeMemoryALLOCA(triList->triangleMaxCount * sizeof(McdUserTriangle));
    count = (*triList->triangleListGenerator)(p, triList->list, cylPosTrans.v, cylBSRad+eps,
        triList->triangleMaxCount);

    ((lsVec3*)result->normal)->setValue(0,0,0);
    
    if(count==0)
        return 0;

    ct.normal = (MeVector3*)&vector[0];
    ct.vertices[0] = (MeVector3*)&vector[1];
    ct.vertices[1] = (MeVector3*)&vector[2];
    ct.vertices[2] = (MeVector3*)&vector[3];

    triToCyl.thisIsFirstThenInverseSecond(triListTM,cylTM);

    for(j=0; j<count && result->contactCount < 400; j++)
    {
        triNE = triList->list + j;
        
        /* Map vertices back into collision then box space  */
        triToCyl.transform((lsVec3 &)*triNE->vertices[0][0], (lsVec3*)ct.vertices[0]);
        triToCyl.transform((lsVec3 &)*triNE->vertices[1][0], (lsVec3*)ct.vertices[1]);
        triToCyl.transform((lsVec3 &)*triNE->vertices[2][0], (lsVec3*)ct.vertices[2]);
        triToCyl.transformWithoutTranslate((lsVec3 &)*triNE->normal[0], (lsVec3*)ct.normal);
        ct.flags = triNE->flags;
        ct.triangleData = triNE->triangleData;
        
        /* Edges */
        MeVector3Subtract(edge[0], *ct.vertices[1], *ct.vertices[0]);
        MeVector3Subtract(edge[1], *ct.vertices[2], *ct.vertices[1]);
        MeVector3Subtract(edge[2], *ct.vertices[0], *ct.vertices[2]);
        
        verts = footprint;
        
        // Overlap test, return overlap values
        if (McdVanillaOverlapCylTri(separation,normal,
                PN,verts,dims,eps,cylRadius,cylHH,&ct,edge,fwk->mScale)) 
        {
            // Transform normal into global frame
            lsVec3 globalNormal;
            cylTM.transformWithoutTranslate(normal,&globalNormal);
            
            if(!(ct.flags & kMcdTriangleTwoSided) && normal.dot(*(lsVec3*)ct.normal) < 0)
                globalNormal *= -1;

            // Transform footprint into global frame, and find depths
            lsVec3 *v = footprint;
            // McdContact *c = result->contacts+result->contactCount;
            while (v != verts && result->contactCount < 400) 
            {
                McdContact *c = result->contacts+result->contactCount++;

                //MeReal s = v->dot(normal)-PN;
                //if (s < eps*separation)
                {
                    lsVec3 *cpos = (lsVec3*)c->position;
                    cylTM.transform(*v,cpos);
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
                    v++;
                }
            }
            
            (*(lsVec3*)result->normal) += globalNormal;
        }
    }
    
    MeVector3Normalize((MeVector3Ptr)result->normal);    
    return result->touch = result->contactCount > 0;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Cylinder, TriangleList,1);
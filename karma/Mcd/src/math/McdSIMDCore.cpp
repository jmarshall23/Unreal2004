#error this file should not be in the build
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.3.8.2 $

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
#include "McdCheck.h"
#include "McdSIMDCore.h"

static const lsVec4 vec4minusone(-1,-1,-1,-1), vec4one(1,1,1,1), vec4zero(0,0,0,0);
static const lsVec3 vec3minusone(-1,-1,-1), vec3one(1,1,1), vec3zero(0,0,0);


static void Vec3Abs(lsVec3 &out, const lsVec3 &in)
{
    out[0] = MeFabs(in[0]);
    out[1] = MeFabs(in[1]);
    out[2] = MeFabs(in[2]);
}

static void Vec3Min(lsVec3 &out, const lsVec3 &in1, const lsVec3 & in2)
{
    out[0] = in1[0] < in2[0] ? in1[0] : in2[0];
    out[1] = in1[1] < in2[1] ? in1[1] : in2[1];
    out[2] = in1[2] < in2[2] ? in1[2] : in2[2];
}

static void Vec3Max(lsVec3 &out, const lsVec3 &in1, const lsVec3 & in2)
{
    out[0] = in1[0] > in2[0] ? in1[0] : in2[0];
    out[1] = in1[1] > in2[1] ? in1[1] : in2[1];
    out[2] = in1[2] > in2[2] ? in1[2] : in2[2];
}

static void Vec3AbsCross(lsVec3 &out, const lsVec3 &in1, const lsVec3 & in2)
{
    out[0] = in1[1] * in2[2] + in1[2] * in2[1];
    out[1] = in1[2] * in2[0] + in1[0] * in2[2];
    out[2] = in1[0] * in2[1] + in1[1] * in2[0];
}


static void Vec4Spread(lsVec4 &vec, MeReal r)
{
    vec[0] = vec[1] = vec[2] = vec[3] = r;
}


static void Vec4Abs(lsVec4 &out, const lsVec4 &in)
{
    out[0] = MeFabs(in[0]);
    out[1] = MeFabs(in[1]);
    out[2] = MeFabs(in[2]);
    out[3] = MeFabs(in[3]);
}

static void Vec4Min(lsVec4 &out, const lsVec4 &in1, const lsVec4 & in2)
{
    out[0] = in1[0] < in2[0] ? in1[0] : in2[0];
    out[1] = in1[1] < in2[1] ? in1[1] : in2[1];
    out[2] = in1[2] < in2[2] ? in1[2] : in2[2];
    out[3] = in1[3] < in2[3] ? in1[3] : in2[3];
}

static void Vec4Max(lsVec4 &out, const lsVec4 &in1, const lsVec4 & in2)
{
    out[0] = in1[0] > in2[0] ? in1[0] : in2[0];
    out[1] = in1[1] > in2[1] ? in1[1] : in2[1];
    out[2] = in1[2] > in2[2] ? in1[2] : in2[2];
    out[3] = in1[3] > in2[3] ? in1[3] : in2[3];
}
    


static void Vec4x3Transpose(lsVec4 out[3], const lsVec3 in[4])
{
    int i,j;
    for(i=0;i<4;i++)
        for(j=0;j<3;j++)
            out[j][i] = in[i][j];
}

static void AddTriBoxSegmentPoints(lsVec3* &outList, const lsVec3 &inRBox,
                                         const int i0, const int i1, const int i2,
                                         const CxTriangleNE &inTri, const lsVec3 axb[],
                                         const MeReal inTriD) 
{
    const MeReal r0 = inRBox[i0];
    const MeReal den = inTri.mNormal[i0];
    const MeReal recipDen = (MeReal)(1.0)/den;
    const bool ccw = den < (MeReal)(0.0);
    
    const lsVec4 m1(-1,-1,1,1);
    const lsVec4 m2(-1,1,1,-1);
    lsVec4 rV, d0,d1,d2;
    int j;
    
    
    lsVec3 x[3];
    x[0] = Vec3CrossAxis(inTri.mEdge[0],i0);
    x[1] = Vec3CrossAxis(inTri.mEdge[1],i0);
    x[2] = Vec3CrossAxis(inTri.mEdge[2],i0);
    
    x[0] *= inRBox;
    x[1] *= inRBox;
    x[2] *= inRBox;
    
    lsVec3 dn;
    dn[i1] = inRBox[i1]*inTri.mNormal[i1];
    dn[i2] = inRBox[i2]*inTri.mNormal[i2];

    Vec4Spread(d0,axb[0][i0]);
    Vec4Spread(d1,axb[1][i0]);
    Vec4Spread(d2,axb[2][i0]);
    d0.addMultiple(x[0][i1],m1);
    d0.addMultiple(x[0][i2],m2);
    d1.addMultiple(x[1][i1],m1);
    d1.addMultiple(x[1][i2],m2);
    d2.addMultiple(x[2][i1],m1);
    d2.addMultiple(x[2][i2],m2);    
    d0*=den;
    d1*=den;
    d2*=den;

    Vec4Spread(rV,inTriD);
    rV.addMultiple(inRBox[i1]*inTri.mNormal[i1], m1);
    rV.addMultiple(inRBox[i2]*inTri.mNormal[i2], m2);
    rV *= recipDen;

    for(j=0;j<4;j++)
    {
        if (d0[j] >= (MeReal)(0.0) &&
            d1[j] >= (MeReal)(0.0) &&
            d2[j] >= (MeReal)(0.0) && rV[j]>=-r0 && rV[j]<=r0) 
        {  // points at triangle
            outList->operator[](i0) = rV[j];
            outList->operator[](i1) = -m1[j]*inRBox[i1];
            outList->operator[](i2) = -m2[j]*inRBox[i2];
            outList++;
        }
    }
}


static int SegmentCubeIntersect(MeReal *tInMax, MeReal *tOutMin, const lsVec3 &orig,
                                const lsVec3 &disp, const lsVec3 &invDisp,
                                const lsVec3 &inR) {
    const MeReal eps = (MeReal)1e-6;
    int j;
    lsVec3 z0,z1,adisp, aorig, epsV(eps,eps,eps);
    lsVec3 tInV, tOutV;
    
    // Early out for when one of the lines is axis-perpendicular 
    // and the line origin for that axis is outside the box

    Vec3Abs(adisp, disp);
    Vec3Abs(aorig, orig);

    int small = adisp <  epsV;

    if(small & (aorig > inR))
        return 0;

    Vec3Abs(z0, invDisp);
    z0*=inR;

    (z1 = orig) *= -invDisp;

    tInV = z1-z0;
    tOutV = z1+z0;

    for (j = 0; j < 3; j++) 
    { // j = axis of box A        
        if(!(small&(1<<j)))
        {
            if (tInV[j] > *tInMax) *tInMax = tInV[j];
            if (tOutV[j] < *tOutMin) *tOutMin = tOutV[j];
            if (*tOutMin < *tInMax) 
                return 0; 
        }
    }
    return 1;
}


static void AddBoxEndSegmentPoints(lsVec3* &outList, const lsVec3 &orig, const lsVec3 &disp,
                            const lsVec3 &inR) 
{
  MeReal tIn = (MeReal)(0.0);
  MeReal tOut = (MeReal)(1.0);
  const lsVec3 invDisp((MeReal)(1.0)/disp[0],
                       (MeReal)(1.0)/disp[1],
                       (MeReal)(1.0)/disp[2]);
  if (SegmentCubeIntersect(&tIn,&tOut,orig,disp,invDisp,inR)) 
  {
    *outList++ = orig+tIn*disp;
    if (tOut < (MeReal)1.0) 
      *outList++ = orig+tOut*disp;
  }
}


void McdSIMDCore::BoxTriIntersect(lsVec3* &outList, const lsVec3 &inRBox, const CxTriangleNE &inTri) 
{
    AddBoxEndSegmentPoints(outList,*inTri.mVertex[0],inTri.mEdge[0],inRBox);
    AddBoxEndSegmentPoints(outList,*inTri.mVertex[1],inTri.mEdge[1],inRBox);
    AddBoxEndSegmentPoints(outList,*inTri.mVertex[2],inTri.mEdge[2],inRBox);
    
    lsVec3 axb[3];
    axb[0] = inTri.mVertex[0]->cross(*inTri.mVertex[1]);
    axb[1] = inTri.mVertex[1]->cross(*inTri.mVertex[2]);
    axb[2] = inTri.mVertex[2]->cross(*inTri.mVertex[0]);
    
    const MeReal triD = inTri.mVertex[0]->dot(inTri.mNormal);
    
    if (inTri.mNormal[0] != (MeReal)(0.0)) {
        AddTriBoxSegmentPoints(outList,inRBox,0,1,2,inTri,axb,triD); // x-direction edges
    }
    if (inTri.mNormal[1] != (MeReal)(0.0)) {
        AddTriBoxSegmentPoints(outList,inRBox,1,2,0,inTri,axb,triD); // y-direction edges
    }
    if (inTri.mNormal[2] != (MeReal)(0.0)) {
        AddTriBoxSegmentPoints(outList,inRBox,2,0,1,inTri,axb,triD); // z-direction edges
    }
}




bool McdSIMDCore::OverlapOBBTri(MeReal &outSep, lsVec3 &outN, MeReal &outPN, lsVec3* &outPos,
                   MeI16 &outDims, const MeReal inEps, const lsVec3 &inR,
                   const CxTriangleNE &inTri) {
    int i;
    int j;
    
    const MeReal eps = inEps<(MeReal)(0.0)?(MeReal)(0.0):inEps;
    const MeReal eps2 = eps*eps;
    const lsVec3 epsV(inEps, inEps, inEps);
    
    // Find maximum separation (early-out for positive separation)
    MeReal maxSeparation;
    MeReal normalSign;
    MeU8 normInfo;
    MeReal nRLen = (MeReal)(1.0);
    lsVec3 aE[3];
    MeReal sqE[3];
    MeReal PN;
    lsVec3 minCoord,maxCoord, sumRV, normDV, aNormDV, sepV;
    
    lsVec3 aNorm;
    
    Vec3Abs(aNorm,inTri.mNormal);
    
    /************
    normInfo = (inClientInfo<<2) | thisClientInfo;
    xInfo = { 0,1,2 => axis #, 3 => other info gives normal }
    If both thisClientInfo and inClientInfo are less than 3,
    then normal is cross product of two axes.
    ************/
    
    // Face separation (face of triangle):
    MeReal sumR = inR[0]*aNorm[0]+inR[1]*aNorm[1]+inR[2]*aNorm[2];
    MeReal normD = inTri.mNormal.dot(*inTri.mVertex[0]);
    MeReal aNormD = MeFabs(normD);
    maxSeparation = aNormD-sumR;
    if (maxSeparation > inEps)  
        return false; 

    PN = -aNormD;
    normalSign = normD;
    normInfo = 3;

    // Face separation (face of OBB):

    Vec3Min(minCoord,*inTri.mVertex[0],*inTri.mVertex[1]);
    Vec3Min(minCoord,minCoord,*inTri.mVertex[2]);

    Vec3Max(maxCoord,*inTri.mVertex[0],*inTri.mVertex[1]);
    Vec3Max(maxCoord,maxCoord,*inTri.mVertex[2]);

    sumRV = inR+(maxCoord-minCoord) * (MeReal)(0.5);
    normDV = (maxCoord+minCoord) * (MeReal)(0.5);

    Vec3Abs(aNormDV, normDV);
    sepV = aNormDV - sumRV;

    if(sepV > epsV)
        return false;

    for (i = 0; i < 3; i++) 
    {
        if (sepV[i] > maxSeparation) 
        {
            maxSeparation = sepV[i];
            PN = -inR[i]-sepV[i];
            normalSign = normDV[i];
            normInfo = 0xC|i;
        }
    }

    Vec3Abs(aE[0],inTri.mEdge[0]);
    Vec3Abs(aE[1],inTri.mEdge[1]);
    Vec3Abs(aE[2],inTri.mEdge[2]);

    sqE[0] = inTri.mEdge[0].square_norm();
    sqE[1] = inTri.mEdge[1].square_norm();
    sqE[2] = inTri.mEdge[2].square_norm();
    
    // Edge separation:
    for (i = 0; i < 3; i++) { // Triangle edges
        int i1 = NextMod3(i);
        const lsVec3 &em = inTri.mEdge[i];
        const lsVec3 &en = inTri.mEdge[i1];
        const lsVec3 &vm = *inTri.mVertex[i];
        const lsVec3 &vn = *inTri.mVertex[i1];

        lsVec3 sRV, rBV, rLenV;

        lsVec3::cross(en, em, &sRV);
        sRV *= (MeReal)0.5;
        Vec3Abs(rBV, sRV);

        Vec3AbsCross(sumRV,inR,aE[i]);
        sumRV+=rBV;

        lsVec3::cross(vm, em, &normDV);
        normDV+=sRV;       
        Vec3Abs(aNormDV,normDV);
        sepV = aNormDV - sumRV;

        // should clean this up as with OBB-box.

        for (j = 0; j < 3; j++) 
        { // Box axes
            MeReal rLen = sqE[i]-aE[i][j]*aE[i][j];
            if (rLen > eps2) {
                rLen = MeSqrt(rLen);

                if (sepV[j]-maxSeparation*rLen > eps*rLen) 
                {
                    rLen = (MeReal)1.0/rLen;
                    maxSeparation = sepV[j]*rLen;
                    if (maxSeparation > inEps) 
                        return false;

                    PN = (rBV[j]-aNormDV[j])*rLen;
                    normalSign = normDV[j];
                    normInfo = (i<<2)|j;
                    nRLen = rLen;
                }
            }
        }
    }
    
    normalSign = normalSign > (MeReal)0 ? (MeReal)-1 : (MeReal)1;
    outSep = maxSeparation;
    
    // normal points from Tri to OBB
    if (normInfo == 3) 
    { // Normal is from Tri
        outN = normalSign*inTri.mNormal;
        outPN = outN.dot(*inTri.mVertex[0])-maxSeparation;
        MeI16 dimA = (MeI16)(aNorm[0] < (MeReal)(1.0e-4))+
                     (MeI16)(aNorm[1] < (MeReal)(1.0e-4))+
                     (MeI16)(aNorm[2] < (MeReal)(1.0e-4));
        outDims = (2<<8)|dimA;
    }
    else if ((normInfo&0xC) == 0xC) 
    { // Normal is from OBB
        MeI8 axis = normInfo&3;
        outN.setValue((MeReal)(0.0),(MeReal)(0.0),(MeReal)(0.0));
        outN[axis] = normalSign;
        outPN = -inR[axis]-maxSeparation;
        MeI16 dimB = (MeI16)(inTri.mVertex[0]->operator[](axis)-minCoord[axis] <= eps)+
            (MeI16)(inTri.mVertex[1]->operator[](axis)-minCoord[axis] <= eps)+
            (MeI16)(inTri.mVertex[2]->operator[](axis)-minCoord[axis] <= eps)-1;
        MCD_CHECK_ASSERT_(dimB >= 0 && dimB <= 2, "overlapOBBTri");
        outDims = (dimB<<8)|2;
    } 
    else
    { // Normal is from crossed edges
        if (normalSign > (MeReal)(0.0)) 
            outN = Vec3CrossAxis(inTri.mEdge[(normInfo&0xC)>>2],normInfo&3);
        else 
            outN = AxisCrossVec3(normInfo&3,inTri.mEdge[(normInfo&0xC)>>2]);
        outPN = PN;
        outDims = (1<<8)|1;
        outN *= nRLen;
    }
    
    lsVec3 *posList = outPos;
    BoxTriIntersect(outPos,inR,inTri);
    
    return outPos != posList;
}



static int SegmentCubeIntersectBy4(lsVec4 &in, lsVec4 &out, const lsVec4 orig[3], const lsVec3 &disp, const lsVec3 &inR)
{
    const MeReal eps = (MeReal)1e-6;
    lsVec4 RV, axin, axout;
    int res = 0xf;
    MeReal invdisp;
    int j;

    for(j=0;j<3;j++)
    {
        Vec4Spread(RV,inR[j]);
        if(MeFabs(disp[j])>eps)
        {
            invdisp = 1/disp[j]; // this is needed almost always, so should probably preschedule
            axin =  orig[j] * -invdisp - RV * MeFabs(invdisp);
            axout = orig[j] * -invdisp + RV * MeFabs(invdisp);
            Vec4Max(in, in, axin);
            Vec4Min(out,out,axout);
        }
        else
        {
            lsVec4 aorig;
            Vec4Abs(aorig,orig[j]);
            res &= aorig <= RV;
        }
    }

    return res & in <= out;
}


static void AddBoxBoxClippedPointsBy4(lsVec3 *&outList, const lsVec3 orig[4], const lsVec3 &disp, const lsVec3 &inR)
{
    lsVec4 in = vec4minusone,out = vec4one;
    lsVec4 o[3];
    int i;

    Vec4x3Transpose(o,orig);

    int flags = SegmentCubeIntersectBy4(in,out,o,disp,inR);

    if(flags)
    {
        int flagsin = flags & in > vec4minusone;
        int flagsout = flags & out < vec4one;

        for(i=0;i<4;i++)
        {
            if(flagsin&(1<<i))
                *outList++=orig[i]+in[i]*disp;
            if(flagsout&(1<<i))
                *outList++=orig[i]+out[i]*disp;
        }
    }
}


static void AddBoxBoxSegmentPointsBy4(lsVec3 *&outList, const lsVec3 orig[4], const lsVec3 &disp, const lsVec3 &inR)
{
    lsVec4 in = vec4minusone,out = vec4one;
    lsVec4 o[3];
    int i;
    
    Vec4x3Transpose(o,orig);
    int flags = SegmentCubeIntersectBy4(in,out,o,disp,inR);
    if(flags)
    {
        for(i=0;i<4;i++)
        {
            if(flags&(1<<i))  
            {
                *outList++=orig[i]+in[i]*disp;
                *outList++=orig[i]+out[i]*disp;
            }
        }
    }
}




void McdSIMDCore::AddBoxBoxClippedPoints(lsVec3* &outList, const lsVec3 &orig,
                                         const lsVec3 &disp, const lsVec3 &invDisp,
                                         const lsVec3 &inR) {
    
    MeReal tIn = -1, tOut = 1;
    if (SegmentCubeIntersect(&tIn,&tOut,orig,disp,invDisp,inR)) {
        if (tIn > -1) outList++->setValue(orig[0]+tIn*disp[0],
            orig[1]+tIn*disp[1],
            orig[2]+tIn*disp[2]);
        if (tOut < 1) outList++->setValue(orig[0]+tOut*disp[0],
            orig[1]+tOut*disp[1],
            orig[2]+tOut*disp[2]);
    }
}

void McdSIMDCore::AddBoxBoxSegmentPoints(lsVec3* &outList, const lsVec3 &orig,
                                         const lsVec3 &disp,
                                         const lsVec3 &invDisp, const lsVec3 &inR) {
    MeReal tIn = -1,tOut = 1;
    if (SegmentCubeIntersect(&tIn,&tOut,orig,disp,invDisp,inR)) {
        outList++->setValue(orig[0]+tIn*disp[0],
            orig[1]+tIn*disp[1],
            orig[2]+tIn*disp[2]);
        outList++->setValue(orig[0]+tOut*disp[0],
            orig[1]+tOut*disp[1],
            orig[2]+tOut*disp[2]);
    }
}





void McdSIMDCore::BoxEdgeBoxIntersect(lsVec3* &outList, const lsTransform &tAB,
                                      const lsVec3 &inRA, const lsVec3 &inRB) {
    lsVec3 d0(tAB.axis(0)), d1(tAB.axis(1)), d2(tAB.axis(2));
    
    d0*=inRB[0];
    d1*=inRB[1];
    d2*=inRB[2];
    lsVec3 orig[4];
    
    const lsVec3 t = tAB.t();
    
    // x-direction edges:
    
    orig[0] = t+d1+d2;
    orig[1] = t-d1+d2;
    orig[2] = t+d1-d2;
    orig[3] = t-d1-d2;
    
    AddBoxBoxClippedPointsBy4(outList,orig,d0,inRA);
    
    // y-direction edges:
    
    orig[0] = t+d0+d2;
    orig[1] = t-d0+d2;
    orig[2] = t+d0-d2;
    orig[3] = t-d0-d2;
    
    AddBoxBoxClippedPointsBy4(outList,orig,d1,inRA);
    
    orig[0] = t+d0+d1;
    orig[1] = t-d0+d1;
    orig[2] = t+d0-d1;
    orig[3] = t-d0-d1;
    
    AddBoxBoxSegmentPointsBy4(outList,orig,d2,inRA);
}



bool McdSIMDCore::OverlapOBBs(MeReal &outSep, lsVec3 &outN, MeReal &outPN, MeI16 &outDims,
                              const MeReal inEps, const lsVec3 &inR1, const lsVec3 &inR2,
                              const lsTransform &inT12)
{
    int i;
    int j;
    
    const MeReal eps2 = inEps * inEps;
    
    bool apart = true;
    MeReal maxSeparation = -MEINFINITY;
    MeReal PN;
    MeReal saveNormD = 1.0;
    MeU8 normInfo = 0xF;
    MeReal nRLen = (MeReal)(1.0);
    lsTransform inT21, aT12, aT21;
    
    const lsVec3 epsV(inEps,inEps,inEps), eps2V(eps2,eps2,eps2);
    lsVec3 sumRV, apos, sepV, normDV, aNormDV, rAV, rBV, sqsepV;
    
    
    inT21.inverseOf(inT12); // actually only need the rotation part!
    
    // Relative center of 2nd cuboid
    const lsVec3 &pos = inT12.t();
    
    // Get the absolute values of the rotation matrix elements
    lsAbsMat(inT12,&aT12);
    lsAbsMat(inT21,&aT21);
    
    // Find maximum separations: (early-out for positive separation)
    
    // Face separation (face of A):

    aT12.transformWithoutTranslate(inR2, &sumRV);
    sumRV += inR1;
    Vec3Abs(apos,pos);
    sepV = apos - sumRV;
    if(sepV >  epsV)
        return false;
    
    for (i = 0; i < 3; i++) 
    {
        if (sepV[i] > maxSeparation) 
        {
            maxSeparation = sepV[i];
            PN = -inR1[i]-sepV[i];
            saveNormD = pos[i];
            normInfo = 0xC|i;
        }
    }
    
    
    // Face separation (face of B):
    aT21.transformWithoutTranslate(inR1,&sumRV);
    sumRV+=inR2;
    inT21.transformWithoutTranslate(pos,&normDV);
    Vec3Abs(aNormDV, normDV);
    sepV = aNormDV - sumRV;
    if(sepV > epsV)
        return false;
    
    for (i = 0; i < 3; i++) 
    {
        if (sepV[i] > maxSeparation) 
        {
            maxSeparation = sepV[i];
            PN = inR2[i]-aNormDV[i];
            saveNormD = normDV[i];
            normInfo = (i<<2)|3;
        }
    }
    

    // crossed edges of A and B
    
    MeReal threshold = maxSeparation + inEps;
    lsVec3 rLenV;
    
    for (j = 0; j < 3; j++) 
    {
        const int j1 = NextMod3(j);
        const int j2 = NextMod3(j1);
        const lsVec3 &aj = aT12.axis(j);
        const lsVec3 &aj1 = aT12.axis(j1);
        const lsVec3 &aj2 = aT12.axis(j2);
        const lsVec3 &tj = inT12.axis(j);
        
        Vec3AbsCross(rAV,inR1,aj);
        (rBV = aj2)*=inR2[j1];
        rBV.addMultiple(inR2[j2], aj1);
        lsVec3::cross(tj,pos,&normDV);
        Vec3Abs(aNormDV, normDV);
        sepV = aNormDV - rAV - rBV;
        
        (rLenV = tj) *= tj;
        rLenV = vec3one - rLenV;
                
        int big = rLenV > eps2V;
        
        // early out if any of the rLenV[i] > eps2 && sepV[i]/(sqrt(rLenV[i])) > eps

        ((sqsepV = sepV) *= sepV);
        if(big & sepV > vec3zero & sqsepV > rLenV*eps2)
            return false;
            
        for (i = 0; i < 3; i++) 
        {
            if (big &(1<<i))  
            {
                const MeReal rLen = MeSqrt(rLenV[i]);
                if (sepV[i] > threshold * rLen) 
                {
                    nRLen = (MeReal)(1.0)/rLen;
                    saveNormD = normDV[i];
                    normInfo = (j<<2)|i;
                    maxSeparation = sepV[i] * nRLen;
                    threshold = maxSeparation;
                    PN = (rBV[i]-aNormDV[i])*nRLen;
                }
            }
        }
    }
    
    MCD_CHECK_ASSERT_(normInfo != 0xF, "overlapOBBs");
    
    outSep = maxSeparation;
    outPN = PN;
    
    // normal points from OBBB to OBBA
    if ((normInfo&0xC) == 0xC) { // Normal is from OBBA
        MeI8 axis = normInfo&3;
        MeI16 dimB = (MeI16)(aT12.axis(0)[axis] < (MeReal)(1.0e-4))+
            (MeI16)(aT12.axis(1)[axis] < (MeReal)(1.0e-4))+
            (MeI16)(aT12.axis(2)[axis] < (MeReal)(1.0e-4));
        
        outN.setValue((MeReal)(0.0),(MeReal)(0.0),(MeReal)(0.0));
        outN[axis] = saveNormD < 0 ? (MeReal)(1.0) : (MeReal)(-1.0);
        outDims = (dimB<<8)|2;
        
    } else if ((normInfo&3) == 3) { // Normal is from OBBB
        
        MeI8 axis = (normInfo&0xC)>>2;
        MeI16 dimA = (MeI16)(aT12.axis(axis)[0] < (MeReal)(1.0e-4))+
            (MeI16)(aT12.axis(axis)[1] < (MeReal)(1.0e-4))+
            (MeI16)(aT12.axis(axis)[2] < (MeReal)(1.0e-4));
        outN = inT12.axis(axis);
        if(saveNormD>0) outN *= (MeReal)(-1.0);
        outDims = (2<<8)|dimA;
        
    } else { // normal is from crossed edges
        
        if (saveNormD < (MeReal)(0.0)) {
            outN = AxisCrossVec3(normInfo&3,inT12.axis((normInfo&0xC)>>2));
        } else {
            outN = Vec3CrossAxis(inT12.axis((normInfo&0xC)>>2),normInfo&3);
        }
        outN *= nRLen;
        outDims = (1<<8)|1;
    }
    
    return true;
}

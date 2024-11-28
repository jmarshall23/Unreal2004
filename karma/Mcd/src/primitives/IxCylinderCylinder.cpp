/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.46.2.3 $

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

#include <vectormath.h>
#include <McdProfile.h>

#include <McdCylinder.h>
#include "McdCheck.h"
#include <McdModel.h>
#include <McdContact.h>
#include <McdInteractionTable.h>
#include "MovingBoxBoxIntersect.h"
#include <McdPrimitives.h>

#include <lsTransform.h>
#include "mesffnmin.h"

static inline bool McdVanillaSegmentCylinderIntersect(
    MeReal &tInMax,MeReal &tOutMin,
    const lsVec3 &orig,const lsVec3 &disp,
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

static inline void McdVanillaAddBoxCylClippedPoints(lsVec3* &outList,
    const lsVec3 &orig, const lsVec3 &disp, const MeReal inR,
    const MeReal inHH, const MeReal minT, const MeReal maxT, const MeReal scale)
{
    MeReal tIn = minT;
    MeReal tOut = maxT;

    if (McdVanillaSegmentCylinderIntersect(tIn,tOut,orig,disp,inR,inHH,scale))
    {
        if (tIn > minT)
            *outList++ = orig+tIn*disp;

        if (tOut < maxT)
            *outList++ = orig+tOut*disp;
    }
}

static void McdVanillaAddBoxCylSegmentPoints(lsVec3* &outList,
    const lsVec3 &orig,const lsVec3 &disp,const MeReal inR,
    const MeReal inHH,const MeReal minT,const MeReal maxT, const MeReal scale)
{
    MeReal tIn = minT;
    MeReal tOut = maxT;

    if (McdVanillaSegmentCylinderIntersect(tIn,tOut,orig,disp,inR,inHH,scale))
    {
        *outList++ = orig+tIn*disp;
        *outList++ = orig+tOut*disp;
    }
}

/*
    Cylinder to Cylinder collision detection
*/

static const MeReal norm_eps = (MeReal) 1.0e-15f;

void CylCylIntersect(lsVec3* &outList, const lsTransform &tAB,
    const MeReal inR1, const MeReal inHH1,
    const lsVec3 &perp1, const lsVec3 &para1, const bool doCyl1,
    const MeReal inR2, const MeReal inHH2,
    const lsVec3 &perp2, const lsVec3 &para2, const bool doCyl2,
    const MeReal scale)
{
    lsTransform tBA;    // = inverse of tAB
    tBA.inverseOf(tAB);
    
    // Proper cylinder must be axis-aligned
    
    if (doCyl2)
    {
        // Cylinder 1 axis aligned, cylinder 2 not:
        const lsVec3 d0 = inR2*perp2;
        const lsVec3 d1 = inR2*para2;
        const lsVec3 d2 = inHH2*tAB.axis(2);
        
        // perp:
        McdVanillaAddBoxCylClippedPoints(outList,
            tAB.t()+d2,d0,inR1,inHH1,-1.0,1.0,scale);
        McdVanillaAddBoxCylClippedPoints(outList,
            tAB.t()-d2,d0,inR1,inHH1,-1.0,1.0,scale);
        
        // para:
#if 0
        AddBoxCylClippedPoints(outList,tAB.t()+d2,d1,inR1,inHH1,-1.0,1.0);
        AddBoxCylClippedPoints(outList,tAB.t()-d2,d1,inR1,inHH1,-1.0,1.0);
#endif
        
        // axis:
        McdVanillaAddBoxCylSegmentPoints(outList,
            tAB.t()+d0,d2,inR1,inHH1,-1.0,1.0,scale);
        McdVanillaAddBoxCylSegmentPoints(outList,
            tAB.t()-d0,d2,inR1,inHH1,-1.0,1.0,scale);
        McdVanillaAddBoxCylSegmentPoints(outList,
            tAB.t()+d1,d2,inR1,inHH1,-1.0,1.0,scale);
        McdVanillaAddBoxCylSegmentPoints(outList,
            tAB.t()-d1,d2,inR1,inHH1,-1.0,1.0,scale);
    }
    
    if (doCyl1) {
        lsVec3 bVertList[12];
        lsVec3 *bVerts = bVertList;
        
        // Cylinder 2 axis aligned, cylinder 1 not:
        lsVec3 e0;
        tBA.transformWithoutTranslate(perp1,&e0);
        e0 *= inR1;
        lsVec3 e1;
        tBA.transformWithoutTranslate(para1,&e1);
        e1 *= inR1;
        const lsVec3 e2 = inHH1*tBA.axis(2);
        
        // perp:
        McdVanillaAddBoxCylClippedPoints(bVerts,
            tBA.t()+e2,e0,inR2,inHH2,-1.0,1.0,scale);
        McdVanillaAddBoxCylClippedPoints(bVerts,
            tBA.t()-e2,e0,inR2,inHH2,-1.0,1.0,scale);
        
        // para:
#if 0
        AddBoxCylClippedPoints(bVerts,tBA.t()+e2,e1,inR2,inHH2,-1.0,1.0);
        AddBoxCylClippedPoints(bVerts,tBA.t()-e2,e1,inR2,inHH2,-1.0,1.0);
#endif
        
        // axis:
        McdVanillaAddBoxCylSegmentPoints(bVerts,
            tBA.t()+e0,e2,inR2,inHH2,-1.0,1.0,scale);
        McdVanillaAddBoxCylSegmentPoints(bVerts,
            tBA.t()-e0,e2,inR2,inHH2,-1.0,1.0,scale);
        McdVanillaAddBoxCylSegmentPoints(bVerts,
            tBA.t()+e1,e2,inR2,inHH2,-1.0,1.0,scale);
        McdVanillaAddBoxCylSegmentPoints(bVerts,
            tBA.t()-e1,e2,inR2,inHH2,-1.0,1.0,scale);
        
        lsVec3 *vert = bVertList;
        while (vert != bVerts)
            tAB.transform(*vert++,outList++);
    }
    
    // also need to solve for end cap/cyl intersection!!!!!
    // OUCH!!!!!!!
}

static MeReal dispDotX1 = 0.0;
static MeReal dispDotY1 = 0.0;
static MeReal dispDotX2 = 0.0;
static MeReal dispDotY2 = 0.0;
static MeReal X1DotX2 = 0.0;
static MeReal X1DotY2 = 0.0;
static MeReal Y1DotX2 = 0.0;
static MeReal Y1DotY2 = 0.0;
static MeReal cylR1 = 0.0;
static MeReal cylR2 = 0.0;

MeReal EndCapMin(MeReal * const theta) {
    const MeReal c1 = MeCos(theta[0]);
    const MeReal s1 = MeSin(theta[0]);
    const MeReal c2 = MeCos(theta[1]);
    const MeReal s2 = MeSin(theta[1]);
    
    return cylR2*(dispDotX2*c2+dispDotY2*s2)-cylR1*(dispDotX1*c1+dispDotY1*s1)
        -cylR1*cylR2*(c1*c2*X1DotX2+c1*s2*X1DotY2+s1*c2*Y1DotX2+s1*s2*Y1DotY2);
}

void EndCapMinGrad(MeReal * grad, MeReal * const theta) {
    const MeReal c1 = MeCos(theta[0]);
    const MeReal s1 = MeSin(theta[0]);
    const MeReal c2 = MeCos(theta[1]);
    const MeReal s2 = MeSin(theta[1]);
    
    grad[0] = -cylR1*(dispDotY1*c1-dispDotX1*s1)
        -cylR1*cylR2*(c1*c2*Y1DotX2+c1*s2*Y1DotY2-s1*c2*X1DotX2-s1*s2*X1DotY2);
    grad[1] =  cylR2*(dispDotY2*c2-dispDotX2*s2)
        -cylR1*cylR2*(c1*c2*X1DotY2+s1*c2*Y1DotY2-c1*s2*X1DotX2-s1*s2*Y1DotX2);
}

inline void CylPerpAndPara(lsVec3 &perp, lsVec3 &para,
  const lsVec3 &axis, const lsVec3 &n,const lsVec3 &disp)
{
    para = axis.cross(n);

    if (para.square_norm() < norm_eps)
    {
        para = axis.cross(disp);
        if (para.square_norm() < norm_eps)
        {
            int i = 0;

            if (axis[1] < axis[0]) i = 1;
            if (axis[2] < axis[i]) i = 2;

            para = Vec3CrossAxis(axis,i);
        }
    }
    para.normalize();
    perp = para.cross(axis);
}

bool OverlapCylCyl(MeReal &outSep,
    lsVec3 &outN, MeReal &outPN, lsVec3* &outPos, MeI16 &outDims,
    const MeReal inEps, const MeReal inR1, const MeReal inHH1,
    const MeReal inR2, const MeReal inHH2, const lsTransform &inT12, const MeReal scale) 
{
   
    lsVec3 nParaN1;
    lsVec3 nPerpN1;
    lsVec3 nParaN2;
    lsVec3 nPerpN2;
    lsVec3 nA1E2;
    lsVec3 nA2E1;
    lsVec3 nParaA1E2;
    lsVec3 nParaA2E1;
    lsVec3 nPerpA1E2;
    lsVec3 nPerpA2E1;
    lsVec3 nW1;
    lsVec3 nW2;
    lsVec3 nParaW1;
    lsVec3 nParaW2;
    lsVec3 nPerpW1;
    lsVec3 nPerpW2;
    lsVec3 nAA;
    lsVec3 nE1E2;
    lsVec3 pE1E2;
    MeReal rCyl;
    MeReal sumR;
    MeReal normD;
    MeReal normalSign;
    MeReal separation;
    MeReal maxSeparation;
    bool apart;
    MeI8 normInfo = 0xF;
    
    lsVec3 c;
    lsVec3 z;
    MeReal disc;
    MeReal dCap;
    MeReal nCapZ;
    
    lsVec3 disp;
    MeReal z1,z2;
    lsVec3 p;
    
    MeI8 axis1,axis2;
    MeI8 minAxis1,minAxis2;
    MeReal minCapD;
    
    MeReal rc1;
    MeReal rc2;
    
    lsVec3 para1;
    lsVec3 para2;
    
    lsVec3 p1;
    lsVec3 p2;
    MeReal theta[2];
    MeReal d2Min;
    
    lsVec3 zCap;
    MeReal zCapZ;
    
    MeReal aNormD;
    MeReal PN;
    
    MeReal sinTheta;
    
    // The ubiquitous axis x axis:
    lsVec3 aa = AxisCrossVec3(2,inT12.axis(2));
    
    // End-cap from cylinder 1:
    nParaN2 = -aa;
    if (nParaN2.square_norm() < norm_eps) {
        nParaN2.setValue(0.0,1.0,0.0);
        nPerpN2.setValue(1.0,0.0,0.0);
    } else {
        nParaN2.normalize();
        nPerpN2 = nParaN2.cross(inT12.axis(2));
    }
    rCyl = MeFabs(inT12.axis(2)[2])*inHH2+MeFabs(nPerpN2[2])*inR2;
    sumR = inHH1+rCyl;
    normD = inT12.t()[2];
    maxSeparation = MeFabs(normD)-sumR;
    normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
    normInfo = 0xC;
    PN = -inHH1-maxSeparation;
    apart = (maxSeparation > inEps);
    if (apart) { goto done; }
    
    // End-cap from cylinder 2:
    nParaN1 = aa;
    if (nParaN1.square_norm() < norm_eps) {
        nParaN1 = inT12.axis(1);
        nPerpN1 = inT12.axis(0);
    } else {
        nParaN1.normalize();
        nPerpN1 = Vec3CrossAxis(nParaN1,2);
    }
    rCyl = MeFabs(inT12.axis(2)[2])
        *inHH1+MeFabs(nPerpN1.dot(inT12.axis(2)))*inR1;
    sumR = inHH2+rCyl;
    normD = inT12.t().dot(inT12.axis(2));
    aNormD = MeFabs(normD);
    separation = aNormD-sumR;
    if (separation > maxSeparation)
    {
        maxSeparation = separation;
        normalSign = normD > (MeReal)(0.0)
            ? (MeReal)(-1.0) : (MeReal)(1.0);
        normInfo = 3;
        PN = inHH2-aNormD;
        apart = (maxSeparation > inEps);
        if (apart) { goto done; }
    }
    
    // Axis x axis:
    nAA = aa;
    if (nAA.square_norm() < norm_eps)
    {
        nAA = inT12.t();
        nAA[2] = 0.0;
    }
    if (nAA.square_norm() >= norm_eps)
    {
        nAA.normalize();
        sumR = inR1+inR2;
        normD = inT12.t().dot(nAA);
        separation = MeFabs(normD)-sumR;
        if (separation > maxSeparation) {
            maxSeparation = separation;
            normalSign = normD > (MeReal)(0.0)
                ? (MeReal)(-1.0) : (MeReal)(1.0);
            normInfo = (2<<2)|2;
            PN = -inR1-separation;
            apart = (maxSeparation > inEps);
            if (apart) { goto done; }
        }
    }
    
    // "First kind" of end-cap tangent tests:
    nCapZ = inT12.axis(2)[2];
    
    dCap = inT12.t().dot(inT12.axis(2));
    
    // Axis of 1 x End-cap tangent of 2:
    disc = dCap-inT12.t()[2]*nCapZ;
    c = inT12.t();
    if (disc > 0.0) {
        c -= inHH2*inT12.axis(2);
        dCap -= inHH2;
    } else {
        c += inHH2*inT12.axis(2);
        dCap += inHH2;
    }
    if (MeFabs(nCapZ) > 1.0e-8) {
        // intersect axis with plane of endcap
        p.setValue(0.0,0.0,dCap/nCapZ);
        p -= c;
        p.normalize();
        p *= inR2;
        // p is now on the end cap boundary, closest to the axis.
        p[2] = 0.0;
        nW1 = p;
        if (nW1.square_norm() > norm_eps) {
            nW1.normalize();
            nParaW2 = inT12.axis(2).cross(nW1);
            if (nParaW2.square_norm() > norm_eps) {
                nParaW2.normalize();
                nPerpW2 = nParaW2.cross(inT12.axis(2));
                rCyl = MeFabs(inT12.axis(2).dot(nW1))*inHH2+MeFabs(nPerpW2.dot(nW1))*inR2;
                sumR = inR1+rCyl;
                normD = inT12.t().dot(nW1);
                separation = MeFabs(normD)-sumR;
                if (separation > maxSeparation) {
                    maxSeparation = separation;
                    normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
                    normInfo = (1<<2)|2;
                    PN = -inR1-separation;
                    apart = (maxSeparation > inEps);
                    if (apart) { goto done; }
                }
            }
        }
    }
    
    // Axis of 2 x End-cap tangent of 1:
    disc = nCapZ*dCap-inT12.t()[2];
    if (disc > 0.0) {
        c.setValue(0,0,-inHH1);
        dCap = -inHH1;
    } else {
        c.setValue(0,0,inHH1);
        dCap = inHH1;
    }
    if (MeFabs(nCapZ) > 1.0e-8) {
        // intersect axis with plane of endcap
        MeReal t = (c[2]-inT12.t()[2])/nCapZ;
        p = inT12.t()+((c[2]-inT12.t()[2])/nCapZ)*inT12.axis(2);
        p -= c;
        p.normalize();
        p *= inR1;
        // p is now on the end cap boundary, closest to the axis.
        p -= p.dot(inT12.axis(2))*inT12.axis(2);
        nW2 = p;
        if (nW2.square_norm() > norm_eps) {
            nW2.normalize();
            nParaW1 = AxisCrossVec3(2,nW2);
            if (nParaW1.square_norm() > norm_eps) {
                nParaW1.normalize();
                nPerpW1 = Vec3CrossAxis(nParaW1,2);
                rCyl = MeFabs(nW2[2])*inHH1+MeFabs(nPerpW1.dot(nW2))*inR1;
                sumR = inR2+rCyl;
                normD = inT12.t().dot(nW2);
                aNormD = MeFabs(normD);
                separation = aNormD-sumR;
                if (separation > maxSeparation) {
                    maxSeparation = separation;
                    normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
                    normInfo = (2<<2)|1;
                    PN = inR2-aNormD;
                    apart = (maxSeparation > inEps);
                    if (apart) { goto done; }
                }
            }
        }
    }
    
    // "Second kind" of end-cap tangent tests:
    dCap = inT12.t().dot(inT12.axis(2));
    
    // Axis of 1 x End-cap tangent of 2:
    disc = dCap-inT12.t()[2]*nCapZ;
    c = inT12.t();
    if (disc > 0.0) {
        c -= inHH2*inT12.axis(2);
        dCap -= inHH2;
    } else {
        c += inHH2*inT12.axis(2);
        dCap += inHH2;
    }
    // Find vector from center of cap to axis 1:
    p = -c;
    p[2] = 0.0;
    p -= p.dot(inT12.axis(2))*inT12.axis(2);
    if (p.square_norm() > norm_eps) {
        p.normalize();
        nPerpA2E1 = p;
        nParaA2E1 = inT12.axis(2).cross(nPerpA2E1);
        p *= inR2;
        p += c;
        // p now lies on the cap of 2
        p[2] = 0.0;
        nA1E2 = p;
        if (nA1E2.square_norm() > norm_eps) {
            nA1E2.normalize();
            rCyl = MeFabs(inT12.axis(2).dot(nA1E2))*inHH2+MeFabs(nPerpA2E1.dot(nA1E2))*inR2;
            sumR = inR1+rCyl;
            normD = inT12.t().dot(nA1E2);
            separation = MeFabs(normD)-sumR;
            if (separation > maxSeparation) {
                maxSeparation = separation;
                normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
                normInfo = (2<<2);
                PN = -inR1-separation;
                apart = (maxSeparation > inEps);
                if (apart) { goto done; }
            }
        }
    }
    
    // Axis of 2 x End-cap tangent of 1:
    disc = nCapZ*dCap-inT12.t()[2];
    if (disc > 0.0) {
        c.setValue(0,0,-inHH1);
        dCap = -inHH1;
    } else {
        c.setValue(0,0,inHH1);
        dCap = inHH1;
    }
    // Find vector from center of cap to axis 2:
    p = inT12.t();
    p[2] -= dCap;
    p -= p.dot(inT12.axis(2))*inT12.axis(2);
    p[2] = 0.0;
    if (p.square_norm() > norm_eps) {
        p.normalize();
        nPerpA1E2 = p;
        nParaA1E2 = AxisCrossVec3(2,nPerpA1E2);
        p *= inR1;
        p += c-inT12.t();
        p -= p.dot(inT12.axis(2))*inT12.axis(2);
        nA2E1 = p;
        if (nA2E1.square_norm() > norm_eps) {
            nA2E1.normalize();
            rCyl = MeFabs(nA2E1[2])*inHH1+MeFabs(nPerpA1E2.dot(nA2E1))*inR1;
            sumR = inR2+rCyl;
            normD = inT12.t().dot(nA2E1);
            aNormD = MeFabs(normD);
            separation = aNormD-sumR;
            if (separation > maxSeparation) {
                maxSeparation = separation;
                normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
                normInfo = 2;
                PN = inR2-aNormD;
                apart = (maxSeparation > inEps);
                if (apart) { goto done; }
            }
        }
    }
    
    // End-cap tangent x End-cap tangent:
    minAxis1 = minAxis2 = 0;
    minCapD = MEINFINITY;
    for (axis2 = 0, z2 = -inHH2; axis2 < 2; axis2++,(z2 += (MeReal)(2.0)*inHH2)) {
        c = inT12.t()+z2*inT12.axis(2);
        dispDotX1 = c[0];
        dispDotY1 = c[1];
        for (axis1 = 0, z1 = -inHH1; axis1 < 2; axis1++,(z1 += (MeReal)(2.0)*inHH1)) {
            disp = c;
            disp[2] -= z1;
            MeReal capD = disp.square_norm();
            if (capD < minCapD) {
                minCapD = capD;
                minAxis1 = axis1;
                minAxis2 = axis2;
            }
        }
    }
    
    axis1 = minAxis1;
    axis2 = minAxis2;
    
    z1 = inHH1*(MeReal)((axis1<<1)-1);
    z2 = inHH2*(MeReal)((axis2<<1)-1);
    
    X1DotX2 = inT12.axis(0)[0];
    X1DotY2 = inT12.axis(1)[0];
    Y1DotX2 = inT12.axis(0)[1];
    Y1DotY2 = inT12.axis(1)[1];
    cylR1 = inR1;
    cylR2 = inR2;
    c = inT12.t()+z2*inT12.axis(2);
    
    sinTheta = MeFabs(nCapZ)>=1 ? 0 : MeSqrt((MeReal)(1.0)-nCapZ*nCapZ);
    
    // This is VERY expensive, so let's see if we can't avoid it:
    if (MeFabs(c[2]) < MeFabs(z1)) {
        goto done; // cap 2 center between caps of 1 ... no need for end-cap to end-cap test
    }
    if (MeFabs(c[2]-z1) < inR2*sinTheta) {
        goto done; // cap 2 entirely outside caps of 1 ... no need for end-cap to end-cap test
    }
    zCap.setValue(0,0,z1);
    zCap -= inT12.t();
    zCapZ = zCap.dot(inT12.axis(2));
    if (MeFabs(zCapZ) < MeFabs(z2)) {
        goto done; // cap 1 center between caps of 2 ... no need for end-cap to end-cap test
    }
    if (MeFabs(zCapZ-z2) < inR1*sinTheta) {
        goto done; // cap 1 entirely outside caps of 2 ... no need for end-cap to end-cap test
    }
    
    dispDotX1 = c[0];
    dispDotY1 = c[1];
    disp = c;
    disp[2] -= z1;
    dispDotX2 = disp.dot(inT12.axis(0));
    dispDotY2 = disp.dot(inT12.axis(1));
    theta[0] = theta[1] = 0.0;
    d2Min = FnMinNd(2,theta,(MeReal)(0.001),(MeReal)(0.001),EndCapMin,EndCapMinGrad);
    d2Min *= 2.0;
    d2Min += disp.square_norm()+inR1*inR1+inR2*inR2;
    p1.setValue(inR1*MeCos(theta[0]),inR1*MeSin(theta[0]),z1);
    p2 = inT12.t()+(inR2*MeCos(theta[1]))*inT12.axis(0)+(inR2*MeSin(theta[1]))*inT12.axis(1)+z2*inT12.axis(2);
    nE1E2 = p2-p1;
    nE1E2.normalize();
    pE1E2 = 0.5*(p1+p2);
    
    para1 = AxisCrossVec3(2,nE1E2);
    if (para1.square_norm() < norm_eps) {
        rc1 = inHH1;
    } else {
        para1.normalize();
        lsVec3 perp1 = Vec3CrossAxis(para1,2);
        rc1 = MeFabs(nE1E2[2])*inHH1+
            MeFabs(perp1.dot(nE1E2))*inR1;
    }
    
    para2 = inT12.axis(2).cross(nE1E2);
    if (para2.square_norm() < norm_eps) {
        rc2 = inHH2;
    } else {
        para2.normalize();
        lsVec3 perp2 = para2.cross(inT12.axis(2));
        rc2 = MeFabs(inT12.axis(2).dot(nE1E2))*inHH2+
            MeFabs(perp2.dot(nE1E2))*inR2;
    }
    sumR = rc1+rc2;
    normD = inT12.t().dot(nE1E2);
    aNormD = MeFabs(normD);
    separation = aNormD-sumR;
    if (separation > maxSeparation) {
        maxSeparation = separation;
        normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
        normInfo = (axis2<<2)|axis1;
        PN = rc2-aNormD;
        apart = (separation > inEps);
        //      if (apart) { goto done; }
    }
    
done:
    
    lsVec3 nPara1;
    lsVec3 nPerp1;
    lsVec3 nPara2;
    lsVec3 nPerp2;
    
    outSep = maxSeparation;
    
    outPN = PN;
    
    lsVec3 *posList = outPos;
    
    if(apart)
        return 0;

    if (normInfo == 0xC) 
    {
        // face of cyl 1
        outN.setValue(0.0,0.0,normalSign);
        outDims = 2;
        nPara2 = nParaN2;
        nPerp2 = nPerpN2;
        CylPerpAndPara(nPerp1,nPara1,lsVec3(0,0,1),inT12.axis(2),inT12.t());
    } 
    else if (normInfo == 0x3) 
    {
        // face of cyl 2
        outN = normalSign*inT12.axis(2);
        outDims = 2<<8;
        nPara1 = nParaN1;
        nPerp1 = nPerpN1;
        CylPerpAndPara(nPerp2,nPara2,inT12.axis(2),lsVec3(0,0,1),inT12.t());
    } 
    else
    {
        axis1 = normInfo&3;
        axis2 = (normInfo&0xC)>>2;
        if (axis1 == 2) 
        {
            if (axis2 == 2) 
            {
                // axis x axis
                outN = normalSign*nAA;
                outDims = (3<<8)|3;
                CylPerpAndPara(nPerp1,nPara1,lsVec3(0,0,1),inT12.axis(2),inT12.t());
                CylPerpAndPara(nPerp2,nPara2,inT12.axis(2),lsVec3(0,0,1),inT12.t());
            } 
            else 
            {
                if (axis2 == 0) 
                {
                    // axis 2 x end-cap 1
                    nPerp1 = nPerpA1E2;
                    nPara1 = nParaA1E2;
                    CylPerpAndPara(nPerp2,nPara2,inT12.axis(2),nA2E1,inT12.t());
                    outN = normalSign*nA2E1;
                    outDims = (3<<8)|1;
                } 
                else 
                {
                    // wall of 1
                    nPerp1 = nW1;
                    nPara1 = AxisCrossVec3(2,nW1);
                    nPerp2 = nPerpW2;
                    nPara2 = nParaW2;
                    outN = normalSign*nW1;
                    outDims = (3<<8)|3;
                }
            }
        } 
        else
        {
            if (axis2 == 2) 
            {
                if (axis1 == 0) 
                {
                    // axis 1 x end-cap 2
                    CylPerpAndPara(nPerp1,nPara1,lsVec3(0,0,1),nA1E2,inT12.t());
                    nPerp2 = nPerpA2E1;
                    nPara2 = nParaA2E1;
                    outN = normalSign*nA1E2;
                    outDims = (1<<8)|3;
                } 
                else 
                {
                    // wall of 2
                    nPerp1 = nPerpW1;
                    nPara1 = nParaW1;
                    nPerp2 = nW2;
                    nPara2 = inT12.axis(2).cross(nW2);
                    outN = normalSign*nW2;
                    outDims = (3<<8)|3;
                }
            } 
            else 
            {
                // end-cap x end-cap
                // Let's just add the midpoint here...
                outN = normalSign*nE1E2;
                *outPos++ = pE1E2;
                outDims = 0;
                return true;
            }
        }
    }

    CylCylIntersect(outPos,
        inT12,inR1,inHH1,nPerp1,nPara1,true,inR2,inHH2,nPerp2,nPara2,true,scale);
    
    return 1;
}

MeBool MEAPI
McdCylinderCylinderIntersect( McdModelPair* p, McdIntersectResult *result )
{
    McdProfileStart("McdCylinderCylinderIntersect");
    
    lsTransform &tA = *(lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform &tB = *(lsTransform*)McdModelGetTransformPtr( p->model2 );
    
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdCylinderID geometry1 = (McdCylinderID)McdModelGetGeometry( p->model1 );
    McdCylinderID geometry2 = (McdCylinderID)McdModelGetGeometry( p->model2 );
    
    McdFramework *fwk = p->model1->frame;

    result->touch = 0;
    result->contactCount = 0;
 
    
    // Radii
    const MeReal rCylA = McdCylinderGetRadius( geometry1);
    const MeReal hhCylA = McdCylinderGetHalfHeight( geometry1);
    const MeReal rCylB = McdCylinderGetRadius( geometry2);
    const MeReal hhCylB = McdCylinderGetHalfHeight( geometry2);
    
    lsTransform tAB;
    tAB.thisIsFirstThenInverseSecond(tB,tA);
    
    lsVec3 footprint[48];
    lsVec3 *verts = footprint;
    MeReal separation;
    lsVec3 normal;
    MeI16 dims;
    MeReal PN;
    
    // Overlap test, return overlap values
    
    if (!OverlapCylCyl(separation,normal,
        PN,verts,dims,eps,rCylA,hhCylA,rCylB,hhCylB,tAB,fwk->mScale)) 
    {
        McdProfileEnd("McdCylinderCylinderIntersect");
        return 0;
    }
#ifdef MCD_CURVATURE
    MeReal rocA, rocB;

    MeReal ZZ2 = Square(normal[2]);

    if (ZZ2 > (MeReal)(0.999999)) 
        rocA = 0.0;
    else if (ZZ2 < (MeReal)(0.000001)) 
        rocA = rCylA;
    else
        rocA = rCylA*MeRecipSqrt((MeReal)(1.0)-ZZ2);
    
    ZZ2 = Square(normal.dot(tAB.axis(2)));

    if (ZZ2 > (MeReal)(0.999999)) 
        rocB = 0.0;
    else if (ZZ2 < (MeReal)(0.000001)) 
        rocB = rCylB;
    else
        rocB = rCylB*MeRecipSqrt((MeReal)(1.0)-ZZ2);
#endif
    
    // Transform normal into global frame
    tA.transformWithoutTranslate(normal,(lsVec3*)result->normal);
    
    // Transform footprint into global frame, and find depths
    lsVec3 *v = footprint;
    McdContact *c = result->contacts;
    result->contactCount = 0;
    while (v != verts && result->contactCount < result->contactMaxCount) 
    {
        MeReal s = v->dot(normal)-PN;
        if (s < separation*(MeReal)(0.01f)) 
        {
            tA.transform(*v,(lsVec3*)c->position);
            c->dims = dims;
            c->separation = s;
#ifdef MCD_CURVATURE
            c->curvature1 = rocA;
            c->curvature2 = rocB;
#endif
            *(lsVec3*)c->normal = *(lsVec3*)result->normal;
            result->contactCount++;
            c++;
        }
        v++;
    }
    
    McdProfileEnd("McdCylinderCylinderIntersect");
    return result->touch = result->contactCount > 0;
}

#define box_sph_rad_const 2/(MeSqrt((MeReal)2.0)+1)

int MEAPI
McdCylinderCylinderSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result){
    result->pair = p;
    result->time = maxTime;
    
    McdCylinderID geometry1 = (McdCylinderID)McdModelGetGeometry( p->model1 );
    McdCylinderID geometry2 = (McdCylinderID)McdModelGetGeometry( p->model2 );
    
    lsVec3 *V0 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model1);
    lsVec3 *V1 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model2);
    
    MeReal radCyl1 = McdCylinderGetRadius( geometry1 );
    MeReal hheightCyl1 = McdCylinderGetHalfHeight( geometry1 );
    radCyl1 *= box_sph_rad_const;
    
    MeReal radCyl2 = McdCylinderGetRadius( geometry1 );
    MeReal hheightCyl2 = McdCylinderGetHalfHeight( geometry1 );
    radCyl2 *= box_sph_rad_const;
    
    lsTransform *tm1 = (lsTransform *)McdModelGetTransformPtr(p->model1);
    lsTransform *tm2 = (lsTransform *)McdModelGetTransformPtr(p->model2);
    
    MeReal ext1[3] = {radCyl1, radCyl1, hheightCyl1};
    MeReal ext2[3] = {radCyl2, radCyl2, hheightCyl2};
    
    MeReal T;
    lsVec3 P;
    
    unsigned int ixt = MovingBoxBoxIntersect( ext1, tm1, *V0, ext2, tm2, *V1, maxTime, T, P );
    
    if ( ixt ) 
        result->time = T;       // during [t0, t1], box/box intersect
                                // if result->time = 0 --> box/box intersect at t0
     else
        result->time = maxTime; // during [t0, t1], box/box does not intersect
    
    return ixt;
}

MCD_IMPLEMENT_SAFETIME_REGISTRATION(Cylinder,Cylinder,1)

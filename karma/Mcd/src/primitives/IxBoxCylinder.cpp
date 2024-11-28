/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.40.2.4 $

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
#include <McdBox.h>
#include <McdCylinder.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <McdContact.h>
#include <MovingBoxBoxIntersect.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>

/*
    Temporary fix; probably required only for the 991111 PS2 compiler.
*/

#ifdef PS2
#   define inline
#endif

/*
    McdBoxCylinder implementation

*/

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

static inline void McdVanillaAddBoxBoxSegmentPoints(lsVec3* &outList,
    const lsVec3 &orig, const lsVec3 &disp,
    const lsVec3 &invDisp, const lsVec3 &inR, const MeReal scale)
{
    MeReal tIn = -1,tOut = 1;

    if (McdVanillaSegmentCubeIntersect(&tIn,&tOut,
        orig,disp,invDisp,inR,(MeReal)1e-6 * scale))
    {
        outList++->setValue(orig[0]+tIn*disp[0],
                            orig[1]+tIn*disp[1],
                            orig[2]+tIn*disp[2]);

        outList++->setValue(orig[0]+tOut*disp[0],
                            orig[1]+tOut*disp[1],
                            orig[2]+tOut*disp[2]);
    }
}

static inline void McdVanillaAddBoxBoxClippedPoints(lsVec3* &outList,
    const lsVec3 &orig, const lsVec3 &disp,
    const lsVec3 &invDisp, const lsVec3 &inR, const MeReal scale)
{

    MeReal tIn = -1, tOut = 1;
    if (McdVanillaSegmentCubeIntersect(&tIn,&tOut,
        orig,disp,invDisp,inR,(MeReal)1e-6 * scale))
    {
        if (tIn > -1 && tIn < 1)
            outList++->setValue(orig[0]+tIn*disp[0],
                                orig[1]+tIn*disp[1],
                                orig[2]+tIn*disp[2]);

        if (tOut > -1 && tOut < 1)
            outList++->setValue(orig[0]+tOut*disp[0],
                                orig[1]+tOut*disp[1],
                                orig[2]+tOut*disp[2]);
    }
}

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

static inline void McdVanillaAddBoxCylClippedPoints(lsVec3* &outList,
    const lsVec3 &orig, const lsVec3 &disp, const MeReal inR,
    const MeReal inHH, const MeReal minT, const MeReal maxT, const MeReal scale)
{
    MeReal tIn = minT;
    MeReal tOut = maxT;

    if (McdVanillaSegmentCylinderIntersect(tIn,tOut,orig,disp,inR,inHH, scale))
    {
        if (tIn > minT)
            *outList++ = orig+tIn*disp;

        if (tOut < maxT)
            *outList++ = orig+tOut*disp;
    }
}

static inline void McdVanillaAddBoxCylSegmentPoints(lsVec3* &outList,
    const lsVec3 &orig,const lsVec3 &disp,const MeReal inR,
    const MeReal inHH,const MeReal minT,const MeReal maxT, const MeReal scale)
{
    MeReal tIn = minT;
    MeReal tOut = maxT;

    if (McdVanillaSegmentCylinderIntersect(tIn,tOut,orig,disp,inR,inHH, scale))
    {
        *outList++ = orig+tIn*disp;
        *outList++ = orig+tOut*disp;
    }
}

/*
    Box to Cylinder collision detection
*/

void MEAPI BoxCylIntersect(lsVec3* &outList, const lsTransform &tAB,
    const lsVec3 &inRA,const MeReal inRCyl, const MeReal inHHCyl,
    const lsVec3 &nPara, const lsVec3 &nPerp, const lsVec3 &cylAxis, const MeReal scale)
{
    
    lsTransform tBA;    // = inverse of tAB
    tBA.inverseOf(tAB);

    lsVec3 bVertList[24];
    lsVec3 *bVerts = bVertList;

    const lsVec3 &cylPos = tAB.t();

    // cylinder "edges" vs. box:

    const lsVec3 a0 = inRCyl*nPara;
    const lsVec3 a1 = inRCyl*nPerp;
    const lsVec3 a2 = inHHCyl*cylAxis;
    const lsVec3 invA1(MeSafeRecip(a1[0]),MeSafeRecip(a1[1]),MeSafeRecip(a1[2]));
    const lsVec3 invA2(MeSafeRecip(a2[0]),MeSafeRecip(a2[1]),MeSafeRecip(a2[2]));

    McdVanillaAddBoxBoxSegmentPoints(outList,cylPos+a0,a2,invA2,inRA, scale);
    McdVanillaAddBoxBoxSegmentPoints(outList,cylPos+a1,a2,invA2,inRA, scale);
    McdVanillaAddBoxBoxSegmentPoints(outList,cylPos-a0,a2,invA2,inRA, scale);
    McdVanillaAddBoxBoxSegmentPoints(outList,cylPos-a1,a2,invA2,inRA, scale);

    // cyl. perp-axes:
    McdVanillaAddBoxBoxClippedPoints(outList,cylPos+a2,a1,invA1,inRA, scale);
    McdVanillaAddBoxBoxClippedPoints(outList,cylPos-a2,a1,invA1,inRA, scale);

    // box edges vs cylinder:
    const lsVec3 d0 = inRA[0]*tBA.axis(0);
    const lsVec3 d1 = inRA[1]*tBA.axis(1);
    const lsVec3 d2 = inRA[2]*tBA.axis(2);

    // x-direction edges:
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()+d1+d2,d0,inRCyl,inHHCyl,-1.0,1.0, scale); // ++
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()+d1-d2,d0,inRCyl,inHHCyl,-1.0,1.0, scale); // +-
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()-d1-d2,d0,inRCyl,inHHCyl,-1.0,1.0, scale); // --
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()-d1+d2,d0,inRCyl,inHHCyl,-1.0,1.0, scale); // -+

    // y-direction edges:
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()+d2+d0,d1,inRCyl,inHHCyl,-1.0,1.0, scale); // ++
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()+d2-d0,d1,inRCyl,inHHCyl,-1.0,1.0, scale); // +-
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()-d2-d0,d1,inRCyl,inHHCyl,-1.0,1.0, scale); // --
    McdVanillaAddBoxCylClippedPoints(bVerts,tBA.t()-d2+d0,d1,inRCyl,inHHCyl,-1.0,1.0, scale); // -+

    // z-direction edges:
    McdVanillaAddBoxCylSegmentPoints(bVerts,tBA.t()+d0+d1,d2,inRCyl,inHHCyl,-1.0,1.0, scale); // ++
    McdVanillaAddBoxCylSegmentPoints(bVerts,tBA.t()+d0-d1,d2,inRCyl,inHHCyl,-1.0,1.0, scale); // +-
    McdVanillaAddBoxCylSegmentPoints(bVerts,tBA.t()-d0-d1,d2,inRCyl,inHHCyl,-1.0,1.0, scale); // --
    McdVanillaAddBoxCylSegmentPoints(bVerts,tBA.t()-d0+d1,d2,inRCyl,inHHCyl,-1.0,1.0, scale); // -+

    lsVec3 *vert = bVertList;
    while (vert != bVerts) {
        tAB.transform(*vert++,outList++);
    }
}

bool OverlapOBBCyl(MeReal &outSep,
  lsVec3 &outN, MeReal &outPN, lsVec3* &outPos,
  const MeReal inEps, const lsVec3 &inROBB, const MeReal inRCyl,
  const MeReal inHHCyl, const lsTransform &inT12, const MeReal scale)
{
   
    int i;

    // Find maximum separation (early-out for positive separation)
    bool apart = true;
    MeReal maxSeparation = -MEINFINITY;
    MeReal normalSign = 1.0;
    MeU8 normInfo = 0xF;
    // MeReal nLen = 1.0;
    lsVec3 sr;
    lsVec3 d;
    lsVec3 d2;
    MeReal separation;
    MeReal sumR;
    MeReal normD;
    MeReal aNormD;

    MeReal PN;

    const lsVec3 &cylAxis = inT12.axis(2);
    const lsVec3 &cylPos = inT12.t();
    const MeReal rZAxis = MeFabs(cylAxis[2])*inHHCyl;

    lsVec3 nCW;
    lsVec3 nBExCA[3];
    lsVec3 nBExET[3];
//  lsVec3 nBExETPara[3];
//  lsVec3 nBExETPerp[3];
    lsVec3 nParaN[3];   // Parallel vectors for each axis
    lsVec3 nPerpN[3];   // Perp. vectors for each axis

    // Face separation:
    for (i = 0; i < 3; i++) {
        const int i1 = NextMod3(i);
        const int i2 = NextMod3(i1);
        nParaN[i] = Vec3CrossAxis(cylAxis,i);
        if (nParaN[i].square_norm() < 1.0e-15) {
            nParaN[i].setValue(0.0,0.0,0.0);
            nParaN[i][i2] = 1.0;
            nPerpN[i].setValue(0.0,0.0,0.0);
            nPerpN[i][i1] = 1.0;
        } else {
            nParaN[i].normalize();
            nPerpN[i] = nParaN[i].cross(cylAxis);
        }
        const MeReal rCyl = MeFabs(cylAxis[i])*inHHCyl+MeFabs(nPerpN[i][i])*inRCyl;
        sumR = inROBB[i]+rCyl;
        normD = cylPos[i];
        separation = MeFabs(normD)-sumR;
        if (separation > maxSeparation) {
            maxSeparation = separation;
            normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
            normInfo = 0xC|i;
            PN = -inROBB[i]-separation;
            apart = (separation > inEps);
            if (apart) { goto done; }
        }
    }

    // Cylinder's endcap separation:
    sumR = inHHCyl+inROBB[0]*(MeReal)MeFabs(inT12.axis(2)[0])+inROBB[1]*MeFabs(inT12.axis(2)[1])+inROBB[2]*MeFabs(inT12.axis(2)[2]);
    normD = cylPos.dot(cylAxis);
    aNormD = MeFabs(normD);
    separation = aNormD-sumR;
    if (separation > maxSeparation) {
        maxSeparation = separation;
        normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
        normInfo = (2<<2)|3;
        apart = (separation > inEps);
        PN = inHHCyl-aNormD;
        if (apart) { goto done; }
    }

    // Box-edge crossed w/cyl axis:
    for (i = 0; i < 3; i++) {
        nBExCA[i] = AxisCrossVec3(i,cylAxis);
        if (nBExCA[i].normalize() > 1.0e-15) {
            sumR = inRCyl+inROBB[0]*MeFabs(nBExCA[i][0])+inROBB[1]*MeFabs(nBExCA[i][1])+
                   inROBB[2]*MeFabs(nBExCA[i][2]);
            normD = cylPos.dot(nBExCA[i]);
            aNormD = MeFabs(normD);
            separation = aNormD-sumR;
            if (separation > maxSeparation) {
                maxSeparation = separation;
                normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
                normInfo = (2<<2)|i;
                apart = (separation > inEps);
                PN = inRCyl-aNormD;
                if (apart) { goto done; }
            }
        }
    }

    // Cylinder's wall separation:
    nCW = -cylPos;
    nCW -= nCW.dot(cylAxis)*cylAxis;
    if (nCW.normalize() > 1.0e-15) {
        sumR = inRCyl+inROBB[0]*MeFabs(nCW[0])+inROBB[1]*MeFabs(nCW[1])+inROBB[2]*MeFabs(nCW[2]);
        normD = cylPos.dot(nCW);
        aNormD = MeFabs(normD);
        separation = aNormD-sumR;
        if (separation > maxSeparation) {
            maxSeparation = separation;
            normalSign = normD > (MeReal)(0.0) ? (MeReal)(-1.0) : (MeReal)(1.0);
            normInfo = 3;
            apart = (separation > inEps);
            PN = inRCyl-aNormD;
            if (apart) { goto done; }
        }
    }

#if 0
    // Box-edge crossed w/tangent to endcap tangent:
    for (i = 0; i < 3; i++) {
        const int i1 = NextMod3(i);
        const int i2 = NextMod3(i1);
        // nBExET = (Box Axis) x ( Cyl Pos x Cyl Axis )
        nBExET[i] = cylaxis(i]*cylPos-cylPos[i]*cylAxis;
        if (nBExET[i].normalize() > 1.0e-15) {
            nBExETPara[i] = cylAxis.cross(nBExET[i]);
            if (nBExETPara[i].square_norm() < 1.0e-15) {
                nBExETPara[i].setValue(0.0,0.0,0.0);
                nBExETPara[i][i2] = 1.0;
                nBExETPerp[i].setValue(0.0,0.0,0.0);
                nBExETPerp[i][i1] = 1.0;
            } else {
                nBExETPara[i].normalize();
                nBExETPerp[i] = nBExETPara[i].cross(cylAxis);
            }
            sumR = inROBB[i]+
                inRCyl*MeFabs(nBExETPerp[i].dot(nBExET[i]))+inHHCyl*MeFabs(cylAxis.dot(nBExET[i]));
            normD = cylPos.dot(nBExET[i]);
            separation = MeFabs(normD)-sumR;
            if (separation > maxSeparation) {
                maxSeparation = separation;
                normalSign = normD > 0.0 ? -1.0 : 1.0;
                normInfo = i;
                apart = (separation > inEps);
                if (apart) { goto done; }
            }
        }
    }
#endif

done:

    lsVec3 nPara;
    lsVec3 nPerp;

    outSep = maxSeparation;

    outPN = PN;

    lsVec3 *posList = outPos;

    if (!apart) {
        if ((normInfo & 0xC) == 0xC) {
            // box face separation
            MeI8 axis = normInfo&3;
            nPara = nParaN[axis];
            nPerp = nPerpN[axis];
            outN.setValue(0,0,0);
            outN[axis] = normalSign;
        } else
        if ((normInfo & 3) == 3) {
            // cylinder face separation
            MeI8 axis = (normInfo&0xC)>>2;
            if (axis == 2) {
                // end cap separation
                nPara = inT12.axis(0);
                nPerp = inT12.axis(1);
                outN = normalSign*cylAxis;
            } else
            {
                // cylinder wall separation
                nPerp = nCW;
                nPara = cylAxis.cross(nPerp);
                outN = nCW;
            }
        } else
        {
            MeI8 axis = normInfo&3;
            // edge separation
//          if ((normInfo&0xC)>>2) {
                // box edge crossed with cylinder primary axis
                nPerp = nBExCA[axis];
                nPara = cylAxis.cross(nPerp);
                outN = normalSign*nBExCA[axis];
/*          } else
            {
                // box edge crossed with cylinder endcap tangent
                nPara = nBExETPara[axis];
                nPerp = nBExETPerp[axis];
                outN = normalSign*nBExET[axis];
            }*/
        }

        BoxCylIntersect(outPos,
            inT12,inROBB,inRCyl,inHHCyl, nPara, nPerp, cylAxis, scale);
    }

    return outPos != posList;
}

MeBool MEAPI
McdBoxCylinderIntersect( McdModelPair* p, McdIntersectResult *result )
{
    lsTransform &tA = *(lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform &tB = *(lsTransform*)McdModelGetTransformPtr( p->model2 );
    
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdBoxID geometry1 = (McdBoxID)McdModelGetGeometry( p->model1 );
    McdCylinderID geometry2 = (McdCylinderID)McdModelGetGeometry( p->model2 );
    
    McdFramework *fwk = p->model1->frame;
    result->contactCount = 0;
    result->touch = 0;

    // Radii
    lsVec3 &rA = *(lsVec3*)McdBoxGetRadii(geometry1);
    const MeReal rCyl = McdCylinderGetRadius(geometry2);
    const MeReal hhCyl = McdCylinderGetHalfHeight(geometry2);


    lsTransform tAB;
    tAB.thisIsFirstThenInverseSecond(tB,tA);

    lsVec3 footprint[48];
    lsVec3 *verts = footprint;
    MeReal separation;
    lsVec3 normal;
    MeReal PN;


    // Overlap test, return overlap values
    bool touch = OverlapOBBCyl(separation,normal,
        PN,verts,eps,rA,rCyl,hhCyl,tAB, fwk->mScale);

    if (!touch)
        return false;

    MeI16 dimA = (MeI16)(normal[0] < (MeReal)(1.0e-4))+
                 (MeI16)(normal[1] < (MeReal)(1.0e-4))+
                 (MeI16)(normal[2] < (MeReal)(1.0e-4));

    MeI16 dimB;

    MeReal ZZ1 = normal.dot(tAB.axis(2));
    MeReal ZZ2 = ZZ1 * ZZ1;
#ifdef MCD_CURVATURE
    MeReal rocB;
#endif
    if (ZZ2 > (MeReal)(0.999999)) {
#ifdef MCD_CURVATURE
        rocB = 0.0;
#endif
        dimB = 2;
    } else
    if (ZZ2 < (MeReal)(0.000001)) {
#ifdef MCD_CURVATURE
        rocB = rCyl;
#endif
        dimB = 3; // usually reserved for sphere - need a different type?
    } else
    {
#ifdef MCD_CURVATURE
        rocB = rCyl*MeRecipSqrt((MeReal)(1.0)-ZZ2);
#endif
        dimB = 1;
    }


    // Transform normal into global frame
    tA.transformWithoutTranslate(normal,(lsVec3*)result->normal);

    // Transform footprint into global frame, and find depths
    lsVec3 *v = footprint;
    McdContact *c = result->contacts;
    result->contactCount = 0;
    while (v != verts && result->contactCount < result->contactMaxCount) 
    {
        MeReal s = v->dot(normal)-PN;
        if (s < separation*(MeReal)(0.01)) 
        {
            tA.transform(*v,(lsVec3*)c->position);
            c->dims = (dimB<<2)|dimA;
            c->separation = s;
#ifdef MCD_CURVATURE
            c->curvature1 = 0;
            c->curvature2 = rocB;
#endif
            *(lsVec3*)c->normal = *(lsVec3*)result->normal;
            result->contactCount++;
            c++;
        }
        v++;
    }

    return result->touch = result->contactCount > 0;
}

#define box_sph_rad_const 2/(MeSqrt((MeReal)2.0)+1)

int MEAPI
McdBoxCylinderSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result)
{
    result->pair = p;
    result->time = maxTime;
    
    McdBoxID geometry1 = (McdBoxID)McdModelGetGeometry( p->model1 );
    McdCylinderID geometry2 = (McdCylinderID)McdModelGetGeometry( p->model2 );
    
    lsVec3 *V0 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model1);
    lsVec3 *V1 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model2);
    
    MeReal radCyl2 = McdCylinderGetRadius( geometry2 );
    MeReal hheightCyl2 = McdCylinderGetHalfHeight( geometry2 );
    radCyl2 *= box_sph_rad_const;
    
    lsTransform *tm1 = (lsTransform *)McdModelGetTransformPtr(p->model1);
    lsTransform *tm2 = (lsTransform *)McdModelGetTransformPtr(p->model2);
    
    MeReal *ext1 = (MeReal*)McdBoxGetRadii(geometry1);
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


MCD_IMPLEMENT_SAFETIME_REGISTRATION(Box,Cylinder,1)

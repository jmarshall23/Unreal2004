#error this file should not be in the build

#include <stdio.h>
#include "McdCheck.h"
#include "McdPS2Core.h"
#include "MeAssert.h"

static void Vec3Abs(lsVec3 &out, const lsVec3 &in)
{
    out[0] = MeFabs(in[0]);
    out[1] = MeFabs(in[1]);
    out[2] = MeFabs(in[2]);
}

static void Vec4Spread(lsVec4 &vec, MeReal r)
{
    vec[0] = vec[1] = vec[2] = vec[3] = r;
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

void McdPS2Core::BoxTriIntersect(lsVec3* &outList, const lsVec3 &inRBox, const CxTriangleNE &inTri) 
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


/*
    OverlapOBBTri
    -------------

    Attempts to find a separating plane between a given triangle and box
    If there is one, the function returns
    If not, the footprint of the intersection is calculated

    Cases for separating planes:

    Face of triangle
    Face of OBB
    

    On Entry:   (input is 8 quadwords)

    inEps    -
    inR      - radius of box (in each direction)
    inTri    - triangle to test (contains vertices,edges and normal)

    On Exit:

    outSep   - closest distance between objects
    outN     - vector specifying direction of line of closest distance
    outPN    - distance between centre of box and closest festure
    outPos   - footprint of intersection
    outDims  - description of the intersection eg face/edge

*/

MeReal obbtriconstants[4] __attribute__ ((aligned(64))) = { 0.5f, 1.0f, 0.0f, 0.0f };

typedef struct {
    MeVector4 inR;
    MeVector4 aNorm;
    MeVector4 mNormal;
    MeVector4 mVertex0;
    MeVector4 sumR;
    MeVector4 normD;
    MeVector4 aNormD;
    MeVector4 maxSeparation;
    MeVector4 minCoord;
    MeVector4 maxCoord;
    MeVector4 sumRV;
    MeVector4 normDV;             
    MeVector4 sepV;
    MeVector4 p3_maxSeparation;
    MeVector4 p4_maxSeparation;
    MeVector4 p5_maxSeparation;
    MeVector4 aE0;
    MeVector4 aE1;
    MeVector4 aE2;
    MeVector4 sqE0;
    MeVector4 sqE1;
    MeVector4 sqE2;
    MeVector4 negaE0;
    MeVector4 negaE1;
    MeVector4 negaE2;
    MeVector4 p7_em;
    MeVector4 p7_en;
    MeVector4 p7_vm;
    MeVector4 p7_vn;
    MeVector4 p7_sRV;
    MeVector4 p7_rBV;
    MeVector4 p7_rLenV;
    MeVector4 p7_sepV;            // 0x200

    MeVector4 p8_rlen;              // 0x210
    MeVector4 p8_eps2;              // 0x220
    MeVector4 p9_maxSeparation;     // 0x230
    MeVector4 p9_PN;                // 0x240
    MeVector4 p9_normalsign;        // 0x250
    MeVector4 p9_nRLen;             // 0x260

    MeVector4 p10_rlen;             // 0x270
    MeVector4 p10_eps2;             // 0x280
    MeVector4 p11_maxSeparation;    // 0x290
    MeVector4 p11_PN;               // 0x2a0
    MeVector4 p11_normalsign;       // 0x2b0
    MeVector4 p11_nRLen;            // 0x2c0

    MeVector4 p12_rlen;             // 0x2d0
    MeVector4 p12_eps2;             // 0x2e0
    MeVector4 p13_maxSeparation;    // 0x2f0
    MeVector4 p13_PN;               // 0x300
    MeVector4 p13_normalsign;       // 0x310
    MeVector4 p13_nRLen;            // 0x320

    MeVector4 p14_em;               // 0x330
    MeVector4 p14_en;               // 0x340
    MeVector4 p14_vm;               // 0x350
    MeVector4 p14_vn;               // 0x360
    MeVector4 p14_sRV;              // 0x370
    MeVector4 p14_rBV;              // 0x380
    MeVector4 p14_rLenV;            // 0x390
    MeVector4 p14_sepV;             // 0x3a0

    MeVector4 p15_rlen;             // 0x3b0
    MeVector4 p15_eps2;             // 0x3c0
    MeVector4 p16_maxSeparation;    // 0x3d0
    MeVector4 p16_PN;               // 0x3e0
    MeVector4 p16_normalsign;       // 0x3f0
    MeVector4 p16_nRLen;            // 0x400

    MeVector4 p17_rlen;             // 0x410             
    MeVector4 p17_eps2;             // 0x420
    MeVector4 p18_maxSeparation;    // 0x430
    MeVector4 p18_PN;               // 0x440
    MeVector4 p18_normalsign;       // 0x450
    MeVector4 p18_nRLen;            // 0x460

    MeVector4 p19_rlen;             // 0x470
    MeVector4 p19_eps2;             // 0x480
    MeVector4 p20_maxSeparation;    // 0x490
    MeVector4 p20_PN;               // 0x4a0
    MeVector4 p20_normalsign;       // 0x4b0
    MeVector4 p20_nRLen;            // 0x4c0

    MeVector4 p21_em;               // 0x4d0
    MeVector4 p21_en;               // 0x4e0
    MeVector4 p21_vm;               // 0x4f0
    MeVector4 p21_vn;               // 0x500
    MeVector4 p21_sRV;              // 0x510
    MeVector4 p21_rBV;              // 0x520
    MeVector4 p21_rLenV;            // 0x530
    MeVector4 p21_sepV;           // 0x540

    MeVector4 p22_rlen;             // 0x550
    MeVector4 p22_eps2;             // 0x560
    MeVector4 p23_maxSeparation;    // 0x570
    MeVector4 p23_PN;               // 0x580
    MeVector4 p23_normalsign;       // 0x590
    MeVector4 p23_nRLen;            // 0x5a0

    MeVector4 p24_rlen;             // 0x5b0             
    MeVector4 p24_eps2;             // 0x5c0
    MeVector4 p25_maxSeparation;    // 0x5d0
    MeVector4 p25_PN;               // 0x5e0
    MeVector4 p25_normalsign;       // 0x5f0
    MeVector4 p25_nRLen;            // 0x600

    MeVector4 p26_rlen;             // 0x610
    MeVector4 p26_eps2;             // 0x620
    MeVector4 p27_maxSeparation;    // 0x630
    MeVector4 p27_PN;               // 0x640
    MeVector4 p27_normalsign;       // 0x650
    MeVector4 p27_nRLen;            // 0x660

} debugdata;

debugdata debug __attribute__ ((aligned(64)));

#define printvec(x,s) printf("%20s = %12.6f %12.6f %12.6f \n",(s),(x)[0],(x)[1],(x)[2]);
#define printreal(x,s) printf("%20s = %12.6f \n",(s),(x)[0]);
#define printint(x,s) printf("%20s = %12d \n",(s),((int *)x)[0]);

MeReal normalSign[4]    __attribute__ ((aligned(64)));
MeReal maxSeparation[4] __attribute__ ((aligned(64)));
MeReal PN[4]            __attribute__ ((aligned(64)));
MeReal nRLen[4]         __attribute__ ((aligned(64)));

int   normInfo;

#define DUMPDEBUGDATA 0

bool McdPS2Core::OverlapOBBTri(MeReal &              outSep,   /* Output */ 
                               lsVec3 &              outN,     /* Output */ 
                               MeReal &              outPN,    /* Output */ 
                               lsVec3* &             outPos,   /* Output */ 
                               MeI16 &               outDims,  /* Output */ 
                               const MeReal          inEps,    /* Input */ 
                               const lsVec3 &        inR,      /* Input */ 
                               const CxTriangleNE &  inTri)    /* Input */ 
{
    int retval;
    MeReal sumR;
    MeReal normD;
    const MeReal eps = inEps<(MeReal)(0.0)?(MeReal)(0.0):inEps;
    lsVec3 minCoord;
    
#if 0
    printf("-------- McdPS2Core::OverlapOBBTri\n");
#endif

    MEASSERTALIGNED(&inR.v,16);
    MEASSERTALIGNED(&inTri,16);
    MEASSERTALIGNED(&inTri.mNormal.v,16);
    MEASSERTALIGNED(&(*(inTri.mVertex[0])->v),16);
    MEASSERTALIGNED(&(*(inTri.mVertex[1])->v),16);
    MEASSERTALIGNED(&(*(inTri.mVertex[2])->v),16);
    MEASSERTALIGNED(&(inTri.mEdge[0].v),16);
    MEASSERTALIGNED(&(inTri.mEdge[1].v),16);
    MEASSERTALIGNED(&(inTri.mEdge[2].v),16);

    __asm__ __volatile__("
    __expression_asm

        # registers [0..11)

        inR
        mNormal
        mVertex0
        mVertex1
        mVertex2
        mEdge0
        mEdge1
        mEdge2
        PN
        normalsign
        half
        nRLen

        'lqc2 @inR,0x00(%0)
        'lqc2 @mNormal,0x00(%1)
        'lqc2 @mVertex0,0x00(%2)
        'lqc2 @mVertex1,0x00(%3)
        'lqc2 @mVertex2,0x00(%4)
        'lqc2 @mEdge0,0x00(%5)
        'lqc2 @mEdge1,0x00(%6)
        'lqc2 @mEdge2,0x00(%7)
        'lqc2 @half,0x00(%9)

        'lwc1 $f1,0x00(%8)
        'lwc1 $f8,0x04(%9)

        # nRLen.x := 1.0f
        nRLen.x = K + K.w
 
    __end_expression_asm
    " : : "r" (inR.v),
          "r" (inTri.mNormal.v),
          "r" (&(*(inTri.mVertex[0])->v)),
          "r" (&(*(inTri.mVertex[1])->v)),
          "r" (&(*(inTri.mVertex[2])->v)),
          "r" (inTri.mEdge[0].v),
          "r" (inTri.mEdge[1].v),
          "r" (inTri.mEdge[2].v),
          "r" (&inEps),
          "r" (obbtriconstants)
    );

    // Face separation (face of triangle):
    // $8 is temporary
    // $9 is norminfo
    // $10 is temp2
    // $f1 is inEps
    // $f8 is 1

    __asm__ __volatile__("
    __expression_asm

    # registers [11..16)
    normD
    anormD
    aNorm
    sumR
    maxSeparation

    'addi $8,$0,0
    'sw   $8,0x00(%0)

    'vabs @aNorm,        @mNormal

#if DUMPDEBUGDATA

    # Debug point 0

    'sqc2              @inR,0x00(%1)
    'sqc2              @aNorm,0x10(%1)
    'sqc2              @mNormal,0x20(%1)
    'sqc2              @mVertex0,0x30(%1)
    
    # --------------
#endif

    sumR.xyz           = inR.xyz * aNorm
    sumR.x             = sumR.x + sumR.y
    sumR.x             = sumR.x + sumR.z

    normD.xyz          = mNormal.xyz * mVertex0
    normD.x            = normD.x + normD.y
    normD.x            = normD.x + normD.z

    'vabs.x @anormD,   @normD
    maxSeparation.x    = anormD.x - sumR.x   

#if DUMPDEBUGDATA
    # Debug point 1

    'sqc2              @sumR,0x40(%1)
    'sqc2              @normD,0x50(%1)
    'sqc2              @anormD,0x60(%1)
    'sqc2              @maxSeparation,0x70(%1)
    
    # --------------
#endif

    'qmfc2             $8,@maxSeparation
    'mtc1              $8,$f0

    'c.lt.s            $f1,$f0
    'bc1t              early_out

    PN.x         = K.x - anormD
    normalsign.x = normD + K.x
    'addi $9,$0,3

    ~normD
    ~anormD
    ~aNorm
    ~sumR
    ~mNormal

    # Face separation (face of OBB):

    #registers [11..17)
    mincoord
    maxcoord
    sumRV
    normDV
    anormDV
    sepV

    'vmini.xyz @mincoord,     @mVertex0,@mVertex1
    'vmini.xyz @mincoord,     @mincoord,@mVertex2
    'vmax.xyz @maxcoord,      @mVertex0,@mVertex1
    'vmax.xyz @maxcoord,      @maxcoord,@mVertex2

    sumRV.xyz = maxcoord.xyz - mincoord
    sumRV.xyz = sumRV.xyz * half.x
    sumRV.xyz = inR.xyz + sumRV

    normDV.xyz            = maxcoord.xyz + mincoord
    normDV.xyz            = normDV.xyz * half.x
    'vabs.xyz @anormDV, @normDV 

    sepV.xyz = anormDV - sumRV

#if DUMPDEBUGDATA
    # Debug point 2

    'sqc2              @mincoord,0x80(%1)
    'sqc2              @maxcoord,0x90(%1)
    'sqc2              @sumRV,0xa0(%1)
    'sqc2              @normDV,0xb0(%1)
    'sqc2              @sepV,0xc0(%1)
    
    # --------------
#endif

    # if any element of sepV is positive, then return

    'qmfc2             $8,@maxSeparation
    'mtc1              $8,$f2

    'qmfc2             $8,@sepV
    'mtc1              $8,$f0
    'c.lt.s            $f1,$f0
    'bc1t              early_out         # exit if eps < orig(sepV).x
    'c.lt.s            $f2,$f0           # if maxSeparation < orig(sepV).x
    'bc1f              x_not_largest     # {
    maxSeparation.x =  K.x + sepV.x      #     maxSeparation := orig(sepV).x
    PN.x            =  K.x - inR.x       #     PN := -inR.x - orig(sepV).x
    PN.x            =  PN.x - sepV.x     #
    normalsign.x    =  K.x + normDV.x    #     normalSign := normDV.x
    'addi              $9,$0,0xC|0       #     normInfo   := 0xC|0
    'x_not_largest:                      # }

#if DUMPDEBUGDATA
    # Debug point 3
    'sqc2              @maxSeparation,0xd0(%1)
    'sw                $9,0xd4(%1)
    # --------------
#endif

    'vmr32             @sepV,@sepV       # sepV.x := orig(sepV).y    
    'qmfc2             $8,@sepV          #
    'mtc1              $8,$f0            #
    'c.lt.s            $f1,$f0           #
    'bc1t              early_out         #
    'c.lt.s            $f2,$f0           # if maxSeparation < sepV.y
    'bc1f              y_not_largest     # {
    maxSeparation.x =  K.x + sepV.y      #     maxSeparation := orig(sepV).y
    PN.x            =  K.x - inR.y       #     PN := -inR.y - orig(sepV).y
    PN.x            =  PN.x - sepV.y     #
    normalsign.x    =  K.x + normDV.y    #     normalSign := normDV.y
    'addi              $9,$0,0xC|1       #     normInfo   := 0xC|1    
    'y_not_largest:                      # }

#if DUMPDEBUGDATA
    # Debug point 4
    'sqc2              @maxSeparation,0xe0(%1)
    'sw                $9,0xe4(%1)
    # --------------
#endif

    'vmr32             @sepV,@sepV       # sepV.x := orig(sepV).z    
    'qmfc2             $8,@sepV          #
    'mtc1              $8,$f0            #
    'c.lt.s            $f1,$f0           #
    'bc1t              early_out         #
    'c.lt.s            $f2,$f0           # if maxSeparation < sepV.z
    'bc1f              z_not_largest     # {
    maxSeparation.x =  K.x + sepV.z      #     maxSeparation := orig(sepV).z
    PN.x            =  K.x - inR.z       #     PN := -inR.z - orig(sepV).z
    PN.x            =  PN.x - sepV.z     #
    normalsign.x    =  K.x + normDV.z    #     normalSign := normDV.z
    'addi              $9,$0,0xC|2       #     normInfo   := 0xC|2    
    'z_not_largest:                      # }

#if DUMPDEBUGDATA
    # Debug point 5
    'sqc2              @maxSeparation,0xf0(%1)
    'sw                $9,0xf4(%1)
    # --------------
#endif

    'sqc2 @mincoord,0x00(%2);

    ~mincoord
    ~maxcoord
    ~sumRV
    ~normDV
    ~anormDV
    ~sepV
    
    # Edge separation

    # registers [11.18)
    aE0
    aE1
    aE2
    sqE
    negaE0
    negaE1
    negaE2

    'vabs @aE0, @mEdge0
    'vabs @aE1, @mEdge1
    'vabs @aE2, @mEdge2

    negaE0.xyz = K.xyz - aE0
    negaE1.xyz = K.xyz - aE1
    negaE2.xyz = K.xyz - aE2

    temp

    temp.xyz         = mEdge0.xyz * mEdge0
    sqE.x            = temp.x + temp.y
    sqE.x            = sqE.x + temp.z
    
    temp.xyz         = mEdge1.xyz * mEdge1
    sqE.y            = temp.y + temp.x
    sqE.y            = sqE.y + temp.z

    temp.xyz         = mEdge2.xyz * mEdge2
    sqE.z            = temp.z + temp.y
    sqE.z            = sqE.z + temp.x
    ~temp

#if DUMPDEBUGDATA
    # Debug point 6

    'sqc2 @aE0,0x100(%1)
    'sqc2 @aE1,0x110(%1)
    'sqc2 @aE2,0x120(%1)
    'sqc2 @sqE,0x130(%1)
    'sqc2 @sqE,0x140(%1)
    'sqc2 @sqE,0x150(%1)
    'sqc2 @negaE0,0x160(%1)
    'sqc2 @negaE1,0x170(%1)
    'sqc2 @negaE2,0x180(%1)
    
    # -------------
#endif

    # Edges
    #
    # On Entry:
    #
    # mEdge0
    # mEdge1
    # mEdge2
    # mVertex0
    # mVertex1
    # mVertex2
    # aE0
    # aE1
    # aE2
    # inR
    # half
    # negaE0
    # negaE1
    # negaE2

    # registers [20..31)
    em
    en
    vm
    vn
    sRV
    rBV
    normDV
    sumRV
    anormDV
    sepV

    # Edge (0,1)

    em = mEdge0 + K.x
    en = mEdge1 + K.x
    vm = mVertex0 + K.x
    vn = mVertex1 + K.x

    'vopmula.xyz ACC,@en,@em
    'vopmsub.xyz @sRV,@em,@en

    sRV = sRV * half.x
    'vabs @rBV, @sRV

    'vopmula.xyz ACC,@inR,@aE0
    'vopmsub.xyz @sumRV,@negaE0,@inR

    sumRV = rBV + sumRV

    'vopmula.xyz ACC,@vm,@em
    'vopmsub.xyz @normDV,@em,@vm

    normDV = normDV + sRV
    'vabs  @anormDV,@normDV
    
    sepV = anormDV - sumRV

#if DUMPDEBUGDATA
    # Debug point 7

    'sqc2 @em,0x190(%1)
    'sqc2 @en,0x1a0(%1)
    'sqc2 @vm,0x1b0(%1)
    'sqc2 @vn,0x1c0(%1)
    'sqc2 @sRV,0x1d0(%1)
    'sqc2 @rBV,0x1e0(%1)
    'sqc2 @sepV,0x200(%1)

    # -------------
#endif

    # Check box axes

    rlen

    ACC.xyz  = K.xyz + sqE.x 
    rlen.xyz = ACC.xyz - aE0 * aE0

    'mul.s $f2,$f1,$f1
    # $f2 = eps2 = eps*eps

    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation    

#if DUMPDEBUGDATA
    # Debug point 8

    'sqc2 @rlen,0x210(%1)
    'swc1 $f2,0x220(%1)

    # -------------
#endif

    # Axis 0

    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = rlen.x
    'c.lt.s    $f2,$f3           # if eps2 < rlen.x
    'bc1f      notaxis0          # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(rLen.x)
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = sepV.x
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*rLen.x
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*rlen
    'c.lt.s    $f7,$f6           #     if eps*rLen.x < sepV.x-maxSeparation*rLen.x
    'bc1f      notaxis0sep       #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(rLen.x)
    'mul.s     $f6,$f5,$f3       #         $f6 = sepV.x/MeSqrt(rLen.x)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(rLen.x)
    'qmtc2     $8,@rlen          #             
    PN.x       = rBV.x-anormDV   #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*rLen
    normalsign.x = K.x + normDV  #         normalSign = normDV[j]       
    'addi      $9,$0,0           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = rLen
    'notaxis0sep:                #     }
    'notaxis0:                   # }

#if DUMPDEBUGDATA
    # Debug point 9
    # -------------

    'sqc2 @maxSeparation,0x230(%1)
    'sw                $9,0x234(%1)
    'sqc2 @PN,0x240(%1)
    'sqc2 @normalsign,0x250(%1)
    'sqc2 @nRLen,0x260(%1)
#endif

    # Axis 1
    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'vmr32     @rlen,@rlen         # rlen.x = orig(rlen).y

#if DUMPDEBUGDATA
    # Debug point 10
    # --------------

    'sqc2 @rlen,0x270(%1)
    'swc1 $f2,0x280(%1)
#endif

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = orig(rlen).y
    'c.lt.s    $f2,$f3           # if eps2 < orig(rlen).y
    'bc1f      notaxis1          # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(orig(rlen).y)
    'vmr32     @sepV,@sepV       #     sepV.x = orig(sepV).y
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = orig(sepV).y
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*orig(rlen).y
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*orig(rlen).y
    'c.lt.s    $f7,$f6           #     if eps*sqrt(orig(rLen.y)) < 
                                 #        orig(sepV).y-maxSeparation*sqrt(orig(rlen).y)
    'bc1f      notaxis1sep       #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(orig(rlen).y)
    'mul.s     $f6,$f5,$f3       #         $f6 = orig(sepV).y/MeSqrt(orig(rlen).y)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(orig(rlen).y)
    'qmtc2     $8,@rlen          #             
    PN.x       = K + rBV.y       #
    PN.x       = PN.x -anormDV.y #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*orig(rlen).y
    normalsign.x = K.x + normDV.y#         normalSign = normDV[j]       
    'addi      $9,$0,1           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = orig(rlen).y
    'notaxis1sep:                #     }
    'notaxis1:                   # }

#if DUMPDEBUGDATA
    # Debug point 11
    # --------------

    'sqc2 @maxSeparation,0x290(%1)
    'sw                $9,0x294(%1)
    'sqc2 @PN,0x2a0(%1)
    'sqc2 @normalsign,0x2b0(%1)
    'sqc2 @nRLen,0x2c0(%1)
#endif

    # Axis 2
    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'vmr32     @rlen,@rlen       # rlen.x = orig(rlen).z

#if DUMPDEBUGDATA
    # Debug point 12
    # --------------

    'sqc2 @rlen,0x2d0(%1)
    'swc1 $f2,0x2e0(%1)
#endif

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = orig(rlen).z
    'c.lt.s    $f2,$f3           # if eps2 < orig(rlen).z
    'bc1f      notedge0axis2     # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(orig(rlen).z)
    'vmr32     @sepV,@sepV       #     sepV.x = orig(sepV).z
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = orig(sepV).z
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*orig(rlen).z
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*orig(rlen).z
    'c.lt.s    $f7,$f6           #     if eps*rLen.x < orig(sepV).y-maxSeparation*orig(rlen).z
    'bc1f      notedge0axis2sep  #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(orig(rlen).y)
    'mul.s     $f6,$f5,$f3       #         $f6 = orig(sepV).z/MeSqrt(orig(rlen).z)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(orig(rlen).z)
    'qmtc2     $8,@rlen          #             
    PN.x       = K + rBV.z       #
    PN.x       = PN.x -anormDV.z #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*orig(rlen).z
    normalsign.x = K.x + normDV.z#         normalSign = normDV[j]       
    'addi      $9,$0,2           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = orig(rlen).z
    'notedge0axis2sep:           #     }
    'notedge0axis2:              # }

#if DUMPDEBUGDATA
    # Debug point 13
    # --------------

    'sqc2 @maxSeparation,0x2f0(%1)
    'sw                $9,0x2f4(%1)
    'sqc2 @PN,0x300(%1)
    'sqc2 @normalsign,0x310(%1)
    'sqc2 @nRLen,0x320(%1)
#endif

    # Edge (1,2)

    em = mEdge1 + K.x
    en = mEdge2 + K.x
    vm = mVertex1 + K.x
    vn = mVertex2 + K.x

    'vopmula.xyz ACC,@en,@em
    'vopmsub.xyz @sRV,@em,@en

    sRV = sRV * half.x
    'vabs @rBV, @sRV

    'vopmula.xyz ACC,@inR,@aE1
    'vopmsub.xyz @sumRV,@negaE1,@inR

    sumRV = rBV + sumRV

    'vopmula.xyz ACC,@vm,@em
    'vopmsub.xyz @normDV,@em,@vm

    normDV = normDV + sRV
    'vabs  @anormDV,@normDV
    
    sepV = anormDV - sumRV

#if DUMPDEBUGDATA
    # Debug point 14

    'sqc2 @em,0x330(%1)
    'sqc2 @en,0x340(%1)
    'sqc2 @vm,0x350(%1)
    'sqc2 @vn,0x360(%1)
    'sqc2 @sRV,0x370(%1)
    'sqc2 @rBV,0x380(%1)
    'sqc2 @sepV,0x3a0(%1)

    # -------------
#endif

    # Check box axes

    ACC.xyz  = K.xyz + sqE.y 
    rlen.xyz = ACC.xyz - aE1 * aE1

    'mul.s $f2,$f1,$f1
    # $f2 = eps2 = eps*eps

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation    

#if DUMPDEBUGDATA
    # Debug point 15

    'sqc2 @rlen,0x3b0(%1)
    'swc1 $f2,0x3c0(%1)

    # -------------
#endif

    # Axis 0
    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = rlen.x
    'c.lt.s    $f2,$f3           # if eps2 < rlen.x
    'bc1f      notedge1axis0     # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(rLen.x)
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = sepV.x
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*rLen.x
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*rlen
    'c.lt.s    $f7,$f6           #     if eps*rLen.x < sepV.x-maxSeparation*rLen.x
    'bc1f      notedge1axis0sep  #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(rLen.x)
    'mul.s     $f6,$f5,$f3       #         $f6 = sepV.x/MeSqrt(rLen.x)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(rLen.x)
    'qmtc2     $8,@rlen          #             
    PN.x       = rBV.x-anormDV   #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*rLen
    normalsign.x = K.x + normDV  #         normalSign = normDV[j]       
    'addi      $9,$0,0           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = rLen
    'notedge1axis0sep:           #     }
    'notedge1axis0:              # }

#if DUMPDEBUGDATA
    # Debug point 16
    # -------------

    'sqc2 @maxSeparation,0x3d0(%1)
    'sw                $9,0x3d4(%1)
    'sqc2 @PN,0x3e0(%1)
    'sqc2 @normalsign,0x3f0(%1)
    'sqc2 @nRLen,0x400(%1)
#endif

    # Axis 1
    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'vmr32     @rlen,@rlen         # rlen.x = orig(rlen).y

#if DUMPDEBUGDATA
    # Debug point 17
    # --------------

    'sqc2 @rlen,0x410(%1)
    'swc1 $f2,0x420(%1)
#endif

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = orig(rlen).y
    'c.lt.s    $f2,$f3           # if eps2 < orig(rlen).y
    'bc1f      notedge1axis1     # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(orig(rlen).y)
    'vmr32     @sepV,@sepV       #     sepV.x = orig(sepV).y
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = orig(sepV).y
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*orig(rlen).y
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*orig(rlen).y
    'c.lt.s    $f7,$f6           #     if eps*rLen.x < orig(sepV).y-maxSeparation*orig(rlen).y
    'bc1f      notedge1axis1sep  #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(orig(rlen).y)
    'mul.s     $f6,$f5,$f3       #         $f6 = orig(sepV).y/MeSqrt(orig(rlen).y)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(orig(rlen).y)
    'qmtc2     $8,@rlen          #             
    PN.x       = K + rBV.y       #
    PN.x       = PN.x -anormDV.y #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*orig(rlen).y
    normalsign.x = K.x + normDV.y#         normalSign = normDV[j]       
    'addi      $9,$0,1           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = orig(rlen).y
    'notedge1axis1sep:           #     }
    'notedge1axis1:              # }

#if DUMPDEBUGDATA
    # Debug point 18
    # --------------

    'sqc2 @maxSeparation,0x430(%1)
    'sw                $9,0x434(%1)
    'sqc2 @PN,0x440(%1)
    'sqc2 @normalsign,0x450(%1)
    'sqc2 @nRLen,0x460(%1)
#endif

    # Axis 2
    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'vmr32     @rlen,@rlen       # rlen.x = orig(rlen).z

#if DUMPDEBUGDATA
    # Debug point 19
    # --------------

    'sqc2 @rlen,0x470(%1)
    'swc1 $f2,0x480(%1)
#endif

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = orig(rlen).z
    'c.lt.s    $f2,$f3           # if eps2 < orig(rlen).z
    'bc1f      notedge1axis2     # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(orig(rlen).z)
    'vmr32     @sepV,@sepV       #     sepV.x = orig(sepV).z
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = orig(sepV).z
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*orig(rlen).z
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*orig(rlen).z
    'c.lt.s    $f7,$f6           #     if eps*rLen.x < orig(sepV).y-maxSeparation*orig(rlen).z
    'bc1f      notedge1axis2sep  #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(orig(rlen).y)
    'mul.s     $f6,$f5,$f3       #         $f6 = orig(sepV).z/MeSqrt(orig(rlen).z)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(orig(rlen).z)
    'qmtc2     $8,@rlen          #             
    PN.x       = K + rBV.z       #
    PN.x       = PN.x -anormDV.z #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*orig(rlen).z
    normalsign.x = K.x + normDV.z#         normalSign = normDV[j]       
    'addi      $9,$0,2           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = orig(rlen).z
    'notedge1axis2sep:           #     }
    'notedge1axis2:              # }

#if DUMPDEBUGDATA
    # Debug point 20
    # --------------

    'sqc2 @maxSeparation,0x490(%1)
    'sw                $9,0x494(%1)
    'sqc2 @PN,0x4a0(%1)
    'sqc2 @normalsign,0x4b0(%1)
    'sqc2 @nRLen,0x4c0(%1)
#endif

    # Edge (2,0)

    em = mEdge2 + K.x
    en = mEdge0 + K.x
    vm = mVertex2 + K.x
    vn = mVertex0 + K.x

    'vopmula.xyz ACC,@en,@em
    'vopmsub.xyz @sRV,@em,@en

    sRV = sRV * half.x
    'vabs @rBV, @sRV

    'vopmula.xyz ACC,@inR,@aE2
    'vopmsub.xyz @sumRV,@negaE2,@inR

    sumRV = rBV + sumRV

    'vopmula.xyz ACC,@vm,@em
    'vopmsub.xyz @normDV,@em,@vm

    normDV = normDV + sRV
    'vabs  @anormDV,@normDV
    
    sepV = anormDV - sumRV

#if DUMPDEBUGDATA
    # Debug point 21

    'sqc2 @em,0x4d0(%1)
    'sqc2 @en,0x4e0(%1)
    'sqc2 @vm,0x4f0(%1)
    'sqc2 @vn,0x500(%1)
    'sqc2 @sRV,0x510(%1)
    'sqc2 @rBV,0x520(%1)
    'sqc2 @sepV,0x540(%1)

    # -------------
#endif

    # Check box axes

    ACC.xyz  = K.xyz + sqE.z 
    rlen.xyz = ACC.xyz - aE2 * aE2

    'mul.s $f2,$f1,$f1
    # $f2 = eps2 = eps*eps

#if DUMPDEBUGDATA
    # Debug point 22

    'sqc2 @rlen,0x550(%1)
    'swc1 $f2,0x560(%1)

    # -------------
#endif

    # Axis 0

    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = rlen.x
    'c.lt.s    $f2,$f3           # if eps2 < rlen.x
    'bc1f      notaxis0          # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(rLen.x)
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = sepV.x
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*rLen.x
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*rlen
    'c.lt.s    $f7,$f6           #     if eps*rLen.x < sepV.x-maxSeparation*rLen.x
    'bc1f      notedge2axis0sep  #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(rLen.x)
    'mul.s     $f6,$f5,$f3       #         $f6 = sepV.x/MeSqrt(rLen.x)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(rLen.x)
    'qmtc2     $8,@rlen          #             
    PN.x       = rBV.x-anormDV   #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*rLen
    normalsign.x = K.x + normDV  #         normalSign = normDV[j]       
    'addi      $9,$0,0           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = rLen
    'notedge2axis0sep:           #     }
    'notedge2axis0:              # }

#if DUMPDEBUGDATA
    # Debug point 23
    # -------------

    'sqc2 @maxSeparation,0x570(%1)
    'sw                $9,0x574(%1)
    'sqc2 @PN,0x580(%1)
    'sqc2 @normalsign,0x590(%1)
    'sqc2 @nRLen,0x5a0(%1)
#endif

    # Axis 1

    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'vmr32     @rlen,@rlen         # rlen.x = orig(rlen).y

#if DUMPDEBUGDATA
    # Debug point 24
    # --------------

    'sqc2 @rlen,0x5b0(%1)
    'swc1 $f2,0x5c0(%1)
#endif

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = orig(rlen).y
    'c.lt.s    $f2,$f3           # if eps2 < orig(rlen).y
    'bc1f      notedge2axis1          # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(orig(rlen).y)
    'vmr32     @sepV,@sepV       #     sepV.x = orig(sepV).y
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = orig(sepV).y
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*orig(rlen).y
    'sub.s     $f6,$f5,$f6       #     $f6 = orig(sepV).y - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*orig(rlen).y
    'c.lt.s    $f7,$f6           #     if eps*rLen.x <
                                 #        orig(sepV).y-maxSeparation*MeSqrt(orig(rlen).y)
    'bc1f      notedge2axis1sep  #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(orig(rlen).y)
    'mul.s     $f6,$f5,$f3       #         $f6 = orig(sepV).y/MeSqrt(orig(rlen).y)
    'c.lt.s    $f1,$f6           #         if inEps < orig(sepV).y/MeSqrt(orig(rlen).y)
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(orig(rlen).y)
    'qmtc2     $8,@rlen          #             
    PN.x       = K + rBV.y       #
    PN.x       = PN.x -anormDV.y #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*orig(rlen).y
    normalsign.x = K.x + normDV.y#         normalSign = normDV[j]       
    'addi      $9,$0,1           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = orig(rlen).y
    'notedge2axis1sep:           #     }
    'notedge2axis1:              # }

#if DUMPDEBUGDATA
    # Debug point 25
    # --------------

    'sqc2 @maxSeparation,0x5d0(%1)
    'sw                $9,0x5d4(%1)
    'sqc2 @PN,0x5e0(%1)
    'sqc2 @normalsign,0x5f0(%1)
    'sqc2 @nRLen,0x600(%1)
#endif

    # Axis 2
    'qmfc2     $8,@maxSeparation #
    'mtc1      $8,$f4            # f4 = maxSeparation
    # $f4 = maxSeparation        

    'vmr32     @rlen,@rlen       # rlen.x = orig(rlen).z

#if DUMPDEBUGDATA
    # Debug point 26
    # --------------

    'sqc2 @rlen,0x610(%1)
    'swc1 $f2,0x620(%1)
#endif

    'qmfc2     $8,@rlen          #
    'mtc1      $8,$f3            # f3 = orig(rlen).z
    'c.lt.s    $f2,$f3           # if eps2 < orig(rlen).z
    'bc1f      notedge2axis2     # {
    'sqrt.s    $f3,$f3           #     $f3 = MeSqrt(orig(rlen).z)
    'vmr32     @sepV,@sepV       #     sepV.x = orig(sepV).z
    'qmfc2     $10,@sepV         #
    'mtc1      $10,$f5           #     $f5 = orig(sepV).z
    'mul.s     $f6,$f4,$f3       #     $f6 = maxSeparation*orig(rlen).z
    'sub.s     $f6,$f5,$f6       #     $f6 = sepV.x - $f6
    'mul.s     $f7,$f1,$f3       #     $f7 = eps*orig(rlen).z
    'c.lt.s    $f7,$f6           #     if eps*rLen.x < orig(sepV).y-maxSeparation*orig(rlen).z
    'bc1f      notedge2axis2sep  #     {
    'div.s     $f3,$f8,$f3       #         $f3 = 1/MeSqrt(orig(rlen).y)
    'mul.s     $f6,$f5,$f3       #         $f6 = orig(sepV).z/MeSqrt(orig(rlen).z)
    'c.lt.s    $f1,$f6           #         if inEps < maxSeparation
    'bc1t      early_out         #             return
    'mfc1      $8,$f3            #         rlen.x = 1/MeSqrt(orig(rlen).z)
    'qmtc2     $8,@rlen          #             
    PN.x       = K + rBV.z       #
    PN.x       = PN.x -anormDV.z #
    PN.x       = PN.x * rlen     #         PN = (rBV[j]-aNormDV[j])*orig(rlen).z
    normalsign.x = K.x + normDV.z#         normalSign = normDV[j]       
    'addi      $9,$0,2           #         normInfo = (0<<2)|j
    nRLen.x    = K.x + rlen.x    #         nRLen = orig(rlen).z
    'notedge2axis2sep:           #     }
    'notedge2axis2:              # }

#if DUMPDEBUGDATA
    # Debug point 27
    # --------------

    'sqc2 @maxSeparation,0x630(%1)
    'sw                $9,0x634(%1)
    'sqc2 @PN,0x640(%1)
    'sqc2 @normalsign,0x650(%1)
    'sqc2 @nRLen,0x660(%1)
#endif

    ~aE0
    ~aE1
    ~aE2
    ~sqE
    ~negaE0
    ~negaE1
    ~negaE2

    'addi $8,$0,1        # Didnt early out, so set retval to true 
    'sw   $8,0x00(%0)    #

    'early_out:

        
    # Store the things we need

    'sqc2 @normalsign,0x00(%3)
    'sqc2 @maxSeparation,0x00(%4)
    'sw   $9,0x00(%5)
    'sqc2 @PN,0x00(%6)
    'sqc2 @nRLen,0x00(%7)

    ~inR
    ~mVertex0
    ~mVertex1
    ~mVertex2
    ~mEdge0
    ~mEdge1
    ~mEdge2
    ~PN
    ~normalsign
    ~half

    __end_expression_asm
    " : : "r" (&retval) ,
          "r" (&debug) , 
          "r" (&minCoord.v[0]) , 
          "r" (normalSign) , 
          "r" (maxSeparation) , 
          "r" (&normInfo) , 
          "r" (PN) , 
          "r" (nRLen) : "$8" , "$9", "$10" );

#if 0
    printf("--- Debug point 0\n");
    printvec(debug.inR,"inR");
    printvec(debug.aNorm,"aNorm");
    printvec(debug.mNormal,"inTri.mNormal");
    printvec(debug.mVertex0,"mVertex0");

    printf("--- Debug point 1\n");
    printreal(debug.sumR         ,"sumR");
    printreal(debug.normD        ,"normD");
    printreal(debug.aNormD       ,"aNormD");
    printreal(debug.maxSeparation,"maxSeparation");
    printint (debug.maxSeparation+1,"normInfo");

    printf("--- Debug point 2\n");
    printvec (debug.minCoord    ,"minCoord");
    printvec (debug.maxCoord    ,"maxCoord");
    printvec (debug.sumRV       ,"sumRV");
    printvec (debug.normDV      ,"normDV");
    printvec (debug.sepV        ,"sepV");

    printf("--- Debug point 3\n");
    printreal(debug.p3_maxSeparation,"maxSeparation");
    printint (debug.p3_maxSeparation+1,"normInfo");

    printf("--- Debug point 4\n");
    printreal(debug.p4_maxSeparation,"maxSeparation");
    printint (debug.p4_maxSeparation+1,"normInfo");

    printf("--- Debug point 5\n");
    printreal(debug.p5_maxSeparation,"maxSeparation");
    printint (debug.p5_maxSeparation+1,"normInfo");

    printf("--- Debug point 6\n");
    printvec (debug.aE0         ,"aE0");
    printvec (debug.aE1         ,"aE1");
    printvec (debug.aE2         ,"aE2");
    printvec (debug.sqE0        ,"sqE");

    printf("--- Debug point 7\n");

    printvec (debug.p7_em      ,"em");
    printvec (debug.p7_en      ,"en");
    printvec (debug.p7_vm      ,"vm");
    printvec (debug.p7_vn      ,"vn");
    printvec (debug.p7_sRV     ,"sRV");
    printvec (debug.p7_rBV     ,"rBV");
    printvec (debug.p7_sepV  ,"sepV");

    printf("--- Debug point 8\n");

    printreal(debug.p8_rlen      ,"rlen");
    printreal(debug.p8_eps2      ,"eps2");

    printf("--- Debug point 9\n");

    printreal(debug.p9_maxSeparation, "maxSeparation");
    printint (debug.p9_maxSeparation+1,"normInfo");
    printreal(debug.p9_PN,            "PN");
    printreal(debug.p9_normalsign,    "normalsign");
    printreal(debug.p9_nRLen,         "nRLen");

    printf("--- Debug point 10\n");

    printreal(debug.p10_rlen      ,"rlen");
    printreal(debug.p10_eps2      ,"eps2");

    printf("--- Debug point 11\n");

    printreal(debug.p11_maxSeparation, "maxSeparation");
    printint (debug.p11_maxSeparation+1,"normInfo");
    printreal(debug.p11_PN,            "PN");
    printreal(debug.p11_normalsign,    "normalsign");
    printreal(debug.p11_nRLen,         "nRLen");

    printf("--- Debug point 12\n");

    printreal(debug.p12_rlen      ,"rlen");
    printreal(debug.p12_eps2      ,"eps2");

    printf("--- Debug point 13\n");

    printreal(debug.p13_maxSeparation, "maxSeparation");
    printint (debug.p13_maxSeparation+1,"normInfo");
    printreal(debug.p13_PN,            "PN");
    printreal(debug.p13_normalsign,    "normalsign");
    printreal(debug.p13_nRLen,         "nRLen");

    printf("--- Debug point 14\n");

    printvec (debug.p14_em      ,"em");
    printvec (debug.p14_en      ,"en");
    printvec (debug.p14_vm      ,"vm");
    printvec (debug.p14_vn      ,"vn");
    printvec (debug.p14_sRV     ,"sRV");
    printvec (debug.p14_rBV     ,"rBV");
    printvec (debug.p14_sepV    ,"sepV");

    printf("--- Debug point 15\n");

    printreal(debug.p15_rlen      ,"rlen");
    printreal(debug.p15_eps2      ,"eps2");

    printf("--- Debug point 16\n");

    printreal(debug.p16_maxSeparation, "maxSeparation");
    printint (debug.p16_maxSeparation+1,"normInfo");
    printreal(debug.p16_PN,            "PN");
    printreal(debug.p16_normalsign,    "normalsign");
    printreal(debug.p16_nRLen,         "nRLen");

    printf("--- Debug point 17\n");

    printreal(debug.p17_rlen      ,"rlen");
    printreal(debug.p17_eps2      ,"eps2");

    printf("--- Debug point 18\n");

    printreal(debug.p18_maxSeparation, "maxSeparation");
    printint (debug.p18_maxSeparation+1,"normInfo");
    printreal(debug.p18_PN,            "PN");
    printreal(debug.p18_normalsign,    "normalsign");
    printreal(debug.p18_nRLen,         "nRLen");

    printf("--- Debug point 19\n");

    printreal(debug.p19_rlen      ,"rlen");
    printreal(debug.p19_eps2      ,"eps2");

    printf("--- Debug point 20\n");

    printreal(debug.p20_maxSeparation, "maxSeparation");
    printint (debug.p20_maxSeparation+1,"normInfo");
    printreal(debug.p20_PN,            "PN");
    printreal(debug.p20_normalsign,    "normalsign");
    printreal(debug.p20_nRLen,         "nRLen");

    printf("--- Debug point 21\n");

    printvec (debug.p21_em      ,"em");
    printvec (debug.p21_en      ,"en");
    printvec (debug.p21_vm      ,"vm");
    printvec (debug.p21_vn      ,"vn");
    printvec (debug.p21_sRV     ,"sRV");
    printvec (debug.p21_rBV     ,"rBV");
    printvec (debug.p21_sepV  ,"sepV");

    printf("--- Debug point 22\n");

    printreal(debug.p22_rlen      ,"rlen");
    printreal(debug.p22_eps2      ,"eps2");

    printf("--- Debug point 23\n");

    printreal(debug.p23_maxSeparation, "maxSeparation");
    printint (debug.p23_maxSeparation+1,"normInfo");
    printreal(debug.p23_PN,            "PN");
    printreal(debug.p23_normalsign,    "normalsign");
    printreal(debug.p23_nRLen,         "nRLen");

    printf("--- Debug point 24\n");

    printreal(debug.p24_rlen      ,"rlen");
    printreal(debug.p24_eps2      ,"eps2");

    printreal(debug.p24_rlen+1   ,"sepV");
    printreal(debug.p24_rlen+2   ,"maxsep");
    printreal(debug.p24_rlen+3   ,"sqrt(rlen.y)");

    printreal(debug.p24_eps2+1   ,"inEps");
    printreal(debug.p24_eps2+2   ,"maxSeparation");


    printf("--- Debug point 25\n");

    printreal(debug.p25_maxSeparation, "maxSeparation");
    printint (debug.p25_maxSeparation+1,"normInfo");
    printreal(debug.p25_PN,            "PN");
    printreal(debug.p25_normalsign,    "normalsign");
    printreal(debug.p25_nRLen,         "nRLen");

    printf("--- Debug point 26\n");

    printreal(debug.p26_rlen      ,"rlen");
    printreal(debug.p26_eps2      ,"eps2");

    printf("--- Debug point 27\n");

    printreal(debug.p27_maxSeparation, "maxSeparation");
    printint (debug.p27_maxSeparation+1,"normInfo");
    printreal(debug.p27_PN,            "PN");
    printreal(debug.p27_normalsign,    "normalsign");
    printreal(debug.p27_nRLen,         "nRLen");
#endif

#if 0
    printf("retval=%d\n",retval);
#endif
    if(retval==0) return false;

    /*
       On Entry
       --------
       normalSign
       maxSeparation
       normInfo

       inTri.mNormal
       inTri.mVertex[0]
       inR
       inTri.mEdge

       On Exit
       -------
       outN
       outPN
       dimA
       outDims
       axis
       dimB
    */

#if 0
    printf("--- Debug point 28\n");

    printreal (normalSign,"normalSign");
    printreal (maxSeparation,"maxSeparation");
    printint  (normInfo,"normInfo");
    printreal (PN,"PN");
    printreal (nRLen,"nRLen");
#endif

    lsVec3 aNorm;

    aNorm[0] = MeFabs(inTri.mNormal[0]);
    aNorm[1] = MeFabs(inTri.mNormal[1]);
    aNorm[2] = MeFabs(inTri.mNormal[2]);

    normalSign[0] = normalSign[0] > (MeReal)0 ? (MeReal)-1 : (MeReal)1;
    outSep = maxSeparation[0];
    
    // normal points from Tri to OBB
    if (normInfo == 3) 
    { // Normal is from Tri

#if 0        
        printf("Normal is from Tri\n");
#endif
        outN = normalSign[0]*inTri.mNormal;
        outPN = outN.dot(*inTri.mVertex[0])-maxSeparation[0];
        MeI16 dimA = (MeI16)(aNorm[0] < (MeReal)(1.0e-4))+
                     (MeI16)(aNorm[1] < (MeReal)(1.0e-4))+
                     (MeI16)(aNorm[2] < (MeReal)(1.0e-4));
        outDims = (2<<8)|dimA;
    }
    else if ((normInfo&0xC) == 0xC) 
    { // Normal is from OBB

#if 0
        printf("Normal is from OBB\n");
#endif
        MeI8 axis = normInfo&3;
        outN.setValue((MeReal)(0.0),(MeReal)(0.0),(MeReal)(0.0));
        outN[axis] = normalSign[0];
        outPN = -inR[axis]-maxSeparation[0];
        MeI16 dimB = (MeI16)(inTri.mVertex[0]->operator[](axis)-minCoord[axis] <= eps)+
            (MeI16)(inTri.mVertex[1]->operator[](axis)-minCoord[axis] <= eps)+
            (MeI16)(inTri.mVertex[2]->operator[](axis)-minCoord[axis] <= eps)-1;
        MCD_CHECK_ASSERT_(dimB >= 0 && dimB <= 2, "overlapOBBTri");
        outDims = (dimB<<8)|2;
    } 
    else
    { // Normal is from crossed edges

#if 0
        printf("Normal is from crossed edges\n");
#endif
        if (normalSign[0] > (MeReal)(0.0)) 
            outN = Vec3CrossAxis(inTri.mEdge[(normInfo&0xC)>>2],normInfo&3);
        else 
            outN = AxisCrossVec3(normInfo&3,inTri.mEdge[(normInfo&0xC)>>2]);
        outPN = PN[0];
        outDims = (1<<8)|1;
        outN[0] *= nRLen[0];
        outN[1] *= nRLen[1];
        outN[2] *= nRLen[2];
    }
    
#if 0
    printf("BoxTriIntersect\n");
#endif
    lsVec3 *posList = outPos;
    BoxTriIntersect(outPos,inR,inTri);
    
    return outPos != posList;
}

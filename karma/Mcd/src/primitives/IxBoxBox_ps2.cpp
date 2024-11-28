/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:56 $ - Revision: $Revision: 1.2.2.17 $

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
#include <MePS2VURegs.h>

#include <McdProfile.h>
#include <vectormath.h>

#include <McdCheck.h>
#include <McdCTypes.h>
#include <McdBox.h>
#include <McdModel.h>
#include <McdContact.h>
#include <MovingBoxBoxIntersect.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>


#include "IxBoxBoxVec4.hpp"

#define McdBOXSIMD      0
#define McdBOXPS2       1

#define McdBOXPS2C      0
#define McdBOXPS2MACRO  1
#define McdBOXPS2MICRO  2

#define McdBOXPS2CODE           McdBOXPS2MACRO
#define McdBOXPS2MACRODEBUG     0

#ifndef McdBOX
#   ifdef PS2
#       define McdBOX   McdBOXPS2
#   else
#       define McdBOX   McdBOXSIMD
#   endif
#endif

#define SAFE_RECIP(_x)  (((_x) != (MeReal) 0.0f) \
                            ? ((MeReal) 1.0f/(_x)) : (MeReal) 0.0f)

#if (McdBOXPS2MACRODEBUG)
#   define debugDOIT(C)         (((C) % 1) == 0)
    static unsigned debugCounter = 0;
#else
#   define debugDOIT(C)         0
#endif

/*
    Box to Box collision detection
*/

#if (McdBOX == McdBOXSIMD)
static inline int McdSIMDSegmentCubeIntersect(
    lsVec4 &in, lsVec4 &out,
    const lsVec4 orig[3],
    const lsVec3 &disp,const lsVec3 &inR,MeReal scale)
{
    const MeReal eps = (MeReal) 1e-6f * scale;
    int res = 0x0f;
    int j;

#if (McdBOXPS2MACRODEBUG)
    if (debugDOIT(debugCounter))
    {
        MeDebug(0,"\n%s",
            "McdSIMDSegmentCubeIntersect\n"
            "===========================");

        MeDebug(0,"disp [%5.2f,%5.2f,%5.2f]",
            disp[0],disp[1],disp[2]);
        MeDebug(0,"initial in [%5.2f,%5.2f,%5.2f,%5.2f]",
            in[0],in[1],in[2],in[3]);
        MeDebug(0,"initial out [%5.2f,%5.2f,%5.2f,%5.2f]",
            out[0],out[1],out[2],out[3]);
    }
#endif

#if 0
    McdProfileStart("McdSIMDSegmentCubeIntersect");
#endif

    for (j = 0; j < 3; j++)
    {
        lsVec4 RV;

        Vec4Set_F(RV,inR[j]);

        if (MeFabs(disp[j]) <= eps)
        {
            lsVec4 aorig;

            Vec4Set_Abs4(aorig,orig[j]);

            res &= (aorig <= RV);
        }
        else
        {
            lsVec4 axin, axout;

            /* this is needed almost always, so should probably preschedule */
            const MeReal invdisp = (MeReal) 1.0f/disp[j];

            axin  = -(orig[j]*invdisp) - RV*MeFabs(invdisp);
            axout = -(orig[j]*invdisp) + RV*MeFabs(invdisp);

            Vec4Set_Max44(in, in, axin);
            Vec4Set_Min44(out,out,axout);
        }
    }

    res &= (in <= out);

#if 0
    McdProfileEnd("McdSIMDSegmentCubeIntersect");
#endif

#if (McdBOXPS2MACRODEBUG)
    if (debugDOIT(debugCounter))
    {
        MeDebug(0,"final in [%5.2f,%5.2f,%5.2f,%5.2f]",
            in[0],in[1],in[2],in[3]);
        MeDebug(0,"final out [%5.2f,%5.2f,%5.2f,%5.2f]",
            out[0],out[1],out[2],out[3]);
        MeDebug(0,"final res: 0x%08x",res);
    }
#endif

#if (McdBOXPS2MACRODEBUG)
    debugCounter++;
#endif

    return res;
}
#endif

#if (McdBOX == McdBOXPS2)
/*
   On PS2 VUs we have 32 vector float registers; they are many enough
   that we can alias every local variable to one, for the whole function
   body, without looking for narrower scopes.
*/

static inline int McdPS2SegmentCubeIntersect(
    lsVec4 &in,lsVec4 &out,
    const lsVec4 orig[3],
    const lsVec3 &disp,const lsVec3 &inR,MeReal scale)
#   define _in                  "vf01"
#   define _out                 "vf02"
#   define _orig0               "vf03"
#   define _orig1               "vf04"
#   define _orig2               "vf05"
{
    const MeReal eps = (MeReal) 1e-6f * scale;
#   define _eps                 "vf06"
    int res = 0x0f;

    lsVec4 disp4,disp4Abs,disp4Inv,disp4InvAbs;
#   define _disp4               "vf07"
#   define _disp4Abs            "vf08"
#   define _disp4Inv            "vf09"
#   define _disp4InvAbs         "vf10"

    lsVec4 inR4,inR4_Div_disp4Abs;
#   define _inR4                "vf11"
#   define _inR4_Div_disp4Abs   "vf12"

    lsVec4 origAbs[3],origNeg_Div_disp[3];
#   define _origAbs0            "vf13"
#   define _origAbs1            "vf14"
#   define _origAbs2            "vf15"
#   define _origNeg_Div_disp0   "vf16"
#   define _origNeg_Div_disp1   "vf17"
#   define _origNeg_Div_disp2   "vf18"

    lsVec4 axin,axout;
#   define _axin                "vf19"
#   define _axout               "vf20"

    /*
        These are the flags for x,y,z,w; unfortunately the PS2 returns
        them in the wrong order (bit 0 is w, not x), so for the PS2 only
        we will have to invert them before '&'ing them with 'res'.
    */
    unsigned flags_res;
    unsigned flags_dispAbsLEeps;
    unsigned flags_disp4EQzero;

    unsigned t0,t1;

    /*
        Now that most alternate C/debugging code has disappeared, what
        remains to be done is handling the conditionals in the SAFE_RECIP
        equivalents and scehduling.

        Once the macrocode has been sussed, the corresponding microcode
        should be easy; but is there a point? I mean, if it runs
        entirely within registers... There may be a point if one can do
        mailboxing/double buffering, but there the streams are _very_
        short.
    */

#if 0
    McdProfileStart("McdPS2SegmentCubeIntersect");
#endif

#if (McdBOXPS2MACRODEBUG)
    if (debugDOIT(debugCounter))
    {
        MeDebug(0,"\n%s",
#   if (McdBOXPS2CODE == McdBOXPS2C)
            "McdPS2SegmentCubeIntersect (C version)\n"
#   elif (McdBOXPS2CODE == McdBOXPS2MACRO)
            "McdPS2SegmentCubeIntersect (macrocode version)\n"
#   else
            "McdPS2SegmentCubeIntersect (UNKNOWN version)\n"
#   endif
            "==========================");

        MeDebug(0,"disp [%5.2f,%5.2f,%5.2f]",
            disp[0],disp[1],disp[2]);
        MeDebug(0,"initial in [%5.2f,%5.2f,%5.2f,%5.2f]",
            in[0],in[1],in[2],in[3]);
        MeDebug(0,"initial out [%5.2f,%5.2f,%5.2f,%5.2f]",
            out[0],out[1],out[2],out[3]);
    }
#endif

    /*
        Here we must compute the reciprocals of 'disp[0..2]', but the
        result must be zero if the operand is zero.  Unfortunately there
        are no macrocode conditionals, so we need to test against zero
        on the CPU. Easiest way is to put all those three values in both
        CPU and VU0 registers, test and branch on the CPU registers, while
        doing the division if needed on VU0.
    */

    asm volatile("\n"
        "       lqc2            "_inR4",(%5)\n"
        "       lqc2            "_disp4",(%6)\n"

        "       lqc2            "_orig0",(%2)\n"
        "       lqc2            "_orig1",(%3)\n"
        "       lqc2            "_orig2",(%4)\n"

        "       lqc2            "_in",(%0)\n"
        "       lqc2            "_out",(%1)\n"

        "       vabs            "_disp4Abs","_disp4"\n"

        "       vabs            "_origAbs0","_orig0"\n"
        "       vabs            "_origAbs1","_orig1"\n"
        "       vabs            "_origAbs2","_orig2"\n"

        "       vsubw.w          "_inR4",vf00,vf00\n"
        "       vsubw.w          "_disp4",vf00,vf00\n"
        :
        :
        /* %0 */ "r" (in.v),
        /* %1 */ "r" (out.v),
        /* %2 */ "r" (orig[0].v),
        /* %3 */ "r" (orig[1].v),
        /* %4 */ "r" (orig[2].v),
        /* %5 */ "r" (inR.v),
        /* %6 */ "r" (disp.v)
    );

    asm volatile("\n"
        "       qmtc2           %2,"_eps"\n"
        "       vsubax          ACC,"_disp4Abs","_eps"\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       cfc2            %0,"_VU_MAC"\n"

        "       ori             %1,$0,0x0f\n"
        :
        /* %0 */ "=r" (flags_dispAbsLEeps),
        /* %1 */ "=r" (flags_res)
        :
        /* %2 */ "r" (eps)
    );

    asm volatile("\n"
        /*
            This preinitializes 'disp4Inv' to be the same as
            'disp4' and sets the VU_MAC flags to indicate those
            elements that are zero and we should leave alone
        */
        "       vsubx           "_disp4Inv","_disp4",vf00\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       cfc2            %0,"_VU_MAC"\n"
        :
        /* %0 */ "=r" (flags_disp4EQzero)
    );

    asm volatile("\n"
        /*
            The two VNOPs after each VADDQ are there as advised by
            note 
              <URL:https://www.devnet.scea.com/docman/display_doc.php?docid=2278&group_id=16437&raw=1>
        */

        /* if (!(disp4Inv[0] == 0)) then disp4Inv[0] = 1/disp4Inv[0] */

        "       andi            %0,%1,"_VU_MAC_ZX"\n"
        "       bnel            %0,$0,0f\n"
        "       nop\n"

        "       vdiv            Q,vf00w,"_disp4Inv"x\n"
        "       vwaitq\n"
        "       vaddq.x         "_disp4Inv",vf00,Q\n"
        "       vnop\n"
        "       vnop\n"
        "0:\n"

        /* if (!(disp4Inv[1] == 0)) then disp4Inv[1] = 1/disp4Inv[1] */

        "       andi            %0,%1,"_VU_MAC_ZY"\n"
        "       bnel            %0,$0,1f\n"
        "       nop\n"

        "       vdiv            Q,vf00w,"_disp4Inv"y\n"
        "       vwaitq\n"
        "       vaddq.y         "_disp4Inv",vf00,Q\n"
        "       vnop\n"
        "       vnop\n"
        "1:\n"

        /* if (!(disp4Inv[2] == 0)) then disp4Inv[2] = 1/disp4Inv[2] */

        "       andi            %0,%1,"_VU_MAC_ZZ"\n"
        "       bnel            %0,$0,2f\n"
        "       nop\n"

        "       vdiv            Q,vf00w,"_disp4Inv"z\n"
        "       vwaitq\n"
        "       vaddq.z         "_disp4Inv",vf00,Q\n"
        "       vnop\n"
        "       vnop\n"
        "2:\n"

        "       vsubw.w         "_disp4Inv",vf00,vf00\n"

        "       vabs            "_disp4InvAbs","_disp4Inv"\n"

        "       vmul            "_inR4_Div_disp4Abs
                                    ","_inR4","_disp4InvAbs"\n"

        "       vsuba           ACC,vf00,vf00\n"
        "       vmsubx          "_origNeg_Div_disp0","_orig0","_disp4Inv"\n"
        "       vmsuby          "_origNeg_Div_disp1","_orig1","_disp4Inv"\n"
        "       vmsubz          "_origNeg_Div_disp2","_orig2","_disp4Inv"\n"
        :
        /* %0 */ "=r" (t0)
        :
        /* %1 */ "r" (flags_disp4EQzero)
    );

    /*
        The cases for 'disp4Abs[0..2]' have to be unrolled for macrocode
        so we can assign register numbers for the [0], [1], [2] values
        statically.

        First we test all elements of 'disp4Abs', then we check the
        resulting flags and act accordingly.
    */

    MEASSERT(VU_MAC_Z_SHIFT == 0);
    MEASSERT((flags_res & ~0x0f) == 0);

    asm volatile ("\n"
        "       andi            %0,%3,"_VU_MAC_ZSX"\n"
        "       beq             %0,$0,0f\n"
        "       nop\n"

        /* when fabs(disp[0]) <= eps */
        "       vsubax          ACC,"_origAbs0","_inR4"\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       cfc2            %0,"_VU_MAC"\n"

        /* no need to mask with 0x0f */
        "       srl             %1,%0,"_VU_MAC_S_SHIFT"\n"
        "       or              %1,%1,%0\n"
        "       and             %2,%2,%1\n"
        
        "       j               1f\n"
        "       nop\n"
        
        "0:\n"
        /* when abs(disp[0]) > eps */
        "       vsubx           "_axin","_origNeg_Div_disp0
                                    ","_inR4_Div_disp4Abs"\n"
        "       vaddx           "_axout","_origNeg_Div_disp0
                                    ","_inR4_Div_disp4Abs"\n"

        "       vmax            "_in","_in","_axin"\n"
        "       vmini           "_out","_out","_axout"\n"
        "1:\n"
        :
        /* %0 */ "=r" (t0),
        /* %1 */ "=r" (t1),
        /* %2 */ "=r" (flags_res)
        :
        /* %3 */ "r" (flags_dispAbsLEeps),
        /* %4 */ "2" (flags_res)
    );

    asm volatile ("\n"
        "       andi            %0,%3,"_VU_MAC_ZSY"\n"
        "       beq             %0,$0,0f\n"
        "       nop\n"

        /* when fabs(disp[1]) <= eps */
        "       vsubay          ACC,"_origAbs1","_inR4"\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       cfc2            %0,"_VU_MAC"\n"

        /* no need to mask with 0x0f */
        "       srl             %1,%0,"_VU_MAC_S_SHIFT"\n"
        "       or              %1,%1,%0\n"
        "       and             %2,%2,%1\n"

        "       j               1f\n"
        "       nop\n"
        
        "0:\n"
        /* when fabs(disp[1]) > eps */
        "       vsuby           "_axin","_origNeg_Div_disp1
                                    ","_inR4_Div_disp4Abs"\n"
        "       vaddy           "_axout","_origNeg_Div_disp1
                                    ","_inR4_Div_disp4Abs"\n"

        "       vmax            "_in","_in","_axin"\n"
        "       vmini           "_out","_out","_axout"\n"
        "1:\n"
        :
        /* %0 */ "=r" (t0),
        /* %1 */ "=r" (t1),
        /* %2 */ "=r" (flags_res)
        :
        /* %3 */ "r" (flags_dispAbsLEeps),
        /* %4 */ "2" (flags_res)
    );

    asm volatile ("\n"
        "       andi            %0,%3,"_VU_MAC_ZSZ"\n"
        "       beq             %0,$0,0f\n"
        "       nop\n"

        /* when fabs(disp[2]) <= eps */
        "       vsubaz          ACC,"_origAbs2","_inR4"\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       cfc2            %0,"_VU_MAC"\n"

        /* no need to mask with 0x0f */
        "       srl             %1,%0,"_VU_MAC_S_SHIFT"\n"
        "       or              %1,%1,%0\n"
        "       and             %2,%2,%1\n"

        "       j               1f\n"
        "       nop\n"

        "0:\n"
        /* when abs(disp[2]) > eps */
        "       vsubz           "_axin","_origNeg_Div_disp2
                                    ","_inR4_Div_disp4Abs"\n"
        "       vaddz           "_axout","_origNeg_Div_disp2
                                    ","_inR4_Div_disp4Abs"\n"

        "       vmax            "_in","_in","_axin"\n"
        "       vmini           "_out","_out","_axout"\n"
        "1:\n"
        :
        /* %0 */ "=r" (t0),
        /* %1 */ "=r" (t1),
        /* %2 */ "=r" (flags_res)
        :
        /* %3 */ "r" (flags_dispAbsLEeps),
        /* %4 */ "2" (flags_res)
    );

    /*
        Needed unconditionally, 'in' and 'out' are return parms
        Put in as early as possible to hide the latency.
    */
    asm volatile("\n"
        "       sqc2                    "_in",(%0)\n"
        "       sqc2                    "_out",(%1)\n"
        :
        :
        /* %0 */ "r" (in.v),
        /* %1 */ "r" (out.v)
        : "memory"
    );

    asm volatile("\n"
        "       vsuba           ACC,"_in","_out"\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       vnop\n"
        "       cfc2            %0,"_VU_MAC"\n"

        /* no need to mask with 0x0f */        
        "       srl             %1,%0,"_VU_MAC_S_SHIFT"\n"
        "       or              %1,%1,%0\n"
        "       and             %2,%2,%1\n"
        :
        /* %0 */ "=r" (t0),
        /* %1 */ "=r" (t1),
        /* %2 */ "=r" (flags_res)
        :
        /* %3 */ "2" (flags_res)
    );

    /*
        Unfortunately the VU flags are in wzyx, not xyzw order like
        everything else, so we have to reverse the order of bits in
        'flags':

        res = res & (t1 = (flags_res & 0x08) >> 3 | (flags_res & 0x04) >> 1
                        | (flags_res & 0x02) << 1 | (flags_res & 0x01) << 3);
    */
    asm volatile("\n"
        "       andi            %0,%3,0x08\n"
        "       srl             %1,%0,3\n"

        "       andi            %0,%3,0x04\n"
        "       srl             %0,%0,1\n"
        "       or              %1,%1,%0\n"

        "       andi            %0,%3,0x02\n"
        "       sll             %0,%0,1\n"
        "       or              %1,%1,%0\n"

        "       andi            %0,%3,0x01\n"
        "       sll             %0,%0,3\n"
        "       or              %1,%1,%0\n"

        "       and             %2,%2,%1\n"
        :
        /* %0 */ "=r" (t0),
        /* %1 */ "=r" (t1),
        /* %2 */ "=r" (res)
        :
        /* %3 */ "r" (flags_res),
        /* %4 */ "2" (res)
    );

#if 0
    McdProfileEnd("McdPS2SegmentCubeIntersect");
#endif

#if (McdBOXPS2MACRODEBUG)
    if (debugDOIT(debugCounter))
    {
        MeDebug(0,"final in [%5.2f,%5.2f,%5.2f,%5.2f]",
            in[0],in[1],in[2],in[3]);
        MeDebug(0,"final out [%5.2f,%5.2f,%5.2f,%5.2f]",
            out[0],out[1],out[2],out[3]);
        MeDebug(0,"final res: 0x%08x",res);
    }
#endif

#if (McdBOXPS2MACRODEBUG)
    debugCounter++;
#endif

    return res;
}
#endif

#if (McdBOX == McdBOXSIMD)
static inline void McdSIMDAddBoxBoxClippedPoints(lsVec3 *&outList,
    const lsVec3 orig[4],const lsVec3 &disp,const lsVec3 &inR,MeReal scale)
{
    lsVec4 in = Vec4NegOne, out = Vec4One;
    lsVec4 o[3];

    Vec4x3Set_Vec3x4(o,orig);

    const int flags = McdSIMDSegmentCubeIntersect(in,out,o,disp,inR,scale);

    if (flags != 0)
    {
        const int flagsin = flags & in > Vec4NegOne;
        const int flagsout = flags & out < Vec4One;
        int i;

        for (i = 0; i < 4; i++)
        {
            if (flagsin & (1<<i))
            {
                *outList = orig[i] + in[i]*disp;
                outList++;
            }

            if (flagsout & (1<<i))
            {
                *outList = orig[i] + out[i]*disp;
                outList++;
            }
        }
    }
}
#endif

#if (McdBOX == McdBOXPS2)
static inline void McdPS2AddBoxBoxClippedPoints(lsVec3 *&outList,
    const lsVec3 orig[4],const lsVec3 &disp,
    const lsVec3 &inR,MeReal scale)
{
    lsVec4 in = Vec4NegOne,out = Vec4One;
    lsVec4 o[3];

    Vec4x3Set_Vec3x4(o,orig);

    const int flags = McdPS2SegmentCubeIntersect(in,out,o,disp,inR,scale);

    if (flags != 0)
    {
        const int flagsin = flags & in > Vec4NegOne;
        const int flagsout = flags & out < Vec4One;
        int i;

        for (i = 0; i < 4; i++)
        {
            if (flagsin & (1<<i))
            {
                *outList = orig[i] + in[i]*disp;
                outList++;
            }

            if (flagsout & (1<<i))
            {
                *outList = orig[i] + out[i]*disp;
                outList++;
            }
        }
    }
}
#endif

#if (McdBOX == McdBOXSIMD)
static inline void McdSIMDAddBoxBoxSegmentPoints(lsVec3 *&outList,
   const lsVec3 orig[4],const lsVec3 &disp,const lsVec3 &inR,MeReal scale)
{
    lsVec4 in = Vec4NegOne, out = Vec4One;
    lsVec4 o[3];
    int i;

    Vec4x3Set_Vec3x4(o,orig);

    const int flags = McdSIMDSegmentCubeIntersect(in,out,o,disp,inR,scale);

    if (flags != 0)
        for (i = 0; i < 4; i++)
            if (flags & (1<<i))
            {
                *outList = orig[i] + in[i]*disp;
                outList++;

                *outList = orig[i] + out[i]*disp;
                outList++;
            }
}
#endif

#if (McdBOX == McdBOXPS2)
static inline void McdPS2AddBoxBoxSegmentPoints(lsVec3 *&outList,
    const lsVec3 orig[4],const lsVec3 &disp,const lsVec3 &inR,MeReal scale)
{
    lsVec4 in = Vec4NegOne, out = Vec4One;
    lsVec4 o[3];
    int i;

    Vec4x3Set_Vec3x4(o,orig);

    const int flags = McdPS2SegmentCubeIntersect(in,out,o,disp,inR,scale);

    if (flags != 0)
        for (i = 0; i < 4; i++)
            if (flags & (1<<i))
            {
                *outList = orig[i] + in[i]*disp;
                outList++;

                *outList = orig[i] + out[i]*disp;
                outList++;
            }
}
#endif

#if (McdBOX == McdBOXSIMD)
static inline void McdSIMDBoxEdgeBoxIntersect(lsVec3* &outList,
    const lsTransform &tAB,const lsVec3 &inRA,const lsVec3 &inRB,MeReal scale)
{
    const lsVec3 &t = tAB.t();
    lsVec3 d0,d1,d2;
    lsVec3 invD0,invD1,invD2;
    lsVec3 x[4];

    d0 = tAB.axis(0) * inRB[0];
    d1 = tAB.axis(1) * inRB[1];
    d2 = tAB.axis(2) * inRB[2];

    invD0.setValue(SAFE_RECIP(d0[0]),SAFE_RECIP(d0[1]),SAFE_RECIP(d0[2]));
    invD1.setValue(SAFE_RECIP(d1[0]),SAFE_RECIP(d1[1]),SAFE_RECIP(d1[2]));
    invD2.setValue(SAFE_RECIP(d2[0]),SAFE_RECIP(d2[1]),SAFE_RECIP(d2[2]));

    /* x-direction edges: */

    x[0].setValue(t[0]+d1[0]+d2[0],t[1]+d1[1]+d2[1],t[2]+d1[2]+d2[2]);
    x[1].setValue(t[0]-d1[0]+d2[0],t[1]-d1[1]+d2[1],t[2]-d1[2]+d2[2]);
    x[2].setValue(t[0]+d1[0]-d2[0],t[1]+d1[1]-d2[1],t[2]+d1[2]-d2[2]);
    x[3].setValue(t[0]-d1[0]-d2[0],t[1]-d1[1]-d2[1],t[2]-d1[2]-d2[2]);

    McdSIMDAddBoxBoxClippedPoints(outList,x,d0,inRA,scale);

    /* y-direction edges: */

    x[0].setValue(t[0]+d0[0]+d2[0],t[1]+d0[1]+d2[1],t[2]+d0[2]+d2[2]);
    x[1].setValue(t[0]-d0[0]+d2[0],t[1]-d0[1]+d2[1],t[2]-d0[2]+d2[2]);
    x[2].setValue(t[0]+d0[0]-d2[0],t[1]+d0[1]-d2[1],t[2]+d0[2]-d2[2]);
    x[3].setValue(t[0]-d0[0]-d2[0],t[1]-d0[1]-d2[1],t[2]-d0[2]-d2[2]);

    McdSIMDAddBoxBoxClippedPoints(outList,x,d1,inRA,scale);

    /* z-direction edges: */

    x[0].setValue(t[0]+d0[0]+d1[0],t[1]+d0[1]+d1[1],t[2]+d0[2]+d1[2]);
    x[1].setValue(t[0]-d0[0]+d1[0],t[1]-d0[1]+d1[1],t[2]-d0[2]+d1[2]);
    x[2].setValue(t[0]+d0[0]-d1[0],t[1]+d0[1]-d1[1],t[2]+d0[2]-d1[2]);
    x[3].setValue(t[0]-d0[0]-d1[0],t[1]-d0[1]-d1[1],t[2]-d0[2]-d1[2]);

    McdSIMDAddBoxBoxSegmentPoints(outList,x,d2,inRA,scale);
}
#endif

#if (McdBOX == McdBOXPS2)
static inline void McdPS2BoxEdgeBoxIntersect(lsVec3* &outList,
    const lsTransform &tAB,const lsVec3 &inRA,const lsVec3 &inRB,MeReal scale)
{
    const lsVec3 &t = tAB.t();
    lsVec3 d0,d1,d2;
    lsVec3 invD0,invD1,invD2;
    lsVec3 x[4];

    d0 = tAB.axis(0) * inRB[0];
    d1 = tAB.axis(1) * inRB[1];
    d2 = tAB.axis(2) * inRB[2];

    invD0.setValue(SAFE_RECIP(d0[0]),SAFE_RECIP(d0[1]),SAFE_RECIP(d0[2]));
    invD1.setValue(SAFE_RECIP(d1[0]),SAFE_RECIP(d1[1]),SAFE_RECIP(d1[2]));
    invD2.setValue(SAFE_RECIP(d2[0]),SAFE_RECIP(d2[1]),SAFE_RECIP(d2[2]));

    /* x-direction edges: */

    x[0].setValue(t[0]+d1[0]+d2[0],t[1]+d1[1]+d2[1],t[2]+d1[2]+d2[2]);
    x[1].setValue(t[0]-d1[0]+d2[0],t[1]-d1[1]+d2[1],t[2]-d1[2]+d2[2]);
    x[2].setValue(t[0]+d1[0]-d2[0],t[1]+d1[1]-d2[1],t[2]+d1[2]-d2[2]);
    x[3].setValue(t[0]-d1[0]-d2[0],t[1]-d1[1]-d2[1],t[2]-d1[2]-d2[2]);

    McdPS2AddBoxBoxClippedPoints(outList,x,d0,inRA,scale);

    /* y-direction edges: */

    x[0].setValue(t[0]+d0[0]+d2[0],t[1]+d0[1]+d2[1],t[2]+d0[2]+d2[2]);
    x[1].setValue(t[0]-d0[0]+d2[0],t[1]-d0[1]+d2[1],t[2]-d0[2]+d2[2]);
    x[2].setValue(t[0]+d0[0]-d2[0],t[1]+d0[1]-d2[1],t[2]+d0[2]-d2[2]);
    x[3].setValue(t[0]-d0[0]-d2[0],t[1]-d0[1]-d2[1],t[2]-d0[2]-d2[2]);

    McdPS2AddBoxBoxClippedPoints(outList,x,d1,inRA,scale);

    /* z-direction edges: */

    x[0].setValue(t[0]+d0[0]+d1[0],t[1]+d0[1]+d1[1],t[2]+d0[2]+d1[2]);
    x[1].setValue(t[0]-d0[0]+d1[0],t[1]-d0[1]+d1[1],t[2]-d0[2]+d1[2]);
    x[2].setValue(t[0]+d0[0]-d1[0],t[1]+d0[1]-d1[1],t[2]+d0[2]-d1[2]);
    x[3].setValue(t[0]-d0[0]-d1[0],t[1]-d0[1]-d1[1],t[2]-d0[2]-d1[2]);

    McdPS2AddBoxBoxSegmentPoints(outList,x,d2,inRA,scale);
}
#endif

static bool McdVanillaOverlapOBBs(MeReal &outSep,
    lsVec3 &outN,MeReal &outPN,MeI16 &outDims,
    const MeReal inEps,const lsVec3 &inR1,
    const lsVec3 &inR2,const lsTransform &inT12)
{
    /* should be a constant, since not dependent on scale */
    const MeReal eps2 = (MeReal) 1e-6f;

    int i;
    int j;

#if 0
    bool apart = true;
#endif
    MeReal maxSeparation = -MEINFINITY;
    MeReal PN;
    MeReal saveNormD = 1.0;
    MeU8 normInfo = 0xF;
    MeReal nRLen = (MeReal)(1.0);

    // Relative center of 2nd cuboid
    const lsVec3 &pos = inT12.t();

    // Get the absolute values of the rotation matrix elements
    lsTransform arot;
    lsAbsMat(inT12,&arot);

    // Find maximum separation (early-out for positive separation)

    // Face separation (face of A):
    for (i = 0; i < 3; i++)
    {
        MeReal sumR = inR1[i] +
                      inR2[0]*arot.axis(0)[i] +
                      inR2[1]*arot.axis(1)[i] +
                      inR2[2]*arot.axis(2)[i];
        MeReal normD = pos[i];
        MeReal separation = MeFabs(normD)-sumR;

        if (separation > maxSeparation)
        {
            maxSeparation = separation;
            PN = -inR1[i]-separation;
            saveNormD = normD;
            normInfo = 0xC|i;
            if (separation > inEps)
                return false;
        }
    }

    // Face separation (face of B):
    for (i = 0; i < 3; i++)
    {
        MeReal sumR = inR2[i] +
                      inR1[0]*arot.axis(i)[0] +
                      inR1[1]*arot.axis(i)[1] +
                      inR1[2]*arot.axis(i)[2];
        MeReal normD = pos[0]*inT12.axis(i)[0] +
                       pos[1]*inT12.axis(i)[1] +
                       pos[2]*inT12.axis(i)[2];
        MeReal aNormD = MeFabs(normD);
        MeReal separation = aNormD-sumR;
        if (separation > maxSeparation)
        {
            maxSeparation = separation;
            PN = inR2[i]-aNormD;
            saveNormD = normD;
            normInfo = (i<<2)|3;
            if (separation > inEps)
                return false;
        }
    }

    // Edge separation:
    //  bool ms = maxSeparation<0;

    // face normals are generally more useful, so only return an edge-edge
    // separation if it's actually a fair bit bigger (i.e. at least > inEps).

    MeReal threshold = maxSeparation + inEps;
    for (j = 0; j < 3; j++)
    {
        const int j1 = NextMod3(j);
        const int j2 = NextMod3(j1);
        const lsVec3 &aj = arot.axis(j);
        const lsVec3 &aj1 = arot.axis(j1);
        const lsVec3 &aj2 = arot.axis(j2);
        const lsVec3 &tj = inT12.axis(j);

        for (i = 0; i < 3; i++)
        {
            MeReal rLen = (MeReal)(1.0)-tj[i]*tj[i];

            if (rLen > eps2)
            {
                const int i1 = NextMod3(i);
                const int i2 = NextMod3(i1);

                const MeReal rA = inR1[i1]*aj[i2]+inR1[i2]*aj[i1];
                const MeReal rB = inR2[j1]*aj2[i]+inR2[j2]*aj1[i];
                const MeReal normD = pos[i2]*tj[i1]-pos[i1]*tj[i2];
                const MeReal aNormD = MeFabs(normD);

                MeReal separation = (aNormD-rA-rB);

                rLen = MeSqrt(rLen);

                if (separation > inEps*rLen)
                    return false;

                if (separation>threshold*rLen)
                {
                    nRLen = (MeReal)1.0/rLen;
                    saveNormD = normD;
                    normInfo = (j<<2)|i;
                    separation *= nRLen;
                    threshold = separation;
                    maxSeparation = separation;
                    PN = (rB-aNormD)*nRLen;
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

        MCD_CHECK_ASSERT_(axis != 3, "overlapOBBs");

        MeI16 dimB = (MeI16)(arot.axis(0)[axis] < (MeReal)(1.0e-4))+
                     (MeI16)(arot.axis(1)[axis] < (MeReal)(1.0e-4))+
                     (MeI16)(arot.axis(2)[axis] < (MeReal)(1.0e-4));

        outN.setValue((MeReal)(0.0),(MeReal)(0.0),(MeReal)(0.0));
        outN[axis] = saveNormD < 0 ? (MeReal)(1.0) : (MeReal)(-1.0);
        outDims = (dimB<<8)|2;

    }
    else if ((normInfo&3) == 3)
    { // Normal is from OBBB

        MeI8 axis = (normInfo&0xC)>>2;
        MeI16 dimA = (MeI16)(arot.axis(axis)[0] < (MeReal)(1.0e-4))+
                     (MeI16)(arot.axis(axis)[1] < (MeReal)(1.0e-4))+
                     (MeI16)(arot.axis(axis)[2] < (MeReal)(1.0e-4));

        outN = (saveNormD > 0) ? -inT12.axis(axis) : inT12.axis(axis);
        outDims = (2<<8) | dimA;
    }
    else
    {
        /*  normal is from crossed edges */

        outN =(saveNormD < (MeReal)(0.0f))
            ? AxisCrossVec3(normInfo&3,inT12.axis((normInfo & 0x0C) >> 2))
            : Vec3CrossAxis(inT12.axis((normInfo & 0x0C) >> 2),normInfo & 3);

        outN *= nRLen;
        outDims = (1<<8) | 1;
    }

    return true;
}

#pragma auto_inline (off)
#pragma optimize( "", off )

static void MEAPI BoxBoxIntersect(lsVec3 *&outList,
  const lsTransform &tAB,const lsVec3 &inRA,const lsVec3 &inRB,MeReal scale)
{
#if (McdBOX == McdBOXSIMD)
    McdSIMDBoxEdgeBoxIntersect(outList,tAB,inRA,inRB,scale);
#elif (McdBOX == McdBOXPS2)
    McdPS2BoxEdgeBoxIntersect(outList,tAB,inRA,inRB,scale);
#endif

    /* inverse of tAB */
    lsTransform tBA;

    tBA.inverseOf(tAB);

    lsVec3 bVertList[24];
    {
        lsVec3 *bVert = bVertList;
        lsVec3 *vert = bVertList;

#if (McdBOX == McdBOXSIMD)
        McdSIMDBoxEdgeBoxIntersect(bVert,tBA,inRB,inRA,scale);
#endif
#if (McdBOX == McdBOXPS2)
        McdPS2BoxEdgeBoxIntersect(bVert,tBA,inRB,inRA,scale);
#endif

        while (vert != bVert)
            tAB.transform(*vert++,outList++);
    }
}

static inline bool isfar(const MeReal x, const MeReal y)
{
    return ((x-y)*(x-y)) > (MeReal) 1.0e-6f;
}

MeBool MEAPI McdBoxBoxIntersect(McdModelPair *p,McdIntersectResult *result)
{
    McdProfileStart("McdBoxBoxIntersect");

    lsTransform *tA = (lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform *tB = (lsTransform*)McdModelGetTransformPtr( p->model2 );

    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );

    McdBoxID geometry1 = (McdBoxID)McdModelGetGeometry( p->model1 );
    McdBoxID geometry2 = (McdBoxID)McdModelGetGeometry( p->model2 );

    result->touch = 0;
    result->contactCount = 0;

    lsVec3 &rA = *(lsVec3*)McdBoxGetRadii(geometry1);
    lsVec3 &rB = *(lsVec3*)McdBoxGetRadii(geometry2);

    lsTransform tAB;
    tAB.thisIsFirstThenInverseSecond(*tB,*tA);

    const McdFramework *const fwk = p->model1->frame;

    MeI16 dims;
    MeReal separation;
    lsVec3 normal;
    MeReal PN;

    /* Overlap test, return overlap values */

    if (!McdVanillaOverlapOBBs(separation,normal,PN,dims,eps,rA,rB,tAB))
    {
        McdProfileEnd("McdBoxBoxIntersect");
        return false;
    }

    lsVec3 footprint[48];
    lsVec3 *verts = footprint;

    BoxBoxIntersect(verts,tAB,rA,rB,fwk->mScale);

    const unsigned vertexCount = verts - footprint;
    MeVector3 *v = (MeVector3 *)footprint;
    McdContact *c = result->contacts;
    unsigned int i;

    result->contactCount = 0;
    MeMatrix4TMRotate(result->normal,
        (MeMatrix4Ptr) tA,(MeVector3Ptr) &normal);

    for (i = 0; i < vertexCount; i++)
    {
        MeReal s = MeVector3Dot(*v,(MeVector3Ptr) &normal)-PN;
        if (s < separation*(MeReal)(0.01f))
        {
            MeMatrix4TMTransform(c->position,(MeMatrix4Ptr) tA,*v);
            c->dims = dims;
            c->separation = s;
#ifdef MCD_CURVATURE
            c->curvature1 = 0;
            c->curvature2 = 0;
#endif
            MeVector3Copy(c->normal,result->normal);
            result->contactCount++;
            c++;
        }
        v++;
    }

    McdProfileEnd("McdBoxBoxIntersect");

    return result->touch = 1;
}

#pragma auto_inline (on)
#pragma optimize( "", on )

/*
 * In addition to linear velocity, we also take into account of angular
 * velocity.
 */
int MEAPI McdBoxBoxSafeTime( McdModelPair *p,
    MeReal maxTime, McdSafeTimeResult *result)
{
    result->pair = p;
    result->time = maxTime;

    lsVec3 *V0 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model1);
    lsVec3 *V1 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model2);

    MCD_CHECK_NULLPTR(V0, "lsVec3","McdBoxBoxSafeTime");
    MCD_CHECK_NULLPTR(V1, "lsVec3","McdBoxBoxSafeTime");

    lsVec3 *W0 = (lsVec3*)McdModelGetAngularVelocityPtr(p->model1);
    lsVec3 *W1 = (lsVec3*)McdModelGetAngularVelocityPtr(p->model2);

    MCD_CHECK_NULLPTR(W0, "lsVec3","McdBoxBoxSafeTime");
    MCD_CHECK_NULLPTR(W1, "lsVec3","McdBoxBoxSafeTime");

        /*
         * We assume that none of these ptrs are null when calling this
         * SafeTime function. Otherwise we need to make rules for
         * different combinations of these ptrs, which make life here
         * too complicated. So, it's rather caller's responsibility to
         * make sure setting up velocity parameters properly including
         * zero velocities.
         */
        if (!V0 || !V1 || !W0 || !W1) return 0;

        MeMatrix4Ptr tm0Ptr = (MeMatrix4Ptr)McdModelGetTransformPtr(p->model1);
        MeMatrix4Ptr tm1Ptr = (MeMatrix4Ptr)McdModelGetTransformPtr(p->model2);

        MCD_CHECK_NULLPTR(tm0Ptr, "MeMatrix4Ptr","McdBoxBoxSafeTime");
        MCD_CHECK_NULLPTR(tm1Ptr, "MeMatrix4Ptr","McdBoxBoxSafeTime");

        McdBoxID geometry0 = (McdBoxID)McdModelGetGeometry(p->model1);
        McdBoxID geometry1 = (McdBoxID)McdModelGetGeometry(p->model2);

        MeReal *ext0 = (MeReal*)McdBoxGetRadii(geometry0);
        MeReal *ext1 = (MeReal*)McdBoxGetRadii(geometry1);

        lsVec3 P;
        static MeReal maxAngle = (MeReal)0.2; /* radians */

        int NumSubIntervals;
        NumSubIntervals = 1 + (int)
            (
                (W0->norm() + W1->norm())
                * maxTime/maxAngle
            );

        MeReal DeltaTime = maxTime/(MeReal)NumSubIntervals;
        MeReal T;
        unsigned int ixt;
        int i, j, bIsect = 0;

        MeMatrix4 tm0i, tm1i;
        MeMatrix4Copy(tm0i, (MeVector4*)tm0Ptr);
        MeMatrix4Copy(tm1i, (MeVector4*)tm1Ptr);

        MeReal DqOverDt0[4], DqOverDt1[4];
        MeReal q0[4], q1[4];

        MeQuaternionFromTM(q0,tm0i);
        MeQuaternionFromTM(q1,tm1i);

        for (i=0; i<NumSubIntervals; i++)
        {
            ixt = MovingBoxBoxIntersect(
                ext0,(lsTransform*) tm0i,*V0,
                ext1,(lsTransform*) tm1i,*V1,
                DeltaTime, T, P
            );

            if (ixt)
            {
                result->time = DeltaTime*i + T + ME_SMALL_EPSILON;
                bIsect = 1;
                break;
            }

            if (i < (NumSubIntervals-1))
            {
#if 1
                /*
                 * Interpolate orientations using dq/dt formula
                 *
                 * This is more consistent with what is doing inside of
                 * MdtKea. So the orientation predicted here is closer to
                 * kea's advance.
                 */

                MeQuaternionWtoDQ(DqOverDt0, q0,  W0->v);
                MeQuaternionWtoDQ(DqOverDt1, q1,  W1->v);

                for (j=0; j<4; j++)
                {
                    q0[j] += DeltaTime*DqOverDt0[j];
                    q1[j] += DeltaTime*DqOverDt1[j];
                }

                /* Normalise the quaternion */
                MeReal s0 = 0.0f, s1 = 0.0f;
                for (j=0; j<4; j++)
                {
                    s0 += MeSqr(q0[j]);
                    s1 += MeSqr(q1[j]);
                }
                s0 = MeRecipSqrt(s0);
                s1 = MeRecipSqrt(s1);

                for (j=0; j<4; j++)
                {
                    q0[j] *= s0;
                    q1[j] *= s1;
                }

                MeQuaternionToTM(tm0i, q0);
                MeQuaternionToTM(tm1i, q1);

                for (j=0; j<3; j++)
                {
                    tm0i[3][j] += V0->v[j]*DeltaTime;
                    tm1i[3][j] += V1->v[j]*DeltaTime;
                }
#else
                /*
                 * Interpolate orientations using the function
                 * "MeMatrix4TMUpdateFromVelocities". This is also a
                 * correct scheme, even a more precise one. But it's not
                 * consistent with what is using inside of MdtKea. So
                 * the orientation predicted by this function does not
                 * correspond to kea's advance.
                 */

                MeReal TimeEnd = DeltaTime*(i+1);

                MeMatrix4TMUpdateFromVelocities(tm0i,
                    ME_SMALL_EPSILON, TimeEnd,
                    V0->v, W0->v, (MeVector4*)tm0Ptr);
                MeMatrix4TMUpdateFromVelocities(tm1i,
                    ME_SMALL_EPSILON, TimeEnd,
                    V1->v, W1->v, (MeVector4*)tm1Ptr);
#endif
            }
        }

        return bIsect;
}

MCD_IMPLEMENT_SAFETIME_REGISTRATION(Box,Box,1)

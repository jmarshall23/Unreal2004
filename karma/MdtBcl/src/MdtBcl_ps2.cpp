/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.5.6.1 $

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

void keaBallAndSocketJointGetInfo(ConstraintInfo &_,
    int *constraint, MdtKeaTransformation *tlist,
    MdtKeaBody *blist, MeReal stepsize)
{
    float *sprptr=(float *)0x70000000;

    __asm__ __volatile__(
        "    __expression_asm\n"
        "\n"
        "    lqc2 zero,\n"
        "\n"
        "    lqc2 body0_R0,\n"
        "    lqc2 body0_R1,\n"
        "    lqc2 body0_R2,\n"
        "    lqc2 pos1,\n"
        "\n"
        "    acc =     body0_R0*pos1.x\n"
        "    acc = acc+body0_R1*pos1.y\n"
        "    at1 = acc+body0_R2*pos1.z\n"
        "    minus_at1 = zero-at1\n"
        "\n"
        "    sw        one,row0(0)\n"
        "    sw       zero,row1(0)\n"
        "    sw       zero,row2(0)\n"
        "    sw       zero,row0(4)\n"
        "    sw        one,row1(4)\n"
        "    sw       zero,row2(4)\n"
        "    sw       zero,row0(8)\n"
        "    sw       zero,row1(8)\n"
        "    sw        one,row2(8)\n"
        "    sw       at1.z,row0(12+4)\n"
        "    sw minus_at1.y,row0(12+8)\n"
        "    sw minus_at1.z,row1(12+0)\n"
        "    sw       at1.x,row1(12+8)\n"
        "    sw       at1.y,row2(12+0)\n"
        "    sw minus_at1.x,row2(12+4)\n"
        "\n"
        "    lqc2 body1_R0,\n"
        "    lqc2 body1_R1,\n"
        "    lqc2 body1_R2,\n"
        "    lqc2 pos1,\n"
        "\n"
        "    acc =     body1_R0*pos1.x\n"
        "    acc = acc+body1_R1*pos1.y\n"
        "    at1 = acc+body1_R2*pos1.z\n"
        "    minus_at1 = zero-at1\n"
        "\n"
        "    sw  minus_one,row0(24+0)\n"
        "    sw       zero,row1(24+0)\n"
        "    sw       zero,row2(24+0)\n"
        "    sw       zero,row0(24+4)\n"
        "    sw  minus_one,row1(24+4)\n"
        "    sw       zero,row2(24+4)\n"
        "    sw       zero,row0(24+8)\n"
        "    sw       zero,row1(24+8)\n"
        "    sw  minus_one,row2(24+8)\n"
        "\n"
        "    sw minus_at1.z,row0(36+4)\n"
        "    sw       at1.y,row0(36+8)\n"
        "    sw       at1.z,row1(36+0)\n"
        "    sw minus_at1.x,row1(36+8)\n"
        "    sw minus_at1.y,row2(36+0)\n"
        "    sw       at1.x,row2(36+4)\n"
        "\n"
        "    __end_expression_asm\n");

    for (i = 0; i < 3; i++)
        _.xi[i] =
            (at1[i] + tlist[bs->body[0]].pos[i])
            - (at2[i] + tlist[bs->body[1]].pos[i]);

    for (i = 0; i < 3; i++)
    {
        _.lower[i] = -MEINFINITY;
        _.upper[i] = MEINFINITY;
    }
}

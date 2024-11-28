#ifndef __KEASSEI_H
#define __KEASSEI_H

/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.2.2.3 $

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

#include <xmmintrin.h>

const float _MM_ALIGN16 MASK_3F[4]   = {3.0f,3.0f,3.0f,3.0f};
const float _MM_ALIGN16 MASK_HALF[4] = {0.5f,0.5f,0.5f,0.5f};
const float _MM_ALIGN16 MASK_1F[4]   = {1.0f,1.0f,1.0f,1.0f};
const int   _MM_ALIGN16 MASK_MPMP[4] = {0x80000000,0x0,0x80000000,0x0};
const int   _MM_ALIGN16 MASK_MPPM[4] = {0x80000000,0x0,0x0,0x80000000};
const int   _MM_ALIGN16 MASK_MMPP[4] = {0x80000000,0x80000000,0x0,0x0};
const int   _MM_ALIGN16 MASK_PMMP[4] = {0x0,0x80000000,0x80000000,0x0};
const int   _MM_ALIGN16 MASK_ID[4][4]= { {-1,0,0,0},
                                         {0,-1,0,0},
                                         {0,0,-1,0},
                                         {0,0,0,-1} };

#define bcast(i) (_MM_SHUFFLE(i,i,i,i))
#define shuffle(i, j, k, l) ( (i<<6) + (j<<4) + (k<<2) + l)
#define broadcast(r, i) (_mm_shuffle_ps(r, r, bcast(i)))

#define shuffhilo (_MM_SHUFFLE(3,2,1,0))
#define shuffhihi (_MM_SHUFFLE(3,2,3,2))
#define shufflolo (_MM_SHUFFLE(1,0,1,0))
#define shufflohi (_MM_SHUFFLE(1,0,3,2))

/**/

__forceinline void acopy(float* d, const float* s)
{
    _mm_store_ps(&d[0], _mm_load_ps(&s[0]));
}

__forceinline __m128 spread(const float* p)
{
    __m128 x;
    x = _mm_load_ss(p);
    x = _mm_shuffle_ps(x, x, 0);
    return x;
}

__forceinline __m128 rspread(float p)
{
    __m128 x;
    x = _mm_load_ss(&p);
    x = _mm_shuffle_ps(x, x, 0);
    return x;
}

__forceinline __m128 rsum(__m128 v)
{
    v = _mm_add_ps(v, _mm_movehl_ps(v, v));
    v = _mm_add_ss(v, _mm_shuffle_ps(v, v, 1));
    return v;
}

__forceinline __m128 rsum3(__m128 v)
{
    v = _mm_add_ps(v, _mm_unpackhi_ps(v, _mm_setzero_ps()));
    v = _mm_add_ss(v, _mm_shuffle_ps(v, v, 1));
    return v;
}

/**/
#ifndef  _XBOX
__forceinline __m128 horzadd4(__m128 r1, __m128 r2, __m128 r3, __m128 r4)
{
    __m128 r5 = _mm_add_ps(_mm_movelh_ps(r1, r2), _mm_movehl_ps(r2, r1));
    __m128 r6 = _mm_add_ps(_mm_movelh_ps(r3, r4), _mm_movehl_ps(r4, r3));

    return _mm_add_ps(_mm_shuffle_ps(r5, r6, 0xDD), _mm_shuffle_ps(r5, r6, 0x88));
}

__forceinline __m128 merge4(__m128 r1, __m128 r2, __m128 r3, __m128 r4)
{
    return _mm_shuffle_ps(_mm_movelh_ps(r1, r2), _mm_movehl_ps(r4, r3), _MM_SHUFFLE(3,0,3,0));
}

#else
#define horzadd4(r1, r2, r3, r4)\
    _mm_add_ps(_mm_shuffle_ps(_mm_add_ps(_mm_movelh_ps(r1, r2), _mm_movehl_ps(r2, r1)), _mm_add_ps(_mm_movelh_ps(r3, r4), _mm_movehl_ps(r4, r3)), 0xDD), _mm_shuffle_ps(_mm_add_ps(_mm_movelh_ps(r1, r2), _mm_movehl_ps(r2, r1)), _mm_add_ps(_mm_movelh_ps(r3, r4), _mm_movehl_ps(r4, r3)), 0x88))

#define merge4(r1, r2, r3, r4)\
    _mm_shuffle_ps(_mm_movelh_ps(r1, r2), _mm_movehl_ps(r4, r3), _MM_SHUFFLE(3,0,3,0))
#endif
/**/

__forceinline __m128 abcmulp(const float* v, float s)
{
    return _mm_mul_ps(_mm_load_ps(v), spread(&s));
}

__forceinline __m128 rbcmulp(const __m128 v, float s)
{
    return _mm_mul_ps(v, spread(&s));
}

#define rbcrmulp(v, s, i) _mm_mul_ps(v, _mm_shuffle_ps(s, s, bcast(i)))

/**/

__forceinline __m128 abcaddp(const float* v, float s)
{
    return _mm_add_ps(_mm_load_ps(v), spread(&s));
}

__forceinline __m128 rbcaddp(const __m128 v, float s)
{
    return _mm_add_ps(v, spread(&s));
}
/**/

__forceinline __m128 rcp_nr_ps(const __m128 a)
{
    __m128 ra = _mm_rcp_ps(a);
    return _mm_sub_ps(_mm_add_ps(ra, ra), _mm_mul_ps(_mm_mul_ps(ra, a), ra));
}

__forceinline float rcp_nr_ss(float a)
{
    __m128 oa = _mm_load_ss(&a);
    __m128 ra = _mm_rcp_ss(oa);
     float aa;
    _mm_store_ss(&aa, _mm_sub_ss(_mm_add_ss(ra, ra), _mm_mul_ss(_mm_mul_ss(ra, oa), ra)));
    return aa;
}

/**/

__forceinline __m128 rcpsqrt_nr_ps(const __m128 a)
{
    __m128 t3  = _mm_load_ps(MASK_3F);
    __m128 th  = _mm_load_ps(MASK_HALF);
    __m128 rsa = _mm_rsqrt_ps(a);
    return _mm_mul_ps(th, _mm_mul_ps(rsa, _mm_sub_ps(t3, _mm_mul_ps(_mm_mul_ps(a, rsa), rsa))));
}

__forceinline float rcpsqrt_nr_ss(float a)
{
    __m128 t3  = _mm_set_ss(3.0f);
    __m128 th  = _mm_set_ss(0.5f);
    __m128 oa  = _mm_load_ss(&a);
    __m128 rsa = _mm_rsqrt_ss(oa);
     float aa;
    _mm_store_ss(&aa, _mm_mul_ss(th, _mm_mul_ss(rsa, _mm_sub_ss(t3, _mm_mul_ss(_mm_mul_ss(oa, rsa), rsa)))));
    return aa;
}

__forceinline __m128 rcpsqrt_nr_ss(__m128 a)
{
    __m128 t3  = _mm_set_ss(3.0f);
    __m128 th  = _mm_set_ss(0.5f);
    __m128 rsa = _mm_rsqrt_ss(a);
    return _mm_mul_ss(th, _mm_mul_ss(rsa, _mm_sub_ss(t3, _mm_mul_ss(_mm_mul_ss(a, rsa), rsa))));
}

/**/

__forceinline __m128 normalize3(__m128 v)
{
    __m128 rs = _mm_mul_ps(v, v);
    rs = _mm_add_ps(_mm_add_ps(broadcast(rs, 0), broadcast(rs, 1)), broadcast(rs, 2));
    v =_mm_mul_ps(v, rcpsqrt_nr_ps(rs));
    return v;
}

__forceinline __m128 normalize4(__m128 v)
{
    __m128 rs = _mm_mul_ps(v, v);
    rs = _mm_add_ps(rs, _mm_movehl_ps(rs, rs));
    rs = _mm_add_ss(rs, _mm_shuffle_ps(rs, rs, 1));
    v = _mm_mul_ps(v, broadcast(rcpsqrt_nr_ss(rs), 0));
    return v;
}

__forceinline void transposeBlock(float* a)
{
    __m128  z1 = _mm_load_ps(a);
    __m128  z2 = _mm_load_ps(a+4);
    __m128  z3 = _mm_load_ps(a+8);
    __m128  z4 = _mm_load_ps(a+12);

    _MM_TRANSPOSE4_PS(z1,z2,z3,z4);

    _mm_store_ps(a,   z1);
    _mm_store_ps(a+4, z2);
    _mm_store_ps(a+8, z3);
    _mm_store_ps(a+12,z4);
}
/**/

#endif //!__KEASSEI_H

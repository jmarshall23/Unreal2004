/*--------------------------------------------------------------------------------
 * Some optimized math functions for PlayStation 2.f
 --------------------------------------------------------------------------------*/
//##ifdef PS2

#ifndef __EE_MATH_INL
#define __EE_MATH_INL

#include <math.h>
#include <assert.h>

// Don't think this actually does anything at the moment.
// #include <machine/fastmath.h>

// This section adjusts for the possiblity that people are
// calling double precision functions with single precision
// arguments - using the equivalent single precision
// function is a better idea.

// Normal Trig

inline float MEEE_sin(float n)
{
  return sinf(n);
}
inline double MEEE_sin(double n)
{
  return sin(n);
}
#define sin MEEE_sin

inline float MEEE_cos(float n)
{
  return cosf(n);
}
inline double MEEE_cos(double n)
{
  return cos(n);
}
#define cos MEEE_cos

inline float MEEE_tan(float n)
{
  return tanf(n);
}
inline double MEEE_tan(double n)
{
  return tan(n);
}
#define tan MEEE_tan

// Arc-Trig!

inline float MEEE_asin(float n)
{
  return asinf(n);
}
inline double MEEE_asin(double n)
{
  return asin(n);
}
#define asin MEEE_asin

inline float MEEE_acos(float n)
{
  return acosf(n);
}
inline double MEEE_acos(double n)
{
  return acos(n);
}
#define acos MEEE_acos

inline float MEEE_atan(float n)
{
  return atanf(n);
}
inline double MEEE_atan(double n)
{
  return atan(n);
}
#define atan MEEE_atan

// Normal Trigh

inline float MEEE_sinh(float n)
{
  return sinhf(n);
}
inline double MEEE_sinh(double n)
{
  return sinh(n);
}
#define sinh MEEE_sinh

inline float MEEE_cosh(float n)
{
  return coshf(n);
}
inline double MEEE_cosh(double n)
{
  return cosh(n);
}
#define cosh MEEE_cosh

inline float MEEE_tanh(float n)
{
  return tanhf(n);
}
inline double MEEE_tanh(double n)
{
  return tanh(n);
}
#define tanh MEEE_tanh

// Arc-Trigh!

//inline float MEEE_asinh(float n)
//{
//  return asinhf(n);
//}
//inline double MEEE_asinh(double n)
//{
//  return asinh(n);
//}
//#define asinh MEEE_asinh

//inline float MEEE_acosh(float n)
//{
//  return acoshf(n);
//}
//inline double MEEE_acosh(double n)
//{
//  return acosh(n);
//}
//#define acosh MEEE_acosh
//
//inline float MEEE_atanh(float n)
//{
//  return atanhf(n);
//}
//inline double MEEE_atanh(double n)
//{
//  return atanh(n);
//}
//#define atanh MEEE_atanh

// Assorted other functions

// inline float MEEE_abs(float n)
// {
//   return absf(n);
// }
//
// inline double MEEE_abs(double n)
// {
//    return abs(n);
// }
// #define abs MEEE_abs
#define abs MEEE_absf

#ifndef PS2
inline float MEEE_sqrt(float n)
{
  return sqrtf(n);
}
/* else defined in MePrecision.h */
#endif

inline double MEEE_sqrt(double n)
{
  return sqrt(n);
}
#define sqrt MEEE_sqrt

// inline float MEEE_exp2(float n)
// {
//   return exp2f(n);
// }
// inline double MEEE_exp2(double n)
// {
//   return exp2(n);
// }
// #define exp2 MEEE_exp2

// inline float MEEE_exp10(float n)
// {
//   return exp10f(n);
// }
// inline double MEEE_exp10(double n)
// {
//   return exp10(n);
// }
// #define exp10 MEEE_exp10

// inline float MEEE_expe(float n)
// {
//   return expef(n);
// }
// inline double MEEE_expe(double n)
// {
//   return expe(n);
// }
// #define expe MEEE_expe


// inline float MEEE_log2(float n)
// {
//   return log2f(n);
// }
// inline double MEEE_log2(double n)
// {
//   return log2(n);
// }
// #define log2 MEEE_log2

// inline float MEEE_log10(float n)
// {
//   return log10f(n);
// }
// inline double MEEE_log10(double n)
// {
//   return log10(n);
// }
// #define log10 MEEE_log10

// inline float MEEE_loge(float n)
// {
//   return logef(n);
// }
// inline double MEEE_loge(double n)
// {
//   return loge(n);
// }
// #define loge MEEE_loge

// Not in the main list ...

inline float MEEE_atan2(float n, float o)
{
  return atan2f(n,o);
}
inline double MEEE_atan2(double n, double o)
{
  return atan2(n,o);
}
#define atan2 MEEE_atan2

#ifndef PS2
inline float MEEE_fabs(float n)
{
    return (float)fabs(n);
}
/* else defined in MePrecision.h */
#endif

inline double MEEE_fabs(double n)
{
    return fabs(n);
}

#define fabs MEEE_fabs

// inline float MEEE_sqrt(float x)
// {
//     float rc;
    
//     __asm__ __volatile__("
//     sqrt.s    %0,%1
//     " : "=f" (rc) : "f" (x));

//     return rc;
// }

// inline float MEEE_sqrt(float x)
// {
//   return sqrtf(x);
// }

// inline double MEEE_sqrt(double x)
// {
//     return sqrt(x);
// }

#ifdef PS2
// Single Precision Square root causes bridge demo to disintegrate ...
// Not yet tracked down. Problem exists with asm or library versions.
// Which may, in fact, be the same thing.
// #define sqrt MEEE_sqrt

/* Returns dot product of vectors a and b, each of length n
 * a and b must both be quadword aligned.
 */
inline float MEEEdot_product(float* a,float* b,int n)
{
    assert(!((unsigned int)a%0x10));
    assert(!((unsigned int)b%0x10));

    int i;
    float rc;

    // Set Accumulator to zero
    __asm__ ("vmulax.xyzw  ACC,vf0,vf0");

    // Multiply accumulate vectors
    // TODO you might find something by unrolling this loop
    for(i=0;i<n;i+=4){
        __asm__ __volatile__("
        lqc2         vf4,0x0(%0)
        lqc2         vf5,0x0(%1)
        vmadda.xyzw  ACC,vf4,vf5
        " : : "r" (a+i), "r" (b+i));
    }

    // Compress accumulator into a single value
    __asm__ __volatile__("
    vmaddx.xyzw  vf4,vf0,vf0
    vaddy.x      vf4,vf4,vf4
    vaddz.x      vf4,vf4,vf4
    vaddw.x      vf4,vf4,vf4
    qmfc2        $8,vf4
    mtc1         $8,%0
    " : "=f" (rc) : : "$8");

    return rc;
}

#endif // PS2
#endif // __EE_MATH_INL

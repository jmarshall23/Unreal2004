/*******************************************************************************
    Polynomial.h

    Utilities for working with polynomials.
*******************************************************************************/

#ifndef _Polynomial_h_
#define _Polynomial_h_

#include <MeMath.h>

// Polynomial evaluation
/*
  Input:
    x = polynomial variable.
    order = polynomial order.
    coef[order+1] = polynomial coefficients.
  Return value:
    p(x) = Sum(i = 0, order) { coef[i]*x^i }.
*/
inline MeReal
PolynomialValue(MeReal x, MeU32 order, const MeReal coef[])
{
    MeReal value = coef[order];
    while (order--)
    {
        value = value*x+coef[order];
    }
    return value;
}

// Polynomial first derivative evaluation
/*
  Input:
    x = polynomial variable.
    order = polynomial order.
    coef[order+1] = polynomial coefficients.
  Return value:
    p'(x) = Sum(i = 1, order) { i*coef[i]*x^(i-1) }.
*/
inline MeReal
PolynomialDerivative(MeReal x, MeU32 order, const MeReal coef[])
{
    MeReal value = coef[order]*(MeReal)order;
    while (--order)
    {
        value = value*x+coef[order]*(MeReal)order;
    }
    return value;
}

// Root finder
/*
  Input:
    xmin = lower bound (exclusive) of range containing roots to be found.
    xmax = upper bound (exclusive) of range containing roots to be found.
    order = polynomial order.
    coef[(order+1)*order] = polynomial coefficients + scratch for derivative coefficients.
  Output:
    roots[order] = sorted array of found roots.  # of roots returned may range from 0 to order.
  Return value:
    # of roots returned.
*/
MeU32
PolynomialRoots(MeReal roots[], const MeReal xmin, const MeReal xmax, MeU32 order, MeReal coef[]);

#endif // #ifndef _Polynomial_h_

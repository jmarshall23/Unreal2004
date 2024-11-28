/*******************************************************************************
    Polynomial.cpp

    Utilities for working with polynomials.
*******************************************************************************/

#include "Polynomial.h"

#include <McdCheck.h>

// Find a bracketed root of a polynomial by false position.
/*
  Input:
    xmin = bracket minimum.
    xmax = bracket maximum.
    f_xmin = polynomial evaluated at xmin.
    f_xmax = polynomial evaluated at xmax.
    order = polynomial order.
    coef[order+1] = polynomial coefficients
  Return value:
    x such that 0 = Sum(i = 0, order) { coef[i]*x^i }.
*/
inline MeReal
BracketedRoot(MeReal xmin, MeReal xmax, MeReal f_xmin, MeReal f_xmax, MeU32 order, const MeReal coef[])
{
    // brackets should be in correct order
    MCD_CHECK_ASSERT((xmax > xmin), kMcdErrorNum_GenericWarning,
        "brackets not ordered correctly", "BracketedRoot");

    // input function values should have opposite signs
    MCD_CHECK_ASSERT((f_xmin*f_xmax < 0), kMcdErrorNum_GenericWarning,
        "brackets invalid", "BracketedRoot");

    // JG: These should work, but for some reason with optimizations on, they fail.
#if 0
    // inputs must have exact relation to assure that the following loop will terminate
    MCD_CHECK_ASSERT((f_xmin == PolynomialValue(xmin, order, coef)), kMcdErrorNum_GenericWarning,
        "lower bracket incorrect", "BracketedRoot");
    MCD_CHECK_ASSERT((f_xmax == PolynomialValue(xmax, order, coef)), kMcdErrorNum_GenericWarning,
        "upper bracket incorrect", "BracketedRoot");
#endif

    MeI32 maxIterations;

#ifdef _ME_API_DOUBLE
    maxIterations = 52;
#else
    maxIterations = 23;
#endif

    // Seed with false position
    MeReal x = (f_xmax*xmin-f_xmin*xmax)/(f_xmax-f_xmin);

    do
    {
        const MeReal f_x = PolynomialValue(x, order, coef);
        if (f_x*f_xmin < 0)
        {
            xmax = x;
            f_xmax = f_x;
        } else
        if (f_x*f_xmax < 0)
        {
            xmin = x;
            f_xmin = f_x;
        } else
        {
            break;
        }
        // Not using Newton iteration.  Use false position instead.
        const MeReal xNew = (f_xmax*xmin-f_xmin*xmax)/(f_xmax-f_xmin);
        const MeReal delta = xNew-x;
        x = xNew;
        if (delta*delta < x*x*ME_MIN_EPSILON*ME_MIN_EPSILON)
        {
            break;
        }
    } while (--maxIterations);

    return x;
}

// Find a bracketed root of a polynomial by Newton's method and false position.
// WARNING: If the slope of the polynomial is not monatonic, Newton's method cannot be safely used here,
//          and one should use BracketedRoot (above).
/*
  Input is the same as for BrackedRoot, with the additional parameter:
    derivativeCoef[order] = polynomial derivative coefficients
  Return value is the same as for BrackedRoot.
    x such that 0 = Sum(i = 0, order) { coef[i]*x^i }.
*/
inline MeReal
BracketedRootN(MeReal xmin, MeReal xmax, MeReal f_xmin, MeReal f_xmax,
               MeU32 order, const MeReal coef[], const MeReal derivativeCoef[])
{
    // brackets should be in correct order
    MCD_CHECK_ASSERT((xmax > xmin), kMcdErrorNum_GenericWarning,
        "brackets not ordered correctly", "BracketedRoot");

    // input function values should have opposite signs
    MCD_CHECK_ASSERT((f_xmin*f_xmax < 0), kMcdErrorNum_GenericWarning,
        "brackets invalid", "BracketedRoot");

    // JG: These should work, but for some reason with optimizations on, they fail.
#if 0
    // inputs must have exact relation to assure that the following loop will terminate
    MCD_CHECK_ASSERT((f_xmin == PolynomialValue(xmin, order, coef)), kMcdErrorNum_GenericWarning,
        "lower bracket incorrect", "BracketedRoot");
    MCD_CHECK_ASSERT((f_xmax == PolynomialValue(xmax, order, coef)), kMcdErrorNum_GenericWarning,
        "upper bracket incorrect", "BracketedRoot");
#endif

    MeI32 maxIterations;

#ifdef _ME_API_DOUBLE
    maxIterations = 52;
#else
    maxIterations = 23;
#endif

    // Seed with false position
    MeReal x = (f_xmax*xmin-f_xmin*xmax)/(f_xmax-f_xmin);

    do
    {
        const MeReal f_x = PolynomialValue(x, order, coef);
        if (f_x*f_xmin < 0)
        {
            xmax = x;
            f_xmax = f_x;
        } else
        if (f_x*f_xmax < 0)
        {
            xmin = x;
            f_xmin = f_x;
        } else
        {
            break;
        }
        // Try Newton's method
        const MeReal df_dx = PolynomialValue(x, order-1, derivativeCoef);
        const MeBool posSlope = df_dx > 0;
        MeReal xNew;
        if (((x-xmin)*df_dx - f_x > 0) == posSlope && ((xmax-x)*df_dx + f_x > 0) == posSlope)
        {
            // Newton iteration keeps us within present brackets, so use it.
            xNew = x-f_x/df_dx;
        } else
        {
            // Not using Newton iteration.  Use false position instead.
            xNew = (f_xmax*xmin-f_xmin*xmax)/(f_xmax-f_xmin);
        }
        const MeReal delta = xNew-x;
        x = xNew;
        if (delta*delta < x*x*ME_MIN_EPSILON*ME_MIN_EPSILON)
        {
            break;
        }
    } while (--maxIterations);

    return x;
}

MeU32
PolynomialRoots(MeReal roots[], const MeReal xmin, const MeReal xmax, MeU32 order, MeReal coef[])
{
    // Find true order of the polynomial
    while (order && coef[order] == 0)
    {
        --order;
    }

    // Trivial case
    if (!order)
    {
        return 0;
    }

    // Fill in array of derivative coefficients (down to linear order).
    MeReal * c = coef;
    for (MeU32 d = 1; d <= order-1; ++d)
    {
        MeReal * lastC = c;
        c += order+1;
        for (MeU32 i = 0; i <= order-d; ++i)
        {
            c[i] = lastC[i+1]*(MeReal)(i+1);
        }
    }

    // Start with 1st order root evaluation
    MeU32 presentOrder = 1;
    MeU32 rootsFound = 0;
    MeReal root = -c[0]/c[1];
    if (root >= xmin && root <= xmax)
    {
        roots[rootsFound++] = root;
    }

    // Work up to full polynomial order
    while (++presentOrder <= order)
    {
        // Derivative coeficients
        MeReal *d = c;

        // Point to next order coefficients
        c -= order+1;
        
        // Evaluate polynomial at boundaries and present roots.  These become bracket points for
        // the next order of root finding.
        MeU32 newRootsFound = 0;
        MeReal x0 = xmin;
        MeReal f0 = PolynomialValue(xmin, presentOrder, c);
        for (MeU32 r = 0; r < rootsFound; ++r)
        {
            MeReal x1 = roots[r];
            MeReal f1 = PolynomialValue(x1, presentOrder, c);
          
            if (f1 == 0)
            {
                // New root lies in the same place as an old root.
                roots[newRootsFound++] = x1;
            } else
            if (f0*f1 < 0)
            {
                // f0 and f1 have opposite signs.  Find the root.
                roots[newRootsFound++] = BracketedRootN(x0, x1, f0, f1, presentOrder, c, d);
            }

            x0 = x1;
            f0 = f1;
        }
        const MeReal f_xmax = PolynomialValue(xmax, presentOrder, c);
        if (f0*f_xmax < 0)
        {
            // f0 and fmax have opposite signs.  Find the root.
            roots[newRootsFound++] = BracketedRootN(x0, xmax, f0, f_xmax, presentOrder, c, d);
        }
        
        rootsFound = newRootsFound;
    }

    return rootsFound;
}

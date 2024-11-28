/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.64.2.2 $

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

#include <math.h>

#include <MePrecision.h>
#include <MeAssert.h>
#ifdef _MECHECK
#include <MeMessage.h>
#endif

#define MePUT_FUNCTIONS_HERE_IF_NOT_INLINED 1
#include <MeMath.h>

/**
 * Calculate area of triangle given one vertex at the origin and other
 * two supplied as arguments.
 */
MeReal MEAPI
MeVector3AreaOfTriangle(const MeVector3 v1, const MeVector3 v2)
{
    return 0.5f * MeSqrt(MeVector3MagnitudeSqr(v1) * MeVector3MagnitudeSqr(v2)
             - (MeVector3Dot(v1, v2) * MeVector3Dot(v1, v2))
    );
}

/**
 * Return b in a and vice-versa.
 */
void MEAPI
MeVector3Swap(MeVector3 a, MeVector3 b)
{
    MeVector3 temp;
    MeVector3Copy(temp, a);
    MeVector3Copy(a, b);
    MeVector3Copy(b, temp);
}

/*
  Quaternion operations.
*/

/**
 * Convert rotation matrix of a transformation to a quaternion.
 */
void MEAPI
MeQuaternionFromTM(MeVector4 q, const MeMatrix4 tm)
{
    const MeReal *const t = (MeReal *) tm;
    MeReal    tr, s, qt[4];
    int       i, j, k;

    static const int nxt[3] = { 1, 2, 0 };

    tr = t[0] + t[5] + t[10];

    /* check the diagonals. */

    if (tr > (MeReal) (0.0)) {
    s = MeSqrt(tr + (MeReal) (1.0));

    /* q[0] = s / 2.0; */
    q[0] = s * (MeReal) (0.5);

    s = (MeReal) (0.5) / s;

    q[1] = (t[6] - t[9]) * s;
    q[2] = (t[8] - t[2]) * s;
    q[3] = (t[1] - t[4]) * s;
    } else {
    /* diagonal is negative */

    i = 0;

    if (t[5] > t[0])
        i = 1;

    if (t[10] > t[(i * 4) + i])
        i = 2;

    j = nxt[i];
    k = nxt[j];

    s = t[(i * 4) + i]
        - t[(j * 4) + j]
        - t[(k * 4) + k]
        + (MeReal) (1.0);

    s = MeSqrt(s);

    qt[i] = s * (MeReal) (0.5);

    if (s != (MeReal) (0.0))
        s = (MeReal) (0.5) / s;

    qt[3] = (t[(j * 4) + k] - t[(k * 4) + j]) * s;
    qt[j] = (t[(i * 4) + j] + t[(j * 4) + i]) * s;
    qt[k] = (t[(i * 4) + k] + t[(k * 4) + i]) * s;

    q[1] = qt[0];
    q[2] = qt[1];
    q[3] = qt[2];
    q[0] = qt[3];
    }
}

/**
 * Rotate a quaternion.by h|w| radians around an axis w
 */
void MEAPI
MeQuaternionFiniteRotation(MeVector4 q, const MeVector3 w, const MeReal h)
{
    int       i;
    MeVector3 tmp, nw;
    MeReal    s, c;
    MeReal    newq0;
    MeReal    wlen1 = (MeReal) 0;
    MeReal    wlen = MeSqrt(MeVector3MagnitudeSqr(w));

    if (MeFabs(wlen) < 0.0001)
    return;

    wlen1 = MeRecip(wlen);

    nw[0] = wlen1 * w[0];
    nw[1] = wlen1 * w[1];
    nw[2] = wlen1 * w[2];

    s = MeSin(wlen * h * (MeReal) (0.5));
    c = MeCos(wlen * h * (MeReal) (0.5));

    newq0 = q[0] * c - MeVector3Dot(q + 1, nw) * s;

    MeVector3Cross(tmp, nw, q + 1);

    for (i = 0; i < 3; i++)
    q[i + 1] = q[0] * s * nw[i] + c * q[i + 1] + s * tmp[i];

    q[0] = newq0;
}

/**
 * Calculates quaternion required to rotate v1 into v2; v1 and v2 MUST be
 * normalised.
 */
void MEAPI
MeQuaternionForRotation(MeVector4 q, const MeVector3 v1, const MeVector3 v2)
{
    MeVector3 rotationAxis;
    MeReal    dot = MeVector3Dot(v1, v2);
    MeReal    cosRotationAngle = 0.5f * dot;

    /* Test if v1 and v2 were parallel/antiparallel */
    if (ME_ARE_EQUAL(dot, 1) || ME_ARE_EQUAL(dot, -1))
    MeVector3MakeOrthogonal(rotationAxis, v1);
    else {
    MeVector3Cross(rotationAxis, v1, v2);
    MeVector3Normalize(rotationAxis);
    }

    MeVector3Scale(rotationAxis, MeSqrt(0.5f - cosRotationAngle));

    q[0] = MeSqrt(0.5f + cosRotationAngle);
    q[1] = rotationAxis[0];
    q[2] = rotationAxis[1];
    q[3] = rotationAxis[2];
}

/**
 * Perform spherical linear interpolation between two quaternions, between howfar=0 returns from,
 * howfar = 1 returns to.
 */
void MEAPI MeQuaternionSlerp(MeVector4 q, const MeVector4 from, const MeVector4 to, const MeReal howFar) {
    /* Remember , ME Quaternions go WXYZ! */
    MeReal angle, c, s, startWeight, endWeight;
    MeVector4 temp;

    c = MeVector4Dot(from, to);
    MeVector4Copy(temp, to);

    if ( c<0 ) {
        c = -c;
        MeVector4Scale(temp, -1);
    }

    if ( (1- c) > 0.05) {
        /* Slerp */
        angle = MeAcos(c);
        s = MeSin(angle);
        startWeight = MeSin((1 - howFar) * angle) / s;
        endWeight = MeSin(howFar * angle) / s;
    } else {
        /* Linear */
        startWeight = (MeReal)1.0 - howFar;
        endWeight = howFar;
    }

    q[1] = startWeight * from[1] + endWeight * temp[1];
    q[2] = startWeight * from[2] + endWeight * temp[2];
    q[3] = startWeight * from[3] + endWeight * temp[3];
    q[0] = startWeight * from[0] + endWeight * temp[0];
}

/*
  Matrix (usually 3x3 or 4x4) operations.
*/

/**
 * Print a matrix to a file with a given format for each element.
 *
 * \deprecated Does not conform to Me I/O abstraction. It will either be updated
 * to conform or removed in a future release
 */

void MEAPI
MeMatrixFPrint(FILE * const file, const MeReal *const A,
           const int n, const int m, const char *const format)
{
#ifndef PS2
    /* fprintf doesnt work on ps2 */

    int       i, j;
    int       width;

    /* first find the field width, hopefully its less than 100! */
    char      s[100];

    sprintf(s, format, 0);
    width = strlen(s);

    for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
        if (A[i + j * n] == 0)
        fprintf(file, "%*d", width, 0);
        else
        fprintf(file, format, A[i + j * n]);

        if (j < (m - 1))
        fputs(" ", file);
    }

    fputs("\n", file);
    }
#endif
}


/**
 * Dump a matrix to a stdio with a given format for each element.
 *
 * \deprecated For historical reasons, this function does not use the MeMessage
 * interface. It does not function on PS/2. It will either be updated to conform
 * in a future release, or removed.
 */

void MEAPI
MeMatrixPrint(const MeReal *const A, const int n, const int m, const char *const format)
{
#ifndef PS2
    /* stdout doesnt work on ps2 */
    MeMatrixFPrint(stdout, A, n, m, format);
#endif
}

/**
 * Replace the 3x3 symmetric matrix a with its inverse.
 * If a is asymmetric or singular, the returned value is zero:
 * otherwise, the returned value is 1.
 */
MeBool MEAPI
MeMatrix3SymmetricInvert(MeMatrix3 a)
{
    MeReal    r, a00, a10, a20, a11, a21, a22;

    if ((a[1][0] != a[0][1]) || (a[2][0] != a[0][2]) || (a[2][1] != a[1][2])) {
#ifdef _MECHECK
    MeWarning(12, "MeMatrix3SymmetricInvert - the input matrix is asymmetric -"
          " use MeMatrix3Invert.");
#endif
    return 0;
    }
    /*
      First, get r which is the triple product of the columns of matrix
      A.
    */
    r = a[0][0] * (a[1][1] * a[2][2] - a[2][1] * a[2][1]) +
    a[1][0] * (a[2][1] * a[2][0] - a[1][0] * a[2][2]) +
    a[2][0] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);

    if (r == 0) {
#ifdef _MECHECK
    MeWarning(12, "MeMatrix3SymmetricInvert - unable to invert a singular matrix.");
#endif
    return 0;
    }
    r = MeRecip(r);

    /*
      Now, we some components of 3 cross products and I can't seem to
      find a way to do that in place.
    */

    a00 = (a[1][1] * a[2][2] - a[2][1] * a[1][2]) * r;
    a10 = (a[2][1] * a[2][0] - a[1][0] * a[2][2]) * r;
    a20 = (a[1][0] * a[2][1] - a[1][1] * a[2][0]) * r;

    /*
      Note that b2[0] is not needed.
    */
    a11 = (a[2][2] * a[0][0] - a[2][0] * a[2][0]) * r;
    a21 = (a[2][0] * a[1][0] - a[2][1] * a[0][0]) * r;

    /*
      Note that b3[0] and b3[2] are not needed.
    */
    a22 = (a[0][0] * a[1][1] - a[1][0] * a[1][0]) * r;

    a[0][0] = a00;
    a[1][0] = a10;
    a[2][0] = a20;

    a[1][1] = a11;
    a[2][1] = a21;

    a[2][2] = a22;

    /*
      Symmetrize if needed.
    */
    a[0][1] = a[1][0];
    a[0][2] = a[2][0];
    a[1][2] = a[2][1];
    return 1;
}

/**
 * Returns true (1) if matrix a is the 3x3 identity matrix to within the given
 * error tolerance, and false (0) otherwise.
 */
MeBool MEAPI
MeMatrix3IsIdentity(const MeMatrix3 a, const MeReal tolerance)
{
    MeVector3 Test;
    unsigned int i;

    MeVectorSetZero(Test, 3);
    for (i = 0; i < 3; ++i) {
    Test[i] = (MeReal) 1;
    if (!ME_ARE_EQUAL_3VEC_TOL(Test, a[i], tolerance)) {
        return 0;
    }
    Test[i] = (MeReal) 0;
    }
    return 1;
}

/**
 * Decompose the 3x3 matrix into a lower-triangular matrix L with unit
 * trace elements and an upper-triangular matrix U, such that LU = a.
 * The returned matrices are always lower- and upper-triangular
 * respectively, but for some conditions of the input matrix a (such as
 * a[0][0] == 0) the rows of L may need to be swapped to recover the
 * input matrix.  The possible returned values of SwappedRow are:
 * SwappedRow   Rows In    Rows Out
 * 0            1 2 3      1 2 3       No swap necessary.
 * 1            1 2 3      2 1 3       Rows 1 and 2 swapped.
 * 2            1 2 3      3 2 1       Rows 1 and 3 swapped.
 * 3            1 2 3      1 3 2       Rows 2 and 3 swapped.
 * 4            1 2 3      2 3 1       Rows cycled up (2 swaps).
 * 5            1 2 3      3 1 2       Rows cycled down (2 swaps).
 *
 * The inverse of the largest element by absolute value in each row of
 * the input matrix "a" is returned in the vector scale.  If the rows
 * of "a" are swapped to perform the LU decomposition, the elements of
 * scale are correspondigly swapped.  If a row of "a" consists
 * entirely of zeros, the corresponding element of scale is zero.
 *
 * \deprecated This is an internal function. It should not be exposed and will be removed in a future release
 */

void MEAPI
MeMatrix3LUDecompose(MeMatrix3 L, MeMatrix3 U, const MeMatrix3 a,
             unsigned int *const SwappedRow, MeVector3 scale)
{
    unsigned int i = 0;
    MeReal    r = 0;
    MeMatrix3 b;

    MEASSERT(SwappedRow != 0);
    *SwappedRow = 0;

    MeMatrix3Copy(b, a);

    L[0][1] = 0;
    L[0][2] = 0;
    L[1][2] = 0;
    U[1][0] = 0;
    U[2][0] = 0;
    U[2][1] = 0;

    /* Find scale factor for each row: */
    for (i = 0; i < 3; ++i) {
    /* scale[i] = max( fabs(a[i][0]), fabs(a[i][1]), fabs(a[i][2]) ) */
    MeReal    temp = MeFabs(a[i][0]);
    scale[i] = MeFabs(a[i][1]);
    scale[i] = ((temp > scale[i]) ? temp : scale[i]);
    temp = MeFabs(a[i][2]);
    scale[i] = ((temp > scale[i]) ? temp : scale[i]);
    }
    for (i = 0; i < 3; ++i) {
    if (scale[i] != 0) {
        scale[i] = MeRecip(scale[i]);
    }
    }
    /* For larger matrices, we should perform partial pivoting (by rows) to ensure
     * stability of this algorithm.  For a 3x3 matrix, it is sufficient to ensure
     * (if possible) that the first element and upper sub-matrix are non-zero:
     */
    if (ME_IS_ZERO_TOL((a[0][0] * scale[0]), ME_MIN_EPSILON)) {
    if (!ME_IS_ZERO_TOL((a[1][0] * scale[1]), ME_MIN_EPSILON)) {
        MeVector3Swap(b[0], b[1]);
        MeRealSwap(scale, scale + 1);
        *SwappedRow = 1;
    } else if (!ME_IS_ZERO_TOL((a[2][0] * scale[2]), ME_MIN_EPSILON)) {
        MeVector3Swap(b[0], b[2]);
        MeRealSwap(scale, scale + 2);
        *SwappedRow = 2;
    }
    }

    /* If possible, make upper sub-matrix non-singular by swapping rows: */
    r = (b[0][0] * b[1][1]) - (b[0][1] * b[1][0]);
    if ((scale[0] != 0) && (scale[1] != 0) &&
    ME_IS_ZERO_TOL((r * scale[0] * scale[1]), ME_MIN_EPSILON)) {
    MeVector3Swap(b[1], b[2]);
    MeRealSwap(scale + 1, scale + 2);
    *SwappedRow += 3;
    }

    if (b[0][0] == 0) {
    r = 0;
    } else {
    r = MeRecip(b[0][0]);
    }
    L[0][0] = 1;
    U[0][0] = b[0][0];
    for (i = 1; i < 3; ++i) {
    L[i][0] = b[i][0] * r;
    L[i][i] = 1;
    U[0][i] = b[0][i];
    }
    U[1][1] = b[1][1] - (b[0][1] * L[1][0]);
    U[1][2] = b[1][2] - (b[0][2] * L[1][0]);

    if ((scale[1] == 0) || ME_IS_ZERO_TOL((U[1][1] * scale[1]), ME_MIN_EPSILON)) {
    /* Matrix a is singular, so we can choose L[2][1] to be zero. */
    L[2][1] = 0;
    } else {
    L[2][1] = (b[2][1] - (b[0][1] * L[2][0])) * MeRecip(U[1][1]);
    }
    U[2][2] = b[2][2] - (b[0][2] * L[2][0]) - (U[1][2] * L[2][1]);
}

/**
 * Swap columns col1 and col2 in the 3x3 matrix a.
 *
 * \deprecated This is an internal function, should not be exposed, and will be removed in
 * a future release
 */
void MEAPI
MeMatrix3SwapColumns(MeMatrix3 a, const unsigned int col1, const unsigned int col2)
{
    unsigned int i;
    MEASSERT(col1 < 3);
    MEASSERT(col2 < 3);
#ifdef _MECHECK
    if (col1 == col2) {
    MeWarning(12, "MeMatrix3SwapColumns - col1 and col2 are equal - no effect.");
    }
#endif

    for (i = 0; i < 3; ++i) {
    MeRealSwap(&a[i][col1], &a[i][col2]);
    }
}

/**
 * Replace the 3x3 non-singular matrix a with its inverse and return 1.
 * If a is singular, the returned value is zero: otherwise, the returned
 * value is 1.
 *
 * \deprecated This is an internal function, should not be exposed, and will be removed in
 * a future release
 */
MeBool MEAPI
MeMatrix3Invert(MeMatrix3 a)
{
    unsigned int i, swap;
    MeVector3 scale;
    MeMatrix3 L, U;
    MeMatrix3LUDecompose(L, U, a, &swap, scale);

    /* Invert the upper-triangular matrix:  */
    U[0][2] = ((U[0][1] * U[1][2]) - (U[0][2] * U[1][1]));
    for (i = 0; i < 3; ++i) {
    /* The absolute determinant of "a" is the product of the trace elements of U:    */
    if (ME_IS_ZERO_TOL((U[i][i] * scale[i]), ME_MIN_EPSILON)) {
#ifdef _MECHECK
        MeWarning(12, "MeMatrix3Invert - unable to invert a singular matrix.");
#endif
        return 0;
    }
    U[i][i] = MeRecip(U[i][i]);
    }
    U[0][1] *= (-U[0][0] * U[1][1]);
    U[1][2] *= (-U[1][1] * U[2][2]);
    U[0][2] *= (U[0][0] * U[1][1] * U[2][2]);

    /* Invert the lower-triangular matrix:  */
    L[2][0] = (L[1][0] * L[2][1]) - L[2][0];
    L[1][0] = -L[1][0];
    L[2][1] = -L[2][1];

    /* If rows of L have been swapped, the corresponding columns of 1/L must be swapped: */
    if (swap >= 3) {
    MeMatrix3SwapColumns(L, 1, 2);
    swap -= 3;
    }
    if (swap != 0) {
    MeMatrix3SwapColumns(L, 0, swap);
    }
    /* a = LU <==> 1/a = (1/U)(1/L) */
/*    MeMatrix3Multiply(a, L, U); */
    MeMatrix3MultiplyMatrix(a, U, L);
    return 1;
}


/*
  Making rotation matrices.
*/

/*
  Here we explicitly factor our the 'MeSin(a)' and 'MeCos(a)' common
  subexpressions because we don't really believe that the compiler is
  smart enough to figure out they are reducible (in the PL/1
  sense)/'const' (in the GNU C sense).
*/

/**
 * Initialize a 3x3 matrix to be a rotation matrix by a given angle
 * about the X axis.
 */
void MEAPI
MeMatrix3MakeRotationX(MeMatrix3Ptr m, const MeReal a)
{
    const MeReal s = MeSin(a);
    const MeReal c = MeCos(a);

    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;

    m[1][0] = 0;
    m[1][1] = c;
    m[1][2] = -s;

    m[2][0] = 0;
    m[2][1] = s;
    m[2][2] = c;
}

/**
 * Initialize a 3x3 matrix to be a rotation matrix by a given angle
 * about the Y axis.
 */
void MEAPI
MeMatrix3MakeRotationY(MeMatrix3 m, const MeReal a)
{
    const MeReal s = MeSin(a);
    const MeReal c = MeCos(a);

    m[0][0] = c;
    m[0][1] = 0;
    m[0][2] = -s;

    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;

    m[2][0] = s;
    m[2][1] = 0;
    m[2][2] = c;
}

/**
 * Simple trace(MeMatrix3) operation, trivial:
 * tr(A) = \Sum_k A_kk
 */
MeReal MEAPI
MeMatrix3Trace(MeMatrix3 m)
{
    return (m[0][0] + m[1][1] + m[2][2]);
}

/**
 * Initialize a 3x3 matrix to be a rotation matrix by a given angle
 * about the Z axis.
 */
void MEAPI
MeMatrix3MakeRotationZ(MeMatrix3 m, const MeReal a)
{
    const MeReal s = MeSin(a);
    const MeReal c = MeCos(a);

    m[0][0] = c;
    m[0][1] = -s;
    m[0][2] = 0;

    m[1][0] = s;
    m[1][1] = c;
    m[1][2] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
}

/**
 * This function returns true (1) if the input 3x3 matrix is both
 * orthonormal to within the given error tolerance, and positive definite.
 * The function returns false (0) otherwise.
 */
MeBool MEAPI
MeMatrix3IsValidOrientationMatrix(const MeMatrix3 rot, const MeReal tolerance)
{
    MeMatrix3 Product, rotT1;
    MeReal    determinant;

    MeMatrix3Copy(rotT1, rot);
    MeMatrix3Transpose(rotT1);
    /* this occurrence of MeMatrix3Multiply doesn't need arg swapping */
    MeMatrix3MultiplyMatrix(Product, rot, rotT1);

    if (!MeMatrix3IsIdentity(Product, tolerance))
    {
        return 0;
    }
    
    determinant = 
        rot[0][0] * (rot[1][1] * rot[2][2] - rot[2][1] * rot[1][2]) +
        rot[1][0] * (rot[2][1] * rot[0][2] - rot[0][1] * rot[2][2]) +
        rot[2][0] * (rot[0][1] * rot[1][2] - rot[1][1] * rot[0][2]);
    return (determinant > ((MeReal) 1 - tolerance));
}

/**
* initialises a 3x3 matrix to the rotation matrix equivalent to
* three rotation by angles around the x,y, and z axes (in that order)
*/
void MEAPI
MeMatrix3FromEulerAngles(MeMatrix3 m, const MeReal xangle, const MeReal yangle, const MeReal zangle)
{
    
    /* see http://www.flipcode.com/documents/matrfaq.html#Q36 for an explanation */
    
    const MeReal cx = MeCos(xangle);
    const MeReal sx = MeSin(xangle);
    const MeReal cy = MeCos(yangle);
    const MeReal sy = MeSin(yangle);
    const MeReal cz = MeCos(zangle);
    const MeReal sz = MeSin(zangle);
    
    m[0][0] = cy * cz;
    m[0][1] = -cy * sz;
    m[0][2] = -sy;
    m[1][0] = -sx * sy * cz + cx * sz;
    m[1][1] = sx * sy * sz + cx * cz;
    m[1][2] = -sx * cy;
    m[2][0] = cx * sy * cz + sx * sz;
    m[2][1] = -cx * sy * sz + sx * cz;
    m[2][2] = cx * cy;
}

/** 
 *  Create a transformation matrix by specifying 3 euler angles and
 *  a position. 
 *  @see MeMatrix3FromEulerAngles
 */
void MEAPI MeMatrix4TMFromEulerAnglesAndPosition(MeMatrix4 tm,
                                                 const MeReal xangle,
                                                 const MeReal yangle, 
                                                 const MeReal zangle,
                                                 const MeReal x,
                                                 const MeReal y,
                                                 const MeReal z)
{
    const MeReal cx = MeCos(xangle);
    const MeReal sx = MeSin(xangle);
    const MeReal cy = MeCos(yangle);
    const MeReal sy = MeSin(yangle);
    const MeReal cz = MeCos(zangle);
    const MeReal sz = MeSin(zangle);
    
    tm[0][0] = cy * cz;
    tm[0][1] = -cy * sz;
    tm[0][2] = -sy;
    tm[0][3] = 0;

    tm[1][0] = -sx * sy * cz + cx * sz;
    tm[1][1] = sx * sy * sz + cx * cz;
    tm[1][2] = -sx * cy;
    tm[1][3] = 0;

    tm[2][0] = cx * sy * cz + sx * sz;
    tm[2][1] = -cx * sy * sz + sx * cz;
    tm[2][2] = cx * cy;
    tm[2][3] = 0;

    tm[3][0] = x;
    tm[3][1] = y;
    tm[3][2] = z;
    tm[3][3] = 1;    
}

/**
 * Returns true (1) if matrix a is the 4x4 identity matrix to within the given
 * error tolerance, and false (0) otherwise.
 */
MeBool MEAPI
MeMatrix4IsIdentity(const MeMatrix4 a, const MeReal tolerance)
{
    MeVector4 Test;
    unsigned int i;

    MeVectorSetZero(Test, 4);
    for (i = 0; i < 4; ++i) {
    Test[i] = (MeReal) 1;
    if (!ME_ARE_EQUAL_4VEC_TOL(Test, a[i], tolerance)) {
        return 0;
    }
    Test[i] = (MeReal) 0;
    }
    return 1;
}

/**
 * Returns true (1) if matrix tm is a valid transformation matrix to within the given
 * error tolerance, and false (0) otherwise.  To be a valid transformation matrix,
 * the upper-left 3x3 sub-matrix must be orthonormal, with a determinant of +1, and
 * the right-most column must be [0 0 0 1].
 */
MeBool MEAPI
MeMatrix4IsTM(const MeMatrix4 tm, const MeReal tolerance)
{
    MeBool    bIsTM = ME_ARE_EQUAL_TOL(tm[3][3], 1, tolerance);
    unsigned int i = 0;

    while (bIsTM && (i < 3)) {
        bIsTM = ME_IS_ZERO_TOL(tm[i++][3], tolerance);
    }
    if (bIsTM) {
        MeMatrix3 rot;
        MeVector3Copy(rot[0], tm[0]);
        MeVector3Copy(rot[1], tm[1]);
        MeVector3Copy(rot[2], tm[2]);
        bIsTM = MeMatrix3IsValidOrientationMatrix(rot, tolerance);
    }
#ifdef _MECHECK
    if (!bIsTM) {
        MeWarning(12, "MeMatrix4IsTM - invalid transformation matrix.");
    }
#endif
    return bIsTM;
}

void MEAPI
MeMatrix4TMOrthoNormalize(MeMatrix4 tm)
{
    MeReal dot;
    MeVector3Normalize(tm[0]);
    dot = MeVector3Dot(tm[0], tm[1]);
    MeVector3MultiplyAdd(tm[1], -dot, tm[0]);
    MeVector3Normalize(tm[1]);
    MeVector3Cross(tm[2], tm[0], tm[1]);
}

/**
 * Replace the transformation matrix tm with its inverse.  Note that in release
 * mode, this routine may fail silently if tm is not a valid transformation matrix.
 */
void MEAPI
MeMatrix4TMInvert(MeMatrix4 tm)
{
    MeVector3 pos;
    MeVector3Copy(pos, tm[3]);
    /* Multiply position vector by -1 * rotation matrix: */
    tm[3][0] = -(tm[0][0] * pos[0] + tm[0][1] * pos[1] + tm[0][2] * pos[2]);
    tm[3][1] = -(tm[1][0] * pos[0] + tm[1][1] * pos[1] + tm[1][2] * pos[2]);
    tm[3][2] = -(tm[2][0] * pos[0] + tm[2][1] * pos[1] + tm[2][2] * pos[2]);
    /* Transpose (i.e. invert) the rotation matrix: */
    MeRealSwap(&tm[0][1], &tm[1][0]);
    MeRealSwap(&tm[0][2], &tm[2][0]);
    MeRealSwap(&tm[1][2], &tm[2][1]);
}

/**
 *  Create a transformation matrix corresponding to the final position
 *  after a non-accelerated motion given by an angular velocity and
 *  a linear velocity and starting at the position given by aTransform
 */

void MEAPI
MeMatrix4TMUpdateFromVelocities(MeMatrix4 aTransformReturn,
                MeReal aEpsilon,
                MeReal aTimeStep,
                const MeReal *aVelocity,
                const MeReal *aAngularVelocity, const MeMatrix4 aTransform)
{
    int       i;
    MeReal    eR[3][3], t, f, h;
    MeReal    w = aAngularVelocity[0] * aAngularVelocity[0]
    + aAngularVelocity[1] * aAngularVelocity[1]
    + aAngularVelocity[2] * aAngularVelocity[2];
    w = MeSqrt(w);

    /* If we're not really moving at all.. dont do anything. */
    if ((aTimeStep < aEpsilon) || (w < aEpsilon)) {
    MeMatrix4Copy(aTransformReturn, aTransform);
    /* velocity : pos := pos + velocity * aTimeStep */
    aTransformReturn[3][0] = aTransform[3][0] + aVelocity[0] * aTimeStep;
    aTransformReturn[3][1] = aTransform[3][1] + aVelocity[1] * aTimeStep;
    aTransformReturn[3][2] = aTransform[3][2] + aVelocity[2] * aTimeStep;
    return;
    }

    /* velocity : pos := pos + velocity * aTimeStep */
    aTransformReturn[3][0] = aTransform[3][0] + aVelocity[0] * aTimeStep;
    aTransformReturn[3][1] = aTransform[3][1] + aVelocity[1] * aTimeStep;
    aTransformReturn[3][2] = aTransform[3][2] + aVelocity[2] * aTimeStep;

    aTransformReturn[3][3] = (MeReal) 1.0;
    aTransformReturn[0][3] = (MeReal) 0.0;
    aTransformReturn[1][3] = (MeReal) 0.0;
    aTransformReturn[2][3] = (MeReal) 0.0;

    /* Matrix rotation :: R[t1] = exp[Omega * aTimeStep] * R[t0]; */
    t = w * aTimeStep;
    f = (MeReal) 1.0 / w;
    h = f * f;
    h *= ((MeReal) 1.0 - MeCos(t));
    f *= MeSin(t);

    eR[0][0] = (MeReal) 1.0 - (aAngularVelocity[1] * aAngularVelocity[1] +
                   aAngularVelocity[2] * aAngularVelocity[2]) * h;

    eR[1][1] = (MeReal) 1.0 - (aAngularVelocity[0] * aAngularVelocity[0] +
                   aAngularVelocity[2] * aAngularVelocity[2]) * h;

    eR[2][2] = (MeReal) 1.0 - (aAngularVelocity[1] * aAngularVelocity[1] +
                   aAngularVelocity[0] * aAngularVelocity[0]) * h;

    eR[0][1] = -aAngularVelocity[2] * f + aAngularVelocity[0] * aAngularVelocity[1] * h;

    eR[1][0] = aAngularVelocity[2] * f + aAngularVelocity[0] * aAngularVelocity[1] * h;

    eR[0][2] = aAngularVelocity[1] * f + aAngularVelocity[0] * aAngularVelocity[2] * h;

    eR[2][0] = -aAngularVelocity[1] * f + aAngularVelocity[0] * aAngularVelocity[2] * h;

    eR[1][2] = -aAngularVelocity[0] * f + aAngularVelocity[1] * aAngularVelocity[2] * h;

    eR[2][1] = aAngularVelocity[0] * f + aAngularVelocity[1] * aAngularVelocity[2] * h;

    for (i = 0; i < 3; i++) {
    int       j;
    for (j = 0; j < 3; j++) {
        aTransformReturn[i][j] = eR[i][0] * aTransform[0][j]
        + eR[i][1] * aTransform[1][j]
        + eR[i][2] * aTransform[2][j];
    }
    }
}

/**
 *  Create a transformation matrix corresponding to the final position after
 *  a motion given by an angular velocity and an angular acceleration and
 *  a linear velocity and ia linear acceleration starting at the position given by aTransform
 */

void MEAPI
MeMatrix4TMUpdateFromVelocitiesAndAcceler(MeMatrix4 aTransformReturn,
                      MeReal aEpsilon,
                      MeReal aTimeStep,
                      const MeVector3 aVelocity,
                      const MeVector3 aAcceler,
                      const MeVector3 aAngularVelocity,
                      const MeVector3 aAngularAcceler,
                      const MeMatrix4 aTransform)
{
    int       i;
    MeReal    eR[3][3], t, f, h;
    MeVector3 eAngular;
    MeReal    w = aAngularVelocity[0] * aAngularVelocity[0]
    + aAngularVelocity[1] * aAngularVelocity[1]
    + aAngularVelocity[2] * aAngularVelocity[2];
    MeReal    a = aAngularAcceler[0] * aAngularAcceler[0]
    + aAngularAcceler[1] * aAngularAcceler[1]
    + aAngularAcceler[2] * aAngularAcceler[2];

    w = MeSqrt(w);
    a = MeSqrt(a);
    w = w + a * aTimeStep;

    /* If we're not really moving at all.. dont do anything. */
    if ((aTimeStep < aEpsilon) || (w < aEpsilon)) {
    MeMatrix4Copy(aTransformReturn, aTransform);
    /* velocity : pos := pos + velocity * aTimeStep  + 1/2 * aAcceler* aTimeStep* aTimeStep */
    aTransformReturn[3][0] =
        aTransform[3][0] + aVelocity[0] * aTimeStep +
        (MeReal) 0.5 *aAcceler[0] * aTimeStep * aTimeStep;
    aTransformReturn[3][1] =
        aTransform[3][1] + aVelocity[1] * aTimeStep +
        (MeReal) 0.5 *aAcceler[1] * aTimeStep * aTimeStep;
    aTransformReturn[3][2] =
        aTransform[3][2] + aVelocity[2] * aTimeStep +
        (MeReal) 0.5 *aAcceler[2] * aTimeStep * aTimeStep;
    return;
    }

    /* velocity : pos := pos + velocity * aTimeStep + 1/2 * aAcceler* aTimeStep* aTimeStep */
    aTransformReturn[3][0] =
    aTransform[3][0] + aVelocity[0] * aTimeStep +
    (MeReal) 0.5 *aAcceler[0] * aTimeStep * aTimeStep;
    aTransformReturn[3][1] =
    aTransform[3][1] + aVelocity[1] * aTimeStep +
    (MeReal) 0.5 *aAcceler[1] * aTimeStep * aTimeStep;
    aTransformReturn[3][2] =
    aTransform[3][2] + aVelocity[2] * aTimeStep +
    (MeReal) 0.5 *aAcceler[2] * aTimeStep * aTimeStep;

    aTransformReturn[3][3] = (MeReal) 1.0;
    aTransformReturn[0][3] = (MeReal) 0.0;
    aTransformReturn[1][3] = (MeReal) 0.0;
    aTransformReturn[2][3] = (MeReal) 0.0;

    /* Matrix rotation :: R[t1] = exp[Omega * aTimeStep] * R[t0]; */
    t = w * aTimeStep;
    f = (MeReal) 1.0 / w;
    h = f * f;
    h *= ((MeReal) 1.0 - MeCos(t));
    f *= MeSin(t);
    eAngular[0] = aAngularVelocity[0] + aAngularAcceler[0] * aTimeStep;
    eAngular[1] = aAngularVelocity[1] + aAngularAcceler[1] * aTimeStep;
    eAngular[2] = aAngularVelocity[2] + aAngularAcceler[2] * aTimeStep;

    eR[0][0] = (MeReal) 1.0 - (eAngular[1] * eAngular[1] + eAngular[2] * eAngular[2]) * h;

    eR[1][1] = (MeReal) 1.0 - (eAngular[0] * eAngular[0] + eAngular[2] * eAngular[2]) * h;

    eR[2][2] = (MeReal) 1.0 - (eAngular[1] * eAngular[1] + eAngular[0] * eAngular[0]) * h;

    eR[0][1] = -eAngular[2] * f + eAngular[0] * eAngular[1] * h;

    eR[1][0] = eAngular[2] * f + eAngular[0] * eAngular[1] * h;

    eR[0][2] = eAngular[1] * f + eAngular[0] * eAngular[2] * h;

    eR[2][0] = -eAngular[1] * f + eAngular[0] * eAngular[2] * h;

    eR[1][2] = -eAngular[0] * f + eAngular[1] * eAngular[2] * h;

    eR[2][1] = eAngular[0] * f + eAngular[1] * eAngular[2] * h;

    for (i = 0; i < 3; i++) {
    int       j;
    for (j = 0; j < 3; j++) {
        aTransformReturn[i][j] = eR[i][0] * aTransform[0][j]
        + eR[i][1] * aTransform[1][j]
        + eR[i][2] * aTransform[2][j];
    }
    }
}

/**
 * Compute the Compound of 2 Homogenous Transforms
 * A = BoC is the composition, that it that B is the 'inner' transform
 * from global coords: A is the result of applying B then C.
 */
void MEAPI
MeMatrix4TMCompound(MeMatrix4 A, const MeMatrix4 B, const MeMatrix4 C)
{
    A[0][0] = B[0][0] * C[0][0] + B[0][1] * C[1][0] + B[0][2] * C[2][0];
    A[0][1] = B[0][0] * C[0][1] + B[0][1] * C[1][1] + B[0][2] * C[2][1];
    A[0][2] = B[0][0] * C[0][2] + B[0][1] * C[1][2] + B[0][2] * C[2][2];
    A[0][3] = 0;

    A[1][0] = B[1][0] * C[0][0] + B[1][1] * C[1][0] + B[1][2] * C[2][0];
    A[1][1] = B[1][0] * C[0][1] + B[1][1] * C[1][1] + B[1][2] * C[2][1];
    A[1][2] = B[1][0] * C[0][2] + B[1][1] * C[1][2] + B[1][2] * C[2][2];
    A[1][3] = 0;

    A[2][0] = B[2][0] * C[0][0] + B[2][1] * C[1][0] + B[2][2] * C[2][0];
    A[2][1] = B[2][0] * C[0][1] + B[2][1] * C[1][1] + B[2][2] * C[2][1];
    A[2][2] = B[2][0] * C[0][2] + B[2][1] * C[1][2] + B[2][2] * C[2][2];
    A[2][3] = 0;

    A[3][0] = B[3][0] * C[0][0] + B[3][1] * C[1][0] + B[3][2] * C[2][0] + C[3][0];
    A[3][1] = B[3][0] * C[0][1] + B[3][1] * C[1][1] + B[3][2] * C[2][1] + C[3][1];
    A[3][2] = B[3][0] * C[0][2] + B[3][1] * C[1][2] + B[3][2] * C[2][2] + C[3][2];
    A[3][3] = 1;

}

//  This is the Cyclic-by-Row Jacobi linear transformation diagonalization
//  based on the method described by Golub & van Loan implemented by 
//  Claude Lacoursiere and ported by Scott Burlington for MathEngine

// Simple programs to test a few things regarding linear algebra.
#include "Jacobi.h"
#include <math.h>

// get the eigenvalues and the rotation matrix for a positive definite 3x3
// matrix.  A is overwriten with the rotation matrix and i is the vector of
// eigenvalues.  We do this using the Jacobi iteration method which should
// be quite fast for 3x3 matrices.  We follow the 'cyclic by row' algorithm
// from Golub and Van Loan which is optimized a bit here.  The algorithm
// relies on the symmetric Schur routine which computes the sine and cosine
// of the transformation we want.

void
sym_schur2(Matrix3x3 a, int p, int q, MeReal *s, MeReal *c)
{
    *c = 1;
    *s = 0;
    if (a[p][q] != 0.0) {
	MeReal     tau = (MeReal) 0.5 * (a[q][q] - a[p][p]) / a[p][q];
	MeReal     t = 0;
	if (tau > 0) {
	    t = (MeReal) 1 / (tau + MeSqrt(1 + tau * tau));
	} else {
	    t = (MeReal) -1 / (-tau + MeSqrt(1 + tau * tau));
	}
	*c = (MeReal) 1 / MeSqrt(1 + t * t);
	*s = *c * t;
    }
}

// Macro to estimate the size of the diagonal elements of a 3x3 matrix, 
// only considering the lower diagonal ones.
#define OFFDIAG(a) \
(sqrt(a[1][0]*a[1][0] + a[2][0]*a[2][0] + a[2][1]*a[2][1]))

// update a matrix by the jacobi transform on the right.
// Here, assume that i>j and that we are working _only_ on the elements
// below the diagonal.

// function to update a matrix by multiplying on the right with a Jacobi
// transformation matrix.

void
jacobi_update_cols(Matrix3x3 a, int i, int j, MeReal c, MeReal s)
{
    MeReal     coli[3];
    MeReal     colj[3];
    coli[0] = c * a[0][i] - s * a[0][j];
    coli[1] = c * a[1][i] - s * a[1][j];
    coli[2] = c * a[2][i] - s * a[2][j];
    colj[0] = s * a[0][i] + c * a[0][j];
    colj[1] = s * a[1][i] + c * a[1][j];
    colj[2] = s * a[2][i] + c * a[2][j];
    a[0][i] = coli[0];
    a[1][i] = coli[1];
    a[2][i] = coli[2];
    a[0][j] = colj[0];
    a[1][j] = colj[1];
    a[2][j] = colj[2];
}

// same as above but multiplying with the transpose of the Jacobi transform
// from the left
void
jacobi_update_rows(Matrix3x3 a, int i, int j, MeReal c, MeReal s)
{
    MeReal     rowi[3];
    MeReal     rowj[3];
    rowi[0] = c * a[i][0] - s * a[j][0];
    rowi[1] = c * a[i][1] - s * a[j][1];
    rowi[2] = c * a[i][2] - s * a[j][2];
    rowj[0] = s * a[i][0] + c * a[j][0];
    rowj[1] = s * a[i][1] + c * a[j][1];
    rowj[2] = s * a[i][2] + c * a[j][2];
    a[i][0] = rowi[0];
    a[i][1] = rowi[1];
    a[i][2] = rowi[2];
    a[j][0] = rowj[0];
    a[j][1] = rowj[1];
    a[j][2] = rowj[2];
}

// replaces matrix a with approximate diagonal and also return rotation
// matrix r
int
jacobi3x3(Matrix3x3 a, Matrix3x3 r)
{
    int       icount = 0;
    while (OFFDIAG(a) > 0.001 && icount < 101) {
	++icount;
	for (int i = 0; i < 3; ++i) {
	    for (int j = 0; j < i; ++j) {
		MeReal     c = 1, s = 0;
		// get the sine and cosine for transform
		// NB this is parallelizable -- see Golub & van Loan
		sym_schur2(a, i, j, &s, &c);
		jacobi_update_cols(r, i, j, c, s);
		jacobi_update_cols(a, i, j, c, s);
		jacobi_update_rows(a, i, j, c, s);
	    }
	}
    }
    return icount;
}

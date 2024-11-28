// $Id: matrix.cxx,v 1.3 2001/05/01 10:47:06 dilips Exp $
// $Date: 2001/05/01 10:47:06 $
// Simple programs to test a few things regarding linear algebra.
#include <time.h>
#include "orig_matrix.h"
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//  extern "C" {
//  #include <cblas.h>
//  }

// here, matrices are row major i.e., a[i][j] = *(a+i*n + j);
// long multiply of matrices a and b, store result in c

void matmult(int ma, int nab, int nb,  Float *a, Float *b, Float*c)
{
  memset(c, 0, ma*nb*sizeof(Float));
  for(int i=0;i!=ma; ++i)
    {
      for(int j=0;j!=nb; ++j)
	{
	  for(int k=0;k!=nab; ++k) { c[i*nab + j] += a[i*nab+k]*b[k*nb+j]; }
	}
    }
}

void randmat(int m, int n, Float *a)
{
  for(int i=0;i!=m; ++i){for(int j=0;j!=n; ++j){a[i*n + j] = drand48();}}
}

void printmat(int m, int n, Float *a) {
  for(int i=0;i!=m; ++i) {
    cerr << "\t[";
    for(int j=0;j!=n; ++j) {
      printf(" %7.3f",a[i*n + j]); 
    }
    cerr<<"]"<<endl;
  }
  cerr<<endl;
//    cerr << "Printing matrix " << endl;
//    int cnt=0;
//    for(int i=0;i!=m; ++i)
//      {
//        for(int j=0;j!=n; ++j)
//  	{
//  	  cerr << "a[" << i << "][" << j << "] = " << a[i*n + j]  << "  " ; 
//  	  if((cnt++ +1)%3==0) {cerr << endl;}
//  	}
//      }
}

double dotprod(int n, Float *a, Float *b)
{
  double r=0;
  for(int i=0;i!=n;++i){r+= a[i]*b[i];}
  return r;
}

void matmult3x3(Matrix3x3 a, Matrix3x3 b, Matrix3x3 c)
{
  memset(c, 0,  9*sizeof(Float));
  for(int i=0;i!=3; ++i)
    {
      for(int j=0;j!=3; ++j) 
	{
	  for(int k=0;k!=3; ++k) {c[i][j] += a[i][k]*b[k][j];}
	}
    }
}

Float square_norm3x3(const Matrix3x3 a)
{
  Float n= 0;
  for(int i=0;i!=3;++i)
    {
      for(int j=0;j!=3;++j)
	{
	  n += a[i][j]*a[i][j];
	}
    }
  return sqrt(n);
}

void transpose3x3(Matrix3x3 at, const Matrix3x3 a)
{
  for(int i=0;i!=3;++i) {for(int j=0;j!=3;++j){ at[i][j] = a[j][i];}}
}

// replace the lower part of a with it's Cholesky factor.
// a is assumed to be symmetric positive definite
void cholesky_factor3x3(Matrix3x3 a)
{
  // do a Cholesky factor of the matrix a, ignoring the upper part
  Float s11 = 1/sqrt(a[0][0]);
  a[0][0] *= s11; 
  a[1][0] *= s11;
  a[2][0] *= s11;
  // now, update the lower 2x2 block
  a[1][1] -= a[1][0]*a[1][0];
  a[2][2] -= a[2][0]*a[2][0];
  a[2][1] -= a[2][0]*a[1][0];
  // now, factor the lower 2x2 block
  Float s22  = 1/sqrt(a[1][1]);
  a[1][1] *= s22;
  a[1][2] *= s22;
  a[2][2] -= a[1][2]*a[1][2];
  a[2][2] = sqrt(a[2][2]);
  // we are all done now: we have factorized the matrix.
}

int test_cholesky_factor3x3(Matrix3x3 a)
{
  return 0;
}

// replaces the lower part of l with its inverse.
// assumes that l is non-singular 
void invert_lower3x3(Matrix3x3 l)
{
  // Invert a lower factor in place
  l[0][0] = 1/l[0][0];
  l[1][1] = 1/l[1][1];
  l[2][2] = 1/l[2][2];
  l[1][0] = -l[0][0]*l[1][1]*l[1][0];
  l[2][0] = -l[1][0]*l[2][1]*l[2][2] - l[2][0]*l[0][0]*l[1][1];
  l[2][1] = -l[2][1]*l[1][1]*l[2][2];
}

int test_invert_lower3x3(Matrix3x3 l)
{
  return 0;
}

// replaces the lower part of l with l*l' 
void multiply_l_lt3x3(Matrix3x3 l)
{
  // Now, multiply this by it's transpose and store the lower diagonal
  // components of that in place.
  l[2][2] = l[2][2]*l[2][2]  + l[2][1]*l[2][1]  + l[2][0]*l[2][0] ;
  l[2][1] = l[2][0]*l[1][0]  + l[2][1]*l[1][1];
  l[2][0] = l[2][0]*l[0][0];
  l[1][1] = l[1][1]*l[1][1]  + l[1][0]*l[1][0];
  l[1][0] = l[0][0]*l[1][0];
  l[0][0] = l[0][0]*l[0][0];
}


// replace the matrix m with it's inverse
void MatrixInvert3x3(Matrix3x3 a)
{

  // first, get r which is the triple product of the columns of Matrix A
  Float r = 
    a[0][0]*(a[1][1]*a[2][2] - a[2][1]*a[2][1]) + 
    a[1][0]*(a[2][1]*a[2][0] - a[1][0]*a[2][2]) + 
    a[2][0]*(a[1][0]*a[2][1] - a[1][1]*a[2][0]); 
  r = 1/r;

  // now, we some components of 3 cross products and I can't seem to find a
  // way to do that in place.

  Float a00 = (a[1][1]*a[2][2] - a[2][1]*a[1][2])*r;
  Float a10 = (a[2][1]*a[2][0] - a[1][0]*a[2][2])*r;
  Float a20 = (a[1][0]*a[2][1] - a[1][1]*a[2][0])*r;

  // note that b2[0] is not needed
  //  Float a01 = (a[2][1]*a[2][0] - a[2][2]*a[1][0])*r;
  Float a11 = (a[2][2]*a[0][0] - a[2][0]*a[2][0])*r;
  Float a21 = (a[2][0]*a[1][0] - a[2][1]*a[0][0])*r;

  // note that b3[0] and b3[2] are not needed
  // Float a02 = (a[1][0]*a[2][1] - a[2][0]*a[1][1])*r;
  // Float a12 =  (a[2][0]*a[1][0] - a[0][0]*a[2][1])*r;
  Float a22 = (a[0][0]*a[1][1] - a[1][0]*a[1][0])*r;

  a[0][0] = a00;
  a[1][0] = a10;
  a[2][0] = a20;
  
  a[1][1] = a11;
  a[2][1] = a21;

  a[2][2] = a22;

  // symmetrize if needed
  a[0][1] = a[1][0];
  a[0][2] = a[2][0];
  a[1][2] = a[2][1];
  
}


int check_3x3_inverter(Matrix3x3 a, Float tol)
{
  // make copy of a
  Matrix3x3 b; 
  Matrix3x3 c; 
  int result = 0;
  memcpy(b, a, (size_t)9*sizeof(Float));
  memset(c, 0, 9*sizeof(Float));
  MatrixInvert3x3(b);
  // now, multiply b with a and check that we get identity
  for(int i=0;i!=3;++i)
    {
      for(int j=0;j!=3;++j)
	{
	  c[i][j] = a[i][0]*b[0][j] + a[i][1]*b[1][j] + a[i][2]*b[2][j] ;
	  if((i!=j) && (MeFabs(c[i][j]) > tol)) 
	    {
	      --result;
	      cerr << "Off-diagonal entry is too big: " << c[i][j] << endl;
	    }
	  else if(((i==j) && (MeFabs(c[i][j]-1)>tol)))
	    {
	      --result;
	      cerr << "Diagonal entry is not 1 " << c[i][j] << endl;
	    }
	}
    }
  
  return result;
}

// make a 3x3 pd matrix with values uniformly distributed between low and
// high 
void make3x3pd(Matrix3x3 a, Float low, Float high)
{
  Matrix3x3 c;
  Float scale = MeFabs(high-low);
  if(high<low) { Float tmp  = high; high = low; low = tmp; }
  for(int i=0;i!=3;++i) 
    { 
      for(int j=0;j!=3;++j) 
	{ 
	  c[i][j]= low + scale*drand48(); 
	} 
    }

  Matrix3x3 b; 
  transpose3x3(b, c);
  
  matmult3x3(b, c, a);
  a[0][0] += 0.001;
  a[1][1] += 0.001;
  a[2][2] += 0.001;

}

void make3x3pd(Matrix3x3 a)
{
  make3x3pd(a, 0, 1);
}


// get the eigenvalues and the rotation matrix for a positive definite 3x3
// matrix.  A is overwriten with the rotation matrix and i is the vector of
// eigenvalues.  We do this using the Jacobi iteration method which should
// be quite fast for 3x3 matrices.  We follow the 'cyclic by row' algorithm
// from Golub and Van Loan which is optimized a bit here.  The algorithm
// relies on the symmetric Schur routine which computes the sine and cosine
// of the transformation we want.

void sym_schur2(Matrix3x3 a, int p, int q, Float *s, Float *c)
{
  *c = 1; *s = 0;
  if(a[p][q]!= 0.0)  {
    Float tau = 0.5*(a[q][q] - a[p][p])/a[p][q];
    Float t = 0;
    if(tau>0) { t = 1/(tau + sqrt(1+tau*tau)); }
    else { t = -1/(-tau + sqrt(1+tau*tau)); }
    *c = 1/sqrt(1+t*t);
    *s = *c*t;
  }
}


// Here is a macro version of the above that assumes we already checked
// that a[p][q]!=0

#define SYM_SCHUR2(A, P, Q, S, C)  \
({ Float tau = 0.5*(A[Q][Q] - A[P][P])/a[P][Q]; Float t=0; \
      if(tau>0) { t = 1/(tau + sqrt(1+tau*tau)); } \
      else { t = -1/(-tau + sqrt(1+tau*tau)); } \
      C = 1/sqrt(1+t*t); S = C*t; }) 

// Macro to estimate the size of the diagonal elements of a 3x3 matrix, 
// only considering the lower diagonal ones.
#define OFFDIAG(a) \
(sqrt(a[1][0]*a[1][0] + a[2][0]*a[2][0] + a[2][1]*a[2][1]))

void test_SYM_SCHUR2()
{
  Matrix3x3  a; 
  make3x3pd(a);
  Float s, c;
  Float s1, c1;
  sym_schur2(a, 0, 1, &s1, &c1);
  sym_schur2(a, 0, 1, &s, &c);
  cerr << "MACRO s = " << s1 << "  c = " << c1 << endl;
  cerr << "Function s = " << s << "  c = " << c << endl;
  
}

// update a matrix by the jacobi transform on the right.
// Here, assume that i>j and that we are working _only_ on the elements
// below the diagonal.

#define JACOBI_UPDATE_COLS(A, I, J, C, S)\
({Float colI[3]; Float colJ[3]; \
 colI[0] = C*A[0][I] - S*A[0][J]; \
 colI[1] = C*A[1][I] - S*A[1][J]; \
 colI[2] = C*A[2][I] - S*A[2][J]; \
 colJ[0] = S*A[0][I] + C*A[0][J]; \
 colJ[1] = S*A[1][I] + C*A[1][J]; \
 colJ[2] = S*A[2][I] + C*A[2][J]; \
 A[0][I] = colI[0]; A[1][I] = colI[1]; A[2][I] = colI[2]; \
 A[0][J] = colJ[0]; A[1][J] = colJ[1]; A[2][J] = colJ[2]; })

// function to update a matrix by multiplying on the right with a Jacobi
// transformation matrix.

void jacobi_update_cols(Matrix3x3 a, int i, int j, Float c, Float s)
{

  Float coli[3]; 
  Float colj[3]; 
  coli[0] = c*a[0][i] - s*a[0][j];
  coli[1] = c*a[1][i] - s*a[1][j];
  coli[2] = c*a[2][i] - s*a[2][j];
  colj[0] = s*a[0][i] + c*a[0][j]; 
  colj[1] = s*a[1][i] + c*a[1][j];
  colj[2] = s*a[2][i] + c*a[2][j];
  a[0][i] = coli[0]; a[1][i] = coli[1]; a[2][i] = coli[2];
  a[0][j] = colj[0]; a[1][j] = colj[1]; a[2][j] = colj[2]; 
}



// same as above but multiplying with the transpose of the Jacobi transform
// from the left


#define JACOBI_UPDATE_ROWS(A, I, J, C, S)\
({ Float rowI[3]; Float rowJ[3]; \
  rowI[0] = C*a[I][0] - S*a[J][0]; \
  rowI[1] = C*a[I][1] - S*a[J][1]; \
  rowI[2] = C*a[I][2] - S*a[J][2]; \
  rowJ[0] = S*a[I][0] + C*a[J][0]; \
  rowJ[1] = S*a[I][1] + C*a[J][1]; \
  rowJ[2] = S*a[I][2] + C*a[J][2]; \
  a[I][0] = rowI[0]; a[I][1] = rowI[1]; a[I][2] = rowI[2]; \
  a[J][0] = rowJ[0]; a[J][1] = rowJ[1]; a[J][2] = rowJ[2]; })

void jacobi_update_rows(Matrix3x3 a, int i, int j, Float c, Float s)
{
  Float rowi[3];
  Float rowj[3];
  rowi[0] = c*a[i][0] - s*a[j][0];
  rowi[1] = c*a[i][1] - s*a[j][1];
  rowi[2] = c*a[i][2] - s*a[j][2];
  rowj[0] = s*a[i][0] + c*a[j][0];
  rowj[1] = s*a[i][1] + c*a[j][1];
  rowj[2] = s*a[i][2] + c*a[j][2];
  a[i][0] = rowi[0]; a[i][1] = rowi[1]; a[i][2] = rowi[2];
  a[j][0] = rowj[0]; a[j][1] = rowj[1]; a[j][2] = rowj[2]; 
}

// replaces matrix a with approximate diagonal and also return rotation
// matrix r
int jacobi3x3(Matrix3x3 a, Matrix3x3 r)
{
  int icount = 0;
  while(OFFDIAG(a) > 0.001) {
    ++icount;
    for(int i=0;i<3;++i) {
      for(int j=0;j<i;++j) {
        Float c=1, s=0;
          // get the sine and cosine for transform
          // use either the macro or the function 
          // SYM_SCHUR2(a, i, j, s, c);
        sym_schur2(a, i, j, &s, &c);
          // update the a matrix and the rotation matrix: this can
          // perhaps be optimized by calculating the formulaes by hand
          // explicitly.
          // Here is a formulation using macros
//          JACOBI_UPDATE_COLS(r, i, j, c, s);
//          JACOBI_UPDATE_COLS(a, i, j, c, s);
//          JACOBI_UPDATE_ROWS(a, i, j, c, s);
          // This uses straight function calls.
          jacobi_update_cols(r, i, j, c, s);
          jacobi_update_cols(a, i, j, c, s);
          jacobi_update_rows(a, i, j, c, s);
      }
    }
  }
  return icount;
}



int test_jacobi3x3()
{
    // first, make a pd matrix a
  Matrix3x3 a;
  make3x3pd(a, -100, 100);
    // copy matrix a
  Matrix3x3 b; 
  memcpy(b, a, (size_t)9*sizeof(Float));
  Matrix3x3 r;
  memset(r, 0, (size_t)9*sizeof(Float));
  r[0][0] = r[1][1] = r[2][2] = 1;
    // now, do the jacobi transformation. 
  int success = jacobi3x3(a, r);
  Matrix3x3 rt;
  transpose3x3(rt, r);


  Matrix3x3 c;
  matmult3x3(a, rt, c);
  Matrix3x3 d;
  matmult3x3(r, c, d);
  for(int i=0;i!=3;++i) for(int j=0;j!=3;++j) d[i][j] -= b[i][j]; 
  Float norm = square_norm3x3(d);
  if(norm>0.0001)
  {
    success = 0;
    cerr << "error in test_jacobi3x3: " 
         << "final R*A*R^t not equal to original " << endl;
  }
  if(a[0][0]<0 || a[1][1] < 0 || a[2][2] < 0 )
  {
    success = 0;
    cerr << "error in test_jacobi3x3: " 
         << " eigenvalues of matrix A are not positive " << endl;
  }
  return  success; 
}

void test_jacobi_sample(int sample)
{
  int good=0;
  int iterations=0;
  for(int i=0;i!=sample;++i)
  {
    int count = test_jacobi3x3();
    iterations += count;
    if(count) ++good;
  }
  Float ratio = Float(good)/Float(sample);
  Float avi = Float(iterations)/Float(sample);
  cerr << "Solved  " << 100*ratio  << " percent of the " << sample 
       << " problems " << endl
       << "Average iterations per problem is : " << avi << endl;
}

void sample_3x3inverter()
{
  Matrix3x3 a;
  int passcount=0;
  int sample = 10000;
  for(int i=0;i!=sample;++i) {
    make3x3pd(a);
    if( check_3x3_inverter(a, 0.01) ==0 ) { ++passcount; }
  }
  cerr << "Pass fraction  " << (double)passcount/(double)sample 
       << "  of " << sample << " problems " <<endl;
}

#ifdef WITH_MAIN
void main(void) {
  srand48(time(0));
  test_jacobi_sample(10000);

  Matrix3x3 eig={
	{ 157278.188, -182271.844, -865894.625},
	{ -182271.844, 211237.484, 1003497.500},
	{ -865894.625, 1003497.500, 4767182.000}
};

  Matrix3x3 eigval={{1,0,0},{0,1,0},{0,0,1}};
  Float *eig_p=(Float*)eig, *eigval_p=(Float*)eigval;
  
  cerr << "Transform in standard basis (with rotation):" << endl;
  printmat(3,3,eig_p);
  jacobi3x3(eig,eigval);
  cerr << "EigenValues [diagonal] & EigenVectors:" << endl;
  printmat(3,3,eig_p);
  printmat(3,3,(Float*)eigval);
}
#endif // With Main Test program

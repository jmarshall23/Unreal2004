// Simple program to test tetrahedral composites
#include <time.h>
//#include "orig_matrix.h"
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include "lsVec3.h"
#include <MeMath.h>

struct MassP {
  MeReal vol;
  lsVec3 com;
  MeMatrix3 MM; // must be scaled by vol for Icm
  MassP& operator=(const MassP& I);
};

MassP& MassP::operator=(const MassP& I){
  vol = I.vol;
  com = I.com;
  MeMatrix3Copy( MM, I.MM);
  return *this;
}  

ostream& operator << (ostream& os, const lsVec3& v) {
  os <<" ("<<v.v[0]<<","<<v.v[1]<<","<<v.v[2]<<")";
  return os;
}

ostream& operator << (ostream& os, const MeMatrix3& v) {
  os <<" ("<<v[0][0]<<","<<v[0][1]<<","<<v[0][2]<<")"<<endl;
  os <<" ("<<v[1][0]<<","<<v[1][1]<<","<<v[1][2]<<")"<<endl;
  os <<" ("<<v[2][0]<<","<<v[2][1]<<","<<v[2][2]<<")";
  return os;
}

void printmat(int m, int n, MeMatrix3& a){
  for(int i=0;i!=m; ++i) {
    cerr << "\t{";
    for(int j=0;j!=n; ++j) {
      printf(" %7.4g,",a[i][j]); 
    }
    cerr<<"}"<<endl;
  }
  cerr<<endl;
}

MeReal trace(const MeMatrix3& m) {
  return m[0][0] + m[1][1] + m[2][2];
}

/** 
 * Calculates Xt.X*I - X.Xt from a 3x1 translation vector
 * Returning: a 3x3 basis xform matrix
 */
void BasisTrans(lsVec3 translate, MeMatrix3& Result){
  MeMatrix3 I1,IP,Itmp,OP;
  MeReal Dot;
  Dot = MeVector3Dot((float*)&translate, (float*)&translate);
  MeMatrix3MakeIdentity(IP);
  MeMatrix3Scale(IP, Dot); // Xt.X*I
  MeVector3OuterProduct( OP, (float*)&translate,  (float*)&translate); // X.Xt
//    cout<<"Xt.X*I"<<endl; //<< C <<endl; printmat(3,3,I);
//    cout<<"X.Xt"<<endl; //<< C <<endl; printmat(3,3,OP);
  MeMatrix3Subtract(Result, IP, OP); // Xt.X*I - X.Xt [M3x3]
//    cout<<"Xt.X*I - X.Xt"<<endl; //<< C <<endl; printmat(3,3,Result);
}

/**
 * Generates tetrahedral mass properties for a triangle in 3-space
 * by making a tetrahedra from the origin and the triagle
 * transforming that to the standard tetrahedron and calculating there.
 * Returns: MassP with Inertia matrix unscaled by volume!
 */
MassP TetraMassProp( lsVec3 x, lsVec3 y, lsVec3 z) {
  lsVec3 u(0.25,0.25,0.25),a,b,c;
  MassP Result;

  MeMatrix3 Basis;
  MeMatrix3CopyVec( Basis, (float*)&x,  (float*)&y,  (float*)&z); MeMatrix3 BasisTmp;
  MeMatrix3Copy(BasisTmp, Basis);
  MeMatrix3Transpose(BasisTmp);
  MeMatrix3 U={{  0.0375, -0.0125, -0.0125},
	       { -0.0125,  0.0375, -0.0125},
	       { -0.0125, -0.0125,  0.0375}};
  MeMatrix3 Id,A,B,C;
  MeMatrix3MakeIdentity(Id);

  cout <<"T:"<<endl<< Basis << endl;
  MeReal volume = x.dot(y.cross(z)) / 6.0;
  Result.vol = volume;
  printf("Volume: %6.2f\n", volume);
  lsVec3 com = 0.25 * (x + y + z);
  Result.com = com;
  cout <<"Center: " << com << endl;

//  MeMatrix3Multiply(A, Basis, U); // TU
  MeMatrix3MultiplyMatrix(A, U, Basis); // TU
//  MeMatrix3Multiply(B, A, BasisTmp); // TUTt
  MeMatrix3MultiplyMatrix(B, BasisTmp, A); // TUTt
  MeMatrix3Scale(Id, trace(B)); // tr(TUTt)*I
  MeMatrix3Subtract(C, Id, B); // tr(TUTt,  Id)*I - TUTt
  MeMatrix3Copy(Result.MM, C);
  MeMatrix3Copy(Basis, C); // for printing the Icm

  BasisTrans(com, Id);// Xt.X*I - X.Xt
  MeMatrix3Add( A, C, Id);
//    cout<<"Returning:\nIorig: Xt.X*I - X.Xt + tr(TUTt)*I - TUTt Not Volume Scaled"<<endl; printmat(3,3,A);
  MeMatrix3Copy( Result.MM, A);

  MeMatrix3Scale(Basis,volume);
//    cout<<"Icm: tr(TUTt)*I - TUTt * Volume"<<endl;  printmat(3,3,Basis);
  return Result;
}

/**
 * This is an inertial tensor transform _from_ the CM to the 
 * new coordinate system, weighted by mass. Atypical of the
 * Xforms in the literature that usually Xforms from the coords _to_
 * the CM.
 * Ix = Ix' + M(x'^2 + y'^2) Inertial Axes
 * Ixy = Ixy' + M(x' * y')   Inertial Products
 */
void Matrix3TranslateIM(const MeMatrix3 IM, const lsVec3& TX, 
			const MeReal Mass, MeMatrix3 Inertia) {
    Inertia[0][0] = IM[0][0] + (Mass * (TX.v[1] * TX.v[1] + TX.v[2] * TX.v[2]));/*  Intertial Axes */
    Inertia[1][1] = IM[1][1] + (Mass * (TX.v[0] * TX.v[0] + TX.v[2] * TX.v[2]));
    Inertia[2][2] = IM[2][2] + (Mass * (TX.v[0] * TX.v[0] + TX.v[1] * TX.v[1]));

    Inertia[0][1] = IM[0][1] + (Mass * (TX.v[0] * TX.v[1]));/*  Inertial Products */
    Inertia[0][2] = IM[0][2] + (Mass * (TX.v[0] * TX.v[2]));
    Inertia[1][2] = IM[1][2] + (Mass * (TX.v[1] * TX.v[2]));

    Inertia[1][0] = Inertia[0][1];/*  Symmetry */
    Inertia[2][0] = Inertia[0][2];
    Inertia[2][1] = Inertia[1][2];
}    

/**
 *  Combine two inertia tensors and relative centers of mass for a
 *  new combined rigid body.  This will be extended to Arrays versus pairs.
 *  X - CM vector
 *  I - Inertial Tensor
 *  M - Mass
 */
void AccumulateIT(MassP* dataTetra, int numTetra, MassP& Result) {
  lsVec3 cm1, cm2, CM;
  MeMatrix3 It1, It2, Iorig;
    // Mass is easy
  Result.vol = 0.0;
  Result.com.setValue(0.0,0.0,0.0);

  for (int i=0; i < numTetra; i++) {
    Result.vol += dataTetra[i].vol;
    Result.com += dataTetra[i].vol * dataTetra[i].com;
  }

  if(Result.vol > 1e-5)
    Result.com = (1/Result.vol) * Result.com; // Xcm = Sum(MiXi)/Sum(Mi) simple weighted average
  else cout << "Error: Bad Mass inputs to AccumulateIT"<<endl;

  for(int i=0; i<3; i++) for(int j=0; j<3; j++) Result.MM[i][j] = 0.0;

  for(int i=0; i < numTetra; i++) {
    MeMatrix3Copy( It1, dataTetra[i].MM);
    MeMatrix3Scale(It1, dataTetra[i].vol);
    MeMatrix3Add(Result.MM, Result.MM, It1); //Sum Mi*Iorig_i
  }
  
  BasisTrans(Result.com, It2);
  MeMatrix3Scale(It2, Result.vol);
  MeMatrix3Subtract(Result.MM, Result.MM, It2);// Icm = Iorig - XtXI-X.Xt
}

#ifdef WITH_MAIN
void main(void) {
  lsVec3 x(5,0,0),y(0,6,0),z(4,5,2); //volInt tetrab
  lsVec3 a(5,0,0),b(4,5,2),c(0,0,4); //volInt tetrad
  lsVec3 r(4,5,2),s(0,6,0),t(0,0,4); //volInt tetrat
  lsVec3 Sq[12][3];
  Sq[0][0].setValue(5,5,5),Sq[0][1].setValue(15,15,5),Sq[0][2].setValue(15,5,5),
    Sq[1][0].setValue(5,5,5),Sq[1][1].setValue(5,15,5),Sq[1][2].setValue(15,15,5),
    Sq[2][0].setValue(5,5,5),Sq[2][1].setValue(5,5,15),Sq[2][2].setValue(5,15,15),
    Sq[3][0].setValue(5,5,5),Sq[3][1].setValue(5,15,15),Sq[3][2].setValue(5,15,5),
    Sq[4][0].setValue(5,5,5),Sq[4][1].setValue(15,5,5),Sq[4][2].setValue(15,5,15),
    Sq[5][0].setValue(5,5,5),Sq[5][1].setValue(15,5,15),Sq[5][2].setValue(5,5,15),
    Sq[6][2].setValue(5,15,5),Sq[6][0].setValue(15,15,15),Sq[6][1].setValue(15,15,5),
    Sq[7][2].setValue(5,15,5),Sq[7][0].setValue(5,15,15),Sq[7][1].setValue(15,15,15),
    Sq[8][0].setValue(15,5,5),Sq[8][1].setValue(15,15,5),Sq[8][2].setValue(15,15,15),
    Sq[9][0].setValue(15,5,5),Sq[9][1].setValue(15,15,15),Sq[9][2].setValue(15,5,15),
    Sq[10][0].setValue(5,5,15),Sq[10][1].setValue(15,5,15),Sq[10][2].setValue(15,15,15),
    Sq[11][0].setValue(5,5,15),Sq[11][1].setValue(15,15,15),Sq[11][2].setValue(5,15,15);
  
  MassP T[12], tetraColl;
  lsVec3 com;
  MeMatrix3 I, tmp;
  MeReal M;

  T[0] = TetraMassProp(x,y,z);
  T[1] = TetraMassProp(a,b,c);
  T[2] = TetraMassProp(r,s,t);

  AccumulateIT(T, 3, tetraColl);
  printf("Volume: %6.2f\n", tetraColl.vol);
  cout <<"Center: " << tetraColl.com << endl;
  cout<<"AccIM:"<<endl; //<< C <<endl;
  printmat(3,3,tetraColl.MM);

  for (int i=0; i<12; i++) 
    T[i] = TetraMassProp(Sq[i][0],Sq[i][1],Sq[i][2]);

  cout << "For the whole collection" << endl;

  AccumulateIT(T,12,tetraColl);
  printf("Volume: %6.2f\n", tetraColl.vol);
  cout <<"Center: " << tetraColl.com << endl;
  cout<<"AccIM:"<<endl; //<< C <<endl;
  printmat(3,3,tetraColl.MM);

}

#endif // With Main Test program

/**
 *  Combine two inertia tensors and relative centers of mass for a
 *  new combined rigid body.  This will be extended to Arrays versus pairs.
 *  X - CM vector
 *  I - Inertial Tensor
 *  M - Mass
 */
void AddIT(lsVec3 X_1, MeMatrix3 I_1, MeReal M_1,
	   lsVec3 X_2, MeMatrix3 I_2, MeReal M_2,
	   lsVec3 *X, MeMatrix3 *I, MeReal *M) {
  lsVec3 cm1, cm2, CM;
  MeMatrix3 It1, It2, Iorig;
  MeMatrix3& Icm = It1;
    // Mass is easy
  *M = M_1 + M_2;

    // Position is a linear weighted blend
  if(*M > 0)
    *X = X_1 + ((X_2-X_1)*(M_2)/(*M)); // divide by zero!!
    // Xcm = Sum(MiXi)/Sum(Mi) simple weighted average
  else cout << "Error: Bad Mass inputs to AccumulateIT"<<endl;

// This is for moving Iorigs to the new common!but it's not a tetrahedra!
  MeMatrix3Copy( It1, I_1);
  MeMatrix3Copy( It2, I_2);
  MeMatrix3Scale(It1, M_1);
  MeMatrix3Scale(It2, M_2);
  MeMatrix3Add(Iorig, It1, It2); //Sum Mi*Iorig_i
  
  BasisTrans(*X, Icm);
  MeMatrix3Scale(Icm, *M);
  MeMatrix3Subtract(*I, Iorig, Icm);// Icm = Iorig - XtXI-X.Xt
}


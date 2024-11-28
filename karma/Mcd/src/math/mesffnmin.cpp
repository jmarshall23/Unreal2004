/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.10.4.1 $

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

#include "mesffnmin.h"

static const int ITMAX = 100;
static const MeReal EPS = (MeReal)1.0e-10;
static const MeReal GOLD = (MeReal)(1.6180339887498948482);
static const MeReal GLIMIT = (MeReal)(100.0);
static const MeReal TINY = (MeReal)1.0e-20;
static const MeReal ZEPS = (MeReal)1.0e-10;

static MeReal pcom[kMaxD];
static MeReal xicom[kMaxD];
static int Ncom = 0;
static MeReal (*nrfunc)(MeReal * const) = 0;
static void (*nrdfun)(MeReal *, MeReal * const) = 0;

#define SHIFT(_a,_b,_c,_d)      (_a) = (_b); (_b) = (_c); (_c) = (_d);
#define MOVE3(_a,_b,_c,_d,_e,_f)  (_a) = (_d); (_b) = (_e); (_c) = (_f);

inline MeReal fmax(const MeReal x, const MeReal y) {
  return x>y?x:y;
}

inline MeReal fmin(const MeReal x, const MeReal y) {
  return x<y?x:y;
}

inline MeReal GiveSign(const MeReal x, const MeReal y) {
  return (y >= 0.0) ? MeFabs(x) : -MeFabs(x);
}

inline void CopyNd(const int N, MeReal *x, MeReal * const y) {
  MeReal *b = y;
  const MeReal *e = x+N;
  while (x < e) { *x++ = *b++; }
}

inline void NegNd(const int N, MeReal *x) {
  const MeReal *e = x+N;
  while (x < e) { const MeReal t = -(*x); *x++ = t; }
}

inline void NegNd(const int N, MeReal *x, MeReal * const y) {
  MeReal *b = y;
  const MeReal *e = x+N;
  while (x < e) { *x++ = -(*b++); }
}

inline void AddNd(const int N, MeReal *x, MeReal * const y) {
  MeReal *b = y;
  const MeReal *e = x+N;
  while (x < e) { *x++ += *b++; }
}

inline void AddNd(const int N, MeReal *x, MeReal * const y, MeReal * const z) {
  MeReal *b = y;
  MeReal *c = z;
  const MeReal *e = x+N;
  while (x < e) { *x++ = (*b++)+(*c++); }
}

inline void SubNd(const int N, MeReal *x, MeReal * const y) {
  MeReal *b = y;
  const MeReal *e = x+N;
  while (x < e) { *x++ -= *b++; }
}

inline void MulNd(const int N, MeReal *x, const MeReal k) {
  const MeReal *e = x+N;
  while (x < e) { *x++ *= k; }
}

inline void MulNd(const int N, MeReal *x, MeReal * const y, const MeReal k) {
  MeReal *b = y;
  const MeReal *e = x+N;
  while (x < e) { *x++ = k*(*b++); }
}

inline void MulAddNd(const int N, MeReal *x, MeReal * const y, const MeReal k, MeReal * const z) {
  MeReal *b = y;
  MeReal *c = z;
  const MeReal *e = x+N;
  while (x < e) { *x++ = k*(*b++)+(*c++); }
}

inline MeReal DotNd(const int N, MeReal * const x, MeReal * const y) {
  MeReal *a = x;
  MeReal *b = y;
  const MeReal *e = x+N;
  MeReal dot = 0.0;
  while (a < e) { dot += (*a++)*(*b++); }
  return dot;
}

inline MeReal Norm2Nd(const int N, MeReal * const x) {
  MeReal *a = x;
  const MeReal *e = x+N;
  MeReal norm2 = 0.0;
  while (a < e) { const MeReal t = *a++; norm2 += t*t; }
  return norm2;
}

MeReal f1d(const MeReal x) {
  MeReal temp[kMaxD];
  MulNd(Ncom,temp,xicom,x);
  AddNd(Ncom,temp,pcom);
  return nrfunc(temp);
}

MeReal df1d(const MeReal x) {
  MeReal temp[kMaxD];
  MulNd(Ncom,temp,xicom,x);
  AddNd(Ncom,temp,pcom);
  MeReal temp2[kMaxD];
  nrdfun(temp2,temp);
  return DotNd(Ncom,temp2,xicom);
}

static char NRCopyright[] =
"Second Edition C: 'Copyright (C) 1987-1992 Numerical Recipes Software'";

void mnbrak(MeReal &ax, MeReal &bx, MeReal &cx, MeReal &fa, MeReal &fb, MeReal &fc,
            MeReal (*f)(const MeReal)) {
  fa = f(ax);
  fb = f(bx);

  if (fb > fa) {
    MeReal dum;
    SHIFT(dum,ax,bx,dum);
    SHIFT(dum,fb,fa,dum);
  }

  cx = bx+GOLD*(bx-ax);
  fc = f(cx);

  while (fb > fc) {
    MeReal r = (bx-ax)*(fb-fc);
    MeReal q = (bx-cx)*(fb-fa);
    MeReal u = bx-((bx-cx)*q-(bx-ax)*r)/((MeReal)(2.0)*GiveSign(::fmax(MeFabs(q-r),TINY),q-r));
    MeReal ulim = bx+GLIMIT*(cx-bx);
    MeReal fu;
    if ((bx-u)*(u-cx) > (MeReal)(0.0)) {
      fu = f(u);
      if (fu < fc) {
        ax = bx;
        bx = u;
        fa = fb;
        fb = fu;
        return;
      } else if (fu > fb) {
        cx = u;
        fc = fu;
        return;
      }
      u = cx+GOLD*(cx-bx);
      fu = f(u);
    } else if ((cx-u)*(u-ulim) > (MeReal)(0.0)) {
      fu = f(u);
      if (fu < fc) {
        SHIFT(bx,cx,u,cx+GOLD*(cx-bx));
        SHIFT(fb,fc,fu,f(u));
      }
    } else if ((u-ulim)*(ulim-cx) >= (MeReal)(0.0)) {
      u = ulim;
      fu = f(u);
    } else {
      u = cx+GOLD*(cx-bx);
      fu = f(u);
    }
    SHIFT(ax,bx,cx,u);
    SHIFT(fa,fb,fc,fu);
  }
}

MeReal dbrent(MeReal &xmin, MeReal ax, MeReal bx, MeReal cx,
              MeReal (*f)(const MeReal), MeReal (*df)(const MeReal), MeReal tol) {
  MeReal a = ::fmin(ax,cx);
  MeReal b = ::fmax(ax,cx);
  MeReal v = bx;
  MeReal w = v;
  MeReal x = w;
  MeReal fx = f(x);
  MeReal fv = fx;
  MeReal fw = fv;
  MeReal dx = df(x);
  MeReal dv = dx;
  MeReal dw = dv;

  MeReal d = (MeReal)(0.0);
  MeReal e = (MeReal)(0.0);

  for (int iter = ITMAX; iter-- > 0;) {
    MeReal u;
    MeReal fu;
    MeReal xm = (MeReal)(0.5)*(a+b);
    MeReal tol1 = tol*(MeFabs(x)+ZEPS);
    MeReal tol2 = (MeReal)(2.0)*tol1;
    if (MeFabs(x-xm) <= (tol2-(MeReal)(0.5)*(b-a))) {
      xmin = x;
      return fx;
    }
    if (MeFabs(e) > tol1) {
      MeReal d1 = (MeReal)(2.0)*(b-a);
      MeReal d2 = d1;
      if (dw != dx) {
        d1 = (w-x)*dx/(dx-dw);
      }
      if (dv != dx) {
        d2 = (v-x)*dx/(dx-dv);
      }
      MeReal u1 = x+d1;
      MeReal u2 = x+d2;
      int ok1 = (a-u1)*(u1-b) > (MeReal)(0.0) && dx*d1 < (MeReal)(0.0);
      int ok2 = (a-u2)*(u2-b) > (MeReal)(0.0) && dx*d2 < (MeReal)(0.0);
      MeReal olde = e;
      e = d;
      if (ok1 || ok2) {
        if (ok1 && ok2) {
          d = (MeFabs(d1) < MeFabs(d2)) ? d1 : d2;
        } else if (ok1) {
          d = d1;
        } else {
          d = d2;
        }
        if (MeFabs(d) <= MeFabs((MeReal)(0.5)*olde)) {
          u = x+d;
          if (u-a < tol2 || b-u < tol2) {
            d = GiveSign(tol1,xm-x);
          }
        } else {
          d = (MeReal)(0.5)*(e = ((dx > (MeReal)(0.0)) ? a-x : b-x));
        }
      } else {
        d = (MeReal)(0.5)*(e = ((dx > (MeReal)(0.0)) ? a-x : b-x));
      }
    } else {
      d = (MeReal)(0.5)*(e = ((dx > (MeReal)(0.0)) ? a-x : b-x));
    }
    if (MeFabs(d) >= tol1) {
      u = x+d;
      fu = f(u);
    } else {
      u = x+GiveSign(tol1,d);
      fu = f(u);
      if (fu > fx) {
        xmin = x;
        return fx;
      }
    }
    MeReal du = df(u);
    if (fu < fx) {
      if (u >= x) {
        a = x;
      } else {
        b = x;
      }
      MOVE3(v,fv,dv,w,fw,dw);
      MOVE3(w,fw,dw,x,fx,dx);
      MOVE3(x,fx,dx,u,fu,du);
    } else {
      if (u < x) {
        a = u;
      } else {
        b = u;
      }
      if (fu < fw || w == x) {
        MOVE3(v,fv,dv,w,fw,dw);
        MOVE3(w,fw,dw,u,fu,du);
      } else if (fu < fv || v == x || v == w) {
        MOVE3(v,fv,dv,u,fu,du);
      }
    }
  }

  // Too many iterations!
  return f(xmin);
}

MeReal LineFnMinNd(const int N, MeReal *p, MeReal *xi, const MeReal tol,
                   MeReal (*f)(MeReal * const),
                   void (*grad)(MeReal *, MeReal * const)) {
  nrfunc = f;
  nrdfun = grad;
  Ncom = N;
  CopyNd(N,pcom,p);
  CopyNd(N,xicom,xi);

  MeReal ax = (MeReal)(0.0);
  MeReal xx = (MeReal)(1.0);

  MeReal bx,fa,fx,fb;
  mnbrak(ax,xx,bx,fa,fx,fb,f1d);

  MeReal xmin;
  MeReal fRet = dbrent(xmin,ax,xx,bx,f1d,df1d,tol);
  MulAddNd(N,p,xi,xmin,p);

  return fRet;
}

MeReal FnMin1d(MeReal &p, const MeReal fTol,
               MeReal (*f)(const MeReal), MeReal (*df)(const MeReal)) {
  MeReal ax = (MeReal)(0.0);
  MeReal xx = (MeReal)(1.0);

  MeReal bx,fa,fx,fb;
  mnbrak(ax,xx,bx,fa,fx,fb,f);

  return dbrent(p,ax,xx,bx,f,df,fTol);
}

MeReal FnMinNd(const int N, MeReal *p, const MeReal tol, const MeReal fTol,
               MeReal (*f)(MeReal * const), void (*grad)(MeReal *, MeReal * const)) {
  MeReal fp = f(p);
  MeReal xi[kMaxD];
  grad(xi,p);
  NegNd(N,xi);
  MeReal g[kMaxD];
  CopyNd(N,g,xi);
  MeReal h[kMaxD];
  CopyNd(N,h,xi);

  MeReal fMin = fp;

  for (int its = ITMAX; its-- > 0;) {
    fMin = LineFnMinNd(N,p,xi,tol,f,grad);
    if ((MeReal)(2.0)*MeFabs(fMin-fp) <= fTol*(MeFabs(fMin)+MeFabs(fp)+EPS)) {
      return fMin;
    }
    MeReal gg = Norm2Nd(N,g);
    if (gg == (MeReal)(0.0)) {
      return fMin;
    }
    fp = f(p);
    grad(xi,p);
    MeReal temp[kMaxD];
    AddNd(N,temp,xi,g);
    MeReal dgg = DotNd(N,temp,xi);
    NegNd(N,g,xi);
    MulAddNd(N,h,h,dgg/gg,g);
    CopyNd(N,xi,h);
  }

  // Too many iterations!
  return fMin;
}

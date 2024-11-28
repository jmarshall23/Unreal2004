#include <McdFrame.h>
#include <McdPrimitives.h>
#include <stdlib.h>
#include <stdio.h>

McdModelID
bgModelCreate()
{
  McdModelID m; 
  m = McdModelCreate( McdSphereCreate(1) );
  MeMatrix4 mat;
  MeMatrix4MakeIdentityTM(mat);
  McdModelSetTransformPtr( m, mat );
  return m;
}

McdModelPair*
bgPairCreate()
{
  McdModelID m1;
  McdModelID m2;
  McdModelPair *p = (McdModelPair*)malloc( sizeof( McdModelPair ) );
  McdSphereID s = McdSphereCreate(1);
  m1 = McdModelCreate( s );
  m2 = McdModelCreate( s );
  McdModelPairInit(p,m1,m2);
  return p;
}

void
bgPairDestroy( McdModelPair* p)
{
  McdModelID m1 = p->model1;
  McdModelID m2 = p->model2;
  free(p);
  McdSphereID s = McdModelGetGeometry(m1);
  McdModelDestroy(m1);
  McdModelDestroy(m2);
  McdGeometryDestroy(s);
}

McdModelPairContainer* 
bgPairContainerCreate( int size, 
		       int numGoodbye, int numHello, int numStay,
		       McdModelPair *dummyElement )
{
  McdModelPairContainer *c
    = McdModelPairContainerCreate( size );

  c->goodbyeCount = numGoodbye;
  c->helloFirstIndex = c->helloFirstIndex - numHello;
  c->stayingLessThanIndex = c->stayingFirstIndex + numStay;

  int i;

  for( i = 0 ; i < c->goodbyeCount ; ++i )
    {
      c->array[i] = dummyElement;
    }
  for( i = c->helloFirstIndex ; i < c->stayingLessThanIndex ; ++i )
    {
      c->array[i] = dummyElement;
    }

  return c;
}

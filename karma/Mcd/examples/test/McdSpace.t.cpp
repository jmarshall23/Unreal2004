#include <MeMath.h>
#include <MeMemory.h>
#include "McduMemoryInit.h"
#include <McdFrame.h>
#include <McdPrimitives.h>
#include <stdlib.h>
#include <stdio.h>

McdModelID
bgModelCreate()
{
  McdModelID m; 
  m = McdModelCreate( McdSphereCreate(1) );
  MeMatrix4Ptr mat = (MeMatrix4Ptr)malloc(sizeof(MeMatrix4));
  MeMatrix4MakeIdentityTM(mat);
  McdModelSetTransformPtr( m, mat );
  return m;
}

int main()
{
  McduMemoryInit();
  McdInit(3, 100);
  McdSphereBoxPlaneRegisterTypes();

  McdModelID m1 = bgModelCreate();
  McdModelID m2 = bgModelCreate();
  McdModelID m3 = bgModelCreate();

  McdSpaceID space = McdSpaceAxisSortCreate(McdAllAxes,50,100,1);

  McdSpaceInsertModel( space, m1 );
  McdSpaceInsertModel( space, m2 );
  McdSpaceInsertModel( space, m3 );

  McdSpaceBuild( space );
  McdSpaceUpdateAll( space );
  int pairArraySize;
  int overflow;
  McdModelPairContainer pairs;
  McdSpacePairIterator iter;

  pairArraySize = 4;
  overflow = 1;

  McdModelPair **array 
    = (McdModelPair**)alloca( pairArraySize * sizeof( McdModelPair* ) );

  McdModelPairContainerInit( &pairs, array, pairArraySize );

  McdSpacePairIteratorBegin( space,&iter );

#ifdef MCDCHECK
  printf("handleContacts\n\n");
#endif

  while( overflow )
    {
      McdModelPairContainerReset( &pairs );
      overflow = McdSpaceGetPairs( space,&iter,&pairs );

#ifdef MCDCHECK
      McdModelPairContainerPrintStats( &pairs );
#endif
    }
}

  

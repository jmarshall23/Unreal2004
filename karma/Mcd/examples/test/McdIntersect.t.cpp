#include "McdTest.h"

void
processPairs( McdModelPairContainer* pairs,
	      int resultArraySize,
	      int contactArraySize)
{
  printf("processPairs() parameters: \n");
  printf("\t resultArraySize: %d \n", resultArraySize );
  printf("\t contactArraySize: %d \n", contactArraySize );

  int i;
  int resultCount;
  int contactCount;
  McdModelPairContainerIterator pairIter;
  MeBool overflow;

  McdIntersectResult *results;
  McdContact *contacts;

  McdGoodbyeEach( pairs );
  McdHelloEach( pairs );

  results = (McdIntersectResult*)alloca( resultArraySize *
					 sizeof ( McdIntersectResult ) );

  contacts = (McdContact*)alloca( contactArraySize *
				  sizeof ( McdContact ) );

  McdModelPairContainerIteratorInit( &pairIter, pairs );  

  overflow = 1;
  while( overflow )
    {
      overflow = McdIntersectEach( pairs, &pairIter,
				   results, resultArraySize,
				   &resultCount,
				   contacts, contactArraySize,
				   &contactCount );

      printf("one McdIntersectEach() batch completed \n");
      printf("\t num results computed: %d \n",resultCount );
      printf("\t total num contacts involved: %d \n",contactCount );

      for( i = 0 ; i < resultCount ; ++i )
	{
	  printf("\t\t processing a result \n");
	}
    }

}

void
testSpace( pairs )
{
  McdGeometryID g;
  McdModelID m1,m2,m3;
  g = McdSphereCreate(1);
  m1 = McdModelCreate(g);
  m2 = McdModelCreate(g);
  m3 = McdModelCreate(g);
  m4 = McdModelCreate(g);
  m5 = McdModelCreate(g);
}

int main()
{
  int containerSize = 10;
  int numBye = 2;
  int numHello = 3;
  int numStay = 4;
  McdModelPair *p;
  McdModelPairContainer *pairs ;

  McdInit(McdPrimitivesGetTypeCount(), 100);
  McdPrimitivesRegisterTypes();
  McdPrimitivesRegisterInteractions();

  p = bgPairCreate(); 
  pairs = bgPairContainerCreate( containerSize, 
				 numBye,numHello,numStay,
				 p); 

#ifdef MCDCHECK
  printf("container after creation: \n");
  McdModelPairContainerPrintStats( pairs );
#endif
    
  int resultArraySize = 3;
  int contactArraySize = 50;
  processPairs( pairs, resultArraySize, contactArraySize );

  testSpace( pairs );
}

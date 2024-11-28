
#ifndef _IxTriangleMeshPrimitives_h_
#define _IxTriangleMeshPrimitives_h_

bool      TriangleMeshTriangleMeshFn(McdIntersectResult *info,
				     McdModelID model1, McdModelID model2,
				     const MeReal eps, void *cachedData);

void      McdTriangleMeshTriangleMeshRegisterInteraction();
/*
bool TriangleMeshSphereFn(McdIntersectResult *info,
		  McdModelID model1, McdModelID model2,
		  const MeReal eps, void *cachedData);

bool TriangleMeshBoxFn(McdIntersectResult *info,
		  McdModelID model1, McdModelID model2,
		  const MeReal eps, void *cachedData);
 
void McdTriangleMeshBoxRegisterInteraction();

void McdTriangleMeshSphereRegisterInteraction();
*/

#endif

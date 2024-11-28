#include <stdio.h>
#include <McdSphere.h>

#include "IxTriangleMeshPrimitive.h"
#include "TriUtils.h"

MCD_PRIMITIVEINTERSECT_REGISTER(TriangleMesh,Box)


bool 
McdTriangleMeshSphereIntersect(McdModelPair *p,
                               McdIntersectResult *info)
{
  return true;
}

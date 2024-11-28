#include <McdConvexMesh.h>
#include <McdFrame.h>
#include <MeMemory.h>

extern "C"
MeBool MEAPI
McdConvexMeshPrimitivesRegisterInteractions(McdFramework *frame)
{
    McdConvexMeshConvexMeshRegisterInteraction(frame);
    McdBoxConvexMeshRegisterInteraction(frame);
    McdSphereConvexMeshRegisterInteraction(frame);
    McdCylinderConvexMeshRegisterInteraction(frame);
    McdConvexMeshTriangleListRegisterInteraction(frame);
    McdConvexMeshPlaneRegisterInteraction(frame);
    McdConvexMeshLineSegmentRegisterInteraction(frame);
    McdSphylConvexMeshRegisterInteraction(frame);
    return 1;
}


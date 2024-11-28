
#include <stdio.h>
#include <stdlib.h>
#include <qhull.h>
#include <McdQHullTypes.h>
//#include <cnvHull2.h>
#include <MeMemory.h>

MeVector3 points[] = 
{
    { 1, 1, 0 }, 
    {-1, 1, 0 }, 
    { 1,-1, 0 }, 
    { 0, 0, 1 }, 
    {-1,-1, 0 }, 
    { 2, 0, 0 }, 
    {-2, 0, 0 }, 
};
int num_points = 7;

void* MEAPI new_calloc(size_t size) { return calloc(1, size); }
void* MEAPI new_malloc(size_t size) { return malloc(size); }
void* MEAPI new_realloc(void * const p, size_t size) { return realloc(p, size); }
void MEAPI new_free(void * const p) { free(p); }

//  Memory api calls the usual C stuff
MeMemoryAPIStruct MeMemoryAPI = 
{
    new_malloc, new_calloc, 0, new_free, 0, new_realloc
};


/****************************************************************************
  This is a test program for McdComputeHull
*/
void main(int argc, const char **argv)
{
    McdConvexHull cnv;
    int ok;

    ok = McdComputeHullSizes(&cnv, num_points, points);

    McdAllocateHull(&cnv);

    ok = McdGetHullData(&cnv);

    printf("result is %d\n",ok);
}



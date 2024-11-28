/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.7.2.3 $

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

#include <RConvex.h>
#include <RGeometryUtils.h>


/**
 * Create a convex hull graphic from an McdConvexMesh collision geometry.
 */
RGraphic* MEAPI RGraphicConvexCreate(RRender*const rc,
    const McdConvexMeshID conv,const float color[4], const MeMatrix4Ptr matrix)
{
    int i,j,k,l, triCount = 0;
    RGraphic *rg;

    int numStrips = McdConvexMeshGetPolygonCount(conv);
    int* stripStart = (int*)MeMemoryALLOCA(numStrips*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(numStrips*sizeof(int));
    RObjectVertex* strips;

    int vertexCount = 0;
    stripStart[0] = 0;

    /* first, count up number of vertices and triangles in each strip. */
    for (i=0; i<numStrips; i++)
    {
        stripSize[i] = McdConvexMeshGetPolygonVertexCount(conv, i);

        if(i+1 < numStrips)
            stripStart[i+1] = stripStart[i] + stripSize[i];

        vertexCount += stripSize[i];
        triCount += McdConvexMeshGetPolygonVertexCount(conv, i) - 2;
    }

    /* Create temporary strip storage. */
    strips = (RObjectVertex*)MeMemoryALLOCA(vertexCount*sizeof(RObjectVertex));

    /* Create grphic with required number of triangles. */
    rg = RGraphicCreateEmpty(triCount * 3);
    if ( !rg )
        return 0;

    /* Convert each triangle in turn into a strip. */
    for (i=0; i<numStrips; i++)
    {
        MeVector3 normal, vertex;
        McdConvexMeshGetPolygonNormal(conv, i, normal);

        j = 0; /* first vertex */
        k = McdConvexMeshGetPolygonVertexCount(conv, i) - 1; /* last vertex */
        l = 0; /* position in strip */

        while(j <= k)
        {
            /* TODO: Texture co-ords for convex meshes */

            McdConvexMeshGetPolygonVertex(conv, i, j, vertex);
            RSetVertex(&strips[stripStart[i]+l], vertex, normal, 0, 0);
            l++;

            if(j != k)
            {
                McdConvexMeshGetPolygonVertex(conv, i, k, vertex);
                RSetVertex(&strips[stripStart[i]+l], vertex, normal, 0, 0);
                l++;
            }

            j++; k--;
        }
    }

    /* Finally convert tri-strips to tri-list */
    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    rg->m_pObject->m_nTextureID = -1; /* no texture */
    RGraphicSetColor(rg,color);
    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);
    RGraphicAddToList(rc, rg, 0);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);

    return rg;
}

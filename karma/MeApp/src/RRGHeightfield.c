/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.7.6.3 $

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

#include <RRGHeightfield.h>
#include <RGeometryUtils.h>

static void CalculateVertex(int i, int j, int numX, int numY,
                     MeReal originX, MeReal originY,
                     MeReal incrementX, MeReal incrementY,
                     MeReal* heights, MeVector3 vertex)
{
    /* ensure co-ords are in range. */
    if(i < 0) i = 0;
    if(j < 0) j = 0;
    if(i >= numY) i = numY - 1;
    if(j >= numX) j = numX - 1;

    vertex[0] = originX + (j * incrementX);
    vertex[1] = originY + (i * incrementY);
    vertex[2] = heights[(i * numX) + j];
}

static void CalculateNormal(int i, int j, int numX, int numY,
                     MeReal originX, MeReal originY,
                     MeReal incrementX, MeReal incrementY,
                     MeReal* heights, MeVector3 normal)
{
    MeVector3 v0, v1, v2;
    MeVector3 e0, e1;

    CalculateVertex(i,   j,   numX, numY, originX, originY, incrementX, incrementY, heights, v0);
    CalculateVertex(i,   j+1, numX, numY, originX, originY, incrementX, incrementY, heights, v1);
    CalculateVertex(i+1, j,   numX, numY, originX, originY, incrementX, incrementY, heights, v2);

    MeVector3Subtract(e0, v1, v0);
    MeVector3Subtract(e1, v2, v0);

    MeVector3Cross(normal, e1, e0);
    MeVector3Normalize(normal);
}

/**
 * Create a heightfield graphic.
 */
RGraphic* MEAPI RGraphicRGHeightfieldCreate(RRender* rc, int numX, int numY,
                                            MeReal incrementX, MeReal incrementY,
                                            MeReal originX, MeReal originY,
                                            MeReal* heights,
                                            float color[4], MeMatrix4Ptr matrix)
{
    int i,j,triCount;
    RGraphic *rg;

    int numStrips = numY - 1;
    int stripTiles = numX - 1;
    int verticesPerStrip = 2 * (stripTiles+1);

    int* stripStart = (int*)MeMemoryALLOCA(numStrips*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(numStrips*sizeof(int));
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA(numStrips *
        verticesPerStrip * sizeof(RObjectVertex));

    triCount = (numStrips * stripTiles * 2);

    stripStart[0] = 0;
    for (i=0; i<numStrips; i++)
    {
        stripSize[i] = verticesPerStrip;

        if(i+1 < numStrips)
            stripStart[i+1] = stripStart[i] + stripSize[i];
    }

    /* Create graphic with required number of triangles. */
    rg = RGraphicCreateEmpty(triCount * 3);
    if ( !rg )
        return 0;

    /* Convert each triangle in turn into a strip. */
    for (i=0; i<numStrips; i++) /* y */
    {
        for(j=0; j<stripTiles+1; j++) /* x */
        {
            MeVector3 normal, vertex;
            MeReal u = (MeReal)j/numX;
            MeReal v = (MeReal)i/numY;

            CalculateVertex(i, j, numX, numY, originX, originY, incrementX, incrementY, heights, vertex);
            CalculateNormal(i, j, numX, numY, originX, originY, incrementX, incrementY, heights, normal);
            RSetVertex(&strips[stripStart[i]+(j*2)], vertex, normal, u, v);

            CalculateVertex(i+1, j, numX, numY, originX, originY, incrementX, incrementY, heights, vertex);
            CalculateNormal(i+1, j, numX, numY, originX, originY, incrementX, incrementY, heights, normal);
            v = (MeReal)(i+1)/numY;
            RSetVertex(&strips[stripStart[i]+(j*2)+1], vertex, normal, u, v);
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

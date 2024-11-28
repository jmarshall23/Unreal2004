/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

    $Date: 2002/04/19 18:30:20 $ $Revision: 1.21.2.4 $

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

#include <ctype.h>

#include <RGeometryUtils.h>
#include <MeMemory.h>
#include <MeMath.h>

/**

   Converts a set of triangle strips to a list of triangles. Useful for
   backends that do not support triangle strips, converting convex hulls and
   meshes to triangle lists, ...

   @param rg Ignored at present.
   @param strips A pointer to the first vertex of the first strip to be
   converted. All vertices for each strip follow in one contiguous chunk.
   @param stripSize An array whose ith element contains the number of
   vertices in the ith strip.
   @param stripStart An array whose ith element contains the index in
   `strips' of the first vertex of th ith strip.
   @param numStrips The number of strips to process.

 */
void MEAPI RConvertTriStripToTriList(RGraphic* rg, RObjectVertex* strips, int* stripSize, int* stripStart, int numStrips)
{
    int i, j, k, tri = 0; /* Triangles so far */

    /* For each strip.... */
    for(i=0; i<numStrips; i++)
    {
        /* Now convert strip 'i' into triangles. */
        for(j=stripStart[i]; j<(stripStart[i]+stripSize[i]-2); j++)
        {
            for(k=0; k<3; k++)
                rg->m_pVertices[(tri*3)+k] = strips[j+k];

            tri++;
        }
    }
}

void MEAPI RSetVertex(RObjectVertex* vertex, MeVector3 vert, MeVector3 norm, MeReal u, MeReal v)
{
    vertex->m_X = vert[0];
    vertex->m_Y = vert[1];
    vertex->m_Z = vert[2];
    vertex->m_NX = norm[0];
    vertex->m_NY = norm[1];
    vertex->m_NZ = norm[2];
    vertex->m_U = u;
    vertex->m_V = v;
}

/********************** TORUS ***********************/
int RCalculateTorusVertexCount(int sides, int rings)
{
    /* calculate (number of triangles) * 3 */
    return (rings * sides * 2) * 3;
}

void RCalculateTorusGeometry(RGraphic* rg, AcmeReal innerRadius,
                             AcmeReal outerRadius, int sides, int rings)
{
    /* first allocate triangles strips. */
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA(2*(sides+1)*(rings)*sizeof(RObjectVertex));
    int* stripStart = (int*)MeMemoryALLOCA(rings*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(rings*sizeof(int));
    int numStrips = rings;

    MeReal torusThickness = (outerRadius-innerRadius)*(MeReal)0.5;
    MeReal torusRadius = innerRadius+torusThickness;
    MeVector3* vertex = (MeVector3*)MeMemoryALLOCA((rings+1)*sizeof(MeVector3));
    MeVector3* normal = (MeVector3*)MeMemoryALLOCA((rings+1)*sizeof(MeVector3));
    MeReal* vcoord = (MeReal*)MeMemoryALLOCA((rings+1)*sizeof(MeReal));
    int i, j;
    int stripLength = 2*(sides+1);

    for(i=0,j=0; i<numStrips; i++,j+=stripLength)
    {
        stripStart[i] = j;
        stripSize[i] = stripLength;
    }

    /**** MAKE Z-AXIS TORUS AS STRIPS ****/

    /* First, make one 'ring' */
    for(i=0; i<rings+1; i++)
    {
        MeReal angle = ((MeReal)i/rings) * 2 * ME_PI;

        normal[i][0] = 0;
        normal[i][1] = MeSin(angle);
        normal[i][2] = MeCos(angle);

        MeVector3Copy(vertex[i], normal[i]);
        MeVector3Scale(vertex[i], torusThickness);
        vertex[i][1] += torusRadius;

        vcoord[i] = ((MeReal)i/rings);
    }

    /* Then transform that ring to each angle. */
    for(j=0; j<sides+1; j++)
    {
        MeMatrix3 R;
        MeReal aroundAngle = ((MeReal)j/sides) * 2 * ME_PI;
        MeMatrix3MakeRotationZ(R, aroundAngle);

        for(i=0; i<rings; i++)
        {
            MeVector3 tmp;

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i]);
            strips[stripStart[i]+(j*2)].m_X = tmp[0];
            strips[stripStart[i]+(j*2)].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i]);
            strips[stripStart[i]+(j*2)].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)].m_NZ = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i+1]);
            strips[stripStart[i]+(j*2)+1].m_X = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i+1]);
            strips[stripStart[i]+(j*2)+1].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_NZ = tmp[2];

            strips[stripStart[i]+(j*2)].m_U = ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)].m_V = vcoord[i];
            strips[stripStart[i]+(j*2)+1].m_U = ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)+1].m_V = vcoord[i+1];
        }
    }

    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);
    MeMemoryFREEA(vertex);
    MeMemoryFREEA(normal);
    MeMemoryFREEA(vcoord);
}

/********************** FRUSTUM ***********************/
int RCalculateFrustumVertexCount(int sides)
{
    /* (number of triangles)*3 */
    int sideTriCount = 2*sides;
    int endTriCount = sides-2;

    return (sideTriCount + 2*endTriCount)*3;
}
void RCalculateFrustumGeometry(RGraphic* rg, AcmeReal bottomRadius,
                               AcmeReal topRadius, AcmeReal bottom,
                               AcmeReal top, int sides)
{
    /* first allocate triangles strips. */
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA((((sides + 1)*2) + ((sides+1)*2))*sizeof(RObjectVertex));
    int* stripStart = (int*)MeMemoryALLOCA(3*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(3*sizeof(int));
    int numStrips = 3;

    RObjectVertex* topPoly = (RObjectVertex*)MeMemoryALLOCA((sides+1) * sizeof(RObjectVertex));
    RObjectVertex* bottomPoly = (RObjectVertex*)MeMemoryALLOCA((sides+1) * sizeof(RObjectVertex));
    MeVector3 vertex[2], normal;
    int i, j, k;

    /* bottom */
    stripSize[0] = sides;
    stripStart[0] = 0;

    /* side */
    stripSize[1] = (sides+1)*2;
    stripStart[1] = stripStart[0] + stripSize[0];

    /* top */
    stripSize[2] = sides;
    stripStart[2] = stripStart[1] + stripSize[1];

    vertex[0][0] = topRadius;
    vertex[0][1] = 0;
    vertex[0][2] = top;

    vertex[1][0] = bottomRadius;
    vertex[1][1] = 0;
    vertex[1][2] = bottom;

    normal[0] = top-bottom;
    normal[1] = 0;
    normal[2] = bottomRadius-topRadius;

    MeVector3Normalize(normal);

    /* Work around cylinder to create side and end polygons. */
    for(j=0; j<sides+1; j++)
    {
        MeMatrix3 R;
        MeVector3 tmp;
        MeReal aroundAngle = ((MeReal)j/sides) * 2 * ME_PI;
        MeMatrix3MakeRotationZ(R, aroundAngle);

        MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[0]);
        strips[stripStart[1]+(j*2)].m_X = tmp[0];
        strips[stripStart[1]+(j*2)].m_Y = tmp[1];
        strips[stripStart[1]+(j*2)].m_Z = tmp[2];

        topPoly[j].m_X = tmp[0];
        topPoly[j].m_Y = tmp[1];
        topPoly[j].m_Z = tmp[2];
        topPoly[j].m_NX = topPoly[j].m_NY = 0;
        topPoly[j].m_NZ = 1;
        topPoly[j].m_U = (MeCos(aroundAngle)+1)*(MeReal)0.5;
        topPoly[j].m_V = (MeSin(aroundAngle)+1)*(MeReal)0.5;

        MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[1]);
        strips[stripStart[1]+(j*2)+1].m_X = tmp[0];
        strips[stripStart[1]+(j*2)+1].m_Y = tmp[1];
        strips[stripStart[1]+(j*2)+1].m_Z = tmp[2];

        bottomPoly[j].m_X = tmp[0];
        bottomPoly[j].m_Y = tmp[1];
        bottomPoly[j].m_Z = tmp[2];
        bottomPoly[j].m_NX = bottomPoly[j].m_NY = 0;
        bottomPoly[j].m_NZ = -1;
        bottomPoly[j].m_U = (MeCos(aroundAngle)+1)*(MeReal)0.5;
        bottomPoly[j].m_V = (MeSin(aroundAngle)+1)*(MeReal)0.5;

        MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal);
        strips[stripStart[1]+(j*2)].m_NX = tmp[0];
        strips[stripStart[1]+(j*2)].m_NY = tmp[1];
        strips[stripStart[1]+(j*2)].m_NZ = tmp[2];
        strips[stripStart[1]+(j*2)+1].m_NX = tmp[0];
        strips[stripStart[1]+(j*2)+1].m_NY = tmp[1];
        strips[stripStart[1]+(j*2)+1].m_NZ = tmp[2];

        strips[stripStart[1]+(j*2)].m_U = ((MeReal)j/sides);
        strips[stripStart[1]+(j*2)].m_V = 0;
        strips[stripStart[1]+(j*2)+1].m_U = ((MeReal)j/sides);
        strips[stripStart[1]+(j*2)+1].m_V = 1;
    }

    /**** CONVERT POLYGONS INTO STRIPS ***/
    i = 0; /* first vertex */
    j = sides-1; /* last vertex */
    k = 0; /* position in strip */

    while(i <= j)
    {
        strips[stripStart[0]+k] = bottomPoly[i];
        strips[stripStart[2]+k] = topPoly[i];
        k++;

        if(i != j)
        {
            strips[stripStart[0]+k] = bottomPoly[j];
            strips[stripStart[2]+k] = topPoly[j];
            k++;
        }

        i++; j--;
    }

    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);
    MeMemoryFREEA(topPoly);
    MeMemoryFREEA(bottomPoly);
}

/********************** SPHERE ***********************/
int RCalculateSphereVertexCount(int sides, int rings)
{
    /* (number of triangles)*3 */
    return (sides * rings * 2) * 3;
}

/* This isn't perfect, cos it doesn't use fans at the top and bottom. */

void RCalculateSphereGeometry(RGraphic* rg, AcmeReal radius,
                              int sides, int rings)
{
    /* first allocate triangles strips. */
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA(2*(sides+1)*(rings)*sizeof(RObjectVertex));
    int* stripStart = (int*)MeMemoryALLOCA(rings*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(rings*sizeof(int));
    int numStrips = rings;

    MeVector3* vertex = (MeVector3*)MeMemoryALLOCA((rings+1)*sizeof(MeVector3));
    MeVector3* normal = (MeVector3*)MeMemoryALLOCA((rings+1)*sizeof(MeVector3));
    MeReal* vcoord = (MeReal*)MeMemoryALLOCA((rings+1)*sizeof(MeReal));
    int i, j;
    int stripLength = 2*(sides+1);

    for(i=0,j=0; i<numStrips; i++,j+=stripLength)
    {
        stripStart[i] = j;
        stripSize[i] = stripLength;
    }

    /* First, make one 'arc' */
    for(i=0; i<rings+1; i++)
    {
        MeReal angle = ((MeReal)i/rings) * ME_PI;

        normal[i][0] = 0;
        normal[i][1] = MeSin(angle);
        normal[i][2] = MeCos(angle);

        MeVector3Copy(vertex[i], normal[i]);
        MeVector3Scale(vertex[i], radius);

        vcoord[i] = ((MeReal)i/rings);
    }

    /* Then transform that arc to each angle. */
    for(j=0; j<sides+1; j++)
    {
        MeMatrix3 R;
        MeReal aroundAngle = ((MeReal)j/sides) * 2 * ME_PI;
        MeMatrix3MakeRotationZ(R, aroundAngle);

        for(i=0; i<rings; i++)
        {
            MeVector3 tmp;

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i]);
            strips[stripStart[i]+(j*2)].m_X = tmp[0];
            strips[stripStart[i]+(j*2)].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i]);
            strips[stripStart[i]+(j*2)].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)].m_NZ = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i+1]);
            strips[stripStart[i]+(j*2)+1].m_X = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i+1]);
            strips[stripStart[i]+(j*2)+1].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_NZ = tmp[2];

            strips[stripStart[i]+(j*2)].m_U = ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)].m_V = vcoord[i];
            strips[stripStart[i]+(j*2)+1].m_U = ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)+1].m_V = vcoord[i+1];
        }
    }

    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);
    MeMemoryFREEA(vertex);
    MeMemoryFREEA(normal);
}

/********************** SPHYL ***********************/
/* Rings refers to one hemisphere end. */
int RCalculateSphylVertexCount(int sides, int rings)
{
    /* (number of triangles)*3 */
    return (sides * ((2 * rings) + 1) * 2) * 3;
}

/* Ripped apart sphere. Not entirely sure how to texture map it though... */
void RCalculateSphylGeometry(RGraphic* rg, AcmeReal radius, AcmeReal height,
                              int sides, int rings)
{
    /* first allocate triangles strips. */
    int numStrips = (2 * rings) + 1;
    int stripLength = 2*(sides+1);
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA(stripLength*numStrips*sizeof(RObjectVertex));
    int* stripStart = (int*)MeMemoryALLOCA(numStrips*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(numStrips*sizeof(int));

    MeVector3* vertex = (MeVector3*)MeMemoryALLOCA((numStrips+1)*sizeof(MeVector3));
    MeVector3* normal = (MeVector3*)MeMemoryALLOCA((numStrips+1)*sizeof(MeVector3));
    MeReal* vcoord = (MeReal*)MeMemoryALLOCA((numStrips+1)*sizeof(MeReal));
    int i, j;

    for(i=0,j=0; i<numStrips; i++,j+=stripLength)
    {
        stripStart[i] = j;
        stripSize[i] = stripLength;
    }

    /* First, make one complete sphere 'arc' */

    /* First hemisphere. */
    for(i=0; i<rings+1; i++)
    {
        MeReal angle = ((MeReal)i/(2*rings)) * ME_PI;

        normal[i][0] = 0;
        normal[i][1] = MeSin(angle);
        normal[i][2] = MeCos(angle);

        MeVector3Copy(vertex[i], normal[i]);
        MeVector3Scale(vertex[i], radius);
        vertex[i][2] += (MeReal)0.5 * height;

        vcoord[i] = ((MeReal)i/(numStrips+1));
    }

    /* Second hemisphere */
    for(i=rings+1; i<numStrips+1; i++)
    {
        MeReal angle = ((MeReal)(i-1)/(2*rings)) * ME_PI;

        normal[i][0] = 0;
        normal[i][1] = MeSin(angle);
        normal[i][2] = MeCos(angle);

        MeVector3Copy(vertex[i], normal[i]);
        MeVector3Scale(vertex[i], radius);
        vertex[i][2] -= (MeReal)0.5 * height;

        vcoord[i] = ((MeReal)(i-1)/(numStrips+1));
    }
    
    /* Then transform that arc to each angle. */
    for(j=0; j<sides+1; j++)
    {
        MeMatrix3 R;
        MeReal aroundAngle = ((MeReal)j/sides) * 2 * ME_PI;
        MeMatrix3MakeRotationZ(R, aroundAngle);

        for(i=0; i<numStrips; i++)
        {
            MeVector3 tmp;

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i]);
            strips[stripStart[i]+(j*2)].m_X = tmp[0];
            strips[stripStart[i]+(j*2)].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i]);
            strips[stripStart[i]+(j*2)].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)].m_NZ = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i+1]);
            strips[stripStart[i]+(j*2)+1].m_X = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i+1]);
            strips[stripStart[i]+(j*2)+1].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_NZ = tmp[2];

            strips[stripStart[i]+(j*2)].m_U = ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)].m_V = vcoord[i];
            strips[stripStart[i]+(j*2)+1].m_U = ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)+1].m_V = vcoord[i+1];
        }
    }

    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);
    MeMemoryFREEA(vertex);
    MeMemoryFREEA(normal);
}

/********************** CONE ***********************/

int RCalculateConeVertexCount(int sides)
{
    /* (number of triangles)*3 */
    int sideTriCount = 2*sides;
    int endTriCount = sides-2;

    return (sideTriCount + endTriCount)*3;
}
void RCalculateConeGeometry(RGraphic* rg, AcmeReal radius,
                               AcmeReal bottom, AcmeReal top, int sides)
{
    /* first allocate triangles strips. */
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA((((sides + 1)*2) + (sides+1))*sizeof(RObjectVertex));
    int* stripStart = (int*)MeMemoryALLOCA(2*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(2*sizeof(int));
    int numStrips = 2;

    RObjectVertex* bottomPoly = (RObjectVertex*)MeMemoryALLOCA((sides+1) * sizeof(RObjectVertex));
    MeVector3 vertex[2], normal;
    int i, j, k;

    /* bottom */
    stripSize[0] = sides;
    stripStart[0] = 0;

    /* side */
    stripSize[1] = (sides+1)*2;
    stripStart[1] = stripStart[0] + stripSize[0];

    vertex[0][0] = 0;
    vertex[0][1] = 0;
    vertex[0][2] = top;

    vertex[1][0] = radius;
    vertex[1][1] = 0;
    vertex[1][2] = bottom;

    normal[0] = top-bottom;
    normal[1] = 0;
    normal[2] = radius;

    MeVector3Normalize(normal);

    /* Work around cylinder to create side and end polygons. */
    for(j=0; j<sides+1; j++)
    {
        MeMatrix3 R;
        MeVector3 tmp;
        MeReal aroundAngle = ((MeReal)j/sides) * 2 * ME_PI;
        MeMatrix3MakeRotationZ(R, aroundAngle);

        MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[0]);
        strips[stripStart[1]+(j*2)].m_X = tmp[0];
        strips[stripStart[1]+(j*2)].m_Y = tmp[1];
        strips[stripStart[1]+(j*2)].m_Z = tmp[2];

        MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[1]);
        strips[stripStart[1]+(j*2)+1].m_X = tmp[0];
        strips[stripStart[1]+(j*2)+1].m_Y = tmp[1];
        strips[stripStart[1]+(j*2)+1].m_Z = tmp[2];

        bottomPoly[j].m_X = tmp[0];
        bottomPoly[j].m_Y = tmp[1];
        bottomPoly[j].m_Z = tmp[2];
        bottomPoly[j].m_NX = bottomPoly[j].m_NY = 0;
        bottomPoly[j].m_NZ = -1;
        bottomPoly[j].m_U = (MeCos(aroundAngle)+1)*(MeReal)0.5;
        bottomPoly[j].m_V = (MeSin(aroundAngle)+1)*(MeReal)0.5;

        MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal);
        strips[stripStart[1]+(j*2)].m_NX = tmp[0];
        strips[stripStart[1]+(j*2)].m_NY = tmp[1];
        strips[stripStart[1]+(j*2)].m_NZ = tmp[2];
        strips[stripStart[1]+(j*2)+1].m_NX = tmp[0];
        strips[stripStart[1]+(j*2)+1].m_NY = tmp[1];
        strips[stripStart[1]+(j*2)+1].m_NZ = tmp[2];

        strips[stripStart[1]+(j*2)].m_U = ((MeReal)j/sides);
        strips[stripStart[1]+(j*2)].m_V = 0;
        strips[stripStart[1]+(j*2)+1].m_U = ((MeReal)j/sides);
        strips[stripStart[1]+(j*2)+1].m_V = 1;
    }

    /**** CONVERT POLYGONS INTO STRIPS ***/
    i = 0; /* first vertex */
    j = sides-1; /* last vertex */
    k = 0; /* position in strip */

    while(i <= j)
    {
        strips[stripStart[0]+k] = bottomPoly[i];
        k++;

        if(i != j)
        {
            strips[stripStart[0]+k] = bottomPoly[j];
            k++;
        }

        i++; j--;
    }

    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);
    MeMemoryFREEA(bottomPoly);
}

/********************** BOX ***********************/
int RCalculateBoxVertexCount()
{
    return (6 * 2) * 3;
}

void makeVertex(RObjectVertex* vertex,
                AcmeReal x, AcmeReal y, AcmeReal z, MeVector3 normal,
                AcmeReal u, AcmeReal v)
{
    vertex->m_X = x;
    vertex->m_Y = y;
    vertex->m_Z = z;
    vertex->m_NX = normal[0];
    vertex->m_NY = normal[1];
    vertex->m_NZ = normal[2];
    vertex->m_U = u;
    vertex->m_V = v;
}

void RCalculateBoxGeometry(RGraphic* rg, AcmeReal lx,
                           AcmeReal ly, AcmeReal lz)
{
    /* first allocate triangles strips. */
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA(6 * 4 * sizeof(RObjectVertex));
    int* stripStart = (int*)MeMemoryALLOCA(6*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(6*sizeof(int));
    int numStrips = 6;
    MeVector3 normal;
    MeReal half = (MeReal)0.5;


    /* y = ly/2 */
    stripStart[0] = 0;
    stripSize[0] = 4;
    normal[0] = 0; normal[1] = 1; normal[2] = 0;
    makeVertex(&strips[stripStart[0]+0], -lx*half,  ly*half, -lz*half, normal, 0, 0);
    makeVertex(&strips[stripStart[0]+1],  lx*half,  ly*half, -lz*half, normal, 1, 0);
    makeVertex(&strips[stripStart[0]+2], -lx*half,  ly*half,  lz*half, normal, 0, 1);
    makeVertex(&strips[stripStart[0]+3],  lx*half,  ly*half,  lz*half, normal, 1, 1);

    /* y = -ly/2 */
    stripStart[1]= stripStart[0] + stripSize[0];
    stripSize[1] = 4;
    normal[0] = 0; normal[1] = -1; normal[2] = 0;
    makeVertex(&strips[stripStart[1]+0], -lx*half, -ly*half, -lz*half, normal, 0, 0);
    makeVertex(&strips[stripStart[1]+1],  lx*half, -ly*half, -lz*half, normal, 1, 0);
    makeVertex(&strips[stripStart[1]+2], -lx*half, -ly*half,  lz*half, normal, 0, 1);
    makeVertex(&strips[stripStart[1]+3],  lx*half, -ly*half,  lz*half, normal, 1, 1);

    /* x = lx/2 */
    stripStart[2]= stripStart[1] + stripSize[1];
    stripSize[2] = 4;
    normal[0] = 1; normal[1] = 0; normal[2] = 0;
    makeVertex(&strips[stripStart[2]+0],  lx*half, -ly*half, -lz*half, normal, 0, 0);
    makeVertex(&strips[stripStart[2]+1],  lx*half,  ly*half, -lz*half, normal, 1, 0);
    makeVertex(&strips[stripStart[2]+2],  lx*half, -ly*half,  lz*half, normal, 0, 1);
    makeVertex(&strips[stripStart[2]+3],  lx*half,  ly*half,  lz*half, normal, 1, 1);

    /* x = -lx/2 */
    stripStart[3]= stripStart[2] + stripSize[2];
    stripSize[3] = 4;
    normal[0] = -1; normal[1] = 0; normal[2] = 0;
    makeVertex(&strips[stripStart[3]+0], -lx*half, -ly*half, -lz*half, normal, 0, 0);
    makeVertex(&strips[stripStart[3]+1], -lx*half,  ly*half, -lz*half, normal, 1, 0);
    makeVertex(&strips[stripStart[3]+2], -lx*half, -ly*half,  lz*half, normal, 0, 1);
    makeVertex(&strips[stripStart[3]+3], -lx*half,  ly*half,  lz*half, normal, 1, 1);

    /* z = lz/2 */
    stripStart[4]= stripStart[3] + stripSize[3];
    stripSize[4] = 4;
    normal[0] = 0; normal[1] = 0; normal[2] = 1;
    makeVertex(&strips[stripStart[4]+0], -lx*half, -ly*half,  lz*half, normal, 0, 0);
    makeVertex(&strips[stripStart[4]+1],  lx*half, -ly*half,  lz*half, normal, 1, 0);
    makeVertex(&strips[stripStart[4]+2], -lx*half,  ly*half,  lz*half, normal, 0, 1);
    makeVertex(&strips[stripStart[4]+3],  lx*half,  ly*half,  lz*half, normal, 1, 1);

    /* z = -lz/2 */
    stripStart[5]= stripStart[4] + stripSize[4];
    stripSize[5] = 4;
    normal[0] = 0; normal[1] = 0; normal[2] = -1;
    makeVertex(&strips[stripStart[5]+0], -lx*half, -ly*half, -lz*half, normal, 0, 0);
    makeVertex(&strips[stripStart[5]+1],  lx*half, -ly*half, -lz*half, normal, 1, 0);
    makeVertex(&strips[stripStart[5]+2], -lx*half,  ly*half, -lz*half, normal, 0, 1);
    makeVertex(&strips[stripStart[5]+3],  lx*half,  ly*half, -lz*half, normal, 1, 1);

    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);
}


/************************ ASE ************************/

/* If 'noMCD' flag is true, it will not load parts of the geometry whose names
    start 'MCDBX','MCDSP','MCDCY' or 'MCDCX'. This is for use with the ase2me
    util which uses these as special keywords for designating collision 
    geometry, and may not be for drawing. */


/* Find if this part has a 'special' name and should not be loaded. */
static MeBool isSpecial(const char* name)
{
#if (defined LINUX)
    return
        (strncasecmp(name,"MCDSP",5) == 0)
        || (strncasecmp(name,"MCDBX",5) == 0)
        || (strncasecmp(name,"MCDCY",5) == 0)
        || (strncasecmp(name,"MCDCX",5) == 0);
#else
    char NAME[256];
    unsigned i;
    
    /* Copy and push to upper case for case-insensitive match. */
    
    for (i = 0; name[i] != '\0' && i < sizeof NAME-1; i++)
        NAME[i] = name[i] - ((islower(name[i])) ? 'a'-'A' : 0);
    NAME[i] = '\0';
    
    if(strncmp(NAME, "MCDSP", 5) == 0)
        return 1;
    else if(strncmp(NAME, "MCDBX", 5) == 0)
        return 1;
    else if(strncmp(NAME, "MCDCY", 5) == 0)
        return 1;
    else if(strncmp(NAME, "MCDCX", 5) == 0)
        return 1;
    
    return 0;
#endif
}

int RCalculateASEVertexCount(MeASEObject* obj, MeBool noMCD)
{
    int totalFaces = 0;
    
    while(obj)
    {
        if(!noMCD || !isSpecial(obj->name))
        {
            totalFaces += obj->numFaces;
        }
        obj = obj->nextObject;
    }
    
    return totalFaces * 3;
}

void RLoadASEGeometry(RGraphic* rg, MeASEObject* obj, MeBool noMCD)
{
    int i,j,tri=0;
    
    while(obj)
    {
        if(!noMCD || !isSpecial(obj->name))
        {
            for (i=0; i<obj->numFaces; i++,tri++)
            {
                for (j=0;j<3;j++)
                {
                    rg->m_pVertices[(tri*3)+j].m_X = obj->verts[obj->faces[i].vertexId[j]][0];
                    rg->m_pVertices[(tri*3)+j].m_Y = obj->verts[obj->faces[i].vertexId[j]][1];
                    rg->m_pVertices[(tri*3)+j].m_Z = obj->verts[obj->faces[i].vertexId[j]][2];
                    
                    rg->m_pVertices[(tri*3)+j].m_NX = obj->faces[i].vNormal[j][0];
                    rg->m_pVertices[(tri*3)+j].m_NY = obj->faces[i].vNormal[j][1];
                    rg->m_pVertices[(tri*3)+j].m_NZ = obj->faces[i].vNormal[j][2];
                    
                    rg->m_pVertices[(tri*3)+j].m_U = obj->faces[i].map[j].u;
                    rg->m_pVertices[(tri*3)+j].m_V = obj->faces[i].map[j].v;
                }
            }
        }

        /* Move on to next geometry part. */
        obj = obj->nextObject;
    }
}

/********************** DOME **************************/

/* well... more of a smartie really... */

int RCalculateDomeVertexCount(int sides, int rings)
{
    /* (number of triangles)*3 */
    return (sides * rings * 2) * 3;
}

void RCalculateDomeGeometry(RGraphic* rg, AcmeReal radius,
                            int sides, int rings,
                            int tileU, int tileV)
{
/* first allocate triangles strips. */
    RObjectVertex* strips = (RObjectVertex*)MeMemoryALLOCA(2*(sides+1)*(rings)*sizeof(RObjectVertex));
    int* stripStart = (int*)MeMemoryALLOCA(rings*sizeof(int));
    int* stripSize = (int*)MeMemoryALLOCA(rings*sizeof(int));
    int numStrips = rings;

    MeVector3* vertex = (MeVector3*)MeMemoryALLOCA((rings+1)*sizeof(MeVector3));
    MeVector3* normal = (MeVector3*)MeMemoryALLOCA((rings+1)*sizeof(MeVector3));
    MeReal* vcoord = (MeReal*)MeMemoryALLOCA((rings+1)*sizeof(MeReal));
    int i, j;
    int stripLength = 2*(sides+1);

    for(i=0,j=0; i<numStrips; i++,j+=stripLength)
    {
        stripStart[i] = j;
        stripSize[i] = stripLength;
    }

    /* First, make one 'arc' */
    for(i=0; i<rings+1; i++)
    {
        MeReal angle = ((MeReal)i/rings) * ME_PI;

        normal[i][0] = 0;
        normal[i][1] = (MeReal)0.5 * MeCos(angle);
        normal[i][2] = MeSin(angle);

        MeVector3Copy(vertex[i], normal[i]);
        MeVector3Scale(vertex[i], radius);

        vcoord[i] = tileV * ((MeReal)i/rings);
    }

    /* Then transform that arc to each angle. */
    for(j=0; j<sides+1; j++)
    {
        MeMatrix3 R;
        MeReal aroundAngle = ((MeReal)j/sides) * 2 * ME_PI;
        MeMatrix3MakeRotationY(R, aroundAngle);

        for(i=0; i<rings; i++)
        {
            MeVector3 tmp;

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i]);
            strips[stripStart[i]+(j*2)].m_X = tmp[0];
            strips[stripStart[i]+(j*2)].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i]);
            strips[stripStart[i]+(j*2)].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)].m_NZ = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, vertex[i+1]);
            strips[stripStart[i]+(j*2)+1].m_X = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_Y = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_Z = tmp[2];

            MeMatrix3MultiplyVector(tmp, *(const MeMatrix3*)&R, normal[i+1]);
            strips[stripStart[i]+(j*2)+1].m_NX = tmp[0];
            strips[stripStart[i]+(j*2)+1].m_NY = tmp[1];
            strips[stripStart[i]+(j*2)+1].m_NZ = tmp[2];

            strips[stripStart[i]+(j*2)].m_U = tileU * ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)].m_V = vcoord[i];
            strips[stripStart[i]+(j*2)+1].m_U = tileU * ((MeReal)j/sides);
            strips[stripStart[i]+(j*2)+1].m_V = vcoord[i+1];
        }
    }

    RConvertTriStripToTriList(rg, strips, stripSize, stripStart, numStrips);

    MeMemoryFREEA(strips);
    MeMemoryFREEA(stripStart);
    MeMemoryFREEA(stripSize);
    MeMemoryFREEA(vertex);
    MeMemoryFREEA(normal);
}

/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:42 $ - Revision: $Revision: 1.3.2.3 $

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
/* MathEngine headers   */
#include <Mst.h>
#include <MeApp.h>
#include <MeViewer.h>
#include <RGeometryUtils.h>

/* Application headers  */
#include "main.h"
#include "terrain.h"
#include "utils.h"

Landscape landscape;

#define USE_TRI_LIST 0

/***********************************************************************************************
*
***********************************************************************************************/
void CreateTerrainGraphics(void)
{
    if(!landscape.collModel) return;

#if USE_TRI_LIST
    landscape.graphic = HeightfieldCreateGraphic(rc, &landscape.heightField, grassColor,
                                                                        landscape.transform);
    RGraphicSetTexture(rc, landscape.graphic, "stone");
#else
    landscape.graphic = RGraphicGroundPlaneCreate(rc, 50, 2, white, 0);
    RGraphicSetTexture(rc, landscape.graphic, "checkerboard");
#endif
}

/***********************************************************************************************
*
***********************************************************************************************/
static int MEAPI TriListGeneratorCB(McdModelPair *mp, 
                                    McdUserTriangle *triangle,
                                    MeVector3 pos, 
                                    MeReal rad,
                                    int maxTriangles)
{
    McdTriangleList* tl = (McdTriangleList*)landscape.collGeom;
    McdUserTriangle *tri;
    int triCount = 0;
    int ix, iz, k;
    int minX, maxX, minZ, maxZ;

    minX = (int)((pos[0] - landscape.heightField.xOrigin - rad)/DELTA_X);
    maxX = (int)(1 + (pos[0] - landscape.heightField.xOrigin + rad)/DELTA_X);

    minZ = (int)((pos[2] - landscape.heightField.zOrigin - rad)/DELTA_Z);
    maxZ = (int)(1 + (pos[2] - landscape.heightField.zOrigin + rad)/DELTA_Z);

    minX = MeCLAMP(minX, 0, GRIDSIZE_X-1);
    maxX = MeCLAMP(maxX, 0, GRIDSIZE_X-1);
    minZ = MeCLAMP(minZ, 0, GRIDSIZE_Z-1);
    maxZ = MeCLAMP(maxZ, 0, GRIDSIZE_Z-1);

    for(ix=minX; ix<maxX; ix++)
    {
        for(iz=minZ; iz<maxZ; iz++)
        {
            /* [i,j] now points at bottom left-hand corner of possible square
               (2 triangles). */
            for(k=0; k<2; k++)
            {
                if (triCount >= maxTriangles)
                    return triCount;

                tri = &triangle[triCount];

                /* Vertices */
                tri->vertices[0] = &landscape.heightField.vertex_store[ix + vOrder[k][0][0]][iz + vOrder[k][0][1]];
                tri->vertices[1] = &landscape.heightField.vertex_store[ix + vOrder[k][1][0]][iz + vOrder[k][1][1]];
                tri->vertices[2] = &landscape.heightField.vertex_store[ix + vOrder[k][2][0]][iz + vOrder[k][2][1]];

                /*  Normal -- Must be related to vertices / edges using RH rule */
                tri->normal = &landscape.heightField.normal_store[ix][iz][k];

                /*  Could use this to keep your own material data for use in
                    contact callbacks etc. */
                tri->triangleData.ptr = 0;

                /*  We wont use any edge contacts (assume landscape is 'smooth')
                    and triangles are 1-sided, so default flags are fine. */
                tri->flags = 0;

                triCount++;
            }
        }
    }

    return triCount;
}

/***********************************************************************************************
*
***********************************************************************************************/
void InitialiseTerrain(void)
{

    /* Add terrain creation here    */
    /* PASTE_05 */

    /*
        Create graphics.
    */
    CreateTerrainGraphics();
}


/***********************************************************************************************
* Must be 24-bit uncompressed .BMP
***********************************************************************************************/
void HeightFieldFromBMP(MyHeightField* hf, char *filename, MeReal vertScale)
{
    int fh;
    unsigned int len, lenread;
    char *fullname = (char *)MeMemoryAPI.create( strlen(filename) + 55 );
    unsigned char *fileBuffer;
    MeU32 bitmap_offset;
    MeU32 compression_type;
    MeU16 bpp;
    int height, width;
    int i,j;
    int bSkipLoad = 0;
    unsigned char r,g,b;

    MeU32 headerLength;

    fullname[0]='\0';
    strcat(fullname, filename);
    strcat(fullname,".bmp");

    fh = MeOpenWithSearch( fullname, kMeOpenModeRDBINARY );
    if( fh < 0)
    {
        ME_REPORT(MeWarning(1,"Couldn't open file: %s", fullname ));
        MeMemoryAPI.destroy(fullname);
        return;
    }
    MeMemoryAPI.destroy(fullname);

    len = MeLseek(fh, 0, kMeSeekEND);

    fileBuffer = (unsigned char *)MeMemoryAPI.create(len);
    if( !fileBuffer )
    {
        ME_REPORT(MeWarning(1,"MeMemoryAPI.create failed for filebuffer."));
        return;
    }

    if( MeLseek(fh, 0, kMeSeekSET) == -1 )
        ME_REPORT(MeWarning(1,"MeSeek failed."));

    else if( (lenread = MeRead(fh, fileBuffer, len)) != len )
        ME_REPORT(MeInfo(0,"MeRead failed.(len %d, lenread %d)\n",
                  len, lenread));

    headerLength =(((MeU32)fileBuffer[17] << 24) & 0xff) |
                  (((MeU32)fileBuffer[16] << 16) & 0xff) |
                  (((MeU32)fileBuffer[15] <<  8) & 0xff) |
                  (((MeU32)fileBuffer[14]      ) & 0xff);
    bitmap_offset = headerLength + 14;

    if(12==headerLength) {
        /* OS/2 header style :) */
        bpp = *(MeU16*)(&fileBuffer[24]);
        compression_type = 0;
    } else {
        /* Windows style */
        bpp =
          ((((MeU16)fileBuffer[29] & 0xff) <<  8 )) |
          ((((MeU16)fileBuffer[28] & 0xff)       ));
        compression_type =
          ((((MeU32)fileBuffer[33] & 0xff) << 24)) |
          ((((MeU32)fileBuffer[32] & 0xff) << 16)) |
          ((((MeU32)fileBuffer[31] & 0xff) <<  8)) |
          ((((MeU32)fileBuffer[30] & 0xff)      ));
        width =
          ((((MeU32)fileBuffer[21] & 0xff) << 24)) |
          ((((MeU32)fileBuffer[20] & 0xff) << 16)) |
          ((((MeU32)fileBuffer[19] & 0xff) <<  8)) |
          ((((MeU32)fileBuffer[18] & 0xff)      ));
        height =
          ((((MeU32)fileBuffer[25] & 0xff) << 24)) |
          ((((MeU32)fileBuffer[24] & 0xff) << 16)) |
          ((((MeU32)fileBuffer[23] & 0xff) <<  8)) |
          ((((MeU32)fileBuffer[22] & 0xff)      ));
    }

    if( bpp != 24 )
    {
        ME_REPORT(MeWarning(1,"%s.bmp %s",
                            filename,
                            "is not 24bpp and so cannot be loaded."));
        bSkipLoad = 1;
    }
    if( compression_type != 0 ) /* BI_RGB */
    {
        ME_REPORT(MeWarning(1,"%s.bmp %s",
                            filename, "is in a compressed format"
                            " and will not be loaded."));
        bSkipLoad = 1;
    }

    if( !bSkipLoad )
    {
        int ix, iz, k;

        hf->xOrigin = -GRIDSIZE_X * DELTA_X * 0.5f;
        hf->zOrigin = -GRIDSIZE_Z * DELTA_Z * 0.5f;

        for(i=0; i<GRIDSIZE_Z; i++)
        {
            for(j=0; j<GRIDSIZE_X; j++)
            {
                MeReal fieldHeight;
                /* ensure we are not reading outside bitmap. */
                if(i < height && j < width)
                {
                    b = (unsigned char)fileBuffer[bitmap_offset + 3*(i*width + j) + 0];
                    g = (unsigned char)fileBuffer[bitmap_offset + 3*(i*width + j) + 1];
                    r = (unsigned char)fileBuffer[bitmap_offset + 3*(i*width + j) + 2];

                    fieldHeight = (MeReal)(b+g+r)/(MeReal)(255*3);
                }
                else
                {
                    fieldHeight = 0;
                }

                hf->heights[i][j] = vertScale * fieldHeight;

                hf->vertex_store[j][i][0] = hf->xOrigin + (j * DELTA_X);
                hf->vertex_store[j][i][1] = hf->heights[i][j];
                hf->vertex_store[j][i][2] = hf->zOrigin + (i * DELTA_X);
            }
        }

        /* Now calculate normals for all triangles in advance. */
        for(ix=0; ix<GRIDSIZE_X-1; ix++)
        {
            for(iz=0; iz<GRIDSIZE_Z-1; iz++)
            {
                for(k=0; k<2; k++)
                {
                    MeVector3 edge1, edge2;
                    MeVector4 normal = {0, 0, 0, 0};

                    /* Normal -- Must be related to vertices / edges using RH rule */

                    /* Edges - Not needed if you are storing the poly Normals */
                    MeVector3Subtract(edge1,
                        hf->vertex_store[ix + vOrder[k][1][0]][iz + vOrder[k][1][1]],
                        hf->vertex_store[ix + vOrder[k][0][0]][iz + vOrder[k][0][1]]);

                    MeVector3Subtract(edge2,
                        hf->vertex_store[ix + vOrder[k][2][0]][iz + vOrder[k][2][1]],
                        hf->vertex_store[ix + vOrder[k][1][0]][iz + vOrder[k][1][1]]);

                    /* Normal -- Must be related to vertices / edges using RH rule */
                    MeVector3Cross(normal, edge1, edge2);
                    MeVector3Normalize(normal);

                    MeVector3Copy(hf->normal_store[ix][iz][k], normal);
                }
            }
        }

    }

    if(MeClose(fh))
        ME_REPORT(MeWarning(1,"Failed to close file handle. Continuing anyway..."));

    MeMemoryAPI.destroy(fileBuffer);

    return;
}

/***********************************************************************************************
*
***********************************************************************************************/
MeReal FindHeight(MyHeightField* hf, MeReal x, MeReal z)
{
    int minX, maxX, minZ, maxZ;
    MeReal maxHeight = 0;

    minX = (int)((x - hf->xOrigin)/DELTA_X);
    maxX = (int)(1 + (x - hf->xOrigin)/DELTA_X);

    minZ = (int)((z - hf->zOrigin)/DELTA_Z);
    maxZ = (int)(1 + (z - hf->zOrigin)/DELTA_Z);

    minX = MeCLAMP(minX, 0, GRIDSIZE_X-1);
    maxX = MeCLAMP(maxX, 0, GRIDSIZE_X-1);
    minZ = MeCLAMP(minZ, 0, GRIDSIZE_Z-1);
    maxZ = MeCLAMP(maxZ, 0, GRIDSIZE_Z-1);

    if(hf->heights[minZ][minX] > maxHeight) maxHeight = hf->heights[minZ][minX];
    if(hf->heights[minZ][maxX] > maxHeight) maxHeight = hf->heights[minZ][maxX];
    if(hf->heights[maxZ][minX] > maxHeight) maxHeight = hf->heights[maxZ][minX];
    if(hf->heights[maxZ][maxX] > maxHeight) maxHeight = hf->heights[maxZ][maxX];

    return maxHeight;
}

/***********************************************************************************************
*
***********************************************************************************************/
RGraphic* MEAPI HeightfieldCreateGraphic(RRender* rc, MyHeightField* hf,
                                         float color[4], MeMatrix4Ptr matrix)
{
    int ix, iz, k, m, triCount, numTris = 0;
    RGraphic *rg;

    triCount = (GRIDSIZE_X - 1) * (GRIDSIZE_Z - 1) * 2;

    /* Create graphic with required number of triangles. */
    rg = RGraphicCreateEmpty(triCount * 3);
    if ( !rg )
        return 0;

    /* Convert each triangle in turn into a strip. */
    for(ix=0; ix<GRIDSIZE_X-1; ix++)
    {
        for(iz=0; iz<GRIDSIZE_Z-1; iz++)
        {
            for(k=0; k<2; k++)
            {
                for(m=0; m<3; m++)
                {
                    MeReal u = (MeReal)(ix + vOrder[k][m][0])/GRIDSIZE_X;
                    MeReal v = (MeReal)(iz + vOrder[k][m][1])/GRIDSIZE_Z;

                    RSetVertex(&(rg->m_pVertices[numTris*3 + m]),
                        hf->vertex_store[ix + vOrder[k][m][0]][iz + vOrder[k][m][1]],
                        hf->normal_store[ix][iz][k], u, v);
                }

                numTris++;
            }
        }
    }

    rg->m_pObject->m_nTextureID = -1; /* no texture */
    RGraphicSetColor(rg,color);
    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);
    RGraphicAddToList(rc, rg, 0);

    return rg;
}

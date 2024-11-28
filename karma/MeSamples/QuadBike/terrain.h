/* -*- mode: c; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:13 $ - Revision: $Revision: 1.2.2.4 $

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

#define TRILIST_SIZE    600

#define GRIDSIZE_X      (64)
#define GRIDSIZE_Z      (64)

#define DELTA_X         (1.4f)
#define DELTA_Z         (1.4f)

typedef struct
{
    MeReal              xOrigin;
    MeReal              zOrigin;
    MeReal              heights[GRIDSIZE_Z][GRIDSIZE_X];
    MeVector3           vertex_store[GRIDSIZE_X][GRIDSIZE_Z];
    MeVector3           normal_store[GRIDSIZE_X-1][GRIDSIZE_Z-1][2];
} MyHeightField;

/*
    Defines how me make 2 triangles out of a heightfield square.
    First/second triangle, vertex1/2/3, di/dj
*/
static int vOrder[2][3][2] =
{
    {{0, 0}, {0, 1}, {1, 0}},
    {{1, 0}, {0, 1}, {1, 1}}
};

typedef struct 
{
    McdGeometryID collGeom;
    McdModelID collModel;
    RGraphic *graphic;
    MeMatrix4Ptr transform;
    MyHeightField heightField;
    McdUserTriangle triListMemory[TRILIST_SIZE];
} Landscape;

extern Landscape landscape;

extern void InitialiseTerrain(void);
void HeightFieldFromBMP(MyHeightField* hf, char *filename, MeReal vertScale);
RGraphic* MEAPI HeightfieldCreateGraphic(RRender* rc, MyHeightField* hf,
                                         float color[4], MeMatrix4Ptr matrix);
MeReal FindHeight(MyHeightField* hf, MeReal x, MeReal z);

#error this file is deprecated
#ifndef _MCDUTRIANGLEMESHIO_H
#define _MCDUTRIANGLEMESHIO_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:53 $ - Revision: $Revision: 1.25.2.3 $

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

/**
   @file

   Function for in-memory construction of a mesh from the contents
   of a description file.

 */

#ifdef __cplusplus
extern "C" {
#endif

/**
    Create a triangle mesh by reading vertices and faces from a .obj file.
    Faces are assumed to be convex and will be triangulated.
    If @a merge is non-zero,
    vertices are merged if their coordinates differ by at most eps.
*/

McdTriangleMeshID MEAPI McduTriangleMeshCreateFromObj( const char *filename,
                McdFrameworkID frame, MeReal scale, int center, int merge, MeReal eps,
                MeReal** vertexPtr, int *outVertexCount,
        McdTrianglesMeshOptions dist);

/**
    Save a TriangleMesh to .obj format.
    Preserves vertex identity.
    If @merge is nonzero, vertices closer than @a eps will be merged together,
    otherwise @a eps is ignored.
    @a eps must be non-negative but can be zero.
*/
int MEAPI McduTriangleMeshWriteToObj( const char *filename,
                McdTriangleMeshID mesh,
                int merge, MeReal eps );

#ifdef __cplusplus
}
#endif

#endif /* _MCDUTRIANGLEMESHIO_H */

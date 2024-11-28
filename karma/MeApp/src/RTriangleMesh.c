/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.6.6.3 $

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

#include <RTriangleMesh.h>
#include <RGeometryUtils.h>

/**
 * Create a triangle mesh graphic from an McdTriangleMesh collision geometry.
 */
RGraphic* MEAPI RGraphicTriangleMeshCreate(RRender* rc, McdTriangleMeshID mesh,
                                           float color[4], MeMatrix4Ptr matrix)
{
  int i, j, k;
  RGraphic *rg = 0;
  MeVector3Ptr normal, v[3];
  RObjectVertex* vertices;
  int numTris;

  numTris = McdTriangleMeshGetTriangleCount(mesh);

  vertices = (RObjectVertex*)MeMemoryALLOCA(numTris*3*sizeof(RObjectVertex));
  rg = RGraphicCreateEmpty(numTris*3);

  for (i=0; i<numTris; i++)
  {
    McdTriangleMeshGetTriangleVertexPtrs(mesh,i, &v[0], &v[1], &v[2]);
    McdTriangleMeshGetTriangleNormalPtr(mesh,i, &normal);
    for(j=0; j<3; j++)
    {
      k = i*3+j;
      RSetVertex(&vertices[k], v[j], normal, 0, 0);
      rg->m_pVertices[k] = vertices[k];
    }
  }

  /* Create temporary strip storage. */
  rg->m_pObject->m_nTextureID = -1; /* no texture */
  RGraphicSetColor(rg,color);
  rg->m_pObject->m_bIsWireFrame = 0;
  rg->m_pLWMatrix = matrix;
  MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);
  RGraphicAddToList(rc, rg, 0);

  MeMemoryFREEA(vertices);

  return rg;
}

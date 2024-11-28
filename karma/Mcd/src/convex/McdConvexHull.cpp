/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/10/03 15:34:44 $ - Revision: $Revision: 1.15.2.5.4.2 $

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

#include <qhull_a.h> /* now using a hacked version of QHull/user.h */
#include <MeAssert.h>
#include <MeMemory.h>
#include <McdConvexMesh.h>
#include <McdQHullTypes.h>


/*****************************************************************************/

char qh_version[] = "version 2.6 1998/8/12";  /* used for error messages */

/*  This is the last cnv used to call McdComputeHullSizes */

static McdConvexHull *last_cnv = 0;
static void *temp_mem = 0;

/****************************************************************************
  This computes the convex hull and returns the number of vertices,
  edges, and faces.

  It is presumed that the caller will invoke this function to
  determine the memory required for the convex hull, then allocate the 
  memory, and then call McdGetHullData to get the actual data.

  returns 1 on success, or 0 on error.
*/
int McdComputeHullSizes(McdConvexHull *cnv,
                        int numpoints, const MeVector3 *points)
{
    facetT *facet;
    int num_edges;
    int rc, i, j, k;
    coordT *pts = (coordT*) points;

    //  Ensure qhull and Karma are using the same floating point type.

    MEASSERT(sizeof(coordT) == sizeof(MeReal));

    //  Ensure MeVector3 actually has three MeReals.

    if (sizeof(MeVector3) != 3*sizeof(MeReal))
    {
        //  On some platforms like PS2 the MeVector3 is actually MeReal[4].
        //  In that case we must remove the padding before calling qhull.
        //  This memory will be destroyed at the end of McdGetHullData.
        //  Note, ALLOCA does not work here because qhull holds pointers to
        //  the memory.

        MEASSERT(!temp_mem);        // test memory leak
        temp_mem = MeMemoryAPI.create(numpoints*3*sizeof(MeReal));
        pts = (coordT*) temp_mem;

        for (k = i = 0; i < numpoints; ++i)
            for (j = 0; j < 3; ++j)
                pts[k++] = points[i][j];
    }

    /* use qhull to compute convex hull: */
    rc = qh_new_qhull(3, numpoints, pts, 0, "qhull s", 0, stderr);

    if (rc)
    {
#ifdef _MECHECK
        MeInfo(0,"QHull failed, exitcode = %d\n", rc);
#endif
        return 0;            // RETURN FAILURE
    }

    //  Count the number of edges (times two)
    num_edges = 0;
    FORALLfacets 
    {
        num_edges += qh_setsize(facet->neighbors);
    }

    cnv->numFace = qh num_facets;
    cnv->numVertex = qh num_vertices;      // This does NOT include interior points
    cnv->numEdge = num_edges;

    last_cnv = cnv;

    return 1;          // RETURN SUCCESS
}

/****************************************************************************
  This allocates memory for the convex hull based on the sizes.
  One extra face and vertex is required for the sentinel.
*/
void McdAllocateHull(McdConvexHull *cnv)
{
    cnv->face = (McdCnvFace*) MeMemoryAPI.create(
        (cnv->numFace + 1) * sizeof(McdCnvFace));

    cnv->vertex = (McdCnvVertex*) MeMemoryAPI.create(
        (cnv->numVertex + 1) * sizeof(McdCnvVertex));

    cnv->edge = (McdCnvEdge*) MeMemoryAPI.create(
        cnv->numEdge * sizeof(McdCnvEdge));

    cnv->edgeIndex = (int*) MeMemoryAPI.create(
        cnv->numEdge * sizeof(int));
}

/****************************************************************************
  This deallocates the memory that is allocated by McdAllocateHull
  or by McdComputeHull.
*/
void McdDeallocateHull(McdConvexHull *cnv)
{
    MeMemoryAPI.destroy(cnv->face);
    MeMemoryAPI.destroy(cnv->vertex);
    MeMemoryAPI.destroy(cnv->edge);
    MeMemoryAPI.destroy(cnv->edgeIndex);
}

/****************************************************************************
  This is an internal function that is used to search for an edge.
    f is the face (or -1 for any face)
    to is 1=to, 0=from
    v is the vertex index
*/
static int McdFindEdge(const McdConvexHull *cnv, int f, int to, int v)
{
    int i, b=0, e=cnv->numEdge;
    if (f >= 0)
    {
        MEASSERT(f < cnv->numFace);
        b = cnv->face[f].firstEdge;
        e = cnv->face[f+1].firstEdge;
    }

    for (i = b; i < e; ++i)
        if (v == (to ? cnv->edge[i].toVert : 
                       cnv->edge[i].fromVert))
            return i;

    MEASSERT(!"edge not found");
    return i;                   //  RETURN FAILURE
}


/****************************************************************************
  This precomputes some stuff, like the invLength of each edge.
*/
static void McdPrecomputeStuff(McdConvexHull *cnv)
{
    int i;
    McdCnvEdge *e;

    for (i = 0; i < cnv->numEdge; ++i)
    {
        e = cnv->edge + i;
        e->invLength = MeVector3Distance(cnv->vertex[e->fromVert].position,
                                         cnv->vertex[e->toVert].position);
        if (e->invLength)
            e->invLength = 1 / e->invLength;
    }
}

/****************************************************************************
  This gets the convex hull data that was generated in the previous call
  to McdComputeHullSizes.

  The caller must allocate the arrays for faces, vertices, edges and 
  edge index.

  returns 1 on success, 0 on failure.
*/
int McdGetHullData(McdConvexHull *cnv)
{
    facetT *facet;             // required by FORALLfacets
    vertexT *vertex;           // required by FORALLvertices
    int f, e, v;

    MEASSERT(cnv && cnv->face && cnv->vertex && cnv->edge && cnv->edgeIndex);
    MEASSERT(last_cnv == cnv);
    MEASSERT(cnv->numFace == qh num_facets);
    MEASSERT(cnv->numVertex == qh num_vertices);

    if (last_cnv != cnv)
        return 0;               //  RETURN FAILURE

    //-------------------------------------------
    //  Copy the xyz coordinates of each vertex and set visitid

    v = 0;
    FORALLvertices
    {
        MeVector3Copy(cnv->vertex[v].position, vertex->point);
        vertex->visitid = v++;
    }

    //-------------------------------------------
    //  Copy the normal of each facet and set visitid

    f = 0;
    FORALLfacets 
    {
        MeVector3Copy(cnv->face[f].normal, facet->normal);
        facet->visitid = f++;
    }
    MEASSERT(f == cnv->numFace);

    //-------------------------------------------
    //  Walk clockwise around each face

    f = 0;
    e = 0;

    FORALLfacets 
    {
        qh_makeridges(facet);

        cnv->face[f].firstEdge = e;

        ridgeT *ridge, **ridgep;   // required by FOREACHridge_

        FOREACHridge_(facet->ridges)
          ridge->seen = 0;

        //-------------------------------------------------------
        //  This loops over all the edges of a face in CW order
        //  However, since we want to store the edges in ACW
        //  order, the edges are stored backwards, hence ei is
        //  decremented.

        int numedge = qh_setsize(facet->ridges);
        e += numedge;

        ridge = SETfirstt_(facet->ridges, ridgeT);
        while (ridge && !ridge->seen) 
        {
            ridge->seen = 1;
            MEASSERT(cnv->face[f].firstEdge < e);
            --e;

            //  Get the two vertices of the ridge
            MEASSERT(qh_setsize(ridge->vertices) == 2);
            int p1 = SETfirstt_(ridge->vertices, vertexT)->visitid;
            int p2 = SETsecondt_(ridge->vertices, vertexT)->visitid;
            MEASSERT(p1 >= 0 && p1 < cnv->numVertex);
            MEASSERT(p2 >= 0 && p2 < cnv->numVertex);
            MEASSERT(p1 != p2);

            //  Decide if the edge is CW or ACW
            //  "top" is the face on the right side of the edge from p1 to p2.

            if (ridge->bottom == facet)
            {
                cnv->edge[e].rightFace = ridge->top->visitid;
                cnv->edge[e].leftFace = ridge->bottom->visitid;
                cnv->edge[e].fromVert = p1;
                cnv->edge[e].toVert = p2;
            }
            else
            {
                MEASSERT(ridge->top == facet);
                cnv->edge[e].rightFace = ridge->bottom->visitid;
                cnv->edge[e].leftFace = ridge->top->visitid;
                cnv->edge[e].fromVert = p2;
                cnv->edge[e].toVert = p1;
            }

            //  If this isn't the first one, check that the end of this
            //  edge matches the start of the prior edge.

            MEASSERT(cnv->face[f].firstEdge+numedge == e+1 ||
                     cnv->edge[e].toVert == cnv->edge[e+1].fromVert);

            ridge= qh_nextridge3d (ridge, facet, NULL);
        }
        MEASSERT(cnv->face[f].firstEdge == e);
        MEASSERT(facet->visitid == (unsigned) f);
        f++;
        e += numedge;
    }
    MEASSERT(f == cnv->numFace);
    MEASSERT(e == cnv->numEdge);
    cnv->face[f].firstEdge = e;    // set the sentinel

    //-------------------------------------------
    //  Walk ACW around every vertex and sort the edges by
    //  fromVertex and ACW.

    int ei = 0;
    int start_face;

    for (v = 0; v < cnv->numVertex; ++v)
    {
        cnv->vertex[v].firstEdgeIndex = ei;
        e = McdFindEdge(cnv, -1, 0, v);      // find any edge from vertex v
        f = cnv->edge[e].rightFace;
        start_face = f;
        do
        {
            e = McdFindEdge(cnv, f, 1, v);  // find edge to vertex v on f
            f = cnv->edge[e].rightFace;     // f = next face ACW
            e = McdFindEdge(cnv, f, 0, v);  // find edge from vertex v on f
            cnv->edgeIndex[ei] = e;
            ++ei;

            //  If this isn't the first one, check that the right side of this
            //  edge matches the left side of the prior edge.

            MEASSERT(cnv->vertex[v].firstEdgeIndex == ei-1 ||
                 cnv->edge[e].rightFace == cnv->edge[cnv->edgeIndex[ei-2]].leftFace);
        }
        while (f != start_face);
    }
    MEASSERT(v == cnv->numVertex);
    MEASSERT(ei == cnv->numEdge);
    cnv->vertex[v].firstEdgeIndex = ei;      // set the sentinel

#ifdef _MECHECK
    //-------------------------------------------
    //  As an extra precaution, walk each face and make sure
    //  all the edges form a contiguous path.

    for (f = 0; f < cnv->numFace; ++f)
        for (e = cnv->face[f].firstEdge+1; e < cnv->face[f+1].firstEdge; ++e)
            MEASSERT(cnv->edge[e-1].toVert==cnv->edge[e].fromVert);
#endif

    //  Precompute the edge invLen, etc.

    McdPrecomputeStuff(cnv);

    last_cnv = 0;

    //  Deallocate QHULL working storage

    qh_freeqhull(1);
    qh_memfreeshort(&f,&v);

    //  Deallocate temp_mem if necessary

    if (temp_mem)
    {
        MEASSERT(sizeof(MeVector3) != 3*sizeof(MeReal));
        MeMemoryAPI.destroy(temp_mem);
        temp_mem = 0;
    }

    return 1;
}


/****************************************************************************
  This takes some points and returns an McdConvexHull structure.
  The memory for the convex hull data is allocated with MeMemoryAPI.create.

  returns 1 on success, or 0 on error.
*/
int McdComputeHull(McdConvexHull *cnv,
                   int numpoints, const MeVector3 *points)
{
    int ok;

    MEASSERT(cnv);

    ok = McdComputeHullSizes(cnv, numpoints, points);

    if (!ok)
        return 0;

    McdAllocateHull(cnv);

    ok = McdGetHullData(cnv);

    return ok;
}

/****************************************************************************
  This populates a triangle convex hull.  It must have 3 vert, 1 face, and
  3 edges.  The points p1,p2,p3 must be ACW.
*/
void McdGetTriangleHull(McdConvexHull *cnv, const MeVector3 p1, 
                        const MeVector3 p2, const MeVector3 p3)
{
    MEASSERT(cnv && cnv->face && cnv->vertex && cnv->edge && cnv->edgeIndex);
    MEASSERT(cnv->numFace==1 && 
             cnv->numVertex==3 && 
             cnv->numEdge==3);

    //  Copy points to vertex
    MeVector3Copy(cnv->vertex[0].position, p1);
    MeVector3Copy(cnv->vertex[1].position, p2);
    MeVector3Copy(cnv->vertex[2].position, p3);

    //  Build face
    MeVector3 e1, e2;
    MeVector3Subtract(e1, p1, p2);
    MeVector3Subtract(e2, p1, p3);
    MeVector3Cross(cnv->face[0].normal, e1, e2);
    cnv->face[0].firstEdge = 0;
    cnv->face[1].firstEdge = 3;

    //  Build edges
    cnv->edge[0].fromVert = 0;
    cnv->edge[0].toVert = 1;
    cnv->edge[0].rightFace = 0;
    cnv->edge[0].leftFace = 0;
    cnv->edge[1].fromVert = 1;
    cnv->edge[1].toVert = 2;
    cnv->edge[1].rightFace = 0;
    cnv->edge[1].leftFace = 0;
    cnv->edge[2].fromVert = 2;
    cnv->edge[2].toVert = 0;
    cnv->edge[2].rightFace = 0;
    cnv->edge[2].leftFace = 0;

    //  Build edge index
    cnv->edgeIndex[0] = 0;
    cnv->edgeIndex[1] = 1;
    cnv->edgeIndex[2] = 2;
    cnv->vertex[0].firstEdgeIndex = 0;
    cnv->vertex[1].firstEdgeIndex = 1;
    cnv->vertex[2].firstEdgeIndex = 2;
    cnv->vertex[3].firstEdgeIndex = 3;
}

/*
    =======================================================================
        ACCESSOR METHODS
    =======================================================================
*/

/****************************************************************************
  Get the number of edges (or vertices or adjacent faces) around a given face.
*/
int McdCnvFaceGetCount(const McdConvexHull *cnv, int face)
{
    MEASSERT(cnv && face>=0 && face < cnv->numFace);

    return cnv->face[face+1].firstEdge - cnv->face[face].firstEdge;
}

/****************************************************************************
  Get the i-th edge of a face.  The edges are guaranteed to form a directed 
  path (from-to) in ACW order around the face. 
  You can look at the edge.rightFace to get all adjacent faces.
*/
const McdCnvEdge *McdCnvFaceGetEdge(const McdConvexHull *cnv, int face, int i)
{
    MEASSERT(i >= 0 && i < McdCnvFaceGetCount(cnv, face));

    return cnv->edge + (i + cnv->face[face].firstEdge);
}

/****************************************************************************
  These three functions get the i-th vertex on the face.
*/
int McdCnvFaceGetVertexId(const McdConvexHull *cnv, int face, int i)
{
    return McdCnvFaceGetEdge(cnv, face, i)->fromVert;
}

const McdCnvVertex *McdCnvFaceGetVertex(const McdConvexHull *cnv,
    int face, int i)
{
    return cnv->vertex + McdCnvFaceGetVertexId(cnv, face, i);
}

const MeReal *McdCnvFaceGetVertexPosition(const McdConvexHull *cnv,
    int face, int i)
{
    return McdCnvFaceGetVertex(cnv, face, i)->position;
}


/****************************************************************************
  Get the number of edges (or vertices or faces) around a given vertex.
*/
int McdCnvVertexGetCount(const McdConvexHull *cnv, int vertex)
{
    MEASSERT(cnv && vertex>=0 && vertex < cnv->numVertex);

    return cnv->vertex[vertex+1].firstEdgeIndex - cnv->vertex[vertex].firstEdgeIndex;
}

/****************************************************************************
  Get the id of the i-th edge around a vertex.
*/
int McdCnvVertexGetEdgeId(const McdConvexHull *cnv, int vertex, int i)
{
    MEASSERT(i >= 0 && i < McdCnvVertexGetCount(cnv, vertex));

    return cnv->edgeIndex[i + cnv->vertex[vertex].firstEdgeIndex];
}

/****************************************************************************
  Get the i-th edge from a vertex.    The edges are guaranteed
  to be all "from" the specified vertex "to" another vertex.
  You can look at the edge.rightFace to get all adjacent faces.
*/
const McdCnvEdge *McdCnvVertexGetEdge(const McdConvexHull *cnv, int vertex, int i)
{
    return cnv->edge + McdCnvVertexGetEdgeId(cnv, vertex, i);
}

/****************************************************************************
  Get the id of the i-th vertex that is adjacent to a given vertex.
*/
int McdCnvVertexGetNeighbor(const McdConvexHull *cnv, int vertex, int i)
{
    return cnv->edge[McdCnvVertexGetEdgeId(cnv, vertex, i)].toVert;
}

/****************************************************************************
  Functions are provided to import/export hull data to an MeStream in XML.
  Below is the typical format of the data.

    <CONVEXHULL numvertex=10 numface=3 numedge=35>
    <VERTEX>1.23,1.23,1.23,3</VERTEX>
    <FACE>1.23,1.23,1.23,12</FACE>
    <EDGE>2,3,5,2</EDGE>
    </CONVEXHULL>

  If the edge/face data is not known, the following format is also acceptable.

    <CONVEXHULL numvertex=10>
    <VERTEX>123,123,123</VERTEX>
    </CONVEXHULL>

*/
/*********

      NOT IMPLEMENTED YET !!

void McdConvexHullWriteXML(const McdConvexHull *cnv, int withTag, MeStream *s)
{
    int meout = MeXMLOutputCreate(s);

    MeXMLWriteElement( ...

}

void McdConvexHullReadXML(const McdConvexHull *cnv, int withTag, MeStream *s)
{


}

*********/


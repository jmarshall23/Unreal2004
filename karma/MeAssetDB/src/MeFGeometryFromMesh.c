/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:05 $ - Revision: $Revision: 1.17.2.3 $

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
#include <string.h>
#include <MeAssetDB.h>
#include <MeASELoad.h>
#include <MeMessage.h>
#include <MeMath.h>
#include <MeMemory.h>

typedef enum
{
    kMesh2GeometrySphere,
    kMesh2GeometryBox,
    kMesh2GeometryCylinder,
    kMesh2GeometryConvex,
    kMesh2GeometryUnknown        
} Mesh2GeometryType;

static Mesh2GeometryType MEAPI name2Type(char *const name)
{
    Mesh2GeometryType type = kMesh2GeometryUnknown;
    char tmpName[256];
    
    /* Copy and push to upper case for case-insensitive match. */
    strncpy(tmpName, name, 256);
    strupr(tmpName);

    if(strncmp(tmpName, "MCDSP", 5) == 0)
        type = kMesh2GeometrySphere;
    else if(strncmp(tmpName, "MCDBX", 5) == 0)
        type = kMesh2GeometryBox;
    else if(strncmp(tmpName, "MCDCY", 5) == 0)
        type = kMesh2GeometryCylinder;
    else if(strncmp(tmpName, "MCDCX", 5) == 0)
        type = kMesh2GeometryConvex;

    return type;
}

static void MEAPI type2Name(Mesh2GeometryType type, char name[256])
{
    if(type == kMesh2GeometrySphere)
        strncpy(name, "McdSphere", 256);
    else if(type == kMesh2GeometryBox)
        strncpy(name, "McdBox", 256);
    else if(type == kMesh2GeometryCylinder)
        strncpy(name, "McdCylinder", 256);
    else if(type == kMesh2GeometryConvex)
        strncpy(name, "McdConvexHull", 256);
    else if(type == kMesh2GeometryUnknown)
        strncpy(name, "Unknown", 256);
}

MeFGeometry *MEAPI MeFGeometryCreateFromASE(const char *const name, char *const file, MeReal xScale, MeReal yScale, MeReal zScale)
{
    MeFGeometry *ag;
    MeASEObject *aseObject, *tmpAse, *headObject;
    MyMesh mesh;
    int i, maxFaces, maxGeom;
    MeBool bFlipX = 0;
    mesh.faceVertex = 0;

    ag = MeFGeometryCreate(name);

    aseObject = MeASEObjectLoadParts(file, xScale, yScale, zScale, 1);
    headObject = aseObject;
    
    if (!aseObject)
    {
        ME_REPORT(MeWarning(3,"MeASEObjectLoadParts() failed."));
        return 0;
    }
    
    /* Work out how big our temporary mesh struct has to be and allocate. */
    tmpAse = aseObject;
    maxFaces = 0;
    maxGeom = 0;
    
    while(tmpAse)
    {
        maxFaces = MeMAX(maxFaces, tmpAse->numFaces);
        maxGeom++;
        tmpAse = tmpAse->nextObject;
    }
    mesh.faceVertex = (int(*)[3])MeMemoryALLOCA(maxFaces * 3 * sizeof(int)); 

    /* While there are bits of geometry to convert. */
    while(aseObject)
    {
        Mesh2GeometryType type;
        MeMatrix4 relTM;
        MeFPrimitive* partGeometry = 0;
        
        /* Convert ASE into generic mesh format for functions. */
        mesh.numFaces = aseObject->numFaces;
        mesh.numVerts = aseObject->numVerts;
        mesh.verts = aseObject->verts;

        if( bFlipX )
            for(i=0; i<mesh.numVerts; i++)
                (*(mesh.verts+i))[0] *= -1;
        
        MEASSERT(mesh.numFaces <= maxFaces);
        
        for(i=0; i<mesh.numFaces; i++)
        {
            mesh.faceVertex[i][0] = aseObject->faces[i].vertexId[0];
            mesh.faceVertex[i][1] = aseObject->faces[i].vertexId[1];
            mesh.faceVertex[i][2] = aseObject->faces[i].vertexId[2];        
        }
        
        /* See if the name hints at what kind of shape it is... */
        type = name2Type(aseObject->name);
        
        /* If the name is not a geometry type, move on.. */
        if(type == kMesh2GeometryUnknown)
        {
            ME_REPORT(MeWarning(3,"Ignoring unknown geometry %s", aseObject->name));
        }
        else
        {
            char typeName[256];
            type2Name(type, typeName);

            if(type == kMesh2GeometrySphere)
                partGeometry = MeFSphereCreateFromMesh(aseObject->name, &mesh, relTM);
            else if(type == kMesh2GeometryBox)
                partGeometry = MeFBoxCreateFromMesh(aseObject->name, &mesh, relTM);
            else if(type == kMesh2GeometryCylinder)    
                partGeometry = MeFCylinderCreateFromMesh(aseObject->name, &mesh, relTM);
            else if(type == kMesh2GeometryConvex)
                partGeometry = MeFConvexCreateFromMesh(aseObject->name, &mesh, relTM);
            
            if(!partGeometry)
            {
                ME_REPORT(MeWarning(3,"ERROR: Converting Part: %s (Geometry Type: %s).",
                    aseObject->name, typeName));
            }
            else
            {
                MeFPrimitiveSetTransform(partGeometry, relTM);
                MeFGeometryInsertPrimitive(ag, partGeometry);
            }
        }
        
        aseObject = aseObject->nextObject;
    }    

    if(headObject)
        MeASEObjectDestroy(headObject);
    
    if(mesh.faceVertex)
        MeMemoryFREEA(mesh.faceVertex);

    return ag;
}

/* returns 1 if vectors are parallel OR anti-parallel */
static MeBool AreParallel(MeVector3 a, MeVector3 b)
{
    MeReal dot = MeVector3Dot(a, b);
    
    if(ME_ARE_EQUAL_TOL(dot, 1, ME_MEDIUM_EPSILON) || 
        ME_ARE_EQUAL_TOL(dot, -1, ME_MEDIUM_EPSILON))
        return 1;
    else
        return 0;
}

/*  Simple Axis-Aligned Bounding-Box generator.
    Also gives centre and size. */
void MEAPI MeMesh2AABB(const MyMesh *const mesh, MyAABB *const bbox)
{
    int i, j;

    MeVector3Copy(bbox->min, mesh->verts[0]);
    MeVector3Copy(bbox->max, mesh->verts[0]);
    
    for(i=1; i<mesh->numVerts; i++)
    {
        for(j=0; j<3; j++)
        {
            bbox->max[j] = MeMAX(mesh->verts[i][j], bbox->max[j]);
            bbox->min[j] = MeMIN(mesh->verts[i][j], bbox->min[j]);
        }
    }

    /* edge lengths */
    MeVector3Subtract(bbox->size, bbox->max, bbox->min);

    /* box centre */
    bbox->centre[0] = bbox->min[0] + (MeReal)0.5 * bbox->size[0];
    bbox->centre[1] = bbox->min[1] + (MeReal)0.5 * bbox->size[1];
    bbox->centre[2] = bbox->min[2] + (MeReal)0.5 * bbox->size[2];
}

/*
    How this works:
    Simply put an AABB around mesh and use that to generate centre and radius.
    It checks that the AABB is square, and that all vertices are either at the
    centre, or within 5% of the radius distance away.
*/
MeFPrimitive* MEAPI MeFSphereCreateFromMesh(const char *const name, const MyMesh *const mesh, const MeMatrix4Ptr relTM)
{
    MyAABB bbox;
    MeReal longest, shortest, maxr, minr, radius;
    int i;
    MeFPrimitive* fgeom = 0;

    if(mesh->numVerts == 0)
        return 0;

    MeMesh2AABB(mesh, &bbox);

    longest = MeMAX3(bbox.size[0], bbox.size[1], bbox.size[2]);
    shortest = MeMIN3(bbox.size[0], bbox.size[1], bbox.size[2]);
    
    /* check that the AABB is roughly a square (5% tolerance) */
    if((longest - shortest)/longest > (MeReal)0.05)
    {
        MeWarning(0, "ERROR: Sphere bounding box not square.");
        goto end;
    }
    
    MeMatrix4TMMakeIdentity(relTM);
    MeVector3Copy(relTM[3], bbox.centre);
    
    radius = (MeReal)0.5 * longest;

    /* Test that all vertices are a similar radius (5%) from the sphere centre. */
    maxr = 0;
    minr = MEINFINITY;
    for(i=0; i<mesh->numVerts; i++)
    {
        MeVector3 ctov;
        MeReal rsqr;

        MeVector3Subtract(ctov, mesh->verts[i], bbox.centre);
        rsqr = MeVector3MagnitudeSqr(ctov);

        maxr = MeMAX(rsqr, maxr);

        /* Sometimes vertex at centre, so reject it. */
        if(rsqr > ME_MEDIUM_EPSILON)
            minr = MeMIN(rsqr, minr);
    }

    maxr = MeSqrt(maxr);
    minr = MeSqrt(minr);
    
    if((maxr-minr)/radius > (MeReal)0.05)
    {
        MeWarning(0, "ERROR: Vertices not at constant radius.");
        goto end;
    }

    fgeom = MeFPrimitiveCreate(name, kMeFPrimitiveTypeSphere);
    if( fgeom )
        MeFPrimitiveSetRadius(fgeom, radius);
end:
    return fgeom;
}



/* 
    How this works:
    We keep a list of triangle normals found so far. For each normal direction,
    we should have 2 distances from the origin (2 parallel box faces). If the 
    mesh is a box, we should have 3 distinct normal directions, and 2 distances
    found for each. The difference between these distances should be the box
    dimensions. The 3 directions give us the key axes, and therefore the
    box transformation matrix. This shouldn't rely on any vertex-ordering on 
    the triangles (normals are compared +ve & -ve). It also shouldn't matter 
    about how many triangles make up each side (but it will take longer). 
    We get the centre of the box from the centre of its AABB.
*/
MeFPrimitive* MEAPI MeFBoxCreateFromMesh(const char *const name, const MyMesh *const mesh, const MeMatrix4Ptr relTM)
{
    /*  We have a list of found normals, and for each there should be 2 
        distances from the origin. */
    MeVector3 *planeNormal = MeMemoryALLOCA(mesh->numFaces * sizeof(MeVector3));
    MeReal (*planeDist)[2] = MeMemoryALLOCA(mesh->numFaces * 2 * sizeof(MeReal));
    int *planeDistCount = MeMemoryALLOCA(mesh->numFaces * sizeof(int));
    int numPlanes = 0, i, j;
    MyAABB bbox;
    MeVector3 size;

    MeFPrimitive* boxgeom = 0;


    for(i=0; i<mesh->numFaces; i++)
    {
        MeVector3 e1, e2, normal;
        MeBool foundPlane = 0;

        /* find triangle normal */
        MeVector3Subtract(e1, 
            mesh->verts[mesh->faceVertex[i][1]], 
            mesh->verts[mesh->faceVertex[i][0]]);
        MeVector3Subtract(e2, 
            mesh->verts[mesh->faceVertex[i][2]], 
            mesh->verts[mesh->faceVertex[i][0]]);
        MeVector3Cross(normal, e1, e2);
        MeVector3Normalize(normal);

        for(j=0; j<numPlanes && !foundPlane; j++)
        {
            /* if this triangle plane is already known... */
            if(AreParallel(normal, planeNormal[j]))
            {
                /* Always use the same normal when comparing distances, 
                    to ensure consistent sign. */
                MeReal dist = MeVector3Dot(mesh->verts[mesh->faceVertex[i][0]],
                    planeNormal[j]);

                foundPlane = 1;

                /* we only have one distance, and its not that one, add it. */
                if(planeDistCount[j] == 1 && 
                    !ME_ARE_EQUAL_TOL(dist, planeDist[j][0], ME_MEDIUM_EPSILON))
                {
                    planeDist[j][1] = dist;
                    planeDistCount[j] += 1;
                }
                /* if we have a second distance, and its not that either,
                    something is wrong. */
                else if(planeDistCount[j] == 2 && 
                    !ME_ARE_EQUAL_TOL(dist, planeDist[j][1], ME_MEDIUM_EPSILON))
                {
                    MeWarning(0, "ERROR: Found more than 2 planes with different distances.");
                    goto end;
                }
            }
        }

        /* If this triangle does not match an existing plane, add to list. */
        if(!foundPlane)
        {
            MEASSERT(numPlanes<mesh->numFaces);

            MeVector3Copy(planeNormal[numPlanes], normal);
            planeDist[numPlanes][0] = MeVector3Dot(mesh->verts[mesh->faceVertex[i][0]],
                planeNormal[numPlanes]);
            planeDistCount[numPlanes] = 1;
            numPlanes++;
        }
    }

    /* Wrong number of planes. */
    if(numPlanes != 3)
    {
        MeWarning(0, "ERROR: Not very box-like (need 3 sets of planes).");
        goto end;
    }

    /* If we dont have 3 pairs, we can't carry on. */
    if((planeDistCount[0] != 2) || (planeDistCount[1] != 2) || (planeDistCount[2] != 2))
    {
        MeWarning(0, "ERROR: Incomplete set of planes (need 2 per axis).");
        goto end;
    }

    MeMatrix4TMMakeIdentity(relTM);
    MeVector3Copy(relTM[0], planeNormal[0]);
    MeVector3Copy(relTM[1], planeNormal[1]);

    /* ensure valid TM by cross-product */
    MeVector3Cross(relTM[2], relTM[0], relTM[1]);

    if(!MeMatrix4IsTM(relTM, ME_MEDIUM_EPSILON) || !AreParallel(relTM[2], planeNormal[2]))
    {
        MeWarning(0, "ERROR: Box axes are not perpendicular.");
        goto end;
    }

    /* OBB centre == AABB centre. */
    MeMesh2AABB(mesh, &bbox);
    MeVector3Copy(relTM[3], bbox.centre);

    /* distance between parallel planes is box edge lengths */
    size[0] = MeFabs(planeDist[0][0] - planeDist[0][1]);
    size[1] = MeFabs(planeDist[1][0] - planeDist[1][1]);
    size[2] = MeFabs(planeDist[2][0] - planeDist[2][1]);
  
    boxgeom = MeFPrimitiveCreate(name, kMeFPrimitiveTypeBox);
    if( boxgeom )
        MeFPrimitiveSetDimensions(boxgeom, size[0], size[1], size[2]);
    
end:
    MeMemoryFREEA(planeNormal);
    MeMemoryFREEA(planeDist);
    MeMemoryFREEA(planeDistCount);
    return boxgeom;
}

/*  
    How this works:
    To find the axis, we look for the most commonly occuring triangle edge
    direction. It checks this is more common (not equally common) than 2nd 
    most common direction (rejects boxes). Then we find the AABB to give the
    cylinder centre. Then project vertices onto axis to find max and min 
    vertices for the ends (should be symmetric) to give length. Also find max 
    vertex distance from axis to give radius. As a check, all vertices should
    wither be more than 95% of the radius from the axis, or on the axis. So
    extra vertices on ends will cause it to fail!
*/

MeFPrimitive* MEAPI MeFCylinderCreateFromMesh(const char *const name, const MyMesh *const mesh, const MeMatrix4Ptr relTM)
{
    MeVector3 *dir = MeMemoryALLOCA(mesh->numFaces * 3 * sizeof(MeVector3));
    int *dirCount = MeMemoryALLOCA(mesh->numFaces * 3 * sizeof(int));
    int numDirs = 0;
    MeFPrimitive* cylgeom = 0;
    MyAABB bbox;
    int i, j, k, mostCommonCount, nextMostCommonCount, axisIx;
    MeVector3 cylAxis;
    MeReal maxr, minr, maxlen, minlen;

    /* For every triangle. */
    for(i=0; i<mesh->numFaces; i++)
    {
        MeVector3 edge[3];

        /*  For each edge of the triangle, find direction vector.
            This is a lot of sqrts... hmm..
        */
        MeVector3Subtract(edge[0], 
            mesh->verts[mesh->faceVertex[i][1]], 
            mesh->verts[mesh->faceVertex[i][0]]);
        MeVector3Normalize(edge[0]);

        MeVector3Subtract(edge[1], 
            mesh->verts[mesh->faceVertex[i][2]], 
            mesh->verts[mesh->faceVertex[i][0]]);
        MeVector3Normalize(edge[1]);
        
        MeVector3Subtract(edge[2], 
            mesh->verts[mesh->faceVertex[i][2]], 
            mesh->verts[mesh->faceVertex[i][1]]);
        MeVector3Normalize(edge[2]);
        
        for(j=0; j<3; j++)
        {
            MeBool dirFound = 0;
            /* See if its direction is already known.. */
            for(k=0; k<numDirs && !dirFound; k++)
            {
                if(AreParallel(edge[j], dir[k]))
                {
                    dirCount[k] += 1;
                    dirFound = 1;
                }
            }

            /* If we didn't find it, add it. */
            if(!dirFound)
            {
                MEASSERT(numDirs < mesh->numFaces * 3);

                MeVector3Copy(dir[numDirs], edge[j]);
                dirCount[numDirs] = 1;
                numDirs++;
            }
        }
    }

    /* Now find most common edge direction. */
    mostCommonCount = 0;
    axisIx = -1;
    for(k=0; k<numDirs; k++)
    {
        if(dirCount[k] > mostCommonCount)
        {
            axisIx = k;
            MeVector3Copy(cylAxis, dir[axisIx]);
            mostCommonCount = dirCount[axisIx];
        }
    }
    
    /* HACKY! We check that the most common direction was at least twice as 
        common as the next most common. */
    nextMostCommonCount = 0;
    for(k=0; k<numDirs; k++)
    {
        if(dirCount[k] > nextMostCommonCount && k != axisIx)
            nextMostCommonCount = dirCount[k];
    }

    /* most common direction MUST be more common than 2nd most common
        (rejects boxes). */
    if(!(mostCommonCount > nextMostCommonCount))
    {
        MeWarning(0, "ERROR: Can't find definitive cylinder axis.");
        goto end;
    }

    /* Now we find centre of cylinder = centre of AABB! */
    MeMesh2AABB(mesh, &bbox);

    MeMatrix4TMMakeIdentity(relTM);
    MeVector3Copy(relTM[3], bbox.centre);

    /* cylinder aligned along z axis (generate other rows) */
    MeVector3Copy(relTM[2], cylAxis);

    MeVector3PlaneSpace(relTM[2], relTM[0], relTM[1]);

    MEASSERT(MeMatrix4IsTM(relTM, ME_MEDIUM_EPSILON));

    
    /* Now iterate over vertices to find radius/length. */
    maxr = 0;
    minr = MEINFINITY;
    maxlen = 0;
    minlen = 0;
    for(i=0; i<mesh->numVerts; i++)
    {
        MeVector3 relOrigin, toClosest;
        MeReal rsqr, alongAxis;

        /* Put cylinder axis through origin. */
        MeVector3Subtract(relOrigin, mesh->verts[i], relTM[3]);

        /* Project vertex onto cylinder axis. */
        alongAxis = MeVector3Dot(relOrigin, cylAxis);
        minlen = MeMIN(alongAxis, minlen);
        maxlen = MeMAX(alongAxis, maxlen);

        /* Find vector from vertex to closest point on cylinder axis. */
        MeVector3Copy(toClosest, cylAxis);
        MeVector3Scale(toClosest, alongAxis);
        MeVector3Subtract(toClosest, relOrigin, toClosest);

        /* Use square of radius until the end. */
        rsqr = MeVector3MagnitudeSqr(toClosest);
        
        maxr = MeMAX(rsqr, maxr);

        /* Sometimes vertex at centre of ends- ignore it.*/
        if(rsqr > ME_MEDIUM_EPSILON)
            minr = MeMIN(rsqr, minr);
    }

    maxr = MeSqrt(maxr);
    minr = MeSqrt(minr);

    /* Check its symmetric about centre */
    if(!ME_ARE_EQUAL_TOL(maxlen, -minlen, ME_MEDIUM_EPSILON))
    {
        MeWarning(0, "ERROR: Cylinder ends are not symmetric.");
        goto end;
    }

    /* Check vertices are at constant distance from cylinder axis. */
    if((maxr-minr)/maxr > (MeReal)0.05)
    {
        MeWarning(0, "ERROR: Vertices not at constant radius.");
        goto end;
    }

    cylgeom = MeFPrimitiveCreate(name, kMeFPrimitiveTypeCylinder);
    if( cylgeom )
    {
        MeFPrimitiveSetHeight(cylgeom, maxlen-minlen);
        MeFPrimitiveSetRadius(cylgeom, maxr);
    }
    
end:
    MeMemoryFREEA(dir);
    MeMemoryFREEA(dirCount);    
    return cylgeom;
}

/* 
    Find a point inside the convex hull of the supplied mesh. 
    We find a tetrahedron, and pick a point inside.
*/
static int MEAPI PointInsideConvex(const MyMesh *const mesh, MeVector3 inside)
{
    int tetra4;
    MeBool haveTetra = 0;
    MeVector3 e1, e2, faceNormal;

    /* First three mesh points form parts of the tetra. */
    
    /* Calculate normal of this tetrahedron face. */
    MeVector3Subtract(e1, mesh->verts[1], mesh->verts[0]);
    MeVector3Subtract(e2, mesh->verts[2], mesh->verts[0]);
    MeVector3Cross(faceNormal, e1, e2);
    MeVector3Normalize(faceNormal);

    /* Look for a 4th tetrahedron vertex (ie. not coplanar with first 3) */
    for(tetra4=3; tetra4<mesh->numVerts && !haveTetra; tetra4++)
    {
        MeVector3 vertRel;
        MeReal fromFace;

        MeVector3Subtract(vertRel, mesh->verts[tetra4], mesh->verts[0]);
        fromFace = MeVector3Dot(vertRel, faceNormal);

        /* If this point is away from the face, we have a tetrahedron. */
        if(MeFabs(fromFace) > ME_MEDIUM_EPSILON)
            haveTetra = 1;
    }

    if(!haveTetra)
    {
        MeWarning(0, "PointInsideConvex: Mesh seem to be flat.");
        return 0;
    }

    /* Now take point inside tetrahedron */
    MeVectorSetZero(inside, 3);
    MeVector3Add(inside, inside, mesh->verts[0]);
    MeVector3Add(inside, inside, mesh->verts[1]);
    MeVector3Add(inside, inside, mesh->verts[2]);    
    MeVector3Add(inside, inside, mesh->verts[tetra4]);    
    MeVector3Scale(inside, (MeReal)0.25);

    return 1;
}

/*
    What it does:
    Rather than create a convex mesh with no transform, where all the vertices 
    are miles from the origin, we find a point inside the convexd, and 
    use that as its new origin.
*/
MeFPrimitive* MEAPI MeFConvexCreateFromMesh(const char *const name, const MyMesh *const mesh, const MeMatrix4Ptr relTM) 
{
    MeFPrimitive* convexGeom = 0;
    MyMesh localMesh;
    MeVector3 newOrigin;
    int i, isok;

    /* Find a point inside the convex. */
    isok = PointInsideConvex(mesh, newOrigin);

    if(!isok)
        return 0;

    /* Now we have a new origin, translate vertices to be relative to it. */
    localMesh.faceVertex = mesh->faceVertex;
    localMesh.numFaces = mesh->numFaces;
    localMesh.numVerts = mesh->numVerts;
    localMesh.verts = MeMemoryALLOCA(localMesh.numVerts * sizeof(MeVector3));

    for(i=0; i<localMesh.numVerts; i++)
    {
        localMesh.verts[i][0] = mesh->verts[i][0] - newOrigin[0];
        localMesh.verts[i][1] = mesh->verts[i][1] - newOrigin[1];
        localMesh.verts[i][2] = mesh->verts[i][2] - newOrigin[2];        
    }

    /* Put offset matrix together. */
    MeMatrix4TMMakeIdentity(relTM);
    relTM[3][0] = newOrigin[0];
    relTM[3][1] = newOrigin[1];
    relTM[3][2] = newOrigin[2];    
    
    /* Then make the convex. */
    convexGeom = MeFPrimitiveCreate(name, kMeFPrimitiveTypeConvex);
    if( convexGeom )
    {
        for(i=0; i<localMesh.numVerts; i++)
            MeFPrimitiveAddVertex(convexGeom, localMesh.verts[i]);
    }    
    
    MeMemoryFREEA(localMesh.verts);

    return convexGeom;
}

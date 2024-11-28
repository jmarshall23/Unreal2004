/* 
   sphereSoup produces 
   a sphere with 2^(2*level+1) triangles.

serveSphereSoup( lsPrimitiveSoup* soup, MeReal radius, 
       int maxlevel, int ccwflag)

*/

/*% cc -g sphere.c -o sphere -lm
 *
 * sphere - generate a triangle mesh approximating a sphere by
 *  recursive subdivision. First approximation is an octahedron;
 *  each level of refinement increases the number of triangles by
 *  a factor of 4.
 * Level 3 (128 triangles) is a good tradeoff if gouraud
 *  shading is used to render the database.
 *
 * Usage: sphere [level] [-p] [-c]
 *	level is an integer >= 1 setting the recursion level (default 1).
 *	-p causes generation of a PPHIGS format ASCII archive
 *	    instead of the default generic output format.
 *	-c causes triangles to be generated with vertices in counterclockwise
 *	    order as viewed from the outside in a RHS coordinate system.
 *	    The default is clockwise order.
 *
 *  The subroutines print_object() and print_triangle() should
 *  be changed to generate whatever the desired database format is.
 *
 * Jon Leech (leech@cs.unc.edu) 3/24/89
 */
#include <stdio.h>
#include <math.h>

#include <McdTriangleMesh.h>
#include <McdCompatibility.h>

typedef struct {
    MeReal  x, y, z;
} point;

typedef struct {
    point     pt[3];	/* Vertices of triangle */
    MeReal    area;	/* Unused; might be used for adaptive subdivision */
} triangle;

typedef struct {
    int       npoly;	/* # of triangles in object */
    triangle *poly;	/* Triangles */
} object;

/* Six equidistant points lying on the unit sphere */
#define XPLUS {  1,  0,  0 }	/*  X */
#define XMIN  { -1,  0,  0 }	/* -X */
#define YPLUS {  0,  1,  0 }	/*  Y */
#define YMIN  {  0, -1,  0 }	/* -Y */
#define ZPLUS {  0,  0,  1 }	/*  Z */
#define ZMIN  {  0,  0, -1 }	/* -Z */


point *normalize_pt( point *p );
point *midpoint( point *a, point *b );
void print_object( object *obj, int level );
void print_triangle( triangle *t );


McdTriangleMeshID
makeMeshSphere(McdFramework *frame, MeReal radius, int maxlevel, int ccwflag, void **data, McdTrianglesMeshOptions dist )
{
    object *old, *newo;
    /*int     ccwflag = 0*/	  /* Reverse vertex order if true */
    int i, level;		          /* Current subdivision level */

    /* McdTriangleMeshID mesh; */
    McdTriangleMeshID mesh = 0;

/* Vertices of a unit octahedron */
    triangle octahedron[] = {
	{ { XPLUS, ZPLUS, YPLUS }, 0.0 },
	{ { YPLUS, ZPLUS, XMIN  }, 0.0 },
	{ { XMIN , ZPLUS, YMIN  }, 0.0 },
	{ { YMIN , ZPLUS, XPLUS }, 0.0 },
	{ { XPLUS, YPLUS, ZMIN  }, 0.0 },
	{ { YPLUS, XMIN , ZMIN  }, 0.0 },
	{ { XMIN , YMIN , ZMIN  }, 0.0 },
	{ { YMIN , XPLUS, ZMIN  }, 0.0 }
    };

/* A unit octahedron */
/*
  object octa = {
  sizeof(octahedron) / sizeof(octahedron[0]),
  &octahedron[0]
  };
*/
    object octa; 

    triangle *tri = &(octahedron[0]);    
    octa.npoly = sizeof(octahedron) / sizeof(octahedron[0]),
	octa.poly = tri;
 

    if (ccwflag) {
	/* Reverse order of points in each triangle */
	for (i = 0; i < octa.npoly; i++) {
	    point tmp;
	    tmp = octa.poly[i].pt[0];
	    octa.poly[i].pt[0] = octa.poly[i].pt[2];
	    octa.poly[i].pt[2] = tmp;
	}
    }

    old = &octa;

    /* Subdivide each starting triangle (maxlevel - 1) times */
    for (level = 1; level < maxlevel; level++) {

	/* Allocate a new object */	
	newo = (object *)MeMemoryAPI.create(sizeof(object));

	if (newo == NULL) {
	    fprintf(stderr, "Out of memory on subdivision level %d\n", level);
	    return 0;
	}
	newo->npoly = old->npoly * 4;

	/* Allocate 4* the number of points in the current approximation */	     
	newo->poly  = (triangle *)MeMemoryAPI.create(newo->npoly * sizeof(triangle));
	if (newo->poly == NULL) {
	    fprintf(stderr, "Out of memory on subdivision level %d\n", level);
	    return 0;
	}

	/* Subdivide each triangle in the old approximation and normalize
	 *  the new points thus generated to lie on the surface of the unit
	 *  sphere.
	 * Each input triangle with vertices labelled [0,1,2] as shown
	 *  below will be turned into four new triangles:
	 *
	 *			Make new points
	 *			    a = (0+2)/2
	 *			    b = (0+1)/2
	 *			    c = (1+2)/2
	 *	     1
	 *	     /\		Normalize a, b, c
	 *	    /  \
	 *    b/____\c     	Construct new triangles
	 *    /\    /\	    [0,b,a]
	 *   /  \  /  \   	    [b,1,c]
	 *  /____\/____\	    [a,b,c]
	 * 0      a	    2	    [a,c,2]
	 */
	for (i = 0; i < old->npoly; i++) {
	    triangle *oldt = &old->poly[i],
		*newt = &newo->poly[i*4];
	    point a, b, c;

	    a = *normalize_pt(midpoint(&oldt->pt[0], &oldt->pt[2]));
	    b = *normalize_pt(midpoint(&oldt->pt[0], &oldt->pt[1]));
	    c = *normalize_pt(midpoint(&oldt->pt[1], &oldt->pt[2]));

	    newt->pt[0] = oldt->pt[0];
	    newt->pt[1] = b;
	    newt->pt[2] = a;
	    newt++;

	    newt->pt[0] = b;
	    newt->pt[1] = oldt->pt[1];
	    newt->pt[2] = c;
	    newt++;

	    newt->pt[0] = a;
	    newt->pt[1] = b;
	    newt->pt[2] = c;
	    newt++;

	    newt->pt[0] = a;
	    newt->pt[1] = c;
	    newt->pt[2] = oldt->pt[2];
	}

	if (level > 1) {
	    MeMemoryAPI.destroy(old->poly);
	    MeMemoryAPI.destroy(old);
	}

	/* Continue subdividing new triangles */
	old = newo;
    }

  if (dist > 0)
    mesh = McdTriangleMeshCreateWithOptions(frame,old->npoly,dist);
  else
    mesh = McdTriangleMeshCreate(old->npoly);  
     
    /* output resulting approximation */
    for (i = 0; i < old->npoly; i++) {
	triangle *t = &old->poly[i];
	t->pt[0].x *= radius;
	t->pt[0].y *= radius;
	t->pt[0].z *= radius;
	t->pt[1].x *= radius;
	t->pt[1].y *= radius;
	t->pt[1].z *= radius;
	t->pt[2].x *= radius;
	t->pt[2].y *= radius;
	t->pt[2].z *= radius;

	McdTriangleMeshAddTriangle(mesh,&(t->pt[0].x),&(t->pt[1].x),&(t->pt[2].x));
    }

    McdTriangleMeshBuild(mesh);     
    *data = (void*) (newo->poly);
    MeMemoryAPI.destroy(newo);

    return mesh;
}

/* Normalize a point p */
point *normalize_pt(point *p)
{
    static point r;
    MeReal mag;

    r = *p;
    mag = r.x * r.x + r.y * r.y + r.z * r.z;
    if (mag != 0.0) {
	    mag = (MeReal)1.0 / (MeReal)sqrt(mag);
	    r.x *= mag;
	    r.y *= mag;
	    r.z *= mag;
    }

    return &r;
}

/* Return the midpoint on the line between two points */
point *midpoint(point *a, point *b)
{
    static point r;

    r.x = (a->x + b->x) * (MeReal)0.5;
    r.y = (a->y + b->y) * (MeReal)0.5;
    r.z = (a->z + b->z) * (MeReal)0.5;

    return &r;
}


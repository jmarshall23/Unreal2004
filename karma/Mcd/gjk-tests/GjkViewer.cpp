/* -*- mode: C; -*- */

/****************************************************************************
  GJK-Viewer.cpp

  By John Henckel Dec 2001

  This program is based on the "Primitives.c" collision tester written
  by Dilip.  This does not use any dynamics.  This calls the collision 
  detection nearfield tests directly.  

  The main program overrides the default intersection functions so
  that GJK is used for ALL collisions.

  There are three "run" modes, (0) single step uses a special hook into
  the GJK and PenetrationDepth code so that the graphics are redraw on 
  every iteration of the algorithm, (1) same as single step, but it 
  automatically steps every 300 ms, (2) real mode, calls the GJK code
  using regular farfield and bridge.

  In mode (0,1) an approximation of the CSO hull of the minkowski sum
  is drawn in gray.  During stepping the simplex is draw superimposed on
  the CSO.

  Keys:

      a - toggle run mode 0,1,2
      d - single step
      o - toggle translate/rotate
      p - toggle model (red/blue)
      space - toggle geometry
      enter - load/save/reset >> Look at the console
      w,s,arrows - translate/rotate

  After you press the ENTER key, look at the DOS window and it will prompt
  for filenames to write / read.

  Compiler flags

      _DEBUG = enable MEASSERT checking and single step api
      LOADSAVE = enable file read write (you'll need 
                 to set /FORCE option in the linker).
----------------------
  NOTE!!!!!!!!!

  In order to use the single step mode you must define the symbol
  MCD_GJK_DEBUG in the file Mcd/include/McdGjk.h and recompile the
  McdConvex library.

*/

#define USE_NEW_GJK 0
#define USE_CONV_BOX 1
#define READ_DATA_FILE 0
#define READ_XML 0

//  do not set both READ_XML and READ_DATA_FILE true at the same time

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#include <crtdbg.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>
#include <RConvex.h>

#ifdef LOADSAVE
#include <iostream.h>
#include <fstream.h>
#endif 

#include <McdGjk.h>
#include <McdGjkMaximumPoint.h>
#include <McdPlaneIntersect.h>

#if READ_XML
#include "ReadTestXML.h"
#endif

// this is data for reading 2 boxes from a datafile

struct
{
    MeVector3 r1,r2;   // radius, position, and quaternion
    MeVector3 p1,p2;
    MeVector4 q1,q2;
}
fbox = 
{
    {2,2,2},         {2,2,2},
    {0,4.2f,0},      {0,2,0},
    {1,0,0,0},       {1,0,0,0},
};

typedef struct
{
    MeVector4 color;
    McdModelID model;

    RGraphic *graphic;
    MeMatrix4Ptr matrix;
    int type;

} Primitive;

Primitive p[2]; 

McdGeometryID geom[5], boxgeom;
McdGeometryID planeGeom;
McdModelID plane;
RGraphic *groundG;

McdSpaceID space;
MdtWorldID world;
MstBridgeID bridge;
MeApp *app;
McdFrameworkID fwk;
MstUniverseID universe;

MeBool change = 1;
MeBool step = 0;
MeBool run = 0;

MeReal fatness = 0; //.1f;

int sharpEdges = 1, faceNormals = 0, twoSided = 0, autoDisable = 1;

typedef struct
{
    MeVector3 vertex[3];
    MeVector3 normal;
} Triangle;

MeVector3 offset;

#define TRI_LIST_SIZE 2

McdUserTriangle tri_list_memory[TRI_LIST_SIZE];

Triangle triList[TRI_LIST_SIZE] =
{
    { {{-1, 1, -1},  {-1, 1, 1},  {1, -1, 1} } },
    { {{-1, 1, -1},  {1, -1, 1},  {1, -1, -1}} }
};

#define NUM_VERT 4

MeVector3 vert[NUM_VERT] = {    /* convex hull */
    { 0, 1, 0 },
    { 1,-1, 0 },
    {-1,-1,-1 },
    {-1,-1, 1 },
};

MeVector3 mink[50];  // minkowski space hull

/* key control stuff */
int linear = 1;
int currentPrim = 0;

/* help text */

char *help[4] =
{
    "$ACTION2 - reset",
    "$ACTION3 - toggle primitive",
        0,
        0,

};

MeVector4 red = {1,0,0,1}, blue = {0,0.3,1,1}, green = {0,1,0,1};
AcmeReal white[4] = { 1, 1, 1, 1 };
AcmeReal yellow[4] = { 1, 1, 0, 1 };
AcmeReal pink[4] = { 1, 0, 1, 1 };
AcmeReal cyan[4] = { 0, 1, 1, 1 };
AcmeReal gray[4] = { .5f, .5f, .5f, 1 };
AcmeReal orange[4] = { 1, .5f, 0, 1 };
AcmeReal purple[4] = { .5f, 0, 1, 1 };

/* Render context */
RRender *rc;
RMenu* menu;


MeMatrix4 groundTM =
{
    1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, -1, 0, 1
};

MeMatrix4 groundRenderTM =
{
    1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, -1.05, 0, 1
};


RGraphic *tListGraphic;

void MakeRObjectVertex(RObjectVertex* vertex, MeVector3 vert, MeVector3 norm, MeReal u, MeReal v)
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



void SetHelp()
{
    return;
    help[2] = linear? "$ACTION4 - control angular": "$ACTION4 - control linear";
    help[3] = currentPrim? "$ACTION5 - control model 0": "$ACTION5 - control model 1";

    RRenderToggleUserHelp(rc);
    RRenderCreateUserHelp(rc,help,4);
    RRenderToggleUserHelp(rc);
}


void SetType(Primitive *p)
{
    int i,j;
    if(p->graphic)
        RGraphicDelete(rc,p->graphic,0);

    McdSpaceRemoveModel(p->model);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    if (p->type==1 && p->color[0] && boxgeom)
        McdModelSetGeometry(p->model,boxgeom);
    else
        McdModelSetGeometry(p->model,geom[p->type]);
    McdSpaceInsertModel(space,p->model);

    switch(p->type)
    {
    case 0:
        p->graphic = RGraphicSphereCreate(rc,1,p->color,p->matrix);
        break;
    case 1:
        if (p->color[0])   // the red one is the first
            p->graphic = RGraphicBoxCreate(rc,fbox.r1[0],fbox.r1[1],fbox.r1[2],p->color,p->matrix);
        else
            p->graphic = RGraphicBoxCreate(rc,fbox.r2[0],fbox.r2[1],fbox.r2[2],p->color,p->matrix);
        break;
    case 2:
        p->graphic = RGraphicCylinderCreate(rc,1,2,p->color,p->matrix);
        break;
    case 3:
        p->graphic = RGraphicCreateEmpty(TRI_LIST_SIZE * 3);
        for(i=0; i<TRI_LIST_SIZE; i++)
        {
            for(j=0; j<3; j++)
            {
                MakeRObjectVertex(p->graphic->m_pVertices+(i*3)+j,
                    triList[i].vertex[j], triList[i].normal, 0, 0);
            }
        }
        p->graphic->m_pObject->m_nTextureID = -1; /* no texture */
        p->graphic->m_pLWMatrix = NULL;

        RGraphicSetColor(p->graphic,p->color);
        RGraphicSetTransformPtr(p->graphic,p->matrix);
        RGraphicAddToList(rc, p->graphic, 0);
        break;
    case 4:
        p->graphic = RGraphicConvexCreate(rc, geom[4], p->color, p->matrix);
        p->graphic->m_pObject->m_nTextureID = -1; /* no texture */
        p->graphic->m_pLWMatrix = NULL;
        RGraphicSetColor(p->graphic,p->color);
        RGraphicSetTransformPtr(p->graphic,p->matrix);
        break;
    }
    RGraphicSetWireframe(p->graphic,1);
}

McdGeometryID MEAPI ConvexBoxCreate(McdFramework *fw, MeReal x, MeReal y, MeReal z);

/****************************************************************************
  This writes data for two boxes to a file.  It writes two lines each
  has size of box x,y,z, position x,y,z, and quaternion w,x,y,z.  
*/
void write_datafile2(char*fn, McdGeometryInstance *ins1, McdGeometryInstance *ins2)
{
    FILE *f = fopen(fn,"w");
    if (!f || ferror(f))
    {
        printf("unable to open %s\n",fn);
        return;
    }
    MeVector3Ptr v;
    MeVector4 q;
    McdBox *g = (McdBox*) ins1->mGeometry;
    MeQuaternionFromTM(q,ins1->mTM);
    v = ins1->mTM[3];
    if (g->m_g.mRefCtAndID&15 != kMcdGeometryTypeBox)
        fprintf(f,"type %d ",g->m_g.mRefCtAndID&15);
    else
        fprintf(f,"%.8e %.8e %.8e     ",g->mR[0],g->mR[1],g->mR[2]);
    fprintf(f,"%.8e %.8e %.8e     ",v[0],v[1],v[2]);
    fprintf(f,"%.8e %.8e %.8e %.8e\n",q[0],q[1],q[2],q[3]);

    g = (McdBox*) ins2->mGeometry;
    MeQuaternionFromTM(q,ins2->mTM);
    v = ins2->mTM[3];
    if (g->m_g.mRefCtAndID&15 != kMcdGeometryTypeBox)
        fprintf(f,"type %d ",g->m_g.mRefCtAndID&15);
    else
        fprintf(f,"%.8e %.8e %.8e     ",g->mR[0],g->mR[1],g->mR[2]);
    fprintf(f,"%.8e %.8e %.8e     ",v[0],v[1],v[2]);
    fprintf(f,"%.8e %.8e %.8e %.8e\n",q[0],q[1],q[2],q[3]);

    fclose(f);
    printf("wrote position data into %s\n",fn);
}
/****************************************************************************
  This processes the data after reading it from a file
*/
void post_read()
{
    MeVector3 dv = {0,3,0};
    MeVector3Subtract(dv,dv,fbox.p2);
    MeVector3Add(fbox.p2,fbox.p2,dv);
    MeVector3Add(fbox.p1,fbox.p1,dv);
    MeVector3Scale(fbox.r1,2);
    MeVector3Scale(fbox.r2,2);

    MeVector3Clamp(fbox.r1,0.01f,99);
    MeVector3Clamp(fbox.r2,0.01f,99);

#if USE_CONV_BOX
    boxgeom = ConvexBoxCreate(fwk,fbox.r1[0],fbox.r1[1],fbox.r1[2]);
//    geom[1] = ConvexBoxCreate(fwk,fbox.r2[0],fbox.r2[1],fbox.r2[2]);
#else
    boxgeom = McdBoxCreate(fwk,fbox.r1[0],fbox.r1[1],fbox.r1[2]);
//    geom[1] = McdBoxCreate(fwk,fbox.r2[0],fbox.r2[1],fbox.r2[2]);
#endif

}
/****************************************************************************
  This reads data for two boxes from a file
*/
void read_afile(char*fn)
{
    FILE *f = fopen(fn,"r");

    if (!f || ferror(f))
    {
        printf("unable to open %s\n",fn);
        return;
    }
    fscanf(f, "%f %f %f", fbox.r1, fbox.r1+1, fbox.r1+2);
    fscanf(f, "%f %f %f", fbox.p1, fbox.p1+1, fbox.p1+2);
    fscanf(f, "%f %f %f %f", fbox.q1, fbox.q1+1, fbox.q1+2, fbox.q1+3);
    fscanf(f, "%f %f %f", fbox.r2, fbox.r2+1, fbox.r2+2);
    fscanf(f, "%f %f %f", fbox.p2, fbox.p2+1, fbox.p2+2);
    fscanf(f, "%f %f %f %f", fbox.q2, fbox.q2+1, fbox.q2+2, fbox.q2+3);
    fclose(f);
    post_read();
}

/****************************************************************************
  This reads data for two boxes from a file
*/
void read_bfile(char*fn)
{
    FILE *f = fopen(fn,"rb");

    if (!f || ferror(f))
    {
        printf("unable to open %s\n",fn);
        return;
    }
    fread(fbox.r1,sizeof(MeReal),3,f);
    fread(fbox.p1,sizeof(MeReal),3,f);
    fread(fbox.q1,sizeof(MeReal),4,f);
    fread(fbox.r2,sizeof(MeReal),3,f);
    fread(fbox.p2,sizeof(MeReal),3,f);
    fread(fbox.q2,sizeof(MeReal),4,f);
    fread(offset,sizeof(MeReal),3,f);
    fclose(f);
    post_read();
}


//-------------------------------------------------------------
//  This sets interaction to GJK, this uses global var fwk.
//  see McdInteractionTableSetElement

void SetGJK(int geoType1, int geoType2)
{
    McdInteractions *e = &fwk->interactionTable[geoType1 + 
              fwk->geometryRegisteredCountMax * geoType2 ];

    e->intersectFn = McdGjkCgIntersect;
    e->helloFn = McdCacheHello;
    e->goodbyeFn = McdCacheGoodbye;

    if (geoType1 != geoType2)
    {
        e = &fwk->interactionTable[geoType2 + 
              fwk->geometryRegisteredCountMax * geoType1 ];
        e->intersectFn = McdGjkCgIntersect;
        e->helloFn = McdCacheHello;
        e->goodbyeFn = McdCacheGoodbye;
    }
}

#define SET_GJK(a,b) SetGJK(kMcdGeometryType##a,kMcdGeometryType##b)

static RGraphic **sline;      // static lines for DrawLine
static int max_sline, num_sline;

/****************************************************************************
    This makes a line width narrower or wider.
    Call this immediately after calling RGraphicLineMoveEnds.
*/
void SetLineWidth(RGraphic *line, AcmeReal z)
{
    int i;
    AcmeReal t;
    for( i = 0; i < line->m_pObject->m_nNumVertices; i+=2 )
    {
        t = (line->m_pVertices[i].m_Z + line->m_pVertices[i+1].m_Z) / 2;
        line->m_pVertices[i  ].m_Z = t+z;
        line->m_pVertices[i+1].m_Z = t-z;
    }
}

/****************************************************************************
    This erases all lines.
*/
void MEAPI ResetLines()
{
    int i;
    AcmeReal z[3] = { 0,0,0 };

    for (i = 0; i < num_sline; ++i)
        RGraphicLineMoveEnds(sline[i], z, z);

    num_sline = 0;
}

/****************************************************************************
    This draws a line. 
*/
void MEAPI DrawLine2(MeVector3 start, MeVector3 offset, AcmeReal *c)
{
    MeVector3 end;
    MeVector3Add(end, start, offset);

    int i;
    AcmeReal z[3] = { 0,0,0 };
    AcmeReal s[3] = { start[0], start[1], start[2] };
    AcmeReal e[3] = { end[0], end[1], end[2] };

    if (num_sline == max_sline)    // need to grow
    {
        max_sline += 10;
        sline = (RGraphic**) realloc(sline, max_sline * sizeof *sline);
        for (i = num_sline; i < max_sline; ++i)
            sline[i] = RGraphicLineCreate(rc,z,z,z,0);
    }
    i = num_sline++;
    RGraphicLineMoveEnds(sline[i], s, e);
    RGraphicSetColor(sline[i], c);
    RGraphicSetWireframe(sline[i], 1);
}

/****************************************************************************
  This adds one point to the minkowski hull
*/
void MinkHullAdd(MeReal x,MeReal y,MeReal z, int i)
{
    MeVector3 u,v,neg;
    MeVector3 d = {x,y,z};
    MeVector3MultiplyScalar(neg, d, -1);
    McdGjkMaximumPoint(&p[0].model->mInstance, neg, u);
    McdGjkMaximumPoint(&p[1].model->mInstance, d, v);
    MeVector3Subtract(mink[i],u,v);
}

/****************************************************************************
  This draw the minkowski hull
*/
void MinkHull()
{
    static RGraphic *gr = 0;
    int i = 0;
    MeReal x,y,z,t=0.05f;

    for (x=-1; x < 2; ++x)
        for (y=-1; y < 2; ++y)
            for (z=-1; z < 2; ++z)
                if (x*x+y*y+z*z > .5)
                    MinkHullAdd(x,y,z,i++);

    MEASSERT(i==26);

    for (x=-1; x < 2; x+=2)
        for (y=-1; y < 2; y+=2)
            for (z=-1; z < 2; z+=2)
            {       
                MinkHullAdd(x,t*y,t*z,i++);
                MinkHullAdd(t*x,y,t*z,i++);
                MinkHullAdd(t*x,t*y,z,i++);
            }

    MEASSERT(i==50);

    static MeMatrix4 mx;

    MeMatrix4TMMakeIdentity(mx);
    MeMatrix4TMSetPosition(mx,3,5,0);

    McdGeometryID geo = McdConvexMeshCreateHull(fwk,mink,i,0);

    if (gr)
        RGraphicDelete(rc, gr, 0);

    gr = RGraphicConvexCreate(rc, geo, gray, mx);

    McdGeometryDestroy(geo);

    RGraphicSetWireframe(gr,1);
}

//  This is to call GJK in single step debugging mode.

int McdGjkMain(McdCache *c, McdGjkSimplex *s);

/***********************************************************************
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc,void *userdata)
{
    static int t;
    static McdModelPair pair;
    static McdGjkSimplex s;

    McdContact contactData[50];
    McdIntersectResult result = { 0, contactData, 50, 0 };

    if (clock() > t && run)
    {
        step = 1;
        t = clock() + 300 - 200 * (run==2);  // delayed step
    }

    if (change || step)   //   press 'd' to set step=1
    {
        step = 0;

        if (run==2)
        {
            McdSpaceUpdateAll(space);
            MstBridgeUpdateContacts(bridge,space,world);
            MeAppDrawContacts(app);
            change = 0;
        }
#if MCD_GJK_DEBUG
        else
        {            //  Call GJK directly using single step mode
            if (change)
            {
                if (pair.model1)
                    McdCacheGoodbye(&pair);
                pair.model1 = p[0].model;
                pair.model2 = p[1].model;
                McdCacheHello(&pair);
                s.debug = 1;            // single step mode - start over
                s.inflate = 0;
                change = 0;
            }
            int f;
            McdCache *c = (McdCache*) pair.m_cachedData;

            MeVector3Copy(c->offset, offset);

            if (!s.inflate)
            {
                f = McdGjkMain(c, &s);
                printf("%d gjk = %g\n",s.debug, c->separation);
                if (f < 2)
                    s.debug = 1;
                if (f == 1)
                {
                    s.inflate = 1;       //  start penetration depth 
                    printf("done\n");
                }
                if (f==0) 
                {
                    printf("done, non-overlapping\n"); 
                    run = 0;
                }

                if (f==0 && c->separation <= c->fat1 + c->fat2)
                {
                    printf("fatness overlapping\n"); 
                    McdContactGen(c, &result);
                }
                else
                    result.contactCount = 0;
            }
            else
            {
                McdGjkPenetrationDepth(c, &s);
                printf("%d p.d = %g : %g %g\n",s.debug, c->normal[0], c->normal[1], c->normal[2]);
                f = s.debug;
                if (f == 1)
                {
                    s.inflate = run = 0;
                    printf("done\n");
                    McdContactGen(c, &result);
                }
            }

            //  Draw the lines for the simplex

            ResetLines();
            if (p[0].type + p[1].type)
                MinkHull();        // minkowski hull

            int i,b,j,d;
            MeVector3 a = {3,5,0};   // simplex origin
            MeVector3 u,v;

            
            FOR_EACH_BIT(i, b, s.bits)
            {
                //  Draw the support vectors 
                DrawLine2(s.point[i].s2,s.point[i].w,i==s.next_i?yellow:white);
                MeVector3Add(u, a, s.point[i].w);

                //  Draw the simplex edges
                FOR_EACH_BIT(j, d, s.bits) if (j < i)
                {
                    MeVector3Subtract(v, s.point[j].w, s.point[i].w);
                    DrawLine2(u, v, green);
                }
            }

            if (s.inflate && s.q)   //  Draw the queue vectors
            {
                McdGjkFaceQueue *q = (McdGjkFaceQueue *) s.q;
                for (i = 0; i<q->face.used; ++i)
                {
                    McdGjkFace *f = (McdGjkFace *) q->face.mem[i+1];
                    MeVector3MultiplyScalar(v, f->v, f->v_len);
                    DrawLine2(a,v,f==q->lastpop?pink:f->slant?yellow:cyan);
                }
            }
            else    // Draw the simplex perpendiculars
            {
                for (i = 0; (b = McdGjkBinarySubset[s.bits][i]) && 
                            (i<2 || b > McdGjkBinarySubset[s.bits][i-1]); ++i)
                {
                    j = McdGjkComputeVector(v, b, 0, &s);
                    if (!j) continue;
                    DrawLine2(a,v,!i?pink:f==1?red:cyan);
                }
            }

            //  draw the cache result

            if (f==1)
            {
                DrawLine2(c->location,c->normal,green);

            }

            //  Draw the polygon slices and intersection

            if (result.contactCount)
            {
                j = result.contactCount - 1;
                for (i = 0; i < result.contactCount; j=i,++i)
                {
                    MeVector3Subtract(u,result.contacts[j].position,result.contacts[i].position); 
                    DrawLine2(result.contacts[i].position, u, yellow);
                }

                //  compute slices so we can draw them

                MeVector3 vspace[200];
                MeReal dist = MeVector3Dot(c->normal, c->location);
                MeReal *ds = (MeReal*) result.data;

                McdGeometryInstanceGetSlice(c->ins1, c->normal, ds[0], 
                        200, &b, vspace);

                j = b - 1;
                for (i = 0; i < b; j=i,++i)
                {
                    MeVector3Subtract(u,vspace[j],vspace[i]); 
                    DrawLine2(vspace[i], u, orange);
                }

                McdGeometryInstanceGetSlice(c->ins2, c->normal, ds[1], 
                        200, &b, vspace);

                j = b - 1;
                for (i = 0; i < b; j=i,++i)
                {
                    MeVector3Subtract(u,vspace[j],vspace[i]); 
                    DrawLine2(vspace[i], u, purple);
                }
            }
        }
#else
        else
        {
            change = 0;
            ResetLines();
            if (p[0].type + p[1].type)
                MinkHull();        // minkowski hull
        }
#endif   //  MCD_GJK_DEBUG

    }

}

/* Reset boxes and balls to initial positions */
void MEAPI reset(RRender* rc, void* userData)     //    press 'enter'
{
    int ok=1;

#ifdef LOADSAVE

    if (p[0].matrix[0][0])
    {
        int i;
        char filename[200];
        cout << "Write to filename:\n";
        cin.getline(filename, sizeof filename);

        if (*filename)
        {
            //  write to file
            ofstream f(filename);
            MeReal *r;
        
            r = (MeReal*)p[0].matrix;
            f << p[0].type << endl;
            for (i=0; i<16; ++i) f<<r[i]<<' ';
            f<<endl;
            r = (MeReal*)p[1].matrix;
            f << p[1].type << endl;
            for (i=0; i<16; ++i) f<<r[i]<<' ';
            f<<endl;
            ok = 0;
        }

        cout << "Read from filename:\n";
        cin.getline(filename, sizeof filename);
        if (*filename)
        {
            //  read from file
            ifstream f(filename);
            MeReal *r;
            r = (MeReal*)p[0].matrix;
            f >> p[0].type;
            for (i=0; i<16; ++i) f>>r[i];
            r = (MeReal*)p[1].matrix;
            f >> p[1].type;
            for (i=0; i<16; ++i) f>>r[i];
            ok = 0;
        }
    }

#endif

#if ! READ_XML

    if (ok)      // set defaults
    {
        p[0].type = 1;
        p[1].type = 1;

        MeMatrix4TMMakeIdentity(p[0].matrix);
        MeMatrix4TMMakeIdentity(p[1].matrix);

        MeMatrix4TMSetPositionVector(p[0].matrix,fbox.p1);
        MeMatrix4TMSetPositionVector(p[1].matrix,fbox.p2);

        MeMatrix4TMSetRotationFromQuaternion(p[0].matrix, fbox.q1);
        MeMatrix4TMSetRotationFromQuaternion(p[1].matrix, fbox.q2);
    }
    SetType(p);
    SetType(p+1);
#endif

    linear = 0;
    currentPrim = 0;
    SetHelp();

    MdtBodyEnable(McdModelGetBody(p[0].model));
    MdtBodyEnable(McdModelGetBody(p[1].model));

    change = 1;
}

//  This routine creates a box using a convex mesh.
//  it's for testing convex mesh.

McdGeometryID MEAPI ConvexBoxCreate(McdFramework *fw, MeReal x, MeReal y, MeReal z)
{
    static MeVector3 vert[8];
    x/=2; y/=2; z/=2;
    MeVector3Set(vert[0],x,y,z);
    MeVector3Set(vert[1],x,y,-z);
    MeVector3Set(vert[2],x,-y,z);
    MeVector3Set(vert[3],x,-y,-z);
    MeVector3Set(vert[4],-x,y,z);
    MeVector3Set(vert[5],-x,y,-z);
    MeVector3Set(vert[6],-x,-y,z);
    MeVector3Set(vert[7],-x,-y,-z);
    return McdConvexMeshCreateHull(fw, vert, 8, fatness);
}


const MeReal moveStep = (MeReal)0.1;
const MeReal rotateStep = ME_PI/16;

void MEAPI move(RRender *rc, void *keypress)
{
    int k = (int)keypress;
    MeMatrix4 trans,tmp;
    MeMatrix3 rot;

    change = 1;

    if (k==6)                  // press 'a'
    {
        run = (run + 1)%3;
        change = 0;
    }
    else if (k==7)               // press 'd'
    {
        step = 1;
        change = 0;
    }
    else if(linear)
    {
        MeVector4Ptr pos = p[currentPrim].matrix[3];
        switch(k)
        {
        case 0:
            pos[2]+=moveStep;
            break;
        case 1:
            pos[2]-=moveStep;
            break;
        case 2:
            pos[0]-=moveStep;
            break;
        case 3:
            pos[0]+=moveStep;
            break;
        case 4:
            pos[1]+=moveStep;
            break;
        case 5:
            pos[1]-=moveStep;
            break;
        }
    }
    else
    {
        MeMatrix3MakeIdentity(rot);

        switch(k)
        {
        case 0:
            MeMatrix3MakeRotationX(rot,rotateStep);
            break;
        case 1:
            MeMatrix3MakeRotationX(rot,-rotateStep);
            break;
        case 2:
            MeMatrix3MakeRotationY(rot,rotateStep);
            break;
        case 3:
            MeMatrix3MakeRotationY(rot,-rotateStep);
            break;
        case 4:
            MeMatrix3MakeRotationZ(rot,rotateStep);
            break;
        case 5:
            MeMatrix3MakeRotationZ(rot,-rotateStep);
            break;
        }
        MeMatrix4TMMakeIdentity(trans);
        MeMatrix4TMSetRotation(trans,rot);
        MeMatrix4Copy(tmp,p[currentPrim].matrix);
        MeMatrix4MultiplyMatrix(p[currentPrim].matrix,trans,tmp);
    }

}

void MEAPI flipModel(RRender *rc,void *userData)
{
    currentPrim = 1 - currentPrim;
    SetHelp();
    change = 1;
}

void MEAPI flipControl(RRender *rc, void *userData)
{
    linear = 1 - linear;
    SetHelp();
    change = 1;
}

void MEAPI incPrimitive(RRender *rc, void *userData)
{
    p[currentPrim].type = (p[currentPrim].type+1)%5;
    SetType(p+currentPrim);
    change = 1;
}

int MEAPI TriListGeneratorCB(McdModelPair *mp, McdUserTriangle *tri, 
                             MeVector3 pos, MeReal rad, int maxTriangles)
{
    McdTriangleList* tl;
    int i;
    
    if(McdGeometryGetTypeId(McdModelGetGeometry(mp->model1))==kMcdGeometryTypeTriangleList)
        tl = (McdTriangleList*)McdModelGetGeometry(mp->model1);
    else
        tl = (McdTriangleList*)McdModelGetGeometry(mp->model2);
    
    for(i=0; i<maxTriangles && i<TRI_LIST_SIZE; i++, tri++)
    {
        /* Vertices */
        tri->vertices[0] = &triList[i].vertex[0];
        tri->vertices[1] = &triList[i].vertex[1];
        tri->vertices[2] = &triList[i].vertex[2];
        
        /* Normal -- Must be related to vertices / edges using RH rule  */
        tri->normal = &triList[i].normal;

        int &flags = (int&)tri->flags;
        
        flags = 0;  
        
        if(sharpEdges)
        {
            flags |= (int)kMcdTriangleUseEdge0;
            flags |= (int)kMcdTriangleUseEdge1;
            flags |= (int)kMcdTriangleUseEdge2;
        }
        
        if(!faceNormals)
            flags |= (int)kMcdTriangleUseSmallestPenetration;
        
        if(twoSided)
            flags |= (int)kMcdTriangleTwoSided;
    }
    
    return i;
}

void MEAPI_CDECL cleanup(void)
{
    MeAppDestroy(app);
    MstUniverseDestroy(universe);
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);
}

void MEAPI toggleTwoSided(MeBool on)
{
    twoSided = on;
    change = 1;
}

void MEAPI toggleFaceNormals(MeBool on)
{
    faceNormals = on;
    change = 1;
}

void MEAPI toggleSharpEdges(MeBool on)
{
    sharpEdges = on;
    change = 1;
}


void MEAPI togglehelp(RRender* rc, void* userdata) {
    RRenderToggleUserHelp(rc);
}

void MePoolxTest_main();
void MeHeapTest_main();
void MeSetTest_main();

/* Main Routine */
int MEAPI_CDECL main(int argc, const char * argv[])
{
    float color[4];
    MstUniverseSizes sizes;
    MeCommandLineOptions* options;
    int i;

//MeSetTest_main();
//MePoolxTest_main();
//MeHeapTest_main();
//return 0;

//#if defined _MSC_VER && defined _MECHECK
{
    //  This code checks for memory leaks
    int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    debugFlag |= _CRTDBG_ALLOC_MEM_DF;
    debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    //debugFlag |= _CRTDBG_CHECK_CRT_DF;
    debugFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(debugFlag);
}
//#endif 


    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 2;
    sizes.dynamicConstraintsMaxCount = 500;
    sizes.materialsMaxCount = 2;
    sizes.collisionModelsMaxCount = 200;
    sizes.collisionPairsMaxCount = 1000;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);
    fwk = MstUniverseGetFramework(universe);

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;


#if READ_DATA_FILE 
    read_bfile("\\temp\\2box.bin");
#endif

    RPerformanceBarCreate(rc);
//    RRenderSkydomeCreate(rc, "skydome", 2, 1);

    /* GROUND PLANE */

    MeMatrix4Ptr tm = (MeMatrix4Ptr)
        MeMemoryAPI.createAligned(sizeof(MeMatrix4),MeALIGNTO);
    MeMatrix4Copy(tm, groundTM);
    planeGeom = McdPlaneCreate(fwk);
    plane = MstFixedModelCreate(universe, planeGeom, tm);        
    color[0] = color[1] = color[2] = color[3] = 0.2;
    groundG = RGraphicGroundPlaneCreate(rc, 24, 2, color, -1);
//    RGraphicSetTexture(rc, groundG, "checkerboard");
    McdSpaceInsertModel(space,plane);
    McdSpaceUpdateModel(plane);
    McdSpaceFreezeModel(plane);
    McdFrameworkGetDefaultRequestPtr(fwk)->contactMaxCount = 12;

    geom[0] = McdSphereCreate(fwk,1);

#if USE_CONV_BOX
    geom[1] = ConvexBoxCreate(fwk,fbox.r2[0],fbox.r2[1],fbox.r2[2]);
#else
    geom[1] = McdBoxCreate(fwk,fbox.r2[0],fbox.r2[1],fbox.r2[2]);
#endif

    geom[2] = McdCylinderCreate(fwk,1,2);

    MeVector3 min1={-200,-200,-200};
    MeVector3 max1={200,200,200};

    geom[3] = McdTriangleListCreate(fwk,min1,max1,10,TriListGeneratorCB);
//    ((McdTriangleList*)geom[3])->triangleList = tri_list_memory;
    ((McdTriangleList*)geom[3])->triangleMaxCount = TRI_LIST_SIZE;

    for(i=0; i<TRI_LIST_SIZE; i++)
    {
        MeVector3 edge1, edge2;

        /* Edges - Not needed if you are storing the poly Normals */
        MeVector3Subtract(edge1, triList[i].vertex[1], triList[i].vertex[0]);
        MeVector3Subtract(edge2, triList[i].vertex[2], triList[i].vertex[1]);

        /* Normal -- Must be related to vertices / edges using RH rule  */
        MeVector3Cross(triList[i].normal, edge1, edge2);
        MeVector3Normalize(triList[i].normal);
        printf("(%f,%f,%f)\n",triList[i].normal[0],triList[i].normal[1],triList[i].normal[2]);

    }

    McdConvexMeshRegisterType(fwk);
    McdConvexMeshPrimitivesRegisterInteractions(fwk);

    geom[4] = McdConvexMeshCreateHull(fwk,vert,NUM_VERT, fatness);

    MeVector4Copy(p[0].color,red);
    MeVector4Copy(p[1].color,blue);


#if READ_XML
    MeStream stream = MeStreamOpen("collision.xml", kMeOpenModeRDONLY);
    XMLInputRead(stream);
    MeStreamClose(stream);

    p[0].model = CreateProps(universe, fwk, rc, properties);
    p[1].model = CreateProps(universe, fwk, rc, properties + 1);

    p[0].matrix = McdModelGetTransformPtr(p[0].model);
    p[1].matrix = McdModelGetTransformPtr(p[1].model);
#else
    p[0].model = MstModelAndBodyCreate(universe,geom[0],1);
    p[1].model = MstModelAndBodyCreate(universe,geom[0],1);

    p[0].graphic = 0;
    p[1].graphic = 0;

    p[0].matrix = (MeMatrix4Ptr)
        MeMemoryAPI.createAligned(sizeof(MeMatrix4),MeALIGNTO);
    p[1].matrix = (MeMatrix4Ptr)
        MeMemoryAPI.createAligned(sizeof(MeMatrix4),MeALIGNTO);

    McdModelSetTransformPtr(p[0].model,p[0].matrix);
    McdModelSetTransformPtr(p[1].model,p[1].matrix);
#endif
    McdSpaceInsertModel(space,p[0].model);
    McdSpaceInsertModel(space,p[1].model);

    app = MeAppCreate(world, space, rc);

#if USE_NEW_GJK
    //--------------------------------------------------------------
    //  Set interaction table pointers to GJK
    //--------------------------------------------------------------
    SET_GJK(Sphere, Sphere);
    SET_GJK(Sphere, Box);
    SET_GJK(Sphere, Plane);
    SET_GJK(Sphere, Cylinder);
    SET_GJK(Sphere, ConvexMesh);
    SET_GJK(Box  , Box);
    SET_GJK(Box  , Plane);
    SET_GJK(Box  , Cylinder);
    SET_GJK(Box  , ConvexMesh);
    SET_GJK(Plane , Plane);
    SET_GJK(Plane , Cylinder);
    SET_GJK(Plane , ConvexMesh);
    SET_GJK(Cylinder, Cylinder);
    SET_GJK(Cylinder, ConvexMesh);
    SET_GJK(ConvexMesh, ConvexMesh);
    //--------------------------------------------------------------
#endif

    RRenderSetWindowTitle(rc, "Primitives");

    RRenderSetUpCallBack(rc,move,(void *)0);
    RRenderSetDownCallBack(rc,move,(void *)1);
    RRenderSetLeftCallBack(rc,move,(void *)2);
    RRenderSetRightCallBack(rc,move,(void *)3);
    RRenderSetUp2CallBack(rc,move,(void *)4);
    RRenderSetDown2CallBack(rc,move,(void *)5);
    RRenderSetLeft2CallBack(rc,move,(void *)6);
    RRenderSetRight2CallBack(rc,move,(void *)7);

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, incPrimitive, 0);
    RRenderSetActionNCallBack(rc, 4, flipControl, 0);
    RRenderSetActionNCallBack(rc, 5, flipModel, 0);

    menu = RMenuCreate(rc, "Options Menu");
    RMenuAddToggleEntry(menu, "Sharp Edges", toggleSharpEdges, sharpEdges);
    RMenuAddToggleEntry(menu, "TwoSided", toggleTwoSided, twoSided);
    RMenuAddToggleEntry(menu, "Always Use Tri Normals", toggleFaceNormals, faceNormals);
    RRenderSetDefaultMenu(rc, menu);

    reset(rc, 0);

    app = MeAppCreate(world,space,rc);
    MeAppDrawContactsInit(app, green, 30);
    MeAppSetContactDrawLength(app, 0.09f);   // 0.02 is actual 1:1

    SetHelp();
    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}
/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 13:44:34 $ - Revision: $Revision: 1.7.2.7 $

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

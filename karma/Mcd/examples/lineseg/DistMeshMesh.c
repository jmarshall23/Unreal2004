/*----------------------------------------------------------------------------*/ 
/* 
	File: DistMeshMesh.c

	Description: Find distance between two meshs
   
  $Name: t-stevet-RWSpre-030110 $:  ($Id: DistMeshMesh.c,v 1.10 2001/09/18 19:02:14 dilips Exp $)    
*/

/*----------------------------------------------------------------------------*/ 
#include <stdio.h> 

#include <McdFrame.h>
#include <McdTriangleMesh.h>  
#include "McdDistanceResult.h"  
#include <MeMath.h>  

#ifdef WIN32
  #include <crtdbg.h>
	#include <windows.h>
#endif 

#include <MeViewer.h>          
#include <McduDrawTriangleMesh.h>  
#include <McduTriangleMeshIO.h>  
#include "../include/McduMeshSphere.h"  

MeReal    Points[2][3];   
MeReal   line[2][3] = {{-10,1,0}, {10,1,0}};  
float    color[3] = { 0.4f, 1, 0 }; 
float    white[3] = {1,1,1};
 
/*----------------------------------------------------------------------------*/ 
typedef struct GCBody
{	 
	McdModel* m_collision;	 
	RGraphic* m_graphic;
} GCBody;

McdGeometryID geom[2];
GCBody        body[2];
McdSpaceID    space;
RRender       *rc; 
MeReal        *vertexPtr[2];  
  
/*-----------------------------------------------------------------------------*/
void SetAxisAngle(MeReal *q, const MeReal nX, const MeReal nY,
                  const MeReal nZ, MeReal angle) 
{
    MeReal s_2 = -(MeReal)sin(0.5f*angle);
    q[1] = nX;
    q[2] = nY;
    q[3] = nZ;
    MeVector3Normalize(q+1);
    q[0] = (MeReal)cos(0.5f*angle);
    q[1] *= s_2;
    q[2] *= s_2;
    q[3] *= s_2;
}  

/*----------------------------------------------------------------------------*/ 
void SetRotation(MeMatrix4Ptr m, MeReal q[4])
{     
    MeQuaternionToTM(m, q);
}

int autoEvolve = 1;

void ToggleAutoEvolve() 
{
  autoEvolve = !autoEvolve;
}

McdModelPair      pair; 
enum { X, Y, Z, W };

/*----------------------------------------------------------------------------*/ 
void autoEvolveWorld( int iObj, unsigned int iAxis, MeReal angle )
{
  MeReal q[4];      
  const MeReal SQRT3 = 1.0f/sqrt(3);

  if ( iAxis == X )
    SetAxisAngle(q, 1, 0, 0, angle);
  else if ( iAxis == Y )  
    SetAxisAngle(q, 0, 1, 0, angle);
  else if (iAxis == Z )
    SetAxisAngle(q, 0, 0, 1, angle);
  else
    SetAxisAngle(q, SQRT3, SQRT3, SQRT3, angle);

  SetRotation( (MeMatrix4Ptr)body[iObj].m_graphic->m_matrixArray, q ); 
}



/*----------------------------------------------------------------------------*/ 
void EvolveWorld()
{ 
  McdDistanceResult resdist;

  McdTriangleMeshTriangleMeshDistance( &pair, &resdist);

  // printf("%f \n", resdist.distanceLB );

  if (resdist.distanceLB > 0) {
    line[0][0] = resdist.point1[0];
    line[0][1] = resdist.point1[1];
    line[0][2] = resdist.point1[2];

    line[1][0] = resdist.point2[0];
    line[1][1] = resdist.point2[1];
    line[1][2] = resdist.point2[2];
  } else {
    line[0][0] = 0;
    line[0][1] = 0;
    line[0][2] = 0;

    line[1][0] = 0;
    line[1][1] = 0;
    line[1][2] = 0;
	{	static int cc; fprintf(stderr,"No DISTANCE %d\n",cc++);}
  }
}

#define USE_SPHERES
 
/*----------------------------------------------------------------------------*/ 
void CreateTriMesh(RRender *rc,int i)
{ 
  int vertexCount = 0;

#ifdef USE_SPHERES   
    geom[i] = makeMeshSphere(2, 3, 1, (void**)&vertexPtr[i], 1 );
#else   
  if (i&1) {
    geom[i] = McduTriangleMeshCreateFromObj("../Resources/teapot566.obj",          // "../Resources/cube1.obj", // 
              1, 0, 0, 0.0, &vertexPtr[i], &vertexCount, 1);  
  } else { 
    geom[i] = McduTriangleMeshCreateFromObj("../Resources/teapot566.obj", 
              1, 0, 0, 0.0, &vertexPtr[i], &vertexCount, 1);  
 
#endif
 

  McdTriangleMeshBuild(geom[i]);
  body[i].m_graphic = RCreateTriangleMesh(rc, geom[i], color, 0);	
  body[i].m_collision = McdModelCreate(geom[i]);    
}
  
 
/*----------------------------------------------------------------------------*/ 
void McuDrawLineSeg()
{
  glLineWidth(3);
  glBegin(GL_LINES);    
		glVertex3f( line[0][0], line[0][1], line[0][2]);
		glVertex3f( line[1][0], line[1][1], line[1][2]);    
  glEnd();
  glLineWidth(1);
}

/*----------------------------------------------------------------------------*/ 
void McuDrawPoints()
{
  int i;
	for ( i=0; i<2; i++) {
		glBegin(GL_POINTS);
			glVertex3f( Points[i][0], Points[i][1], Points[i][2]);		 
		glEnd();
	}
}
 
/*----------------------------------------------------------------------------*/ 
void CreateWorld(RRender *rc)
{
  int i;
  MeReal q[4];  

  McdInit( 1, 100 );   

  McdTriangleMeshRegisterType(); 
  McdTriangleMeshRegisterInteractions(); 

  space = McdSpaceAxisSortCreate(McdAllAxes, 2, 100, 1);     
  
  CreateTriMesh(rc,0);   
  CreateTriMesh(rc,1); 

  for(i = 0; i < 2; i++) {

    McdSpaceInsertModel(space, body[i].m_collision);
    McdModelSetUserData(body[i].m_collision, (void*) &body[i]);                         
    McdModelSetTransformPtr(body[i].m_collision, 
          (MeMatrix4Ptr)body[i].m_graphic->m_matrixArray);
 
    SetAxisAngle(q, 1, 0, 0, (MeReal)ME_PI/2.0f) ;			     
    SetRotation( (MeMatrix4Ptr)body[i].m_graphic->m_matrixArray, q );    
    MeMatrix4TMSetPosition((MeMatrix4Ptr)body[i].m_graphic->m_matrixArray, 
          10*(i-0.5), 0, 0); 
  }   

  RCreateUserProceduralObject(rc, (RproceduralObjectCallback)McuDrawLineSeg, 
          0, "seg", 1, white, 0);    
}

/*----------------------------------------------------------------------------*/ 
void Click(int button, int state, int x, int y, RGraphic* graphic) 
{
  if (graphic) {
    /* fprintf(stderr,"\nindex: %d\n", (int) graphic->m_userData); */
  }
}
MeReal ang = 0;
/*----------------------------------------------------------------------------*/ 
void Tick(RRender* rc)
{ 
  if (autoEvolve) {
    ang += 0.075;    

    if( ang < 10 ) {
      autoEvolveWorld( 0, X, ang);    
      autoEvolveWorld( 1, Z, ang);    
    } else if( ang > 10 && ang < 20 ) {
      autoEvolveWorld( 0, W, ang);    
      autoEvolveWorld( 1, X, ang);
    } else if( ang > 20 && ang < 30 ) {
      autoEvolveWorld( 0, Z, ang);
      autoEvolveWorld( 1, Y, ang);
    } else if( ang > 30 && ang < 40 ) {
      autoEvolveWorld( 0, Z, ang);
      autoEvolveWorld( 1, W, ang);
    } if( ang > 40 && ang < 50 ) {
      autoEvolveWorld( 0, W, ang);
      autoEvolveWorld( 1, W, ang);
    }
    
    if (ang >50) ang = 0;
    
  }  

  EvolveWorld();
}

/*----------------------------------------------------------------------------*/ 
void
cleanup() 
{
  int i;
  /* graphics */
  RDeleteRenderContext(rc);

  for (i=0; i<2; i++) {
    McdGeometryDestroy(geom[i]);
    McdModelDestroy(body[i].m_collision);     
  }

  McdSpaceDestroy(space);  
  McdTerm();

  MeMemoryAPI.destroy(vertexPtr[0]);
  MeMemoryAPI.destroy(vertexPtr[1]);
}

/*----------------------------------------------------------------------------*/ 

int main(int argc, const char **argv)
{ 
	const RRenderType render = RParseRenderType(&argc,&argv); 
	

#if defined WIN32 && defined _DEBUG && 1
    int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    debugFlag |= _CRTDBG_ALLOC_MEM_DF;
    debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    debugFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(debugFlag);
#endif

  rc = RNewRenderContext(render, kRQualitySmooth); 

	CreateWorld(rc);

  pair.model1 = body[0].m_collision;
  pair.model2 = body[1].m_collision;

	rc->m_cameraOffset = 20;
	RUpdateCamera();  
 
  RUseKey('a', (RKeyCallback) ToggleAutoEvolve);
	RMouseFunc(Click);	 

  atexit((RKeyCallback) cleanup);
 
  rc->m_useDisplayLists = 0;	/* see the color changes */
	RRun( rc, Tick );
	RDeleteRenderContext( rc );
 
	return 0;
}
/*----------------------------------------------------------------------------*/ 


 
/*
   file: vispoints.cpp
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <MePrecision.h>
#include <MeViewer.h>

#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */
#include <GL/gl.h> 

#include <fstream>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

const int MAXVERTICES = 200;
const int MAXTRIANGLES = 200;

int DrawSingleTriangle = 1;
int NextTri = 0;

float white[3] = {1,1,1};

struct VisPolyhedron // roughly taken from Aerodynamics Geometry.h and .cpp
{
	int			m_NumberOfVertices;
	int			m_NumberOfTriangles;
	MeVector3	m_Vertices			[MAXVERTICES];
	int			m_Triangles			[MAXTRIANGLES][3];
	MeVector3   m_Normals			[MAXTRIANGLES];
} visPoly;

void ToggleDrawSingleTriangle()
{
    NextTri = 0;
    DrawSingleTriangle = DrawSingleTriangle ? 0 : 1;
}

void IncrementNextTri()
{
    NextTri++;
    NextTri %= visPoly.m_NumberOfTriangles;
}

bool LoadMeshFromFile(const char *filename, VisPolyhedron *g)
{
	FILE* f = fopen(filename,"rb");
	if(f == NULL)
	{
		cout<<"Failed to open ASE file: "<<filename<<endl;
        g->m_NumberOfTriangles = g->m_NumberOfVertices = 0;
		return false;
	}
	else
	{
		//data is arranged with number of vertices first then number of panels
		cout<<"Loading "<<filename<<"  "<<endl;
		ifstream input_file(filename);

		int numberOfVertices;
		int numberOfTriangles;

		//find number of vertices and panels
		input_file >> numberOfVertices >> numberOfTriangles;
		cout<<"numberOfV= "<<numberOfVertices<<" "<<numberOfTriangles<<endl;

		int i;
		int index;
		MeReal x,y,z;
	
		g->m_NumberOfVertices = numberOfVertices;
		g->m_NumberOfTriangles= numberOfTriangles;

		//assign vertices to geometry
		for(i=0; i<numberOfVertices; i++)
		{
			//read vertex data
			input_file>>index>>x>>y>>z;

            MeReal *vertex = &g->m_Vertices[i][0];
			vertex[0]		= x;
			vertex[1]		= y;
			vertex[2]		= z;
		}
	
		int vertex1,vertex2,vertex3;
		
		//Triangles to geometry
		for(i=0; i<numberOfTriangles; i++)
		{
			//read file
			input_file>>index>>vertex1>>vertex2>>vertex3;
			//cout<<"i= "<<i<<"    v= "<<vertex1<<endl;

			//assign data to geometry
			g->m_Triangles[i][0] = vertex1;
			g->m_Triangles[i][1] = vertex2;
			g->m_Triangles[i][2] = vertex3;
		}

        //Triangle normals
		for(i=0; i<numberOfTriangles; i++)
		{
			//read normal data
			input_file>>index>>x>>y>>z;

            MeReal *normal = &g->m_Normals[i][0];
			normal[0]		= x;
			normal[1]		= y;
			normal[2]		= z;
		}

		input_file.close();
	
		return true;
	}
}

void RenderMeshObject(VisPolyhedron* g)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
    //Draw vertices
	int i;
	for(i=0;i<g->m_NumberOfVertices;i++)
	{
		MeReal *p= g->m_Vertices[i];

		glColor4f(1.0f,1.0f,0.0f,0.0f);
		glBegin(GL_POINTS);
		APPEND_FV_OR_DV(glVertex3)(p);
		glEnd();		
	}

    if(DrawSingleTriangle)
    {
        i = NextTri;

		MeReal *p1= g->m_Vertices[g->m_Triangles[i][0]];
		MeReal *p2= g->m_Vertices[g->m_Triangles[i][1]];
		MeReal *p3= g->m_Vertices[g->m_Triangles[i][2]];
		
		MeReal panelcol	= 0;

		glColor4f(panelcol,0.0f,1.0f-panelcol,0.0f);
		glBegin(GL_TRIANGLES);
        APPEND_FV_OR_DV(glNormal3)(g->m_Normals[i]);
		glVertex3f(p1[0],p1[1],p1[2]);
		glVertex3f(p2[0],p2[1],p2[2]);
		glVertex3f(p3[0],p3[1],p3[2]);
		glEnd();		
    }
    else
    {    
        //Draw panels
	    for(i=0;i<g->m_NumberOfTriangles;i++)
	    {
		    MeReal *p1= g->m_Vertices[g->m_Triangles[i][0]];
		    MeReal *p2= g->m_Vertices[g->m_Triangles[i][1]];
		    MeReal *p3= g->m_Vertices[g->m_Triangles[i][2]];
		    
		    MeReal panelcol	= 0;

		    glColor4f(panelcol,0.0f,1.0f-panelcol,0.0f);
		    glBegin(GL_TRIANGLES);
            APPEND_FV_OR_DV(glNormal3)(g->m_Normals[i]);
		    glVertex3f(p1[0],p1[1],p1[2]);
		    glVertex3f(p2[0],p2[1],p2[2]);
		    glVertex3f(p3[0],p3[1],p3[2]);
		    glEnd();		
	    }
    }

    glNormal3f(0,1,0);

	//white lines
	glColor4f(1.0f,1.0f,1.0f,0.0f);
	
	//Draw mesh
	for(i=0;i<g->m_NumberOfTriangles;i++)
	{
		MeReal *p1= g->m_Vertices[g->m_Triangles[i][0]];
		MeReal *p2= g->m_Vertices[g->m_Triangles[i][1]];
		MeReal *p3= g->m_Vertices[g->m_Triangles[i][2]];
		
		glBegin(GL_LINES);
		glVertex3f(p1[0],p1[1],p1[2]);
		glVertex3f(p2[0],p2[1],p2[2]);
		glEnd();
		
		glBegin(GL_LINES);
		glVertex3f(p2[0],p2[1],p2[2]);
		glVertex3f(p3[0],p3[1],p3[2]);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(p3[0],p3[1],p3[2]);
		glVertex3f(p1[0],p1[1],p1[2]);
		glEnd();		
	}

}

void McuDrawMesh(void *cv)
{
    RenderMeshObject(&visPoly);
}

void Reset()
{
    LoadMeshFromFile("tempPoly.txt", &visPoly);
}

/*----------------------------------------------------------------------------*/ 
void Click(int button, int state, int x, int y, RGraphic* graphic) 
{
  if (graphic) {
    fprintf(stderr,"\nindex: %d\n", (int) graphic->m_userData); 
  }
}

/*----------------------------------------------------------------------------*/ 
void Tick(RRender* rc)
{   
}


/*----------------------------------------------------------------------------
void Reset()
{   
    int i, j;
    for(j = 0; j < maxPolys; j++) {
	    for (i=0; i<maxPoints; i++) {
	       pointss[j][i][0] = pointss[j][i][1] = pointss[j][i][2] = 0;
	    }
 	    pointCounts[j] = 0;
    }

	float x,y,z;

	printf("Enter n polys:\n");
    scanf("%i", &polyCount);
    for(i = 0; i < polyCount; i++) {
        int &pointCount = pointCounts[i];
        MeVector3 *points = pointss[i];

        printf("Poly %i\n", i);
        printf("Enter points:\n");
        while (3==scanf("%g %g %g",&x, &y, &z))
        {
	        points[pointCount][0] = x; 
	        points[pointCount][1] = y; 
	        points[pointCount][2] = z;
	            printf("point %d: %g %g %g\n", pointCount,
			        (float)points[pointCount][0],
			        (float)points[pointCount][1],
			        (float)points[pointCount][2]);
           pointCount++;
        }

	    printf(" %d points\n", pointCount);
    }
}

/*----------------------------------------------------------------------------*/ 
int main(int argc, const char **argv)
{ 
	const RRenderType render = RParseRenderType(&argc,&argv);

	RRender* rc = RNewRenderContext(render, kRQualitySmooth); 

    Reset();

    RCreateUserProceduralObject( rc, (RproceduralObjectCallback)McuDrawMesh  , 0, "seg", 1, white, 0);

	rc->m_cameraOffset = 20;
	RUpdateCamera(); 

	RMouseFunc(Click);	 
	rc->m_useDisplayLists = 0; /* see the color changes */

    RUseKey('\r', Reset);
    RUseKey('z', ToggleDrawSingleTriangle);
    RUseKey('x', IncrementNextTri);

	RRun( rc, Tick );
	RDeleteRenderContext( rc );
 
	return 0;
}

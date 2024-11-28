#ifndef _TrebuchetProto_h
#define _TrebuchetProto_h

#include <MeMath.h>
#include <Mcd.h>
#include <Mst.h>
#include <Mdt.h>
#include <MeViewer.h>

#include "CompoundObject.h"


#define NUM_BODIES	2
#define NUM_MODELS	7
#define NUM_JOINTS	10
#define NUM_AGGS	1

#define PIVOT_HEIGHT	4.0f
#define	PIVOT_OFFSET	1.5f

#define WHEEL_RADIUS	0.5f
#define WHEEL_BASE		2.0f
#define WHEEL_WIDTH		1.0f
#define WHEEL_THICKNESS	0.2f

#define BEAM_THICKNESS	0.2f




typedef struct
{
	MdtConstraintID		bBS1;
	MdtConstraintID		tether;

	int	numBodies;
	int	numModels;
	int numJoints;
	int numAggs;
	int numAggElements;

	MeReal pivotHeight;
	MeReal pivotOffset;
	MeReal wheelRadius;
	MeReal wheelBase;
	MeReal wheelWidth;
	MeReal wheelThickness;
	MeReal beamThickness;

	// Could make these dynamic :-)
	bodyData	 bodyList[NUM_BODIES];
	modelData	modelList[NUM_MODELS];
	jointData	jointList[NUM_JOINTS];
	aggData		  aggList[NUM_AGGS];

	int numInstance;

} TrebuchetProto;



/* Create A Trebuchet */
TrebuchetProto* TrebuchetProto_create(MstUniverseID u);
/* This shoiuld be static code just to initialise the structures that contain initialisation information */
void TrebuchetProto_initialiseData(MstUniverseID u, TrebuchetProto* t);
void TrebuchetProto_initialiseAggregates(MstUniverseID u, TrebuchetProto* t);
//void TrebuchetProto_BuildSupportGeometry (MstUniverseID u, TrebuchetProto* t);

//(MstUniverseID u, RRender* rc);
/* Delete a Trebuchet */
void TrebuchetProto_destroy(TrebuchetProto* t);

void TrebuchetProto_newInstance(TrebuchetProto* t);
void TrebuchetProto_removeInstance(TrebuchetProto* t);



/* (inline?) functions to return the number of bodies etc.
Could put in Trebuchet struct, but that would just waste memory */
int TrebuchetProto_getNumBodies();
int TrebuchetProto_getNumModels();
int TrebuchetProto_getNumJoints();
int TrebuchetProto_getNumAggs();



#endif


#include "TrebuchetProto.h"

int TrebuchetProto_getNumBodies(){return NUM_BODIES;}
int TrebuchetProto_getNumModels(){return NUM_MODELS;}
int TrebuchetProto_getNumJoints(){return NUM_JOINTS;}
int TrebuchetProto_getNumAggs(){return NUM_AGGS;}


//////////////////////////////////////////////////////////
// need to calc density for use in mstmodandbodcreate.
///////////////////////////////////////////////////////////

/* Create A TrebuchetProto */
TrebuchetProto* TrebuchetProto_create(MstUniverseID u) {

	// Allocate memory for the structures
    TrebuchetProto* t = MeMemoryAPI.create(sizeof(TrebuchetProto));


/* Setup the dimension parameters */
	t->numBodies = NUM_BODIES;
	t->numModels = NUM_MODELS;
	t->numJoints = NUM_JOINTS;
	t->numAggs	 = NUM_AGGS;

	t->pivotHeight =	PIVOT_HEIGHT;
	t->pivotOffset =	PIVOT_OFFSET;

	t->wheelRadius =	WHEEL_RADIUS;
	t->wheelBase =		WHEEL_BASE;
	t->wheelWidth =		WHEEL_WIDTH;
	t->wheelThickness =	WHEEL_THICKNESS;

	t->beamThickness =	BEAM_THICKNESS;
	t->numInstance = 0;
	
	TrebuchetProto_initialiseData(u,t);
	TrebuchetProto_initialiseAggregates(u,t);


	/* Initialise all the structures */


	return t;
}

void TrebuchetProto_initialiseData(MstUniverseID u, TrebuchetProto* t)
{




/*	// Setup pivot Piece
	pivot.type	= cylinder;
	pivot.rad	= 0.1;
	pivot.len	= 1.0;
	pivot.density	= 1.0;
	setPosition(&pivot, 
		t->pivotOffset, 
		t->pivotHeight, 
		pivot_Z);
	MeMatrix3MakeRotationY(pivot.rot, 0);
	setColor(&pivot, 1.0, 0.0, 0.0);
*/
	enum {bRope=0, wRope};
	#define BUCKET_ROPE	t->bodyList[bRope]
	#define WEIGHT_ROPE	t->bodyList[wRope]
//	#define TREB_FRAME	t->bodyList[trebFrame]

	enum {beam=0, bucket, weight, wheel1, wheel2, wheel3, wheel4};
	#define BEAM	t->modelList[beam]
	#define BUCKET	t->modelList[bucket]
	#define WEIGHT	t->modelList[weight]

	#define WHEEL1	t->modelList[wheel1]
	#define WHEEL2	t->modelList[wheel2]
	#define WHEEL3	t->modelList[wheel3]
	#define WHEEL4	t->modelList[wheel4]

	enum {beamHinge=0, bBS1, bBS2, wBS1, wBS2, tether, wh1, wh2, wh3, wh4};
	#define BEAM_HINGE	t->jointList[beamHinge]
	#define BUCKET_BS1	t->jointList[bBS1]
	#define BUCKET_BS2	t->jointList[bBS2]
	#define WEIGHT_BS1	t->jointList[wBS1]
	#define WEIGHT_BS2	t->jointList[wBS2]
	#define TETHER		t->jointList[tether]

	enum {axles=0, side, total};
	#define FRAME_AXLES	t->aggList[axles]
	#define FRAMES_SIDE	t->aggList[side]
	#define FRAME		t->aggList[total]



/* Setup some macros to keep thinds tidy */
#define SET_DIM(o,x,y,z)	\
	o.elem.dim[0] = x;			\
	o.elem.dim[1] = y;			\
	o.elem.dim[2] = z;

#define SET_POS(o,x,y,z)	\
	o.elem.pos[0] = x;			\
	o.elem.pos[1] = y;			\
	o.elem.pos[2] = z;

#define SET_ROT(o,x,y,z)	\
	o.elem.rot[0] = x;			\
	o.elem.rot[1] = y;			\
	o.elem.rot[2] = z;

#define SET_COL(o,r,g,b)	\
	o.elem.col[0] = r;			\
	o.elem.col[1] = g;			\
	o.elem.col[2] = b;

#define SET_JOINT_AXIS(o,x,y,z)	\
	o.axis[0] = x;			\
	o.axis[1] = y;			\
	o.axis[2] = z;

#define SET_JOINT_POS(o,x,y,z)	\
	o.pos[0] = x;			\
	o.pos[1] = y;			\
	o.pos[2] = z;





/***************************/
/* Do the Collision Models */
/***************************/
/* NB The ropes do not have collision */
/*
#define OBJ TREB_FRAME
	OBJ.elem.type	= cylinder;
	OBJ.density	= 1.0f;
	SET_DIM(OBJ,	0.1f,	0.1f,	t->pivotHeight-t->wheelRadius)
	SET_POS(OBJ,
		0.0f,
		t->wheelRadius+t->pivotHeight*0.5f,
		0.0f)
	SET_ROT(OBJ,	-ME_PI/2,	0.0f,	0.0f)
	SET_COL(OBJ, 0.0f, 0.0f, 1.0f)
#undef OBJ
*/
	
	// Setup Beam
#define OBJ BEAM
	OBJ.elem.type	= box;
	OBJ.density	= 0.01f;
	SET_DIM(OBJ,	0.2f,	0.2f,	5.0f)
	SET_POS(OBJ,
		-t->pivotOffset,
		t->pivotHeight,
		0.0f)
	SET_ROT(OBJ,	0.0f,-ME_PI/2,	0.0f)
	SET_COL(OBJ,	0.0f,	0.0f,	1.0f)
	OBJ.geom	= McdBoxCreate(u->frame,OBJ.elem.dim[0],OBJ.elem.dim[1],OBJ.elem.dim[2]);
#undef OBJ

	// Setup Bucket Rope
#define OBJ BUCKET_ROPE
	OBJ.elem.type	= cylinder;
	OBJ.density	= 0.01f;
	SET_DIM(OBJ,	0.025f,	0.025f,	1.0f)
	SET_POS(OBJ,
		-t->pivotOffset-BEAM.elem.dim[2]/2.0f,
		t->pivotHeight-BUCKET_ROPE.elem.dim[2]/2.0f,
		0.0f)
	SET_ROT(OBJ,	-ME_PI/2,	0.0f,	0.0f)
	SET_COL(OBJ, 0.0f, 0.0f, 1.0f)
#undef OBJ

	// Setup Bucket
#define OBJ BUCKET
	OBJ.elem.type	= box;
	OBJ.density	= 1.0f;
	SET_DIM(OBJ,	0.25f,	0.25f,	0.5f)
	SET_POS(OBJ,
		-t->pivotOffset-BEAM.elem.dim[2]/2.0f,
		(t->pivotHeight-BUCKET_ROPE.elem.dim[2])-BUCKET.elem.dim[2]/2.0f,
		0.0f)
	SET_ROT(OBJ,-ME_PI/2,	0.0f,	0.0f)
	SET_COL(OBJ,	0.0f,	0.0f,	1.0f)
	OBJ.geom	= McdBoxCreate(u->frame,OBJ.elem.dim[0],OBJ.elem.dim[1],OBJ.elem.dim[2]);
#undef OBJ

	// Setup Weight Rope
#define OBJ WEIGHT_ROPE
	OBJ.elem.type	= cylinder;
	OBJ.density	= 0.1f;
	SET_DIM(OBJ, 0.04f, 0.04f, 1.0f)
	SET_POS(OBJ,
		-t->pivotOffset+BEAM.elem.dim[2]/2.0f,
		t->pivotHeight-WEIGHT_ROPE.elem.dim[2]/2.0f,
		0.0f)
	SET_ROT(OBJ,	-ME_PI/2,	0.0f,	0.0f)
	SET_COL(OBJ, 1.0f, 0.0f, 0.0f)
#undef OBJ

	// Setup Weight
#define OBJ WEIGHT
	OBJ.elem.type	= box;
	OBJ.density	= 60.0f;
	SET_DIM(OBJ,	0.5f,	0.5f,	1.0f)
	SET_POS(OBJ,
		-t->pivotOffset+BEAM.elem.dim[2]/2.0f,
		(t->pivotHeight-WEIGHT_ROPE.elem.dim[2])-WEIGHT.elem.dim[2]/2.0f,
		0.0f)
	SET_ROT(OBJ,-ME_PI/2,	0.0f,	0.0f)
	SET_COL(OBJ,	0.0f,	0.0f,	1.0f)
	OBJ.geom	= McdBoxCreate(u->frame,OBJ.elem.dim[0],OBJ.elem.dim[1],OBJ.elem.dim[2]);
#undef OBJ






/*****************/
/* Do the Wheels */
/*****************/
#define OBJ WHEEL1
	OBJ.elem.type	= cylinder;
	OBJ.density	= 20.0f;
	SET_DIM(OBJ, t->wheelRadius, t->wheelRadius, t->wheelThickness)
	SET_POS(OBJ,
		t->wheelBase,
		t->wheelRadius,
		t->wheelWidth)
	SET_ROT(OBJ, 0.0f, 0.0f, 0.0f)
	SET_COL(OBJ, 1.0f, 1.0f, 1.0f)
	OBJ.geom = McdCylinderCreate(u->frame,t->wheelRadius,t->wheelThickness);
#undef OBJ

#define OBJ WHEEL2
	OBJ.elem.type	= cylinder;
	OBJ.density	= WHEEL1.density;
	SET_DIM(OBJ, t->wheelRadius, t->wheelRadius, t->wheelThickness)
	SET_POS(OBJ,
		t->wheelBase,
		t->wheelRadius,
		-t->wheelWidth)
	SET_ROT(OBJ, 0.0f, 0.0f, 0.0f)
	SET_COL(OBJ, WHEEL1.elem.col[0], WHEEL1.elem.col[1], WHEEL1.elem.col[2])
	OBJ.geom = WHEEL1.geom;
#undef OBJ

#define OBJ WHEEL3
	OBJ.elem.type	= cylinder;
	OBJ.density	= WHEEL1.density;
	SET_DIM(OBJ, t->wheelRadius, t->wheelRadius, t->wheelThickness)
	SET_POS(OBJ,
		-t->wheelBase,
		t->wheelRadius,
		t->wheelWidth)
	SET_ROT(OBJ, 0.0f, 0.0f, 0.0f)
	SET_COL(OBJ, WHEEL1.elem.col[0], WHEEL1.elem.col[1], WHEEL1.elem.col[2])
	OBJ.geom = WHEEL1.geom;
#undef OBJ

#define OBJ WHEEL4
	OBJ.elem.type	= cylinder;
	OBJ.density	= WHEEL1.density;
	SET_DIM(OBJ, t->wheelRadius, t->wheelRadius, t->wheelThickness)
	SET_POS(OBJ,
		-t->wheelBase,
		t->wheelRadius,
		-t->wheelWidth)
	SET_ROT(OBJ, 0.0f, 0.0f, 0.0f)
	SET_COL(OBJ, WHEEL1.elem.col[0], WHEEL1.elem.col[1], WHEEL1.elem.col[2])
	OBJ.geom = WHEEL1.geom;
#undef OBJ

	/* Set the wheel hinges */
#define OBJ 	t->jointList[wh1]
	OBJ.type		= HingeJoint;
	OBJ.ModOrBod[0] = isModel;
	OBJ.bodies[0]	= wheel1;
	OBJ.ModOrBod[1]	= isAggregate;
	OBJ.bodies[1]	= axles;
//	OBJ.ModOrBod[1]	= isBody;
//	OBJ.bodies[1]	= trebFrame;
	SET_JOINT_POS(OBJ,	  WHEEL1.elem.pos[0], WHEEL1.elem.pos[1], WHEEL1.elem.pos[2])
	SET_JOINT_AXIS(OBJ,	  0.0f, 0.0f, 1.0f)
	OBJ.isEnabled	= 1;
#undef OBJ
#define OBJ 	t->jointList[wh2]
	OBJ.type		= HingeJoint;
	OBJ.ModOrBod[0] = isModel;
	OBJ.bodies[0]	= wheel2;
	OBJ.ModOrBod[1]	= isAggregate;
	OBJ.bodies[1]	= axles;
	SET_JOINT_POS(OBJ,	  WHEEL2.elem.pos[0], WHEEL2.elem.pos[1], WHEEL2.elem.pos[2])
	SET_JOINT_AXIS(OBJ,	  0.0f, 0.0f, 1.0f)
	OBJ.isEnabled	= 1;
#undef OBJ
#define OBJ 	t->jointList[wh3]
	OBJ.type		= HingeJoint;
	OBJ.ModOrBod[0] = isModel;
	OBJ.bodies[0]	= wheel3;
	OBJ.ModOrBod[1]	= isAggregate;
	OBJ.bodies[1]	= axles;
	SET_JOINT_POS(OBJ,	  WHEEL3.elem.pos[0], WHEEL3.elem.pos[1], WHEEL3.elem.pos[2])
	SET_JOINT_AXIS(OBJ,	  0.0f, 0.0f, 1.0f)
	OBJ.isEnabled	= 1;
#undef OBJ
#define OBJ 	t->jointList[wh4]
	OBJ.type		= HingeJoint;
	OBJ.ModOrBod[0] = isModel;
	OBJ.bodies[0]	= wheel4;
	OBJ.ModOrBod[1]	= isAggregate;
	OBJ.bodies[1]	= axles;
	SET_JOINT_POS(OBJ,	  WHEEL4.elem.pos[0], WHEEL4.elem.pos[1], WHEEL4.elem.pos[2])
	SET_JOINT_AXIS(OBJ,	  0.0f, 0.0f, 1.0f)
	OBJ.isEnabled	= 1;
#undef OBJ

//	structureSetInertiaBox(&modelList[0]);
//	structureSetInertiaBox(&BUCKET);
//	structureSetInertiaBox(&weight);






/*****************************/
/* Setup the Aggregate frame */
/*****************************/
#define OBJ FRAME_AXLES
	OBJ.elem.type	= aggregate;
	OBJ.density	= 1.0f;
	SET_POS(OBJ,
		0.0f, 
		0.0f, 
		0.0f);
	SET_ROT(OBJ,	0.0f,	0.0f,	0.0f)
	SET_COL(OBJ,	0.0f,	0.0f,	1.0f)

	OBJ.numElements = 2;
	OBJ.element = MeMemoryAPI.create(sizeof(elemData)*OBJ.numElements);

#undef OBJ
	
#define OBJ FRAME_AXLES.element[0]
	OBJ.elem.type	= cylinder;
	OBJ.density	= 10.0f;
	SET_DIM(OBJ,	t->beamThickness, t->beamThickness, t->wheelWidth*2.0f)
	SET_POS(OBJ,
		t->wheelBase, 
		t->wheelRadius, 
		0.0f);
	SET_ROT(OBJ,	0.0f,	0.0f,	0.0f)
	SET_COL(OBJ,	0.0f,	0.0f,	1.0f)
	OBJ.geom	= McdBoxCreate(u->frame,OBJ.elem.dim[0],OBJ.elem.dim[1],OBJ.elem.dim[2]);
#undef OBJ

#define OBJ FRAME_AXLES.element[1]
	OBJ.elem.type	= cylinder;
	OBJ.density	= 10.0f;
	SET_DIM(OBJ,	t->beamThickness, t->beamThickness, t->wheelWidth*2.0f)
	SET_POS(OBJ,
		-t->wheelBase, 
		t->wheelRadius, 
		0.0f);
	SET_ROT(OBJ,	0.0f,	0.0f,	0.0f)
	SET_COL(OBJ,	0.0f,	0.0f,	1.0f)
	OBJ.geom	= t->aggList[0].element[0].geom;
#undef OBJ

/*************/
/* Do Joints */
/*************/

	/* Set the hinges */
#define OBJ BEAM_HINGE
	OBJ.type		= HingeJoint;
	OBJ.ModOrBod[0] = isModel;
	OBJ.bodies[0]	= beam;
	OBJ.ModOrBod[1]	= isAggregate;
	// Attached to the world, so use negative index
	OBJ.bodies[1]	= axles;
	SET_JOINT_POS(OBJ,	  0.0f, t->pivotHeight, 0.0f)
	SET_JOINT_AXIS(OBJ,	  0.0f, 0.0f, 1.0f)
	OBJ.isEnabled	= 1;
#undef OBJ

#define OBJ BUCKET_BS1
	OBJ.type		= BSJoint;
	OBJ.ModOrBod[0]	= isBody;
	OBJ.bodies[0]	= bRope;
	OBJ.ModOrBod[1]	= isModel;
	OBJ.bodies[1]	= beam;
	SET_JOINT_POS(OBJ,	  BUCKET_ROPE.elem.pos[0], t->pivotHeight, 0.0f)
	OBJ.isEnabled	= 1;
#undef OBJ

#define OBJ BUCKET_BS2
	OBJ.type		= BSJoint;
	OBJ.ModOrBod[0]	= isModel;
	OBJ.bodies[0]	= bucket;
	OBJ.ModOrBod[1]	= isBody;
	OBJ.bodies[1]	= bRope;
	SET_JOINT_POS(OBJ,	  BUCKET_ROPE.elem.pos[0], t->pivotHeight-BUCKET_ROPE.elem.dim[2], 0.0f)
	OBJ.isEnabled	= 1;
#undef OBJ

#define OBJ WEIGHT_BS1
	OBJ.type		= BSJoint;
	OBJ.ModOrBod[0]	= isBody;
	OBJ.bodies[0]	= wRope;
	OBJ.ModOrBod[1]	= isModel;
	OBJ.bodies[1]	= beam;
	SET_JOINT_POS(OBJ,	  WEIGHT_ROPE.elem.pos[0], t->pivotHeight, 0.0f)
	OBJ.isEnabled	= 1;
#undef OBJ

#define OBJ WEIGHT_BS2
	OBJ.type		= BSJoint;
	OBJ.ModOrBod[0]	= isModel;
	OBJ.bodies[0]	= weight;
	OBJ.ModOrBod[1]	= isBody;
	OBJ.bodies[1]	= wRope;
	SET_JOINT_POS(OBJ,	  WEIGHT_ROPE.elem.pos[0], t->pivotHeight-WEIGHT_ROPE.elem.dim[2], 0.0f)
	OBJ.isEnabled	= 1;
#undef OBJ

#define OBJ TETHER
	OBJ.type		= RPROJoint;
	OBJ.ModOrBod[0]	= isModel;
	OBJ.bodies[0]	= bucket;
	OBJ.ModOrBod[1]	= isAggregate;
	// Attached to the world, so use negative index
	OBJ.bodies[1]	= axles;
	// Pos and axis store the attachment positions. ;-)
	SET_JOINT_POS(OBJ,	  0.0f,-BUCKET.elem.dim[1]*0.5f,  0.0f)
	// Total blag - store the second attachment position in the axis field - coz it's not used for owt else
	SET_JOINT_AXIS(OBJ,	  t->wheelBase,0.0f,0.0f)
	//-t->pivotHeight*0.5f, 0.0f, 0.0f)//t->wheelBase)
	OBJ.isEnabled	= 1;
#undef OBJ


/* Clean up Definitions */
#undef BUCKET_ROPE
#undef WEIGHT_ROPE

#undef BEAM
#undef BUCKET
#undef WEIGHT

#undef WHEEL1
#undef WHEEL2
#undef WHEEL3
#undef WHEEL4

#undef BEAM_HINGE
#undef BUCKET_BS1
#undef BUCKET_BS2
#undef WEIGHT_BS1
#undef WEIGHT_BS2
#undef TETHER

#undef FRAME_AXLES
#undef FRAMES_SIDE
#undef FRAME

/* Setup some macros to keep thinds tidy */
#undef SET_DIM
#undef SET_POS
#undef SET_ROT
#undef SET_COL
#undef SET_JOINT_AXIS
#undef SET_JOINT_POS


	// Build the aggregate geometry
//	TrebuchetProto_BuildSupportGeometry (u,t);
}

/* Initialise all the Aggregates */
void TrebuchetProto_initialiseAggregates(MstUniverseID u, TrebuchetProto* t){

    int i,j,totalComponentCount;
	MeMatrix3 R;
	MeMatrix4 tmRel;
	McdGeometryID* component;

	totalComponentCount=0;
    for(i=0; i<t->numAggs; i++)
    {

        t->aggList[i].geom	= McdAggregateCreate(u->frame,t->aggList[i].numElements);
		component = (McdGeometryID*)MeMemoryAPI.create(t->aggList[i].numElements * sizeof(McdGeometryID));
 	    for(j=0; j<t->aggList[i].numElements; j++)
	    {
			component[j] = t->aggList[i].element[j].geom;
		    MeMatrix4TMMakeIdentity(tmRel);
			MeMatrix4TMSetPosition(tmRel, 
				t->aggList[i].element[j].elem.pos[0], 
				t->aggList[i].element[j].elem.pos[1], 
				t->aggList[i].element[j].elem.pos[2]);
	        MeMatrix3FromEulerAngles(R,
				t->aggList[i].element[j].elem.rot[0],
				t->aggList[i].element[j].elem.rot[1],
				t->aggList[i].element[j].elem.rot[2]);
			MeMatrix4TMSetRotation(tmRel,R);
		    McdAggregateAddElement(t->aggList[i].geom, component[j], tmRel);

			// increase the coutn of teh total components - for use in the Graphics array
			totalComponentCount++;
		}


	}

	t->numAggElements=totalComponentCount;
}
  
	
    //Initialze useful transformation matrices

/*    sideGeom = McdAggregateCreate(fw,3);

	// Bottom modelList[0] joins the 2 wheels
	bottomModels[0].geom	= McdBoxCreate(fw, t->beamThickness, t->beamThickness, t->wheelBase*2.0f);
    MeMatrix4TMMakeIdentity(tmRel);
	MeMatrix4TMSetPosition(tmRel, 
		t->pivotOffset, 
		t->wheelRadius, 
		t->wheelWidth-t->wheelThickness/2.0f);
    McdAggregateAddElement(sideGeom, bottomModels[0].Geom, tmRel);

	// Sloping modelList[0] joins a wheel to the pivot
	slopingmodelList[0]Geom	= McdBoxCreate(fw, modelList[0]_THICKNESS, modelList[0]_THICKNESS, MeSqrt(MeSqr(t->wheelBase)+MeSqr(t->pivotHeight-t->wheelRadius)));
    MeMatrix4TMMakeIdentity(tmRel);
	MeMatrix4TMSetPosition(tmRel, 
		t->pivotOffset+t->wheelBase/2.0f, 
		t->wheelRadius+(t->pivotHeight-t->wheelRadius)/2.0f,
		t->wheelWidth-t->wheelThickness/2.0f);
	MeMatrix3MakeRotationZ(rot,MeTan(t->wheelBase/t->pivotHeight-t->wheelRadius));
	MeMatrix4TMSetRotation(tmRel,rot);
    McdAggregateAddElement(sideGeom, slopingmodelList[0]Geom, tmRel);
	// Other sloping modelList[0] (same geom)
    MeMatrix4TMMakeIdentity(tmRel);
	MeMatrix4TMSetPosition(tmRel, 
		t->pivotOffset-t->wheelBase/2.0f, 
		t->wheelRadius+(t->pivotHeight-t->wheelRadius)/2.0f,
		t->wheelWidth-t->wheelThickness/2.0f);
	MeMatrix3MakeRotationZ(rot,-MeTan(t->wheelBase/t->pivotHeight-t->wheelRadius));
	MeMatrix4TMSetRotation(tmRel,rot);
    McdAggregateAddElement(sideGeom, slopingmodelList[0]Geom, tmRel);


}
*/


void TrebuchetProto_newInstance(TrebuchetProto* t) {
	t->numInstance++;
}

void TrebuchetProto_removeInstance(TrebuchetProto* t) {
	if (t->numInstance > 0){
		t->numInstance--;
	}else {
		MeFatalError(1,"Trying to remove instances of Trebuchet Prototype when none are registered!");
	}
}

void TrebuchetProto_destroy(TrebuchetProto* t)
{
//    CompoundObject_destroy(t->co);
	if (t->numInstance != 0){
		MeFatalError(1,"There are still instances of the Trebuchet Protottype in memory!");
	}
    MeMemoryAPI.destroy(t);
}


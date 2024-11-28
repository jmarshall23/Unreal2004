#ifndef _utils_h
#define _utils_h

#include <MeMath.h>
#include <Mdt.h>
#include <Mcd.h>
#include <MeViewer.h>
#include <Mst.h>

/****************************/
/* enumerate the primatives */
/****************************/
typedef enum {
	box,
	cylinder,
	sphere,
	aggregate,
} bodyType;

/*****************************/
/* enumerate the joint types */
/*****************************/
typedef enum 
{
	HingeJoint=0,
	BSJoint,
	RPROJoint
}jointType;

/********************************************************************/
/* Structure to hold intialisation data for general geometric forms */
/********************************************************************/
typedef struct
{
	bodyType	type;
	MeVector3	pos;
    MeVector3	rot;
	MeVector3	dim;
	MeVector3	col;
} elemData;


/***************************************************/
/* Structure to hold intialisation data for bodies */
/***************************************************/
typedef struct
{
	elemData	elem;
	MeReal		density;
} bodyData;

/***************************************************/
/* Structure to hold intialisation data for models */
/***************************************************/
typedef struct
{
	elemData	elem;
	McdGeometryID geom;
	MeReal		density;
} modelData;

/***************************************************/
/* Structure to hold intialisation data for models */
/***************************************************/
typedef struct
{
	elemData	elem;
	modelData*	element;
	int			numElements;
	MeReal		density;
	McdAggregateID	geom;
} aggData;

/***************************************************/
/* Structure to hold intialisation data for joints */
/***************************************************/
typedef struct
{
	jointType type;
	MeVector3 pos;
	MeVector3 axis;
	enum {isModel=0,isBody,isAggregate}	ModOrBod[2];
	int bodies[2];
//	MeReal stiffness;
//	MeReal damping;
	MeBool isEnabled;
} jointData;


/***************************************************/
/* Structure that holds data for a compound object */
/* IE information on bodies models joints and      */
/* graphics objects                                */
/***************************************************/
typedef struct
{
	// Store all the data as pointers to arrays - to be allocated later

	// Store objects that are just dynamic bodies
    MdtBodyID*          body;
    RGraphic**          bodyG;

	// store object that have collision models
    McdModelID*         model;
    RGraphic**          modelG;

    MdtConstraintID*    joint;
    MeMatrix4*          jointTM;

    McdModelID*			aggModel;
    RGraphic**          aggG;
	MeMatrix4*			aggTransMatrix;
	int numBodies,numModels,numJoints,numAggs,numAggElements;

	// Arrays to hold initialisation data
	bodyData*			bl;
	modelData*			ml;
	jointData*			jl;
	aggData*			al;

//	McdAggregateID*		aggGeom;

} CompoundObject;



/***********************************************/
/* Functions for manipulating Compound Objects */
/***********************************************/
CompoundObject* CompoundObject_create(MstUniverseID u, RRender* rc,
									  bodyData* bl,	modelData* ml,	jointData* jl,	aggData*	al,
									  int numB,		int numM,		int numJ,		int numA,
									  int numAE);

/* Delete a CompoundObject */
void CompoundObject_destroy(CompoundObject* t);

/* Setup functions */
void CompoundObject_setupBodies(MstUniverseID u, RRender* rc, CompoundObject* t);
void CompoundObject_setupModels(MstUniverseID u, RRender* rc, CompoundObject* t);
void CompoundObject_setupJoints(MstUniverseID u, RRender* rc, CompoundObject* t);
void CompoundObject_setupAggregates(MstUniverseID u, RRender* rc, CompoundObject* t);

void CompoundObject_generateGraphics(RRender* rc, RGraphic* g, elemData e, MeMatrix4Ptr t);

void CompoundObject_resetBodies(CompoundObject* t);
void CompoundObject_resetModels(CompoundObject* t);
void CompoundObject_resetJoints(CompoundObject* t);

/* Actions */
void CompoundObject_reset(CompoundObject* t);
//void CompoundObject_GraphicsCreate(RRender* rc);

#endif

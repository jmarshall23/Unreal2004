/*
 * $Id: Universe.c,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */
#include "MeMath.h"
#include "MeViewer.h"
#include "MeApp.h"
#include "Mst.h"

#include "BarSkittles.h"
#include "Universe.h"
#include "Viewer.h"
#include "Object.h"
#include "Rope.h"

/*
 * Globals.
 */
MstUniverseID	universe;
MdtWorldID	world;
MeVector3	gravity = { 0, GRAVITY, 0 };
MeReal		step = DEFAULT_STEP;

/*
 * Initialise the universe.
 */
void
universe_init()
{
	MstUniverseSizes	sizes;
	MdtContactParamsID	p;

	sizes.collisionModelsMaxCount = NCOLLISION_MODELS;
	sizes.collisionPairsMaxCount = NCOLLISION_PAIRS;
	sizes.collisionGeometryTypesMaxCount = McdPrimitivesGetTypeCount();
	sizes.dynamicBodiesMaxCount = NDYNAMIC_OBJECTS;
	sizes.dynamicConstraintsMaxCount = NCONTACTS;
	sizes.materialsMaxCount = 1;

	universe = MstUniverseCreate(&sizes);
	world = MstUniverseGetWorld(universe);
	MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

	p = MstBridgeGetContactParams(MstUniverseGetBridge(universe),
				      MstBridgeGetDefaultMaterial(),
				      MstBridgeGetDefaultMaterial());
	MdtContactParamsSetType(p, MdtContactTypeFriction2D);
	MdtContactParamsSetFriction(p, FRICTION);
	MdtContactParamsSetRestitution(p, RESTITUTION);
	MdtWorldSetEpsilon(world, EPSILON);
}

void
universe_finish()
{
	MstUniverseDestroy(universe);
}


void
universe_tick()
{
	MstUniverseStep(universe, step);
}

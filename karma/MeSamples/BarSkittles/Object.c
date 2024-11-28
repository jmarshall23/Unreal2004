/*
 * $Id: Object.c,v 1.2 2002/01/18 20:52:26 jamesg Exp $
 */
#include "MeMath.h"
#include "MeViewer.h"
#include "MeApp.h"
#include "Mst.h"

#include "BarSkittles.h"
#include "Universe.h"
#include "Viewer.h"
#include "Game.h"
#include "Object.h"

/*
 * Transformations for fixed objects.
 */
MeMatrix4	tm_plane = {
	1, 0, 0, 0,
	0, 0, -1, 0,
	0, 1, 0, 0,
	0, 0, 0, 1
};

/*
 * Geometries we will use.
 */
McdGeometryID	geom[NGEOMS];

/*
 * Actual objects.
 */
object_t object[NOBJECTS];

/*
 * Initialise all the objects.
 */
void
object_init()
{
	int			i;

	geom[GEOM_PLANE] = McdPlaneCreate();
	geom[GEOM_BALL] = McdSphereCreate(BALL_RADIUS);
	geom[GEOM_BASE] = McdBoxCreate(base_width, BASE_HEIGHT, base_depth);
	geom[GEOM_TOWER] = McdCylinderCreate(TOWER_RADIUS, TOWER_HEIGHT);
	geom[GEOM_SKITTLE] = McdCylinderCreate(SKITTLE_RADIUS, SKITTLE_HEIGHT);

	/*
	 * Create all the objects.
	 */
	object_init_plane(&(object[PLANE0]));
	object_init_ball(&(object[BALL0]));
	object_init_base(&(object[BASE0]));
	object_init_tower(&(object[TOWER0]));
	for (i = 0; i < NSKITTLES; i++)
		object_init_skittle(&(object[SKITTLE0 + i]), i);

	/*
	 * Must reset all objects to their starting positions.
	 */
	object_reset();
}

/*
 * Finish everything required for objects.
 */
void
object_finish()
{
	int	i;

	for (i = 0; i < NGEOMS; i++)
		McdGeometryDestroy(geom[i]);
}

void
object_reset()
{
	int	i;

	for (i = 0; i < NOBJECTS; i++)
		object_reset_item(&(object[i]));
}

/*
 * Initialise a ground plane.
 */
void
object_init_plane(object_t *	op)
{
	// op->reset_position not used */
	// op->reset_rotation not used */

	op->geom = geom[GEOM_PLANE];
	op->cm = MstFixedModelCreate(universe, geom[GEOM_PLANE], tm_plane);
	op->body = 0;
	op->graphic = RGraphicGroundPlaneCreate(rc, PLANE_LENGTH,
						PLANE_TRIANGLES, PLANE_COLOR,
						PLANE_Y);
	RGraphicSetTexture(rc, op->graphic, "checkerboard");
	op->fallen = 0;
#ifdef SHOW_UP
	op->upG = 0;
#endif /* SHOW_UP */
}

/*
 * Initialise a ball.
 */
void
object_init_ball(object_t *	op)
{
	MeMatrix4Ptr	tmp;

	op->reset_position[0] = skittle_leftx - BALL_OFFSET;
	op->reset_position[1] = BASE_HEIGHT + BALL_RADIUS + BALL_CLEARANCE;
	op->reset_position[2] = (skittle_frontz + skittle_backz) / TWO;
	MeVector4Copy(op->reset_rotation, q_identity);
	op->geom = geom[GEOM_BALL];
	op->cm = MstModelAndBodyCreate(universe, geom[GEOM_BALL],
				       BALL_DENSITY);
	op->body = McdModelGetBody(op->cm);
	tmp = McdModelGetTransformPtr(op->cm);
	op->graphic = RGraphicSphereCreate(rc, BALL_RADIUS, BALL_COLOR, tmp);
	RGraphicSetTexture(rc, op->graphic, "ME_ball3");
	op->fallen = 0;
#ifdef SHOW_UP
	op->upG = 0;
#endif /* SHOW_UP */
}

/*
 * Initialise a base on which the skittles are placed.
 */
void
object_init_base(object_t *	op)
{
	MeMatrix4Ptr	tmp;

	op->reset_position[0] = (skittle_rightx + skittle_leftx) / TWO;
	op->reset_position[1] = BASE_HEIGHT / TWO;
	op->reset_position[2] = (skittle_frontz + skittle_backz) / TWO;
	MeVector4Copy(op->reset_rotation, q_identity);

	op->geom = geom[GEOM_BASE];
#ifdef DYNAMIC_BASE
	op->cm = MstModelAndBodyCreate(universe, geom[GEOM_BASE],
				       BASE_DENSITY);
	op->body = McdModelGetBody(op->cm);
	tmp = McdModelGetTransformPtr(op->cm);
#else /* !DYNAMIC_BASE */
	tmp = MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
	MeMatrix4Copy(tmp, tm_identity);
	MeMatrix4TMSetPositionVector(tmp, op->reset_position);
	MeMatrix4TMSetRotationFromQuaternion(tmp, op->reset_rotation);
	op->cm = MstFixedModelCreate(universe, geom[GEOM_BASE], tmp);
	op->body = 0;
#endif /* !DYNAMIC_BASE */
	op->graphic = RGraphicBoxCreate(rc, base_width, BASE_HEIGHT,
					base_depth, BASE_COLOR, tmp);
	RGraphicSetTexture(rc, op->graphic, "stone");
	op->fallen = 0;
#ifdef SHOW_UP
	op->upG = 0;
#endif /* SHOW_UP */
}

/*
 * Initialise a tower which the ball is joined to via a rope.
 */
void
object_init_tower(object_t *	op)
{
	MeMatrix4Ptr	tmp;

	op->reset_position[0] = skittle_leftx - TOWER_OFFSET;
	op->reset_position[1] = (TOWER_HEIGHT / TWO) + BASE_HEIGHT;
	op->reset_position[2] = (skittle_frontz + skittle_backz) / TWO;
	MeVector4Copy(op->reset_rotation, q_standup);

	op->geom = geom[GEOM_TOWER];
#ifdef DYNAMIC_TOWER
	op->cm = MstModelAndBodyCreate(universe, geom[GEOM_TOWER],
				       TOWER_DENSITY);
	op->body = McdModelGetBody(op->cm);
	tmp = McdModelGetTransformPtr(op->cm);
#else /* !DYNAMIC_TOWER */
	tmp = MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
	MeMatrix4Copy(tmp, tm_identity);
	MeMatrix4TMSetPositionVector(tmp, op->reset_position);
	MeMatrix4TMSetRotationFromQuaternion(tmp, op->reset_rotation);
	op->cm = MstFixedModelCreate(universe, geom[GEOM_TOWER], tmp);
	op->body = 0;
#endif /* !DYNAMIC_TOWER */
	op->graphic = RGraphicCylinderCreate(rc, TOWER_RADIUS, TOWER_HEIGHT,
					     TOWER_COLOR, tmp);
	RGraphicSetTexture(rc, op->graphic, "wood1");
#ifdef SHOW_UP
	op->fallen = 0;
#endif /* SHOW_UP */
}

/*
 * Initialise a skittle.
 */
void
object_init_skittle(object_t *	op,
		    int		i)
{
	MeMatrix4Ptr		tmp;

	op->reset_position[0] = SKITTLE_X(i);
	op->reset_position[1] = SKITTLE_Y(i);
	op->reset_position[2] = SKITTLE_Z(i);
	MeVector4Copy(op->reset_rotation, q_standup);

	op->geom = geom[GEOM_SKITTLE];
	op->cm = MstModelAndBodyCreate(universe, geom[GEOM_SKITTLE],
				       SKITTLE_DENSITY);
	op->body = McdModelGetBody(op->cm);
	tmp = McdModelGetTransformPtr(op->cm);
	op->graphic = RGraphicCylinderCreate(rc, SKITTLE_RADIUS,
					     SKITTLE_HEIGHT, SKITTLE_COLOR,
					     tmp);
	RGraphicSetTexture(rc, op->graphic, "wood1");
	op->fallen = 0;
#ifdef SHOW_UP
	{
		AcmeReal	start[3], end[3];

		start[0] = start[1] = start[2] = (AcmeReal)0.0;
		end[0] = (AcmeReal)0.0;
		end[1] = (AcmeReal)SKITTLE_HEIGHT;
		end[2] = (AcmeReal)0.0;
		op->upG = RGraphicLineCreate(rc, start, end, color_blue,
					     tm_identity);
	}
#endif /* SHOW_UP */
}

/*
 * Reset the given object to start.
 */
void
object_reset_item(object_t *	op)
{
	object_reset_item_damping(op, DAMPING_LINEAR_VEL, DAMPING_ANGULAR_VEL);
}

/*
 * Reset the given object to start with a specified damping.
 */
void
object_reset_item_damping(object_t *	op,
			  MeReal	lvel,
			  MeReal	avel)
{
	op->fallen = 0;

	/*
	 * reset position and orientation only if dynamic.
	 */
	if (op->body == 0)
		return;
	McdModelDynamicsReset(op->cm);
	McdModelDynamicsSetPosition(op->cm,
				    op->reset_position[0],
				    op->reset_position[1],
				    op->reset_position[2]);
	McdModelDynamicsSetQuaternion(op->cm,
				      op->reset_rotation[0],
				      op->reset_rotation[1],
				      op->reset_rotation[2],
				      op->reset_rotation[3]);
	McdModelDynamicsSetDamping(op->cm, lvel, avel);
	McdModelDynamicsEnable(op->cm);
}


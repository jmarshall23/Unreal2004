/*
 * $Id: Cursor.c,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 * 
 * Provides a cursor for aiming and launching the ball.
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
#include "Cursor.h"

/*
 * How to move the cursor.
 */
cursor_movement_t cursor_movement[NCURSOR_MOVEMENTS] = {
	{ 2, -1, -1, CURSOR_TRANSLATE_STEP },
	{ 2, -1, -1, -CURSOR_TRANSLATE_STEP },
	{ 0, -1, -1, -CURSOR_TRANSLATE_STEP },
	{ 0, -1, -1, CURSOR_TRANSLATE_STEP },
	{ -1, 1, -1, -CURSOR_ROTATE_STEP },
	{ -1, 1, -1, CURSOR_ROTATE_STEP },
	{ -1, -1, 0, CURSOR_POWER_STEP },
	{ -1, -1, 0, -CURSOR_POWER_STEP },
};

/*
 * Starting position of the cursor.
 */
MeVector3	cursor_zero_position = {
	0, -BALL_RADIUS - BALL_CLEARANCE / TWO, 0
};

MeVector3	cursor_zero_rotation = {
	ME_PI / TWO, ME_PI, 0
};

/*
 * Is the cursor holding the ball or not?
 */
int		cursor_holding_ball;

/*
 * Information about the cursor.
 */
MeMatrix4	cursor_tm;
MeVector3	cursor_position;
MeVector3	cursor_rotation;
RGraphic	*cursorG;
MeReal		cursor_plane_distance;

/*
 * About the cursor power bar.
 */
MeReal		cursor_power;
#ifdef CURSOR_POWER_TEXT
RGraphic	*cursor_powerG;
#endif

/*
 * Update the cursor's translation matrix for it's position and rotation.
 */
static void
cursor_update()
{
	MeMatrix3	m;

	MeMatrix3FromEulerAngles(m,
				 cursor_rotation[0],
				 cursor_rotation[1],
				 cursor_rotation[2]);
	MeMatrix4TMMakeFromRotationAndPosition(cursor_tm,
					       m,
					       cursor_position[0],
					       cursor_position[1],
					       cursor_position[2]);

#ifdef CURSOR_POWER_TEXT
	{
		char		buf[32];

		if (cursor_powerG != NULL)
			RGraphicDelete(rc, cursor_powerG, 1);
		sprintf(buf, "Power %03.0f%%", cursor_power);
		cursor_powerG = RGraphicTextCreate(rc, buf,
						   CURSOR_POWER_X,
						   CURSOR_POWER_Y,
						   color_white);
	}
#endif /* CURSOR_POWER_TEXT */

#ifndef CURSOR_TEXTURE
	if (cursorG != NULL) {
		RObjectVertex	*rv;

		rv = cursorG->m_pVertices;
		rv[1].m_Y = rv[4].m_Y =
			CURSOR_SIZE * cursor_power / CURSOR_POWER_MAX;
	}
#endif /* !CURSOR_TEXTURE */
}

/*
 * Initialise the cursor.
 */
void
cursor_init()
{
	cursor_reset();

#ifdef CURSOR_TEXTURE
	cursor_powerG = NULL;
	cursorG = RGraphicSquareCreate(rc, CURSOR_SIZE, color_white,
				       cursor_tm);
	RGraphicSetTexture(rc, cursorG, "Cursor");
#else /* CURSOR_TEXTURE */
	{
		RObjectVertex	*rv;

		cursorG = RGraphicCreateEmpty(6);
		rv = cursorG->m_pVertices;
		/* 0 */
		rv->m_X = -CURSOR_SIZE / TWO;
		rv->m_Y = -CURSOR_SIZE / TWO;
		rv->m_Z = 0;
		rv->m_NX = rv->m_NY = 0;
		rv->m_NZ = -1;
		rv->m_U = 0;
		rv->m_V = 0;
		rv++;

		/* 1 */
		rv->m_X = 0;
		rv->m_Y = CURSOR_SIZE / TWO;
		rv->m_Z = 0;
		rv->m_NX = rv->m_NY = 0;
		rv->m_NZ = -1;
		rv->m_U = 1;
		rv->m_V = 2;
		rv++;

		/* 2 */
		rv->m_X = 0;
		rv->m_Y = 0;
		rv->m_Z = 0;
		rv->m_NX = rv->m_NY = 0;
		rv->m_NZ = -1;
		rv->m_U = 1;
		rv->m_V = 1;
		rv++;

		/* 3 */
		rv->m_X = CURSOR_SIZE / TWO;
		rv->m_Y = -CURSOR_SIZE / TWO;
		rv->m_Z = 0;
		rv->m_NX = rv->m_NY = 0;
		rv->m_NZ = -1;
		rv->m_U = 2;
		rv->m_V = 0;
		rv++;

		/* 4 */
		rv->m_X = 0;
		rv->m_Y = CURSOR_SIZE / TWO;
		rv->m_Z = 0;
		rv->m_NX = rv->m_NY = 0;
		rv->m_NZ = -1;
		rv->m_U = 1;
		rv->m_V = 2;
		rv++;

		/* 5 */
		rv->m_X = 0;
		rv->m_Y = 0;
		rv->m_Z = 0;
		rv->m_NX = rv->m_NY = 0;
		rv->m_NZ = -1;
		rv->m_U = 1;
		rv->m_V = 1;
		rv++;

		cursorG->m_pObject->m_nTextureID = -1;
		cursorG->m_pObject->m_bIsWireFrame = 0;

		RGraphicSetColor(cursorG, color_green);
		RGraphicSetTransformPtr(cursorG, cursor_tm);
		RGraphicAddToList(rc, cursorG, 0);
	}
#endif /* !CURSOR_TEXTURE */

#ifdef CURSOR_USE_KEYBOARD
	/*
	 * Set up movement for cursor.
	 */
	RRenderSetUpCallBack(rc, cursor_move_keyboard,
			     &(cursor_movement[CURSOR_UP]));
	RRenderSetDownCallBack(rc, cursor_move_keyboard,
			     &(cursor_movement[CURSOR_DOWN]));
	RRenderSetLeftCallBack(rc, cursor_move_keyboard,
			     &(cursor_movement[CURSOR_LEFT]));
	RRenderSetRightCallBack(rc, cursor_move_keyboard,
			     &(cursor_movement[CURSOR_RIGHT]));
	RRenderSetLeft2CallBack(rc, cursor_move_keyboard,
				&(cursor_movement[CURSOR_CCW]));
	RRenderSetRight2CallBack(rc, cursor_move_keyboard,
				 &(cursor_movement[CURSOR_CW]));
	RRenderSetUp2CallBack(rc, cursor_move_keyboard,
				&(cursor_movement[CURSOR_POWER_UP]));
	RRenderSetDown2CallBack(rc, cursor_move_keyboard,
				 &(cursor_movement[CURSOR_POWER_DOWN]));
#endif /* CURSOR_USE_KEYBOARD */
}

/*
 * Finish using the cursor.
 */
void
cursor_finish()
{
}

/*
 * Move the ball according to the cursor.
 */
void
cursor_tick()
{
	MeVector3	f, ball, ballv, g;
	MeReal		fmag;

	if (!cursor_holding_ball)
		return;

	/*
	 * Make sure that the body is enabled so we can move it.
	 */
	MdtBodyEnable(object[BALL0].body);

	/*
	 * Find the vector from the ball to the cursor and apply a stiffness
	 * to this.
	 */
	MdtBodyGetPosition(object[BALL0].body, ball);
	MeVector3Subtract(f, cursor_position, ball);
	MeVector3Scale(f, CURSOR_STIFFNESS);

	/*
	 * Damp due to velocity.
	 */
	MdtBodyGetVelocityAtPoint(object[BALL0].body, ball, ballv);
	MeVector3Scale(ballv, -CURSOR_DAMPING);

	/*
	 * Now add damped and original and multiply by ball mass.
	 */
	MeVector3Add(f, f, ballv);
	MeVector3Scale(f, MdtBodyGetMass(object[BALL0].body));

	/*
	 * Limit this value so we don't snap the rope.
	 */
	fmag = MeVector3Magnitude(f);
	if (fmag > CURSOR_FORCE_LIMIT)
		MeVector3Scale(f, CURSOR_FORCE_LIMIT / fmag);

	/*
	 * Compensate for gravity.
	 */
	MeVector3Copy(g, gravity);
	MeVector3Scale(g, -MdtBodyGetMass(object[BALL0].body));
	MeVector3Add(f, g, f);

	/*
	 * Apply the force.
	 */
	MdtBodyAddForceAtPosition(object[BALL0].body,
				  f[0], f[1], f[2],
				  ball[0], ball[1], ball[2]);
}

/*
 * Cause the cursor to throw the ball.
 */
void
cursor_throw_ball()
{
	MeVector3	f, ball, totower, offcursor, tmp;
	MeMatrix3	m;

	if (!cursor_holding_ball)
		return;
	cursor_holding_ball = 0;

	/*
	 * We need the vector which is in the direction of the cursor,
	 * but perpendicular to the line between the ball and the top of the
	 * tower.
	 * 
	 * To do this we get a cross product of this vector and one 90degrees 
	 * rotated of the cursor direction.
	 */
	MdtBodyGetPosition(object[BALL0].body, ball);
	MeVector3Subtract(totower, object[TOWER0].reset_position, ball);

	tmp[0] = 1;
	tmp[1] = 0;
	tmp[2] = 0;
	MeMatrix3FromEulerAngles(m,
				 cursor_rotation[0],
				 cursor_rotation[1],
				 cursor_rotation[2]);
	MeMatrix3MultiplyVector(offcursor, m, tmp);

	MeVector3Cross(f, totower, offcursor);
	MeVector3Scale(f, cursor_power * CURSOR_POWER_SCALE);
	MeVector3Scale(f, MdtBodyGetMass(object[BALL0].body));

	/*
	 * Impart the throwing force.
	 */
	MdtBodyAddForceAtPosition(object[BALL0].body,
				  f[0], f[1], f[2],
				  ball[0], ball[1], ball[2]);
}

/*
 * Cause the cursor to grab the ball again.
 */
void
cursor_grab_ball()
{
	cursor_holding_ball = 1;
	cursor_update();
}

/*
 * Cause the cursor to reset and hold the ball again.
 */
void
cursor_reset()
{
	MeVector3Add(cursor_position, object[BALL0].reset_position,
		     cursor_zero_position);
	MeVector3Copy(cursor_rotation, cursor_zero_rotation);
	cursor_plane_distance = cursor_position[1];
	cursor_holding_ball = 1;
	cursor_power = CURSOR_POWER_MAX / TWO;
	cursor_update();
}

/*
 * Check the cursor position is ok, if not restore to old position.
 */
static void
cursor_check_position(MeVector3	old)
{
	MeReal	offx, offz;

	/*
	 * Make sure we can't roam into the skittles.
	 */
	offx = cursor_position[0] - object[TOWER0].reset_position[0];
	offz = cursor_position[2] - object[TOWER0].reset_position[2];
	if (offx > -offz) {
		/*
		 * We want maxx = maxz, by changing the Z coordinate.
		 * 
		 * C0 - Z0 = Z2 - C2
		 * -C0 + Z0 + Z2 = C2
		 */
		cursor_position[2] = -cursor_position[0]
			+ object[TOWER0].reset_position[0]
			+ object[TOWER0].reset_position[2];
	}

	/*
	 * Make sure cursor position can't go behind tower.
	 */
	if (offx < 0 && offz > 0)
		cursor_position[2] = old[2];
}

static void
cursor_check_power()
{
	if (cursor_power > CURSOR_POWER_MAX)
		cursor_power = CURSOR_POWER_MAX;
	if (cursor_power < CURSOR_POWER_MIN)
		cursor_power = CURSOR_POWER_MIN;
}

/*
 * Move the cursor using the given movement information via the keyboard.
 */
void MEAPI_STDCALL
cursor_move_keyboard(RRender *			rc,
		     cursor_movement_t *	mp)
{
	MeVector3	old;

	if (mp->t_index >= 0) {
		/*
		 * Move along an axis, don't let cursor stray behind tower.
		 */
		MeVector3Copy(old, cursor_position);
		cursor_position[mp->t_index] += mp->amount;
		cursor_check_position(old);
	}

	if (mp->r_index >= 0) {
		/*
		 * Rotate cursor
		 */
		cursor_rotation[mp->r_index] += mp->amount;
	}

	if (mp->p_index >= 0) {
		/*
		 * Change cursor power.
		 */
		cursor_power += mp->amount;
		cursor_check_power();
	}
		
	cursor_update();
}

/*
 * Move the cursor, according to the mouse.
 */
void MEAPI
cursor_move_mouse(RRender *		rc,
		  int			x,
		  int			y,
		  int	  		modifiers,
		  RMouseButtonWhich	which,
		  RMouseButtonEvent	event,
		  void *		userdata)
{
	MeApp		*meapp = (MeApp *)userdata;
	MeVector3	dir, normal, camera, pos, old;
	MeReal		dot, t;

	if (modifiers != RCONTROL) {
#ifdef CURSOR_MOUSE_FORCE
		MeAppMousePickCB(rc, x, y, modifiers, which, event, userdata);
#endif
		return;
	}

#ifdef CURSOR_USE_MOUSE
	/*
	 * Find the direction from the eye to the mouse click.
	 */
	MeAppFindClickDir(meapp, x, y, dir);

	/*
	 * Find the normal of the plane.  If the direction is at 90 degrees,
	 * it's parallel to the plane.
	 */
	McdPlaneGetNormal(object[PLANE0].geom,
			  McdModelGetTransformPtr(object[PLANE0].cm),
			  normal);

	dot = MeVector3Dot(normal, dir);
	if (ME_IS_ZERO(dot))
		return;

	/*
	 * Now the intersection satisfies;
	 * 
	 * I = C + tD
	 * 
	 * where C is the camera position, D is the direction away from the
	 * camera, and t is some multiple of D.
	 *
	 * t = (-P.C + D) / (P.D)
	 * 
	 * Where P is the planes normal, and D is the plane's distance from
	 * the origin (here it's cursor_plane_distance).
	 */
	RCameraGetPosition(rc, camera);
	t = (-MeVector3Dot(normal, camera) + cursor_plane_distance) / dot;
	MeVector3Scale(dir, t);
	MeVector3Add(pos, camera, dir);

	switch (which) {
	case kRLeftButton:
		/*
		 * Move the cursor to the given position if possible.
		 */
		MeVector3Copy(old, cursor_position);
		MeVector3Copy(cursor_position, pos);
		cursor_check_position(old);
		cursor_update();
		break;

	case kRMiddleButton:
		launch(rc, 0);
		break;

	case kRRightButton:
		/*
		 * Direct the cursor in the given direction, with the
		 * given power.
		 */
		MeVector3Subtract(dir, pos, cursor_position);
		cursor_rotation[1] = MeAtan2(-dir[0], -dir[2]);
		cursor_power = MeSqrt(MeSqr(dir[0]) + MeSqr(dir[2]));
		cursor_power *= CURSOR_POWER_MOUSE;
		cursor_check_power();
		cursor_update();
		break;

	default:
		break;
	}
#endif CURSOR_USE_MOUSE
}

/*
 * $Id: BarSkittles.c,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 *
 * Overview:
 * 
 * BarSkittles.
 * 
 * This program implements a single player version of the popular pub game
 * Bar Skittles.  The player has at minimum ten attempts to knock over
 * nine skittles by swinging the ball clockwise around a pole mounted on the
 * left hand side of the nine skittles (ie. not aimed directly at the
 * skittles).  If the player knocks over nine skittles in one go, and extra
 * "free" shot is given.  When all nine skittles are knocked over, the table
 * is reset.  The final score is the total number of skittles knocked over.
 */
#include <stdlib.h>
#include <stdio.h>

#include "MeMath.h"
#include "MeViewer.h"
#include "MeApp.h"
#include "Mst.h"

#include "BarSkittles.h"
#include "Universe.h"
#include "Viewer.h"
#include "Game.h"
#include "Object.h"
#include "Rope.h"
#include "Cursor.h"

/*
 * Global values.
 */
MeVector4	q_standup;
MeVector4	q_identity = {
	1, 0, 0, 0
};
MeMatrix4	tm_identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};
MeVector3	v_flat = { 0, 0, 1 };
MeVector3	v_up = { 0, 1, 0 };

/*
 * Step the simulation.
 */
void MEAPI
tick(RRender *	rc,
     void *	userData)
{
	cursor_tick();	// before so we can force ball towards cursor
	universe_tick();
	viewer_tick();
	game_tick();
}

void MEAPI
launch(RRender *	rc,
       void *		userData)
{
	cursor_throw_ball();
	game_throw_ball();
}

void MEAPI
reset(RRender *	rc,
      void *	userData)
{
	object_reset();
	game_reset();
	rope_reset();
	cursor_reset();
}

/*
 * Clean up.
 */
void MEAPI_CDECL
cleanup(void)
{
	cursor_finish();
	rope_finish();
	object_finish();
	game_finish();
	universe_finish();
	viewer_finish();
}

int MEAPI_CDECL
main(int		argc,
     const char **	argv)
{
	MeCommandLineOptions 	*options;

	/*
	 * The standup quaternion is used to make the skittles stand up.
	 */
	MeQuaternionForRotation(q_standup, v_flat, v_up);

	/*
	 * Create the renderer.
	 */
	options = MeCommandLineOptionsCreate(argc, argv);
	rc = RRenderContextCreate(options, 0, !MEFALSE);
	MeCommandLineOptionsDestroy(options);
	if (!rc)
		return 1;

	universe_init();
	viewer_init();
	game_init();
	object_init();
	rope_init();
	cursor_init();
#ifndef PS2
	atexit(cleanup);
#endif /* !PS2 */

	/*
	 * Reset the game and begin.
	 */
	reset(rc, 0);
	RRun(rc, tick, 0);
	return 0;
}

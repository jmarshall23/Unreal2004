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
 * Computed extends of the bounding box around all the skittles, plus the
 * size of the base that would hold this and the tower.
 */
MeReal		skittle_leftx, skittle_rightx, skittle_frontz, skittle_backz;
MeReal		base_width, base_depth;

/*
 * Game information.
 */
int		try;			/* which try we are at */
int		num_tries;		/* how many tries we are allowed */
int		score;	  		/* score */
int		score_try_start;	/* score at start of try */
int		fall_count;		/* count of fallen skittles */
ball_state_t	ball_state; 		/* state of ball */
int		throw_fall_count;	/* fall count when ball thrown */
int		notice_frames;		/* >= 0 time left for notice */
int		notice_reset;		/* >= 0 if reset when notice done */
RGraphic	*scoreG;		/* score graphic */
RGraphic	*noticeG;		/* notice graphic */


static void
game_show_notice(char *	text,
		 MeReal	time,
		 int	reset)
{
	notice_frames = (int)((time / step) + (MeReal)0.5);
	notice_reset += reset;
	if (noticeG != NULL)
		RGraphicDelete(rc, noticeG, 1);
	if (notice_frames <= 0)
		return;
	noticeG = RGraphicTextCreate(rc, text, NOTICE_X, NOTICE_Y,
				     color_white);
}

static void
game_notice_tick()
{
	if (notice_frames <= 0 || --notice_frames > 0)
		return;
	RGraphicDelete(rc, noticeG, 1);
	if (notice_reset) {
		game_reset_skittles();
		notice_reset = 0;
	}
	noticeG = NULL;
}

static void
game_update_score()
{
	char	buf[64];

	if (scoreG != NULL)
		RGraphicDelete(rc, scoreG, 1);
	sprintf(buf, "Score %04d  Try %02d/%02d",
		score, try, num_tries);
	scoreG = RGraphicTextCreate(rc, buf, SCORE_X, SCORE_Y, color_white);
}

static void
game_check_fallen()
{
	int		i;
	object_t	*op;
	MeVector3	n;
	MeMatrix4Ptr	m;
	MeReal		dot;

	/*
	 * Now check to see if any of the skittles have fallen over.  If
	 * they have for the first time, we add to the fall count (and the
	 * score).
	 */
	for (i = 0; i < NSKITTLES; i++) {
		op = &(object[SKITTLE0 + i]);
		if (op->fallen)
			continue;

		/*
		 * n = unit vector of the cylinder's orientation
		 * up = unit up vector
		 *    
		 * if n.up < cos(45) then cylinder has fallen over.
		 * 
		 *    n = TM * flat
		 *    
		 *    where	TM = 3x3 rotation part of transform matrix
		 *		flat = { 0, 0, 1 }
		 *    
		 * => n = | a b c |   | 0 |
		 *        | d e f | * | 0 |
		 *	  | g h i |   | 1 |
		 *	  
		 * => n	= | c |
		 *        | f |
		 *	  | i |
		 */
		m = McdModelGetTransformPtr(op->cm);
		n[0] = m[2][0];
		n[1] = m[2][1];
		n[2] = m[2][2];
#ifdef SHOW_UP
		op->end[0] = op->start[0] + n[0] * SKITTLE_HEIGHT;
		op->end[1] = op->start[1] + n[1] * SKITTLE_HEIGHT;
		op->end[2] = op->start[2] + n[2] * SKITTLE_HEIGHT;
		RGraphicLineMoveEnds(op->upG, op->start, op->end);
#endif
		dot = MeVector3Dot(n, v_up);

		if (dot >= RSQRT2)
			continue;

		op->fallen = 1;
		score++;
		fall_count++;

		/*
		 * If we've knocked over all the skittles, reset them.
		 * If we knock them all over in one go (strike), we get an
		 * extra throw.
		 */
		if (fall_count == NSKITTLES) {
			if (throw_fall_count == 0) {
				num_tries++;
				game_show_notice("STRIKE!",
						 STRIKE_NOTICE_TIME, 1);
			} else {
				game_show_notice("New Frame",
						 STRIKE_NOTICE_TIME, 1);
			}
		}

		game_update_score();
	}
}

/*
 * Complete a try.
 */
static void
game_try_finished()
{
	/*
	 * Next try.  If we've run out, then we print out the score and
	 * strikes on the console and reset the game.
	 */
	if (++try > num_tries) {
		char	buf[64];

		sprintf(buf, "Final Score %d,  Strikes %d",
			score, num_tries - START_NUM_TRIES);
#ifndef PS2
		// We output to the console for a permanent log.
		puts(buf);
#endif
		game_show_notice(buf, FINAL_SCORE_NOTICE_TIME, 0);
		reset(rc, 0);
	}

	game_update_score();
}

/*
 * Compute the skittles bounding box and base size.
 */
void
game_init()
{
	int 		i;
	MeReal		x, z;

	scoreG = noticeG = NULL;
	notice_frames = 0;
	notice_reset = 0;

	/*
	 * Compute the skittle area and base size.
	 */
	skittle_leftx = skittle_rightx = skittle_frontz = skittle_backz
		= (MeReal)0.0;
	for (i = 0; i < NSKITTLES; i++) {
		x = SKITTLE_X(i);
		z = SKITTLE_Z(i);
		if (x < skittle_leftx)
			skittle_leftx = x;
		if (x > skittle_rightx)
			skittle_rightx = x;
		if (z < skittle_frontz)
			skittle_frontz = z;
		if (z > skittle_backz)
 			skittle_backz = z;
	}
	base_width = skittle_rightx - skittle_leftx + TWO * BASE_BORDER;
	base_depth = skittle_backz - skittle_frontz + TWO * BASE_BORDER;
}

/*
 * Finish the game.
 */
void
game_finish()
{
}

/*
 * Process game events.
 */
void
game_tick()
{
	object_t	*op;
	MeVector3	ballv, ball;

	game_notice_tick();

	if (!cursor_holding_ball) {
		/*
		 * The ball must travel forwards (positive Z-axis) past the
		 * left hand side of the tower, then backwards (negative
		 * Z-aix) past the right hand side of the tower.  Once the
		 * ball moves forwards (positive Z-axis) we will catch it
		 * again automatically.
		 */
		op = &(object[BALL0]);
		MdtBodyGetLinearVelocity(op->body, ballv);
		MdtBodyGetPosition(op->body, ball);

		switch (ball_state) {
		case BALL_RESET:
		case BALL_INVALID:
			break;

		case BALL_THROWN:
			/*
			 * If moving forward in back left zone, change state.
			 */
			if (ball[0] <= object[TOWER0].reset_position[0]
			    && ball[2] >= object[TOWER0].reset_position[2]
			    && ballv[2] > 0)
				ball_state = BALL_FORWARD_BL;

			/*
			 * If we throw into the back right zone, this is
			 * illegal.
			 */
			if (ball[0] >= object[TOWER0].reset_position[0]
			    && ball[2] >= object[TOWER0].reset_position[2])
				ball_state = BALL_INVALID;
			break;

		case BALL_FORWARD_BL:
			/*
			 * If moving backward in back right zone, change state.
			 */
			if (ball[0] >= object[TOWER0].reset_position[0]
			    && ball[2] >= object[TOWER0].reset_position[2]
			    && ballv[2] < 0)
				ball_state = BALL_BACKWARD_BR;
			break;

		case BALL_BACKWARD_BR:
			/*
			 * If we start moving forward again, we're done.
			 */
			if (ballv[2] > 0)
				ball_state = BALL_FINISHED;
			break;

		case BALL_FINISHED:
			break;
		}
	}

	if (ball_state == BALL_INVALID) {
		/*
		 * If the ball was invalid, reset the ball, skittles and
		 * the cursor.
		 */
		game_show_notice("INVALID THROW", INVALID_NOTICE_TIME, 0);
		game_reset_ball();
		game_reset_skittles();
		cursor_reset();
		ball_state = BALL_RESET;
		score = score_try_start;
		game_try_finished();
		return;
	}

	game_check_fallen();

	if (ball_state == BALL_FINISHED) {
		/*
		 * If the ball has gone into the finished state, we reset the
		 * ball back to the start position.  If all the skittles are
		 * knocked over we reset the skittles also.
		 *
		 * Make the cursor grab the ball again.
		 */
		ball_state = BALL_RESET;
		cursor_grab_ball();
		game_try_finished();
	}
}

/*
 * Throw the ball.
 */
void
game_throw_ball()
{
	ball_state = BALL_THROWN;
	throw_fall_count = fall_count;
	score_try_start = score;
}

/*
 * Reset the game.
 */
void
game_reset()
{
	try = 1;
	num_tries = START_NUM_TRIES;
	score = 0;
	score_try_start = 0;
	fall_count = 0;
	ball_state = BALL_RESET;

	game_update_score();
}

/*
 * Reset just the ball.
 */
void
game_reset_ball()
{
	object_reset_item(&(object[BALL0]));
}

/*
 * Reset just the skittles.
 */
void
game_reset_skittles()
{
	int		i;
	object_t	*op;

	fall_count = 0;
	for (i = 0; i < NSKITTLES; i++) {
		op = &(object[SKITTLE0 + i]);
		object_reset_item(op);
	}
}

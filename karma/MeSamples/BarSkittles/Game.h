#ifndef _GAME_H_
#define _GAME_H_

/*
 * $Id: Game.h,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */

/*
 * Constants for the game.
 */
#define SCORE_X			((AcmeReal)200.0)
#define SCORE_Y			((AcmeReal)360.0)
#define NOTICE_X		((AcmeReal)200.0)
#define NOTICE_Y		((AcmeReal)250.0)
#define STRIKE_NOTICE_TIME	((AcmeReal)2.0)
#define INVALID_NOTICE_TIME	((AcmeReal)2.0)
#define FINAL_SCORE_NOTICE_TIME	((AcmeReal)5.0)

/*
 * State of ball in play.
 */
typedef enum ball_state {
	BALL_RESET,		/* ball is being reset */
	BALL_THROWN,     	/* ball in unknown zone */
	BALL_FORWARD_BL,	/* ball moving forward in back left zone */
	BALL_BACKWARD_BR,	/* ball moving backward in back right zone */
	BALL_FINISHED,		/* ball has finished moving */
	BALL_INVALID,		/* ball has been thrown incorrectly */
} ball_state_t;

/*
 * Globals.
 */
extern MeReal	skittle_leftx, skittle_rightx, skittle_frontz, skittle_backz;
extern MeReal	base_width, base_depth;

/*
 * Prototypes.
 */
void game_init(void);
void game_finish(void);
void game_tick(void);
void game_throw_ball(void);
void game_reset(void);
void game_reset_ball(void);
void game_reset_skittles(void);

#endif /* _GAME_H_ */

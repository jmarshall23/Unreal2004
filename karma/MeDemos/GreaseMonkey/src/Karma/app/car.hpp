/***********************************************************************************************
*
*	$Id: car.hpp,v 1.1.2.2 2002/03/04 17:40:41 richardm Exp $
*
************************************************************************************************/
#ifndef _KART_H
#define _KART_H


#include "rtworld.h"

//#define PI (3.14159265359f)
#define TWO_PI (2.0f*PI)

#define CAM_DIST (10.0f)
#define CAM_HEIGHT (3.5f)

#define CAM_MIN_HEIGHT (1.0f)

#define CAM_ROT_INC (180.0f)
#define CAM_DIST_INC (100.0f)

//#define MIN_FRAME_TIME (1.0f/1000.0f)
#define MIN_FRAME_TIME (1.0f/150.0f)
#define MAX_FRAME_TIME (1.0f/20.0f)

#define NUM_TRACK_TEXTURES (9)
#define NUM_CAR_TEXTURES (6)
#define NUM_F1_CAMS (4)
#define NUM_F2_CAMS (5)

#define MAX_LIGHTS (10)

#define MAX_PATHLEN (255)
#define MAX_CARS (25)

typedef struct _game_var {
	char world_bsp[MAX_PATHLEN];
	char lights_file[MAX_PATHLEN];
	char tracks_file[MAX_PATHLEN];
//	char dashboard_shape[MAX_PATHLEN];
	char texture_path[MAX_PATHLEN];
	char shape_dir[MAX_PATHLEN];
	char driver_files[MAX_CARS][MAX_PATHLEN];
	char driver_tracks[MAX_CARS][MAX_PATHLEN];
	int start_point[MAX_CARS];
	int num_cars;
	int	player_car;
	int vm_width,vm_height,vm_depth; //PC Video mode settings
	int rear_view_mirror; //flag to say whether to render a rvm or not
} GAME_VARIABLES;

typedef enum {
	CM_FIXED_TO_CAR = 0,
	CM_FIXED_IN_CAR,
	CM_FIXED_VIEW_POINT,
	CM_MOVING_VIEW_POINT,
	CM_FOLLOW,
	CM_DRIVE_BY,
	CM_FIXED_DEFAULT,
	CM_MOVEABLE,
	NUM_CAM_MODES
} CAMERA_MODE;

typedef struct game_camera_s {
	RwInt32 index;
	RwReal pos_lag,ang_lag, dir, pitch, dist;
	RwCamera *cam;
	RwMatrix offset;
	RpClump *view_clump;
	CAMERA_MODE mode;
	RwInt32 view_driver;
} GAME_CAMERA;

typedef struct _mirror_s {
	RwReal x_frac,y_frac,w_frac,h_frac;
	RwCamera *cam;
	RwMatrix offset;
	RwInt32 on; //turn on or off
} MIRROR;

extern MeReal frame_delta_t;
//extern RwReal camera_angle;
extern int paused;
extern RwInt32 num_lights;
extern RpLight *lights[];
extern METER_ARC speedo, tacho;
extern _3D_PLINE debug_lines;

extern void ResetCamera(CAMERA_MODE, RpClump *);
extern void UpdateShapes(void);
extern void DoFrame(void);
extern void UpdateAllShapes(void);
extern RwReal SmallestAngle(RwReal curr, RwReal req);

extern MIRROR rvm; //rear view mirror

extern void TerminateCarDemo(void);

extern RwUInt8 track_mats[];
extern RwTexture *chassis_textures[];

#ifdef __cplusplus
extern "C" {
#endif
extern GAME_VARIABLES game_vars;
extern GAME_CAMERA game_cam;
#ifdef __cplusplus
}
#endif

#endif //_KART_H
